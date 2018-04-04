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

#ifndef _databrowseritem_h_
#define _databrowseritem_h_

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/QVector>

#include <QtGui/QIcon>


#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SVWidgetsLib/SVWidgetsLib.h"




class SVWidgetsLib_EXPORT DataStructureItem
{
  public:
    enum ColumnData
    {
      Name,
      Path
    };

    using EnumType = unsigned int;

    enum class ItemType : EnumType
    {
      DataContainerArray = 0,
      DataContainer = 1,
      AttributeMatrix = 2,
      AttributeArray = 3,
      RootItem = 4,
      Unknown = 5
    };

    /**
     * @brief DataStructureItem
     * @param data
     * @param itemType
     * @param parent
     */
    DataStructureItem(const QVector<QVariant>& data, ItemType itemType, DataStructureItem* parent = 0);

    virtual ~DataStructureItem();

    ItemType itemType();
    void setItemType(ItemType itemType);

    QColor backgroundColor();
    QColor foregroundColor();

    DataStructureItem* child(int number);
    DataStructureItem* parent();

    int childCount() const;
    int columnCount() const;

    QVariant data(int column) const;
    bool setData(int column, const QVariant& value);

    QString getItemTooltip();
    bool setItemTooltip(const QString& value);

    QIcon getIcon();
    bool setIcon(const QIcon& icon);

    bool needsToBeExpanded();
    void setNeedsToBeExpanded(bool value);

    bool insertChild(int position, DataStructureItem* child);
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);

    bool removeChild(int position);
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);

    int childNumber() const;

    void setParent(DataStructureItem* parent);

    static QString TopLevelString();

  private:
    QList<DataStructureItem*>               m_ChildItems;
    QVector<QVariant>                   m_ItemData;
    DataStructureItem*                      m_ParentItem;
    bool                                m_ItemHasErrors;
    QString                             m_ItemTooltip;
    bool                                m_NeedsToBeExpanded;
    QIcon                               m_Icon;
    ItemType                            m_ItemType;

    DataStructureItem(const DataStructureItem&);    // Copy Constructor Not Implemented
    void operator=(const DataStructureItem&);       // Move assignment Not Implemented
};

#endif // DataStructureItem_H
