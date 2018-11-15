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
#include "DataStructureItem.h"

#include <QtCore/QStringList>
#include <QtGui/QColor>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureItem::DataStructureItem(const QVector<QVariant>& data, ItemType itemType, DataStructureItem* parent)
: m_ItemData(data)
, m_ParentItem(parent)
, m_ItemTooltip("")
, m_NeedsToBeExpanded(false)
, m_Icon(QIcon())
, m_ItemType(itemType)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureItem::~DataStructureItem()
{
  qDeleteAll(m_ChildItems);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataStructureItem::TopLevelString()
{
  return QString::fromLatin1("[Top Level]");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureItem* DataStructureItem::child(int number)
{
  return m_ChildItems.value(number);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataStructureItem::childCount() const
{
  return m_ChildItems.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataStructureItem::childNumber() const
{
  if(m_ParentItem != nullptr)
  {
    return m_ParentItem->m_ChildItems.indexOf(const_cast<DataStructureItem*>(this));
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataStructureItem::columnCount() const
{
  return m_ItemData.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant DataStructureItem::data(int column) const
{
  return m_ItemData.value(column);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataStructureItem::insertChild(int position, DataStructureItem* child)
{
  m_ChildItems.insert(position, child);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataStructureItem::insertChildren(int position, int count, int columns)
{
  if(position < 0 || position > m_ChildItems.size())
  {
    return false;
  }

  for(int row = 0; row < count; ++row)
  {
    QVector<QVariant> data(columns);
    DataStructureItem* item = new DataStructureItem(data, ItemType::Unknown, this);
    insertChild(position, item);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataStructureItem::insertColumns(int position, int columns)
{
  if(position < 0 || position > m_ItemData.size())
  {
    return false;
  }

  for(int column = 0; column < columns; ++column)
  {
    m_ItemData.insert(position, QVariant());
  }

  foreach(DataStructureItem* child, m_ChildItems)
  {
    child->insertColumns(position, columns);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureItem* DataStructureItem::parent()
{
  return m_ParentItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataStructureItem::removeChild(int position)
{
  m_ChildItems.removeAt(position);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataStructureItem::removeChildren(int position, int count)
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
bool DataStructureItem::removeColumns(int position, int columns)
{
  if(position < 0 || position + columns > m_ItemData.size())
  {
    return false;
  }

  for(int column = 0; column < columns; ++column)
  {
    m_ItemData.remove(position);
  }

  foreach(DataStructureItem* child, m_ChildItems)
  {
    child->removeColumns(position, columns);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataStructureItem::setData(int column, const QVariant& value)
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
QString DataStructureItem::getItemTooltip()
{
  return m_ItemTooltip;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataStructureItem::setItemTooltip(const QString& value)
{
  m_ItemTooltip = value;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureItem::getIcon()
{
  return m_Icon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataStructureItem::setIcon(const QIcon& icon)
{
  m_Icon = icon;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataStructureItem::needsToBeExpanded()
{
  return m_NeedsToBeExpanded;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItem::setNeedsToBeExpanded(bool value)
{
  m_NeedsToBeExpanded = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItem::setParent(DataStructureItem* parent)
{
  m_ParentItem = parent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureItem::ItemType DataStructureItem::itemType()
{
  return m_ItemType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItem::setItemType(DataStructureItem::ItemType itemType)
{
  m_ItemType = itemType;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor DataStructureItem::backgroundColor()
{
  switch(m_ItemType)
  {
    case ItemType::DataContainerArray:
      return QColor(0, 77, 245);
    case ItemType::DataContainer:
      return QColor(153, 97, 242);
    case ItemType::AttributeMatrix:
      return QColor(0, 142, 30);
    case ItemType::AttributeArray:
      return QColor(215, 104, 0);
    case ItemType::RootItem:
      return QColor(100, 100, 100);
    case ItemType::Unknown:
      return QColor(215, 0, 0);
  }

  return QColor(255, 255, 255);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor DataStructureItem::foregroundColor()
{
  switch(m_ItemType)
  {
    case ItemType::DataContainerArray:
      return QColor(255, 255, 255);
    case ItemType::DataContainer:
      return QColor(255, 255, 255);
    case ItemType::AttributeMatrix:
      return QColor(255, 255, 255);
    case ItemType::AttributeArray:
      return QColor(255, 255, 255);
    case ItemType::RootItem:
      return QColor(255, 255, 255);
    case ItemType::Unknown:
      return QColor(255, 255, 255);
  }

  return QColor(0,0,0);
}
