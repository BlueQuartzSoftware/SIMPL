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

#include "FilterPipeline.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/CoreFilters/EmptyFilter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/RenameDataPath.h"
#include "SIMPLib/Filtering/BadFilter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Messages/AbstractMessageHandler.h"
#include "SIMPLib/Messages/FilterErrorMessage.h"
#include "SIMPLib/Messages/FilterProgressMessage.h"
#include "SIMPLib/Messages/FilterStatusMessage.h"
#include "SIMPLib/Messages/FilterWarningMessage.h"
#include "SIMPLib/Messages/PipelineErrorMessage.h"
#include "SIMPLib/Messages/PipelineProgressMessage.h"
#include "SIMPLib/Messages/PipelineStatusMessage.h"
#include "SIMPLib/Messages/PipelineWarningMessage.h"
#include "SIMPLib/Utilities/StringOperations.h"

#define RENAME_ENABLED 1

/**
 * @brief This message handler is used by FilterPipeline to re-emit filter progress messages as pipeline progress messages
 */
class FilterPipelineMessageHandler : public AbstractMessageHandler
{
public:
  explicit FilterPipelineMessageHandler(FilterPipeline* pipeline)
  : m_Pipeline(pipeline)
  {
  }

  /**
   * @brief Converts filter progress messages into pipeline progress messages.  This enables the overall pipeline
   * progress to update along with the filter's progress updates
   */
  void processMessage(const FilterProgressMessage* msg) const override
  {
    int filterProgress = msg->getProgressValue();
    float filterProgressStep = (1.0f / m_Pipeline->size()) * filterProgress / 100.0f;
    int pipelineProgress = static_cast<int>((static_cast<float>(msg->getPipelineIndex()) / (m_Pipeline->size()) + filterProgressStep) * 100.0f);
    m_Pipeline->notifyProgressMessage(pipelineProgress, "");
  }

private:
  FilterPipeline* m_Pipeline = nullptr;
};

class PipelineIdleException : public std::exception
{
  const char* what() const noexcept
  {
    return "A pipeline that was finishing execution was marked as in the idle state.";
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::FilterPipeline()
: m_PipelineName("")
, m_Dca(nullptr)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::~FilterPipeline() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterPipeline::getName() const
{
  return m_PipelineName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::setName(const QString& newName)
{
  QString oldName = m_PipelineName;
  m_PipelineName = newName;

  Q_EMIT pipelineNameChanged(oldName, newName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer FilterPipeline::deepCopy()
{
  // This is not the fastest way to perform a deep copy, but it does avoid
  // issues with newFilterInstance retaining bindings to the original filter
  // that may no longer exist.

  // Convert to JSon
  QJsonObject json = toJson();

  // Convert from JSon
  FilterPipeline::Pointer copy = FilterPipeline::New();
  copy->fromJson(json);

  return copy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject FilterPipeline::toJson()
{
  QJsonObject json;

  FilterContainerType container = getFilterContainer();
  int count = container.size();
  int offset = 0;

  for(qint32 i = 0; i < count; i++)
  {
    const AbstractFilter::Pointer& filter = container.at(i);
    if(nullptr != filter.get())
    {
      DataContainerReader::Pointer reader = std::dynamic_pointer_cast<DataContainerReader>(filter);
      if(reader.get() != nullptr)
      {
        offset = reader->writeExistingPipelineToFile(json, i);
      }

      json.insert(QString::number(i + offset), filter->toJson());
    }
    else
    {
      AbstractFilter::Pointer badFilter = BadFilter::New();

      QJsonObject badFilterJson = badFilter->toJson();
      badFilterJson["Unknown Filter"] = "ERROR: Filter instance was nullptr within the FilterPipeline instance. Report this error to the DREAM3D Developers";

      json.insert(QString::number(i + offset), badFilterJson);
    }
  }

  // Add meta data about the pipeline
  {
    QJsonObject meta;
    meta[SIMPL::Settings::PipelineName] = m_PipelineName;
    meta[SIMPL::Settings::Version] = SIMPL::PipelineVersionNumbers::CurrentVersion;

    if(!json.empty())
    {
      meta[SIMPL::Settings::NumFilters] = count;
    }
    else
    {
      meta[SIMPL::Settings::NumFilters] = 0;
    }

    json[SIMPL::Settings::PipelineBuilderGroup] = meta;
  }

  return json;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer FilterPipeline::FromJson(const QJsonObject& json, IObserver* obs)
{
  if(!json.contains(SIMPL::Settings::PipelineBuilderGroup) || !json[SIMPL::Settings::PipelineBuilderGroup].isObject())
  {
    return FilterPipeline::NullPointer();
  }

  QJsonObject builderObj = json[SIMPL::Settings::PipelineBuilderGroup].toObject();
  if(!builderObj.contains(SIMPL::Settings::NumFilters) || !builderObj[SIMPL::Settings::NumFilters].isDouble())
  {
    return FilterPipeline::NullPointer();
  }

  int filterCount = builderObj[SIMPL::Settings::NumFilters].toInt();

  FilterPipeline::Pointer pipeline;
  if(filterCount >= 0)
  {
    pipeline = FilterPipeline::New();
    pipeline->fromJson(json, obs);
  }
  else
  {
    pipeline = FilterPipeline::NullPointer();
  }
  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::fromJson(const QJsonObject& json, IObserver* obs)
{
  // Clear the pipeline first
  clear();

  // Store FilterManager
  FilterManager* filtManager = FilterManager::Instance();
  FilterFactory<EmptyFilter>::Pointer emptyFilterFactory = FilterFactory<EmptyFilter>::New();
  if(!filtManager->contains(emptyFilterFactory->getUuid()))
  {
    filtManager->addFilterFactory("EmptyFilter", emptyFilterFactory);
  }

  QJsonObject builderObj = json[SIMPL::Settings::PipelineBuilderGroup].toObject();
  int filterCount = builderObj[SIMPL::Settings::NumFilters].toInt();
  //  int maxFilterIndex = filterCount - 1; // Zero based indexing

  setName(builderObj[SIMPL::Settings::PipelineName].toString());

  for(int i = 0; i < filterCount; ++i)
  {
    bool errorLoadingFilter = true;
    QJsonObject currentFilterIndex;
    QString filterName;
    bool filterEnabled = false;
    QString numStr = QString::number(i);
    if(json.find(numStr) != json.end())
    {
      currentFilterIndex = json[numStr].toObject();
      errorLoadingFilter = false;
    }
    if(currentFilterIndex.isEmpty())
    {
      numStr = StringOperations::GenerateIndexString(i, filterCount);
      currentFilterIndex = json[numStr].toObject();
    }

    if(!currentFilterIndex.isEmpty())
    {
      IFilterFactory::Pointer factory = IFilterFactory::NullPointer();
      filterEnabled = currentFilterIndex[SIMPL::Settings::FilterEnabled].toBool(true);
      // First try the UUID for the filter and see what we get.
      QUuid uuid = QUuid(currentFilterIndex[SIMPL::Settings::FilterUuid].toString(""));
      if(!uuid.isNull())
      {
        factory = filtManager->getFactoryFromUuid(uuid);
      }
      // If the UUID was not available, then try the filter class name
      if(nullptr == factory.get())
      {
        QJsonValue jsValue = currentFilterIndex[SIMPL::Settings::FilterName];
        if(jsValue.isString())
        {
          filterName = jsValue.toString("JSON Key 'Filter_Name' missing.");
          factory = filtManager->getFactoryFromClassName(filterName);
          errorLoadingFilter = true;
        }
      }

      if(nullptr != factory.get())
      {
        AbstractFilter::Pointer filter = factory->create();
        errorLoadingFilter = true; // Make this an error until we get through the next logic statement
        if(nullptr != filter.get())
        {
          errorLoadingFilter = false;
          filter->setEnabled(filterEnabled);
          filter->readFilterParameters(currentFilterIndex);
          this->pushBack(filter);
        }
      }
    }

    if(errorLoadingFilter)
    {
      EmptyFilter::Pointer filter = EmptyFilter::New();
      QString humanLabel = QString("UNKNOWN FILTER: ") + filterName;
      filter->setHumanLabel(humanLabel);
      filter->setOriginalFilterName(filterName);
      filter->setEnabled(filterEnabled);
      this->pushBack(filter);

      if(nullptr != obs)
      {
        QString ss = QObject::tr("An attempt to instantiate a filter from the pipeline file resulted in an error.\
                                 Possible reasons include a name change of the filter, plugin not loading or a simple spelling mistake? A \
                                                                                                                                        blank filter has been inserted in its place. Possible error message is: %1")
                         .arg(filterName);
        QString prefix = "JsonFilterParametersReader::ReadPipelineFromFile()";
        PipelineErrorMessage::Pointer pm = PipelineErrorMessage::New(getName(), ss, -66066);
        obs->processPipelineMessage(pm);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::cancel()
{
  if(m_State != FilterPipeline::State::Executing)
  {
    // We cannot cancel a pipeline that is not executing
    QString ss;
    if(m_State == FilterPipeline::State::Idle)
    {
      ss = QObject::tr("Pipeline '%1' could not be canceled because it is not executing.").arg(getName());
    }
    else if(m_State == FilterPipeline::State::Canceling)
    {
      ss = QObject::tr("Pipeline '%1' could not be canceled because it is already canceling.").arg(getName());
    }
    else
    {
      ss = QObject::tr("Pipeline '%1' could not be canceled.").arg(getName());
    }
    setErrorCondition(-201, ss);
    return;
  }

  m_State = FilterPipeline::State::Canceling;
  if(nullptr != m_CurrentFilter.get())
  {
    m_CurrentFilter->setCancel(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer FilterPipeline::run()
{
  m_Dca = execute();

  Q_EMIT pipelineFinished();

  return m_Dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::pushFront(const AbstractFilter::Pointer& f)
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // Do not allow anyone to add filters to a pipeline that is not idle
    QString ss = QObject::tr("Filter '%1' could not be added to pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    setErrorCondition(-202, ss);
    return false;
  }

  m_Pipeline.push_front(f);
  updatePrevNextFilters();
  Q_EMIT pipelineWasEdited();
  return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::popFront()
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // Do not allow anyone to remove filters from a pipeline that is not idle
    AbstractFilter::Pointer f = m_Pipeline.front();

    QString ss = QObject::tr("Filter '%1' could not be removed from pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    setErrorCondition(-203, ss);
    return false;
  }

  m_Pipeline.pop_front();
  updatePrevNextFilters();
  Q_EMIT pipelineWasEdited();
  return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::pushBack(const AbstractFilter::Pointer& f)
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // Do not allow anyone to add filters to a pipeline that is not idle
    QString ss = QObject::tr("Filter '%1' could not be added to pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    setErrorCondition(-204, ss);
    return false;
  }

  m_Pipeline.push_back(f);
  updatePrevNextFilters();
  Q_EMIT pipelineWasEdited();
  return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::popBack()
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // Do not allow anyone to remove filters from a pipeline that is not idle
    AbstractFilter::Pointer f = m_Pipeline.back();

    QString ss = QObject::tr("Filter '%1' could not be removed from pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    setErrorCondition(-205, ss);
    return false;
  }

  m_Pipeline.pop_back();
  updatePrevNextFilters();
  Q_EMIT pipelineWasEdited();
  return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::insert(size_t index, const AbstractFilter::Pointer& f)
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // Do not allow anyone to add filters to a pipeline that is not idle
    QString ss = QObject::tr("Filter '%1' could not be added to pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    setErrorCondition(-206, ss);
    return false;
  }

  FilterContainerType::iterator it = m_Pipeline.begin();
  for(size_t i = 0; i < index; ++i)
  {
    ++it;
  }
  m_Pipeline.insert(it, f);
  updatePrevNextFilters();
  Q_EMIT pipelineWasEdited();
  return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::erase(size_t index)
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // Do not allow anyone to remove filters from a pipeline that is not idle
    AbstractFilter::Pointer f = m_Pipeline[index];

    QString ss = QObject::tr("Filter '%1' could not be removed from pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    setErrorCondition(-207, ss);
    return false;
  }

  FilterContainerType::iterator it = m_Pipeline.begin();
  for(size_t i = 0; i < index; ++i)
  {
    ++it;
  }
  m_Pipeline.erase(it);
  updatePrevNextFilters();
  Q_EMIT pipelineWasEdited();
  return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::clear()
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // We cannot clear a pipeline that is not idle
    QString ss = QObject::tr("Pipeline '%1' could not be cleared because it is executing.").arg(getName());
    setErrorCondition(-208, ss);
    return false;
  }

  for(const auto& filter : m_Pipeline)
  {
    filter->setPreviousFilter(AbstractFilter::NullPointer());
    filter->setNextFilter(AbstractFilter::NullPointer());
  }
  m_Pipeline.clear();
  Q_EMIT pipelineWasEdited();
  return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t FilterPipeline::size()
{
  return m_Pipeline.size();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::empty()
{
  return m_Pipeline.isEmpty();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FilterPipeline::removeFirstFilterByName(const QString& name)
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // Do not allow anyone to remove filters from a pipeline that is not idle
    QString ss = QObject::tr("Filter '%1' could not be removed from pipeline '%2' because the pipeline is already executing.").arg(name).arg(getName());
    setErrorCondition(-209, ss);
    return AbstractFilter::NullPointer();
  }

  AbstractFilter::Pointer f = AbstractFilter::NullPointer();
  for(FilterContainerType::iterator it = m_Pipeline.begin(); it != m_Pipeline.end(); ++it)
  {
    if((*it)->getHumanLabel().compare(name) == 0)
    {
      f = *it;
      m_Pipeline.erase(it);
      break;
    }
  }
  updatePrevNextFilters();
  Q_EMIT pipelineWasEdited();

  return f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::FilterContainerType& FilterPipeline::getFilterContainer()
{
  return m_Pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::updatePrevNextFilters()
{
  FilterContainerType::iterator prev;
  FilterContainerType::iterator next;

  for(FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter)
  {
    (*iter)->setPreviousFilter(AbstractFilter::NullPointer());
    (*iter)->setNextFilter(AbstractFilter::NullPointer());
    if(iter != m_Pipeline.begin())
    {
      prev = iter;
      prev--;
      // prevFilt = *prev;
      (*iter)->setPreviousFilter(*prev);
    }
    else
    {
      (*iter)->setPreviousFilter(AbstractFilter::NullPointer());
    }

    if(iter != m_Pipeline.end())
    {
      next = iter;
      next++;
      //  nextFilt = *next;
      if(next != m_Pipeline.end())
      {
        (*iter)->setNextFilter(*next);
      }
    }
    else
    {
      (*iter)->setNextFilter(AbstractFilter::NullPointer());
    }
  }
  int index = 0;
  for(const auto& filter : m_Pipeline)
  {
    filter->setPipelineIndex(index++);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::setErrorCondition(int code, const QString& messageText)
{
  m_ErrorCode = code;
  PipelineErrorMessage::Pointer pm = PipelineErrorMessage::New(getName(), messageText, code);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::notifyStatusMessage(const QString& messageText) const
{
  PipelineStatusMessage::Pointer pm = PipelineStatusMessage::New(getName(), messageText);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::setWarningCondition(int code, const QString& messageText)
{
  m_WarningCode = code;
  PipelineWarningMessage::Pointer pm = PipelineWarningMessage::New(getName(), messageText, code);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::notifyProgressMessage(int progress, const QString& messageText) const
{
  PipelineProgressMessage::Pointer pm = PipelineProgressMessage::New(getName(), messageText, progress);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::clearErrorCode()
{
  m_ErrorCode = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::clearWarningCode()
{
  m_WarningCode = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::addMessageReceiver(QObject* obj)
{
  connect(this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), obj, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
  m_MessageReceivers.push_back(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::removeMessageReceiver(QObject* obj)
{
  disconnect(this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), obj, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
  m_MessageReceivers.removeAll(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::addObserver(Observer* obj)
{
  connect(this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), obj, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
  m_MessageReceivers.push_back(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::removeObserver(Observer* obj)
{
  disconnect(this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), obj, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
  m_MessageReceivers.removeAll(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::connectFilterNotifications(AbstractFilter* filter)
{
  for(const auto& messageReceiver : m_MessageReceivers)
  {
    connect(filter, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), messageReceiver, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
  }

  connect(filter, &AbstractFilter::messageGenerated, [=](AbstractMessage::Pointer msg) {
    FilterPipelineMessageHandler msgHandler(this);
    msg->visit(&msgHandler);
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::disconnectFilterNotifications(AbstractFilter* filter)
{
  for(const auto& messageReceiver : m_MessageReceivers)
  {
    disconnect(filter, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), messageReceiver, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
  }

  disconnect(filter, &AbstractFilter::messageGenerated, nullptr, nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FilterPipeline::preflightPipeline()
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // We cannot preflight a pipeline that is not idle
    QString ss = QObject::tr("Pipeline '%1' could not be preflighted because it is already executing.").arg(getName());
    int err = -203;
    setErrorCondition(err, ss);
    return err;
  }

  // Create the DataContainer object
  DataContainerArray::Pointer dca = DataContainerArray::New();

  clearErrorCode();
  int preflightError = 0;

  DataArrayPath::RenameContainer renamedPaths;

  // Start looping through each filter in the Pipeline and preflight everything
  for(const auto& filter : m_Pipeline)
  {
    // Do not preflight disabled filters
    if(filter->getEnabled())
    {
      filter->setDataContainerArray(dca->deepCopy(true));
#if RENAME_ENABLED
      // Avoid renaming filters as soon as they are added to the pipeline
      if(filter->property("HasRenameValues").toBool())
      {
        filter->renameDataArrayPaths(renamedPaths);
        RenameDataPath::CalculateRenamedPaths(filter, renamedPaths);
      }
      else
      {
        filter->setProperty("HasRenameValues", true);
      }
#endif
      filter->setDataContainerArray(dca);
      setCurrentFilter(filter);
      connectFilterNotifications(filter.get());
      filter->clearRenamedPaths();
      filter->preflight();
      disconnectFilterNotifications(filter.get());

      filter->setCancel(false); // Reset the cancel flag
      preflightError |= filter->getErrorCode();
      filter->setDataContainerArray(dca->deepCopy(false));
#if RENAME_ENABLED
      // Check if an existing renamed path was deleted by this filter
      const std::list<DataArrayPath> deletedPaths = filter->getDeletedPaths();
      for(const DataArrayPath& deletedPath : deletedPaths)
      {
        for(const DataArrayPath::RenameType& rename : renamedPaths)
        {
          const DataArrayPath& originalPath = rename.first;
          // const DataArrayPath& renamePath = std::get<1>(rename);
          if(originalPath == deletedPath)
          {
            const auto iter = std::find(renamedPaths.begin(), renamedPaths.end(), rename);
            renamedPaths.erase(iter);
            break;
          }
        }
      }
      // Filter renamed existing DataArrayPaths
      const DataArrayPath::RenameContainer newRenamePaths = filter->getRenamedPaths();
      for(const DataArrayPath::RenameType& newRename : newRenamePaths)
      {
        // Loop through all existing rename paths and update as appropriate
        for(auto iter = renamedPaths.cbegin(); iter != renamedPaths.cend(); ++iter)
        {
          const auto& existingRename = (*iter);
          const auto updatedRenameOpt = DataArrayPath::CreateLinkingRename(existingRename, newRename);
          if(true == updatedRenameOpt.first)
          {
            // Remove the old rename, insert the updated one, and update the iterator
            renamedPaths.insert(iter, updatedRenameOpt.second);
          }
        }
        // Add the new rename path
        renamedPaths.push_back(newRename);
      }
#endif
    }
#if RENAME_ENABLED
    else
    {
      // Some widgets require the updated path to be valid before it can be set in the widget
      filter->setDataContainerArray(dca->deepCopy(false));
      filter->renameDataArrayPaths(renamedPaths);

      // Undo filter renaming
      const DataArrayPath::RenameContainer filterRenamedPaths = filter->getRenamedPaths();
      for(const DataArrayPath::RenameType& renameType : filterRenamedPaths)
      {
        const DataArrayPath& oldPath = renameType.first;
        const DataArrayPath& newPath = renameType.second;

        renamedPaths.push_back(std::make_pair(newPath, oldPath));
      }
    }
#endif
  }
  setCurrentFilter(AbstractFilter::NullPointer());

  return preflightError;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer FilterPipeline::execute()
{
  return execute(DataContainerArray::New());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer FilterPipeline::execute(DataContainerArray::Pointer dca)
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // We cannot execute a pipeline that is not idle
    QString ss = QObject::tr("Pipeline '%1' could not be executed because it is already executing.").arg(getName());
    setErrorCondition(-200, ss);

    disconnectSignalsSlots();
    return DataContainerArray::NullPointer();
  }

  int err = 0;

  connectSignalsSlots();

  m_ExecutionResult = FilterPipeline::ExecutionResult::Invalid;

  m_State = FilterPipeline::State::Executing;

  m_Dca = dca;

  QDateTime now = QDateTime::currentDateTime();
  QString msg;
  QTextStream out(&msg);
  out << "Pipline Start: " << now.toString(Qt::ISODate);
  notifyStatusMessage(msg);
  // Start looping through the Pipeline
  for(const auto& filt : m_Pipeline)
  {
    int filtIndex = filt->getPipelineIndex();
    QString ss = QObject::tr("[%1/%2] %3").arg(filtIndex + 1).arg(m_Pipeline.size()).arg(filt->getHumanLabel());
    notifyStatusMessage(ss);

    Q_EMIT filt->filterInProgress(filt.get());

    // Do not execute disabled filters
    if(filt->getEnabled())
    {
      //      filt->setMessagePrefix(ss);
      connectFilterNotifications(filt.get());
      filt->setDataContainerArray(m_Dca);
      setCurrentFilter(filt);
      filt->execute();
      disconnectFilterNotifications(filt.get());
      filt->setDataContainerArray(DataContainerArray::NullPointer());
      err = filt->getErrorCode();
      if(err < 0)
      {
        ss = QObject::tr("[%1/%2] %3 caused an error during execution.").arg(filtIndex + 1).arg(m_Pipeline.size()).arg(filt->getHumanLabel());
        setErrorCondition(err, ss);

        notifyProgressMessage(100, "");

        Q_EMIT filt->filterCompleted(filt.get());
        Q_EMIT pipelineFinished();
        disconnectSignalsSlots();
        m_State = FilterPipeline::State::Idle;
        m_ExecutionResult = FilterPipeline::ExecutionResult::Failed;
        return m_Dca;
      }
    }

    if(m_State == FilterPipeline::State::Canceling)
    {
      // Clear cancel filter state
      filt->setCancel(false);
      break;
    }

    // Emit that the filter is completed for those objects that care, even the disabled ones.
    Q_EMIT filt->filterCompleted(filt.get());

    notifyProgressMessage(static_cast<int>(static_cast<float>(filtIndex + 1) / (m_Pipeline.size()) * 100.0f), "");
  }
  now = QDateTime::currentDateTime();
  msg.clear();
  out << "Pipline End: " << now.toString(Qt::ISODate);
  notifyStatusMessage(msg);

  disconnectSignalsSlots();

  switch(m_State)
  {
  case FilterPipeline::State::Canceling:
    m_ExecutionResult = FilterPipeline::ExecutionResult::Canceled;
    notifyStatusMessage("Pipeline Canceled");
    break;
  case FilterPipeline::State::Executing:
    m_ExecutionResult = FilterPipeline::ExecutionResult::Completed;
    notifyStatusMessage("Pipeline Complete");
    break;
  case FilterPipeline::State::Idle:
    throw PipelineIdleException();
    break;
  }

  m_State = FilterPipeline::State::Idle;

  Q_EMIT pipelineFinished();

  return m_Dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::printFilterNames(QTextStream& out)
{
  out << "---------------------------------------------------------------------";
  for(const auto& filter : m_Pipeline)
  {
    out << filter->getNameOfClass() << "\n";
  }
  out << "---------------------------------------------------------------------";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::connectSignalsSlots()
{
  for(const auto& filter : m_Pipeline)
  {
    connect(this, SIGNAL(pipelineFinished()), filter.get(), SLOT(cleanupFilter()));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::disconnectSignalsSlots()
{
  for(const auto& filter : m_Pipeline)
  {
    disconnect(this, SIGNAL(pipelineFinished()), filter.get(), SLOT(cleanupFilter()));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::isExecuting() const
{
  return (m_State == FilterPipeline::State::Executing);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::isCanceling() const
{
  return (m_State == FilterPipeline::State::Canceling);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::isIdle() const
{
  return (m_State == FilterPipeline::State::Idle);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer FilterPipeline::getDataContainerArray()
{
  return m_Dca;
}

// -----------------------------------------------------------------------------
FilterPipeline::Pointer FilterPipeline::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
QString FilterPipeline::getNameOfClass() const
{
  return QString("FilterPipeline");
}

// -----------------------------------------------------------------------------
QString FilterPipeline::ClassName()
{
  return QString("FilterPipeline");
}

// -----------------------------------------------------------------------------
FilterPipeline::Pointer FilterPipeline::New()
{
  Pointer sharedPtr(new(FilterPipeline));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
void FilterPipeline::setCurrentFilter(const AbstractFilter::Pointer& value)
{
  m_CurrentFilter = value;
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer FilterPipeline::getCurrentFilter() const
{
  return m_CurrentFilter;
}

// -----------------------------------------------------------------------------
FilterPipeline::ExecutionResult FilterPipeline::getExecutionResult() const
{
  return m_ExecutionResult;
}

// -----------------------------------------------------------------------------
FilterPipeline::State FilterPipeline::getState() const
{
  return m_State;
}

// -----------------------------------------------------------------------------
int FilterPipeline::getErrorCode() const
{
  return m_ErrorCode;
}

// -----------------------------------------------------------------------------
int FilterPipeline::getWarningCode() const
{
  return m_WarningCode;
}
