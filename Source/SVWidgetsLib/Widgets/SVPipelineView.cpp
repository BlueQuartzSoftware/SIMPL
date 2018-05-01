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

#include "SVPipelineView.h"

#include <iostream>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMimeData>
#include <QtCore/QTemporaryFile>
#include <QtCore/QUrl>
#include <QtCore/QSharedPointer>
#include <QtCore/QThread>
#include <QtCore/QSignalMapper>

#include <QtGui/QClipboard>
#include <QtGui/QDrag>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QVBoxLayout>

#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/DocRequestManager.h"
#include "SIMPLib/CoreFilters/Breakpoint.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/SIMPLib.h"

#include "SVWidgetsLib/QtSupport/QtSDroppableScrollArea.h"

#include "SVWidgetsLib/Core/FilterWidgetManager.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"
#include "SVWidgetsLib/Widgets/BreakpointFilterWidget.h"
#include "SVWidgetsLib/Widgets/PipelineModel.h"
#include "SVWidgetsLib/Widgets/PipelineFilterMimeData.h"
#include "SVWidgetsLib/Widgets/util/AddFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/MoveFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/RemoveFilterCommand.h"
#include "SVWidgetsLib/Widgets/DataStructureWidget.h"
#include "SVWidgetsLib/Widgets/ProgressDialog.h"
#include "SVWidgetsLib/QtSupport/QtSStyles.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineView::SVPipelineView(QWidget* parent)
: QListView(parent)
, PipelineView()
, m_PipelineIsRunning(false)
, m_BlockPreflight(false)
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineView::~SVPipelineView()
{
  if(m_WorkerThread)
  {
    delete m_WorkerThread;
  }
  if(m_FilterOutlineWidget && !m_FilterOutlineWidget->parent())
  {
    delete m_FilterOutlineWidget;
  }
  // Delete action if it exists
  if(m_ActionEnableFilter)
  {
    delete m_ActionEnableFilter;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::setupGui()
{
  // Delete action if it exists
  if(m_ActionEnableFilter)
  {
    delete m_ActionEnableFilter;
  }

  m_ActionEnableFilter = new QAction("Enable", this);
  m_ActionEnableFilter->setCheckable(true);
  m_ActionEnableFilter->setChecked(true);
  m_ActionEnableFilter->setEnabled(false);

  m_ActionCut = new QAction("Cut", this);
  m_ActionCopy = new QAction("Copy", this);
  m_ActionPaste = new QAction("Paste", this);

  m_ActionCut->setShortcut(QKeySequence::Cut);
  m_ActionCopy->setShortcut(QKeySequence::Copy);
  m_ActionPaste->setShortcut(QKeySequence::Paste);
  m_ActionClearPipeline->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Backspace));

  m_ActionCut->setDisabled(true);
  m_ActionCopy->setDisabled(true);

  // Run this once, so that the Paste button availability is updated for what is currently on the system clipboard
  updatePasteAvailability();

  QClipboard* clipboard = QApplication::clipboard();
  connect(clipboard, SIGNAL(dataChanged()), this, SLOT(updatePasteAvailability()));

  m_FilterOutlineWidget = new SVPipelineFilterOutlineWidget(nullptr);
  m_FilterOutlineWidget->setObjectName("m_DropBox");

  setContextMenuPolicy(Qt::CustomContextMenu);
  setFocusPolicy(Qt::StrongFocus);

  connectSignalsSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::connectSignalsSlots()
{
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(requestContextMenu(const QPoint&)));

  connect(this, &SVPipelineView::deleteKeyPressed, this, &SVPipelineView::listenDeleteKeyTriggered);

  connect(m_ActionCut, &QAction::triggered, this, &SVPipelineView::listenCutTriggered);
  connect(m_ActionCopy, &QAction::triggered, this, &SVPipelineView::listenCopyTriggered);
  connect(m_ActionPaste, &QAction::triggered, this, &SVPipelineView::listenPasteTriggered);
  connect(m_ActionClearPipeline, &QAction::triggered, this, &SVPipelineView::listenClearPipelineTriggered);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::addPipelineMessageObserver(QObject* pipelineMessageObserver)
{
  m_PipelineMessageObservers.push_back(pipelineMessageObserver);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::addFilterFromClassName(const QString &filterClassName, int insertIndex)
{
  FilterManager* fm = FilterManager::Instance();
  if (fm != nullptr)
  {
    IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterClassName);
    if (factory.get() != nullptr)
    {
      AbstractFilter::Pointer filter = factory->create();
      addFilter(filter, insertIndex);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::addFilter(AbstractFilter::Pointer filter, int insertIndex)
{
  AddFilterCommand* cmd = new AddFilterCommand(filter, this, insertIndex, "Add");
  addUndoCommand(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::addFilters(std::vector<AbstractFilter::Pointer> filters, int insertIndex)
{
  AddFilterCommand* cmd = new AddFilterCommand(filters, this, insertIndex, "Add");
  addUndoCommand(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::moveFilter(AbstractFilter::Pointer filter, int insertIndex)
{
  MoveFilterCommand* cmd = new MoveFilterCommand(filter, this, insertIndex);
  addUndoCommand(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::moveFilters(std::vector<AbstractFilter::Pointer> filters, int insertIndex)
{
  MoveFilterCommand* cmd = new MoveFilterCommand(filters, this, insertIndex);
  addUndoCommand(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::removeFilter(AbstractFilter::Pointer filter)
{
  RemoveFilterCommand* cmd = new RemoveFilterCommand(filter, this, "Remove");
  addUndoCommand(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::removeFilters(std::vector<AbstractFilter::Pointer> filters)
{
  RemoveFilterCommand* cmd = new RemoveFilterCommand(filters, this, "Remove");
  addUndoCommand(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::cutFilter(AbstractFilter::Pointer filter)
{
  RemoveFilterCommand* cmd = new RemoveFilterCommand(filter, this, "Cut");
  addUndoCommand(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::cutFilters(std::vector<AbstractFilter::Pointer> filters)
{
  RemoveFilterCommand* cmd = new RemoveFilterCommand(filters, this, "Cut");
  addUndoCommand(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::pasteFilter(AbstractFilter::Pointer filter)
{
  AddFilterCommand* addCmd = new AddFilterCommand(filter, this, -1, "Paste");
  addUndoCommand(addCmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::pasteFilters(std::vector<AbstractFilter::Pointer> filters)
{
  AddFilterCommand* addCmd = new AddFilterCommand(filters, this, -1, "Paste");
  addUndoCommand(addCmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::preflightPipeline()
{
  if(m_BlockPreflight)
  {
    return;
  }

  emit clearIssuesTriggered();

  PipelineModel* model = getPipelineModel();

  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = getFilterPipeline();

  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
  for(int i = 0; i < filters.size(); i++)
  {
    filters.at(i)->setErrorCondition(0);
    filters.at(i)->setCancel(false);

    QModelIndex childIndex = model->index(i, PipelineItem::Contents);
    if(childIndex.isValid())
    {
      model->setErrorState(childIndex, PipelineItem::ErrorState::Ok);
      AbstractFilter::Pointer filter = model->filter(childIndex);
      if(filter->getEnabled() == true)
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
    QModelIndex childIndex = model->index(i, PipelineItem::Contents);
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

  emit preflightFinished(pipeline, err);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::executePipeline()
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

  // Clear out the Issues Table
  emit clearIssuesTriggered();

  // Create a FilterPipeline Object
//  m_PipelineInFlight = getCopyOfFilterPipeline();
  m_PipelineInFlight = getFilterPipeline();

  emit stdOutMessage("<b>Preflight Pipeline.....</b>");
  // Give the pipeline one last chance to preflight and get all the latest values from the GUI
  int err = m_PipelineInFlight->preflightPipeline();
  if(err < 0)
  {
    m_PipelineInFlight = FilterPipeline::NullPointer();
    emit displayIssuesTriggered();
    return;
  }
  emit stdOutMessage("    Preflight Results: 0 Errors");

  // Save each of the DataContainerArrays from each of the filters for when the pipeline is complete
  m_PreflightDataContainerArrays.clear();
  FilterPipeline::FilterContainerType filters = m_PipelineInFlight->getFilterContainer();
  for(FilterPipeline::FilterContainerType::size_type i = 0; i < filters.size(); i++)
  {
    m_PreflightDataContainerArrays.push_back(filters[i]->getDataContainerArray()->deepCopy(true));
  }

  // Save the preferences file NOW in case something happens
  emit writeSIMPLViewSettingsTriggered();

  toReadyState();

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

  toRunningState();
  m_PipelineRunning = true;
  m_WorkerThread->start();
  stdOutMessage("");
  stdOutMessage("<b>*************** PIPELINE STARTED ***************</b>");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::processPipelineMessage(const PipelineMessage& msg)
{
  emit pipelineHasMessage(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::cancelPipeline()
{
  m_PipelineInFlight->cancelPipeline();
  m_PipelineRunning = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::finishPipeline()
{
  if(m_PipelineInFlight->getCancel() == true)
  {
    stdOutMessage("<b>*************** PIPELINE CANCELED ***************</b>");
  }
  else
  {
    stdOutMessage("<b>*************** PIPELINE FINISHED ***************</b>");
  }
  stdOutMessage("");

  // Put back the DataContainerArray for each filter at the conclusion of running
  // the pipeline. this keeps the data browser current and up to date.
  FilterPipeline::FilterContainerType filters = m_PipelineInFlight->getFilterContainer();
  for(FilterPipeline::FilterContainerType::size_type i = 0; i < filters.size(); i++)
  {
    filters[i]->setDataContainerArray(m_PreflightDataContainerArrays[i]);
  }

  m_PipelineInFlight = FilterPipeline::NullPointer(); // This _should_ remove all the filters and deallocate them

  m_PipelineRunning = false;

  toStoppedState();

  emit displayIssuesTriggered();
  emit pipelineFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer SVPipelineView::getFilterPipeline()
{
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  PipelineModel* model = getPipelineModel();

  qint32 count = model->rowCount();
  for(qint32 i = 0; i < count; ++i)
  {
    QModelIndex childIndex = model->index(i, PipelineItem::Contents);
    PipelineItem::WidgetState widgetState = model->widgetState(childIndex);
    if(childIndex.isValid() && widgetState != PipelineItem::WidgetState::Disabled)
    {
      AbstractFilter::Pointer filter = model->filter(childIndex);
      Breakpoint::Pointer breakpoint = std::dynamic_pointer_cast<Breakpoint>(filter);
      if(nullptr != breakpoint)
      {
        connect(pipeline.get(), SIGNAL(pipelineCanceled()), breakpoint.get(), SLOT(resumePipeline()));
      }

      pipeline->pushBack(filter);
    }
  }
  for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
  {
    pipeline->addMessageReceiver(m_PipelineMessageObservers[i]);
  }
  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SVPipelineView::writePipeline(const QString &outputPath)
{
  QFileInfo fi(outputPath);
  QString ext = fi.completeSuffix();

  // If the filePath already exists - delete it so that we get a clean write to the file
  if(fi.exists() == true && (ext == "dream3d" || ext == "json"))
  {
    QFile f(outputPath);
    if(f.remove() == false)
    {
      QMessageBox::warning(nullptr, QString::fromLatin1("Pipeline Write Error"), QString::fromLatin1("There was an error removing the existing pipeline file. The pipeline was NOT saved."));
      return -1;
    }
  }

  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = getFilterPipeline();

  int err = 0;
  if(ext == "dream3d")
  {
    QList<IObserver*> observers;
    for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
    {
      observers.push_back(reinterpret_cast<IObserver*>(m_PipelineMessageObservers[i]));
    }

    H5FilterParametersWriter::Pointer dream3dWriter = H5FilterParametersWriter::New();
    err = dream3dWriter->writePipelineToFile(pipeline, fi.absoluteFilePath(), fi.fileName(), observers);
  }
  else if(ext == "json")
  {
    QList<IObserver*> observers;
    for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
    {
      observers.push_back(reinterpret_cast<IObserver*>(m_PipelineMessageObservers[i]));
    }

    JsonFilterParametersWriter::Pointer jsonWriter = JsonFilterParametersWriter::New();
    jsonWriter->writePipelineToFile(pipeline, fi.absoluteFilePath(), fi.fileName(), observers);
  }
  else
  {
    emit statusMessage(tr("The pipeline was not written to file '%1'. '%2' is an unsupported file extension.").arg(fi.fileName()).arg(ext));
    return -1;
  }

  if(err < 0)
  {
    emit statusMessage(tr("There was an error while saving the pipeline to file '%1'.").arg(fi.fileName()));
    return -1;
  }
  else
  {
    emit statusMessage(tr("The pipeline has been saved successfully to '%1'.").arg(fi.fileName()));
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------s
void SVPipelineView::updatePasteAvailability()
{
  QClipboard* clipboard = QApplication::clipboard();
  QString text = clipboard->text();

  JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
  FilterPipeline::Pointer pipeline = jsonReader->readPipelineFromString(text);

  if(text.isEmpty() || FilterPipeline::NullPointer() == pipeline)
  {
    m_ActionPaste->setDisabled(true);
  }
  else
  {
    m_ActionPaste->setEnabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::listenCutTriggered()
{
  copySelectedFilters();

  std::vector<AbstractFilter::Pointer> filters = getSelectedFilters();
  cutFilters(filters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::listenCopyTriggered()
{
  copySelectedFilters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::copySelectedFilters()
{
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  std::vector<AbstractFilter::Pointer> filters = getSelectedFilters();
  for(int i = 0; i < filters.size(); i++)
  {
    pipeline->pushBack(filters[i]);
  }

  JsonFilterParametersWriter::Pointer jsonWriter = JsonFilterParametersWriter::New();
  QString jsonString = jsonWriter->writePipelineToString(pipeline, "Pipeline");

  QClipboard* clipboard = QApplication::clipboard();
  clipboard->setText(jsonString);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<AbstractFilter::Pointer> SVPipelineView::getSelectedFilters()
{
  QModelIndexList selectedIndexes = selectionModel()->selectedRows();

  std::vector<AbstractFilter::Pointer> filters;
  PipelineModel* model = getPipelineModel();
  for(int i = 0; i < selectedIndexes.size(); i++)
  {
    AbstractFilter::Pointer filter = model->filter(selectedIndexes[i]);
    filters.push_back(filter);
  }

  return filters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::listenPasteTriggered()
{  
  QClipboard* clipboard = QApplication::clipboard();
  QString jsonString = clipboard->text();

  JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
  FilterPipeline::Pointer pipeline = jsonReader->readPipelineFromString(jsonString);
  FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();

  std::vector<AbstractFilter::Pointer> filters;
  for (int i = 0; i < container.size(); i++)
  {
    filters.push_back(container[i]);
  }

  pasteFilters(filters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::listenDeleteKeyTriggered()
{
  QModelIndexList selectedIndexes = selectionModel()->selectedRows();
  if(selectedIndexes.size() <= 0)
  {
    return;
  }

  PipelineModel* model = getPipelineModel();

  std::vector<AbstractFilter::Pointer> filters;
  for (int i = 0; i < selectedIndexes.size(); i++)
  {
    AbstractFilter::Pointer filter = model->filter(selectedIndexes[i]);
    filters.push_back(filter);
  }

  removeFilters(filters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::listenClearPipelineTriggered()
{  
  clearPipeline();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* SVPipelineView::getActionEnableFilter()
{
  return m_ActionEnableFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::updateActionEnableFilter()
{
  QModelIndexList selectedIndexes = selectionModel()->selectedRows();

  // Set Enabled / Disabled
  disconnect(m_ActionEnableFilter, &QAction::toggled, 0, 0);
  m_ActionEnableFilter->setEnabled(selectedIndexes.size());

  // Set checked state
  int count = selectedIndexes.size();
  PipelineItem::WidgetState widgetState = PipelineItem::WidgetState::Ready;
  PipelineModel* model = getPipelineModel();
  for(int i = 0; i < count && widgetState != PipelineItem::WidgetState::Disabled; i++)
  {
    QModelIndex index = selectedIndexes[i];
    widgetState = model->widgetState(index);
  }

  // Lambda connections don't allow Qt::UniqueConnection
  // Also, this needs to be disconnected before changing the checked state
  if (widgetState == PipelineItem::WidgetState::Disabled)
  {
    m_ActionEnableFilter->setChecked(false);
  }
  else
  {
    m_ActionEnableFilter->setChecked(true);
  }

  connect(m_ActionEnableFilter, &QAction::toggled, [=] { setSelectedFiltersEnabled(m_ActionEnableFilter->isChecked()); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SVPipelineView::filterCount()
{
  PipelineModel* model = getPipelineModel();
  return model->rowCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::resetLayout()
{
//  // Check to see if we have removed all the filters
//  if(filterCount() <= 0)
//  {
//    // Emit a signal to tell SIMPLView_UI to erase the Filter Input Widget.
//    emit filterInputWidgetNeedsCleared();

//    // Remove the current Layout
//    QLayout* l = layout();
//    if(nullptr != l && l == m_FilterWidgetLayout)
//    {
//      qDeleteAll(l->children());
//      delete l;
//      m_FilterWidgetLayout = nullptr;
//    }

//    // and add the empty pipeline layout instead
//    newEmptyPipelineViewLayout();

//    if(m_DataStructureWidget)
//    {
//      m_DataStructureWidget->filterObjectActivated(nullptr);
//    }
//  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::clearPipeline()
{
  PipelineModel* model = getPipelineModel();

  std::vector<AbstractFilter::Pointer> filters;
  for (int i = 0; i < model->rowCount(); i++)
  {
    QModelIndex filterIndex = model->index(i, PipelineItem::Contents);
    filters.push_back(model->filter(filterIndex));
  }

  RemoveFilterCommand* removeCmd = new RemoveFilterCommand(filters, this, "Clear");
  addUndoCommand(removeCmd);

  emit clearDataStructureWidgetTriggered();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::mouseMoveEvent(QMouseEvent* event)
{
//  if(!(event->buttons() & Qt::LeftButton))
//  {
//    return;
//  }
//  if((event->pos() - m_DragStartPosition).manhattanLength() < QApplication::startDragDistance())
//  {
//    return;
//  }

//  beginDrag(event);

  QListView::mouseMoveEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::startDrag(Qt::DropActions supportedActions)
{
  Q_UNUSED(supportedActions)

  std::vector<PipelineFilterMimeData::FilterDragMetadata> filtersDragData;
  std::vector<AbstractFilter::Pointer> filters;
  PipelineModel* model = getPipelineModel();

  QModelIndexList selectedIndexes = selectionModel()->selectedRows();
  for (int i = 0; i < selectedIndexes.size(); i++)
  {
    QModelIndex selectedIndex = selectedIndexes[i];
    AbstractFilter::Pointer filter = model->filter(selectedIndex);

    PipelineFilterMimeData::FilterDragMetadata filterDragData;
    filterDragData.first = filter;
    filterDragData.second = selectedIndex.row();
    filtersDragData.push_back(filterDragData);

    filters.push_back(filter);
  }

  PipelineFilterMimeData* mimeData = new PipelineFilterMimeData();
  mimeData->setFilterDragData(filtersDragData);
  mimeData->setData(SIMPLView::DragAndDrop::FilterPipelineItem, QByteArray());

  Qt::KeyboardModifiers modifiers = QApplication::queryKeyboardModifiers();

  if (modifiers.testFlag(Qt::AltModifier) == false)
  {
    m_DragCommand = new QUndoCommand();

    RemoveFilterCommand* cmd = new RemoveFilterCommand(filters, this, "Remove", m_DragCommand);
    m_DragCommand->setText(cmd->text());

    addUndoCommand(m_DragCommand);
  }

  QDrag* drag = new QDrag(this);
  drag->setMimeData(mimeData);

  if (modifiers.testFlag(Qt::AltModifier))
  {
    drag->exec(Qt::CopyAction);
  }
  else
  {
    drag->exec(Qt::MoveAction);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::dragMoveEvent(QDragMoveEvent* event)
{  
  QListView::dragMoveEvent(event);

//  const QMimeData* mimedata = event->mimeData();

//  if(nullptr != dynamic_cast<const PipelineViewPtrMimeData*>(mimedata))
//  {
//    // The user is moving existing filter widgets, either within the same pipeline view or between pipeline views
//    SVPipelineViewWidget* origin = dynamic_cast<SVPipelineViewWidget*>(dynamic_cast<const PipelineViewPtrMimeData*>(mimedata)->getPipelineViewPtr());

//    if(origin == this)
//    {
//      if(qApp->queryKeyboardModifiers() != Qt::AltModifier)
//      {
//        QList<IndexedFilterObject> draggedFilterObjects = getDraggedFilterObjects();

//        for(int i = 0; i < draggedFilterObjects.size(); i++)
//        {
//          // Remove the filter widget
//          if(nullptr != m_FilterWidgetLayout && origin->containsFilterWidget(draggedFilterObjects[i].second))
//          {
//            PipelineFilterObject* draggedObject = draggedFilterObjects[i].second;
//            SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(draggedObject);
//            if(filterWidget == nullptr)
//            {
//              return;
//            }

//            m_FilterOrigPos = origin->valueOfFilterWidget(filterWidget).toInt();
//            m_FilterWidgetLayout->removeWidget(filterWidget);
//            filterWidget->setParent(nullptr);
//          }
//        }
//      }
//    }

//    int count = filterCount();
//    if(count <= 0)
//    {
//      return;
//    }

//    // insert filter
//    {
//      int i = getIndexAtPoint(event->pos());

//      if(i < 0)
//      {
//        i = filterCount();
//      }

//      SVPipelineFilterWidget* w = getFilterWidgetAtIndex(i);

//      if (w != nullptr)
//      {
//        if ((i >= count && event->pos().y() >= w->geometry().y() + w->geometry().height() / 2) || (event->pos().y() <= w->geometry().y() + w->geometry().height() / 2))
//        {
//          QList<IndexedFilterObject> draggedObjects = origin->getDraggedFilterObjects();
//          if (draggedObjects.size() > 1)
//          {
//           // m_FilterOutlineWidget->setFilter(nullptr);
//            m_FilterOutlineWidget->setFilterIndex(i + 1, count);
//            m_FilterOutlineWidget->setFilterTitle("Place " + QString::number(draggedObjects.size()) + " Filters Here");
//          }
//          else if (draggedObjects.size() == 1)
//          {
//            AbstractFilter::Pointer f = draggedObjects[0].second->getFilter();
//            m_FilterOutlineWidget->setFilter(f.get());
//            m_FilterOutlineWidget->setFilterIndex(i + 1, count);
//            m_FilterOutlineWidget->setFilterTitle(draggedObjects[0].second->getHumanLabel());
//          }
//          else
//          {
//            event->ignore();
//            return;
//          }
//          m_FilterWidgetLayout->insertWidget(i, m_FilterOutlineWidget);
//          m_FilterOutlineWidget->show();
//          reindexWidgetTitles();
//        }
//      }
//      else if(i == count)
//      {
//        QList<IndexedFilterObject> draggedObjects = origin->getDraggedFilterObjects();
//        if (draggedObjects.size() == 1)
//        {
//          m_FilterOutlineWidget->setFilter(draggedObjects[0].second->getFilter().get());
//          m_FilterOutlineWidget->setFilterIndex(i + 1, count);
//          m_FilterOutlineWidget->setFilterTitle(draggedObjects[0].second->getHumanLabel());
//        }

//        m_FilterWidgetLayout->insertWidget(i, m_FilterOutlineWidget);
//        m_FilterOutlineWidget->show();
//        reindexWidgetTitles();
//      }

//      event->accept();
//    }
//  }
//  else if(mimedata->hasUrls() || mimedata->hasFormat(SIMPLView::DragAndDrop::BookmarkItem) || mimedata->hasFormat(SIMPLView::DragAndDrop::FilterItem))
//  {
//    QString data;
//    if(mimedata->hasUrls())
//    {
//      data = mimedata->text();
//      QUrl url(data);
//      data = url.toLocalFile();
//    }
//    else if(mimedata->hasText())
//    {
//      data = mimedata->text();
//    }
//    else if(mimedata->hasFormat(SIMPLView::DragAndDrop::BookmarkItem))
//    {
//      QByteArray jsonArray = mimedata->data(SIMPLView::DragAndDrop::BookmarkItem);
//      QJsonDocument doc = QJsonDocument::fromJson(jsonArray);
//      QJsonObject obj = doc.object();

//      if(obj.size() > 1)
//      {
//        event->accept();
//        return;
//      }

//      QJsonObject::iterator iter = obj.begin();
//      data = iter.value().toString();
//    }
//    else
//    {
//      QByteArray jsonArray = mimedata->data(SIMPLView::DragAndDrop::FilterItem);
//      QJsonDocument doc = QJsonDocument::fromJson(jsonArray);
//      QJsonObject obj = doc.object();
//      QJsonObject::iterator iter = obj.begin();
//      data = iter.value().toString();
//    }

//    QFileInfo fi(data);
//    QString ext = fi.completeSuffix();
//    FilterManager* fm = FilterManager::Instance();
//    if(nullptr == fm)
//    {
//      return;
//    }
//    IFilterFactory::Pointer wf = fm->getFactoryFromClassName(data);

//    // If the dragged item is a filter item...
//    if(nullptr != wf)
//    {
//      QString humanName = wf->getFilterHumanLabel();

//      //bool didInsert = false;
//      // This path is taken if a filter is dragged from the list of filters

//      int i = getIndexAtPoint(event->pos());

//      if(i < 0 || i > filterCount())
//      {
//        return;
//      }

//      m_FilterOutlineWidget->setFilterIndex(i + 1, filterCount());
//      m_FilterOutlineWidget->setFilterTitle(humanName);
//      m_FilterWidgetLayout->insertWidget(i, m_FilterOutlineWidget);
//      m_FilterOutlineWidget->show();
//      reindexWidgetTitles();

//      event->accept();
//    }
//    // If the dragged item is a pipeline file...
//    else if(ext == "dream3d" || ext == "json" )
//    {
//      if(nullptr == m_FilterOutlineWidget)
//      {
//        event->ignore();
//        return;
//      }

//      QString pipelineName = fi.baseName();

//      //bool didInsert = false;
//      // This path is taken if a filter is dragged from the list of filters

//      int i = getIndexAtPoint(event->pos());

//      if (i < 0 || i > filterCount())
//      {
//        return;
//      }

//      m_FilterOutlineWidget->setFilterIndex(i + 1, filterCount());
//      m_FilterOutlineWidget->setFilterTitle("Place '" + pipelineName + "' Here");
//      m_FilterWidgetLayout->insertWidget(i, m_FilterOutlineWidget);
//      m_FilterOutlineWidget->show();
//      reindexWidgetTitles();

//      event->accept();
//    }
//    else
//    {
//      event->ignore();
//    }
//  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::dropEvent(QDropEvent* event)
{
  QListView::dropEvent(event);

  PipelineModel* model = getPipelineModel();

  const QMimeData* mimedata = event->mimeData();
  const PipelineFilterMimeData* filterData = dynamic_cast<const PipelineFilterMimeData*>(mimedata);
  if (filterData != nullptr)
  {
    // This is filter data from an SVPipelineView instance
    int dropRow = model->getDropIndex();

    std::vector<PipelineFilterMimeData::FilterDragMetadata> dragData = filterData->getFilterDragData();

    std::vector<AbstractFilter::Pointer> filters;
    for (size_t i = 0; i < dragData.size(); i++)
    {
      filters.push_back(dragData[i].first);
    }

    if (event->source() == this)
    {
      // This is an internal move, so we need to create an Add command and add it as a child to the overall drag command.
      AddFilterCommand* cmd = new AddFilterCommand(filters, this, dropRow, "Move", m_DragCommand);

      // Set the text of the drag command
      QString text = cmd->text();

      m_DragCommand->setText(text);

      // The overall drag command already has a child command that removed the filters initially, and
      // has already been placed on the undo stack and executed.  This new child command needs to be executed
      // so that it matches up with the state of its parent command.
      cmd->redo();
    }
    else
    {
      addFilters(filters, dropRow);
    }

    event->accept();
  }
  else if(mimedata->hasUrls())
  {
    QString data = mimedata->text();
    QUrl url(data);
    QString filePath = url.toLocalFile();

    int err = openPipeline(filePath, model->getDropIndex());

    if (err >= 0)
    {
      event->accept();
    }
    else
    {
      event->ignore();
    }
  }
  else if (mimedata->hasFormat(SIMPLView::DragAndDrop::BookmarkItem))
  {
    QByteArray jsonArray = mimedata->data(SIMPLView::DragAndDrop::BookmarkItem);
    QJsonDocument doc = QJsonDocument::fromJson(jsonArray);
    QJsonObject obj = doc.object();

    if(obj.size() > 1)
    {
      QMessageBox::warning(nullptr, "Warning", "This application currently does not support dragging and dropping multiple bookmarks.", QMessageBox::Ok);
      event->ignore();
      return;
    }

    QJsonObject::iterator iter = obj.begin();
    QString filePath = iter.value().toString();

    int err = openPipeline(filePath, model->getDropIndex());

    if (err >= 0)
    {
      event->accept();
    }
    else
    {
      event->ignore();
    }
  }
  else if (mimedata->hasFormat(SIMPLView::DragAndDrop::FilterListItem))
  {
    QByteArray jsonArray = mimedata->data(SIMPLView::DragAndDrop::FilterListItem);
    QJsonDocument doc = QJsonDocument::fromJson(jsonArray);
    QJsonObject obj = doc.object();
    QJsonObject::iterator iter = obj.begin();
    QString filterClassName = iter.value().toString();

    FilterManager* fm = FilterManager::Instance();
    if(nullptr == fm)
    {
      event->ignore();
      return;
    }

    IFilterFactory::Pointer wf = fm->getFactoryFromClassName(filterClassName);
    if(nullptr == wf)
    {
      event->ignore();
      return;
    }

    AbstractFilter::Pointer filter = wf->create();
    addFilter(filter, model->getDropIndex());

    event->accept();
  }
  else
  {
    event->ignore();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::blockPreflightSignals(bool b)
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
void SVPipelineView::setFiltersEnabled(QModelIndexList indexes, bool enabled)
{
  int count = indexes.size();
  PipelineModel* model = getPipelineModel();
  for(int i = 0; i < count; i++)
  {
    QModelIndex index = indexes[i];
    AbstractFilter::Pointer filter = model->filter(index);
    if (enabled == true)
    {
      filter->setEnabled(true);
      model->setWidgetState(index, PipelineItem::WidgetState::Ready);
    }
    else
    {
      filter->setEnabled(false);
      model->setWidgetState(index, PipelineItem::WidgetState::Disabled);
    }
  }

  preflightPipeline();
  emit filterEnabledStateChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::setSelectedFiltersEnabled(bool enabled)
{
  QModelIndexList indexes = selectionModel()->selectedRows();
  setFiltersEnabled(indexes, enabled);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::keyPressEvent(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
  {
    bool isRunning = getPipelineIsRunning();
    if(isRunning == false)
    {
      emit deleteKeyPressed();
    }
  }
  else if(event->key() == Qt::Key_A && qApp->queryKeyboardModifiers() == Qt::ControlModifier)
  {
    selectAll();
  }

  QListView::keyPressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::toReadyState()
{
  PipelineModel* model = getPipelineModel();
  for(int i = 0; i < model->rowCount(); i++)
  {
    QModelIndex index = model->index(i, PipelineItem::Contents);
    model->setWidgetState(index, PipelineItem::WidgetState::Ready);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::toRunningState()
{
  setPipelineIsRunning(true);
  setAcceptDrops(false);

  PipelineModel* model = getPipelineModel();
  for(int i = 0; i < model->rowCount(); i++)
  {
    QModelIndex index = model->index(i, PipelineItem::Contents);
    model->setPipelineState(index, PipelineItem::PipelineState::Running);
    FilterInputWidget* inputWidget = model->filterInputWidget(index);
    inputWidget->toRunningState();
  }

  m_ActionClearPipeline->setDisabled(true);

//  deleteBtn->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::toStoppedState()
{
  setPipelineIsRunning(false);
  setAcceptDrops(true);

  PipelineModel* model = getPipelineModel();
  for(int i = 0; i < model->rowCount(); i++)
  {
    QModelIndex index = model->index(i, PipelineItem::Contents);
    model->setPipelineState(index, PipelineItem::PipelineState::Stopped);
    FilterInputWidget* inputWidget = model->filterInputWidget(index);
    AbstractFilter::Pointer filter = model->filter(index);
    inputWidget->toIdleState();

    if (filter->getEnabled() == true)
    {
      model->setWidgetState(index, PipelineItem::WidgetState::Ready);
    }
  }

  m_ActionClearPipeline->setEnabled(model->rowCount() > 0);

//  deleteBtn->setEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SVPipelineView::openPipeline(const QString& filePath, int insertIndex)
{
  QFileInfo fi(filePath);
  if(fi.exists() == false)
  {
    QMessageBox::warning(nullptr, QString::fromLatin1("Pipeline Read Error"), QString::fromLatin1("There was an error opening the specified pipeline file. The pipeline file does not exist."));
    return -1;
  }

  QString ext = fi.suffix();
  QString name = fi.fileName();
  QString baseName = fi.baseName();

  if(ext == "dream3d")
  {
    QtSFileDragMessageBox* msgBox = new QtSFileDragMessageBox(this);
    msgBox->exec();
    msgBox->deleteLater();

    if(msgBox->didPressOkBtn() == true)
    {
      if(msgBox->isExtractPipelineBtnChecked() == false)
      {
        DataContainerReader::Pointer reader = DataContainerReader::New();
        reader->setInputFile(filePath);

        addFilter(reader, insertIndex);
        return true;
      }
    }
  }

  // Read the pipeline from the file
  FilterPipeline::Pointer pipeline = readPipelineFromFile(filePath);

  // Check that a valid extension was read...
  if(pipeline == FilterPipeline::NullPointer())
  {
    emit statusMessage(tr("The pipeline was not read correctly from file '%1'. '%2' is an unsupported file extension.").arg(name).arg(ext));
    emit stdOutMessage(tr("The pipeline was not read correctly from file '%1'. '%2' is an unsupported file extension.").arg(name).arg(ext));
    return -1;
  }

  // Notify user of successful read
  emit statusMessage(tr("Opened \"%1\" Pipeline").arg(baseName));
  emit stdOutMessage(tr("Opened \"%1\" Pipeline").arg(baseName));

  QList<AbstractFilter::Pointer> pipelineFilters = pipeline->getFilterContainer();
  std::vector<AbstractFilter::Pointer> filters;
  for (int i = 0; i < pipelineFilters.size(); i++)
  {
    filters.push_back(pipelineFilters[i]);
  }

  // Populate the pipeline view
  addFilters(filters, insertIndex);

  emit pipelineFilePathUpdated(filePath);
  emit pipelineChanged();

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer SVPipelineView::readPipelineFromFile(const QString& filePath)
{
  QFileInfo fi(filePath);
  QString ext = fi.suffix();

  FilterPipeline::Pointer pipeline;
  if(ext == "dream3d")
  {
    H5FilterParametersReader::Pointer dream3dReader = H5FilterParametersReader::New();
    pipeline = dream3dReader->readPipelineFromFile(filePath);
  }
  else if(ext == "json")
  {
    JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
    pipeline = jsonReader->readPipelineFromFile(filePath);
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
void SVPipelineView::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton)
  {
    m_DragStartPosition = event->pos();
    
    if (indexAt(event->pos()).isValid() == false)
    {
      clearSelection();

      emit filterInputWidgetNeedsCleared();
    }
  }

  QListView::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::requestContextMenu(const QPoint& pos)
{
  activateWindow();

  QModelIndex index = indexAt(pos);
  PipelineModel* model = getPipelineModel();
  QPoint mapped;

  if(model->itemType(index) == PipelineItem::ItemType::Filter)
  {
    mapped = viewport()->mapToGlobal(pos);
  }
  else if (model->itemType(index) == PipelineItem::ItemType::Pipeline)
  {
    mapped = viewport()->mapToGlobal(pos);
  }
  else
  {
    mapped = mapToGlobal(pos);
  }

  if(model->itemType(index) == PipelineItem::ItemType::Filter)
  {
    requestFilterItemContextMenu(mapped, index);
  }
  else if (model->itemType(index) == PipelineItem::ItemType::Pipeline)
  {
    requestPipelineItemContextMenu(mapped);
  }
  else
  {
    requestDefaultContextMenu(mapped);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::requestFilterItemContextMenu(const QPoint &pos, const QModelIndex &index)
{
  PipelineModel* model = getPipelineModel();
  QModelIndexList selectedIndexes = selectionModel()->selectedRows();

  QMenu menu;

  menu.addAction(m_ActionCut);
  menu.addAction(m_ActionCopy);
  menu.addSeparator();

  int count = selectedIndexes.size();
  bool widgetEnabled = true;

  for(int i = 0; i < count && widgetEnabled; i++)
  {
    AbstractFilter::Pointer filter = model->filter(selectedIndexes[i]);
    if (filter != nullptr)
    {
      widgetEnabled = filter->getEnabled();
    }
  }

  if(selectedIndexes.contains(index) == false)
  {
    // Only toggle the target filter widget if it is not in the selected objects
    QModelIndexList toggledIndices = QModelIndexList();
    toggledIndices.push_back(index);

    AbstractFilter::Pointer filter = model->filter(index);
    if (filter != nullptr)
    {
      widgetEnabled = filter->getEnabled();
    }

    disconnect(m_ActionEnableFilter, &QAction::toggled, 0, 0);
    connect(m_ActionEnableFilter, &QAction::toggled, [=] { setFiltersEnabled(toggledIndices, m_ActionEnableFilter->isChecked()); });
  }
  else
  {
    disconnect(m_ActionEnableFilter, &QAction::toggled, 0, 0);
    connect(m_ActionEnableFilter, &QAction::toggled, [=] { setFiltersEnabled(selectedIndexes, m_ActionEnableFilter->isChecked()); });
  }

  m_ActionEnableFilter->setChecked(widgetEnabled);
  m_ActionEnableFilter->setEnabled(true);
  m_ActionEnableFilter->setDisabled(getPipelineIsRunning());
  menu.addAction(m_ActionEnableFilter);

  menu.addSeparator();

  QAction* removeAction;
  QList<QKeySequence> shortcutList;
  shortcutList.push_back(QKeySequence(Qt::Key_Backspace));
  shortcutList.push_back(QKeySequence(Qt::Key_Delete));

  if (selectedIndexes.contains(index) == false || selectedIndexes.size() == 1)
  {
    removeAction = new QAction("Delete Filter", &menu);
    connect(removeAction, &QAction::triggered, [=] {
      AbstractFilter::Pointer filter = model->filter(index);
      removeFilter(filter);
    });
  }
  else
  {
    removeAction = new QAction(tr("Delete %1 Filters").arg(selectedIndexes.size()), &menu);
    connect(removeAction, &QAction::triggered, [=] {
      QList<QPersistentModelIndex> persistentList;
      for(int i = 0; i < selectedIndexes.size(); i++)
      {
        persistentList.push_back(selectedIndexes[i]);
      }

      std::vector<AbstractFilter::Pointer> filters;
      for(int i = 0; i < persistentList.size(); i++)
      {
        AbstractFilter::Pointer filter = model->filter(persistentList[i]);
        filters.push_back(filter);
      }

      removeFilters(filters);
    });
  }
  removeAction->setShortcuts(shortcutList);
  if (getPipelineIsRunning() == true)
  {
    removeAction->setDisabled(true);
  }

  menu.addAction(removeAction);

  menu.addSeparator();

  menu.addAction(m_ActionClearPipeline);

  menu.addSeparator();

  QAction* actionLaunchHelp = new QAction("Filter Help", this);
  connect(actionLaunchHelp, &QAction::triggered, [=] {
    AbstractFilter::Pointer filter = model->filter(index);
    if (filter != nullptr)
    {
      // Launch the help for this filter
      QString className = filter->getNameOfClass();

      DocRequestManager* docRequester = DocRequestManager::Instance();
      docRequester->requestFilterDocs(className);
    }
  });

  menu.addAction(actionLaunchHelp);

  menu.exec(pos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::requestPipelineItemContextMenu(const QPoint &pos)
{
  PipelineModel* model = getPipelineModel();
  QModelIndexList selectedIndexes = selectionModel()->selectedRows();

  if (selectedIndexes.size() <= 0)
  {
    return;
  }

  // Build up the contextual menu
  QMenu menu;
  menu.addAction(m_ActionCut);
  menu.addAction(m_ActionCopy);

  // Check to see if all the selected items are pipeline items
  bool allPipelines = true;
  for (int i = 0; i < selectedIndexes.size(); i++)
  {
    if (model->itemType(selectedIndexes[i]) != PipelineItem::ItemType::Pipeline)
    {
      allPipelines = false;
    }
  }

  requestSinglePipelineContextMenu(menu);

  menu.exec(pos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::requestSinglePipelineContextMenu(QMenu &menu)
{
  menu.addAction(m_ActionClearPipeline);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::requestDefaultContextMenu(const QPoint &pos)
{
  QMenu menu;
  menu.addAction(m_ActionPaste);
  menu.addSeparator();
  menu.addAction(m_ActionClearPipeline);

  menu.exec(pos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::handleFilterParameterChanged(QUuid id)
{
  Q_UNUSED(id)

  emit filterInputWidgetEdited();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::setModel(QAbstractItemModel* model)
{
  QAbstractItemModel* oldModel = this->model();
  if (oldModel != nullptr)
  {
    delete oldModel;
  }

  QListView::setModel(model);

  PipelineModel* pipelineModel = dynamic_cast<PipelineModel*>(model);

  if (pipelineModel != nullptr)
  {
    connect(pipelineModel, &PipelineModel::rowsInserted, this, [=] {
      m_ActionClearPipeline->setEnabled(true);
    });

    connect(pipelineModel, &PipelineModel::rowsRemoved, this, [=] {
      m_ActionClearPipeline->setEnabled(model->rowCount() > 0);
    });

    connect(pipelineModel, &PipelineModel::rowsMoved, this, [=] {
      m_ActionClearPipeline->setEnabled(model->rowCount() > 0);
    });
  }

  connect(selectionModel(), &QItemSelectionModel::selectionChanged, [=] (const QItemSelection &selected, const QItemSelection &deselected) {
    m_ActionCut->setEnabled(selected.size() > 0);
    m_ActionCopy->setEnabled(selected.size() > 0);
  });

  m_ActionClearPipeline->setEnabled(model->rowCount() > 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVPipelineView::isPipelineCurrentlyRunning()
{
  return m_PipelineRunning;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineModel* SVPipelineView::getPipelineModel()
{
  return static_cast<PipelineModel*>(model());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QObject*> SVPipelineView::getPipelineMessageObservers()
{
  return m_PipelineMessageObservers;
}
