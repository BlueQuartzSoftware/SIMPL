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

#include <QtWidgets>

#include <QtCore/QVector>


#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"


#include "SVWidgetsLib/Widgets/DataBrowserModel.h"
#include "SVWidgetsLib/Widgets/DataBrowserTreeView.h"
#include "SVWidgetsLib/Widgets/SIMPLViewToolbox.h"
#include "SVWidgetsLib/QtSupport/QtSSettings.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_DataBrowserModel.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataBrowserModel::DataBrowserModel(QObject* parent)
: QStandardItemModel(parent)
{
  QVector<QVariant> vector;
  vector.push_back("Name");
  vector.push_back("Type");
  rootItem = new DataBrowserItem(vector, DataBrowserItem::ItemType::RootItem);

  connect(this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
          this, SLOT(updateModel(const QModelIndex&, const QModelIndex&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataBrowserModel::~DataBrowserModel()
{
  delete rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex DataBrowserModel::index(int row, int column, const QModelIndex& parent) const
{
  if(parent.isValid() && parent.column() != 0)
  {
    return QModelIndex();
  }

  DataBrowserItem* parentItem = getItem(parent);

  DataBrowserItem* childItem = parentItem->child(row);
  if(childItem)
  {
    return createIndex(row, column, childItem);
  }
  else
  {
    return QModelIndex();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex DataBrowserModel::parent(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return QModelIndex();
  }

  DataBrowserItem* childItem = getItem(index);
  DataBrowserItem* parentItem = childItem->parent();

  if(parentItem == rootItem)
  {
    return QModelIndex();
  }

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataBrowserModel::rowCount(const QModelIndex& parent) const
{
  DataBrowserItem* parentItem = getItem(parent);

  return parentItem->childCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataBrowserModel::columnCount(const QModelIndex& parent) const
{
  return rootItem->columnCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant DataBrowserModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
  {
    return QVariant();
  }

  DataBrowserItem* item = getItem(index);

  if(role == Qt::DisplayRole)
  {
    return item->data(index.column());
  }
  else if(role == Qt::UserRole)
  {
    //return item->getItemHasErrors();
  }
  else if(role == Qt::BackgroundRole)
  {
    return item->backgroundColor();
  }
  else if(role == Qt::ForegroundRole)
  {
    return item->foregroundColor();
  }
  else if(role == Qt::ToolTipRole)
  {
    QString tooltip = "Tooltip is not implemented yet";
    return tooltip;
  }
  else if(role == Qt::DecorationRole)
  {
    QModelIndex nameIndex = this->index(index.row(), DataBrowserItem::Name, index.parent());
    if(nameIndex == index)
    {
      DataBrowserItem* item = getItem(index);
      return item->getIcon();
    }
    else
    {
      return QVariant();
    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant DataBrowserModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    return rootItem->data(section);
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataBrowserModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  DataBrowserItem* item = getItem(index);
  bool result = false;

  if(role == Qt::UserRole)
  {
   // result = item->setItemHasErrors(value.value<bool>());
  }
  else if(role == Qt::DecorationRole)
  {
    result = item->setIcon(value.value<QIcon>());
  }
  else if(role == Qt::ToolTipRole)
  {
    result = item->setItemTooltip(value.toString());
  }
  else
  {
    result = item->setData(index.column(), value);
  }

  if(result)
  {
    emit dataChanged(index, index);
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags DataBrowserModel::flags(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return 0;
  }

  Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

  DataBrowserItem* item = getItem(index);
  QString name = item->data(DataBrowserItem::Name).toString();
  if(item->data(DataBrowserItem::Path).toString().isEmpty())
  {
    // This is a node
    return (defaultFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
  }
  else
  {
    // This is a leaf
    return (defaultFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataBrowserItem* DataBrowserModel::getRootItem()
{
  return rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBrowserModel::syncDataModel(DataContainerArray::Pointer dca)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBrowserModel::setNeedsToBeExpanded(const QModelIndex& index, bool value)
{
  DataBrowserItem* item = getItem(index);
  item->setNeedsToBeExpanded(value);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataBrowserModel::needsToBeExpanded(const QModelIndex& index)
{
  DataBrowserItem* item = getItem(index);
  return item->needsToBeExpanded();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataBrowserItem* DataBrowserModel::getItem(const QModelIndex& index) const
{
  if(index.isValid())
  {
    DataBrowserItem* item = static_cast<DataBrowserItem*>(index.internalPointer());
    if(item)
    {
      return item;
    }
  }
  return rootItem;
}

#if 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataBrowserModel::insertRows(int position, int rows, const QModelIndex& parent)
{
  DataBrowserItem* parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, rootItem->columnCount());
  endInsertRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataBrowserModel::removeRows(int position, int rows, const QModelIndex& parent)
{
  DataBrowserItem* parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataBrowserModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
  beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);

  DataBrowserItem* srcParentItem = getItem(sourceParent);
  DataBrowserItem* destParentItem = getItem(destinationParent);

  for(int i = sourceRow; i < sourceRow + count; i++)
  {
    QModelIndex srcIndex = index(i, DataBrowserItem::Name, sourceParent);
    DataBrowserItem* srcItem = getItem(srcIndex);

    destParentItem->insertChild(destinationChild, srcItem);
    srcItem->setParent(destParentItem);
    srcParentItem->removeChild(i);
  }

  endMoveRows();

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataBrowserModel::isEmpty()
{
  if(rowCount(QModelIndex()) <= 0)
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataBrowserModel::getFilePaths()
{
  return getFilePaths(rootItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataBrowserModel::getFilePaths(DataBrowserItem* item)
{
  QStringList list;
  if(item != rootItem && item->childCount() <= 0)
  {
    QString filePath = item->data(DataBrowserItem::Path).toString();
    if(filePath.isEmpty() == false)
    {
      list.append(filePath);
    }
    return list;
  }

  for(int i = 0; i < item->childCount(); i++)
  {
    list.append(getFilePaths(item->child(i)));
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList DataBrowserModel::findIndexByPath(QString filePath)
{
  QModelIndexList list;
  for(int i = 0; i < rootItem->childCount(); i++)
  {
    QModelIndex child = index(i, DataBrowserItem::Path, QModelIndex());
    if(rowCount(child) <= 0 && child.data().toString() == filePath)
    {
      list.append(child);
    }

    for(int j = 0; j < rowCount(child); j++)
    {
      QModelIndexList subList = findIndexByPath(child, filePath);
      list.append(subList);
    }
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList DataBrowserModel::findIndexByPath(const QModelIndex& current, QString filePath)
{
  QModelIndex actual = index(current.row(), DataBrowserItem::Name, current.parent());

  QModelIndexList list;
  for(int i = 0; i < rowCount(actual); i++)
  {
    QModelIndex pathIndex = index(i, DataBrowserItem::Path, actual);

    if(rowCount(pathIndex) <= 0 && pathIndex.data().toString() == filePath)
    {
      list.append(pathIndex);
    }

    QModelIndexList subList = findIndexByPath(pathIndex, filePath);
    list.append(subList);
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBrowserModel::updateModel(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  if(topLeft.isValid())
  {
    QString path = index(topLeft.row(), DataBrowserItem::Path, topLeft.parent()).data().toString();
    QFileInfo fi(path);
    if(nullptr != m_Watcher && path.isEmpty() == false && fi.exists())
    {
      m_Watcher->addPath(path);
    }
  }
  else if(bottomRight.isValid())
  {
    QString path = index(bottomRight.row(), DataBrowserItem::Path, bottomRight.parent()).data().toString();
    QFileInfo fi(path);
    if(nullptr != m_Watcher && path.isEmpty() == false && fi.exists())
    {
      m_Watcher->addPath(path);
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBrowserModel::updateRowState(const QString& path)
{
  QFileInfo fi(path);
  if(fi.exists() == false)
  {
    QModelIndexList indexList = findIndexByPath(path);
    for(int i = 0; i < indexList.size(); i++)
    {
      QModelIndex nameIndex = index(indexList[i].row(), DataBrowserItem::Name, indexList[i].parent());

      // Set the itemHasError variable
      setData(nameIndex, true, Qt::UserRole);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex DataBrowserModel::sibling(int row, int column, const QModelIndex& currentIndex) const
{
  if(currentIndex.column() == DataBrowserItem::Name)
  {
    return index(currentIndex.row(), DataBrowserItem::Path, currentIndex.parent());
  }
  else
  {
    return index(currentIndex.row(), DataBrowserItem::Name, currentIndex.parent());
  }
}


#endif


