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

#include "PipelineTreeItem.h"

#include <QtCore/QStringList>
#include <QtGui/QColor>

#include "SVWidgetsLib/Widgets/FilterInputWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeItem::PipelineTreeItem(const QVector<QVariant>& data, PipelineTreeItem* parent)
: m_FilterInputWidget(nullptr)
, m_ActivePipeline(false)
, m_PipelineSaved(true)
, m_Icon(QIcon())
, m_Expanded(false)
, m_ItemTooltip("")
, m_WidgetState(PipelineTreeItem::WidgetState::Ready)
, m_PipelineState(PipelineTreeItem::PipelineState::Stopped)
, m_ErrorState(PipelineTreeItem::ErrorState::Ok)
, m_ItemType(PipelineTreeItem::ItemType::Unknown)
, m_ItemData(data)
, m_ParentItem(parent)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeItem::~PipelineTreeItem()
{
  qDeleteAll(m_ChildItems);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineTreeItem::TopLevelString()
{
  return QString::fromLatin1("[Top Level]");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeItem* PipelineTreeItem::child(int number)
{
  return m_ChildItems.value(number);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineTreeItem::childCount() const
{
  return m_ChildItems.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineTreeItem::childNumber() const
{
  if(m_ParentItem)
  {
    return m_ParentItem->m_ChildItems.indexOf(const_cast<PipelineTreeItem*>(this));
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineTreeItem::columnCount() const
{
  return m_ItemData.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant PipelineTreeItem::data(int column) const
{
  return m_ItemData.value(column);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PipelineTreeItem::getFilter()
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeItem::setFilter(AbstractFilter::Pointer filter)
{
  m_Filter = filter;

  setupFilterInputWidget();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeItem::insertChild(int position, PipelineTreeItem* child)
{
  m_ChildItems.insert(position, child);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeItem::insertChildren(int position, int count, int columns)
{
  if(position < 0 || position > m_ChildItems.size())
  {
    return false;
  }

  for(int row = 0; row < count; ++row)
  {
    QVector<QVariant> data(columns);
    PipelineTreeItem* item = new PipelineTreeItem(data, this);
    insertChild(position, item);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeItem::insertColumns(int position, int columns)
{
  if(position < 0 || position > m_ItemData.size())
  {
    return false;
  }

  for(int column = 0; column < columns; ++column)
  {
    m_ItemData.insert(position, QVariant());
  }

  foreach(PipelineTreeItem* child, m_ChildItems)
  {
    child->insertColumns(position, columns);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeItem* PipelineTreeItem::parent()
{
  return m_ParentItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeItem::removeChild(int position)
{
  m_ChildItems.removeAt(position);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeItem::removeChildren(int position, int count)
{
  if(position < 0 || position + count > m_ChildItems.size())
  {
    return false;
  }

  for(int row = 0; row < count; ++row)
  {
    delete m_ChildItems.takeAt(position);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeItem::removeColumns(int position, int columns)
{
  if(position < 0 || position + columns > m_ItemData.size())
  {
    return false;
  }

  for(int column = 0; column < columns; ++column)
  {
    m_ItemData.remove(position);
  }

  foreach(PipelineTreeItem* child, m_ChildItems)
  {
    child->removeColumns(position, columns);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineTreeItem::setData(int column, const QVariant& value)
{
  if(column < 0 || column >= m_ItemData.size())
  {
    return false;
  }

  m_ItemData[column] = value;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeItem::setParent(PipelineTreeItem* parent)
{
  m_ParentItem = parent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeItem::setupFilterInputWidget()
{
  // Instantiate the filter input widget object
  if(m_FilterInputWidget)
  {
    m_FilterInputWidget->deleteLater();
  }

  m_FilterInputWidget = new FilterInputWidget(m_Filter, nullptr);

  m_FilterInputWidget->displayFilterParameters(m_Filter);
}
