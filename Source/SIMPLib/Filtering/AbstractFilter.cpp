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

#include "AbstractFilter.h"

#include <QtCore/QDebug>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Messages/FilterErrorMessage.h"
#include "SIMPLib/Messages/FilterProgressMessage.h"
#include "SIMPLib/Messages/FilterStatusMessage.h"
#include "SIMPLib/Messages/FilterWarningMessage.h"
#include "SIMPLib/Plugin/PluginManager.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::AbstractFilter()
{
  m_DataContainerArray = DataContainerArray::New();
  m_PreviousFilter = NullPointer();
  m_NextFilter = NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::~AbstractFilter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AbstractFilter::CreateFilterFromClassName(const QString& className)
{
  FilterManager* fm = FilterManager::Instance();
  if(nullptr == fm)
  {
    return AbstractFilter::NullPointer();
  }
  IFilterFactory::Pointer wf = fm->getFactoryFromClassName(className);
  if(nullptr == wf)
  {
    return AbstractFilter::NullPointer();
  }

  // Create an instance of the filter. Since we are dealing with the AbstractFilter interface we can not
  // actually use the concrete filter class. We are going to have to rely on QProperties or Signals/Slots
  // to communicate changes back to the filter.
  AbstractFilter::Pointer filter = wf->create();
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::setCancel(bool value)
{
  m_Cancel = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::renameDataArrayPath(const DataArrayPath::RenameType& renamePath)
{
  // If the path is created here, do not rename
  for(const auto& createdPath : m_CreatedPaths)
  {
    if(createdPath.second == renamePath.first)
    {
      return;
    }
  }

  // Some filter parameters handle paths as nothing but a QString (i.e. DataContainerSelectionFilterParameter)
  // This does not store data in a way that represents what is stored or in a consistent manner with anything else.
  // Because this format cannot be quieried nicely like the above code, filter parameters have to be able to update
  // their own paths in these cases.  In cases where private properties need to be updated but do not have
  // assigned filter parameters, although this should not be the case, this code, unlike the above snippet, will not update them.
  FilterParameterVectorType filterParams = getFilterParameters();
  for(FilterParameter::Pointer& filterParam : filterParams)
  {
    filterParam->dataArrayPathRenamed(this, renamePath);
  }

  // Update created paths
  for(const auto& createdPathItem : m_CreatedPaths)
  {
    DataArrayPath createdPath = createdPathItem.second;
    if(createdPath.updatePath(renamePath))
    {
      m_CreatedPaths[createdPathItem.first] = createdPath;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::setupFilterParameters()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::preflight()
{
  setInPreflight(true);
  Q_EMIT preflightAboutToExecute();
  Q_EMIT updateFilterParameters(this);
  dataCheck();
  Q_EMIT preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::doesPipelineContainFilterBeforeThis(const QString& name)
{
  bool contains = false;
  // Check the previous filter
  AbstractFilter::Pointer prev = getPreviousFilter().lock();
  while(prev.get() != nullptr)
  {
    if(prev->getNameOfClass().compare(name) == 0)
    {
      contains = true;
      break;
    }
    prev = prev->getPreviousFilter().lock();
  }
  return contains;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::doesPipelineContainFilterAfterThis(const QString& name)
{
  bool contains = false;
  // Check the previous filter
  AbstractFilter::Pointer next = getNextFilter().lock();
  while(next.get() != nullptr)
  {
    if(next->getNameOfClass().compare(name) == 0)
    {
      contains = true;
      break;
    }
    next = next->getNextFilter().lock();
  }
  return contains;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void insertAttributeMatrixPaths(DataContainer::Pointer dc, AttributeMatrix::Pointer am, std::list<DataArrayPath>& paths)
{
  paths.push_back(DataArrayPath(dc->getName(), am->getName(), ""));
  // Insert all DataArrayPaths
  for(const QString& daName : am->getAttributeArrayNames())
  {
    DataArrayPath daPath(dc->getName(), am->getName(), daName);
    paths.push_back(daPath);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void insertDataContainerPaths(const DataContainer::Pointer& dc, std::list<DataArrayPath>& paths)
{
  paths.emplace_back(DataArrayPath(dc->getName(), "", ""));
  // Insert all AttributeMatrix paths
  for(const AttributeMatrix::Pointer& am : dc->getAttributeMatrices())
  {
    insertAttributeMatrixPaths(dc, am, paths);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<DataArrayPath> AbstractFilter::getCreatedPaths()
{
  std::list<DataArrayPath> createdPaths;
  // Check if the DataContainerArray is valid
  if(nullptr == getDataContainerArray())
  {
    return createdPaths;
  }

  AbstractFilter::Pointer previousFilter = getPreviousFilter().lock();
  if(previousFilter && nullptr != previousFilter->getDataContainerArray())
  {
    DataContainerArray::Pointer prevDca = previousFilter->getDataContainerArray();
    DataContainerArray::Pointer dca = getDataContainerArray();

    // Check DataContainers
    for(const DataContainer::Pointer& dc : dca->getDataContainers())
    {
      if(!prevDca->doesDataContainerExist(dc->getName()))
      {
        insertDataContainerPaths(dc, createdPaths);
      }
      else
      {
        // Check AttributeMatrices
        for(const AttributeMatrix::Pointer& am : dc->getAttributeMatrices())
        {
          DataArrayPath amPath(dc->getName(), am->getName(), "");
          if(!prevDca->doesAttributeMatrixExist(amPath))
          {
            insertAttributeMatrixPaths(dc, am, createdPaths);
          }
          else
          {
            // Check DataArrays
            for(const QString& daName : am->getAttributeArrayNames())
            {
              DataArrayPath daPath(dc->getName(), am->getName(), daName);
              if(!prevDca->doesAttributeArrayExist(daPath))
              {
                createdPaths.push_back(daPath);
              }
            }
          }
        }
      }
    }
  }
  else
  {
    // Add all paths if this is the first filter in the pipeline
    DataContainerArray::Pointer dca = getDataContainerArray();
    if(nullptr != dca)
    {
      // Add all paths if there is no previous filter to compare to
      for(const DataContainer::Pointer& dc : dca->getDataContainers())
      {
        insertDataContainerPaths(dc, createdPaths);
      }
    }
  }

  return createdPaths;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<DataArrayPath> AbstractFilter::getCreatedPathsNotRenamed()
{
  std::list<DataArrayPath> createdPaths = getCreatedPaths();
  const DataArrayPath::RenameContainer renamedPaths = getRenamedPaths();

  for(const auto& renamedPath : renamedPaths)
  {
    auto iter = std::find(createdPaths.begin(), createdPaths.end(), std::get<0>(renamedPath));
    if(iter != createdPaths.end())
    {
      createdPaths.erase(iter);
    }
  }

  return createdPaths;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<DataArrayPath> AbstractFilter::getDeletedPaths()
{
  return std::list<DataArrayPath>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::RenameContainer AbstractFilter::getRenamedPaths()
{
  // Implemented in filters that rename existing paths
  return m_RenamedPaths;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  Q_ASSERT(reader != nullptr);
  qDebug() << "AbstractFilter::readFilterParameters() -> Writing Filter Options"
           << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::readFilterParameters(QJsonObject& obj)
{
  FilterParameterVectorType filterParameters = getFilterParameters();
  for(auto const& fp : filterParameters)
  {
    fp->readJson(obj);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::preWriteFilterParameters(QJsonObject& obj, QJsonObject& rootObject)
{
  Q_UNUSED(obj);
  Q_UNUSED(rootObject);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::writeFilterParameters(QJsonObject& obj) const
{
  FilterParameterVectorType filterParameters = getFilterParameters();
  for(auto const& fp : filterParameters)
  {
    fp->writeJson(obj);
  }
  obj[SIMPL::Settings::FilterVersion] = getFilterVersion();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::postWriteFilterParameters(QJsonObject& obj, QJsonObject& rootObject)
{
  Q_UNUSED(obj);
  Q_UNUSED(rootObject);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject AbstractFilter::toJson()
{
  QJsonObject json;

  json[SIMPL::Settings::FilterName] = getNameOfClass();
  json[SIMPL::Settings::HumanLabel] = getHumanLabel();
  json[SIMPL::Settings::GroupName] = getGroupName();
  json[SIMPL::Settings::SubGroupName] = getSubGroupName();
  json[SIMPL::Settings::BrandingString] = getBrandingString();
  json[SIMPL::Settings::CompiledLibraryName] = getCompiledLibraryName();
  json[SIMPL::Settings::FilterEnabled] = getEnabled();
  json[SIMPL::Settings::FilterUuid] = getUuid().toString();

  writeFilterParameters(json);

  return json;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::getCancel() const
{
  return m_Cancel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ISIMPLibPlugin* AbstractFilter::getPluginInstance()
{
  PluginManager* manager = PluginManager::Instance();
  QString pluginName = manager->getPluginName(getNameOfClass());
  ISIMPLibPlugin* plugin = manager->findPlugin(pluginName);
  return plugin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AbstractFilter::newFilterInstance(bool copyFilterParameters) const
{
  return NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::copyFilterParameterInstanceVariables(AbstractFilter* filter) const
{
  QJsonObject filterJson;
  writeFilterParameters(filterJson);
  filter->readFilterParameters(filterJson);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbstractFilter::getGroupName() const
{
  return "YOUR CLASS SHOULD IMPLEMENT THIS";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbstractFilter::getSubGroupName() const
{
  return "YOUR CLASS SHOULD IMPLEMENT THIS";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbstractFilter::getHumanLabel() const
{
  return "YOUR CLASS SHOULD IMPLEMENT THIS";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbstractFilter::getBrandingString() const
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbstractFilter::getCompiledLibraryName() const
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbstractFilter::getFilterVersion() const
{
  return QString("0.0.0");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbstractFilter::generateHtmlSummary() const
{
  QString html;
  QTextStream ss(&html);
  ss << "<html><head></head>\n";
  ss << "<body>\n";

  // A table for the summary items
  ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\">\n";
  ss << "<tbody>\n";
  ss << "	<tr><th align=\"right\">Filter Name:</th><td>" << getHumanLabel() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Class Name:</th><td>" << getNameOfClass() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Group Name:</th><td>" << getGroupName() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Subroup Name:</th><td>" << getSubGroupName() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Version:</th><td>" << getFilterVersion() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Branding String:</th><td>" << getBrandingString() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Compiled Plugin Name:</th><td>" << getCompiledLibraryName() << "</td></tr>\n";
  ss << "</tbody></table>\n";
  ss << "</body></html>";
  return html;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::printValues(std::ostream& out) const
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::cleanupFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::setErrorCondition(int code, const QString& messageText)
{
  m_ErrorCode = code;
  FilterErrorMessage::Pointer pm = FilterErrorMessage::New(getNameOfClass(), getHumanLabel(), getPipelineIndex(), messageText, code);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::notifyStatusMessage(const QString& messageText) const
{
  FilterStatusMessage::Pointer pm = FilterStatusMessage::New(getNameOfClass(), getHumanLabel(), getPipelineIndex(), messageText);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::setWarningCondition(int code, const QString& messageText)
{
  m_WarningCode = code;
  FilterWarningMessage::Pointer pm = FilterWarningMessage::New(getNameOfClass(), getHumanLabel(), getPipelineIndex(), messageText, code);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::notifyProgressMessage(int progress, const QString& messageText) const
{
  FilterProgressMessage::Pointer pm = FilterProgressMessage::New(getNameOfClass(), getHumanLabel(), getPipelineIndex(), messageText, progress);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::clearErrorCode()
{
  m_ErrorCode = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::clearWarningCode()
{
  m_WarningCode = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::notifyMissingProperty(FilterParameter* filterParameter)
{
  QString ss =
      QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
              " Please report this issue to the developers of this filter.")
          .arg(filterParameter->getPropertyName(), getHumanLabel());

  setWarningCondition(-1, ss);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::checkIfPathRenamed(const RenameDataPath::DataID_t id, const DataArrayPath& path)
{
  if(id == RenameDataPath::k_Invalid_ID)
  {
    return false;
  }

  if(m_CreatedPaths.find(id) == m_CreatedPaths.end())
  {
    m_CreatedPaths[id] = path;
    return false;
  }

  if(m_CreatedPaths[id] == path)
  {
    return false;
  }

  m_RenamedPaths.push_back(std::make_pair(m_CreatedPaths[id], path));
  m_CreatedPaths[id] = path;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::clearRenamedPaths()
{
  m_RenamedPaths.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addPathRename(const DataArrayPath& oldPath, const DataArrayPath& newPath)
{
  m_RenamedPaths.push_back(std::make_pair(oldPath, newPath));
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer AbstractFilter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
QString AbstractFilter::ClassName()
{
  return QString("AbstractFilter");
}

// -----------------------------------------------------------------------------
void AbstractFilter::setDataContainerArray(const DataContainerArray::Pointer& value)
{
  m_DataContainerArray = value;
}

// -----------------------------------------------------------------------------
DataContainerArray::Pointer AbstractFilter::getDataContainerArray() const
{
  return m_DataContainerArray;
}

// -----------------------------------------------------------------------------
void AbstractFilter::setFilterParameters(const FilterParameterVectorType& value)
{
  m_FilterParameters = value;
}

// -----------------------------------------------------------------------------
FilterParameterVectorType AbstractFilter::getFilterParameters() const
{
  return m_FilterParameters;
}

// -----------------------------------------------------------------------------
void AbstractFilter::setInPreflight(bool value)
{
  m_InPreflight = value;
}

// -----------------------------------------------------------------------------
bool AbstractFilter::getInPreflight() const
{
  return m_InPreflight;
}

// -----------------------------------------------------------------------------
void AbstractFilter::setEnabled(bool value)
{
  m_Enabled = value;
}

// -----------------------------------------------------------------------------
bool AbstractFilter::getEnabled() const
{
  return m_Enabled;
}

// -----------------------------------------------------------------------------
void AbstractFilter::setRemoving(bool value)
{
  m_Removing = value;
}

// -----------------------------------------------------------------------------
bool AbstractFilter::getRemoving() const
{
  return m_Removing;
}

// -----------------------------------------------------------------------------
void AbstractFilter::setPipelineIndex(int value)
{
  m_PipelineIndex = value;
}

// -----------------------------------------------------------------------------
int AbstractFilter::getPipelineIndex() const
{
  return m_PipelineIndex;
}

// -----------------------------------------------------------------------------
void AbstractFilter::setPreviousFilter(const AbstractFilter::WeakPointer& value)
{
  m_PreviousFilter = value;
}

// -----------------------------------------------------------------------------
AbstractFilter::WeakPointer AbstractFilter::getPreviousFilter() const
{
  return m_PreviousFilter;
}

// -----------------------------------------------------------------------------
void AbstractFilter::setNextFilter(const AbstractFilter::WeakPointer& value)
{
  m_NextFilter = value;
}

// -----------------------------------------------------------------------------
AbstractFilter::WeakPointer AbstractFilter::getNextFilter() const
{
  return m_NextFilter;
}

// -----------------------------------------------------------------------------
int AbstractFilter::getErrorCode() const
{
  return m_ErrorCode;
}

// -----------------------------------------------------------------------------
int AbstractFilter::getWarningCode() const
{
  return m_WarningCode;
}
