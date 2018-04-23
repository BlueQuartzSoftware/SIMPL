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

  QClipboard* clipboard = QApplication::clipboard();
  connect(clipboard, SIGNAL(dataChanged()), this, SLOT(updatePasteAvailability()));

  // Run this once, so that the Paste button availability is updated for what is currently on the system clipboard
  updatePasteAvailability();

  m_FilterOutlineWidget = new SVPipelineFilterOutlineWidget(nullptr);
  m_FilterOutlineWidget->setObjectName("m_DropBox");

  setContextMenuPolicy(Qt::CustomContextMenu);
  setFocusPolicy(Qt::StrongFocus);

  setupUndoStack();

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
void SVPipelineView::addFilters(std::vector<AbstractFilter::Pointer> filters)
{
  AddFilterCommand* cmd = new AddFilterCommand(filters, this, -1, "Add");
  addUndoCommand(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::removeFilter(AbstractFilter::Pointer filter)
{
  PipelineModel* model = getPipelineModel();

  RemoveFilterCommand* cmd = new RemoveFilterCommand(filter, model, "Remove");
  addUndoCommand(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::removeFilters(std::vector<AbstractFilter::Pointer> filters)
{
  PipelineModel* model = getPipelineModel();

  RemoveFilterCommand* cmd = new RemoveFilterCommand(filters, model, "Remove");
  addUndoCommand(cmd);
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
  // SIMPL-FIXME: Implement Cut using one window

//    SVPipelineView* viewWidget = m_ActiveWindow->getPipelineViewWidget();

//    QList<PipelineFilterObject*> filterWidgets = viewWidget->getSelectedFilterObjects();

//    FilterPipeline::Pointer pipeline = FilterPipeline::New();
//    for(int i = 0; i < filterWidgets.size(); i++)
//    {
//      pipeline->pushBack(filterWidgets[i]->getFilter());
//    }

//    JsonFilterParametersWriter::Pointer jsonWriter = JsonFilterParametersWriter::New();
//    QString jsonString = jsonWriter->writePipelineToString(pipeline, "Pipeline");

//    QClipboard* clipboard = QApplication::clipboard();
//    clipboard->setText(jsonString);

//    RemoveFilterCommand* removeCmd = new RemoveFilterCommand(filterWidgets, viewWidget, "Cut");
//    viewWidget->addUndoCommand(removeCmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::listenCopyTriggered()
{
  // SIMPL-FIXME: Implement Copy using one window

//    SVPipelineView* viewWidget = m_ActiveWindow->getPipelineViewWidget();

//    FilterPipeline::Pointer pipeline = FilterPipeline::New();
//    QList<PipelineFilterObject*> filterWidgets = viewWidget->getSelectedFilterObjects();
//    for(int i = 0; i < filterWidgets.size(); i++)
//    {
//      pipeline->pushBack(filterWidgets[i]->getFilter());
//    }

//    JsonFilterParametersWriter::Pointer jsonWriter = JsonFilterParametersWriter::New();
//    QString json = jsonWriter->writePipelineToString(pipeline, "Copy - Pipeline");
//    QClipboard* clipboard = QApplication::clipboard();
//    clipboard->setText(json);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::listenPasteTriggered()
{
  // SIMPL-FIXME: Implement Paste using one window

//    SVPipelineView* viewWidget = m_ActiveWindow->getPipelineViewWidget();

//    QClipboard* clipboard = QApplication::clipboard();
//    QString jsonString = clipboard->text();

//    JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
//    FilterPipeline::Pointer pipeline = jsonReader->readPipelineFromString(jsonString);
//    FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();

//    AddFilterCommand* addCmd = new AddFilterCommand(container, viewWidget, "Paste", -1);
//    viewWidget->addUndoCommand(addCmd);
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

  RemoveFilterCommand* removeCmd = new RemoveFilterCommand(filters, model, "Remove");
  addUndoCommand(removeCmd);
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
  bool widgetEnabled = true;
  PipelineModel* model = getPipelineModel();
  for(int i = 0; i < count && widgetEnabled; i++)
  {
    QModelIndex index = selectedIndexes[i];
    widgetEnabled = model->filterEnabled(index);
  }

  // Lambda connections don't allow Qt::UniqueConnection
  // Also, this needs to be disconnected before changing the checked state
  m_ActionEnableFilter->setChecked(widgetEnabled);

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

  RemoveFilterCommand* removeCmd = new RemoveFilterCommand(filters, model, "Clear");
  addUndoCommand(removeCmd);

  if(m_DataStructureWidget)
  {
    m_DataStructureWidget->filterObjectActivated(nullptr);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::startDrag(QMouseEvent* event, SVPipelineFilterWidget* fw)
{
//  // The user is dragging the filter widget so we should set it as selected.
//  if(fw->isSelected() == false)
//  {
//    setSelectedFilterObject(fw, Qt::NoModifier);
//  }

//  QList<IndexedFilterObject> selectedObjects = getSelectedIndexedFilterObjects();
//  setDraggedFilterObjects(selectedObjects);

//  QPixmap pixmap = m_ShiftStart->grab();

//  int pWidth = pixmap.size().width();
//  int pHeight = pixmap.size().height() * selectedObjects.size() + (3 * (selectedObjects.size() - 1));

//  // Create new picture for transparent
//  QPixmap transparent(pWidth, pHeight);
//  // Do transparency
//  transparent.fill(Qt::transparent);

//  QPainter p;
//  p.begin(&transparent);
//  p.setOpacity(0.70);
//  int offset = 0;
//  for(int i = 0; i < selectedObjects.size(); i++)
//  {
//    SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(selectedObjects[i].second);
//    if(filterWidget == nullptr)
//    {
//      continue;
//    }

//    QPixmap currentPixmap = filterWidget->grab();
//    p.drawPixmap(0, offset, currentPixmap);
//    offset = offset + pixmap.size().height() + 3;
//  }
//  p.end();

//  PipelineViewPtrMimeData* mimeData = new PipelineViewPtrMimeData();
//  mimeData->setPipelineViewPtr(this);

//  QDrag* drag = new QDrag(this);
//  drag->setMimeData(mimeData);
//  drag->setPixmap(transparent);
//  drag->setHotSpot(event->pos());

//  drag->exec(Qt::CopyAction);
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
    model->setFilterEnabled(index, enabled);
  }

  emit preflightTriggered(QModelIndex(), model);
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
    inputWidget->toIdleState();

    if (model->filterEnabled(index) == true)
    {
      model->setWidgetState(index, PipelineItem::WidgetState::Ready);
//      disableBtn->setChecked(false);
    }
    else
    {
      model->setWidgetState(index, PipelineItem::WidgetState::Disabled);
//      disableBtn->setChecked(true);
    }
  }

  m_ActionClearPipeline->setEnabled(model->rowCount() > 0);

//  deleteBtn->setEnabled(true);
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
    clearSelection();

    emit filterInputWidgetNeedsCleared();
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
void SVPipelineView::setupUndoStack()
{
  m_UndoStack = new QUndoStack(this);
  m_UndoStack->setUndoLimit(10);

  m_ActionUndo = m_UndoStack->createUndoAction(this);
  m_ActionRedo = m_UndoStack->createRedoAction(this);
  m_ActionUndo->setShortcut(QKeySequence::Undo);
  m_ActionRedo->setShortcut(QKeySequence::Redo);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::undo()
{
  m_UndoStack->undo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::redo()
{
  m_UndoStack->redo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::addUndoCommand(QUndoCommand* cmd)
{
  m_UndoStack->push(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::setDataStructureWidget(DataStructureWidget* w)
{
  if(nullptr == w)
  {
    disconnect(this, SIGNAL(pipelineFilterObjectSelected(PipelineFilterObject*)),
               m_DataStructureWidget, SLOT(filterObjectActivated(PipelineFilterObject*)));
    m_DataStructureWidget = w;
  }
  else
  {
    m_DataStructureWidget = w;
    connect(this, SIGNAL(pipelineFilterObjectSelected(PipelineFilterObject*)),
            m_DataStructureWidget, SLOT(filterObjectActivated(PipelineFilterObject*)));
  }
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureWidget* SVPipelineView::getDataStructureWidget()
{
  return m_DataStructureWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* SVPipelineView::getActionUndo()
{
  return m_ActionUndo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* SVPipelineView::getActionRedo()
{
  return m_ActionRedo;
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

  m_ActionClearPipeline->setEnabled(model->rowCount() > 0);

  QListView::setModel(model);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineModel* SVPipelineView::getPipelineModel()
{
  return static_cast<PipelineModel*>(model());
}
