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
#include "SetOriginResolutionImageGeom.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/Utilities/ParallelDataAlgorithm.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

/**
 * @brief The SetOriginResolutionImageUpdateVerticesImpl class implements a threaded algorithm that adjusts the
 * positions of a set of nodes relative to a new origin
 */
class SetOriginResolutionImageUpdateVerticesImpl
{
  float* m_Nodes;
  float* m_Delta;

public:
  SetOriginResolutionImageUpdateVerticesImpl(float* nodes, float* delta)
  : m_Nodes(nodes)
  , m_Delta(delta)
  {
  }

  virtual ~SetOriginResolutionImageUpdateVerticesImpl() = default;

  void generate(size_t start, size_t end) const
  {

    for(size_t i = start; i < end; i++)
    {
      m_Nodes[3 * i] -= m_Delta[0];
      m_Nodes[3 * i + 1] -= m_Delta[1];
      m_Nodes[3 * i + 2] -= m_Delta[2];
    }
  }

  void operator()(const SIMPLRange& range) const
  {
    generate(range.min(), range.max());
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SetOriginResolutionImageGeom::SetOriginResolutionImageGeom()
: m_DataContainerName("")
, m_ChangeOrigin(false)
, m_ChangeResolution(false)
{
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
SetOriginResolutionImageGeom::~SetOriginResolutionImageGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SetOriginResolutionImageGeom::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    req.dcGeometryTypes = IGeometry::Types(1, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container Image Geometry to Modify", DataContainerName, FilterParameter::RequiredArray, SetOriginResolutionImageGeom, req));
  }

  QStringList linkedProps("Origin");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Change Origin", ChangeOrigin, FilterParameter::Parameter, SetOriginResolutionImageGeom, linkedProps));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, SetOriginResolutionImageGeom));

  linkedProps.clear();
  linkedProps << "Spacing";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Change Spacing", ChangeResolution, FilterParameter::Parameter, SetOriginResolutionImageGeom, linkedProps));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Spacing", Spacing, FilterParameter::Parameter, SetOriginResolutionImageGeom));
  parameters.back()->setLegacyPropertyName("Resolution");

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SetOriginResolutionImageGeom::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setChangeOrigin(reader->readValue("ChangeOrigin", getChangeOrigin()));
  setChangeResolution(reader->readValue("ChangeResolution", getChangeResolution()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));
  setSpacing(reader->readFloatVec3("Spacing", getSpacing()));
  setDataContainerName(reader->readDataArrayPath("DataContainerName", getDataContainerName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SetOriginResolutionImageGeom::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SetOriginResolutionImageGeom::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom>(this, getDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }
  if(getChangeOrigin())
  {
    image->setOrigin(std::make_tuple(m_Origin[0], m_Origin[1], m_Origin[2]));
  }
  if(getChangeResolution())
  {
    image->setSpacing(std::make_tuple(m_Spacing[0], m_Spacing[1], m_Spacing[2]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SetOriginResolutionImageGeom::execute()
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
AbstractFilter::Pointer SetOriginResolutionImageGeom::newFilterInstance(bool copyFilterParameters) const
{
  SetOriginResolutionImageGeom::Pointer filter = SetOriginResolutionImageGeom::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SetOriginResolutionImageGeom::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SetOriginResolutionImageGeom::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SetOriginResolutionImageGeom::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SetOriginResolutionImageGeom::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid SetOriginResolutionImageGeom::getUuid() const
{
  return QUuid("{6d3a3852-6251-5d2e-b749-6257fd0d8951}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SetOriginResolutionImageGeom::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::SpatialFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SetOriginResolutionImageGeom::getHumanLabel() const
{
  return "Set Origin & Spacing (Image)";
}

// -----------------------------------------------------------------------------
SetOriginResolutionImageGeom::Pointer SetOriginResolutionImageGeom::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<SetOriginResolutionImageGeom> SetOriginResolutionImageGeom::New()
{
  struct make_shared_enabler : public SetOriginResolutionImageGeom
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString SetOriginResolutionImageGeom::getNameOfClass() const
{
  return QString("SetOriginResolutionImageGeom");
}

// -----------------------------------------------------------------------------
QString SetOriginResolutionImageGeom::ClassName()
{
  return QString("SetOriginResolutionImageGeom");
}

// -----------------------------------------------------------------------------
void SetOriginResolutionImageGeom::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath SetOriginResolutionImageGeom::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void SetOriginResolutionImageGeom::setChangeOrigin(bool value)
{
  m_ChangeOrigin = value;
}

// -----------------------------------------------------------------------------
bool SetOriginResolutionImageGeom::getChangeOrigin() const
{
  return m_ChangeOrigin;
}

// -----------------------------------------------------------------------------
void SetOriginResolutionImageGeom::setOrigin(const FloatVec3Type& value)
{
  m_Origin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type SetOriginResolutionImageGeom::getOrigin() const
{
  return m_Origin;
}

// -----------------------------------------------------------------------------
void SetOriginResolutionImageGeom::setChangeResolution(bool value)
{
  m_ChangeResolution = value;
}

// -----------------------------------------------------------------------------
bool SetOriginResolutionImageGeom::getChangeResolution() const
{
  return m_ChangeResolution;
}

// -----------------------------------------------------------------------------
void SetOriginResolutionImageGeom::setSpacing(const FloatVec3Type& value)
{
  m_Spacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type SetOriginResolutionImageGeom::getSpacing() const
{
  return m_Spacing;
}
