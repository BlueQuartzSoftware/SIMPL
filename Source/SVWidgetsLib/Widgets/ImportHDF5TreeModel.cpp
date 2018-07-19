/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#include <QtGui/QIcon>
#include <QtGui/QPixmap>

#include "H5Support/H5Utilities.h"

#include "ImportHDF5TreeModel.h"
#include "ImportHDF5TreeModelItem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5TreeModel::ImportHDF5TreeModel(hid_t fileId, QObject* parent)
: QAbstractItemModel(parent)
, m_FileId(fileId)
{
  m_RootItem = new ImportHDF5TreeModelItem(m_FileId, "HEADER");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5TreeModel::~ImportHDF5TreeModel()
{
  delete m_RootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportHDF5TreeModel::columnCount(const QModelIndex& parent) const
{
  if(parent.isValid())
    return static_cast<ImportHDF5TreeModelItem*>(parent.internalPointer())->columnCount();
  else
    return m_RootItem->columnCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant ImportHDF5TreeModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
    return QVariant();

  ImportHDF5TreeModelItem* item = static_cast<ImportHDF5TreeModelItem*>(index.internalPointer());

  if(role == Qt::DecorationRole)
  {
    return item->icon();
  }
  else if(role == Qt::DisplayRole)
  {
    return item->data(index.column());
  }
  else if(role == Qt::CheckStateRole && item->isGroup() == false)
  {
    return item->getCheckState();
  }
  else if(role == Qt::ForegroundRole)
  {
    bool itemHasErrors = item->getHasErrors();
    if(itemHasErrors)
    {
      return QColor(Qt::red);
    }

    return QVariant();
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportHDF5TreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if(index.column() != 0)
  {
    return false;
  }

  ImportHDF5TreeModelItem* item = static_cast<ImportHDF5TreeModelItem*>(index.internalPointer());

  if(role == Qt::CheckStateRole && item->isGroup() == false)
  {
    Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt());
    item->setCheckState(checkState);
    QString hdf5Path = item->generateHDFPath();
    if(checkState == Qt::Checked)
    {
      m_SelectedHDF5Paths.push_back(hdf5Path);
    }
    else if(checkState == Qt::Unchecked)
    {
      m_SelectedHDF5Paths.removeAll(hdf5Path);
    }

    emit selectedHDF5PathsChanged();
  }
  else if(role == Roles::HasErrorsRole)
  {
    item->setHasErrors(value.toBool());
  }

  emit dataChanged(index, index);

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags ImportHDF5TreeModel::flags(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return 0;
  }

  ImportHDF5TreeModelItem* item = static_cast<ImportHDF5TreeModelItem*>(index.internalPointer());

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if(item->isGroup() == false)
  {
    flags = flags | Qt::ItemIsUserCheckable;
  }

  return flags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant ImportHDF5TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    return QVariant("HEADER");
    // return rootItem->data(section);
  }
  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex ImportHDF5TreeModel::index(int row, int column, const QModelIndex& parent) const
{
  if(!hasIndex(row, column, parent))
    return QModelIndex();

  ImportHDF5TreeModelItem* parentItem;

  if(!parent.isValid())
    parentItem = m_RootItem;
  else
    parentItem = static_cast<ImportHDF5TreeModelItem*>(parent.internalPointer());

  ImportHDF5TreeModelItem* childItem = parentItem->child(row);
  if(childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex ImportHDF5TreeModel::parent(const QModelIndex& index) const
{
  if(!index.isValid())
    return QModelIndex();

  ImportHDF5TreeModelItem* childItem = static_cast<ImportHDF5TreeModelItem*>(index.internalPointer());
  ImportHDF5TreeModelItem* parentItem = childItem->parent();

  if(parentItem == m_RootItem)
    return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportHDF5TreeModel::hasChildren(const QModelIndex& parent) const
{
  if(parent.column() > 0)
    return false;

  ImportHDF5TreeModelItem* parentItem;
  if(!parent.isValid())
    parentItem = m_RootItem;
  else
    parentItem = static_cast<ImportHDF5TreeModelItem*>(parent.internalPointer());

  return parentItem->isGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportHDF5TreeModel::rowCount(const QModelIndex& parent) const
{
  ImportHDF5TreeModelItem* parentItem;
  if(parent.column() > 0)
    return 0;

  if(!parent.isValid())
    parentItem = m_RootItem;
  else
    parentItem = static_cast<ImportHDF5TreeModelItem*>(parent.internalPointer());

  return parentItem->childCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5TreeModel::setupModelData()
{
  if(m_FileId < 0)
  {
    return;
  }

  m_RootItem->appendChild(new ImportHDF5TreeModelItem(m_FileId, QString("/"), m_RootItem));

#if 0
  std::list<std::string> itemList;
  herr_t err = H5Utilities::getGroupObjects(_fileId, H5Utilities::MXA_ANY, itemList);
  if (err < 0)
  {
    std::cout << "Error getting group objects. " <<__FILE__ << ":" << __LINE__ << std::endl;
    return;
  }


  for (std::list<std::string>::iterator iter = itemList.begin(); iter != itemList.end(); ++iter )
  {
    rootItem->appendChild(new ImportHDF5TreeModelItem(_fileId, QString( (*iter).c_str() ), rootItem) );
  }
#endif
  // parents.last()->appendChild(new ImportHDF5TreeModelItem(columnData, parents.last()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportHDF5TreeModel::indexToHDF5Path(const QModelIndex& index)
{
  ImportHDF5TreeModelItem* item = static_cast<ImportHDF5TreeModelItem*>(index.internalPointer());
  return item->generateHDFPath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex ImportHDF5TreeModel::hdf5PathToIndex(const QString& hdf5Path)
{
  QStringList hdf5PathTokens = hdf5Path.split("/", QString::SplitBehavior::SkipEmptyParts);
  if(hdf5PathTokens.size() > 0)
  {
    QModelIndex rootIndex = index(0, 0);
    QModelIndex startIndex = index(0, 0, rootIndex);
    QModelIndex foundIndex;
    for(int i = 0; i < hdf5PathTokens.size(); i++)
    {
      QString hdf5PathToken = hdf5PathTokens[i];
      QModelIndexList indexList = match(startIndex, Qt::DisplayRole, hdf5PathToken);
      if(indexList.size() == 1)
      {
        foundIndex = indexList[0];
        startIndex = index(0, 0, foundIndex);
      }
      else
      {
        return QModelIndex();
      }
    }

    return foundIndex;
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5TreeModelItem* ImportHDF5TreeModel::getItem(const QModelIndex& index) const
{
  if(index.isValid())
  {
    ImportHDF5TreeModelItem* item = static_cast<ImportHDF5TreeModelItem*>(index.internalPointer());
    if(item)
    {
      return item;
    }
  }
  return m_RootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ImportHDF5TreeModel::getSelectedHDF5Paths()
{
  return m_SelectedHDF5Paths;
}
