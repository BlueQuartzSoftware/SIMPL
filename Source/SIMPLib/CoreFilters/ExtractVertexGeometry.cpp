/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ExtractVertexGeometry.h"

#include <numeric>

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1,
  AttributeMatrixID
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractVertexGeometry::ExtractVertexGeometry()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractVertexGeometry::~ExtractVertexGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractVertexGeometry::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractVertexGeometry::setupFilterParameters()
{
  m_NewDCGeometryChoices.clear();
  m_ArrayHandlingChoices.clear();

  FilterParameterVectorType parameters;

  {
    m_ArrayHandlingChoices.push_back("Move Attribute Arrays");
    m_ArrayHandlingChoices.push_back("Copy Attribute Arrays");
    // Create the Choice Filter Parameter and add it to the list of parameters
    parameters.push_back(SIMPL_NEW_CHOICE_FP("Array Handling", ArrayHandling, FilterParameter::Category::Parameter, ExtractVertexGeometry, m_ArrayHandlingChoices, false));
  }
  std::vector<QString> linkedProps = {"MaskArrayPath"};
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Mask", UseMask, FilterParameter::Category::Parameter, ExtractVertexGeometry, linkedProps));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", MaskArrayPath, FilterParameter::Category::RequiredArray, ExtractVertexGeometry, req));
  }

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    req.dcGeometryTypes = {IGeometry::Type::Image, IGeometry::Type::RectGrid};
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container with Input Geometry", SelectedDataContainerName, FilterParameter::Category::RequiredArray, ExtractVertexGeometry, req));
  }

  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    req.amTypes = AttributeMatrix::Types(1, AttributeMatrix::Type::Cell);
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Included Attribute Arrays", IncludedDataArrayPaths, FilterParameter::Category::RequiredArray, ExtractVertexGeometry, req));
  }

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Vertex Data Container Name", VertexDataContainerName, FilterParameter::Category::CreatedArray, ExtractVertexGeometry));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractVertexGeometry::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(m_ArrayHandling < 0)
  {
    QString ss = QObject::tr("'Array Handling' has not been selected.");
    setErrorCondition(-2002, ss);
    return;
  }

  if(m_ArrayHandling > m_ArrayHandlingChoices.size() - 1)
  {
    QString ss = QObject::tr("'Array Handling' index is out of bounds. Valid values are 0 or 1.");
    setErrorCondition(-2003, ss);
    return;
  }

  if(m_SelectedDataContainerName.isEmpty())
  {
    QString ss = QObject::tr("The name of the input DataContainer is empty. Please specify a value.");
    setErrorCondition(-2004, ss);
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer(this, getSelectedDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getVertexDataContainerName().isEmpty())
  {
    QString ss = QObject::tr("The name of the created Vertex DataContainer is empty. Please specify a value.");
    setErrorCondition(-2006, ss);
    return;
  }

  if(getDataContainerArray()->doesDataContainerExist(getVertexDataContainerName()))
  {
    QString ss = QObject::tr("A Data Container with name '%1' already exists.").arg(getVertexDataContainerName().getDataContainerName());
    setErrorCondition(-2007, ss);
    return;
  }

  DataContainer::Pointer vertexDataContainer = DataContainer::NullPointer();
  IGeometry::Pointer fromGeometry = dc->getGeometry();
  if(nullptr == fromGeometry.get())
  {
    QString ss = QObject::tr("Selected input DataContainer must contain either an Image Geometry or RectLinearGrid Geometry. The DataContainer did not contain a Geometry object.");
    setErrorCondition(-2008, ss);
    return;
  }
  IGeometry::Type geomType = fromGeometry->getGeometryType();
  size_t elementCount = 0;
  if(IGeometry::Type::Image == geomType || IGeometry::Type::RectGrid == geomType)
  {
    vertexDataContainer = getDataContainerArray()->createNonPrereqDataContainer(this, getVertexDataContainerName(), DataContainerID);
    IGeometryGrid::Pointer imageGeom = std::dynamic_pointer_cast<IGeometryGrid>(fromGeometry);
    SizeVec3Type imageDims = imageGeom->getDimensions();
    VertexGeom::Pointer vertexGeom = VertexGeom::CreateGeometry(imageDims[0] * imageDims[1] * imageDims[2], "VertexGeometry", !getInPreflight());
    vertexDataContainer->setGeometry(vertexGeom);
    elementCount = imageDims[0] * imageDims[1] * imageDims[2];
  }
  else
  {
    QString ss =
        QObject::tr("Data Container's Geometry type must be either an Image Geometry or RectLinearGrid Geometry. The selected geometry is of type %1").arg(fromGeometry->getGeometryTypeAsString());
    setErrorCondition(-2010, ss);
    return;
  }
  if(m_UseMask)
  {
    std::vector<size_t> cDims = {1};
    m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<BoolArrayType>(this, getMaskArrayPath(), cDims);
    if(nullptr != m_MaskPtr.lock())
    {
      m_Mask = m_MaskPtr.lock()->getPointer(0);
      if(m_MaskPtr.lock()->getNumberOfTuples() != elementCount)
      {
        QString ss = QObject::tr("The data array with path '%1' has a tuple count of %2, but this does not match the "
                                 "number of tuples required by %4's geometry (%3)")
                         .arg(getMaskArrayPath().serialize("/"))
                         .arg(m_MaskPtr.lock()->getNumberOfTuples())
                         .arg(elementCount)
                         .arg(dc->getName());
        setErrorCondition(-2019, ss);
        return;
      }
    }
  }

  int selectedArraysSize = m_IncludedDataArrayPaths.size();
  for(int i = 0; i < selectedArraysSize; i++)
  {
    DataArrayPath dap = m_IncludedDataArrayPaths[i];
    dc = getDataContainerArray()->getPrereqDataContainer(this, dap.getDataContainerName());

    int err = 0;
    AttributeMatrix::Pointer sourceCellAttrMat = dc->getPrereqAttributeMatrix(this, dap.getAttributeMatrixName(), err);
    if(getErrorCode() < 0)
    {
      return;
    }

    if(sourceCellAttrMat->getNumberOfTuples() != elementCount)
    {
      QString ss = QObject::tr("The data array with path '%1' has a tuple count of %2, but this does not match the "
                               "number of tuples required by %4's geometry (%3)")
                       .arg(dap.serialize("/"))
                       .arg(sourceCellAttrMat->getNumberOfTuples())
                       .arg(elementCount)
                       .arg(m_VertexDataContainerName.getDataContainerName());
      setErrorCondition(-2009, ss);
      return;
    }

    IDataArray::Pointer iDataArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, dap);
    if(getErrorCode() < 0)
    {
      return;
    }

    IDataArray::Pointer newArrayPtr;
    if(static_cast<ArrayHandlingType>(m_ArrayHandling) == ArrayHandlingType::CopyArrays)
    {
      newArrayPtr = iDataArrayPtr->deepCopy();
    }
    else
    {
      if(!m_UseMask)
      {
        newArrayPtr = sourceCellAttrMat->removeAttributeArray(dap.getDataArrayName());
      }
    }

    DataArrayPath newDap = m_VertexDataContainerName;
    newDap.setAttributeMatrixName(sourceCellAttrMat->getName());
    newDap.setDataArrayName("");

    AttributeMatrix::Pointer vertexCellAttrMat = vertexDataContainer->getAttributeMatrix(newDap);
    if(vertexCellAttrMat == nullptr)
    {
      vertexCellAttrMat =
          vertexDataContainer->createNonPrereqAttributeMatrix(this, sourceCellAttrMat->getName(), sourceCellAttrMat->getTupleDimensions(), AttributeMatrix::Type::Vertex, AttributeMatrixID);
    }

    vertexCellAttrMat->insertOrAssign(newArrayPtr);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractVertexGeometry::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer sourceGeomDC = getDataContainerArray()->getDataContainer(getSelectedDataContainerName());
  IGeometryGrid::Pointer sourceGeometry = sourceGeomDC->getGeometryAs<IGeometryGrid>();

  DataContainer::Pointer vertexDC = getDataContainerArray()->getDataContainer(getVertexDataContainerName());
  VertexGeom::Pointer vertexGeom = vertexDC->getGeometryAs<VertexGeom>();

  SizeVec3Type dims = sourceGeometry->getDimensions();
  size_t cellCount = std::accumulate(dims.begin(), dims.end(), static_cast<size_t>(1), std::multiplies<size_t>());
  size_t totalCells = cellCount; // We save this here because it may change based on the use_mask flag.

  BoolArrayType::Pointer maskArrayPtr = m_MaskPtr.lock();
  if(m_UseMask)
  {
    BoolArrayType& maskArray = *(maskArrayPtr);
    size_t size = maskArray.size();
    cellCount = 0;
    for(size_t i = 0; i < size; i++)
    {
      if(maskArray[i])
      {
        cellCount++;
      }
    }
    // Resize the vertex geometry to the proper size
    vertexGeom->resizeVertexList(cellCount);
  }

  SharedVertexList::Pointer vertices = vertexGeom->getVertices();
  float coords[3] = {0.0f, 0.0f, 0.0f};

  // Use the APIs from the IGeometryGrid to get the XYZ coord for the center of each cell and then set that into
  // the new VertexGeometry
  size_t vertIdx = 0;
  for(size_t idx = 0; idx < totalCells; idx++)
  {
    if(m_UseMask)
    {
      if(maskArrayPtr->getValue(idx))
      {
        sourceGeometry->getCoords(idx, coords);
        vertices->setTuple(vertIdx, coords);
        vertIdx++;
      }
    }
    else
    {
      sourceGeometry->getCoords(idx, coords);
      vertices->setTuple(idx, coords);
    }
  }

  // If we are using a mask we need to copy the data from the cell data arrays to the vertex cell data arrays
  if(m_UseMask && !m_IncludedDataArrayPaths.empty())
  {
    BoolArrayType& maskArray = *(maskArrayPtr);

    AttributeMatrix& imageGeomCellAM = *(sourceGeomDC->getAttributeMatrix(m_IncludedDataArrayPaths[0].getAttributeMatrixName()));

    DataArrayPath vertCelAMPath = m_VertexDataContainerName;
    vertCelAMPath.setAttributeMatrixName(imageGeomCellAM.getName());
    vertCelAMPath.setDataArrayName("");

    AttributeMatrix& vertexCellAttrMat = *(vertexDC->getAttributeMatrix(vertCelAMPath));
    // Clear all the data arrays from the Vertex Cell Attribute Matrix. This is done because the arrays are probably resized,
    // so we can't do a copy or move. We have to copy the values one at a time.
    vertexCellAttrMat.clearAttributeArrays();
    // Correctly set the dimensions on the AttributeMatrix for the vertex array
    vertexCellAttrMat.resizeAttributeArrays({vertIdx});

    for(const auto& dataArrayPath : m_IncludedDataArrayPaths)
    {
      QString msg;
      QTextStream ss(&msg);
      ss << "Copying '" << dataArrayPath.serialize("/") << "' data from cell data arrays to vertex cell data array.";
      notifyStatusMessage(msg);

      IDataArray::Pointer imageGeomDataArrayPtrPtr = imageGeomCellAM[dataArrayPath.getDataArrayName()];
      std::vector<size_t> cDims = imageGeomDataArrayPtrPtr->getComponentDimensions();
      QString name = imageGeomDataArrayPtrPtr->getName();

      // Create a new DataArray to copy the cell values into
      IDataArray::Pointer destDataArray = imageGeomDataArrayPtrPtr->createNewArray(cellCount, cDims, name, true);

      vertIdx = 0;
      for(size_t idx = 0; idx < totalCells; idx++)
      {
        if(maskArray[idx])
        {
          destDataArray->copyFromArray(vertIdx, imageGeomDataArrayPtrPtr, idx, 1);
          vertIdx++;
        }
      }

      // Insert the new data array into the vertex cell attribute matrix
      vertexCellAttrMat.insertOrAssign(destDataArray);
      // if the user selected to "move" the arrays then remove the source array from its attribute matrix
      if(static_cast<ArrayHandlingType>(m_ArrayHandling) == ArrayHandlingType::MoveArrays)
      {
        imageGeomCellAM.removeAttributeArray(name);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExtractVertexGeometry::newFilterInstance(bool copyFilterParameters) const
{
  ExtractVertexGeometry::Pointer filter = ExtractVertexGeometry::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractVertexGeometry::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractVertexGeometry::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractVertexGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractVertexGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractVertexGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractVertexGeometry::getHumanLabel() const
{
  return "Extract Vertex Geometry";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ExtractVertexGeometry::getUuid() const
{
  return QUuid("{bc8a91ca-0cee-59c6-b5cb-acc1aab8617f}");
}

// -----------------------------------------------------------------------------
ExtractVertexGeometry::Pointer ExtractVertexGeometry::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ExtractVertexGeometry> ExtractVertexGeometry::New()
{
  struct make_shared_enabler : public ExtractVertexGeometry
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ExtractVertexGeometry::getNameOfClass() const
{
  return QString("ExtractVertexGeometry");
}

// -----------------------------------------------------------------------------
QString ExtractVertexGeometry::ClassName()
{
  return QString("ExtractVertexGeometry");
}

// -----------------------------------------------------------------------------
void ExtractVertexGeometry::setArrayHandling(int value)
{
  m_ArrayHandling = value;
}

// -----------------------------------------------------------------------------
int ExtractVertexGeometry::getArrayHandling() const
{
  return m_ArrayHandling;
}

// -----------------------------------------------------------------------------
void ExtractVertexGeometry::setSelectedDataContainerName(const DataArrayPath& value)
{
  m_SelectedDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExtractVertexGeometry::getSelectedDataContainerName() const
{
  return m_SelectedDataContainerName;
}

// -----------------------------------------------------------------------------
void ExtractVertexGeometry::setIncludedDataArrayPaths(const std::vector<DataArrayPath>& value)
{
  m_IncludedDataArrayPaths = value;
}

// -----------------------------------------------------------------------------
std::vector<DataArrayPath> ExtractVertexGeometry::getIncludedDataArrayPaths() const
{
  return m_IncludedDataArrayPaths;
}

// -----------------------------------------------------------------------------
void ExtractVertexGeometry::setVertexDataContainerName(const DataArrayPath& value)
{
  m_VertexDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExtractVertexGeometry::getVertexDataContainerName() const
{
  return m_VertexDataContainerName;
}

// -----------------------------------------------------------------------------
void ExtractVertexGeometry::setMaskArrayPath(const DataArrayPath& value)
{
  m_MaskArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExtractVertexGeometry::getMaskArrayPath() const
{
  return m_MaskArrayPath;
}

// -----------------------------------------------------------------------------
void ExtractVertexGeometry::setUseMask(bool value)
{
  m_UseMask = value;
}

// -----------------------------------------------------------------------------
bool ExtractVertexGeometry::getUseMask() const
{
  return m_UseMask;
}
