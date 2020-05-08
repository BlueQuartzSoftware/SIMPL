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

#pragma once

#include <QtCore/QList>

#include <QtGui/QIcon>

#include "SVWidgetsLib/SVWidgetsLib.h"

class SVWidgetsLib_EXPORT BookmarksItem
{
public:
  BookmarksItem(const QString& name, const QString& path = "", BookmarksItem* parent = 0);
  virtual ~BookmarksItem();

  enum BookmarkItemData
  {
    Contents
  };

  enum class ItemType : unsigned int
  {
    Folder,
    Bookmark
  };

  void setExpanded(bool value);
  bool isExpanded() const;

  /**
   * @brief Setter property for Name
   */
  void setName(const QString& value);
  /**
   * @brief Getter property for Name
   * @return Value of Name
   */
  QString getName() const;

  /**
   * @brief Setter property for Path
   */
  void setPath(const QString& value);
  /**
   * @brief Getter property for Path
   * @return Value of Path
   */
  QString getPath() const;

  /**
   * @brief Setter property for HasErrors
   */
  void setHasErrors(bool value);
  /**
   * @brief Getter property for HasErrors
   * @return Value of HasErrors
   */
  bool getHasErrors() const;

  Q_PROPERTY(bool HasErrors READ getHasErrors WRITE setHasErrors)

  /**
   * @brief Setter property for ItemTooltip
   */
  void setItemTooltip(const QString& value);
  /**
   * @brief Getter property for ItemTooltip
   * @return Value of ItemTooltip
   */
  QString getItemTooltip() const;

  /**
   * @brief Setter property for Icon
   */
  void setIcon(const QIcon& value);
  /**
   * @brief Getter property for Icon
   * @return Value of Icon
   */
  QIcon getIcon() const;

  /**
   * @brief Setter property for ItemType
   */
  void setItemType(const BookmarksItem::ItemType& value);
  /**
   * @brief Getter property for ItemType
   * @return Value of ItemType
   */
  BookmarksItem::ItemType getItemType() const;

  BookmarksItem* child(int number);
  BookmarksItem* parent();

  int childCount() const;

  bool insertChild(int position, BookmarksItem* child);
  bool insertChildren(int position, int count, int columns);

  bool removeChild(int position);
  bool removeChildren(int position, int count);

  int childNumber() const;

  void setParent(BookmarksItem* parent);

  static QString TopLevelString();

private:
  QString m_Name = {};
  QString m_Path = {};
  bool m_HasErrors = {};
  QString m_ItemTooltip = {};
  QIcon m_Icon = {};
  BookmarksItem::ItemType m_ItemType = {};
  bool m_Expanded = false;

  QList<BookmarksItem*> m_ChildItems;
  BookmarksItem* m_ParentItem;

public:
  BookmarksItem(const BookmarksItem&) = delete;            // Copy Constructor Not Implemented
  BookmarksItem(BookmarksItem&&) = delete;                 // Move Constructor Not Implemented
  BookmarksItem& operator=(const BookmarksItem&) = delete; // Copy Assignment Not Implemented
  BookmarksItem& operator=(BookmarksItem&&) = delete;      // Move Assignment Not Implemented
};
