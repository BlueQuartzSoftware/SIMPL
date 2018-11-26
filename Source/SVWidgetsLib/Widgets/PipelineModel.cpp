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

#include "PipelineModel.h"

#include <QtWidgets>

#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/H5FilterParametersWriter.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/PipelineItem.h"
#include "SVWidgetsLib/Widgets/PipelineFilterMimeData.h"
#include "SVWidgetsLib/QtSupport/QtSSettings.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineModel::PipelineModel(QObject* parent)
: QAbstractItemModel(parent)
, m_MaxNumberOfPipelines(std::numeric_limits<int>::max())
{
  QVector<QVariant> vector;
  vector.push_back("");
  m_RootItem = new PipelineItem(vector);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineModel::~PipelineModel()
{
  delete m_RootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::updateActivePipeline(const QModelIndex &pipelineIdx)
{
  emit clearIssuesTriggered();

  setActivePipeline(m_ActivePipelineIndex, false);
  setActivePipeline(pipelineIdx, true);

  m_ActivePipelineIndex = pipelineIdx;

  if(m_ActivePipelineIndex.isValid())
  {
    emit preflightTriggered(m_ActivePipelineIndex, this);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineModel::columnCount(const QModelIndex& parent) const
{
  return m_RootItem->columnCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant PipelineModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
  {
    return QVariant();
  }

  PipelineItem* item = getItem(index);

  if (role == PipelineModel::Roles::WidgetStateRole)
  {
    return static_cast<int>(item->getWidgetState());
  }
  if(role == PipelineModel::Roles::ErrorStateRole)
  {
    return static_cast<int>(item->getErrorState());
  }
  if(role == PipelineModel::Roles::PipelineStateRole)
  {
    return static_cast<int>(item->getPipelineState());
  }
  if(role == PipelineModel::Roles::ItemTypeRole)
  {
    return static_cast<int>(item->getItemType());
  }
  else if (role == PipelineModel::Roles::BorderSizeRole)
  {
    return item->getBorderSize();
  }
  else if (role == PipelineModel::Roles::HeightRole)
  {
    return item->getHeight();
  }
  else if (role == PipelineModel::Roles::WidthRole)
  {
    return item->getWidth();
  }
  else if (role == PipelineModel::Roles::XOffsetRole)
  {
    return item->getXOffset();
  }
  else if (role == PipelineModel::Roles::YOffsetRole)
  {
    return item->getYOffset();
  }
  else if (role == PipelineModel::Roles::ExpandedRole)
  {
    return item->getExpanded();
  }
  else if (role == PipelineModel::Roles::AnimationTypeRole)
  {
    return item->getCurrentAnimationType();
  }
  else if(role == Qt::DisplayRole)
  {
    return item->data(index.column());
  }
  else if(role == Qt::SizeHintRole)
  {
    return item->getSize();
  }
  else if (role == Qt::FontRole)
  {
    if (item->isActivePipeline())
    {
      QFont font;
      font.setBold(true);
      return font;
    }

      return QVariant();
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
    QModelIndex nameIndex = this->index(index.row(), PipelineItem::Contents, index.parent());
    if(nameIndex == index)
    {
      PipelineItem* item = getItem(index);
      return item->getIcon();
    }

      return QVariant();
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PipelineModel::filter(const QModelIndex &index) const
{
  if(!index.isValid())
  {
    return AbstractFilter::NullPointer();
  }

  PipelineItem* item = getItem(index);
  if (item == nullptr)
  {
    return AbstractFilter::NullPointer();
  }

  return item->getFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setFilter(const QModelIndex &index, AbstractFilter::Pointer filter)
{
  if(!index.isValid())
  {
    return;
  }

  PipelineItem* item = getItem(index);
  if (item == nullptr)
  {
    return;
  }

  item->setFilter(filter);

  emit dataChanged(index, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineModel::dropIndicatorText(const QModelIndex &index) const
{
  if(!index.isValid())
  {
    return QString();
  }

  PipelineItem* item = getItem(index);
  if (item == nullptr)
  {
    return QString();
  }

  return item->getDropIndicatorText();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setDropIndicatorText(const QModelIndex &index, const QString &text)
{
  if(!index.isValid())
  {
    return;
  }

  PipelineItem* item = getItem(index);
  if (item == nullptr)
  {
    return;
  }

  item->setDropIndicatorText(text);

  emit dataChanged(index, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex PipelineModel::indexOfFilter(AbstractFilter* filter, const QModelIndex &parent)
{
  for (int i = 0; i < rowCount(parent); i++)
  {
    QModelIndex childIndex = index(i, PipelineItem::Contents, parent);
    if (this->filter(childIndex).get() == filter)
    {
      return childIndex;
    }
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex PipelineModel::getActivePipeline()
{
  return m_ActivePipelineIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setActivePipeline(const QModelIndex &index, bool value)
{
  PipelineItem* item = getItem(index);
  item->setActivePipeline(value);
  
  if (value)
  {
    m_ActivePipelineIndex = index;
  }
  else
  {
    m_ActivePipelineIndex = QModelIndex();
  }

  emit dataChanged(index, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::clearActivePipeline()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags PipelineModel::flags(const QModelIndex& index) const
{
  if (!index.isValid() || index.model() != this)
  {
    return Qt::ItemIsDropEnabled;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList PipelineModel::mimeTypes() const
{
  QStringList types;
  types << SIMPLView::DragAndDrop::FilterPipelineItem;
  types << SIMPLView::DragAndDrop::FilterListItem;
  types << SIMPLView::DragAndDrop::BookmarkItem;
  types << SIMPLView::DragAndDrop::Url;
  return types;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
  Q_UNUSED(row);
  Q_UNUSED(parent);

  if (action == Qt::IgnoreAction)
  {
    return false;
  }

  if (!data->hasFormat(SIMPLView::DragAndDrop::FilterPipelineItem)
      && !data->hasFormat(SIMPLView::DragAndDrop::FilterListItem)
      && !data->hasFormat(SIMPLView::DragAndDrop::BookmarkItem)
      && !data->hasFormat(SIMPLView::DragAndDrop::Url))
  {
    return false;
  }

  if (column > 0)
  {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineItem* PipelineModel::getItem(const QModelIndex& index) const
{
  if(index.isValid())
  {
    PipelineItem* item = static_cast<PipelineItem*>(index.internalPointer());
    if(item != nullptr)
    {
      return item;
    }
  }
  return m_RootItem;
}

//// -----------------------------------------------------------------------------
////
//// -----------------------------------------------------------------------------
//QVariant PipelineModel::headerData(int section, Qt::Orientation orientation, int role) const
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
QModelIndex PipelineModel::index(int row, int column, const QModelIndex& parent) const
{
  if(parent.isValid() && parent.column() != 0)
  {
    return QModelIndex();
  }

  PipelineItem* parentItem = getItem(parent);

  PipelineItem* childItem = parentItem->child(row);
  if(childItem != nullptr)
  {
    return createIndex(row, column, childItem);
  }

    return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineModel::insertRows(int position, int rows, const QModelIndex& parent)
{
  PipelineItem* parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, m_RootItem->columnCount());
  endInsertRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineModel::removeRows(int position, int rows, const QModelIndex& parent)
{
  if(position < 0)
  {
    return false;
  }

  PipelineItem* parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
  beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);

  PipelineItem* srcParentItem = getItem(sourceParent);
  PipelineItem* destParentItem = getItem(destinationParent);

  for(int i = sourceRow; i < sourceRow + count; i++)
  {
    QModelIndex srcIndex = index(i, PipelineItem::Contents, sourceParent);
    PipelineItem* srcItem = getItem(srcIndex);

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
QModelIndex PipelineModel::parent(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return QModelIndex();
  }

  PipelineItem* childItem = getItem(index);
  PipelineItem* parentItem = childItem->parent();

  if(parentItem == m_RootItem)
  {
    return QModelIndex();
  }

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineModel::rowCount(const QModelIndex& parent) const
{
  PipelineItem* parentItem = getItem(parent);

  return parentItem->childCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  PipelineItem* item = getItem(index);

  if (role == PipelineModel::Roles::WidgetStateRole)
  {
    bool ok = false;
    int intValue = value.toInt(&ok);
    if(!ok)
    {
      return false;
    }

    PipelineItem::WidgetState value = static_cast<PipelineItem::WidgetState>(intValue);
    item->setWidgetState(value);
  }
  else if (role == PipelineModel::Roles::ErrorStateRole)
  {
    bool ok = false;
    int intValue = value.toInt(&ok);
    if(!ok)
    {
      return false;
    }

    PipelineItem::ErrorState value = static_cast<PipelineItem::ErrorState>(intValue);
    item->setErrorState(value);
  }
  else if (role == PipelineModel::Roles::PipelineStateRole)
  {
    bool ok = false;
    int intValue = value.toInt(&ok);
    if(!ok)
    {
      return false;
    }

    PipelineItem::PipelineState value = static_cast<PipelineItem::PipelineState>(intValue);
    item->setPipelineState(value);
  }
  else if (role == PipelineModel::Roles::ItemTypeRole)
  {
    bool ok = false;
    int intValue = value.toInt(&ok);
    if(!ok)
    {
      return false;
    }

    PipelineItem::ItemType value = static_cast<PipelineItem::ItemType>(intValue);
    item->setItemType(value);
  }
  else if (role == PipelineModel::Roles::BorderSizeRole)
  {
    bool ok = false;
    int borderSize = value.toInt(&ok);
    if(!ok)
    {
      return false;
    }

    item->setBorderSize(borderSize);
  }
  else if (role == PipelineModel::Roles::HeightRole)
  {
    bool ok = false;
    int height = value.toInt(&ok);
    if(!ok)
    {
      return false;
    }

    item->setHeight(height);
  }
  else if (role == PipelineModel::Roles::WidthRole)
  {
    bool ok = false;
    int width = value.toInt(&ok);
    if(!ok)
    {
      return false;
    }

    item->setWidth(width);
  }
  else if (role == PipelineModel::Roles::XOffsetRole)
  {
    bool ok = false;
    int offset = value.toInt(&ok);
    if(!ok)
    {
      return false;
    }

    item->setXOffset(offset);
  }
  else if (role == PipelineModel::Roles::YOffsetRole)
  {
    bool ok = false;
    int offset = value.toInt(&ok);
    if(!ok)
    {
      return false;
    }

    item->setYOffset(offset);
  }
  else if (role == PipelineModel::Roles::AnimationTypeRole)
  {
    bool ok = false;
    int animationInt = value.toInt(&ok);
    if(!ok)
    {
      return false;
    }

    PipelineItem::AnimationType animationType = static_cast<PipelineItem::AnimationType>(animationInt);
    item->setCurrentAnimationType(animationType);
  }
  else if (role == PipelineModel::Roles::ExpandedRole)
  {
    int expanded = static_cast<int>(value.toBool());
    item->setExpanded(expanded != 0);
  }
  else if(role == Qt::DecorationRole)
  {
    item->setIcon(value.value<QIcon>());
  }
  else if(role == Qt::ToolTipRole)
  {
    item->setItemTooltip(value.toString());
  }
  else if (role == Qt::DisplayRole)
  {
    item->setData(index.column(), value);
  }
  else if (role == Qt::SizeHintRole)
  {
    item->setSize(value.toSize());
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
Qt::DropActions PipelineModel::supportedDropActions() const
{
  return Qt::CopyAction | Qt::MoveAction;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineModel::getMaxFilterCount() const
{
  int numFilters = rowCount();
  int maxFilterCount = 0;
  if(numFilters < 10)
  {
    numFilters = 11;
    maxFilterCount = 99;
  }

  if(numFilters > 9)
  {
    int mag = 0;
    int max = numFilters;
    maxFilterCount = 1;
    while(max > 0)
    {
      mag++;
      max = max / 10;
      maxFilterCount *= 10;
    }
    maxFilterCount -= 1;
  }

  return maxFilterCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineModel::pipelineSaved(const QModelIndex &index)
{
  PipelineItem* item = getItem(index);
  return item->isPipelineSaved();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setPipelineSaved(const QModelIndex &index, bool saved)
{
  PipelineItem* item = getItem(index);
  item->setPipelineSaved(saved);

  emit dataChanged(index, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterInputWidget* PipelineModel::filterInputWidget(const QModelIndex &index)
{
  PipelineItem* item = getItem(index);
  return item->getFilterInputWidget();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineItem* PipelineModel::getRootItem()
{
  return m_RootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineModel::isEmpty()
{
  return rowCount(QModelIndex()) <= 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor PipelineModel::getForegroundColor(const QModelIndex &index) const
{
  if(!index.isValid())
  {
    return QColor();
  }

  PipelineItem* item = getItem(index);

  PipelineItem::WidgetState wState = item->getWidgetState();
  PipelineItem::PipelineState pState = item->getPipelineState();
  PipelineItem::ErrorState eState = item->getErrorState();

  QColor fgColor;

  switch(wState)
  {
  case PipelineItem::WidgetState::Ready:
    fgColor = QColor();
    break;
  case PipelineItem::WidgetState::Executing:
    fgColor = QColor(6, 140, 190);
    break;
  case PipelineItem::WidgetState::Completed:
    fgColor = QColor(6, 118, 6);
    break;
  case PipelineItem::WidgetState::Disabled:
    fgColor = QColor(96, 96, 96);
    break;
  }

  // Do not change the background color if the widget is disabled.
  if(wState != PipelineItem::WidgetState::Disabled)
  {
    switch(pState)
    {
    case PipelineItem::PipelineState::Running:
      fgColor = QColor(190, 190, 190);
      break;
    case PipelineItem::PipelineState::Stopped:
      fgColor = QColor(0, 0, 0);
      break;
    case PipelineItem::PipelineState::Paused:
      fgColor = QColor(0, 0, 0);
      break;
    }
  }

  switch(eState)
  {
  case PipelineItem::ErrorState::Ok:

    break;
  case PipelineItem::ErrorState::Error:
    fgColor = QColor(179, 2, 5);
    break;
  case PipelineItem::ErrorState::Warning:
    fgColor = QColor(215, 197, 1);
    break;
  }

  return fgColor;
}
