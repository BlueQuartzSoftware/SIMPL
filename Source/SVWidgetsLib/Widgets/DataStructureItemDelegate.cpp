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

#include <QtWidgets/QApplication>
#include <QtCore/QFileInfo>

#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QLineEdit>

#include <QtGui/QIntValidator>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QStaticText>

#include "SVWidgetsLib/Widgets/SVStyle.h"
#include "SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h"
#include "SVWidgetsLib/Widgets/DataStructureItem.h"
#include "SVWidgetsLib/Widgets/DataStructureItemDelegate.h"
#include "SVWidgetsLib/Widgets/DataStructureModel.h"

namespace Detail
{
  const QString Delimiter(" / ");
}

namespace
{
  QColor InvalidColor(100, 100, 100);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureItemDelegate::DataStructureItemDelegate(QObject* parent)
: QStyledItemDelegate(parent)
{
  createNewPathIcons();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureItemDelegate::~DataStructureItemDelegate() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItemDelegate::createNewPathIcons()
{
  QImage baseImage(SIMPLView::DataArrayPath::BaseNewPathIcon);
  QImage dcImage = baseImage.copy();
  QImage amImage = baseImage.copy();
  QImage daImage = baseImage.copy();
  QImage invalidImage = baseImage.copy();

  QColor invalidColor = DataArrayPathSelectionWidget::GetActiveColor(DataArrayPath::DataType::None);
  QColor dcColor = DataArrayPathSelectionWidget::GetActiveColor(DataArrayPath::DataType::DataContainer);
  QColor amColor = DataArrayPathSelectionWidget::GetActiveColor(DataArrayPath::DataType::AttributeMatrix);
  QColor daColor = DataArrayPathSelectionWidget::GetActiveColor(DataArrayPath::DataType::DataArray);

  int height = baseImage.height();
  int width = baseImage.width();

  for(int y = 0; y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
      QColor color = baseImage.pixelColor(x, y);
      qreal alpha = color.alphaF();

      QColor dcPixel = dcColor;
      QColor amPixel = amColor;
      QColor daPixel = daColor;
      QColor invalidPixel = invalidColor;

      dcPixel.setAlphaF(alpha);
      amPixel.setAlphaF(alpha);
      daPixel.setAlphaF(alpha);
      invalidPixel.setAlphaF(alpha);

      dcImage.setPixelColor(x, y, dcPixel);
      amImage.setPixelColor(x, y, amPixel);
      daImage.setPixelColor(x, y, daPixel);
      invalidImage.setPixelColor(x, y, invalidPixel);
    }
  }

  m_CreatedDcIcon = QIcon(QPixmap::fromImage(dcImage));
  m_CreatedAmIcon = QIcon(QPixmap::fromImage(amImage));
  m_CreatedDaIcon = QIcon(QPixmap::fromImage(daImage));
  m_CreatedInvalidIcon = QIcon(QPixmap::fromImage(invalidImage));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItemDelegate::setActiveFilter(AbstractFilter::Pointer filter)
{
  m_Filter = filter;
  if(filter)
  {
    m_CreatedPaths = m_Filter->getCreatedPaths();
  }
  else
  {
    m_CreatedPaths.clear();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItemDelegate::setViewRequirements(DataContainerSelectionFilterParameter::RequirementType reqs)
{
  m_DcReqs = reqs;
  m_ReqType = DataArrayPath::DataType::DataContainer;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItemDelegate::setViewRequirements(AttributeMatrixSelectionFilterParameter::RequirementType reqs)
{
  m_AmReqs = reqs;
  m_ReqType = DataArrayPath::DataType::AttributeMatrix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItemDelegate::setViewRequirements(DataArraySelectionFilterParameter::RequirementType reqs)
{
  m_DaReqs = reqs;
  m_ReqType = DataArrayPath::DataType::DataArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItemDelegate::clearRequirements()
{
  m_ReqType = DataArrayPath::DataType::None;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataStructureItemDelegate::isFiltered()
{
  return m_ReqType != DataArrayPath::DataType::None;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* DataStructureItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* editor = new QLineEdit(parent);
  return editor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  QString value = index.model()->data(index, Qt::DisplayRole).toString();
  QLineEdit* line = static_cast<QLineEdit*>(editor);
  line->setText(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  DataStructureModel* bModel = qobject_cast<DataStructureModel*>(model);

  QLineEdit* line = static_cast<QLineEdit*>(editor);
  QString value = line->text();

  if(value.isEmpty() == false)
  {
    QModelIndex bIndex = bModel->index(index.row(), DataStructureItem::Name, index.parent());
    bModel->setData(bIndex, value, Qt::DisplayRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  editor->setGeometry(option.rect);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  // QStyledItemDelegate::paint(painter, op, index);

  // Place any painting code here
  QStyleOptionViewItem op(option);
  painter->setPen(QPen());
  painter->setBrush(QBrush());
  painter->setFont(op.font);

  SVStyle* styles = SVStyle::Instance();

  QColor bgColor;
  if (option.state & QStyle::State_Selected)
  {
    bgColor = styles->getQTreeViewItemSelectedActive_background_color();
  }
  else if (option.state & QStyle::State_MouseOver)
  {
    bgColor = styles->getQTreeViewItemHover_background_color();
  }
  else
  {
    bgColor = styles->getQTreeViewItem_background_color();
  }

  if (!bgColor.isValid())
  {
    if (option.state & QStyle::State_Selected)
    {
      bgColor = option.palette.color(QPalette::Highlight);
    }
    else if (option.features & QStyleOptionViewItem::Alternate)
    {
      bgColor = option.palette.color(QPalette::AlternateBase);
    }
    else
    {
      bgColor = option.palette.color(QPalette::Base);
    }
  }

  painter->fillRect(option.rect, bgColor);

  // Get DataArrayPath
  QVariant var = index.model()->itemData(index)[Qt::DisplayRole];
  if(false == var.isValid())
  {
    return;
  }
  QString text = var.toString();
  DataArrayPath path = getDataArrayPath(index);
  int textOffset = 4;

  // Get mouse position for mouseOver effects
  QPoint mousePos = QCursor::pos();
  if(dynamic_cast<QWidget*>(parent()))
  {
    mousePos = dynamic_cast<QWidget*>(parent())->mapFromGlobal(mousePos);
  }

  bool filterData = (m_ReqType != DataArrayPath::DataType::None);
  bool isCreatedPath = std::find(m_CreatedPaths.begin(), m_CreatedPaths.end(), path) != m_CreatedPaths.end();

  // Check for a corresponding icon
  QIcon icon;
  int iconSize = option.rect.height();
  var = index.model()->itemData(index)[Qt::DecorationRole];
  if(var.isValid() && var.canConvert<QIcon>())
  {
    icon = var.value<QIcon>();
    if(icon.isNull())
    {
      iconSize = 0;
    }
  }

  bool pathHighlighted = false;

  // Check if the view is being filtered
  if(filterData)
  {
    bool matchesReqs = pathMatchesReqs(path);
    pathHighlighted = matchesReqs && !isCreatedPath;

    // Check if the current path matches the requirements
    if(!matchesReqs)
    {
      // Set text color
      op.palette.setColor(QPalette::Normal, QPalette::WindowText, ::InvalidColor);
    }
  }
  else
  {
    if(isCreatedPath)
    {
      QColor color = DataArrayPathSelectionWidget::GetActiveColor(path.getDataType());
      op.palette.setColor(QPalette::Normal, QPalette::WindowText, color);
    }
    //else
    //{
    //  pathHighlighted = mouseOver;
    //}
  }

  // Draw the decoration role if available
  if(iconSize > 0)
  {
    QRect iconRect(op.rect.x(), op.rect.y(), option.rect.height(), option.rect.height());
    icon.paint(painter, iconRect);
    textOffset = textOffset + iconSize;
  }

  QFontMetrics fm(op.font);
  int textWidth = fm.width(text);

  // Highlight the path
  if(pathHighlighted)
  {
    // Rounding options
    int radius = 1;
    int rounded = 2;
    int xPadding = 4;
    
    // Rect dimensions
    int xPos = op.rect.x() + textOffset - xPadding;
    int yPos = op.rect.y() - (radius / 2);
    int width = textWidth + 2 * xPadding;
    int height = op.rect.height() - radius;
    QRect borderRect(xPos, yPos, width, height);

    QColor penColor = QColor(0, 0, 0, 0);
    QColor bgColor = DataArrayPathSelectionWidget::GetActiveColor(path.getDataType());
    op.palette.setColor(QPalette::ColorRole::WindowText, QColor(255, 255, 255));


    painter->setPen(penColor);
    painter->setBrush(bgColor);
    painter->drawRoundedRect(borderRect, rounded, rounded);

    // Set transparent brush
    painter->setBrush(QColor(0, 0, 0, 0));
  }

  // Draw Text
  QRect textRect(op.rect.x() + textOffset, op.rect.y() + fm.descent() / 2, op.rect.width(), op.rect.height());
  painter->setPen(op.palette.color(QPalette::Normal, QPalette::WindowText));
  painter->drawText(textRect, text);

  // Draw icon for created paths
  if(isCreatedPath)
  {
    int textWidth = fm.width(text);

    QRect imgRect;
    int xOffset = 5;
    iconSize = op.rect.height();
    imgRect.setX(op.rect.x() + textWidth + textOffset + xOffset);
    imgRect.setY(op.rect.y());
    imgRect.setWidth(iconSize);
    imgRect.setHeight(iconSize);

    switch(path.getDataType())
    {
    case DataArrayPath::DataType::DataContainer:
      m_CreatedDcIcon.paint(painter, imgRect);
      break;
    case DataArrayPath::DataType::AttributeMatrix:
      m_CreatedAmIcon.paint(painter, imgRect);
      break;
    case DataArrayPath::DataType::DataArray:
      m_CreatedDaIcon.paint(painter, imgRect);
      break;
    default:
      m_CreatedInvalidIcon.paint(painter, imgRect);
      break;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataStructureItemDelegate::getDataArrayPath(const QModelIndex& index) const
{
  // Store path as a std::list<QString> based on the index and its parents
  std::list<QString> itemName;
  QModelIndex recursiveIndex = index;
  do
  {
    QVariant var = recursiveIndex.model()->itemData(recursiveIndex)[Qt::DisplayRole];
    if(var.isValid() && var.canConvert<QString>())
    {
      itemName.push_front(var.toString());
    }
    recursiveIndex = recursiveIndex.parent();
  } while(recursiveIndex.isValid());

  // Parse out the DataArrayPath from the std::list<QString>
  DataArrayPath path;
  auto iter = itemName.begin();

  if(iter != itemName.end())
  {
    path.setDataContainerName(*iter);
    iter++;
  }
  if(iter != itemName.end())
  {
    path.setAttributeMatrixName(*iter);
    iter++;
  }
  if(iter != itemName.end())
  {
    path.setDataArrayName(*iter);
    iter++;
  }

  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataStructureItemDelegate::pathMatchesReqs(DataArrayPath path) const
{
  switch(m_ReqType)
  {
  case DataArrayPath::DataType::DataContainer:
    return DataArrayPathSelectionWidget::CheckPathRequirements(m_Filter.get(), path, m_DcReqs);
  case DataArrayPath::DataType::AttributeMatrix:
    return DataArrayPathSelectionWidget::CheckPathRequirements(m_Filter.get(), path, m_AmReqs);
  case DataArrayPath::DataType::DataArray:
    return DataArrayPathSelectionWidget::CheckPathRequirements(m_Filter.get(), path, m_DaReqs);
  default:
    return false;
  }
}
