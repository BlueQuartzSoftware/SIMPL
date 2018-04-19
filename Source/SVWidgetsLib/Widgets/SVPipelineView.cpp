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

  m_ActionEnableFilter->setCheckable(true);
  m_ActionEnableFilter->setChecked(true);
  m_ActionEnableFilter->setEnabled(false);

  m_ActionCut->setShortcut(QKeySequence::Cut);
  m_ActionCopy->setShortcut(QKeySequence::Copy);
  m_ActionPaste->setShortcut(QKeySequence::Paste);
  m_ActionClearPipeline->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Backspace));

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

  connect(this, &SVPipelineView::entered, this, [=] (const QModelIndex &index) {
    PipelineModel* model = getPipelineModel();

    model->setHovering(index);
  });

  connect(this, &SVPipelineView::deleteKeyPressed, this, &SVPipelineView::listenDeleteKeyTriggered);

  connect(m_ActionCut, &QAction::triggered, this, &SVPipelineView::listenCutTriggered);
  connect(m_ActionCopy, &QAction::triggered, this, &SVPipelineView::listenCopyTriggered);
  connect(m_ActionPaste, &QAction::triggered, this, &SVPipelineView::listenPasteTriggered);
  connect(m_ActionClearPipeline, &QAction::triggered, this, &SVPipelineView::listenClearPipelineTriggered);
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
  PipelineModel* model = getPipelineModel();

  if(model->rowCount() > 0)
  {
    m_ActionClearPipeline->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject* SVPipelineView::createFilterObjectFromFilter(AbstractFilter::Pointer filter)
{
  Breakpoint::Pointer breakpoint = std::dynamic_pointer_cast<Breakpoint>(filter);

  // Create a FilterWidget object
  SVPipelineFilterWidget* fw;
  if(nullptr != breakpoint)
  {
    fw = new BreakpointFilterWidget(filter, nullptr, this);
  }
  else
  {
    fw = new SVPipelineFilterWidget(filter, nullptr, this);
  }

  return fw;
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
void SVPipelineView::newEmptyPipelineViewLayout()
{
  if(m_EmptyPipelineLabel == nullptr)
  {
    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    QSpacerItem* verticalSpacer = new QSpacerItem(20, 341, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer, 0, 1, 1, 1);

    QSpacerItem* horizontalSpacer_3 = new QSpacerItem(102, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_3, 1, 0, 1, 1);

    m_EmptyPipelineLabel = new QLabel(this);
    m_EmptyPipelineLabel->setObjectName(QString::fromUtf8("label"));
    m_EmptyPipelineLabel->setMinimumSize(QSize(325, 250));
    m_EmptyPipelineLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
                                                          "border-radius: 20px;\n"
                                                          "/*border: 1px solid rgb(120, 120, 120);*/\n"
                                                          "/* background-color: rgb(160, 160, 160); */\n"
                                                          "font-weight: bold;\n"
                                                          "color : rgb(150, 150, 150);\n"
                                                          "text-align: center;\n"
                                                          "margin: 5px;\n"
                                                          "padding: 10px;\n"
                                                          "}"));
    m_EmptyPipelineLabel->setAlignment(Qt::AlignCenter);
    QString text;
    QTextStream ss(&text);
    ss << "<h2>Creating a Pipeline</h2>";
    ss << "<hr>";
    ss << "File -> Open <br />";
    ss << "File -> New <br />";
    ss << "Drag and drop filters here<br />";
    ss << "Double click a Bookmark<br />";
    ss << "Double click a Prebuilt Pipeline<br />";
    m_EmptyPipelineLabel->setText(text);

    gridLayout->addWidget(m_EmptyPipelineLabel, 1, 1, 1, 1);

    QSpacerItem* horizontalSpacer_4 = new QSpacerItem(102, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_4, 1, 2, 1, 1);

    QSpacerItem* verticalSpacer_2 = new QSpacerItem(20, 341, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer_2, 2, 1, 1, 1);
  }
  emit windowTitleNeedsRefresh();
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
void SVPipelineView::clearPipeline(bool addToUndoStack)
{
  PipelineModel* model = getPipelineModel();

  if (addToUndoStack == true)
  {
    std::vector<AbstractFilter::Pointer> filters;
    for (int i = 0; i < model->rowCount(); i++)
    {
      QModelIndex filterIndex = model->index(i, PipelineItem::Name);
      filters.push_back(model->filter(filterIndex));
    }

    RemoveFilterCommand* removeCmd = new RemoveFilterCommand(filters, model, "Clear");
    addUndoCommand(removeCmd);
  }
  else
  {
    for (int i = 0; i < model->rowCount(); i++)
    {
      model->removeRow(0);
    }
  }

  if(m_DataStructureWidget)
  {
    m_DataStructureWidget->filterObjectActivated(nullptr);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::reindexWidgetTitles()
{
//  qint32 count = filterCount();
//  for(qint32 i = 0; i < count; ++i)
//  {
//    PipelineFilterObject* fw = filterObjectAt(i);
//    if(fw)
//    {
//      AbstractFilter::Pointer filter = fw->getFilter();
//      if(filter)
//      {
//        QString hl = filter->getHumanLabel();

//        fw->setFilterTitle(hl);
//        fw->setFilterIndex(i+1, count);
//        filter->setPipelineIndex(i);
//      }
//    }
//  }
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
void SVPipelineView::on_focusInEventStarted(QFocusEvent* event)
{
  Q_UNUSED(event)

  setFocus();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::on_focusOutEventStarted(QFocusEvent* event)
{
  focusOutEvent(event);
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
void SVPipelineView::setSelectedFilterObject(PipelineFilterObject* w, Qt::KeyboardModifiers modifiers)
{
//  SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(w);
//  if(filterWidget == nullptr)
//  {
//    return;
//  }

//  if(modifiers == Qt::ShiftModifier)
//  {
//    clearSelectedFilterObjects();

//    if(nullptr == m_ShiftStart)
//    {
//      m_ShiftStart = filterWidget;
//    }

//    int begin;
//    int end;
//    if(m_FilterWidgetLayout->indexOf(filterWidget) < m_FilterWidgetLayout->indexOf(m_ShiftStart))
//    {
//      // The filter widget that was just selected is before the "active" widget
//      begin = m_FilterWidgetLayout->indexOf(filterWidget);
//      end = m_FilterWidgetLayout->indexOf(m_ShiftStart);
//    }
//    else
//    {
//      // The filter widget that was just selected is after the "active" widget
//      begin = m_FilterWidgetLayout->indexOf(m_ShiftStart);
//      end = m_FilterWidgetLayout->indexOf(filterWidget);
//    }

//    for(int i = begin; i <= end; i++)
//    {
//      SVPipelineFilterWidget* fw = dynamic_cast<SVPipelineFilterWidget*>(filterObjectAt(i));
//      if(fw)
//      {
//        fw->setSelected(true);
//      }
//    }
//  }
//  else if(modifiers == Qt::ControlModifier)
//  {
//    m_ShiftStart = filterWidget;

//    if(filterWidget->isSelected())
//    {
//      filterWidget->setSelected(true);
//      if(getSelectedFilterObjects().isEmpty())
//      {
//        m_ShiftStart = nullptr;
//      }
//    }
//    else
//    {
//      filterWidget->setSelected(true);
//    }
//  }
//  else
//  {
//    clearSelectedFilterObjects();
//    m_ShiftStart = filterWidget;
//    filterWidget->setSelected(true);
//  }

//  QList<PipelineFilterObject*> selectedObjects = getSelectedFilterObjects();

//  if(selectedObjects.size() == 1)
//  {
//    emit filterInputWidgetChanged(selectedObjects[0]->getFilterInputWidget());
//    emit pipelineFilterObjectSelected(selectedObjects[0]);
//    //    if(m_DataStructureWidget)
//    //    {
//    //      m_DataStructureWidget->filterObjectActivated(selectedObjects[0]);
//    //    }
//  }
//  else
//  {
//    emit filterInputWidgetNeedsCleared();

//    if(m_DataStructureWidget)
//    {
//      m_DataStructureWidget->filterObjectActivated(nullptr);
//    }
//  }

//  filterWidget->setFocus();

//  updateActionEnableFilter();
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

  QFrame::keyPressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineView::toReadyState()
{
  PipelineModel* model = getPipelineModel();
  for(int i = 0; i < model->rowCount(); i++)
  {
    QModelIndex index = model->index(i, PipelineItem::Name);
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
    QModelIndex index = model->index(i, PipelineItem::Name);
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
    QModelIndex index = model->index(i, PipelineItem::Name);
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

  m_ActionClearPipeline->setEnabled(true);

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
//    m_ShiftStart = nullptr;
    emit filterInputWidgetNeedsCleared();
  }

  QFrame::mousePressEvent(event);
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
      RemoveFilterCommand* cmd = new RemoveFilterCommand(filter, model, "Remove");
      addUndoCommand(cmd);
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

      RemoveFilterCommand* cmd = new RemoveFilterCommand(filters, model, "Remove");
      addUndoCommand(cmd);
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

  if (getPipelineIsRunning() == true)
  {
    m_ActionClearPipeline->setDisabled(true);
  }
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

//  connect(m_UndoStack, &QUndoStack::undoTextChanged, [=] (const QString &text) {
//    m_PreviousUndoText = m_CurrentUndoText;
//    m_CurrentUndoText = text;
//  });
//  connect(m_UndoStack.data(), &QUndoStack::redoTextChanged, [=] (const QString &text) {
//    m_PreviousRedoText = m_CurrentRedoText;
//    m_CurrentRedoText = text;
//  });

  connect(m_ActionUndo, &QAction::triggered, [=] {
//    emit stdOutMessage("Undo " + m_PreviousUndoText);
    QString text = m_ActionUndo->text();
    emit stdOutMessage(text);
  });
  connect(m_ActionRedo, &QAction::triggered, [=] {
//    emit stdOutMessage("Redo " + m_PreviousRedoText);
    QString text = m_ActionRedo->text();
    emit stdOutMessage(text);
  });
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
PipelineModel* SVPipelineView::getPipelineModel()
{
  return static_cast<PipelineModel*>(model());
}
