/* ============================================================================
* Copyright (c) 2017 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software nor the names of its
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
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SIMPLController.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>
#include <QtCore/QThread>

#include <QtWidgets/QMessageBox>

#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"

#include "SVWidgetsLib/Widgets/PipelineModel.h"
#include "SVWidgetsLib/Widgets/PipelineItem.h"
#include "SVWidgetsLib/QtSupport/QtSRecentFileList.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLController::SIMPLController(QObject* parent)
: QObject(parent)
, m_PipelineSignalMapper(new QSignalMapper(this))
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLController::~SIMPLController()
{
  if(m_WorkerThread)
  {
    delete m_WorkerThread;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLController::preflightPipeline(const QModelIndex &pipelineIndex, PipelineModel* model)
{
  if(m_BlockPreflight || pipelineIndex.isValid() == false)
  {
    return;
  }

  emit clearIssuesTriggered();
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = model->getFilterPipeline(pipelineIndex);

  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
  for(int i = 0; i < filters.size(); i++)
  {
    filters.at(i)->setErrorCondition(0);
    filters.at(i)->setCancel(false);

    QModelIndex childIndex = model->index(i, PipelineItem::Name, pipelineIndex);
    if(childIndex.isValid())
    {
      model->setErrorState(childIndex, PipelineItem::ErrorState::Ok);
      if(PipelineItem::WidgetState::Disabled != model->widgetState(childIndex))
      {
        model->setWidgetState(childIndex, PipelineItem::WidgetState::Ready);
      }
    }
  }

//  QSharedPointer<ProgressDialog> progressDialog(new ProgressDialog());
//  progressDialog->setWindowTitle("Pipeline Preflighting");
//  QString msg = QString("Please wait for %1 filters to preflight...").arg(pipeline->getFilterContainer().count());
//  progressDialog->setLabelText(msg);
//  progressDialog->show();
//  progressDialog->raise();
//  progressDialog->activateWindow();

  // Preflight the pipeline
  int err = pipeline->preflightPipeline();
  if(err < 0)
  {
    // FIXME: Implement error handling.
  }

  int count = pipeline->getFilterContainer().size();
  // Now that the preflight has been executed loop through the filters and check their error condition and set the
  // outline on the filter widget if there were errors or warnings
  for(qint32 i = 0; i < count; ++i)
  {
    QModelIndex childIndex = model->index(i, PipelineItem::Name, pipelineIndex);
    if(childIndex.isValid())
    {
      AbstractFilter::Pointer filter = model->filter(childIndex);
      if(filter->getWarningCondition() < 0)
      {
        model->setErrorState(childIndex, PipelineItem::ErrorState::Warning);
      }
      if(filter->getErrorCondition() < 0)
      {
        model->setErrorState(childIndex, PipelineItem::ErrorState::Error);
      }
    }
  }
  emit preflightFinished(pipelineIndex, err);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLController::runPipeline(const QModelIndex &pipelineIndex, PipelineModel* model)
{
  if(m_WorkerThread != nullptr)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    if(m_WorkerThread->isFinished() == true)
    {
      delete m_WorkerThread;
      m_WorkerThread = nullptr;
    }
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  connect(m_PipelineSignalMapper, SIGNAL(mapped(const QModelIndex &)), m_WorkerThread, SIGNAL(finished(const QModelIndex &)));

  // Clear out the Issues Table
  emit clearIssuesTriggered();

  // Create a FilterPipeline Object
//  m_PipelineInFlight = getCopyOfFilterPipeline();
  m_PipelineInFlight = model->getFilterPipeline(pipelineIndex);

  emit standardOutputMessageGenerated("<b>Preflight Pipeline.....</b>");
  // Give the pipeline one last chance to preflight and get all the latest values from the GUI
  int err = m_PipelineInFlight->preflightPipeline();
  if(err < 0)
  {
    m_PipelineInFlight = FilterPipeline::NullPointer();
    emit displayIssuesTriggered();
    return;
  }
  emit standardOutputMessageGenerated("    Preflight Results: 0 Errors");

  // Save each of the DataContainerArrays from each of the filters for when the pipeline is complete
  m_PreflightDataContainerArrays.clear();
  FilterPipeline::FilterContainerType filters = m_PipelineInFlight->getFilterContainer();
  for(FilterPipeline::FilterContainerType::size_type i = 0; i < filters.size(); i++)
  {
    m_PreflightDataContainerArrays.push_back(filters[i]->getDataContainerArray()->deepCopy(true));
  }

  // Save the preferences file NOW in case something happens
  emit writeSIMPLViewSettingsTriggered();

  emit pipelineReady(pipelineIndex);

//  // Block FilterListToolboxWidget signals, so that we can't add filters to the view while running the pipeline
//  getFilterListToolboxWidget()->blockSignals(true);

//  // Block FilterLibraryToolboxWidget signals, so that we can't add filters to the view while running the pipeline
//  getFilterLibraryToolboxWidget()->blockSignals(true);

  // Move the FilterPipeline object into the thread that we just created.
  m_PipelineInFlight->moveToThread(m_WorkerThread);

  // Allow the GUI to receive messages - We are only interested in the progress messages
  m_PipelineInFlight->addMessageReceiver(this);

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * PipelineBuilder object. Since the PipelineBuilder object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the PipelineBuilder going
  connect(m_WorkerThread, SIGNAL(started()), m_PipelineInFlight.get(), SLOT(run()));

  // When the PipelineBuilder ends then tell the QThread to stop its event loop
  connect(m_PipelineInFlight.get(), SIGNAL(pipelineFinished()), m_WorkerThread, SLOT(quit()));

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()), this, SLOT(finishPipeline()));

  emit pipelineStarted(pipelineIndex);
  m_CurrentlyRunningPipelines.push_back(pipelineIndex);
  m_WorkerThread->start();
  standardOutputMessageGenerated("");
  standardOutputMessageGenerated("<b>*************** PIPELINE STARTED ***************</b>");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLController::cancelPipeline(const QModelIndex &pipelineIndex)
{
  m_PipelineInFlight->cancelPipeline();
  m_CurrentlyRunningPipelines.removeAll(pipelineIndex);
  emit displayIssuesTriggered();
  emit pipelineFinished(pipelineIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLController::finishPipeline(const QModelIndex &pipelineIndex)
{
  if(m_PipelineInFlight->getCancel() == true)
  {
    standardOutputMessageGenerated("<b>*************** PIPELINE CANCELED ***************</b>");
  }
  else
  {
    standardOutputMessageGenerated("<b>*************** PIPELINE FINISHED ***************</b>");
  }
  standardOutputMessageGenerated("");

  // Put back the DataContainerArray for each filter at the conclusion of running
  // the pipeline. this keeps the data browser current and up to date.
  FilterPipeline::FilterContainerType filters = m_PipelineInFlight->getFilterContainer();
  for(FilterPipeline::FilterContainerType::size_type i = 0; i < filters.size(); i++)
  {
    filters[i]->setDataContainerArray(m_PreflightDataContainerArrays[i]);
  }

  m_PipelineInFlight = FilterPipeline::NullPointer(); // This _should_ remove all the filters and deallocate them

  m_CurrentlyRunningPipelines.removeAll(pipelineIndex);

  emit displayIssuesTriggered();
  emit pipelineFinished(pipelineIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLController::processPipelineMessage(const PipelineMessage& msg)
{
  emit pipelineMessageGenerated(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLController::blockPreflightSignals(bool b)
{
  if(b)
  {
    m_BlockPreflightStack.push(b);
  }
  else
  {
    m_BlockPreflightStack.pop();
  }

  m_BlockPreflight = (m_BlockPreflightStack.size() > 0) ? true : false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject SIMPLController::toJsonObject(PipelineModel* model)
{
  PipelineItem* rootItem = model->getRootItem();

  QJsonObject treeObj;
  for(int i = 0; i < rootItem->childCount(); i++)
  {
    QModelIndex childIndex = model->index(i, PipelineItem::Name, QModelIndex());
    QString name = childIndex.data().toString();

    if(name.compare("Prebuilt Pipelines") != 0)
    {
      QJsonObject childObj = wrapModel(childIndex, model);
      treeObj[name] = childObj;
    }
  }

  return treeObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject SIMPLController::wrapModel(QModelIndex currentIndex, PipelineModel* model)
{
  QJsonObject obj;

  QString name = model->index(currentIndex.row(), PipelineItem::Name, currentIndex.parent()).data().toString();

  for(int i = 0; i < model->rowCount(currentIndex); i++)
  {
    QModelIndex childIndex = model->index(i, PipelineItem::Name, currentIndex);
    QString childName = childIndex.data().toString();

    QJsonObject childObj = wrapModel(childIndex, model);
    obj[childName] = childObj;
  }

  if(model->flags(currentIndex).testFlag(Qt::ItemIsDropEnabled) == true)
  {
    obj.insert("Expanded", model->needsToBeExpanded(currentIndex));
  }

  return obj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineModel* SIMPLController::fromJsonObject(QJsonObject treeObject, PipelineModel* model)
{
  QStringList keys = treeObject.keys();
  keys.sort(Qt::CaseInsensitive);
  for(int i = 0; i < keys.size(); i++)
  {
    if(keys[i].compare("Prebuilt Pipelines") == 0)
    {
      continue;
    }
    QJsonValue val = treeObject.value(keys[i]);
    if(val.isObject())
    {
      unwrapModel(keys[i], val.toObject(), model, QModelIndex());
    }
  }

  return model;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLController::unwrapModel(QString objectName, QJsonObject object, PipelineModel* model, QModelIndex parentIndex)
{
  int row = model->rowCount(parentIndex);
  model->insertRow(row, parentIndex);
  QModelIndex nameIndex = model->index(row, PipelineItem::Name, parentIndex);

  QString path = object["Path"].toString();
  bool expanded = object["Expanded"].toBool();

  QFileInfo fi(path);
  if(path.isEmpty() == false)
  {
    model->setData(nameIndex, QIcon(":/bookmark.png"), Qt::DecorationRole);
    if(fi.exists() == false)
    {
      // Set the itemHasError variable
      model->setData(nameIndex, true, Qt::UserRole);
    }
  }
  else
  {
    model->setData(nameIndex, QIcon(":/folder_blue.png"), Qt::DecorationRole);
  }

  path = QDir::toNativeSeparators(path);

  model->setData(nameIndex, objectName, Qt::DisplayRole);
  model->setNeedsToBeExpanded(nameIndex, expanded);

  QStringList keys = object.keys();
  keys.sort(Qt::CaseInsensitive);
  for(int i = 0; i < keys.size(); i++)
  {
    QJsonValue val = object.value(keys[i]);
    if(val.isObject())
    {
      unwrapModel(keys[i], val.toObject(), model, nameIndex);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLController::isPipelineCurrentlyRunning(const QModelIndex &pipelineIndex)
{
  return m_CurrentlyRunningPipelines.contains(pipelineIndex);
}


