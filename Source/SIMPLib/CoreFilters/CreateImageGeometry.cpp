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
#include "CreateImageGeometry.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateImageGeometry::CreateImageGeometry()
: m_SelectedDataContainer("ImageGeomDataContainer")
{
  m_Dimensions.x = 0;
  m_Dimensions.y = 0;
  m_Dimensions.z = 0;

  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateImageGeometry::~CreateImageGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateImageGeometry::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container Destination", SelectedDataContainer, FilterParameter::Parameter, CreateImageGeometry, req));
  }
  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Dimensions", Dimensions, FilterParameter::Parameter, CreateImageGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, CreateImageGeometry));

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Resolution", Resolution, FilterParameter::Parameter, CreateImageGeometry));

  PreflightUpdatedValueFilterParameter::Pointer param =
             SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Box Size in Length Units", BoxDimensions, FilterParameter::Parameter, CreateImageGeometry);
  param->setReadOnly(true);
  parameters.push_back(param);

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateImageGeometry::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDimensions(reader->readIntVec3("Dimensions", getDimensions()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));
  setResolution(reader->readFloatVec3("Resolution", getResolution()));
  setSelectedDataContainer(reader->readString("SelectedDataContainer", getSelectedDataContainer()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateImageGeometry::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateImageGeometry::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  if(m_Dimensions.x == 0 || m_Dimensions.y == 0 || m_Dimensions.z == 0)
  {
    QString ss = QObject::tr("One of the dimensions has a size less than or equal to zero. The minimum size must be postive");
    setErrorCondition(-390);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getSelectedDataContainer().isEmpty())
  {
    QString ss = QObject::tr("The Data Container must have a name");
    setErrorCondition(-391);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getSelectedDataContainer());
  if(getErrorCondition() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry("ImageGeometry");
  image->setDimensions(std::make_tuple(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z));
  image->setResolution(std::make_tuple(m_Resolution.x, m_Resolution.y, m_Resolution.z));
  image->setOrigin(std::make_tuple(m_Origin.x, m_Origin.y, m_Origin.z));
  m->setGeometry(image);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateImageGeometry::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateImageGeometry::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateImageGeometry::getBoxDimensions()
{
  QString desc;
  QTextStream ss(&desc);
  float halfRes[3] = { m_Resolution.x/2.0f, m_Resolution.y/2.0f, m_Resolution.z/2.0f };
  ss << "Extents:\n"
     << "X Extent: 0 to " << m_Dimensions.x-1 << " (dimension: " << m_Dimensions.x << ")\n" 
     << "Y Extent: 0 to " << m_Dimensions.y-1 << " (dimension: " << m_Dimensions.y << ")\n" 
     << "Z Extent: 0 to " << m_Dimensions.z-1 << " (dimension: " << m_Dimensions.z << ")\n"     
     << "Bounds:\n"
     << "X Range: " << (m_Origin.x-halfRes[0]) << " to " << (m_Origin.x-halfRes[0] + m_Dimensions.x*m_Resolution.x) << " (delta: " << (m_Dimensions.x*m_Resolution.x) << ")\n" 
     << "Y Range: " << (m_Origin.y-halfRes[1]) << " to " << (m_Origin.y-halfRes[1] + m_Dimensions.y*m_Resolution.y) << " (delta: " << (m_Dimensions.y*m_Resolution.y) << ")\n"
     << "Z Range: " << (m_Origin.z-halfRes[2]) << " to " << (m_Origin.z-halfRes[2] + m_Dimensions.z*m_Resolution.z) << " (delta: " << (m_Dimensions.z*m_Resolution.z) << ")\n";
  return desc;      
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateImageGeometry::newFilterInstance(bool copyFilterParameters) const
{
  CreateImageGeometry::Pointer filter = CreateImageGeometry::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateImageGeometry::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateImageGeometry::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateImageGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateImageGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CreateImageGeometry::getUuid()
{
  return QUuid("{f2132744-3abb-5d66-9cd9-c9a233b5c4aa}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateImageGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateImageGeometry::getHumanLabel() const
{
  return "Create Geometry (Image)";
}
