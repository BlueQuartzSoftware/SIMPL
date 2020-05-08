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
#include "RenameDataContainer.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameDataContainer::RenameDataContainer()
: m_SelectedDataContainerName("")
, m_NewDataContainerName("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameDataContainer::~RenameDataContainer() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataContainer::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container to Rename", SelectedDataContainerName, FilterParameter::RequiredArray, RenameDataContainer, req));
  }

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("New Data Container Name", NewDataContainerName, FilterParameter::Parameter, RenameDataContainer));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataContainer::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataContainer::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(getNewDataContainerName().isEmpty())
  {
    QString ss = QObject::tr("The new Data Container name must be set");
    setErrorCondition(-11001, ss);
    return;
  }

  if(m_LastContainerName != m_NewDataContainerName)
  {
    DataArrayPath oldContainerPath = m_LastContainerName;
    DataArrayPath newContainerPath = getNewDataContainerName();
    addPathRename(oldContainerPath, newContainerPath);
    m_LastContainerName = getNewDataContainerName();
  }

  getDataContainerArray()->getPrereqDataContainer(this, getSelectedDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }

  bool check = getDataContainerArray()->renameDataContainer(getSelectedDataContainerName(), getNewDataContainerName());
  if(!check)
  {
    QString ss = QObject::tr("Attempt to rename DataContainer '%1' to '%2' failed").arg(getSelectedDataContainerName().getDataContainerName()).arg(getNewDataContainerName().getDataContainerName());
    setErrorCondition(-11006, ss);
  }
  else
  {
    DataArrayPath oldPath = getSelectedDataContainerName();
    DataArrayPath newPath = getNewDataContainerName();
    addPathRename(oldPath, newPath);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataContainer::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck(); // calling the dataCheck will rename the array, so nothing is required here
  if(getErrorCode() < 0)
  {
    return;
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RenameDataContainer::newFilterInstance(bool copyFilterParameters) const
{
  RenameDataContainer::Pointer filter = RenameDataContainer::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameDataContainer::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameDataContainer::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameDataContainer::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameDataContainer::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RenameDataContainer::getUuid() const
{
  return QUuid("{d53c808f-004d-5fac-b125-0fffc8cc78d6}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameDataContainer::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameDataContainer::getHumanLabel() const
{
  return "Rename Data Container";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::RenameContainer RenameDataContainer::getRenamedPaths()
{
  DataArrayPath oldPath = getSelectedDataContainerName();
  DataArrayPath newPath = getNewDataContainerName();

  DataArrayPath::RenameContainer container;
  container.push_back(std::make_pair(oldPath, newPath));

  return container;
}

// -----------------------------------------------------------------------------
RenameDataContainer::Pointer RenameDataContainer::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<RenameDataContainer> RenameDataContainer::New()
{
  struct make_shared_enabler : public RenameDataContainer
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RenameDataContainer::getNameOfClass() const
{
  return QString("RenameDataContainer");
}

// -----------------------------------------------------------------------------
QString RenameDataContainer::ClassName()
{
  return QString("RenameDataContainer");
}

// -----------------------------------------------------------------------------
void RenameDataContainer::setSelectedDataContainerName(const DataArrayPath& value)
{
  m_SelectedDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RenameDataContainer::getSelectedDataContainerName() const
{
  return m_SelectedDataContainerName;
}

// -----------------------------------------------------------------------------
void RenameDataContainer::setNewDataContainerName(const DataArrayPath& value)
{
  m_NewDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RenameDataContainer::getNewDataContainerName() const
{
  return m_NewDataContainerName;
}
