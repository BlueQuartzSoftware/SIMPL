/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5280
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DataPathLabel.h"

#include <algorithm>

#include <QtWidgets/QStyleOptionFrame>
#include <QtGui/QPainter>

#include "SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor DataPathLabel::GetBackgroundColor(DataArrayPathHelper::DataType type)
{
  // Remain consistent with DataArrayPathSelectionWidget
  return DataArrayPathSelectionWidget::GetActiveColor(type);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor DataPathLabel::GetInactiveColor()
{
  // Remain consistent with DataArrayPathSelectionWidget
  return DataArrayPathSelectionWidget::GetActiveColor(DataArrayPathHelper::DataType::None);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataPathLabel::DataPathLabel(QWidget* parent)
: QLabel(parent)
{
  setAttribute(Qt::WA_MacShowFocusRect, false);
  // setAutoFillBackground(true);
  // setFrameShape(QFrame::StyledPanel);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataPathLabel::~DataPathLabel() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathHelper::DataType DataPathLabel::getDataType() const
{
  return m_DataType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataPathLabel::setDataType(DataArrayPathHelper::DataType type)
{
  m_DataType = type;
  ensurePolished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataPathLabel::getDataTypei() const
{
  return static_cast<int>(m_DataType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataPathLabel::getYMargin() const
{
  ensurePolished();

  // This roundabout calculation is to solve the problem where methods designed to return the margins or content rect
  // after the stylesheet has been applied are not returning the correct result.
  int yMargin;
  {
    QStyleOptionFrame option;
    option.initFrom(this);

    QRect contentsRect1 = style()->subElementRect(QStyle::SE_PushButtonLayoutItem, &option, this);
    QRect contentsRect2 = style()->subElementRect(QStyle::SE_PushButtonContents, &option, this);

    int height1 = contentsRect1.height();
    int height2 = contentsRect2.height();
    yMargin = (height1 - height2) / 2;
  }

  return yMargin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRect DataPathLabel::getStyledContentsRect() const
{
  ensurePolished();

  QStyleOptionFrame option;
  option.initFrom(this);

  return style()->subElementRect(QStyle::SE_ShapedFrameContents, &option, this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRect DataPathLabel::getStyledBorderRect() const
{
  ensurePolished();

  QStyleOptionFrame option;
  option.initFrom(this);

  QRect styledRect = style()->subElementRect(QStyle::SE_LabelLayoutItem, &option, this);
  if(styledRect.isValid())
  {
    return styledRect;
  }

  // return the standard rect if the styled rect is not valid
  return rect();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// void DataPathLabel::paintEvent(QPaintEvent* event)
//{
//  const int radius = 4;
//  const int rectWidth = getStyledBorderRect().width() - margin() * 2;
//  const int yMargin = std::max(getYMargin(), 0);
//
//  QRect fillRect(margin(), yMargin / 2, rectWidth, rectWidth - yMargin);
//  QColor fillColor = isEnabled() ? GetBackgroundColor(getDataType()) : GetInactiveColor();
//
//  QPainter painter{ this };
//  painter.setPen(Qt::PenStyle::NoPen);
//  painter.setBrush(fillColor);
//  painter.drawRoundedRect(rect(), radius, radius);
//  painter.drawRect(fillRect.x(), fillRect.y(), fillRect.width(), fillRect.height());
//
//  // Call parent class's paint event
//  QLabel::paintEvent(event);
//}
