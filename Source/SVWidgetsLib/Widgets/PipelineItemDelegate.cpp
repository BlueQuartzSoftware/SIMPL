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

#include <QtCore/QFileInfo>

#include <QtWidgets/QLineEdit>

#include <QtGui/QIntValidator>
#include <QtGui/QPainter>

#include <QtWidgets/QStylePainter>

#include "SVWidgetsLib/Widgets/PipelineItem.h"
#include "SVWidgetsLib/Widgets/PipelineItemDelegate.h"
#include "SVWidgetsLib/Widgets/PipelineModel.h"
#include "SVWidgetsLib/Widgets/SVPipelineView.h"
#include "SVWidgetsLib/QtSupport/QtSStyles.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineItemDelegate::PipelineItemDelegate(SVPipelineView* view)
: QStyledItemDelegate(nullptr)
, m_View(view)
{
  connect(&m_AnimationTimer, SIGNAL(timeout()), this, SLOT(updateBorderThickness()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineItemDelegate::~PipelineItemDelegate() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSize PipelineItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
  return {option.rect.width(), 36};
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  const PipelineModel* model = getPipelineModel(index);
  painter->setRenderHint(QPainter::Antialiasing);

  PipelineItem::WidgetState wState = model->widgetState(index);
  PipelineItem::PipelineState pState = model->pipelineState(index);
  PipelineItem::ErrorState eState = model->errorState(index);

  AbstractFilter::Pointer filter = model->filter(index);
  QString grpName = filter->getGroupName();

  QColor grpColor = QtSStyles::ColorForFilterGroup(grpName);

  QColor widgetBackgroundColor;
  QColor labelColor;
  QColor indexBackgroundColor;
  QColor bgColor = grpColor;
  QColor disabledBgColor = QColor(124, 124, 124);

  QColor hoveredColor = grpColor;
  hoveredColor.setRedF((hoveredColor.redF() * 1.10 > 1.0) ? 1.0 : hoveredColor.redF() * 1.10);
  hoveredColor.setGreenF((hoveredColor.greenF() * 1.10 > 1.0) ? 1.0 : hoveredColor.greenF() * 1.10);
  hoveredColor.setBlueF((hoveredColor.blueF() * 1.10 > 1.0) ? 1.0 : hoveredColor.blueF() * 1.10);
  if(model->isHovering(index) == true)
  {
    bgColor = hoveredColor;
  }
  switch(wState)
  {
  case PipelineItem::WidgetState::Ready:
    widgetBackgroundColor = bgColor;
    labelColor = QColor(190, 190, 190);
    indexBackgroundColor = QColor(48, 48, 48);
    break;
  case PipelineItem::WidgetState::Executing:
    widgetBackgroundColor = QColor(130, 130, 130);
    labelColor = QColor(190, 190, 190);
    indexBackgroundColor = QColor(6, 140, 190);
    break;
  case PipelineItem::WidgetState::Completed:
    widgetBackgroundColor = bgColor.name();
    labelColor = QColor(190, 190, 190);
    indexBackgroundColor = QColor(6, 118, 6);
    break;
  case PipelineItem::WidgetState::Disabled:
    bgColor = disabledBgColor;
    widgetBackgroundColor = disabledBgColor.name();
    labelColor = QColor(190, 190, 190);
    indexBackgroundColor = QColor(96, 96, 96);
    break;
  }
  QColor selectedColor = QColor::fromHsv(bgColor.hue(), 100, 120);

  // Do not change the background color if the widget is disabled.
  if(wState != PipelineItem::WidgetState::Disabled)
  {
    switch(pState)
    {
    case PipelineItem::PipelineState::Running:
      widgetBackgroundColor = selectedColor.name();
      labelColor = QColor(190, 190, 190);
      break;
    case PipelineItem::PipelineState::Stopped:
      widgetBackgroundColor = bgColor.name();
      labelColor = QColor(0, 0, 0);
      break;
    case PipelineItem::PipelineState::Paused:
      widgetBackgroundColor = QColor(160, 160, 160);
      labelColor = QColor(0, 0, 0);
      break;
    }
  }

  switch(eState)
  {
  case PipelineItem::ErrorState::Ok:

    break;
  case PipelineItem::ErrorState::Error:
    indexBackgroundColor = QColor(179, 2, 5);
    break;
  case PipelineItem::ErrorState::Warning:
    indexBackgroundColor = QColor(215, 197, 1);
    break;
  }

  QColor indexFontColor(242, 242, 242);
  QFont font = QtSStyles::GetHumanLabelFont();

#if defined(Q_OS_MAC)
  font.setPointSize(font.pointSize() - 4);
#elif defined(Q_OS_WIN)
  font.setPointSize(font.pointSize() - 3);
#else
  font.setPointSize(font.pointSize() - 1);
#endif

  int maxHeight = option.rect.height();
  QFontMetrics fontMetrics(font);
  int fontHeight = fontMetrics.height();
  int fontMargin = ((maxHeight - fontHeight) / 2) - 1;

  int indexFontWidth = fontMetrics.width(QString::number(model->getMaxFilterCount()));

  painter->setFont(font);

  // back fill with RED so we know if we missed something
  // painter->fillRect(rect(), QColor(255, 0, 0));

  const int textMargin = 6;
  const int indexBoxWidth = 35;

  // Draw the Index area
  QRect rect = option.rect;
  QRect indexRect = option.rect;
  indexRect.setWidth(2 * textMargin + indexFontWidth);
  painter->fillRect(indexRect, indexBackgroundColor);

  // Draw the Title area
  QRect coloredRect(2 * textMargin + indexFontWidth, rect.y(), rect.width() - (2 * textMargin + indexFontWidth), rect.height()); // +4? without it it does not paint to the edge
  painter->fillRect(coloredRect, widgetBackgroundColor);

  // Draw the Index number
  painter->setPen(QPen(indexFontColor));
  QString number = getFilterIndexString(index); // format the index number with a leading zero
  painter->drawText(rect.x() + textMargin, rect.y() + fontMargin + fontHeight, number);

  // Compute the Width to draw the text based on the visibility of the various buttons
  int fullWidth = rect.width() - indexBoxWidth;
  int allowableWidth = fullWidth;

//  QModelIndex deleteIndex = model->index(index.row(), PipelineItem::DeleteBtn, index.parent());
//  QWidget* deleteBtn = m_View->indexWidget(deleteIndex);

//  if(deleteBtn->isVisible())
//  {
//    allowableWidth -= deleteBtn->width();
//  }

//  QModelIndex disableIndex = model->index(index.row(), PipelineItem::DisableBtn, index.parent());
//  QWidget* disableBtn = m_View->indexWidget(disableIndex);
//  if(disableBtn->isVisible())
//  {
//    allowableWidth -= disableBtn->width();
//  }
  // QString elidedHumanLabel = fontMetrics.elidedText(m_FilterHumanLabel, Qt::ElideRight, allowableWidth);
  int humanLabelWidth = fontMetrics.width(filter->getHumanLabel());

  // Draw the filter human label
  painter->setPen(QPen(labelColor));
  font.setWeight(QFont::Normal);
  painter->setFont(font);

  // Compute a Fade out of the text if it is too long to fit in the widget
  if(humanLabelWidth > allowableWidth)
  {
    QRect fadedRect = coloredRect;
    fadedRect.setWidth(fullWidth);
    if(model->isHovering(index) == true)
    {
      fadedRect.setWidth(allowableWidth);
    }

    QLinearGradient gradient(fadedRect.topLeft(), fadedRect.topRight());
    gradient.setColorAt(0.8, labelColor);
    gradient.setColorAt(1.0, QColor(0, 0, 0, 10));

    QPen pen;
    pen.setBrush(QBrush(gradient));
    painter->setPen(pen);
  }

  painter->drawText(rect.x() + indexBoxWidth + textMargin, rect.y() + fontMargin + fontHeight, filter->getHumanLabel());

  // If the filter widget is selected, draw a border around it.
  if(model->isSelected(index) == true)
  {
    QColor selectedColor = QColor::fromHsv(bgColor.hue(), 180, 150);
    QPainterPath path;
    path.addRect(rect);
    QPen pen(selectedColor, m_BorderThickness);
    painter->setPen(pen);
    painter->drawPath(path);
    if(m_BorderThickness < 1.0)
    {
//      m_AnimationTimer.start(33);
    }
  }
  else
  {
//    m_AnimationTimer.stop();
  }

  QStyledItemDelegate::paint(painter, option, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineItemDelegate::getFilterIndexString(const QModelIndex &index) const
{
  const PipelineModel* model = getPipelineModel(index);
  int numFilters = model->rowCount();
  int i = index.row() + 1;

  if(numFilters < 10)
  {
    numFilters = 11;
  }
  QString numStr = QString::number(i);

  if(numFilters > 9)
  {
    int mag = 0;
    int max = numFilters;
    while(max > 0)
    {
      mag++;
      max = max / 10;
    }
    numStr = "";             // Clear the string
    QTextStream ss(&numStr); // Create a QTextStream to set up the padding
    ss.setFieldWidth(mag);
    ss.setPadChar('0');
    ss << i;
  }
  QString paddedIndex = numStr;

  return paddedIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineItemDelegate::updateBorderThickness()
{
  m_BorderThickness += m_BorderIncrement;
  if(m_BorderThickness > m_SelectionBorderWidth)
  {
    // m_BorderIncrement = -1.0;
    m_AnimationTimer.stop();
  }
  else if(m_BorderThickness < 0.0)
  {
    m_BorderIncrement = 1.0;
    m_BorderThickness = 0.0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const PipelineModel* PipelineItemDelegate::getPipelineModel(const QModelIndex &index) const
{
  return dynamic_cast<const PipelineModel*>(index.model());
}
