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

#include <QtCore/QFileInfo>

#include <QtWidgets/QLineEdit>

#include <QtGui/QIntValidator>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QStaticText>

#include "SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h"
#include "SVWidgetsLib/Widgets/DataStructureItem.h"
#include "SVWidgetsLib/Widgets/DataStructureItemDelegate.h"
#include "SVWidgetsLib/Widgets/DataStructureModel.h"
#include "SVWidgetsLib/QtSupport/QtSStyles.h"

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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureItemDelegate::~DataStructureItemDelegate() = default;

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
  //QFont font = QtSStyles::GetCategoryFont();
  QStyleOptionViewItem op(option);
  //op.font = font;
  painter->setPen(QPen());
  painter->setBrush(QBrush());
  painter->setFont(op.font);

  // Get DataArrayPath
  QVariant var = index.model()->itemData(index)[Qt::DisplayRole];
  if(false == var.isValid())
  {
    return;
  }
  QString text = var.toString();
  DataArrayPath path = getDataArrayPath(index);

  bool filterData = (m_ReqType != DataArrayPath::DataType::None);
  bool isCreatedPath = std::find(m_CreatedPaths.begin(), m_CreatedPaths.end(), path) != m_CreatedPaths.end();

  // Check if the view is being filtered
  if(filterData)
  {
    bool matchesReqs = pathMatchesReqs(path);
    QColor color = ::InvalidColor;
    QPen pen;

    int radius = 1;
    int rounded = 4;
    int r2 = radius / 2;
    QRect rect = option.rect;
    QRect borderRect(rect.x() + r2, rect.y() - r2, rect.width() - radius, rect.height() - radius);

    // Check if the current path matches the requirements
    if(matchesReqs)
    {
      color = DataArrayPathSelectionWidget::GetActiveColor(path.getDataType());
      
      if(false == isCreatedPath)
      {
        // Set text color white
        op.palette.setColor(QPalette::Normal, QPalette::WindowText, QColor(255, 255, 255));

        QBrush brush(color);
        painter->setBrush(brush);
      }
      
      pen.setColor(color);
      pen.setWidth(radius);
      painter->setPen(pen);

      painter->drawRoundedRect(borderRect, rounded, rounded);

      if(false == isCreatedPath)
      {
        painter->setPen(QColor(255, 255, 255));
      }
    }
    else
    {
      // Set text color grey
      op.palette.setColor(QPalette::Normal, QPalette::WindowText, color);
    }
  }
  else
  {
    if(isCreatedPath)
    {
      QColor color = DataArrayPathSelectionWidget::GetActiveColor(path.getDataType());
      op.palette.setColor(QPalette::Normal, QPalette::WindowText, color);
    }
  }

  // Draw Text
  int textOffset = 4;
  QPoint textPoint(op.rect.x() + textOffset, op.rect.y());
  painter->setPen(op.palette.color(QPalette::Normal, QPalette::WindowText));
  // drawStaticText renders rich text
  painter->drawStaticText(textPoint, text);

  // Draw icon for created paths
  if(isCreatedPath)
  {
    QFontMetrics fm(op.font);
    int textWidth = fm.width(text);

    QRect imgRect;
    int xOffset = 5;
    int imgSize = op.rect.height();
    imgRect.setX(op.rect.x() + textWidth + textOffset + xOffset);
    imgRect.setY(op.rect.y());
    imgRect.setWidth(imgSize);
    imgRect.setHeight(imgSize);

    // Create placeholder image for marking created paths
    QImage image(":delete_plain.png");
    painter->drawImage(imgRect, image);
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
