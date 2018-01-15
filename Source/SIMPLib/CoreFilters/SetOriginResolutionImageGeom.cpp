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

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/SIMPLibVersion.h"

#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

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

  virtual ~SetOriginResolutionImageUpdateVerticesImpl()
  {
  }

  void generate(size_t start, size_t end) const
  {

    for(size_t i = start; i < end; i++)
    {
      m_Nodes[3 * i] -= m_Delta[0];
      m_Nodes[3 * i + 1] -= m_Delta[1];
      m_Nodes[3 * i + 2] -= m_Delta[2];
    }
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    generate(r.begin(), r.end());
  }
#endif
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SetOriginResolutionImageGeom::SetOriginResolutionImageGeom()
: AbstractFilter()
, m_DataContainerName("")
, m_ChangeOrigin(false)
, m_ChangeResolution(false)
{
  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;

  setupFilterParameters();
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
  FilterParameterVector parameters;
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    req.dcGeometryTypes = IGeometry::Types(1, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container Image Geometry to Modify", DataContainerName, FilterParameter::RequiredArray, SetOriginResolutionImageGeom, req));
  }

  QStringList linkedProps("Origin");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Change Origin", ChangeOrigin, FilterParameter::Parameter, SetOriginResolutionImageGeom, linkedProps));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, SetOriginResolutionImageGeom));

  linkedProps.clear();
  linkedProps << "Resolution";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Change Resolution", ChangeResolution, FilterParameter::Parameter, SetOriginResolutionImageGeom, linkedProps));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Resolution", Resolution, FilterParameter::Parameter, SetOriginResolutionImageGeom));

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
  setResolution(reader->readFloatVec3("Resolution", getResolution()));
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
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
  setErrorCondition(0);
  setWarningCondition(0);

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }
  if(getChangeOrigin())
  {
    image->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
  }
  if(getChangeResolution())
  {
    image->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SetOriginResolutionImageGeom::preflight()
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
void SetOriginResolutionImageGeom::execute()
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
AbstractFilter::Pointer SetOriginResolutionImageGeom::newFilterInstance(bool copyFilterParameters)
{
  SetOriginResolutionImageGeom::Pointer filter = SetOriginResolutionImageGeom::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SetOriginResolutionImageGeom::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SetOriginResolutionImageGeom::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SetOriginResolutionImageGeom::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SetOriginResolutionImageGeom::getGroupName()
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid SetOriginResolutionImageGeom::getUuid()
{
  return QUuid("{6d3a3852-6251-5d2e-b749-6257fd0d8951}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SetOriginResolutionImageGeom::getSubGroupName()
{
  return SIMPL::FilterSubGroups::SpatialFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SetOriginResolutionImageGeom::getHumanLabel()
{
  return "Set Origin & Resolution (Image)";
}
