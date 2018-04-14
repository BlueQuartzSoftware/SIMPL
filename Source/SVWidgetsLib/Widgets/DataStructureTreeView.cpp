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

#include "DataStructureTreeView.h"

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
#include "SVWidgetsLib/Widgets/DataStructureItemDelegate.h"
#include "SVWidgetsLib/Widgets/SIMPLViewMenuItems.h"
#include "SVWidgetsLib/Widgets/SIMPLViewToolbox.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureTreeView::DataStructureTreeView(QWidget* parent)
: QTreeView(parent)
, m_ActiveIndexBeingDragged(QPersistentModelIndex())
, m_TopLevelItemPlaceholder(QModelIndex())
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(requestContextMenu(const QPoint&)));

  connect(this, SIGNAL(collapsed(const QModelIndex&)), SLOT(collapseIndex(const QModelIndex&)));
  connect(this, SIGNAL(expanded(const QModelIndex&)), SLOT(expandIndex(const QModelIndex&)));

  DataStructureItemDelegate* dlg = new DataStructureItemDelegate(this);
  setItemDelegate(dlg);

  m_Model = new DataStructureModel(this);
  this->setModel(m_Model);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureTreeView::~DataStructureTreeView()
{
  if (m_Model != nullptr)
  {
    delete m_Model;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::addActionList(QList<QAction*> actionList)
{
  for(int i = 0; i < actionList.size(); i++)
  {
    m_Menu.addAction(actionList[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::mousePressEvent(QMouseEvent* event)
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
void DataStructureTreeView::mouseMoveEvent(QMouseEvent* event)
{


  if(event->buttons() & Qt::LeftButton)
  {
    QModelIndex index = m_Model->index(currentIndex().row(), DataStructureItem::Name, currentIndex().parent());
    bool itemHasErrors = m_Model->data(index, Qt::UserRole).value<bool>();
    int distance = (event->pos() - m_StartPos).manhattanLength();
    if(distance >= QApplication::startDragDistance() && itemHasErrors == false)
    {
//      performDrag();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::requestContextMenu(const QPoint& pos)
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


  //TODO: to be filled out if needed
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList DataStructureTreeView::filterOutDescendants(QModelIndexList indexList)
{

  for(int i = indexList.size() - 1; i >= 0; i--)
  {
    QPersistentModelIndex index = indexList[i];
    QString name = m_Model->index(index.row(), DataStructureItem::Name, index.parent()).data().toString();
    // Walk up the tree from the index...if an ancestor is selected, remove the index
    while(index.isValid() == true)
    {
      QPersistentModelIndex parent = index.parent();
      QString parentName = m_Model->index(parent.row(), DataStructureItem::Name, parent.parent()).data().toString();
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

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::performDrag()
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


  QJsonObject obj;
  for(int i = 0; i < m_IndexesBeingDragged.size(); i++)
  {
    QModelIndex draggedIndex = m_IndexesBeingDragged[i];
    QString name = m_Model->index(draggedIndex.row(), DataStructureItem::Name, draggedIndex.parent()).data(Qt::DisplayRole).toString();
    QString path = m_Model->index(draggedIndex.row(), DataStructureItem::Path, draggedIndex.parent()).data(Qt::DisplayRole).toString();
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
void DataStructureTreeView::dragEnterEvent(QDragEnterEvent* event)
{
  DataStructureTreeView* source = qobject_cast<DataStructureTreeView*>(event->source());
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
void DataStructureTreeView::dragLeaveEvent(QDragLeaveEvent* event)
{


  if(m_TopLevelItemPlaceholder.isValid())
  {
    m_Model->removeRow(m_Model->rowCount() - 1, rootIndex());
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
void DataStructureTreeView::dragMoveEvent(QDragMoveEvent* event)
{

  int topLevelPHPos = m_Model->rowCount();

  QModelIndex index = indexAt(event->pos());
  if(index.isValid() == false || index.row() == m_TopLevelItemPlaceholder.row())
  {
    if(m_TopLevelItemPlaceholder.isValid() == false)
    {
      clearSelection();
      blockSignals(true);
      m_Model->insertRow(topLevelPHPos, rootIndex());
      m_TopLevelItemPlaceholder = m_Model->index(topLevelPHPos, 0, rootIndex());
      m_Model->setData(m_TopLevelItemPlaceholder, DataStructureItem::TopLevelString(), Qt::DisplayRole);
      setCurrentIndex(m_TopLevelItemPlaceholder);
      blockSignals(false);
    }
  }
  else if(index.isValid() && index.row() != m_TopLevelItemPlaceholder.row())
  {
    if(m_TopLevelItemPlaceholder.isValid())
    {
      m_Model->removeRow(topLevelPHPos - 1, rootIndex());
      m_TopLevelItemPlaceholder = QModelIndex();
    }
    clearSelection();

    if(m_Model->flags(index).testFlag(Qt::ItemIsDropEnabled) == true)
    {
      setCurrentIndex(index);
    }
    else
    {
      // Set the current index back to the index being dragged, but don't highlight it
      selectionModel()->setCurrentIndex(m_ActiveIndexBeingDragged, QItemSelectionModel::NoUpdate);
    }
  }

  DataStructureTreeView* source = qobject_cast<DataStructureTreeView*>(event->source());
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
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setModel(QAbstractItemModel* m_Model)
{
  // Set the m_Model
  QTreeView::setModel(m_Model);

  DataStructureModel* bModel = qobject_cast<DataStructureModel*>(m_Model);

  // Expand indexes that have their expand member set to true
  expandChildren(QModelIndex(), bModel);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::expandChildren(const QModelIndex& parent, DataStructureModel* m_Model)
{
  for(int row = 0; row < m_Model->rowCount(parent); row++)
  {
    QModelIndex index = m_Model->index(row, 0, parent);
    if(m_Model->needsToBeExpanded(index))
    {
      expand(index);
    }

    // Recursive call, to expand all items in the tree
    expandChildren(index, m_Model);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::collapseIndex(const QModelIndex& index)
{
  if(index.isValid())
  {

    QModelIndex sibling = m_Model->sibling(0, 0, index);

    QTreeView::collapse(index);
    m_Model->setNeedsToBeExpanded(index, false);
    m_Model->setNeedsToBeExpanded(sibling, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::expandIndex(const QModelIndex& index)
{
  QModelIndex sibling = m_Model->sibling(0, 0, index);

  QTreeView::expand(index);
  m_Model->setNeedsToBeExpanded(index, true);
  m_Model->setNeedsToBeExpanded(sibling, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  emit currentIndexChanged(current, previous);
}
