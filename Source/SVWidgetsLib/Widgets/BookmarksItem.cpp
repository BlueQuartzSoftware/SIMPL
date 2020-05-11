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
#include "BookmarksItem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem::BookmarksItem(const QString& name, const QString& path, BookmarksItem* parent)
: m_Name(name)
, m_Path(path)
, m_ItemTooltip("")
, m_Icon(QIcon())
, m_ParentItem(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem::~BookmarksItem()
{
  qDeleteAll(m_ChildItems);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BookmarksItem::TopLevelString()
{
  return QString::fromLatin1("[Top Level]");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem* BookmarksItem::child(int number)
{
  return m_ChildItems.value(number);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksItem::childCount() const
{
  return m_ChildItems.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksItem::childNumber() const
{
  if(m_ParentItem != nullptr)
  {
    return m_ParentItem->m_ChildItems.indexOf(const_cast<BookmarksItem*>(this));
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::insertChild(int position, BookmarksItem* child)
{
  m_ChildItems.insert(position, child);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::insertChildren(int position, int count, int columns)
{
  if(position < 0 || position > m_ChildItems.size())
  {
    return false;
  }

  for(int row = 0; row < count; ++row)
  {
    BookmarksItem* item = new BookmarksItem("", "", this);
    insertChild(position, item);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem* BookmarksItem::parent()
{
  return m_ParentItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::removeChild(int position)
{
  m_ChildItems.removeAt(position);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::removeChildren(int position, int count)
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
void BookmarksItem::setParent(BookmarksItem* parent)
{
  m_ParentItem = parent;
}

// -----------------------------------------------------------------------------
void BookmarksItem::setName(const QString& value)
{
  m_Name = value;
}

// -----------------------------------------------------------------------------
QString BookmarksItem::getName() const
{
  return m_Name;
}

// -----------------------------------------------------------------------------
void BookmarksItem::setPath(const QString& value)
{
  m_Path = value;
}

// -----------------------------------------------------------------------------
QString BookmarksItem::getPath() const
{
  return m_Path;
}

// -----------------------------------------------------------------------------
void BookmarksItem::setHasErrors(bool value)
{
  m_HasErrors = value;
}

// -----------------------------------------------------------------------------
bool BookmarksItem::getHasErrors() const
{
  return m_HasErrors;
}

// -----------------------------------------------------------------------------
void BookmarksItem::setItemTooltip(const QString& value)
{
  m_ItemTooltip = value;
}

// -----------------------------------------------------------------------------
QString BookmarksItem::getItemTooltip() const
{
  return m_ItemTooltip;
}

// -----------------------------------------------------------------------------
void BookmarksItem::setIcon(const QIcon& value)
{
  m_Icon = value;
}

// -----------------------------------------------------------------------------
QIcon BookmarksItem::getIcon() const
{
  return m_Icon;
}

// -----------------------------------------------------------------------------
void BookmarksItem::setItemType(const BookmarksItem::ItemType& value)
{
  m_ItemType = value;
}

// -----------------------------------------------------------------------------
BookmarksItem::ItemType BookmarksItem::getItemType() const
{
  return m_ItemType;
}

// -----------------------------------------------------------------------------
void BookmarksItem::setExpanded(bool value)
{
  m_Expanded = value;
}

// -----------------------------------------------------------------------------
bool BookmarksItem::isExpanded() const
{
  return m_Expanded;
}
