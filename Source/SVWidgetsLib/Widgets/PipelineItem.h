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

#pragma once

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/QVector>

#include <QtGui/QIcon>

#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class FilterInputWidget;

class SVWidgetsLib_EXPORT PipelineItem
{
public:
  PipelineItem(const QVector<QVariant>& data, PipelineItem* parent = nullptr);
  virtual ~PipelineItem();

  /**
   * @brief Setter property for FilterInputWidget
   */
  void setFilterInputWidget(FilterInputWidget* value);
  /**
   * @brief Getter property for FilterInputWidget
   * @return Value of FilterInputWidget
   */
  FilterInputWidget* getFilterInputWidget() const;

  /**
   * @brief Setter property for FilterEnabled
   */
  void setFilterEnabled(bool value);
  /**
   * @brief Getter property for FilterEnabled
   * @return Value of FilterEnabled
   */
  bool getFilterEnabled() const;

  void setActivePipeline(bool value);
  bool isActivePipeline() const;

  void setPipelineSaved(bool value);
  bool isPipelineSaved() const;

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
   * @brief Setter property for Expanded
   */
  void setExpanded(bool value);
  /**
   * @brief Getter property for Expanded
   * @return Value of Expanded
   */
  bool getExpanded() const;

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
   * @brief Setter property for DropIndicatorText
   */
  void setDropIndicatorText(const QString& value);
  /**
   * @brief Getter property for DropIndicatorText
   * @return Value of DropIndicatorText
   */
  QString getDropIndicatorText() const;

  /**
   * @brief Setter property for BorderSize
   */
  void setBorderSize(int value);
  /**
   * @brief Getter property for BorderSize
   * @return Value of BorderSize
   */
  int getBorderSize() const;

  /**
   * @brief Setter property for Size
   */
  void setSize(const QSize& value);
  /**
   * @brief Getter property for Size
   * @return Value of Size
   */
  QSize getSize() const;

  /**
   * @brief Setter property for Height
   */
  void setHeight(int value);
  /**
   * @brief Getter property for Height
   * @return Value of Height
   */
  int getHeight() const;

  /**
   * @brief Setter property for Width
   */
  void setWidth(int value);
  /**
   * @brief Getter property for Width
   * @return Value of Width
   */
  int getWidth() const;

  /**
   * @brief Setter property for XOffset
   */
  void setXOffset(int value);
  /**
   * @brief Getter property for XOffset
   * @return Value of XOffset
   */
  int getXOffset() const;

  /**
   * @brief Setter property for YOffset
   */
  void setYOffset(int value);
  /**
   * @brief Getter property for YOffset
   * @return Value of YOffset
   */
  int getYOffset() const;

  static const int MaxHeight = 28;

  enum AnimationType
  {
    None,
    Add,
    Remove
  };

  /**
   * @brief Setter property for CurrentAnimationType
   */
  void setCurrentAnimationType(const PipelineItem::AnimationType& value);
  /**
   * @brief Getter property for CurrentAnimationType
   * @return Value of CurrentAnimationType
   */
  PipelineItem::AnimationType getCurrentAnimationType() const;

  enum PipelineItemData
  {
    Contents
  };

  using EnumType = unsigned int;

  enum class WidgetState : EnumType
  {
    Ready = 0,     //!<
    Executing = 1, //!<
    Completed = 2, //!<
    Disabled = 3
  };
  /**
   * @brief Setter property for WidgetState
   */
  void setWidgetState(const PipelineItem::WidgetState& value);
  /**
   * @brief Getter property for WidgetState
   * @return Value of WidgetState
   */
  PipelineItem::WidgetState getWidgetState() const;

  enum class PipelineState : EnumType
  {
    Running = 0,
    Stopped = 1,
    Paused = 4,
  };
  /**
   * @brief Setter property for PipelineState
   */
  void setPipelineState(const PipelineItem::PipelineState& value);
  /**
   * @brief Getter property for PipelineState
   * @return Value of PipelineState
   */
  PipelineItem::PipelineState getPipelineState() const;

  enum class ErrorState : EnumType
  {
    Ok = 0,
    Error = 1,
    Warning = 2,
  };
  /**
   * @brief Setter property for ErrorState
   */
  void setErrorState(const PipelineItem::ErrorState& value);
  /**
   * @brief Getter property for ErrorState
   * @return Value of ErrorState
   */
  PipelineItem::ErrorState getErrorState() const;

  enum class ItemType : EnumType
  {
    Pipeline,
    Filter,
    DropIndicator,
    Unknown
  };
  /**
   * @brief Setter property for ItemType
   */
  void setItemType(const PipelineItem::ItemType& value);
  /**
   * @brief Getter property for ItemType
   * @return Value of ItemType
   */
  PipelineItem::ItemType getItemType() const;

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
  FilterInputWidget* m_FilterInputWidget = nullptr;

  bool m_FilterEnabled = true;
  QIcon m_Icon;
  bool m_Expanded = false;
  QString m_ItemTooltip = "";
  QString m_DropIndicatorText = "";
  int m_BorderSize = 0;
  QSize m_Size = QSize(0, MaxHeight);
  int m_Height = 0;
  int m_Width = 0;
  int m_XOffset = 0;
  int m_YOffset = 0;
  PipelineItem::AnimationType m_CurrentAnimationType = {};
  PipelineItem::WidgetState m_WidgetState = {};
  PipelineItem::PipelineState m_PipelineState = {};
  PipelineItem::ErrorState m_ErrorState = {};
  PipelineItem::ItemType m_ItemType = {};

  QList<PipelineItem*> m_ChildItems;
  QVector<QVariant> m_ItemData;
  AbstractFilter::Pointer m_Filter = nullptr;
  PipelineItem* m_ParentItem = nullptr;

  bool m_ActivePipeline = false;
  bool m_PipelineSaved = false;

  void setupFilterInputWidget();

public:
  PipelineItem(const PipelineItem&) = delete;            // Copy Constructor Not Implemented
  PipelineItem(PipelineItem&&) = delete;                 // Move Constructor Not Implemented
  PipelineItem& operator=(const PipelineItem&) = delete; // Copy Assignment Not Implemented
  PipelineItem& operator=(PipelineItem&&) = delete;      // Move Assignment Not Implemented
};
