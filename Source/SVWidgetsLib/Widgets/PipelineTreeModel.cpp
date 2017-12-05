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

#include "PipelineTreeModel.h"

#include <QtWidgets>

#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"

#include "SVWidgetsLib/Widgets/PipelineTreeItem.h"
#include "SVWidgetsLib/Widgets/PipelineTreeView.h"
#include "SVWidgetsLib/QtSupport/QtSSettings.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data

PipelineTreeModel* PipelineTreeModel::self = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeModel::PipelineTreeModel(QObject* parent)
: QAbstractItemModel(parent)
{
  QVector<QVariant> vector;
  vector.push_back("Name");
  vector.push_back("Path");
  rootItem = new PipelineTreeItem(vector);

  m_Watcher = new QFileSystemWatcher(this);

  connect(this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(updateModel(const QModelIndex&, const QModelIndex&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeModel::~PipelineTreeModel()
{
  delete rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeModel* PipelineTreeModel::Instance()
{
  if(self == nullptr)
  {
    self = new PipelineTreeModel();
  }

  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeModel* PipelineTreeModel::NewInstance(QtSSettings* prefs)
{
  // Erase the old content
  if(self)
  {
    delete self;
    self = nullptr;
  }

  QJsonObject modelObj = prefs->value("Bookmarks Model", QJsonObject());
  self = PipelineTreeView::FromJsonObject(modelObj);

  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::setFileSystemWatcher(QFileSystemWatcher* watcher)
{
  m_Watcher = watcher;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFileSystemWatcher* PipelineTreeModel::getFileSystemWatcher()
{
  return m_Watcher;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::updateRowState(const QString& path)
{
  QFileInfo fi(path);

  QModelIndexList indexList = findIndexByPath(path);
  for(int i = 0; i < indexList.size(); i++)
  {
    QModelIndex nameIndex = index(indexList[i].row(), PipelineTreeItem::Name, indexList[i].parent());

    // Set the itemHasError variable
    setData(nameIndex, !fi.exists(), Qt::UserRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineTreeModel::columnCount(const QModelIndex& parent) const
{
  return rootItem->columnCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex PipelineTreeModel::sibling(int row, int column, const QModelIndex& currentIndex) const
{
  if(currentIndex.column() == PipelineTreeItem::Name)
  {
    return index(currentIndex.row(), PipelineTreeItem::Path, currentIndex.parent());
  }
  else
  {
    return index(currentIndex.row(), PipelineTreeItem::Name, currentIndex.parent());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant PipelineTreeModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
  {
    return QVariant();
  }

  PipelineTreeItem* item = getItem(index);

  if(role == Qt::DisplayRole)
  {
    return item->data(index.column());
  }
  else if(role == Qt::UserRole)
  {
    return item->getItemHasErrors();
  }
  else if(role == Qt::BackgroundRole)
  {
    if(item->getItemHasErrors() == true)
    {
      return QColor(235, 110, 110);
    }
    else
    {
      return QVariant();
    }
  }
  else if(role == Qt::ForegroundRole)
  {
    if(item->getItemHasErrors() == true)
    {
      return QColor(240, 240, 240);
    }
    else
    {
      return QColor(Qt::black);
    }
  }
  else if(role == Qt::ToolTipRole)
  {
    QString path = item->data(1).toString();
    QFileInfo info(path);
    if(info.exists() == false)
    {
      QString tooltip = "'" + this->index(index.row(), PipelineTreeItem::Path, index.parent()).data().toString() +
                        "' was not found on the file system.\nYou can either locate the file or delete the entry from the table.";
      return tooltip;
    }
    else
    {
      if(info.suffix().compare("json") == 0)
      {
        QString html = JsonFilterParametersReader::HtmlSummaryFromFile(path, nullptr);
        return html;
      }
      else
      {
        return path;
      }
    }
  }
  else if(role == Qt::DecorationRole)
  {
    QModelIndex nameIndex = this->index(index.row(), PipelineTreeItem::Name, index.parent());
    if(nameIndex == index)
    {
      PipelineTreeItem* item = getItem(index);
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
Qt::ItemFlags PipelineTreeModel::flags(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return 0;
  }

  Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

  PipelineTreeItem* item = getItem(index);
  QString name = item->data(PipelineTreeItem::Name).toString();
  if(item->data(PipelineTreeItem::Path).toString().isEmpty())
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
PipelineTreeItem* PipelineTreeModel::getItem(const QModelIndex& index) const
{
  if(index.isValid())
  {
    PipelineTreeItem* item = static_cast<PipelineTreeItem*>(index.internalPointer());
    if(item)
    {
      return item;
    }
  }
  return rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant PipelineTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
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
QModelIndex PipelineTreeModel::index(int row, int column, const QModelIndex& parent) const
{
  if(parent.isValid() && parent.column() != 0)
  {
    return QModelIndex();
  }

  PipelineTreeItem* parentItem = getItem(parent);

  PipelineTreeItem* childItem = parentItem->child(row);
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
bool PipelineTreeModel::insertRows(int position, int rows, const QModelIndex& parent)
{
  PipelineTreeItem* parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, rootItem->columnCount());
  endInsertRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeModel::removeRows(int position, int rows, const QModelIndex& parent)
{
  PipelineTreeItem* parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
  beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);

  PipelineTreeItem* srcParentItem = getItem(sourceParent);
  PipelineTreeItem* destParentItem = getItem(destinationParent);

  for(int i = sourceRow; i < sourceRow + count; i++)
  {
    QModelIndex srcIndex = index(i, PipelineTreeItem::Name, sourceParent);
    PipelineTreeItem* srcItem = getItem(srcIndex);

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
QModelIndex PipelineTreeModel::parent(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return QModelIndex();
  }

  PipelineTreeItem* childItem = getItem(index);
  PipelineTreeItem* parentItem = childItem->parent();

  if(parentItem == rootItem)
  {
    return QModelIndex();
  }

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineTreeModel::rowCount(const QModelIndex& parent) const
{
  PipelineTreeItem* parentItem = getItem(parent);

  return parentItem->childCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  PipelineTreeItem* item = getItem(index);
  bool result = false;

  if(role == Qt::UserRole)
  {
    result = item->setItemHasErrors(value.value<bool>());
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
void PipelineTreeModel::setNeedsToBeExpanded(const QModelIndex& index, bool value)
{
  PipelineTreeItem* item = getItem(index);
  item->setNeedsToBeExpanded(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeModel::needsToBeExpanded(const QModelIndex& index)
{
  PipelineTreeItem* item = getItem(index);
  return item->needsToBeExpanded();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeItem* PipelineTreeModel::getRootItem()
{
  return rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeModel::isEmpty()
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
void PipelineTreeModel::addFileToTree(QString& path, QModelIndex& specifiedParent)
{
  path = QDir::toNativeSeparators(path);

  QFileInfo fi(path);

  int rowPos = self->rowCount(specifiedParent);
  self->insertRow(rowPos, specifiedParent);
  QModelIndex newNameIndex = self->index(rowPos, PipelineTreeItem::Name, specifiedParent);

  if(fi.isFile())
  {
    QString name = fi.baseName();
    self->setData(newNameIndex, name, Qt::DisplayRole);
  }
  else
  {
    QDir dir(path);
    self->setData(newNameIndex, dir.dirName(), Qt::DisplayRole);
  }

  if(fi.isFile())
  {
    QModelIndex newPathIndex = self->index(rowPos, PipelineTreeItem::Path, specifiedParent);
    self->setData(newPathIndex, path, Qt::DisplayRole);
    self->setData(newNameIndex, QIcon(":/bookmark.png"), Qt::DecorationRole);
  }
  else
  {
    self->setData(newNameIndex, QIcon(":/folder_blue.png"), Qt::DecorationRole);

    QStringList filters;
    filters << "*.dream3d"
            << "*.ini"
            << "*.txt"
            << "*.json";
    QDirIterator iter(path, filters, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    while(iter.hasNext())
    {
      QString nextPath = iter.next();
      addFileToTree(nextPath, newNameIndex);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList PipelineTreeModel::getFilePaths()
{
  return getFilePaths(rootItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList PipelineTreeModel::getFilePaths(PipelineTreeItem* item)
{
  QStringList list;
  if(item != rootItem && item->childCount() <= 0)
  {
    QString filePath = item->data(PipelineTreeItem::Path).toString();
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
QModelIndexList PipelineTreeModel::findIndexByPath(QString filePath)
{
  QModelIndexList list;
  for(int i = 0; i < rootItem->childCount(); i++)
  {
    QModelIndex child = index(i, PipelineTreeItem::Path, QModelIndex());
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
QModelIndexList PipelineTreeModel::findIndexByPath(const QModelIndex& current, QString filePath)
{
  QModelIndex actual = index(current.row(), PipelineTreeItem::Name, current.parent());

  QModelIndexList list;
  for(int i = 0; i < rowCount(actual); i++)
  {
    QModelIndex pathIndex = index(i, PipelineTreeItem::Path, actual);

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
void PipelineTreeModel::updateModel(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  if(topLeft.isValid())
  {
    QString path = index(topLeft.row(), PipelineTreeItem::Path, topLeft.parent()).data().toString();
    QFileInfo fi(path);
    if(nullptr != m_Watcher && path.isEmpty() == false && fi.exists())
    {
      m_Watcher->addPath(path);
    }
  }
  else if(bottomRight.isValid())
  {
    QString path = index(bottomRight.row(), PipelineTreeItem::Path, bottomRight.parent()).data().toString();
    QFileInfo fi(path);
    if(nullptr != m_Watcher && path.isEmpty() == false && fi.exists())
    {
      m_Watcher->addPath(path);
    }
  }
}
