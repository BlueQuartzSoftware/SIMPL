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
#include "SVWidgetsLib/Widgets/PipelineTreeItem.h"
#include "SVWidgetsLib/Widgets/SIMPLViewMenuItems.h"
#include "SVWidgetsLib/Widgets/PipelineTreeModel.h"

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

  SIMPLViewMenuItems* menuItems = SIMPLViewMenuItems::Instance();

  QModelIndex index = indexAt(pos);

  QPoint mapped;
  if(index.isValid())
  {
    // Note: We must map the point to global from the viewport to
    // account for the header.
    mapped = viewport()->mapToGlobal(pos);
  }
  else
  {
    index = QModelIndex();
    mapped = mapToGlobal(pos);
  }

  PipelineTreeModel* model = PipelineTreeModel::Instance();

  QModelIndexList indexList = selectionModel()->selectedRows(PipelineTreeItem::Name);

  QMenu menu;
  if(index.isValid() == false)
  {
    menu.addAction(menuItems->getActionPaste());
    menu.addSeparator();
    menu.addAction(menuItems->getActionClearPipeline());
  }
  else
  {
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
      m_ActionEnableFilter->setChecked(widgetEnabled);

      disconnect(m_ActionEnableFilter, &QAction::toggled, 0, 0);

      connect(m_ActionEnableFilter, &QAction::toggled, [=] { setFiltersEnabled(toggledIndices, m_ActionEnableFilter->isChecked()); });
      menu.addAction(m_ActionEnableFilter);
    }
    else
    {
      disconnect(m_ActionEnableFilter, &QAction::toggled, 0, 0);
      m_ActionEnableFilter->setChecked(widgetEnabled);

      connect(m_ActionEnableFilter, &QAction::toggled, [=] { setSelectedFiltersEnabled(m_ActionEnableFilter->isChecked()); });
      menu.addAction(m_ActionEnableFilter);
    }

    menu.addSeparator();

    QAction* removeAction;
    QList<QKeySequence> shortcutList;
    shortcutList.push_back(QKeySequence(Qt::Key_Backspace));
    shortcutList.push_back(QKeySequence(Qt::Key_Delete));

    if (indexList.contains(index) == false || indexList.size() == 1)
    {
      removeAction = new QAction("Delete", &menu);
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

    menu.exec(mapped);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::setFiltersEnabled(QModelIndexList indices, bool enabled)
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();

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
    emit needsPreflight(pipelineIndices[i]);
  }

  emit filterEnabledStateChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::setSelectedFiltersEnabled(bool enabled)
{
  QModelIndexList selectedIndices = selectionModel()->selectedRows(PipelineTreeItem::Name);
  setFiltersEnabled(selectedIndices, enabled);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList PipelineTreeView::filterOutDescendants(QModelIndexList indexList)
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();

  for(int i = indexList.size() - 1; i >= 0; i--)
  {
    QPersistentModelIndex index = indexList[i];
    QString name = model->index(index.row(), PipelineTreeItem::Name, index.parent()).data().toString();
    // Walk up the tree from the index...if an ancestor is selected, remove the index
    while(index.isValid() == true)
    {
      QPersistentModelIndex parent = index.parent();
      QString parentName = model->index(parent.row(), PipelineTreeItem::Name, parent.parent()).data().toString();
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

  PipelineTreeModel* bModel = qobject_cast<PipelineTreeModel*>(model);

  // Expand indexes that have their expand member set to true
  expandChildren(QModelIndex(), bModel);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::expandChildren(const QModelIndex& parent, PipelineTreeModel *model)
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
    PipelineTreeModel* model = PipelineTreeModel::Instance();
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
  PipelineTreeModel* model = PipelineTreeModel::Instance();
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
QJsonObject PipelineTreeView::toJsonObject()
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();
  PipelineTreeItem* rootItem = model->getRootItem();

  QJsonObject treeObj;
  for(int i = 0; i < rootItem->childCount(); i++)
  {
    QModelIndex childIndex = model->index(i, PipelineTreeItem::Name, QModelIndex());
    QString name = childIndex.data().toString();

    if(name.compare("Prebuilt Pipelines") != 0)
    {
      QJsonObject childObj = wrapModel(childIndex);
      treeObj[name] = childObj;
    }
  }

  return treeObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject PipelineTreeView::wrapModel(QModelIndex currentIndex)
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();

  QJsonObject obj;

  QString name = model->index(currentIndex.row(), PipelineTreeItem::Name, currentIndex.parent()).data().toString();

  for(int i = 0; i < model->rowCount(currentIndex); i++)
  {
    QModelIndex childIndex = model->index(i, PipelineTreeItem::Name, currentIndex);
    QString childName = childIndex.data().toString();

    QJsonObject childObj = wrapModel(childIndex);
    obj[childName] = childObj;
  }

  if(model->flags(currentIndex).testFlag(Qt::ItemIsDropEnabled) == true)
  {
    obj.insert("Expanded", isExpanded(currentIndex));
  }

  return obj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeModel *PipelineTreeView::FromJsonObject(QJsonObject treeObject)
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();

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
      PipelineTreeView::UnwrapModel(keys[i], val.toObject(), model, QModelIndex());
    }
  }

  return model;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::UnwrapModel(QString objectName, QJsonObject object, PipelineTreeModel* model, QModelIndex parentIndex)
{
  int row = model->rowCount(parentIndex);
  model->insertRow(row, parentIndex);
  QModelIndex nameIndex = model->index(row, PipelineTreeItem::Name, parentIndex);

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
      PipelineTreeView::UnwrapModel(keys[i], val.toObject(), model, nameIndex);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::updateActionEnableFilter()
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();

  QModelIndexList indices = selectionModel()->selectedRows(PipelineTreeItem::Name);

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
