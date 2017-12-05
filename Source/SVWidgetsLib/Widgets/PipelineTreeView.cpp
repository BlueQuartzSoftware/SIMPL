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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeView::~PipelineTreeView() = default;

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
void PipelineTreeView::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton)
  {
    m_StartPos = event->pos();
  }

  QModelIndex index = indexAt(event->pos());

  if(index.isValid() == false)
  {
    // Deselect the current item
    clearSelection();
    clearFocus();
    setCurrentIndex(QModelIndex());
  }

  QTreeView::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::mouseMoveEvent(QMouseEvent* event)
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();

  if(event->buttons() & Qt::LeftButton)
  {
    QModelIndex index = model->index(currentIndex().row(), PipelineTreeItem::Name, currentIndex().parent());
    bool itemHasErrors = model->data(index, Qt::UserRole).value<bool>();
    int distance = (event->pos() - m_StartPos).manhattanLength();
    if(distance >= QApplication::startDragDistance() && itemHasErrors == false)
    {
      performDrag();
    }
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

  QAction* actionAddBookmark = menuItems->getActionAddBookmark();
  QAction* actionNewFolder = menuItems->getActionNewFolder();
  QAction* actionRenameBookmark = menuItems->getActionRenameBookmark();
  QAction* actionRemoveBookmark = menuItems->getActionRemoveBookmark();
  QAction* actionLocateFile = menuItems->getActionLocateFile();
  QAction* actionShowBookmarkInFileSystem = menuItems->getActionShowBookmarkInFileSystem();
  QAction* actionOpenBookmark = menuItems->getActionOpenBookmark();
  QAction* actionExecuteBookmark = menuItems->getActionOpenExecuteBookmark();

  QModelIndexList indexList = selectionModel()->selectedRows(PipelineTreeItem::Name);

  QMenu menu;
  if(index.isValid() == false)
  {
    menu.addAction(actionAddBookmark);
    {
      QAction* separator = new QAction(this);
      separator->setSeparator(true);
      menu.addAction(separator);
    }
    menu.addAction(actionNewFolder);
  }
  else
  {
    QModelIndex actualIndex = model->index(index.row(), PipelineTreeItem::Path, index.parent());
    QString path = actualIndex.data().toString();
    if(indexList.size() > 1)
    {
      actionRemoveBookmark->setText("Remove Items");
      menu.addAction(actionRemoveBookmark);
    }
    else if(path.isEmpty() == false)
    {
      bool itemHasErrors = model->data(actualIndex, Qt::UserRole).value<bool>();
      if(itemHasErrors == true)
      {
        menu.addAction(actionLocateFile);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        actionRemoveBookmark->setText("Remove Bookmark");
        menu.addAction(actionRemoveBookmark);
      }
      else
      {
        menu.addAction(actionOpenBookmark);
        menu.addAction(actionExecuteBookmark);
        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }
        actionRenameBookmark->setText("Rename Bookmark");
        menu.addAction(actionRenameBookmark);
        actionRemoveBookmark->setText("Remove Bookmark");
        menu.addAction(actionRemoveBookmark);
        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        menu.addAction(actionShowBookmarkInFileSystem);
      }
    }
    else if(path.isEmpty())
    {
      menu.addAction(actionAddBookmark);

      actionRenameBookmark->setText("Rename Folder");
      menu.addAction(actionRenameBookmark);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      actionRemoveBookmark->setText("Remove Folder");
      menu.addAction(actionRemoveBookmark);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      menu.addAction(actionNewFolder);
    }
  }

  menu.exec(mapped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::on_actionLocateFile_triggered()
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();

  QModelIndex current = currentIndex();

  QModelIndex nameIndex = model->index(current.row(), PipelineTreeItem::Name, current.parent());
  QModelIndex pathIndex = model->index(current.row(), PipelineTreeItem::Path, current.parent());

  QFileInfo fi(pathIndex.data().toString());
  QString restrictions;
  if(fi.completeSuffix() == "json")
  {
    restrictions = "Json File (*.json)";
  }
  else if(fi.completeSuffix() == "dream3d")
  {
    restrictions = "Dream3d File(*.dream3d)";
  }
  else if(fi.completeSuffix() == "txt")
  {
    restrictions = "Text File (*.txt)";
  }
  else
  {
    restrictions = "Ini File (*.ini)";
  }

  QString filePath = QFileDialog::getOpenFileName(this, tr("Locate Pipeline File"), pathIndex.data().toString(), tr(restrictions.toStdString().c_str()));
  if(true == filePath.isEmpty())
  {
    return;
  }

  filePath = QDir::toNativeSeparators(filePath);

  // Set the new path into the item
  model->setData(pathIndex, filePath, Qt::DisplayRole);

  // Change item back to default look and functionality
  model->setData(nameIndex, false, Qt::UserRole);
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
void PipelineTreeView::performDrag()
{
  m_ActiveIndexBeingDragged = QPersistentModelIndex(currentIndex());

  m_IndexesBeingDragged.clear();

  QModelIndexList list = selectionModel()->selectedRows();

  // We need to filter out all indexes that already have parents/ancestors selected
  list = filterOutDescendants(list);

  for(int i = 0; i < list.size(); i++)
  {
    m_IndexesBeingDragged.push_back(list[i]);
  }

  PipelineTreeModel* model = PipelineTreeModel::Instance();
  QJsonObject obj;
  for(int i = 0; i < m_IndexesBeingDragged.size(); i++)
  {
    QModelIndex draggedIndex = m_IndexesBeingDragged[i];
    QString name = model->index(draggedIndex.row(), PipelineTreeItem::Name, draggedIndex.parent()).data(Qt::DisplayRole).toString();
    QString path = model->index(draggedIndex.row(), PipelineTreeItem::Path, draggedIndex.parent()).data(Qt::DisplayRole).toString();
    if(path.isEmpty() == false)
    {
      obj[name] = path;
    }
  }

  QJsonDocument doc(obj);
  QByteArray jsonArray = doc.toJson();

  QMimeData* mimeData = new QMimeData;
  mimeData->setData(SIMPLView::DragAndDrop::BookmarkItem, jsonArray);

  QDrag* drag = new QDrag(this);
  drag->setMimeData(mimeData);
  drag->exec(Qt::CopyAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::dragEnterEvent(QDragEnterEvent* event)
{
  PipelineTreeView* source = qobject_cast<PipelineTreeView*>(event->source());
  if(source && source != this)
  {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  }
  else
  {
    event->acceptProposedAction();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::dragLeaveEvent(QDragLeaveEvent* event)
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();

  if(m_TopLevelItemPlaceholder.isValid())
  {
    model->removeRow(model->rowCount() - 1, rootIndex());
    m_TopLevelItemPlaceholder = QModelIndex();
  }

  clearSelection();

  setCurrentIndex(m_ActiveIndexBeingDragged);

  for(int i = 0; i < m_IndexesBeingDragged.size(); i++)
  {
    selectionModel()->select(m_IndexesBeingDragged[i], QItemSelectionModel::Select);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::dragMoveEvent(QDragMoveEvent* event)
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();
  int topLevelPHPos = model->rowCount();

  QModelIndex index = indexAt(event->pos());
  if(index.isValid() == false || index.row() == m_TopLevelItemPlaceholder.row())
  {
    if(m_TopLevelItemPlaceholder.isValid() == false)
    {
      clearSelection();
      blockSignals(true);
      model->insertRow(topLevelPHPos, rootIndex());
      m_TopLevelItemPlaceholder = model->index(topLevelPHPos, 0, rootIndex());
      model->setData(m_TopLevelItemPlaceholder, PipelineTreeItem::TopLevelString(), Qt::DisplayRole);
      setCurrentIndex(m_TopLevelItemPlaceholder);
      blockSignals(false);
    }
  }
  else if(index.isValid() && index.row() != m_TopLevelItemPlaceholder.row())
  {
    if(m_TopLevelItemPlaceholder.isValid())
    {
      model->removeRow(topLevelPHPos - 1, rootIndex());
      m_TopLevelItemPlaceholder = QModelIndex();
    }
    clearSelection();

    if(model->flags(index).testFlag(Qt::ItemIsDropEnabled) == true)
    {
      setCurrentIndex(index);
    }
    else
    {
      // Set the current index back to the index being dragged, but don't highlight it
      selectionModel()->setCurrentIndex(m_ActiveIndexBeingDragged, QItemSelectionModel::NoUpdate);
    }
  }

  PipelineTreeView* source = qobject_cast<PipelineTreeView*>(event->source());
  if(source && source != this)
  {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  }
  else
  {
    event->acceptProposedAction();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeView::dropEvent(QDropEvent* event)
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();

  const QMimeData* mimedata = event->mimeData();
  if(mimedata->hasFormat(SIMPLView::DragAndDrop::BookmarkItem))
  {
    QPersistentModelIndex newParent = model->index(currentIndex().row(), PipelineTreeItem::Name, currentIndex().parent());

    // Don't count the destination item in the list of dragged items (if it happens to be in there)
    if(m_IndexesBeingDragged.contains(newParent) == true)
    {
      m_IndexesBeingDragged.removeAt(m_IndexesBeingDragged.indexOf(newParent));
    }

    if(model->flags(newParent).testFlag(Qt::ItemIsDropEnabled) == true)
    {
      if(m_TopLevelItemPlaceholder.isValid())
      {
        // If the parent is the placeholder, change the parent to the root.
        if(m_TopLevelItemPlaceholder == newParent)
        {
          newParent = QModelIndex();
        }

        model->removeRow(model->rowCount() - 1, rootIndex());
        m_TopLevelItemPlaceholder = QModelIndex();
      }

      /* Check to make sure that all selected indexes can be moved before moving them.
           This is where we check for cases where a selected index is trying to be moved
           into the child of that selected index, which, according to a common tree
           structure and implementation, should not be allowed */

      if(newParent != QModelIndex())
      {
        QModelIndex testIndex = newParent.parent();
        while(testIndex.isValid() == true)
        {
          for(int i = m_IndexesBeingDragged.size() - 1; i >= 0; i--)
          {
            if(testIndex == m_IndexesBeingDragged[i])
            {
              QMessageBox::critical(this, "Bookmarks Error", "Cannot move bookmarks.\nThe destination folder \"" + newParent.data(PipelineTreeItem::Name).toString() +
                                                                 "\" is a subfolder of the source folder \"" + m_IndexesBeingDragged[i].data(PipelineTreeItem::Name).toString() + "\".",
                                    QMessageBox::Ok, QMessageBox::Ok);
              return;
            }
          }
          testIndex = testIndex.parent();
        }
      }

      int insertRow = model->rowCount(newParent);
      for(int i = m_IndexesBeingDragged.size() - 1; i >= 0; i--)
      {
        QPersistentModelIndex index = m_IndexesBeingDragged[i];
        QModelIndex oldParent = index.parent();

        if(index.isValid())
        {
          model->moveRow(oldParent, index.row(), newParent, insertRow);
        }
      }

      expand(newParent);
      model->sort(PipelineTreeItem::Name, Qt::AscendingOrder);
      event->accept();
      return;
    }
  }
  else if(mimedata->hasUrls() || mimedata->hasText())
  {
    QByteArray dropData = mimedata->data("text/plain");
    QString data(dropData);

    QList<QString> paths;
    if(mimedata->hasUrls())
    {
      QList<QUrl> urls = mimedata->urls();
      for(int i = 0; i < urls.size(); i++)
      {
        paths.push_back(urls[i].toLocalFile());
      }
    }
    else
    {
      paths.push_back(data);
    }

    QModelIndex parentIndex = currentIndex();
    if(m_TopLevelItemPlaceholder.isValid())
    {
      if(parentIndex == m_TopLevelItemPlaceholder)
      {
        parentIndex = rootIndex();
      }

      model->removeRow(model->rowCount() - 1, rootIndex());
      m_TopLevelItemPlaceholder = QModelIndex();
    }

    for(int i = 0; i < paths.size(); i++)
    {
      model->addFileToTree(paths[i], parentIndex);
      expand(parentIndex);
    }

    model->sort(PipelineTreeItem::Name, Qt::AscendingOrder);
    event->accept();
    return;
  }

  event->ignore();
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
  QString path = model->index(currentIndex.row(), PipelineTreeItem::Path, currentIndex.parent()).data().toString();

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
  else
  {
    obj.insert("Path", path);
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

  QStringList paths = model->getFilePaths();
  if(!paths.isEmpty())
  {
    model->getFileSystemWatcher()->addPaths(paths);
  }
  connect(model->getFileSystemWatcher(), SIGNAL(fileChanged(const QString&)), model, SLOT(updateRowState(const QString&)));

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
  QModelIndex pathIndex = model->index(row, PipelineTreeItem::Path, parentIndex);

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
  model->setData(pathIndex, path, Qt::DisplayRole);
  model->setNeedsToBeExpanded(nameIndex, expanded);
  model->setNeedsToBeExpanded(pathIndex, expanded);

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
