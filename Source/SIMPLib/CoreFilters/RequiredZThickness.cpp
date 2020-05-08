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

#include "RequiredZThickness.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RequiredZThickness::RequiredZThickness()
: m_DataContainerSelection("")
, m_NumZVoxels(-1)
, m_PreflightCheck(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RequiredZThickness::~RequiredZThickness() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RequiredZThickness::setupFilterParameters()
{
  FilterParameterVectorType parameters = getFilterParameters();

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("DataContainer", DataContainerSelection, FilterParameter::RequiredArray, RequiredZThickness, req));
  }
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Minimum Z Dimension", NumZVoxels, FilterParameter::Parameter, RequiredZThickness, 0));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Preflight Check", PreflightCheck, FilterParameter::Parameter, RequiredZThickness));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RequiredZThickness::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  AbstractDecisionFilter::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setDataContainerSelection(reader->readDataArrayPath("DataContainerSelection", getDataContainerSelection()));
  setNumZVoxels(reader->readValue("NumZVoxels", getNumZVoxels()));
  setPreflightCheck(reader->readValue("PreflightCheck", getPreflightCheck()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RequiredZThickness::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RequiredZThickness::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer dataContainer = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerSelection());
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = dataContainer->getGeometryAs<ImageGeom>();
  if(nullptr == image.get())
  {
    setErrorCondition(-7789, "Missing Image Geometry in the selected DataContainer");
    return;
  }

  SizeVec3Type dims = image->getDimensions();

  if(dims[2] < getNumZVoxels() && m_PreflightCheck)
  {
    QString str;
    QTextStream ss(&str);
    ss << "Number of Z Voxels does not meet required value during preflight of the filter. \n";
    ss << "  Required Z Voxels: " << m_NumZVoxels << "\n";
    ss << "  Current Z Voxels: " << dims[2];

    setErrorCondition(-7787, str);
  }
  else if(dims[2] < getNumZVoxels() && !m_PreflightCheck)
  {
    QString str;
    QTextStream ss(&str);
    ss << "Number of Z Voxels does not meet required value during preflight but will be checked during pipeline execution.\n";
    ss << "  Required Z Voxels: " << m_NumZVoxels << "\n";
    ss << "  Current Z Voxels: " << dims[2];

    setWarningCondition(-7788, str);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RequiredZThickness::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer dataContainer = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerSelection());
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = dataContainer->getGeometryAs<ImageGeom>();

  SizeVec3Type dims = image->getDimensions();

  if(dims[2] < getNumZVoxels())
  {
    QString str;
    QTextStream ss(&str);
    ss << "Number of Z Voxels does not meet required value during execution of the filter. \n";
    ss << "  Required Z Voxels: " << m_NumZVoxels << "\n";
    ss << "  Current Z Voxels: " << dims[2];

    setErrorCondition(-7788, str);
    bool needMoreData = true;
    emit decisionMade(needMoreData);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RequiredZThickness::newFilterInstance(bool copyFilterParameters) const
{
  RequiredZThickness::Pointer filter = RequiredZThickness::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RequiredZThickness::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RequiredZThickness::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RequiredZThickness::getUuid() const
{
  return QUuid("{1fe19578-6856-55f2-adc8-2236fac22c25}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RequiredZThickness::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RequiredZThickness::getHumanLabel() const
{
  return "Required Z Dimension (Image Geometry)";
}

// -----------------------------------------------------------------------------
RequiredZThickness::Pointer RequiredZThickness::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
RequiredZThickness::Pointer RequiredZThickness::New()
{
  Pointer sharedPtr(new(RequiredZThickness));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString RequiredZThickness::getNameOfClass() const
{
  return QString("RequiredZThickness");
}

// -----------------------------------------------------------------------------
QString RequiredZThickness::ClassName()
{
  return QString("RequiredZThickness");
}

// -----------------------------------------------------------------------------
void RequiredZThickness::setDataContainerSelection(const DataArrayPath& value)
{
  m_DataContainerSelection = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RequiredZThickness::getDataContainerSelection() const
{
  return m_DataContainerSelection;
}

// -----------------------------------------------------------------------------
void RequiredZThickness::setNumZVoxels(int value)
{
  m_NumZVoxels = value;
}

// -----------------------------------------------------------------------------
int RequiredZThickness::getNumZVoxels() const
{
  return m_NumZVoxels;
}

// -----------------------------------------------------------------------------
void RequiredZThickness::setPreflightCheck(bool value)
{
  m_PreflightCheck = value;
}

// -----------------------------------------------------------------------------
bool RequiredZThickness::getPreflightCheck() const
{
  return m_PreflightCheck;
}
