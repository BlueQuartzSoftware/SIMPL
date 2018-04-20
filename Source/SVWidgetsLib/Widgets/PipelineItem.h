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

#ifndef _pipelinetreeitem_h_
#define _pipelinetreeitem_h_

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/QVector>

#include <QtGui/QIcon>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class FilterInputWidget;

class SVWidgetsLib_EXPORT PipelineItem
{
  public:
    PipelineItem(const QVector<QVariant>& data, PipelineItem* parent = 0);
    virtual ~PipelineItem();

    SIMPL_INSTANCE_PROPERTY(FilterInputWidget*, FilterInputWidget)
    SIMPL_INSTANCE_PROPERTY(bool, FilterEnabled)
    SIMPL_BOOL_PROPERTY(ActivePipeline)
    SIMPL_BOOL_PROPERTY(PipelineSaved)
    SIMPL_INSTANCE_PROPERTY(QIcon, Icon)
    SIMPL_INSTANCE_PROPERTY(bool, Expanded)
    SIMPL_INSTANCE_PROPERTY(bool, Hovering)
    SIMPL_BOOL_PROPERTY(Selected)
    SIMPL_INSTANCE_PROPERTY(QString, ItemTooltip)

    enum TreeItemData
    {
      Name,
      DisableBtn,
      DeleteBtn
    };

    using EnumType = unsigned int;

    enum class WidgetState : EnumType
    {
      Ready = 0,      //!<
      Executing = 1, //!<
      Completed = 2, //!<
      Disabled = 3
    };
    SIMPL_INSTANCE_PROPERTY(WidgetState, WidgetState)

    enum class PipelineState : EnumType
    {
      Running = 0,
      Stopped = 1,
      Paused = 4,
    };
    SIMPL_INSTANCE_PROPERTY(PipelineState, PipelineState)

    enum class ErrorState : EnumType
    {
        Ok = 0,
        Error = 1,
        Warning = 2,
    };
    SIMPL_INSTANCE_PROPERTY(ErrorState, ErrorState)

    enum class ItemType : EnumType
    {
      Pipeline,
      Filter,
      Unknown
    };
    SIMPL_INSTANCE_PROPERTY(ItemType, ItemType)

    PipelineItem* child(int number);
    PipelineItem* parent();

    int childCount() const;
    int columnCount() const;

    QVariant data(int column) const;
    bool setData(int column, const QVariant& value);

    AbstractFilter::Pointer getFilter();
    void setFilter(AbstractFilter::Pointer filter);

    bool insertChild(int position, PipelineItem* child);
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);

    bool removeChild(int position);
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);

    int childNumber() const;

    void setParent(PipelineItem* parent);

    static QString TopLevelString();

  private:
    QList<PipelineItem*>                    m_ChildItems;
    QVector<QVariant>                       m_ItemData;
    AbstractFilter::Pointer                 m_Filter = nullptr;
    PipelineItem*                           m_ParentItem;

    void setupFilterInputWidget();

    PipelineItem(const PipelineItem&);    // Copy Constructor Not Implemented
    void operator=(const PipelineItem&);  // Operator '=' Not Implemented
};

#endif // _pipelinetreeitem_h_
