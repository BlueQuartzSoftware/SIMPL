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

#include "PipelineTreeModel.h"

#include <QtWidgets>

#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"

#include "SVWidgetsLib/Widgets/PipelineTreeItem.h"
#include "SVWidgetsLib/Widgets/PipelineTreeView.h"
#include "SVWidgetsLib/QtSupport/QtSSettings.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeModel::PipelineTreeModel(QObject* parent)
: QAbstractItemModel(parent)
, m_MaxNumberOfPipelines(std::numeric_limits<int>::max())
, m_ActionUndo(nullptr)
, m_ActionRedo(nullptr)
{
  QVector<QVariant> vector;
  vector.push_back("");
  m_RootItem = new PipelineTreeItem(vector);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeModel::~PipelineTreeModel()
{
  delete m_RootItem;
}

//// -----------------------------------------------------------------------------
////
//// -----------------------------------------------------------------------------
//PipelineTreeModel* PipelineTreeModel::NewInstance(QtSSettings* prefs)
//{
//  // Erase the old content
//  if(self)
//  {
//    delete self;
//    self = nullptr;
//  }

//  QJsonObject modelObj = prefs->value("Bookmarks Model", QJsonObject());
//  self = PipelineTreeView::FromJsonObject(modelObj);

//  return self;
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineTreeModel::columnCount(const QModelIndex& parent) const
{
  return m_RootItem->columnCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex PipelineTreeModel::sibling(int row, int column, const QModelIndex& currentIndex) const
{
  if(currentIndex.column() == PipelineTreeItem::Name)
  {
    return index(currentIndex.row(), PipelineTreeItem::FilterEnabledBtn, currentIndex.parent());
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
  else if (role == Qt::FontRole)
  {
    if (item->isActivePipeline())
    {
      QFont font;
      font.setBold(true);
      return font;
    }
    else
    {
      return QVariant();
    }
  }
  else if(role == Qt::ForegroundRole)
  {
    QColor fgColor = getForegroundColor(index);
    return fgColor;
  }
  else if(role == Qt::ToolTipRole)
  {
    return QString();
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
AbstractFilter::Pointer PipelineTreeModel::filter(const QModelIndex &index)
{
  if(!index.isValid())
  {
    return AbstractFilter::NullPointer();
  }

  PipelineTreeItem* item = getItem(index);
  if (item == nullptr)
  {
    return AbstractFilter::NullPointer();
  }

  return item->getFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::setFilter(const QModelIndex &index, AbstractFilter::Pointer filter)
{
  if(!index.isValid())
  {
    return;
  }

  PipelineTreeItem* item = getItem(index);
  if (item == nullptr)
  {
    return;
  }

  item->setFilter(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeModel::filterEnabled(const QModelIndex &index)
{
  if(!index.isValid())
  {
    return false;
  }

  PipelineTreeItem* item = getItem(index);
  if (item == nullptr)
  {
    return false;
  }

  return item->getFilterEnabled();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::setFilterEnabled(const QModelIndex &index, bool enabled)
{
  if(!index.isValid())
  {
    return;
  }

  PipelineTreeItem* item = getItem(index);
  if (item == nullptr)
  {
    return;
  }

  item->setFilterEnabled(enabled);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeModel::isActivePipeline(const QModelIndex &index)
{
  PipelineTreeItem* item = getItem(index);
  return item->isActivePipeline();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::setActivePipeline(const QModelIndex &index, bool value)
{
  PipelineTreeItem* item = getItem(index);
  item->setActivePipeline(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::clearActivePipeline()
{

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
  if(item->childCount() > 0)
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
  return m_RootItem;
}

//// -----------------------------------------------------------------------------
////
//// -----------------------------------------------------------------------------
//QVariant PipelineTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//  if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
//  {
//    return m_RootItem->data(section);
//  }

//  return QVariant();
//}

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
  success = parentItem->insertChildren(position, rows, m_RootItem->columnCount());
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

  if(parentItem == m_RootItem)
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

  if(role == Qt::DecorationRole)
  {
    item->setIcon(value.value<QIcon>());
  }
  else if(role == Qt::ToolTipRole)
  {
    item->setItemTooltip(value.toString());
  }
  else
  {
    item->setData(index.column(), value);
  }

  emit dataChanged(index, index);

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeItem::WidgetState PipelineTreeModel::widgetState(const QModelIndex &index)
{
  PipelineTreeItem* item = getItem(index);
  return item->getWidgetState();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::setWidgetState(const QModelIndex &index, PipelineTreeItem::WidgetState state)
{
  PipelineTreeItem* item = getItem(index);
  item->setWidgetState(state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeItem::ErrorState PipelineTreeModel::errorState(const QModelIndex &index)
{
  PipelineTreeItem* item = getItem(index);
  return item->getErrorState();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::setErrorState(const QModelIndex &index, PipelineTreeItem::ErrorState state)
{
  PipelineTreeItem* item = getItem(index);
  item->setErrorState(state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeItem::PipelineState PipelineTreeModel::pipelineState(const QModelIndex &index)
{
  PipelineTreeItem* item = getItem(index);
  return item->getPipelineState();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::setPipelineState(const QModelIndex &index, PipelineTreeItem::PipelineState state)
{
  PipelineTreeItem* item = getItem(index);
  item->setPipelineState(state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeItem::ItemType PipelineTreeModel::itemType(const QModelIndex &index)
{
  PipelineTreeItem* item = getItem(index);
  return item->getItemType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::setItemType(const QModelIndex &index, PipelineTreeItem::ItemType type)
{
  PipelineTreeItem* item = getItem(index);
  item->setItemType(type);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeModel::pipelineSaved(const QModelIndex &index)
{
  PipelineTreeItem* item = getItem(index);
  return item->isPipelineSaved();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::setPipelineSaved(const QModelIndex &index, bool saved)
{
  PipelineTreeItem* item = getItem(index);
  item->setPipelineSaved(saved);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeModel::setNeedsToBeExpanded(const QModelIndex& index, bool value)
{
  PipelineTreeItem* item = getItem(index);
  item->setExpanded(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeModel::needsToBeExpanded(const QModelIndex& index)
{
  PipelineTreeItem* item = getItem(index);
  return item->getExpanded();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterInputWidget* PipelineTreeModel::filterInputWidget(const QModelIndex &index)
{
  PipelineTreeItem* item = getItem(index);
  return item->getFilterInputWidget();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeItem* PipelineTreeModel::getRootItem()
{
  return m_RootItem;
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
QColor PipelineTreeModel::getForegroundColor(const QModelIndex &index) const
{
  if (index.isValid() == false)
  {
    return QColor();
  }

  PipelineTreeItem* item = getItem(index);

  PipelineTreeItem::WidgetState wState = item->getWidgetState();
  PipelineTreeItem::PipelineState pState = item->getPipelineState();
  PipelineTreeItem::ErrorState eState = item->getErrorState();

  QColor fgColor;

  switch(wState)
  {
  case PipelineTreeItem::WidgetState::Ready:
    fgColor = QColor();
    break;
  case PipelineTreeItem::WidgetState::Executing:
    fgColor = QColor(6, 140, 190);
    break;
  case PipelineTreeItem::WidgetState::Completed:
    fgColor = QColor(6, 118, 6);
    break;
  case PipelineTreeItem::WidgetState::Disabled:
    fgColor = QColor(96, 96, 96);
    break;
  }

  // Do not change the background color if the widget is disabled.
  if(wState != PipelineTreeItem::WidgetState::Disabled)
  {
    switch(pState)
    {
    case PipelineTreeItem::PipelineState::Running:
      fgColor = QColor(190, 190, 190);
      break;
    case PipelineTreeItem::PipelineState::Stopped:
      fgColor = QColor(0, 0, 0);
      break;
    case PipelineTreeItem::PipelineState::Paused:
      fgColor = QColor(0, 0, 0);
      break;
    }
  }

  switch(eState)
  {
  case PipelineTreeItem::ErrorState::Ok:

    break;
  case PipelineTreeItem::ErrorState::Error:
    fgColor = QColor(179, 2, 5);
    break;
  case PipelineTreeItem::ErrorState::Warning:
    fgColor = QColor(215, 197, 1);
    break;
  }

  return fgColor;
}
