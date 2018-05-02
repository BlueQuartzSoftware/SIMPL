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
#include <QtGui/QMouseEvent>

#include <QtWidgets/QStylePainter>

#include "SVWidgetsLib/Widgets/PipelineItem.h"
#include "SVWidgetsLib/Widgets/PipelineItemDelegate.h"
#include "SVWidgetsLib/Widgets/PipelineModel.h"
#include "SVWidgetsLib/Widgets/SVPipelineView.h"
#include "SVWidgetsLib/QtSupport/QtSStyles.h"

namespace {
  const int k_ButtonSize = 24;
  const int k_TextMargin = 4;

  const QColor k_DropIndicatorWidgetBackgroundColor = QColor(150, 150, 150);
  const QColor k_DropIndicatorIndexBackgroundColor = QColor(48, 48, 48);
  const QColor k_DropIndicatorIndexLabelColor = QColor(190, 190, 190);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineItemDelegate::PipelineItemDelegate(SVPipelineView* view)
  : QStyledItemDelegate(nullptr)
  , m_View(view)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineItemDelegate::~PipelineItemDelegate() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSize PipelineItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  return {0, 36};
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  painter->setRenderHint(QPainter::Antialiasing);

  PipelineModel* model = m_View->getPipelineModel();

  PipelineItem::WidgetState wState = model->widgetState(index);
  PipelineItem::PipelineState pState = model->pipelineState(index);
  PipelineItem::ErrorState eState = model->errorState(index);

  AbstractFilter::Pointer filter = model->filter(index);
  QColor grpColor;
  if (filter.get() != nullptr)
  {
    QString grpName = filter->getGroupName();
    grpColor = QtSStyles::ColorForFilterGroup(grpName);
  }

  QColor widgetBackgroundColor;
  QColor labelColor;
  QColor indexBackgroundColor;
  QColor bgColor = grpColor;
  QColor disabledBgColor = QColor(124, 124, 124);

  bool drawButtons = false;
  if((option.state & QStyle::State_MouseOver) && !(QApplication::mouseButtons() & Qt::LeftButton) )
  {
    QColor hoveredColor = grpColor;
    hoveredColor.setRedF((hoveredColor.redF() * 1.10 > 1.0) ? 1.0 : hoveredColor.redF() * 1.10);
    hoveredColor.setGreenF((hoveredColor.greenF() * 1.10 > 1.0) ? 1.0 : hoveredColor.greenF() * 1.10);
    hoveredColor.setBlueF((hoveredColor.blueF() * 1.10 > 1.0) ? 1.0 : hoveredColor.blueF() * 1.10);
    bgColor = hoveredColor;

    drawButtons = true;
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
  }

  if (model->itemType(index) == PipelineItem::ItemType::DropIndicator)
  {
    indexBackgroundColor = k_DropIndicatorIndexBackgroundColor;
    widgetBackgroundColor = k_DropIndicatorWidgetBackgroundColor;
    labelColor = k_DropIndicatorIndexLabelColor;
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

  QFontMetrics fontMetrics(font);
  int fontHeight = fontMetrics.height();
  int fontMargin = ((option.rect.height() - fontHeight) / 2) - 1;

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
  int humanLabelWidth;
  if (model->itemType(index) == PipelineItem::ItemType::DropIndicator)
  {
    QString dropIndicatorText = model->dropIndicatorText(index);
    humanLabelWidth = fontMetrics.width(dropIndicatorText);
  }
  else
  {
    humanLabelWidth = fontMetrics.width(filter->getHumanLabel());
  }

  // Draw the filter human label
  painter->setPen(QPen(labelColor));
  font.setWeight(QFont::Normal);
  painter->setFont(font);

  // Compute a Fade out of the text if it is too long to fit in the widget
  if(humanLabelWidth > allowableWidth)
  {
    QRect fadedRect = coloredRect;
    fadedRect.setWidth(fullWidth);
    if(option.state & QStyle::State_MouseOver)
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

  if (model->itemType(index) == PipelineItem::ItemType::DropIndicator)
  {
    QString text = model->dropIndicatorText(index);
    painter->drawText(rect.x() + indexBoxWidth + textMargin, rect.y() + fontMargin + fontHeight, text);
  }
  else if (model->itemType(index) == PipelineItem::ItemType::Filter)
  {
    painter->drawText(rect.x() + indexBoxWidth + textMargin, rect.y() + fontMargin + fontHeight, filter->getHumanLabel());
  }

  if (model->itemType(index) == PipelineItem::ItemType::DropIndicator)
  {
    drawButtons = false;
  }

  if (drawButtons == true)
  {
    // Draw the "delete" button
    QRectF deleteBtnRect;
    deleteBtnRect.setX(option.rect.width() - ::k_ButtonSize - ::k_TextMargin);
    deleteBtnRect.setY(option.rect.y() + ( (option.rect.height() / 2) - (::k_ButtonSize / 2) ) );
    deleteBtnRect.setWidth(::k_ButtonSize);
    deleteBtnRect.setHeight(::k_ButtonSize);

    QPoint mousePos = QCursor::pos();
    mousePos = m_View->viewport()->mapFromGlobal(mousePos);

    QPixmap deleteBtnPixmap;
    if(deleteBtnRect.contains(mousePos))
    {
      deleteBtnPixmap = QPixmap(":/trash_hover.png");
      if (painter->device()->devicePixelRatio() == 2)
      {
        deleteBtnPixmap = QPixmap(":/trash_hover@2x.png");
      }
    }
    else
    {
      deleteBtnPixmap = QPixmap(":/trash.png");
      if (painter->device()->devicePixelRatio() == 2)
      {
        deleteBtnPixmap = QPixmap(":/trash@2x.png");
      }
    }

    painter->drawPixmap(deleteBtnRect.center().x() - (deleteBtnRect.width() / 2), deleteBtnRect.center().y() - (deleteBtnRect.height() / 2 + 1), deleteBtnPixmap);  // y is 1px offset due to how the images were cut

    // Draw the "disable" button
    QRectF disableBtnRect;
    disableBtnRect.setX(deleteBtnRect.x() - ::k_TextMargin - ::k_ButtonSize);
    disableBtnRect.setY(option.rect.y() + ( (option.rect.height() / 2) - (::k_ButtonSize / 2) ) );
    disableBtnRect.setWidth(::k_ButtonSize);
    disableBtnRect.setHeight(::k_ButtonSize);

    QPixmap disableBtnPixmap;
    if (model->widgetState(index) == PipelineItem::WidgetState::Disabled)
    {
      disableBtnPixmap = QPixmap(":/ban_red.png");
      if (painter->device()->devicePixelRatio() == 2)
      {
         disableBtnPixmap = QPixmap(":/ban_red@2x.png");
      }
    }
    else if(disableBtnRect.contains(mousePos))
    {
      disableBtnPixmap = QPixmap(":/ban_hover.png");
      if (painter->device()->devicePixelRatio() == 2)
      {
         disableBtnPixmap = QPixmap(":/ban_hover@2x.png");
      }
    }
    else
    {
      disableBtnPixmap = QPixmap(":/ban.png");
      if (painter->device()->devicePixelRatio() == 2)
      {
         disableBtnPixmap = QPixmap(":/ban@2x.png");
      }
    }

    painter->drawPixmap(disableBtnRect.center().x() - (disableBtnRect.width() / 2), disableBtnRect.center().y() - (disableBtnRect.height() / 2 + 1), disableBtnPixmap);  // y is 1px offset due to how the images were cut
  }

  // If the filter is selected, draw a border around it.
  if(option.state & QStyle::State_Selected)
  {
    QColor selectedColor = QColor::fromHsv(bgColor.hue(), 180, 150);
    QPen pen(QBrush(selectedColor), m_BorderThickness);
    painter->setPen(pen);

    // Draw inside option.rect to avoid painting artifacts
    qreal x = option.rect.x() + (m_BorderThickness / 2);
    qreal y = option.rect.y() + (m_BorderThickness / 2);
    painter->drawRoundedRect(QRectF(x, y, option.rect.width() - m_BorderThickness  + 0.5 , option.rect.height() - m_BorderThickness + 0.5), 1, 1);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
  PipelineModel* pipelineModel = dynamic_cast<PipelineModel*>(model);

  QRect deleteBtnRect;
  deleteBtnRect.setX(option.rect.width() - ::k_ButtonSize - ::k_TextMargin);
  deleteBtnRect.setY(option.rect.y() + (option.rect.height()/2 - ::k_ButtonSize/2));
  deleteBtnRect.setWidth(::k_ButtonSize);
  deleteBtnRect.setHeight(::k_ButtonSize);

  QRect disableBtnRect = deleteBtnRect;
  disableBtnRect.setX(disableBtnRect.x() - ::k_TextMargin - ::k_ButtonSize);

  // Looking for click in the delete button area
  QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
  if (mouseEvent != nullptr)
  {
    if(event->type() == QEvent::MouseMove)
    {
      if (deleteBtnRect.contains(mouseEvent->pos()))
      {
        // We are inside the delete button
        if (m_CurrentlyHoveredItem != HoverItem::DeleteButton)
        {
          // We were not inside the delete button before, so set the current hovered item to DeleteButton and schedule a repaint
          m_CurrentlyHoveredItem = HoverItem::DeleteButton;
          return true;
        }
      }
      else if (disableBtnRect.contains(mouseEvent->pos()))
      {
        // We are inside the disable button
        if (m_CurrentlyHoveredItem != HoverItem::DisableButton)
        {
          // We were not inside the disable button before, so set the current hovered item to DisableButton and schedule a repaint
          m_CurrentlyHoveredItem = HoverItem::DisableButton;
          return true;
        }
      }
      else if (m_CurrentlyHoveredItem != HoverItem::Widget)
      {
        // Otherwise, we have to be inside the main widget.
        // We were not inside the main widget before, so set the current hovered item to Widget and schedule a repaint
        m_CurrentlyHoveredItem = HoverItem::Widget;
        return true;
      }
    }

    if(event->type() == QEvent::MouseButtonPress)
    {
      if(deleteBtnRect.contains(mouseEvent->pos()) || disableBtnRect.contains(mouseEvent->pos()))
      {
        m_MousePressIndex = index.row();
        return true;  // don't call the base class, we handled the event here
      }
    }

    if(event->type() == QEvent::MouseButtonRelease)
    {
      if(deleteBtnRect.contains(mouseEvent->pos()) || disableBtnRect.contains(mouseEvent->pos()))
      {
        m_MousePressIndex = -1;
        //qDebug() << "Clicked the Pipeline Filter delete button on: " << index.data(Qt::DisplayRole).toString();

        if (deleteBtnRect.contains(mouseEvent->pos()))
        {
          AbstractFilter::Pointer filter = pipelineModel->filter(index);
          m_View->removeFilter(filter);
          return true;
        }
        else if (disableBtnRect.contains(mouseEvent->pos()))
        {
          AbstractFilter::Pointer filter = pipelineModel->filter(index);
          bool enabled = filter->getEnabled();
          if (enabled)
          {
            filter->setEnabled(false);
            pipelineModel->setWidgetState(index, PipelineItem::WidgetState::Disabled);
          }
          else
          {
            filter->setEnabled(true);
            pipelineModel->setWidgetState(index, PipelineItem::WidgetState::Ready);
          }

          m_View->preflightPipeline();
          return true;
        }
      }
    }
  }

  return QStyledItemDelegate::editorEvent(event, model, option, index);
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
QPixmap PipelineItemDelegate::getPixmap(const QModelIndex &index) const
{
  QRect indexRect = m_View->visualRect(index);

  QPixmap pixmap(indexRect.width(), indexRect.height());
  QPainter painter;

  QStyleOptionViewItem option;
  indexRect.setY(0);
  indexRect.setX(0);
  option.rect = indexRect;
  option.state = QStyle::State_None;

  painter.begin(&pixmap);
  paint(&painter, option, index);
  painter.end();

  return pixmap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const PipelineModel* PipelineItemDelegate::getPipelineModel(const QModelIndex &index) const
{
  return dynamic_cast<const PipelineModel*>(index.model());
}
