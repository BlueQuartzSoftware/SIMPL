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

#include "PipelineTreeView.h"

#include <iostream>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "SIMPLib/Common/DocRequestManager.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/PipelineTreeItemDelegate.h"
#include "SVWidgetsLib/Widgets/PipelineItem.h"
#include "SVWidgetsLib/Widgets/SIMPLViewMenuItems.h"
#include "SVWidgetsLib/Widgets/PipelineModel.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeView::PipelineTreeView(QWidget* parent)
: QTreeView(parent)
, m_ActiveIndexBeingDragged(QPersistentModelIndex())
, m_TopLevelItemPlaceholder(QModelIndex())
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeView::~PipelineTreeView()
{
  // Delete action if it exists
  if(m_ActionEnableFilter)
  {
    delete m_ActionEnableFilter;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::setupGui()
{
  m_ActionNewPipeline = new QAction("New Pipeline", this);
  m_ActionCut = new QAction("Paste", this);
  m_ActionCopy = new QAction("Paste", this);
  m_ActionPaste = new QAction("Paste", this);
  m_ActionClearFilters = new QAction("Clear Pipeline", this);

  connectSignalsSlots();

  setContextMenuPolicy(Qt::CustomContextMenu);

  PipelineTreeItemDelegate* dlg = new PipelineTreeItemDelegate(this);
  setItemDelegate(dlg);

  m_ActionEnableFilter = new QAction("Enable", this);
  m_ActionEnableFilter->setCheckable(true);
  m_ActionEnableFilter->setChecked(true);
  m_ActionEnableFilter->setEnabled(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::connectSignalsSlots()
{
  connect(this, &PipelineTreeView::customContextMenuRequested, this, &PipelineTreeView::contextMenuRequested);

  connect(this, SIGNAL(collapsed(const QModelIndex&)), SLOT(collapseIndex(const QModelIndex&)));
  connect(this, SIGNAL(expanded(const QModelIndex&)), SLOT(expandIndex(const QModelIndex&)));

  // Menu Item Connections
  connect(m_ActionNewPipeline, &QAction::triggered, this, &PipelineTreeView::listenNewPipelineTriggered);
  connect(m_ActionCut, &QAction::triggered, this, &PipelineTreeView::listenCutTriggered);
  connect(m_ActionCopy, &QAction::triggered, this, &PipelineTreeView::listenCopyTriggered);
  connect(m_ActionPaste, &QAction::triggered, this, &PipelineTreeView::listenPasteTriggered);
  connect(m_ActionClearFilters, &QAction::triggered, this, &PipelineTreeView::listenClearPipelineTriggered);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::contextMenuRequested(const QPoint &pos)
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
    requestPipelineItemContextMenu(mapped, index);
  }
  else
  {
    requestDefaultContextMenu(mapped);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::requestFilterItemContextMenu(const QPoint &pos, const QModelIndex &index)
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
    connect(removeAction, &QAction::triggered, [=] { model->removeRow(index.row()); });
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

      for(int i = 0; i < persistentList.size(); i++)
      {
        model->removeRow(persistentList[i].row(), persistentList[i].parent());
      }
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
void PipelineTreeView::requestPipelineItemContextMenu(const QPoint &pos, const QModelIndex &index)
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

  if (selectedIndexes.contains(index) == false || selectedIndexes.size() == 1 || allPipelines == false)
  {
    requestSinglePipelineContextMenu(menu, index);
  }
  else
  {
    requestMultiplePipelineContextMenu(menu, selectedIndexes);
  }

  menu.exec(pos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::requestSinglePipelineContextMenu(QMenu &menu, const QModelIndex &pipelineIdx)
{
  PipelineModel* model = getPipelineModel();

  if (model->isActivePipeline(pipelineIdx) == false)
  {
    menu.addSeparator();

    QAction* activePipelineAction = new QAction(tr("Set '%1' as Active Pipeline").arg(model->data(pipelineIdx, Qt::DisplayRole).toString()));
    connect(activePipelineAction, &QAction::triggered, [=] { updateActivePipeline(pipelineIdx); });
    menu.addAction(activePipelineAction);
  }

  menu.addSeparator();
  menu.addAction(m_ActionClearFilters);

  QAction* removeAction = new QAction("Delete Pipeline", &menu);
  connect(removeAction, &QAction::triggered, [=] {
    if (model->isActivePipeline(pipelineIdx))
    {
      // We are trying to delete the active pipeline, so find another pipeline to set as the active pipeline
      QModelIndex nextActivePipeline = findNewActivePipeline(pipelineIdx);
      updateActivePipeline(nextActivePipeline);
    }
    model->removeRow(pipelineIdx.row());
  });

  QList<QKeySequence> shortcutList;
  shortcutList.push_back(QKeySequence(Qt::Key_Backspace));
  shortcutList.push_back(QKeySequence(Qt::Key_Delete));
  removeAction->setShortcuts(shortcutList);

  if (getPipelineIsRunning() == true)
  {
    removeAction->setDisabled(true);
  }

  menu.addAction(removeAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex PipelineTreeView::findNewActivePipeline(const QModelIndex &oldActivePipeline)
{
  PipelineModel* model = getPipelineModel();

  int row = oldActivePipeline.row();
  if (row == model->rowCount(oldActivePipeline.parent()) - 1)
  {
    // This is the last pipeline, so set the previous pipeline as the active pipeline
    row--;
  }
  else
  {
    // Set the next pipeline as the active pipeline
    row++;
  }
  QModelIndex nextActivePipeline = model->index(row, PipelineItem::Name, oldActivePipeline.parent());
  return nextActivePipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::requestMultiplePipelineContextMenu(QMenu &menu, QModelIndexList pipelineIndices)
{
  PipelineModel* model = getPipelineModel();

  QAction* removeAction = new QAction(tr("Delete %1 Pipelines").arg(pipelineIndices.size()), &menu);
  connect(removeAction, &QAction::triggered, [=] {
    QList<QPersistentModelIndex> persistentList;
    for(int i = 0; i < pipelineIndices.size(); i++)
    {
      persistentList.push_back(pipelineIndices[i]);
    }

    for(int i = 0; i < persistentList.size(); i++)
    {
      if (model->isActivePipeline(persistentList[i]))
      {
        // We are trying to delete the active pipeline, so find another pipeline to set as the active pipeline
        QModelIndex nextActivePipeline = findNewActivePipeline(persistentList[i]);
        updateActivePipeline(nextActivePipeline);
      }

      model->removeRow(persistentList[i].row(), persistentList[i].parent());
    }
  });

  QList<QKeySequence> shortcutList;
  shortcutList.push_back(QKeySequence(Qt::Key_Backspace));
  shortcutList.push_back(QKeySequence(Qt::Key_Delete));
  removeAction->setShortcuts(shortcutList);

  if (getPipelineIsRunning() == true)
  {
    removeAction->setDisabled(true);
  }

  menu.addAction(removeAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::requestDefaultContextMenu(const QPoint &pos)
{
  QMenu menu;
  menu.addAction(m_ActionNewPipeline);
  menu.addSeparator();
  menu.addAction(m_ActionPaste);
  menu.addSeparator();
  menu.addAction(m_ActionClearFilters);

  menu.exec(pos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::listenCutTriggered()
{
  // SIMPL-FIXME: Implement Cut using one window

//    SVPipelineViewWidget* viewWidget = m_ActiveWindow->getPipelineViewWidget();

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
void PipelineTreeView::listenCopyTriggered()
{
  // SIMPL-FIXME: Implement Copy using one window

//    SVPipelineViewWidget* viewWidget = m_ActiveWindow->getPipelineViewWidget();

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
void PipelineTreeView::listenPasteTriggered()
{
  // SIMPL-FIXME: Implement Paste using one window

//    SVPipelineViewWidget* viewWidget = m_ActiveWindow->getPipelineViewWidget();

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
void PipelineTreeView::listenNewPipelineTriggered()
{
  QString pipelineName = "Untitled Pipeline";
  bool setPipelineAsActive = true;

  addPipeline(pipelineName, FilterPipeline::New(), setPipelineAsActive);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::listenClearPipelineTriggered()
{
  PipelineModel* model = getPipelineModel();

  if(model->rowCount() > 0)
  {
    removePipeline
    m_ActionClearFilters->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::addPipeline(const QString &pipelineName, FilterPipeline::Pointer pipeline, bool setAsActive, QModelIndex parentIndex, int insertionIndex)
{
  PipelineModel* model = getPipelineModel();

  if (parentIndex == QModelIndex())
  {
    if (model->getMaxNumberOfPipelines() == 1)
    {
      updateActivePipeline(QModelIndex());
      model->removeRow(0);
    }

    int row = model->rowCount();
    model->insertRow(row);
    QModelIndex pipelineIndex = model->index(row, PipelineItem::Name);
    model->setData(pipelineIndex, pipelineName, Qt::DisplayRole);
    model->setItemType(pipelineIndex, PipelineItem::ItemType::Pipeline);
    parentIndex = pipelineIndex;
  }

  if (insertionIndex == -1)
  {
    insertionIndex = model->rowCount(parentIndex);
  }

  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
  QModelIndexList list;
  for (int i = 0; i < filters.size(); i++)
  {
    AbstractFilter::Pointer filter = filters[i];
    blockSignals(true);
    addFilterToModel(filter, parentIndex, insertionIndex);
    blockSignals(false);

    QModelIndex filterIndex = model->index(insertionIndex, PipelineItem::Name, parentIndex);
    list.push_back(filterIndex);

    insertionIndex++;
  }

  if (setAsActive == true)
  {
    updateActivePipeline(parentIndex);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::addFilterToModel(AbstractFilter::Pointer filter, const QModelIndex &parentIndex, int insertionIndex)
{
  PipelineModel* model = getPipelineModel();

  QModelIndex pipelineIndex = parentIndex;
  if (pipelineIndex.isValid() == false)
  {
    pipelineIndex = m_ActivePipelineIndex;
    if (pipelineIndex.isValid() == false)
    {
      FilterPipeline::Pointer pipeline = FilterPipeline::New();
      pipeline->pushBack(filter);
      addPipeline("Untitled Pipeline", pipeline, true);
    }
  }

  model->insertRow(insertionIndex, pipelineIndex);
  QModelIndex filterIndex = model->index(insertionIndex, PipelineItem::Name, pipelineIndex);
  model->setData(filterIndex, filter->getHumanLabel(), Qt::DisplayRole);
  model->setItemType(filterIndex, PipelineItem::ItemType::Filter);
  model->setFilter(filterIndex, filter);

  if (m_ActivePipelineIndex.isValid() == false)
  {
    updateActivePipeline(pipelineIndex);
  }

  QModelIndexList list;
  list.push_back(filterIndex);

  emit needsPreflight(m_ActivePipelineIndex, model);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::updateActivePipeline(const QModelIndex &pipelineIdx)
{
  emit clearIssuesTriggered();

  PipelineModel* model = getPipelineModel();
  model->setActivePipeline(m_ActivePipelineIndex, false);

  m_ActivePipelineIndex = pipelineIdx;

  if (m_ActivePipelineIndex.isValid() == true)
  {
    model->setActivePipeline(pipelineIdx, true);
    emit needsPreflight(m_ActivePipelineIndex, model);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::removePipeline(int pipelineIndex, QModelIndex parentIndex)
{
  PipelineModel* model = getPipelineModel();

  QModelIndex pipelineModelIdx = model->index(pipelineIndex, 0, parentIndex);
  for (int i = 0; i < model->rowCount(pipelineModelIdx); i++)
  {
    model->removeRow(0, pipelineModelIdx);
  }

  int removalRow = pipelineModelIdx.row();
  model->removeRow(pipelineModelIdx.row(), parentIndex);

  QModelIndex newActivePipelineIdx;
  if (model->index(removalRow, 0, parentIndex).isValid())
  {
    newActivePipelineIdx = model->index(removalRow, 0, parentIndex);
  }
  else if (model->index(removalRow + 1, 0, parentIndex).isValid())
  {
    newActivePipelineIdx = model->index(removalRow + 1, 0, parentIndex);
  }
  else if (model->index(removalRow - 1, 0, parentIndex).isValid())
  {
    newActivePipelineIdx = model->index(removalRow - 1, 0, parentIndex);
  }
  else
  {
    newActivePipelineIdx = QModelIndex();
  }

  updateActivePipeline(newActivePipelineIdx);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::setFiltersEnabled(QModelIndexList indices, bool enabled)
{
  PipelineModel* model = getPipelineModel();

  int count = indices.size();
  QModelIndexList pipelineIndices;
  for(int i = 0; i < count; i++)
  {
    QModelIndex index = indices[i];
    model->setFilterEnabled(index, enabled);
    if (pipelineIndices.contains(index.parent()) == false)
    {
      pipelineIndices.push_back(index.parent());
    }
  }

  for (int i = 0; i < pipelineIndices.size(); i++)
  {
    emit needsPreflight(pipelineIndices[i], getPipelineModel());
  }

  emit filterEnabledStateChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::addActionList(QList<QAction*> actionList)
{
  for(int i = 0; i < actionList.size(); i++)
  {
    m_Menu.addAction(actionList[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList PipelineTreeView::filterOutDescendants(QModelIndexList indexList)
{
  PipelineModel* model = getPipelineModel();

  for(int i = indexList.size() - 1; i >= 0; i--)
  {
    QPersistentModelIndex index = indexList[i];
    QString name = model->index(index.row(), PipelineItem::Name, index.parent()).data().toString();
    // Walk up the tree from the index...if an ancestor is selected, remove the index
    while(index.isValid() == true)
    {
      QPersistentModelIndex parent = index.parent();
      QString parentName = model->index(parent.row(), PipelineItem::Name, parent.parent()).data().toString();
      if(indexList.contains(index.parent()) == true)
      {
        indexList.removeAt(i);
        break;
      }
      index = index.parent();
    }
  }

  return indexList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::setModel(QAbstractItemModel* model)
{
  // Set the model
  QTreeView::setModel(model);

  PipelineModel* bModel = qobject_cast<PipelineModel*>(model);

  // Expand indexes that have their expand member set to true
  expandChildren(QModelIndex(), bModel);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::expandChildren(const QModelIndex& parent, PipelineModel *model)
{
  for(int row = 0; row < model->rowCount(parent); row++)
  {
    QModelIndex index = model->index(row, 0, parent);
    if(model->needsToBeExpanded(index))
    {
      expand(index);
    }

    // Recursive call, to expand all items in the tree
    expandChildren(index, model);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::collapseIndex(const QModelIndex& index)
{
  if(index.isValid())
  {
    PipelineModel* model = getPipelineModel();
    QModelIndex sibling = model->sibling(0, 0, index);

    QTreeView::collapse(index);
    model->setNeedsToBeExpanded(index, false);
    model->setNeedsToBeExpanded(sibling, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::expandIndex(const QModelIndex& index)
{
  PipelineModel* model = getPipelineModel();
  QModelIndex sibling = model->sibling(0, 0, index);

  QTreeView::expand(index);
  model->setNeedsToBeExpanded(index, true);
  model->setNeedsToBeExpanded(sibling, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  emit currentIndexChanged(current, previous);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::updateActionEnableFilter()
{
  PipelineModel* model = getPipelineModel();

  QModelIndexList indices = selectionModel()->selectedRows(PipelineItem::Name);

  // Set Enabled / Disabled
  disconnect(m_ActionEnableFilter, &QAction::toggled, 0, 0);
  m_ActionEnableFilter->setEnabled(indices.size());

  // Set checked state
  int count = indices.size();
  bool widgetEnabled = true;
  for(int i = 0; i < count && widgetEnabled; i++)
  {
    AbstractFilter::Pointer filter = model->filter(indices[i]);
    if (filter != nullptr)
    {
      widgetEnabled = filter->getEnabled();
    }
  }

  // Lambda connections don't allow Qt::UniqueConnection
  // Also, this needs to be disconnected before changing the checked state
  m_ActionEnableFilter->setChecked(widgetEnabled);

  connect(m_ActionEnableFilter, &QAction::toggled, [=] { setSelectedFiltersEnabled(m_ActionEnableFilter->isChecked()); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineModel* PipelineTreeView::getPipelineModel()
{
  return dynamic_cast<PipelineModel*>(model());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* PipelineTreeView::getNewPipelineAction()
{
  return m_ActionNewPipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* PipelineTreeView::getCutAction()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* PipelineTreeView::getCopyAction()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* PipelineTreeView::getPasteAction()
{
  return m_ActionPaste;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* PipelineTreeView::getClearPipelineAction()
{
  return m_ActionClearFilters;
}
