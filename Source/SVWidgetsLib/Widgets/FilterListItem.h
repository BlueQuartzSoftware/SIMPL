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

class SVWidgetsLib_EXPORT FilterListItem
{
public:
  FilterListItem(const QString& name, FilterListItem* parent = 0);
  virtual ~FilterListItem();

  enum ItemType
  {
    Filter,
    Group,
    Unknown
  };

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
   * @brief Setter property for ItemTooltip
   */
  void setItemTooltip(const QString& value);
  /**
   * @brief Getter property for ItemTooltip
   * @return Value of ItemTooltip
   */
  QString getItemTooltip() const;

  /**
   * @brief Setter property for ClassName
   */
  void setClassName(const QString& value);
  /**
   * @brief Getter property for ClassName
   * @return Value of ClassName
   */
  QString getClassName() const;

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
  void setItemType(const FilterListItem::ItemType& value);
  /**
   * @brief Getter property for ItemType
   * @return Value of ItemType
   */
  FilterListItem::ItemType getItemType() const;

  FilterListItem* child(int number);
  FilterListItem* parent();

  int childCount() const;

  bool insertChild(int position, FilterListItem* child);
  bool insertChildren(int position, int count, int columns);

  bool removeChild(int position);
  bool removeChildren(int position, int count);

  int childNumber() const;

  void setParent(FilterListItem* parent);

private:
  QString m_Name = {};
  QString m_ItemTooltip = {};
  QString m_ClassName = {};
  QIcon m_Icon = {};
  FilterListItem::ItemType m_ItemType = {};

  QList<FilterListItem*> m_ChildItems;
  FilterListItem* m_ParentItem;

public:
  FilterListItem(const FilterListItem&) = delete;            // Copy Constructor Not Implemented
  FilterListItem(FilterListItem&&) = delete;                 // Move Constructor Not Implemented
  FilterListItem& operator=(const FilterListItem&) = delete; // Copy Assignment Not Implemented
  FilterListItem& operator=(FilterListItem&&) = delete;      // Move Assignment Not Implemented
};
