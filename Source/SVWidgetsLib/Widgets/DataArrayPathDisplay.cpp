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

#include "DataArrayPathDisplay.h"

#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SVWidgetsLib/QtSupport/QtSSettings.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "ui_DataArrayPathDisplay.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QColor DataArrayPathDisplay::GetActiveColor(DataArrayPathHelper::DataType type)
{
  QColor color;
  SVStyle* style = SVStyle::Instance();

  switch(type)
  {
  case DataArrayPathHelper::DataType::DataContainer:
    color = style->getDataArrayPath_DataContainer_color();
    break;
  case DataArrayPathHelper::DataType::AttributeMatrix:
    color = style->getDataArrayPath_AttributeMatrix_color();
    break;
  case DataArrayPathHelper::DataType::DataArray:
    color = style->getDataArrayPath_DataArray_color();
    break;
  case DataArrayPathHelper::DataType::None:
    break;
  }

  return color;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QColor DataArrayPathDisplay::GetCheckedColor(DataArrayPathHelper::DataType type)
{
  QColor color;
  SVStyle* style = SVStyle::Instance();

  switch(type)
  {
  case DataArrayPathHelper::DataType::DataContainer:
    color = style->getDataArrayPath_DataContainer_background_color();
    break;
  case DataArrayPathHelper::DataType::AttributeMatrix:
    color = style->getDataArrayPath_AttributeMatrix_background_color();
    break;
  case DataArrayPathHelper::DataType::DataArray:
    color = style->getDataArrayPath_DataArray_background_color();
    break;
  case DataArrayPathHelper::DataType::None:
    break;
  }

  return color;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathDisplay::DataArrayPathDisplay(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::DataArrayPathDisplay)
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathDisplay::setupGui()
{
  m_Ui->setupUi(this);
  m_Ui->dataConPathLabel->setDataType(DataArrayPathHelper::DataType::DataContainer);
  m_Ui->attrMatPathLabel->setDataType(DataArrayPathHelper::DataType::AttributeMatrix);
  m_Ui->dataArrayPathLabel->setDataType(DataArrayPathHelper::DataType::DataArray);

  setDataArrayPath(DataArrayPath("", "", ""));
  updateStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathDisplay::updateStyle()
{
  // Force update the styling
  style()->unpolish(this);
  style()->polish(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathHelper::DataType DataArrayPathDisplay::getDataType() const
{
  return m_DataType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathDisplay::setDataArrayPath(const DataArrayPath& path)
{
  m_Ui->dataConPathLabel->setText(path.getDataContainerName());
  m_Ui->attrMatPathLabel->setText(path.getAttributeMatrixName());
  m_Ui->dataArrayPathLabel->setText(path.getDataArrayName());

  m_DataType = path.getDataType();
  switch(m_DataType)
  {
  case DataArrayPathHelper::DataType::DataContainer:
    m_Ui->dataConWidget->setVisible(true);
    m_Ui->attrMatWidget->setVisible(false);
    m_Ui->dataArrayPathLabel->setVisible(false);
    break;
  case DataArrayPathHelper::DataType::AttributeMatrix:
    m_Ui->dataConWidget->setVisible(true);
    m_Ui->attrMatWidget->setVisible(true);
    m_Ui->dataArrayPathLabel->setVisible(false);
    break;
  case DataArrayPathHelper::DataType::DataArray:
    m_Ui->dataConWidget->setVisible(true);
    m_Ui->attrMatWidget->setVisible(true);
    m_Ui->dataArrayPathLabel->setVisible(true);
    break;
  case DataArrayPathHelper::DataType::None:
    m_Ui->dataConWidget->setVisible(false);
    m_Ui->attrMatWidget->setVisible(false);
    m_Ui->dataArrayPathLabel->setVisible(false);
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathDisplay::setDataArrayPath(const QString& dcName, const QString& amName, const QString& daName)
{
  setDataArrayPath(DataArrayPath(dcName, amName, daName));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataArrayPathDisplay::getDataArrayPath() const
{
  QString dcName;
  QString amName;
  QString daName;
  switch(m_DataType)
  {
  case DataArrayPathHelper::DataType::DataArray:
    daName = m_Ui->dataArrayPathLabel->text();
    // [[fallthrough]]
  case DataArrayPathHelper::DataType::AttributeMatrix:
    amName = m_Ui->attrMatPathLabel->text();
    // [[fallthrough]]
  case DataArrayPathHelper::DataType::DataContainer:
    dcName = m_Ui->dataConPathLabel->text();
    break;
  case DataArrayPathHelper::DataType::None:
    break;
  }

  return DataArrayPath(dcName, amName, daName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathDisplay::setFilter(AbstractFilter* filter)
{
  m_Filter = filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathDisplay::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathDisplay::afterPreflight()
{
  if(nullptr == m_Filter)
  {
    return;
  }

  if(m_Filter->getDataContainerArray())
  {
    return;
  }

  DataContainerArray::Pointer dca = m_Filter->getDataContainerArray();
  bool found = false;
  switch(getDataType())
  {
  case DataArrayPathHelper::DataType::DataContainer:
    if(dca->getDataContainer(getDataArrayPath()))
    {
      found = true;
    }
    break;
  case DataArrayPathHelper::DataType::AttributeMatrix:
    if(dca->getAttributeMatrix(getDataArrayPath()))
    {
      found = true;
    }
    break;
  case DataArrayPathHelper::DataType::DataArray: {
    AttributeMatrix::Pointer am = dca->getAttributeMatrix(getDataArrayPath());
    if(am && am->getAttributeArray(getDataArrayPath().getDataArrayName()))
    {
      found = true;
    }
  }
  break;
  case DataArrayPathHelper::DataType::None:
    break;
  }

  // Update StyleSheet
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataArrayPathDisplay::getXMargin() const
{
  ensurePolished();

  // This roundabout calculation is to solve the problem where methods designed to return the margins or content rect
  // after the stylesheet has been applied are not returning the correct result.
  int xMargin;
  {
    QStyleOptionButton option;
    option.initFrom(this);

    QRect contentsRect1 = style()->subElementRect(QStyle::SE_PushButtonLayoutItem, &option, this);
    QRect contentsRect2 = style()->subElementRect(QStyle::SE_PushButtonContents, &option, this);

    int width1 = contentsRect1.width();
    int width2 = contentsRect2.width();
    xMargin = (width1 - width2) / 2;
  }

  return xMargin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataArrayPathDisplay::getYMargin() const
{
  ensurePolished();

  // This roundabout calculation is to solve the problem where methods designed to return the margins or content rect
  // after the stylesheet has been applied are not returning the correct result.
  int yMargin;
  {
    QStyleOptionButton option;
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
QRect DataArrayPathDisplay::getStyledContentsRect() const
{
  ensurePolished();

  QStyleOptionButton option;
  option.initFrom(this);

  return style()->subElementRect(QStyle::SE_PushButtonContents, &option, this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRect DataArrayPathDisplay::getStyledBorderRect() const
{
  ensurePolished();

  QStyleOptionButton option;
  option.initFrom(this);

  QRect styledRect = style()->subElementRect(QStyle::SE_PushButtonLayoutItem, &option, this);
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
QSize DataArrayPathDisplay::updatedSizeHint(QSize sizeHint) const
{
  int xMargin = getXMargin();

  // Check Width
  QFontMetrics fm(font());
  int textPadding = xMargin;
  int textWidth = fm.QFONTMETRICS_WIDTH(getDataArrayPath().serialize()) + textPadding;
  int minWidth = textWidth + xMargin + (2 * sizeHint.height());
  if(sizeHint.width() < minWidth)
  {
    sizeHint.setWidth(minWidth);
  }

  return sizeHint;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSize DataArrayPathDisplay::minimumSizeHint() const
{
  ensurePolished();
  QSize minHint = updatedSizeHint(QWidget::minimumSizeHint());

  return minHint;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSize DataArrayPathDisplay::sizeHint() const
{
  ensurePolished();
  QSize hint = updatedSizeHint(QWidget::sizeHint());

  return hint;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayPathDisplay::getPropertyName()
{
  return m_PropName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathDisplay::setPropertyName(QString propName)
{
  m_PropName = propName;
}
