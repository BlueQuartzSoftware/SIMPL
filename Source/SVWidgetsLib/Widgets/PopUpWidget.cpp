/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Softwae, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
#include "PopUpWidget.h"

#include <QtGui/QPainter>

#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QGridLayout>

#include "moc_PopUpWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PopUpWidget::PopUpWidget(QWidget *parent)
  : QWidget (parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PopUpWidget::~PopUpWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PopUpWidget::setupGui()
{
  setWindowFlags(Qt::Popup);
  setAttribute(Qt::WA_NoSystemBackground);
  setAttribute(Qt::WA_TranslucentBackground);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PopUpWidget::paintEvent(QPaintEvent* event)
{
  QWidget::paintEvent(event);

  QPainterPath painterPath;
  QPolygonF polygon;

  if (m_ArrowOrientation == PopUpWidget::ArrowOrientation::Left)
  {
    setContentsMargins(m_ArrowHeight, 0, 0, 0);
    polygon << QPointF(m_ArrowHeight, m_ArrowOffset) << QPointF(0, m_ArrowOffset + m_ArrowWidth / 2)
            << QPointF(m_ArrowHeight, m_ArrowOffset + m_ArrowWidth) << QPointF(m_ArrowHeight, m_ArrowOffset);
    painterPath.addPolygon(polygon);
  }
  else if (m_ArrowOrientation == PopUpWidget::ArrowOrientation::Right)
  {
    setContentsMargins(0, 0, m_ArrowHeight, 0);
    polygon << QPointF(size().width() - m_ArrowHeight, m_ArrowOffset) << QPointF(size().width(), m_ArrowOffset + m_ArrowWidth / 2)
            << QPointF(size().width() - m_ArrowHeight, m_ArrowOffset + m_ArrowWidth) << QPointF(size().width() - m_ArrowHeight, m_ArrowOffset);
    painterPath.addPolygon(polygon);
  }
//  else if (m_ArrowOrientation == PopUpWidget::ArrowOrientation::Top)
//  {
//    setContentsMargins(0, m_ArrowHeight, 0, 0);
//    polygon << QPointF(m_ArrowOffset, m_ArrowHeight) << QPointF(m_ArrowOffset + m_ArrowWidth / 2, 0)
//            << QPointF(m_ArrowOffset + m_ArrowWidth, m_ArrowHeight) << QPointF(m_ArrowOffset, m_ArrowHeight);
//    painterPath.addPolygon(polygon);
//  }
//  else if (m_ArrowOrientation == PopUpWidget::ArrowOrientation::Bottom)
//  {
//    setContentsMargins(0, 0, 0, m_ArrowHeight);
//    polygon << QPointF(m_ArrowOffset, size().height()) << QPointF(m_ArrowOffset + m_ArrowWidth / 2, size().height() + m_ArrowHeight)
//            << QPointF(m_ArrowOffset + m_ArrowWidth, size().height()) << QPointF(m_ArrowOffset, size().height());
//    painterPath.addPolygon(polygon);
//  }
  else if(m_ArrowOrientation == PopUpWidget::ArrowOrientation::None)
  {
    setContentsMargins(0, 0, 0, 0);
  }
  else
  {
    return;
  }

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  QPen pen = painter.pen();
  pen.setWidth(5);
  pen.setColor(QColor(121, 121, 121));
  painter.setPen(pen);
  painter.drawPath(painterPath);
  painter.fillPath(painterPath, QColor(180, 180, 180));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PopUpWidget::setWidget(QWidget* widget)
{
  if(widget == nullptr)
  {
    return;
  }

//  // Clear the existing widget
//  QLayoutItem* item = gridLayout->takeAt(0);
//  if(item)
//  {
//    QWidget* w = item->widget();
//    if(w)
//    {
//      w->hide();
//      w->setParent(nullptr);
//    }
//  }

  // Set the widget into the frame
  scrollArea->setWidget(widget);
  widget->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PopUpWidget::setArrowOrientation(ArrowOrientation orientation)
{
  m_ArrowOrientation = orientation;
}
