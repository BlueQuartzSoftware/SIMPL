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
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ExtractVertexGeometry.h"

#include <numeric>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractVertexGeometry::ExtractVertexGeometry()
: m_ArrayHandling(static_cast<int>(ArrayHandlingType::MoveArrays))
, m_SelectedDataContainerName()
, m_VertexDataContainerName("VertexDataContainer")
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
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractVertexGeometry::setupFilterParameters()
{
  m_NewDCGeometryChoices.clear();
  m_ArrayHandlingChoices.clear();

  FilterParameterVector parameters;

  //  {
  //    m_NewDCGeometryChoices.push_back("Vertex Geometry");
  //    // Create the Choice Filter Parameter and add it to the list of parameters
  //    parameters.push_back(SIMPL_NEW_CHOICE_FP("New Data Container Geometry", NewDataContainerGeometry, FilterParameter::Parameter, ExtractVertexGeometry, m_NewDCGeometryChoices, false));
  //  }

  {
    m_ArrayHandlingChoices.push_back("Move Attribute Arrays");
    m_ArrayHandlingChoices.push_back("Copy Attribute Arrays");
    // Create the Choice Filter Parameter and add it to the list of parameters
    parameters.push_back(SIMPL_NEW_CHOICE_FP("Array Handling", ArrayHandling, FilterParameter::Parameter, ExtractVertexGeometry, m_ArrayHandlingChoices, false));
  }

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    req.dcGeometryTypes = IGeometry::Types(2);
    req.dcGeometryTypes[0] = IGeometry::Type::Image;
    req.dcGeometryTypes[0] = IGeometry::Type::RectGrid;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container with Input Geometry", SelectedDataContainerName, FilterParameter::RequiredArray, ExtractVertexGeometry, req));
  }

  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    req.amTypes = AttributeMatrix::Types(1, AttributeMatrix::Type::Cell);
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Included Attribute Arrays", IncludedDataArrayPaths, FilterParameter::RequiredArray, ExtractVertexGeometry, req));
  }

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Vertex Data Container Name", VertexDataContainerName, FilterParameter::CreatedArray, ExtractVertexGeometry));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractVertexGeometry::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  if(m_ArrayHandling < 0)
  {
    QString ss = QObject::tr("'Array Handling' has not been selected.");
    setErrorCondition(-2002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_ArrayHandling > m_ArrayHandlingChoices.size() - 1)
  {
    QString ss = QObject::tr("'Array Handling' index is out of bounds. Valid values are 0 or 1.");
    setErrorCondition(-2003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_SelectedDataContainerName.isEmpty())
  {
    QString ss = QObject::tr("The name of the input DataContainer is empty. Please specify a value.");
    setErrorCondition(-2004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_SelectedDataContainerName);
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(getVertexDataContainerName().isEmpty())
  {
    QString ss = QObject::tr("The name of the created Vertex DataContainer is empty. Please specify a value.");
    setErrorCondition(-2006);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(getDataContainerArray()->doesDataContainerExist(getVertexDataContainerName()))
  {
    QString ss = QObject::tr("A Data Container with name '%1' already exists.").arg(getVertexDataContainerName());
    setErrorCondition(-2007);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataContainer::Pointer vertexDataContainer = DataContainer::NullPointer();
  IGeometry::Pointer fromGeometry = dc->getGeometry();
  if(nullptr == fromGeometry.get())
  {
    QString ss = QObject::tr("Selected input DataContainer must contain either an Image Geometry or RectLinearGrid Geometry. The DataContainer did not contain a Geometry object.");
    setErrorCondition(-2008);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  IGeometry::Type geomType = fromGeometry->getGeometryType();
  size_t elementCount = 0;
  if(IGeometry::Type::Image == geomType || IGeometry::Type::RectGrid == geomType)
  {
    vertexDataContainer = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getVertexDataContainerName());
    IGeometryGrid::Pointer imageGeom = std::dynamic_pointer_cast<IGeometryGrid>(fromGeometry);
    SIMPL::Tuple3SVec imageDims = imageGeom->getDimensions();
    VertexGeom::Pointer vertexGeom = VertexGeom::CreateGeometry(static_cast<int64_t>(std::get<0>(imageDims) * std::get<1>(imageDims) * std::get<2>(imageDims)), "VertexGeometry", !getInPreflight());
    vertexDataContainer->setGeometry(vertexGeom);
    elementCount = std::get<0>(imageDims) * std::get<1>(imageDims) * std::get<2>(imageDims);
  }
  else
  {
    QString ss = QObject::tr("Data Container's Geometry type must be either an Image Geometry or RectLinearGrid Geometry. The Geomerty is of type %1").arg(fromGeometry->getGeometryTypeAsString());
    setErrorCondition(-2010);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  int selectedArraysSize = m_IncludedDataArrayPaths.size();
  for(int i = 0; i < selectedArraysSize; i++)
  {
    DataArrayPath dap = m_IncludedDataArrayPaths[i];
    dc = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, dap.getDataContainerName());

    int err = 0;
    AttributeMatrix::Pointer sourceCellAttrMat = dc->getPrereqAttributeMatrix(this, dap.getAttributeMatrixName(), err);
    if(getErrorCondition() < 0)
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
                       .arg(m_VertexDataContainerName);
      setErrorCondition(-2009);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    IDataArray::Pointer iDataArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, dap);
    if(getErrorCondition() < 0)
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
      newArrayPtr = sourceCellAttrMat->removeAttributeArray(dap.getDataArrayName());
    }

    DataArrayPath newDap = dap;
    newDap.setDataContainerName(m_VertexDataContainerName);

    AttributeMatrix::Pointer vertexCellAttrMat = vertexDataContainer->getAttributeMatrix(newDap);
    if(vertexCellAttrMat == nullptr)
    {
      vertexCellAttrMat = vertexDataContainer->createNonPrereqAttributeMatrix(this, sourceCellAttrMat->getName(), sourceCellAttrMat->getTupleDimensions(), AttributeMatrix::Type::Vertex);
    }

    vertexCellAttrMat->addAttributeArray(newArrayPtr->getName(), newArrayPtr);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractVertexGeometry::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractVertexGeometry::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  IGeometryGrid::Pointer sourceGeometry = getDataContainerArray()->getDataContainer(getSelectedDataContainerName())->getGeometryAs<IGeometryGrid>();

  float coords[3] = {0.0f, 0.0f, 0.0f};
  size_t xPoints = 0;
  size_t yPoints = 0;
  size_t zPoints = 0;

  std::tie(xPoints, yPoints, zPoints) = sourceGeometry->getDimensions();
  size_t cellCount = xPoints * yPoints * zPoints;

  VertexGeom::Pointer vertexGeom = getDataContainerArray()->getDataContainer(getVertexDataContainerName())->getGeometryAs<VertexGeom>();
  SharedVertexList::Pointer vertices = vertexGeom->getVertices();

  // Use the APIs from the IGeometryGrid to get the XYZ coord for the center of each cell and then set that into the
  // the new VertexGeometry
  for(size_t idx = 0; idx < cellCount; idx++)
  {
    sourceGeometry->getCoords(idx, coords);
    vertices->setTuple(idx, coords);
  }

  // The moving or copying of the Cell DataArrays was already handled in the dataCheck() method.

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
const QString ExtractVertexGeometry::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractVertexGeometry::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractVertexGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractVertexGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractVertexGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractVertexGeometry::getHumanLabel() const
{
  return "Extract Vertex Geometry";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ExtractVertexGeometry::getUuid()
{
  return QUuid("{bc8a91ca-0cee-59c6-b5cb-acc1aab8617f}");
}
