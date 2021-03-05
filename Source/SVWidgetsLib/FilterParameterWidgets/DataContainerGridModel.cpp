/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DataContainerGridModel.h"

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtWidgets/QAbstractItemDelegate>
#include <QtWidgets/QStyleOptionComboBox>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridModel::DataContainerGridModel(const DataContainerGrid& grid, QObject* parent)
: QAbstractTableModel(parent)
, m_DCGrid(grid)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridModel::~DataContainerGridModel() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags DataContainerGridModel::flags(const QModelIndex& index) const
{
  //  qDebug() << "DataContainerGridModel::flags" << "\n";
  if(!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  if(!index.isValid())
  {
    return QAbstractTableModel::flags(index);
  }

  Qt::ItemFlags flags = QAbstractTableModel::flags(index);
  flags |= Qt::ItemIsDropEnabled | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  return flags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant DataContainerGridModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
  {
    return QVariant();
  }

  switch(role)
  {
  case Qt::TextAlignmentRole:
    return int(Qt::AlignRight | Qt::AlignVCenter);
  case Qt::DisplayRole: {
    int col = index.column();
    int row = index.row();
    SizeVec3Type pos(row, col, m_CurrentDepth);
    return m_DCGrid.getDataContainerName(pos);
  }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerGridModel::rowCount(const QModelIndex& index) const
{
  return index.isValid() ? 0 : m_DCGrid.getDimensions()[0];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerGridModel::columnCount(const QModelIndex& index) const
{
  return index.isValid() ? 0 : m_DCGrid.getDimensions()[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerGridModel::depthCount() const
{
  return m_DCGrid.getDimensions()[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerGridModel::getCurrentDepth() const
{
  return m_CurrentDepth;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridModel::setCurrentDepth(int depth)
{
  if(depth < 0 || depth > depthCount())
  {
    return;
  }
  m_CurrentDepth = depth;

  QModelIndex index;
  Q_EMIT dataChanged(index, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerGridModel::setHeaderData(int col, Qt::Orientation o, const QVariant& var, int role)
{
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerGridModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if(!index.isValid() || index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount())
  {
    return false;
  }

  if(role == Qt::DisplayRole)
  {
    SizeVec3Type pos(index.row(), index.column(), m_CurrentDepth);
    m_DCGrid.setDataContainerName(pos, value.toString());
    Q_EMIT dataChanged(index, index);
    Q_EMIT modelChanged();
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerGridModel::insertRows(int row, int count, const QModelIndex& index)
{
  beginInsertRows(QModelIndex(), row, row + count - 1);
  endInsertRows();
  Q_EMIT dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerGridModel::removeRows(int row, int count, const QModelIndex& index)
{
  if(count < 1)
  {
    return true;
  } // No Rows to remove
  beginRemoveRows(QModelIndex(), row, row + count - 1);
  endRemoveRows();
  Q_EMIT dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerGridModel::insertColumns(int col, int count, const QModelIndex& index)
{
  beginInsertColumns(QModelIndex(), col, col + count - 1);
  endInsertColumns();
  Q_EMIT dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerGridModel::removeColumns(int col, int count, const QModelIndex& index)
{
  if(count < 1)
  {
    return true;
  } // No Rows to remove
  beginRemoveColumns(QModelIndex(), col, col + count - 1);
  endRemoveColumns();
  Q_EMIT dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::DropActions DataContainerGridModel::supportedDropActions() const
{
  return Qt::CopyAction;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerGridModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const
{
  // QModelIndex targetIndex = index(row, column, parent);
  if(!parent.isValid())
  {
    return false;
  }

  DataArrayPath dap = DataArrayPath::Deserialize(data->data(SIMPLView::DragAndDrop::DataArrayPath), "|");
  return !dap.getDataContainerName().isEmpty();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerGridModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
  // QModelIndex targetIndex = index(row, column, parent);
  if(!parent.isValid())
  {
    return false;
  }

  DataArrayPath dap = DataArrayPath::Deserialize(data->data(SIMPLView::DragAndDrop::DataArrayPath), "|");
  QString dcName = dap.getDataContainerName();
  return setData(parent, dcName, Qt::DisplayRole);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridModel::setGridData(DataContainerGrid& grid)
{
  size_t count = grid.size();
  int32_t rows = grid.getDimensions()[0] - 1;
  int32_t cols = grid.getDimensions()[1] - 1;
  // Remove all the current rows in the table model
  removeRows(0, rowCount());
  removeColumns(0, columnCount());
  // Check to make sure we have data to insert.
  if(count == 0)
  {
    return;
  }
  // Now mass insert the data to the table then emit that the data has changed
  beginInsertRows(QModelIndex(), 0, rows);
  beginInsertColumns(QModelIndex(), 0, cols);
  m_DCGrid = grid;
  endInsertColumns();
  endInsertRows();
  QModelIndex topLeft = createIndex(0, 0);
  QModelIndex botRight = createIndex(rows, cols);
  Q_EMIT dataChanged(topLeft, botRight);
  Q_EMIT modelChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridModel::getGridData(DataContainerGrid& grid)
{
  grid = m_DCGrid;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemDelegate* DataContainerGridModel::getItemDelegate()
{
  return new DataContainerGridItemDelegate(m_CurrentDepth, this);
}
#endif
