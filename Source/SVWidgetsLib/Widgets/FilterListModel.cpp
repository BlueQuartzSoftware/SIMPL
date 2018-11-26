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

#include "FilterListModel.h"

#include <QtGui/QFont>

#include "SVWidgetsLib/Widgets/FilterListItem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListModel::FilterListModel(QObject* parent)
: QAbstractItemModel(parent)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListModel::~FilterListModel()
{
  delete rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListModel::initialize()
{
  rootItem = new FilterListItem("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FilterListModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant FilterListModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
  {
    return QVariant();
  }

  FilterListItem* item = getItem(index);

  if(role == Qt::DisplayRole)
  {
    if (item->getItemType() == FilterListItem::ItemType::Group)
    {
      QString text = item->getName();
      text.append(tr(" (%1)").arg(rowCount(index)));
      return text;
    }
    return item->getName();
  }
  if(role == static_cast<int>(Roles::ItemTypeRole))
  {
    return static_cast<int>(item->getItemType());
  }
  if(role == static_cast<int>(Roles::ClassNameRole))
  {
    return item->getClassName();
  }
  if(role == Qt::FontRole)
  {
    if (item->getItemType() == FilterListItem::Group)
    {
      QFont font;
      font.setBold(true);
      return font;
    }

    return QVariant();
  }
  else if(role == Qt::ToolTipRole)
  {
    return item->getItemTooltip();
  }
  else if (role == Qt::BackgroundRole)
  {
    if (item->getItemType() == FilterListItem::Group)
    {
      return QColor(225, 219, 225);
    }

    return QVariant();
  }
  else if(role == Qt::DecorationRole)
  {
    return item->getIcon();
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags FilterListModel::flags(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return nullptr;
  }

  FilterListItem* item = getItem(index);
  QString name = item->getName();
  if(item->getItemType() == FilterListItem::ItemType::Filter)
  {
    // This is a node
    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
  }

    // This is a leaf
    return Qt::ItemIsEnabled;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListItem* FilterListModel::getItem(const QModelIndex& index) const
{
  if(index.isValid())
  {
    FilterListItem* item = static_cast<FilterListItem*>(index.internalPointer());
    if(item != nullptr)
    {
      return item;
    }
  }
  return rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex FilterListModel::index(int row, int column, const QModelIndex& parent) const
{
  if(parent.isValid() && parent.column() != 0)
  {
    return QModelIndex();
  }

  FilterListItem* parentItem = getItem(parent);

  FilterListItem* childItem = parentItem->child(row);
  if(childItem != nullptr)
  {
    return createIndex(row, column, childItem);
  }

    return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterListModel::insertRows(int position, int rows, const QModelIndex& parent)
{
  FilterListItem* parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, columnCount());
  endInsertRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterListModel::removeRows(int position, int rows, const QModelIndex& parent)
{
  if(rows <= 0)
  {
    return false;
  }

  FilterListItem* parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex FilterListModel::parent(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return QModelIndex();
  }

  FilterListItem* childItem = getItem(index);
  FilterListItem* parentItem = childItem->parent();

  if(parentItem == rootItem)
  {
    return QModelIndex();
  }

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FilterListModel::rowCount(const QModelIndex& parent) const
{
  FilterListItem* parentItem = getItem(parent);

  return parentItem->childCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  FilterListItem* item = getItem(index);

  if(role == Qt::DisplayRole)
  {
    item->setName(value.toString());
  }
  else if(role == static_cast<int>(Roles::ClassNameRole))
  {
    item->setClassName(value.toString());
  }
  else if(role == static_cast<int>(Roles::ItemTypeRole))
  {
    item->setItemType(static_cast<FilterListItem::ItemType>(value.toInt()));
  }
  else if(role == Qt::DecorationRole)
  {
    item->setIcon(value.value<QIcon>());
  }
  else if(role == Qt::ToolTipRole)
  {
    item->setItemTooltip(value.toString());
  }
  else
  {
    return false;
  }

  emit dataChanged(index, index);

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListItem* FilterListModel::getRootItem()
{
  return rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterListModel::isEmpty()
{
  return rowCount(QModelIndex()) <= 0;
}
