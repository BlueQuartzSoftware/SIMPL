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

#include "SVWidgetsLib/Widgets/PipelineItem.h"
#include "SVWidgetsLib/Widgets/PipelineTreeView.h"
#include "SVWidgetsLib/Widgets/BreakpointFilterWidget.h"
#include "SVWidgetsLib/QtSupport/QtSSettings.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineModel::PipelineModel(QObject* parent)
: QAbstractItemModel(parent)
, m_MaxNumberOfPipelines(std::numeric_limits<int>::max())
, m_ActionUndo(nullptr)
, m_ActionRedo(nullptr)
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

  if (m_ActivePipelineIndex.isValid() == true)
  {
    emit preflightTriggered(m_ActivePipelineIndex, this);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer PipelineModel::getFilterPipeline(const QModelIndex &pipelineIndex)
{
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  qint32 count = rowCount(pipelineIndex);
  for(qint32 i = 0; i < count; ++i)
  {
    QModelIndex childIndex = index(i, PipelineItem::Name, pipelineIndex);
    if(childIndex.isValid())
    {
      AbstractFilter::Pointer filter = this->filter(childIndex);
      Breakpoint::Pointer breakpoint = std::dynamic_pointer_cast<Breakpoint>(filter);
      if(nullptr != breakpoint)
      {
        connect(pipeline.get(), SIGNAL(pipelineCanceled()), breakpoint.get(), SLOT(resumePipeline()));
      }

      pipeline->pushBack(filter);
    }
  }
  for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
  {
    pipeline->addMessageReceiver(m_PipelineMessageObservers[i]);
  }
  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer PipelineModel::getCopyOfFilterPipeline()
{
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  for(int i = 0; i < rowCount(); i++)
  {
    QModelIndex filterIndex = index(i, PipelineItem::Name);
    AbstractFilter::Pointer filter = this->filter(filterIndex);
    AbstractFilter::Pointer copy = filter->newFilterInstance(true);
    pipeline->pushBack(copy);
  }
  for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
  {
    pipeline->addMessageReceiver(m_PipelineMessageObservers[i]);
  }

  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineModel::writePipeline(const QModelIndex &pipelineIndex, const QString &outputPath)
{
  QFileInfo fi(outputPath);
  QString ext = fi.completeSuffix();

  // If the filePath already exists - delete it so that we get a clean write to the file
  if(fi.exists() == true && (ext == "dream3d" || ext == "json"))
  {
    QFile f(outputPath);
    if(f.remove() == false)
    {
      QMessageBox::warning(nullptr, QString::fromLatin1("Pipeline Write Error"), QString::fromLatin1("There was an error removing the existing pipeline file. The pipeline was NOT saved."));
      return -1;
    }
  }

  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = getFilterPipeline(pipelineIndex);

  int err = 0;
  if(ext == "dream3d")
  {
    QList<IObserver*> observers;
    for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
    {
      observers.push_back(reinterpret_cast<IObserver*>(m_PipelineMessageObservers[i]));
    }

    H5FilterParametersWriter::Pointer dream3dWriter = H5FilterParametersWriter::New();
    err = dream3dWriter->writePipelineToFile(pipeline, fi.absoluteFilePath(), fi.fileName(), observers);
  }
  else if(ext == "json")
  {
    QList<IObserver*> observers;
    for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
    {
      observers.push_back(reinterpret_cast<IObserver*>(m_PipelineMessageObservers[i]));
    }

    JsonFilterParametersWriter::Pointer jsonWriter = JsonFilterParametersWriter::New();
    jsonWriter->writePipelineToFile(pipeline, fi.absoluteFilePath(), fi.fileName(), observers);
  }
  else
  {
    emit statusMessageGenerated(tr("The pipeline was not written to file '%1'. '%2' is an unsupported file extension.").arg(fi.fileName()).arg(ext));
    return -1;
  }

  if(err < 0)
  {
    emit statusMessageGenerated(tr("There was an error while saving the pipeline to file '%1'.").arg(fi.fileName()));
    return -1;
  }
  else
  {
    emit statusMessageGenerated(tr("The pipeline has been saved successfully to '%1'.").arg(fi.fileName()));
  }

  return 0;
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
    QModelIndex nameIndex = this->index(index.row(), PipelineItem::Name, index.parent());
    if(nameIndex == index)
    {
      PipelineItem* item = getItem(index);
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex PipelineModel::indexOfFilter(AbstractFilter::Pointer filter, const QModelIndex &parent)
{
  for (int i = 0; i < rowCount(parent); i++)
  {
    QModelIndex childIndex = index(i, PipelineItem::Name, parent);
    if (this->filter(childIndex) == filter)
    {
      return childIndex;
    }
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineModel::filterEnabled(const QModelIndex &index)
{
  if(!index.isValid())
  {
    return false;
  }

  PipelineItem* item = getItem(index);
  if (item == nullptr)
  {
    return false;
  }

  return item->getFilterEnabled();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setFilterEnabled(const QModelIndex &index, bool enabled)
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

  item->setFilterEnabled(enabled);
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
  if(!index.isValid())
  {
    return 0;
  }

  Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

  PipelineItem* item = getItem(index);
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
PipelineItem* PipelineModel::getItem(const QModelIndex& index) const
{
  if(index.isValid())
  {
    PipelineItem* item = static_cast<PipelineItem*>(index.internalPointer());
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
    QModelIndex srcIndex = index(i, PipelineItem::Name, sourceParent);
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
PipelineItem::WidgetState PipelineModel::widgetState(const QModelIndex &index) const
{
  PipelineItem* item = getItem(index);
  return item->getWidgetState();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setWidgetState(const QModelIndex &index, PipelineItem::WidgetState state)
{
  PipelineItem* item = getItem(index);
  item->setWidgetState(state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineItem::ErrorState PipelineModel::errorState(const QModelIndex &index) const
{
  PipelineItem* item = getItem(index);
  return item->getErrorState();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setErrorState(const QModelIndex &index, PipelineItem::ErrorState state)
{
  PipelineItem* item = getItem(index);
  item->setErrorState(state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineItem::PipelineState PipelineModel::pipelineState(const QModelIndex &index) const
{
  PipelineItem* item = getItem(index);
  return item->getPipelineState();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setPipelineState(const QModelIndex &index, PipelineItem::PipelineState state)
{
  PipelineItem* item = getItem(index);
  item->setPipelineState(state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineItem::ItemType PipelineModel::itemType(const QModelIndex &index)
{
  PipelineItem* item = getItem(index);
  return item->getItemType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setItemType(const QModelIndex &index, PipelineItem::ItemType type)
{
  PipelineItem* item = getItem(index);
  item->setItemType(type);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineModel::isHovering(const QModelIndex &index) const
{
  PipelineItem* item = getItem(index);
  return item->getHovering();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setHovering(const QModelIndex &index)
{
  if (m_CurrentHoveringIndex.isValid())
  {
    PipelineItem* item = getItem(m_CurrentHoveringIndex);
    item->setHovering(false);
  }

  PipelineItem* item = getItem(index);
  item->setHovering(true);
  m_CurrentHoveringIndex = index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setFilterIndexString(const QModelIndex &index, int i)
{
  QString paddedIndex = "";
  int numFilters = rowCount();

  if(numFilters < 10)
  {
    numFilters = 11;
  }
  QString numStr = QString::number(i);

  if(numFilters > 9)
  {
    int mag = 0;
    int max = numFilters;
    while(max > 0)
    {
      mag++;
      max = max / 10;
    }
    numStr = "";             // Clear the string
    QTextStream ss(&numStr); // Create a QTextStream to set up the padding
    ss.setFieldWidth(mag);
    ss.setPadChar('0');
    ss << i;
  }
  paddedIndex = numStr;

  PipelineItem* item = getItem(index);
  item->setFilterIndex(paddedIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineModel::filterIndexString(const QModelIndex &index) const
{
  PipelineItem* item = getItem(index);
  return item->getFilterIndex();
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::setNeedsToBeExpanded(const QModelIndex& index, bool value)
{
  PipelineItem* item = getItem(index);
  item->setExpanded(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineModel::needsToBeExpanded(const QModelIndex& index)
{
  PipelineItem* item = getItem(index);
  return item->getExpanded();
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
  if(rowCount(QModelIndex()) <= 0)
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor PipelineModel::getForegroundColor(const QModelIndex &index) const
{
  if (index.isValid() == false)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineModel::addPipelineMessageObserver(QObject* pipelineMessageObserver)
{
  m_PipelineMessageObservers.push_back(pipelineMessageObserver);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QObject*> PipelineModel::getPipelineMessageObservers()
{
  return m_PipelineMessageObservers;
}
