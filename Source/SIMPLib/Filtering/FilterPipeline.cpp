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

#include "SIMPLib/CoreFilters/EmptyFilter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"

#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Utilities/StringOperations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::FilterPipeline()
: m_ErrorCondition(0)
, m_PipelineName("")
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
QString FilterPipeline::getName()
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
      AbstractFilter::Pointer badFilter = AbstractFilter::New();

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
  filtManager->addFilterFactory("EmptyFilter", emptyFilterFactory);

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
        PipelineMessage pm(filterName, ss, -66066, PipelineMessage::MessageType::Error);
        pm.setPrefix("JsonFilterParametersReader::ReadPipelineFromFile()");
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
    setErrorCondition(-201);

    PipelineMessage progValue;
    progValue.setType(PipelineMessage::MessageType::Error);
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
    progValue.setText(ss);
    progValue.setCode(getErrorCondition());
    Q_EMIT pipelineGeneratedMessage(progValue);
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
    setErrorCondition(-202);

    PipelineMessage progValue;
    progValue.setType(PipelineMessage::MessageType::Error);
    QString ss = QObject::tr("Filter '%1' could not be added to pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    progValue.setText(ss);
    progValue.setCode(getErrorCondition());
    Q_EMIT pipelineGeneratedMessage(progValue);
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
    setErrorCondition(-203);

    AbstractFilter::Pointer f = m_Pipeline.front();

    PipelineMessage progValue;
    progValue.setType(PipelineMessage::MessageType::Error);
    QString ss = QObject::tr("Filter '%1' could not be removed from pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    progValue.setText(ss);
    progValue.setCode(getErrorCondition());
    Q_EMIT pipelineGeneratedMessage(progValue);
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
    setErrorCondition(-204);

    PipelineMessage progValue;
    progValue.setType(PipelineMessage::MessageType::Error);
    QString ss = QObject::tr("Filter '%1' could not be added to pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    progValue.setText(ss);
    progValue.setCode(getErrorCondition());
    Q_EMIT pipelineGeneratedMessage(progValue);
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
    setErrorCondition(-205);

    AbstractFilter::Pointer f = m_Pipeline.back();

    PipelineMessage progValue;
    progValue.setType(PipelineMessage::MessageType::Error);
    QString ss = QObject::tr("Filter '%1' could not be removed from pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    progValue.setText(ss);
    progValue.setCode(getErrorCondition());
    Q_EMIT pipelineGeneratedMessage(progValue);
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
    setErrorCondition(-206);

    PipelineMessage progValue;
    progValue.setType(PipelineMessage::MessageType::Error);
    QString ss = QObject::tr("Filter '%1' could not be added to pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    progValue.setText(ss);
    progValue.setCode(getErrorCondition());
    Q_EMIT pipelineGeneratedMessage(progValue);
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
    setErrorCondition(-207);

    AbstractFilter::Pointer f = m_Pipeline[index];

    PipelineMessage progValue;
    progValue.setType(PipelineMessage::MessageType::Error);
    QString ss = QObject::tr("Filter '%1' could not be removed from pipeline '%2' because the pipeline is already executing.").arg(f->getHumanLabel()).arg(getName());
    progValue.setText(ss);
    progValue.setCode(getErrorCondition());
    Q_EMIT pipelineGeneratedMessage(progValue);
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
    setErrorCondition(-208);

    PipelineMessage progValue;
    progValue.setType(PipelineMessage::MessageType::Error);
    QString ss = QObject::tr("Pipeline '%1' could not be cleared because it is executing.").arg(getName());
    progValue.setText(ss);
    progValue.setCode(getErrorCondition());
    Q_EMIT pipelineGeneratedMessage(progValue);
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
    setErrorCondition(-209);

    PipelineMessage progValue;
    progValue.setType(PipelineMessage::MessageType::Error);
    QString ss = QObject::tr("Filter '%1' could not be removed from pipeline '%2' because the pipeline is already executing.").arg(name).arg(getName());
    progValue.setText(ss);
    progValue.setCode(getErrorCondition());
    Q_EMIT pipelineGeneratedMessage(progValue);
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
void FilterPipeline::addMessageReceiver(QObject* obj)
{
  connect(this, SIGNAL(pipelineGeneratedMessage(const PipelineMessage&)), obj, SLOT(processPipelineMessage(const PipelineMessage&)));
  m_MessageReceivers.push_back(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::removeMessageReceiver(QObject* obj)
{
  disconnect(this, SIGNAL(pipelineGeneratedMessage(const PipelineMessage&)), obj, SLOT(processPipelineMessage(const PipelineMessage&)));
  m_MessageReceivers.removeAll(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::connectFilterNotifications(QObject* filter)
{
  for(const auto& messageReceiver : m_MessageReceivers)
  {
    connect(filter, SIGNAL(filterGeneratedMessage(const PipelineMessage&)), messageReceiver, SLOT(processPipelineMessage(const PipelineMessage&)));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::disconnectFilterNotifications(QObject* filter)
{
  for(const auto& messageReceiver : m_MessageReceivers)
  {
    disconnect(filter, SIGNAL(filterGeneratedMessage(const PipelineMessage&)), messageReceiver, SLOT(processPipelineMessage(const PipelineMessage&)));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FilterPipeline::preflightPipeline()
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // We cannot preflight a pipeline that is not idle
    setErrorCondition(-203);

    PipelineMessage progValue;
    progValue.setType(PipelineMessage::MessageType::Error);
    QString ss = QObject::tr("Pipeline '%1' could not be preflighted because it is already executing.").arg(getName());
    progValue.setText(ss);
    progValue.setCode(getErrorCondition());
    Q_EMIT pipelineGeneratedMessage(progValue);
    return getErrorCondition();
  }

  // Create the DataContainer object
  DataContainerArray::Pointer dca = DataContainerArray::New();

  setErrorCondition(0);
  int preflightError = 0;

  DataArrayPath::RenameContainer renamedPaths;
  DataArrayPath::RenameContainer filterRenamedPaths;

  // Start looping through each filter in the Pipeline and preflight everything
  // for(FilterContainerType::iterator filter = m_Pipeline.begin(); filter != m_Pipeline.end(); ++filter)
  for(const auto& filter : m_Pipeline)
  {
    // Do not preflight disabled filters
    if(filter->getEnabled())
    {
      // Update renamed paths before getting old created paths
      DataContainerArray::Pointer oldDca = filter->getDataContainerArray();
      oldDca->renameDataArrayPaths(filterRenamedPaths);
      filter->setDataContainerArray(oldDca);
      filter->renameDataArrayPaths(filterRenamedPaths);

      std::list<DataArrayPath> oldCreatedPaths = filter->getCreatedPaths();

      filter->setDataContainerArray(dca);
      filter->renameDataArrayPaths(renamedPaths);
      setCurrentFilter(filter);
      connectFilterNotifications(filter.get());
      filter->preflight();
      disconnectFilterNotifications(filter.get());

      filter->setCancel(false); // Reset the cancel flag
      preflightError |= filter->getErrorCondition();
      filter->setDataContainerArray(dca->deepCopy(false));
      std::list<DataArrayPath> currentCreatedPaths = filter->getCreatedPaths();

      // Check if an existing renamed path was created by this filter
      for(const DataArrayPath& createdPath : currentCreatedPaths)
      {
        // Filter Parameter changes
        for(const DataArrayPath::RenameType& rename : renamedPaths)
        {
          DataArrayPath originalPath;
          DataArrayPath renamePath;
          std::tie(originalPath, renamePath) = rename;
          if(originalPath == createdPath)
          {
            renamedPaths.remove(rename);
            break;
          }
        }
        // Rename Filters
        for(const DataArrayPath::RenameType& rename : filterRenamedPaths)
        {
          DataArrayPath originalPath;
          DataArrayPath renamePath;
          std::tie(originalPath, renamePath) = rename;
          if(originalPath == createdPath)
          {
            filterRenamedPaths.remove(rename);
            break;
          }
        }
      }

      DataArrayPath::RenameContainer newRenamedPaths = DataArrayPath::CheckForRenamedPaths(oldDca, dca, oldCreatedPaths, currentCreatedPaths);
      for(const DataArrayPath::RenameType& renameType : newRenamedPaths)
      {
        renamedPaths.push_back(renameType);
      }

      // Filter renamed existing DataArrayPaths
      DataArrayPath::RenameContainer hardRenamePaths = filter->getRenamedPaths();
      for(const DataArrayPath::RenameType& renameType : hardRenamePaths)
      {
        renamedPaths.push_back(renameType);
        filterRenamedPaths.push_back(renameType);
      }
    }
    else
    {
      // Some widgets require the updated path to be valid before it can be set in the widget
      filter->setDataContainerArray(dca->deepCopy(false));
      filter->renameDataArrayPaths(renamedPaths);

      // Undo filter renaming
      DataArrayPath::RenameContainer filterRenamedPaths = filter->getRenamedPaths();
      for(DataArrayPath::RenameType renameType : filterRenamedPaths)
      {
        DataArrayPath oldPath;
        DataArrayPath newPath;
        std::tie(oldPath, newPath) = renameType;
        renameType = std::make_pair(newPath, oldPath);

        renamedPaths.push_back(renameType);
      }
    }
  }
  setCurrentFilter(AbstractFilter::NullPointer());

  return preflightError;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer FilterPipeline::execute()
{
  if(m_State != FilterPipeline::State::Idle)
  {
    // We cannot execute a pipeline that is not idle
    setErrorCondition(-200);

    PipelineMessage progValue;
    progValue.setType(PipelineMessage::MessageType::Error);
    QString ss = QObject::tr("Pipeline '%1' could not be executed because it is already executing.").arg(getName());
    progValue.setText(ss);
    progValue.setCode(getErrorCondition());
    Q_EMIT pipelineGeneratedMessage(progValue);

    disconnectSignalsSlots();
    return DataContainerArray::NullPointer();
  }

  int err = 0;

  connectSignalsSlots();

  m_ExecutionResult = FilterPipeline::ExecutionResult::Invalid;

  m_State = FilterPipeline::State::Executing;

  m_Dca = DataContainerArray::New();

  // Start looping through the Pipeline
  float progress = 0.0f;

  PipelineMessage progValue("", "", 0, PipelineMessage::MessageType::ProgressValue, -1);
  for(const auto& filt : m_Pipeline)
  {
    progress = progress + 1.0f;
    progValue.setType(PipelineMessage::MessageType::ProgressValue);
    progValue.setProgressValue(static_cast<int>(progress / (m_Pipeline.size() + 1) * 100.0f));
    Q_EMIT pipelineGeneratedMessage(progValue);

    QString ss = QObject::tr("[%1/%2] %3 ").arg(progress).arg(m_Pipeline.size()).arg(filt->getHumanLabel());

    progValue.setType(PipelineMessage::MessageType::StatusMessage);
    progValue.setText(ss);
    Q_EMIT pipelineGeneratedMessage(progValue);
    Q_EMIT filt->filterInProgress(filt.get());

    // Do not execute disabled filters
    if(filt->getEnabled())
    {
      filt->setMessagePrefix(ss);
      connectFilterNotifications(filt.get());
      filt->setDataContainerArray(m_Dca);
      setCurrentFilter(filt);
      filt->execute();
      disconnectFilterNotifications(filt.get());
      filt->setDataContainerArray(DataContainerArray::NullPointer());
      err = filt->getErrorCondition();
      if(err < 0)
      {
        setErrorCondition(err);
        progValue.setFilterClassName(filt->getNameOfClass());
        progValue.setFilterHumanLabel(filt->getHumanLabel());
        progValue.setType(PipelineMessage::MessageType::Error);
        progValue.setProgressValue(100);
        ss = QObject::tr("[%1/%2] %3 caused an error during execution.").arg(progress).arg(m_Pipeline.size()).arg(filt->getHumanLabel());
        progValue.setText(ss);
        progValue.setPipelineIndex(filt->getPipelineIndex());
        progValue.setCode(filt->getErrorCondition());
        Q_EMIT pipelineGeneratedMessage(progValue);
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
  }

  disconnectSignalsSlots();

  PipelineMessage completeMessage("", "", 0, PipelineMessage::MessageType::StatusMessage, -1);
  if(m_State == FilterPipeline::State::Canceling)
  {
    completeMessage.setText("Pipeline Canceled");
    m_ExecutionResult = FilterPipeline::ExecutionResult::Canceled;
  }
  else if(m_State == FilterPipeline::State::Executing)
  {
    completeMessage.setText("Pipeline Complete");
    m_ExecutionResult = FilterPipeline::ExecutionResult::Completed;
  }
  else
  {
    // This should never get here
    completeMessage.setText("Unsupported Pipeline Execution Result");
    Q_EMIT pipelineGeneratedMessage(completeMessage);

    setErrorCondition(-210);
    completeMessage.setType(PipelineMessage::MessageType::Error);
    completeMessage.setCode(getErrorCondition());
  }

  Q_EMIT pipelineGeneratedMessage(completeMessage);

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
