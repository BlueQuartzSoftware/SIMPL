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
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h"
#include "SVWidgetsLib/Widgets/DataStructureItemDelegate.h"
#include "SVWidgetsLib/Widgets/SIMPLViewMenuItems.h"
#include "SVWidgetsLib/Widgets/SIMPLViewToolbox.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureTreeView::DataStructureTreeView(QWidget* parent)
: QTreeView(parent)
{
  //setContextMenuPolicy(Qt::CustomContextMenu);
  //connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(requestContextMenu(const QPoint&)));
  setAcceptDrops(true);
  setMouseTracking(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureTreeView::~DataStructureTreeView() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataStructureTreeView::getDataArrayPath(QModelIndex index)
{
  DataArrayPath path;
  QStandardItemModel* stdModel = dynamic_cast<QStandardItemModel*>(model());
  
  if(nullptr == stdModel)
  {
    return path;
  }

  if(index.isValid())
  {
    QStandardItem* item = stdModel->itemFromIndex(index);
    if(item->parent())
    {
      QStandardItem* parentItem = item->parent();
      if(parentItem->parent())
      {
        path.setDataContainerName(parentItem->parent()->text());
        path.setAttributeMatrixName(parentItem->text());
        path.setDataArrayName(item->text());
      }
      else
      {
        path.setDataContainerName(parentItem->text());
        path.setAttributeMatrixName(item->text());
      }
    }
    else
    {
      path.setDataContainerName(item->text());
    }
  }

  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::emitFilterPath(QModelIndex& index)
{
  if(false == index.isValid())
  {
    emit endPathFiltering();
    return;
  }

  DataArrayPath path = getDataArrayPath(index);
  emit filterPath(path);
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

  QTreeView::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::mouseMoveEvent(QMouseEvent* event)
{
  if(event->buttons() & Qt::LeftButton)
  {
    QModelIndex index = indexAt(m_StartPos);
    int distance = (event->pos() - m_StartPos).manhattanLength();
    if(distance >= QApplication::startDragDistance() && index.isValid())
    {
      performDrag();
    }
  }
  else
  {
    QModelIndex index = indexAt(event->pos());
    emitFilterPath(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::leaveEvent(QEvent* event)
{
  if(m_Dragging)
  {
    return;
  }

  emit endPathFiltering();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::performDrag()
{
  QModelIndex index = indexAt(m_StartPos);
  if(false == index.isValid())
  {
    return;
  }

  DataArrayPath path = getDataArrayPath(index);
  DataArrayPathSelectionWidget::DataType dataType;
  if(false == path.getDataArrayName().isEmpty())
  {
    dataType = DataArrayPathSelectionWidget::DataType::DataArray;
  }
  else if(false == path.getAttributeMatrixName().isEmpty())
  {
    dataType = DataArrayPathSelectionWidget::DataType::AttributeMatrix;
  }
  else
  {
    dataType = DataArrayPathSelectionWidget::DataType::DataContainer;
  }

  QMimeData* mimeData = new QMimeData;
  mimeData->setData(SIMPLView::DragAndDrop::DataArrayPath, path.serialize().toUtf8());
  QPixmap dragIcon = DataArrayPathSelectionWidget::GetDragIcon(dataType);

  m_Dragging = true;
  QDrag* drag = new QDrag(this);
  drag->setMimeData(mimeData);
  drag->setPixmap(dragIcon);
  //drag->setDragCursor(dragIcon, Qt::DropAction::CopyAction);
  drag->exec(Qt::CopyAction);

  // drag->exec is a blocking method
  dragComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::dragComplete()
{
  emit endPathFiltering();
  m_Dragging = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::dragEnterEvent(QDragEnterEvent* event)
{
  if(event->mimeData()->hasFormat(SIMPLView::DragAndDrop::SelectionWidget))
  {
    DataArrayPathSelectionWidget* selectionWidget = dynamic_cast<DataArrayPathSelectionWidget*>(event->source());
    if(selectionWidget)
    {
      event->accept();
      return;
    }
  }

  event->ignore();
}

#if 0
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
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::dragMoveEvent(QDragMoveEvent* event)
{
  // If Source is DataArrayPathSelectionWidget
  if(event->mimeData()->hasFormat(SIMPLView::DragAndDrop::SelectionWidget))
  {
    DataArrayPathSelectionWidget* selectionWidget = dynamic_cast<DataArrayPathSelectionWidget*>(event->source());
    if(selectionWidget)
    {
      // Get the DataArrayPath under the cursor
      QModelIndex index = indexAt(event->pos());
      if(false == index.isValid())
      {
        return;
      }
      DataArrayPath path = getDataArrayPath(index);

      // Check path requirements
      if(selectionWidget->checkPathReqs(path))
      {
        event->accept();
        return;
      }
    }
    // End SelectionWidget
  }

  event->ignore();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::dropEvent(QDropEvent* event)
{
  // If Source is DataArrayPathSelectionWidget
  if(event->mimeData()->hasFormat(SIMPLView::DragAndDrop::SelectionWidget))
  {
    DataArrayPathSelectionWidget* selectionWidget = dynamic_cast<DataArrayPathSelectionWidget*>(event->source());
    if(selectionWidget)
    {
      QModelIndex index = indexAt(event->pos());
      if(false == index.isValid())
      {
        return;
      }
      DataArrayPath path = getDataArrayPath(index);

      // Check path requirements
      if(selectionWidget->checkPathReqs(path))
      {
        selectionWidget->setDataArrayPath(path);
        event->accept();
        return;
      }
    }
  }

  event->ignore();
}

#if 0
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
#endif

