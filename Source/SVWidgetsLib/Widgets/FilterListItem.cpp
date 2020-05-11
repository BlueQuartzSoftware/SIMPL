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
#include "FilterListItem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListItem::FilterListItem(const QString& name, FilterListItem* parent)
: m_Name(name)
, m_ItemTooltip("")
, m_Icon(QIcon())
, m_ParentItem(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListItem::~FilterListItem()
{
  qDeleteAll(m_ChildItems);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListItem* FilterListItem::child(int number)
{
  return m_ChildItems.value(number);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FilterListItem::childCount() const
{
  return m_ChildItems.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FilterListItem::childNumber() const
{
  if(m_ParentItem != nullptr)
  {
    return m_ParentItem->m_ChildItems.indexOf(const_cast<FilterListItem*>(this));
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterListItem::insertChild(int position, FilterListItem* child)
{
  m_ChildItems.insert(position, child);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterListItem::insertChildren(int position, int count, int columns)
{
  if(position < 0 || position > m_ChildItems.size())
  {
    return false;
  }

  for(int row = 0; row < count; ++row)
  {
    FilterListItem* item = new FilterListItem("", this);
    insertChild(position, item);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListItem* FilterListItem::parent()
{
  return m_ParentItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterListItem::removeChild(int position)
{
  m_ChildItems.removeAt(position);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterListItem::removeChildren(int position, int count)
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
void FilterListItem::setParent(FilterListItem* parent)
{
  m_ParentItem = parent;
}

// -----------------------------------------------------------------------------
void FilterListItem::setName(const QString& value)
{
  m_Name = value;
}

// -----------------------------------------------------------------------------
QString FilterListItem::getName() const
{
  return m_Name;
}

// -----------------------------------------------------------------------------
void FilterListItem::setItemTooltip(const QString& value)
{
  m_ItemTooltip = value;
}

// -----------------------------------------------------------------------------
QString FilterListItem::getItemTooltip() const
{
  return m_ItemTooltip;
}

// -----------------------------------------------------------------------------
void FilterListItem::setClassName(const QString& value)
{
  m_ClassName = value;
}

// -----------------------------------------------------------------------------
QString FilterListItem::getClassName() const
{
  return m_ClassName;
}

// -----------------------------------------------------------------------------
void FilterListItem::setIcon(const QIcon& value)
{
  m_Icon = value;
}

// -----------------------------------------------------------------------------
QIcon FilterListItem::getIcon() const
{
  return m_Icon;
}

// -----------------------------------------------------------------------------
void FilterListItem::setItemType(const FilterListItem::ItemType& value)
{
  m_ItemType = value;
}

// -----------------------------------------------------------------------------
FilterListItem::ItemType FilterListItem::getItemType() const
{
  return m_ItemType;
}
