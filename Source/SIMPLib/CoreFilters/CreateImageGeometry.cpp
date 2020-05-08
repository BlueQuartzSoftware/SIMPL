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

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateImageGeometry::CreateImageGeometry()
: m_SelectedDataContainer("ImageGeomDataContainer")
{
  m_Dimensions[0] = 0;
  m_Dimensions[1] = 0;
  m_Dimensions[2] = 0;

  m_Origin[0] = 0.0f;
  m_Origin[1] = 0.0f;
  m_Origin[2] = 0.0f;

  m_Spacing[0] = 1.0f;
  m_Spacing[1] = 1.0f;
  m_Spacing[2] = 1.0f;
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
  FilterParameterVectorType parameters;
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container Destination", SelectedDataContainer, FilterParameter::Parameter, CreateImageGeometry, req));
  }
  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Dimensions", Dimensions, FilterParameter::Parameter, CreateImageGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, CreateImageGeometry));

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Spacing", Spacing, FilterParameter::Parameter, CreateImageGeometry));
  parameters.back()->setLegacyPropertyName("Resolution");

  PreflightUpdatedValueFilterParameter::Pointer param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Box Size in Length Units", BoxDimensions, FilterParameter::Parameter, CreateImageGeometry);
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
  setSpacing(reader->readFloatVec3("Spacing", getSpacing()));
  setSelectedDataContainer(reader->readDataArrayPath("SelectedDataContainer", getSelectedDataContainer()));
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
  clearErrorCode();
  clearWarningCode();

  if(m_Dimensions[0] == 0 || m_Dimensions[1] == 0 || m_Dimensions[2] == 0)
  {
    QString ss = QObject::tr("One of the dimensions has a size less than or equal to zero. The minimum size must be postive");
    setErrorCondition(-390, ss);
  }

  if(getSelectedDataContainer().isEmpty())
  {
    QString ss = QObject::tr("The Data Container must have a name");
    setErrorCondition(-391, ss);
    return;
  }
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, getSelectedDataContainer());
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry("ImageGeometry");
  image->setDimensions(m_Dimensions.convertType<size_t>());
  image->setSpacing(m_Spacing);
  image->setOrigin(m_Origin);
  m->setGeometry(image);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateImageGeometry::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateImageGeometry::getBoxDimensions()
{
  QString desc;
  QTextStream ss(&desc);
  float halfRes[3] = {m_Spacing[0] / 2.0f, m_Spacing[1] / 2.0f, m_Spacing[2] / 2.0f};
  ss << "Extents:\n"
     << "X Extent: 0 to " << m_Dimensions[0] - 1 << " (dimension: " << m_Dimensions[0] << ")\n"
     << "Y Extent: 0 to " << m_Dimensions[1] - 1 << " (dimension: " << m_Dimensions[1] << ")\n"
     << "Z Extent: 0 to " << m_Dimensions[2] - 1 << " (dimension: " << m_Dimensions[2] << ")\n"
     << "Bounds:\n"
     << "X Range: " << (m_Origin[0] - halfRes[0]) << " to " << (m_Origin[0] - halfRes[0] + m_Dimensions[0] * m_Spacing[0]) << " (delta: " << (m_Dimensions[0] * m_Spacing[0]) << ")\n"
     << "Y Range: " << (m_Origin[1] - halfRes[1]) << " to " << (m_Origin[1] - halfRes[1] + m_Dimensions[1] * m_Spacing[1]) << " (delta: " << (m_Dimensions[1] * m_Spacing[1]) << ")\n"
     << "Z Range: " << (m_Origin[2] - halfRes[2]) << " to " << (m_Origin[2] - halfRes[2] + m_Dimensions[2] * m_Spacing[2]) << " (delta: " << (m_Dimensions[2] * m_Spacing[2]) << ")\n";
  return desc;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateImageGeometry::newFilterInstance(bool copyFilterParameters) const
{
  CreateImageGeometry::Pointer filter = CreateImageGeometry::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateImageGeometry::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateImageGeometry::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateImageGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateImageGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid CreateImageGeometry::getUuid() const
{
  return QUuid("{f2132744-3abb-5d66-9cd9-c9a233b5c4aa}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateImageGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateImageGeometry::getHumanLabel() const
{
  return "Create Geometry (Image)";
}

// -----------------------------------------------------------------------------
CreateImageGeometry::Pointer CreateImageGeometry::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<CreateImageGeometry> CreateImageGeometry::New()
{
  struct make_shared_enabler : public CreateImageGeometry
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString CreateImageGeometry::getNameOfClass() const
{
  return QString("CreateImageGeometry");
}

// -----------------------------------------------------------------------------
QString CreateImageGeometry::ClassName()
{
  return QString("CreateImageGeometry");
}

// -----------------------------------------------------------------------------
void CreateImageGeometry::setSelectedDataContainer(const DataArrayPath& value)
{
  m_SelectedDataContainer = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateImageGeometry::getSelectedDataContainer() const
{
  return m_SelectedDataContainer;
}

// -----------------------------------------------------------------------------
void CreateImageGeometry::setDimensions(const IntVec3Type& value)
{
  m_Dimensions = value;
}

// -----------------------------------------------------------------------------
IntVec3Type CreateImageGeometry::getDimensions() const
{
  return m_Dimensions;
}

// -----------------------------------------------------------------------------
void CreateImageGeometry::setOrigin(const FloatVec3Type& value)
{
  m_Origin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type CreateImageGeometry::getOrigin() const
{
  return m_Origin;
}

// -----------------------------------------------------------------------------
void CreateImageGeometry::setSpacing(const FloatVec3Type& value)
{
  m_Spacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type CreateImageGeometry::getSpacing() const
{
  return m_Spacing;
}
