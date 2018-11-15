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

#include "ConvertDataContainerGeometry.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertDataContainerGeometry::ConvertDataContainerGeometry() :
  AbstractFilter()
, m_NewDataContainerGeometry(0)
, m_ArrayHandling(static_cast<int>(ArrayHandlingType::MoveArrays))
, m_NewDataContainerName("DataContainer")
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertDataContainerGeometry::~ConvertDataContainerGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertDataContainerGeometry::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertDataContainerGeometry::setupFilterParameters()
{
  m_NewDCGeometryChoices.clear();
  m_ArrayHandlingChoices.clear();

  FilterParameterVector parameters;

  {
    m_NewDCGeometryChoices.push_back("Vertex Geometry");
    // Create the Choice Filter Parameter and add it to the list of parameters
    parameters.push_back(SIMPL_NEW_CHOICE_FP("New Data Container Geometry", NewDataContainerGeometry, FilterParameter::Parameter, ConvertDataContainerGeometry, m_NewDCGeometryChoices, false));
  }

  {
    m_ArrayHandlingChoices.push_back("Move Attribute Arrays");
    m_ArrayHandlingChoices.push_back("Copy Attribute Arrays");
    // Create the Choice Filter Parameter and add it to the list of parameters
    parameters.push_back(SIMPL_NEW_CHOICE_FP("Array Handling", ArrayHandling, FilterParameter::Parameter, ConvertDataContainerGeometry, m_ArrayHandlingChoices, false));
  }

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    req.dcGeometryTypes = IGeometry::Types(2);
    req.dcGeometryTypes[0] = IGeometry::Type::Image;
    req.dcGeometryTypes[0] = IGeometry::Type::RectGrid;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container with Input Geometry", SelectedDataContainerName, FilterParameter::RequiredArray, ConvertDataContainerGeometry, req));
  }

  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    req.amTypes = AttributeMatrix::Types(1, AttributeMatrix::Type::Cell);
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Included Attribute Arrays", IncludedDataArrayPaths, FilterParameter::RequiredArray, ConvertDataContainerGeometry, req));
  }

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("New Data Container Name", NewDataContainerName, FilterParameter::CreatedArray, ConvertDataContainerGeometry));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertDataContainerGeometry::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  
  if (m_NewDataContainerGeometry < 0)
  {
    QString ss = QObject::tr("'New Data Container Geometry' has not been selected.");
    setErrorCondition(-2000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_NewDataContainerGeometry > m_NewDCGeometryChoices.size() - 1)
  {
    QString ss = QObject::tr("'New Data Container Geometry' index is out of bounds.");
    setErrorCondition(-2001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_ArrayHandling < 0)
  {
    QString ss = QObject::tr("'Array Handling' has not been selected.");
    setErrorCondition(-2002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_ArrayHandling > m_ArrayHandlingChoices.size() - 1)
  {
    QString ss = QObject::tr("'Array Handling' index is out of bounds.");
    setErrorCondition(-2003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_SelectedDataContainerName.isEmpty())
  {
    QString ss = QObject::tr("'Data Container with Input Geometry' is empty.  Please input a value.");
    setErrorCondition(-2004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_SelectedDataContainerName);
  if (getErrorCondition() < 0)
  {
    return;
  }

  if (m_NewDataContainerName.isEmpty())
  {
    QString ss = QObject::tr("'New Data Container Name' is empty.  Please input a value.");
    setErrorCondition(-2006);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataContainer::Pointer newDc = getDataContainerArray()->getDataContainer(m_NewDataContainerName);
  if (newDc != nullptr)
  {
    QString ss = QObject::tr("Data Container with name '%1' already exists.").arg(m_NewDataContainerName);
    setErrorCondition(-2007);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  newDc = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, m_NewDataContainerName);
  VertexGeom::Pointer newVertexGeom;
  ImageGeom::Pointer selectedImageGeom = dc->getGeometryAs<ImageGeom>();
  RectGridGeom::Pointer selectedRectGridGeom = dc->getGeometryAs<RectGridGeom>();
  int64_t elementCount;
  if (selectedImageGeom != nullptr)
  {
    size_t xPoints = selectedImageGeom->getXPoints();
    size_t yPoints = selectedImageGeom->getYPoints();
    size_t zPoints = selectedImageGeom->getZPoints();

    elementCount = xPoints * yPoints * zPoints;

    newVertexGeom = VertexGeom::CreateGeometry(elementCount, "Vertex Geometry", false);
  }
  else if (selectedRectGridGeom != nullptr)
  {
    // Rectilinear Grid
  }
  else
  {
    QString ss = QObject::tr("Data Container '%1' does not have an acceptable geometry.  Acceptable geometries are"
                             " Image and Rectilinear Grid.").arg(m_SelectedDataContainerName);
    setErrorCondition(-2008);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  newDc->setGeometry(newVertexGeom);

  int selectedArraysSize = m_IncludedDataArrayPaths.size();
  for (int i = 0; i < selectedArraysSize; i++)
  {
    DataArrayPath dap = m_IncludedDataArrayPaths[i];
    dc = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, dap.getDataContainerName());

    int err = 0;
    AttributeMatrix::Pointer am = dc->getPrereqAttributeMatrix(this, dap.getAttributeMatrixName(), err);
    if (getErrorCondition() < 0)
    {
      return;
    }

    if (am->getNumberOfTuples() != elementCount)
    {
      QString ss = QObject::tr("The data array with path '%1' has a tuple count of %2, but this does not match the "
                               "number of tuples required by %4's geometry (%3)").arg(dap.serialize("/")).arg(am->getNumberOfTuples()).arg(elementCount).arg(m_NewDataContainerName);
      setErrorCondition(-2009);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    IDataArray::Pointer iDataArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, dap);
    if (getErrorCondition() < 0)
    {
      return;
    }

    IDataArray::Pointer newArrayPtr;
    if (static_cast<ArrayHandlingType>(m_ArrayHandling) == ArrayHandlingType::CopyArrays)
    {
      newArrayPtr = iDataArrayPtr->deepCopy();
    }
    else
    {
      newArrayPtr = am->removeAttributeArray(dap.getDataArrayName());
    }

    DataArrayPath newDap = dap;
    newDap.setDataContainerName(m_NewDataContainerName);

    AttributeMatrix::Pointer newAm = newDc->getAttributeMatrix(newDap);
    if (newAm == nullptr)
    {
      newAm = newDc->createNonPrereqAttributeMatrix(this, am->getName(), am->getTupleDimensions(), am->getType());
    }

    newAm->addAttributeArray(newArrayPtr->getName(), newArrayPtr);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertDataContainerGeometry::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertDataContainerGeometry::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_SelectedDataContainerName);

  VertexGeom::Pointer newVertexGeom;
  ImageGeom::Pointer selectedImageGeom = dc->getGeometryAs<ImageGeom>();
  RectGridGeom::Pointer selectedRectGridGeom = dc->getGeometryAs<RectGridGeom>();
  if (selectedImageGeom != nullptr)
  {
    newVertexGeom = Image2Vertex(selectedImageGeom);
  }
  else if (selectedRectGridGeom != nullptr)
  {
    newVertexGeom = RectGrid2Vertex(selectedRectGridGeom);
  }

  DataContainer::Pointer newDc = getDataContainerArray()->getDataContainer(m_NewDataContainerName);
  newDc->setGeometry(newVertexGeom);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::Pointer ConvertDataContainerGeometry::Image2Vertex(ImageGeom::Pointer imageGeom)
{
  VertexGeom::Pointer vertexGeom = VertexGeom::New();
  int64_t count = 0;

  float coords[3] = {0.0f, 0.0f, 0.0f};
  size_t xPoints = imageGeom->getXPoints();
  size_t yPoints = imageGeom->getYPoints();
  size_t zPoints = imageGeom->getZPoints();
  float resX;
  float resY;
  float resZ;
  float originX;
  float originY;
  float originZ;
  imageGeom->getResolution(resX, resY, resZ);
  imageGeom->getOrigin(originX, originY, originZ);
  for(int64_t k = 0; k < zPoints; k++)
  {
    for(int64_t j = 0; j < yPoints; j++)
    {
      for(int64_t i = 0; i < xPoints; i++)
      {
        coords[0] = originX + (i * resX) + (resX / 2);
        coords[1] = originY + (j * resY) + (resY / 2);
        coords[2] = originZ + (k * resZ) + (resZ / 2);
        vertexGeom->setCoords(count, coords);
        count++;
      }
    }
  }

  return vertexGeom;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::Pointer ConvertDataContainerGeometry::RectGrid2Vertex(RectGridGeom::Pointer rectGridGeom)
{
  VertexGeom::Pointer vertexGeom = VertexGeom::New();

  return vertexGeom;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertDataContainerGeometry::newFilterInstance(bool copyFilterParameters) const
{
  ConvertDataContainerGeometry::Pointer filter = ConvertDataContainerGeometry::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertDataContainerGeometry::getCompiledLibraryName() const
{ 
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertDataContainerGeometry::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertDataContainerGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertDataContainerGeometry::getGroupName() const
{ 
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertDataContainerGeometry::getSubGroupName() const
{ 
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertDataContainerGeometry::getHumanLabel() const
{ 
  return "Convert Data Container Geometry";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ConvertDataContainerGeometry::getUuid()
{
  return QUuid("{bc8a91ca-0cee-59c6-b5cb-acc1aab8617f}");
}

