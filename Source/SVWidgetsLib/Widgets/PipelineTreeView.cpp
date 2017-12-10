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
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(requestContextMenu(const QPoint&)));

  connect(this, SIGNAL(collapsed(const QModelIndex&)), SLOT(collapseIndex(const QModelIndex&)));
  connect(this, SIGNAL(expanded(const QModelIndex&)), SLOT(expandIndex(const QModelIndex&)));

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
void PipelineTreeView::requestContextMenu(const QPoint& pos)
{
  activateWindow();

  QModelIndex index = indexAt(pos);

  PipelineModel* model = getPipelineTreeModel();

  QPoint mapped;
  if(model->itemType(index) == PipelineItem::ItemType::Filter)
  {
    mapped = viewport()->mapToGlobal(pos);

    requestFilterContextMenu(mapped, index);
  }
  else if (model->itemType(index) == PipelineItem::ItemType::Pipeline)
  {
    mapped = viewport()->mapToGlobal(pos);

    requestPipelineContextMenu(mapped, index);
  }
  else
  {
    mapped = mapToGlobal(pos);
    requestDefaultContextMenu(mapped);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::requestFilterContextMenu(const QPoint &pos, const QModelIndex &index)
{
  SIMPLViewMenuItems* menuItems = SIMPLViewMenuItems::Instance();
  PipelineModel* model = getPipelineTreeModel();

  QModelIndexList indexList = selectionModel()->selectedRows(PipelineItem::Name);

  QMenu menu;

  menu.addAction(menuItems->getActionCut());
  menu.addAction(menuItems->getActionCopy());
  menu.addSeparator();

  int count = indexList.size();
  bool widgetEnabled = true;

  for(int i = 0; i < count && widgetEnabled; i++)
  {
    AbstractFilter::Pointer filter = model->filter(indexList[i]);
    if (filter != nullptr)
    {
      widgetEnabled = filter->getEnabled();
    }
  }

  if(indexList.contains(index) == false)
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
    connect(m_ActionEnableFilter, &QAction::toggled, [=] { setSelectedFiltersEnabled(m_ActionEnableFilter->isChecked()); });
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

  if (indexList.contains(index) == false || indexList.size() == 1)
  {
    removeAction = new QAction("Delete Filter", &menu);
    connect(removeAction, &QAction::triggered, [=] { model->removeRow(index.row()); });
  }
  else
  {
    removeAction = new QAction(tr("Delete %1 Filters").arg(indexList.size()), &menu);
    connect(removeAction, &QAction::triggered, [=] {
      QList<QPersistentModelIndex> persistentList;
      for(int i = 0; i < indexList.size(); i++)
      {
        persistentList.push_back(indexList[i]);
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
void PipelineTreeView::requestPipelineContextMenu(const QPoint &pos, const QModelIndex &index)
{
  SIMPLViewMenuItems* menuItems = SIMPLViewMenuItems::Instance();
  PipelineModel* model = getPipelineTreeModel();

  QModelIndexList indexList = selectionModel()->selectedRows(PipelineItem::Name);
  if (indexList.size() <= 0)
  {
    return;
  }

  // Build up the contextual menu
  QMenu menu;
  menu.addAction(menuItems->getActionCut());
  menu.addAction(menuItems->getActionCopy());
  menu.addSeparator();

  // Check to see if all the selected items are pipeline items
  bool allPipelines = true;
  for (int i = 0; i < indexList.size(); i++)
  {
    if (model->itemType(indexList[i]) != PipelineItem::ItemType::Pipeline)
    {
      allPipelines = false;
    }
  }

  if (indexList.contains(index) == false || indexList.size() == 1 || allPipelines == false)
  {
    requestSinglePipelineContextMenu(menu, index);
  }
  else
  {
    requestMultiplePipelineContextMenu(menu, indexList);
  }

  menu.exec(pos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::requestSinglePipelineContextMenu(QMenu &menu, const QModelIndex &pipelineIdx)
{
  PipelineModel* model = getPipelineTreeModel();

  if (model->isActivePipeline(pipelineIdx) == false)
  {
    QAction* activePipelineAction = new QAction(tr("Set '%1' as Active Pipeline").arg(model->data(pipelineIdx, Qt::DisplayRole).toString()));
    connect(activePipelineAction, &QAction::triggered, [=] { emit activePipelineChanged(pipelineIdx, getPipelineTreeModel()); });
    menu.addAction(activePipelineAction);
  }

  menu.addSeparator();

  QAction* removeAction = new QAction("Delete Pipeline", &menu);
  connect(removeAction, &QAction::triggered, [=] {
    if (model->isActivePipeline(pipelineIdx))
    {
      // We are trying to delete the active pipeline, so find another pipeline to set as the active pipeline
      QModelIndex nextActivePipeline = findNewActivePipeline(pipelineIdx);
      emit activePipelineChanged(nextActivePipeline, getPipelineTreeModel());
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
void PipelineTreeView::requestMultiplePipelineContextMenu(QMenu &menu, QModelIndexList pipelineIndices)
{
  PipelineModel* model = getPipelineTreeModel();

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
        emit activePipelineChanged(nextActivePipeline, getPipelineTreeModel());
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
QModelIndex PipelineTreeView::findNewActivePipeline(const QModelIndex &oldActivePipeline)
{
  PipelineModel* model = getPipelineTreeModel();

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
void PipelineTreeView::requestDefaultContextMenu(const QPoint &pos)
{
  SIMPLViewMenuItems* menuItems = SIMPLViewMenuItems::Instance();

  QMenu menu;
  menu.addAction(menuItems->getActionNewPipeline());
  menu.addSeparator();
  menu.addAction(menuItems->getActionPaste());
  menu.addSeparator();
  menu.addAction(menuItems->getActionClearPipeline());

  menu.exec(pos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::setFiltersEnabled(QModelIndexList indices, bool enabled)
{
  PipelineModel* model = getPipelineTreeModel();

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
    emit needsPreflight(pipelineIndices[i], getPipelineTreeModel());
  }

  emit filterEnabledStateChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::setSelectedFiltersEnabled(bool enabled)
{
  QModelIndexList selectedIndices = selectionModel()->selectedRows(PipelineItem::Name);
  setFiltersEnabled(selectedIndices, enabled);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList PipelineTreeView::filterOutDescendants(QModelIndexList indexList)
{
  PipelineModel* model = getPipelineTreeModel();

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
    PipelineModel* model = getPipelineTreeModel();
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
  PipelineModel* model = getPipelineTreeModel();
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
  PipelineModel* model = getPipelineTreeModel();

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
PipelineModel* PipelineTreeView::getPipelineTreeModel()
{
  return dynamic_cast<PipelineModel*>(model());
}
