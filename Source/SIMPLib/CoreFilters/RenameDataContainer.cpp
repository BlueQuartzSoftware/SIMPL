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

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

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
  FilterParameterVector parameters;

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container to Rename", SelectedDataContainerName, FilterParameter::RequiredArray, RenameDataContainer, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("New Data Container Name", NewDataContainerName, FilterParameter::Parameter, RenameDataContainer));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataContainer::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedDataContainerName(reader->readString("SelectedDataContainerName", getSelectedDataContainerName()));
  setNewDataContainerName(reader->readString("NewDataContainerName", getNewDataContainerName()));
  reader->closeFilterGroup();
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
  setErrorCondition(0);
  setWarningCondition(0);

  if(getNewDataContainerName().isEmpty() == true)
  {
    setErrorCondition(-11001);
    QString ss = QObject::tr("The new Data Container name must be set");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getSelectedDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  bool check = getDataContainerArray()->renameDataContainer(getSelectedDataContainerName(), getNewDataContainerName());
  if(check == false)
  {
    setErrorCondition(-11006);
    QString ss = QObject::tr("Attempt to rename DataContainer '%1' to '%2' failed").arg(getSelectedDataContainerName()).arg(getNewDataContainerName());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataContainer::preflight()
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
void RenameDataContainer::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck(); // calling the dataCheck will rename the array, so nothing is required here
  if(getErrorCondition() < 0)
  {
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RenameDataContainer::newFilterInstance(bool copyFilterParameters) const
{
  RenameDataContainer::Pointer filter = RenameDataContainer::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameDataContainer::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameDataContainer::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameDataContainer::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameDataContainer::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid RenameDataContainer::getUuid()
{
  return QUuid("{d53c808f-004d-5fac-b125-0fffc8cc78d6}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameDataContainer::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameDataContainer::getHumanLabel() const
{
  return "Rename Data Container";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::RenameContainer RenameDataContainer::getRenamedPaths()
{
  DataArrayPath oldPath(getSelectedDataContainerName(), "", "");
  DataArrayPath newPath(getNewDataContainerName(), "", "");

  DataArrayPath::RenameContainer container;
  container.push_back(DataArrayPath::RenameType(oldPath, newPath));

  return container;
}
