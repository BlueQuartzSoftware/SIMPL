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

#include "BookmarksTreeView.h"

#include <iostream>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>

#include <QtWidgets/QHeaderView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/BookmarksItemDelegate.h"
#include "SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksTreeView::BookmarksTreeView(QWidget* parent)
: QTreeView(parent)
, m_ActiveIndexBeingDragged(QPersistentModelIndex())
, m_TopLevelItemPlaceholder(QModelIndex())
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(requestContextMenu(const QPoint&)));

  connect(this, &BookmarksTreeView::collapsed, this, &BookmarksTreeView::collapseIndex);

  connect(this, &BookmarksTreeView::expanded, this, &BookmarksTreeView::expandIndex);

  BookmarksItemDelegate* dlg = new BookmarksItemDelegate(this);
  setItemDelegate(dlg);
  
  setAttribute(Qt::WA_MacShowFocusRect, false);
  
#if defined(Q_OS_WIN)
  m_ActionShowBookmarkInFileSystem->setText("Show in Windows Explorer");
#elif defined(Q_OS_MAC)
  m_ActionShowBookmarkInFileSystem->setText("Show in Finder");
#else
  m_ActionShowBookmarkInFileSystem->setText("Show in File System");
#endif
  
  m_ActionAddBookmark->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
  m_ActionAddBookmarkFolder->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));

  connect(m_ActionAddBookmark, &QAction::triggered, this, &BookmarksTreeView::listenAddBookmarkTriggered);
  connect(m_ActionAddBookmarkFolder, &QAction::triggered, this, &BookmarksTreeView::listenAddBookmarkFolderTriggered);
  connect(m_ActionShowBookmarkInFileSystem, &QAction::triggered, this, &BookmarksTreeView::listenShowBookmarkInFileSystemTriggered);
  connect(m_ActionRenameBookmark, &QAction::triggered, this, &BookmarksTreeView::listenRenameBookmarkTriggered);
  connect(m_ActionRemoveBookmark, &QAction::triggered, this, &BookmarksTreeView::listenRemoveBookmarkTriggered);
  connect(m_ActionOpenBookmark, &QAction::triggered, this, &BookmarksTreeView::listenOpenBookmarkTriggered);
  connect(m_ActionExecuteBookmark, &QAction::triggered, this, &BookmarksTreeView::listenExecuteBookmarkTriggered);
  connect(m_ActionClearBookmarks, &QAction::triggered, this, &BookmarksTreeView::listenClearBookmarksTriggered);
  connect(m_ActionLocateFile, &QAction::triggered, this, &BookmarksTreeView::locateBookmarkTriggered);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksTreeView::~BookmarksTreeView() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::addBookmark(const QString& filePath, const QModelIndex& parent)
{
  BookmarksModel* model = BookmarksModel::Instance();
  QFileInfo fi(filePath);
  QString fileTitle = fi.baseName();
  QModelIndex index = model->addTreeItem(parent, fileTitle, QIcon(":/SIMPL/icons/images/bookmark.png"), filePath, model->rowCount(parent), BookmarksItem::ItemType::Bookmark, false);
  if(index.isValid())
  {
    emit updateStatusBar("The pipeline '" + fileTitle + "' has been added successfully.");
    expand(parent);
  }
  else
  {
    emit updateStatusBar("The pipeline '" + fileTitle + "' could not be added, because the pipeline file extension was not recognized.");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::listenAddBookmarkTriggered()
{
  QString proposedDir = m_OpenDialogLastFilePath;
  QList<QString> newPrefPaths;

  newPrefPaths =
      QFileDialog::getOpenFileNames(this, tr("Choose Pipeline File(s)"), proposedDir, tr("Json File (*.json);;DREAM3D File (*.dream3d);;Text File (*.txt);;Ini File (*.ini);;All Files (*.*)"));
  if(true == newPrefPaths.isEmpty())
  {
    return;
  }

  QModelIndex parent = currentIndex();

  if(parent.isValid() == false)
  {
    parent = QModelIndex();
  }

  for(int i = 0; i < newPrefPaths.size(); i++)
  {
    QString newPrefPath = newPrefPaths[i];
    newPrefPath = QDir::toNativeSeparators(newPrefPath);
    addBookmark(newPrefPath, parent);
  }

  if(newPrefPaths.size() > 0)
  {
    // Cache the directory from the last path added
    m_OpenDialogLastFilePath = newPrefPaths[newPrefPaths.size() - 1];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::listenAddBookmarkFolderTriggered()
{
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndex parent = currentIndex();

  if(parent.isValid() == false || static_cast<BookmarksItem::ItemType>(model->data(parent, BookmarksModel::Roles::ItemTypeRole).toInt()) == BookmarksItem::ItemType::Bookmark)
  {
    parent = QModelIndex();
  }

  emit raiseBookmarksWidget();

  QString name = "New Folder";

  QModelIndex index = model->addTreeItem(parent, name, QIcon(":/SIMPL/icons/images/folder_blue.png"), "", model->rowCount(parent), BookmarksItem::ItemType::Folder, false);
  expandIndex(parent);
  setCurrentIndex(index);
  scrollTo(index);
  edit(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::listenShowBookmarkInFileSystemTriggered()
{
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndex index = currentIndex();
  if(index.isValid())
  {
    QString pipelinePath = model->data(index, static_cast<int>(BookmarksModel::Roles::PathRole)).toString();

    QFileInfo fi(pipelinePath);
    QString path;
    if(fi.isFile())
    {
      path = fi.absoluteFilePath();
    }
    else
    {
      path = fi.absolutePath();
    }

    QtSFileUtils::ShowPathInGui(this, path);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::listenRenameBookmarkTriggered()
{
  edit(currentIndex());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::listenRemoveBookmarkTriggered()
{
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndexList indexList = selectionModel()->selectedRows(BookmarksItem::Contents);
  qSort(indexList);

  indexList = filterOutDescendants(indexList);

  if(indexList.size() <= 0)
  {
    return;
  }

  QList<QPersistentModelIndex> persistentList;
  for(int i = 0; i < indexList.size(); i++)
  {
    persistentList.push_back(indexList[i]);
  }

  QModelIndex singleIndex = model->index(indexList[0].row(), BookmarksItem::Contents, indexList[0].parent());

  QMessageBox msgBox;
  if(indexList.size() > 1)
  {
    msgBox.setWindowTitle("Remove Bookmark Items");
    msgBox.setText("Are you sure that you want to remove these bookmark items? The original bookmark files will not be removed.");
  }
  else if(model->flags(singleIndex).testFlag(Qt::ItemIsDropEnabled) == false)
  {
    QString name = model->data(singleIndex, Qt::DisplayRole).toString();
    msgBox.setWindowTitle("Remove Bookmark");
    msgBox.setText("Are you sure that you want to remove the bookmark \"" + name + "\"? The original file will not be removed.");
  }
  else
  {
    QString name = model->data(singleIndex, Qt::DisplayRole).toString();
    msgBox.setWindowTitle("Remove Folder");
    msgBox.setText("Are you sure that you want to remove the folder \"" + name + "\"? The folder's contents will also be removed.");
  }
  msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  msgBox.setDefaultButton(QMessageBox::Yes);
  int ret = msgBox.exec();

  if(ret == QMessageBox::Yes)
  {
    for(int i = 0; i < persistentList.size(); i++)
    {
      QModelIndex index = model->index(persistentList[i].row(), BookmarksItem::Contents, persistentList[i].parent());
      QString name = model->data(index, Qt::DisplayRole).toString();

      // Remove bookmark from the SIMPLView interface
      model->removeRow(persistentList[i].row(), persistentList[i].parent());
    }

    // Write these changes out to the preferences file
    emit fireWriteSettings();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::listenOpenBookmarkTriggered()
{
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndexList indexList = selectionModel()->selectedRows(BookmarksItem::Contents);
  qSort(indexList);

  QModelIndex index = indexList.at(0);

  QString pipelinePath = model->data(index, static_cast<int>(BookmarksModel::Roles::PathRole)).toString();
  if(pipelinePath.isEmpty())
  {
    return; // The user double clicked a folder, so don't do anything
  }
  else
  {
    QFileInfo fi(pipelinePath);
    if(fi.exists())
    {
      emit newSIMPLViewInstanceTriggered(pipelinePath);

      // Cache the last directory on old instance
      m_OpenDialogLastFilePath = pipelinePath;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::listenExecuteBookmarkTriggered()
{
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndexList indexList = selectionModel()->selectedRows(BookmarksItem::Contents);
  qSort(indexList);

  QModelIndex index = indexList.at(0);

  QString pipelinePath = model->data(index, static_cast<int>(BookmarksModel::Roles::PathRole)).toString();
  if(pipelinePath.isEmpty())
  {
    return; // The user double clicked a folder, so don't do anything
  }
  else
  {
    QFileInfo fi(pipelinePath);
    if(fi.exists())
    {
      emit newSIMPLViewInstanceTriggered(pipelinePath, true);

      // Cache the last directory on old instance
      m_OpenDialogLastFilePath = pipelinePath;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::listenClearBookmarksTriggered()
{
  QMessageBox msgBox;
  QString title = QString("Clear %1 Bookmarks").arg(QApplication::applicationName());
  msgBox.setWindowTitle(title);
  msgBox.setText(QString("Are you sure that you want to clear all %1 bookmarks?").arg(QApplication::applicationName()));

  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::Yes);
  int response = msgBox.exec();

  if(response == QMessageBox::Yes)
  {
    BookmarksModel* model = BookmarksModel::Instance();
    if(model->isEmpty() == false)
    {
      model->removeRows(0, model->rowCount(QModelIndex()));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::addActionList(QList<QAction*> actionList)
{
  for(int i = 0; i < actionList.size(); i++)
  {
    m_Menu.addAction(actionList[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::mousePressEvent(QMouseEvent* event)
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
void BookmarksTreeView::mouseMoveEvent(QMouseEvent* event)
{
  BookmarksModel* model = BookmarksModel::Instance();

  if(model != nullptr && event->buttons() & Qt::LeftButton)
  {
    QModelIndex index = model->index(currentIndex().row(), BookmarksItem::Contents, currentIndex().parent());
    bool itemHasErrors = model->data(index, Qt::UserRole).value<bool>();
    int distance = (event->pos() - m_StartPos).manhattanLength();
    if(distance >= QApplication::startDragDistance() && itemHasErrors == false)
    {
      performDrag();
    }
  }

  QTreeView::mouseMoveEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::requestContextMenu(const QPoint& pos)
{
  activateWindow();

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

  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndexList indexList = selectionModel()->selectedRows(BookmarksItem::Contents);
  qSort(indexList);

  QMenu menu;
  if(index.isValid() == false)
  {
    menu.addAction(m_ActionAddBookmark);
    {
      QAction* separator = new QAction(this);
      separator->setSeparator(true);
      menu.addAction(separator);
    }
    menu.addAction(m_ActionAddBookmarkFolder);
  }
  else
  {
    QString path = model->data(index, static_cast<int>(BookmarksModel::Roles::PathRole)).toString();
    if(indexList.size() > 1)
    {
      m_ActionRemoveBookmark->setText("Remove Items");
      menu.addAction(m_ActionRemoveBookmark);
    }
    else if(path.isEmpty() == false)
    {
      bool itemHasErrors = model->data(index, static_cast<int>(BookmarksModel::Roles::ErrorsRole)).toBool();
      if(itemHasErrors == true)
      {
        menu.addAction(m_ActionLocateFile);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        m_ActionRemoveBookmark->setText("Remove Bookmark");
        menu.addAction(m_ActionRemoveBookmark);
      }
      else
      {
        menu.addAction(m_ActionOpenBookmark);
        menu.addAction(m_ActionExecuteBookmark);
        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }
        m_ActionRenameBookmark->setText("Rename Bookmark");
        menu.addAction(m_ActionRenameBookmark);
        m_ActionRemoveBookmark->setText("Remove Bookmark");
        menu.addAction(m_ActionRemoveBookmark);
        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        menu.addAction(m_ActionShowBookmarkInFileSystem);
      }
    }
    else if(path.isEmpty())
    {
      menu.addAction(m_ActionAddBookmark);

      m_ActionRenameBookmark->setText("Rename Folder");
      menu.addAction(m_ActionRenameBookmark);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      m_ActionRemoveBookmark->setText("Remove Folder");
      menu.addAction(m_ActionRemoveBookmark);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      menu.addAction(m_ActionAddBookmarkFolder);
    }
  }

  menu.exec(mapped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList BookmarksTreeView::filterOutDescendants(QModelIndexList indexList)
{
  BookmarksModel* model = BookmarksModel::Instance();

  for(int i = indexList.size() - 1; i >= 0; i--)
  {
    QPersistentModelIndex index = indexList[i];
    QString name = model->data(index, Qt::DisplayRole).toString();
    // Walk up the tree from the index...if an ancestor is selected, remove the index
    while(index.isValid() == true)
    {
      QPersistentModelIndex parent = index.parent();
      QString parentName = model->data(parent, Qt::DisplayRole).toString();
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
void BookmarksTreeView::performDrag()
{
  m_ActiveIndexBeingDragged = QPersistentModelIndex(currentIndex());

  m_IndexesBeingDragged.clear();

  QModelIndexList list = selectionModel()->selectedRows();
  qSort(list);

  // We need to filter out all indexes that already have parents/ancestors selected
  list = filterOutDescendants(list);

  for(int i = 0; i < list.size(); i++)
  {
    m_IndexesBeingDragged.push_back(list[i]);
  }

  BookmarksModel* model = BookmarksModel::Instance();
  QJsonObject obj;
  for(int i = 0; i < m_IndexesBeingDragged.size(); i++)
  {
    QModelIndex draggedIndex = m_IndexesBeingDragged[i];
    QString name = model->data(draggedIndex, Qt::DisplayRole).toString();
    QString path = model->data(draggedIndex, static_cast<int>(BookmarksModel::Roles::PathRole)).toString();
    if(path.isEmpty() == false)
    {
      obj[name] = path;
    }
  }

  QJsonDocument doc(obj);
  QByteArray jsonArray = doc.toJson();

  QMimeData* mimeData = new QMimeData;
  mimeData->setData(SIMPLView::DragAndDrop::BookmarkItem, jsonArray);

  QString dragText = "";
  if (m_IndexesBeingDragged.size() == 1)
  {
    BookmarksItem::ItemType type = static_cast<BookmarksItem::ItemType>(model->data(m_IndexesBeingDragged[0], static_cast<int>(BookmarksModel::Roles::ItemTypeRole)).toInt());
    if (type == BookmarksItem::ItemType::Bookmark)
    {
      dragText = model->data(m_IndexesBeingDragged[0], Qt::DisplayRole).toString();
    }
  }
  else
  {
    dragText = "Multiple Bookmarks Items";
  }

  QDrag* drag = new QDrag(this);

  QColor backgroundColor = QColor(Qt::gray);
  QPixmap dragIcon = DataArrayPathSelectionWidget::CreateDragIcon(dragText, backgroundColor);

  drag->setPixmap(dragIcon);
  drag->setMimeData(mimeData);
  drag->exec(Qt::CopyAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::dragEnterEvent(QDragEnterEvent* event)
{
  BookmarksTreeView* source = qobject_cast<BookmarksTreeView*>(event->source());
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
void BookmarksTreeView::dragLeaveEvent(QDragLeaveEvent* event)
{
  BookmarksModel* model = BookmarksModel::Instance();

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
void BookmarksTreeView::dragMoveEvent(QDragMoveEvent* event)
{
  BookmarksModel* model = BookmarksModel::Instance();
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
      model->setData(m_TopLevelItemPlaceholder, BookmarksItem::TopLevelString(), Qt::DisplayRole);
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

  BookmarksTreeView* source = qobject_cast<BookmarksTreeView*>(event->source());
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
void BookmarksTreeView::dropEvent(QDropEvent* event)
{
  BookmarksModel* model = BookmarksModel::Instance();

  const QMimeData* mimedata = event->mimeData();
  if(mimedata->hasFormat(SIMPLView::DragAndDrop::BookmarkItem))
  {
    QPersistentModelIndex newParent = model->index(currentIndex().row(), BookmarksItem::Contents, currentIndex().parent());

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
              QString newParentName = model->data(newParent, Qt::DisplayRole).toString();
              QString draggedIndexName = model->data(m_IndexesBeingDragged[i], Qt::DisplayRole).toString();
              QMessageBox::critical(this, "Bookmarks Error", "Cannot move bookmarks.\nThe destination folder \"" + newParentName +
                                                                 "\" is a subfolder of the source folder \"" + draggedIndexName + "\".",
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

        if(index.isValid() && oldParent != newParent)
        {
          model->moveRow(oldParent, index.row(), newParent, insertRow);
        }
      }

      expand(newParent);
      model->sort(BookmarksItem::Contents, Qt::AscendingOrder);
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

    model->sort(BookmarksItem::Contents, Qt::AscendingOrder);
    event->accept();
    return;
  }

  event->ignore();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::setModel(QAbstractItemModel* model)
{
  // Set the model
  QTreeView::setModel(model);

  BookmarksModel* bModel = qobject_cast<BookmarksModel*>(model);

  // Expand indexes that have their expand member set to true
  expandChildren(QModelIndex(), bModel);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::expandChildren(const QModelIndex& parent, BookmarksModel* model)
{
  for(int row = 0; row < model->rowCount(parent); row++)
  {
    QModelIndex index = model->index(row, 0, parent);
    bool expanded = model->data(index, static_cast<int>(BookmarksModel::Roles::ExpandedRole)).toBool();
    if(expanded)
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
void BookmarksTreeView::collapseIndex(const QModelIndex& index)
{
  if(index.isValid())
  {
    BookmarksModel* model = BookmarksModel::Instance();

    QTreeView::collapse(index);
    model->setData(index, false, static_cast<int>(BookmarksModel::Roles::ExpandedRole));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::expandIndex(const QModelIndex& index)
{
  BookmarksModel* model = BookmarksModel::Instance();

  QTreeView::expand(index);
  model->setData(index, true, static_cast<int>(BookmarksModel::Roles::ExpandedRole));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  emit currentIndexChanged(current, previous);
}
