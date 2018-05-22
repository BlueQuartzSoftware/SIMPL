#include "StatusBarButton.h"


#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QStyleOption>

#include "SVWidgetsLib/Widgets/SVStyle.h"
#include "SVWidgetsLib/QtSupport/QtSSettings.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatusBarButton::StatusBarButton(QWidget* parent)
: QToolButton (parent)
{
  //  QFont font = SVStyle::Instance()->GetHumanLabelFont();

  //  QFontMetrics fontMetrics(font);
  //  int fontHeight = fontMetrics.height();
  //  int fontWidth = fontMetrics.width(text());
  //  setMinimumWidth(m_BadgeDiameter * 2 + fontWidth + m_Spacing * 3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatusBarButton::~StatusBarButton() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarButton::updateMinimumWidth()
{
  setMinimumWidth(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarButton::mousePressEvent(QMouseEvent* event)
{
  QToolButton::mousePressEvent(event);
  if(event->button() == Qt::LeftButton)
  {
    m_Pressed = true;
  }
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarButton::mouseReleaseEvent(QMouseEvent* event)
{
  QToolButton::mouseReleaseEvent(event);

  m_Pressed = false;

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarButton::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event)

  QStyleOption opt;
  opt.init(this);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  QFont font = SVStyle::Instance()->GetHumanLabelFont();

#if defined(Q_OS_MAC)
  font.setPointSize(font.pointSize() - 4);
#elif defined(Q_OS_WIN)
  font.setPointSize(font.pointSize() - 3);
#else
  font.setPointSize(font.pointSize() - 1);
#endif

  int maxHeight = maximumHeight();
  QFontMetrics fontMetrics(font);
  int fontHeight = fontMetrics.height();
  int fontMargin = ((maxHeight - fontHeight) / 2) - 2;

  painter.setFont(font);

  QColor backgroundColor(200, 200, 200);
  QColor borderColor(120, 120, 120);
  QColor fontColor(50, 50, 50);
  qreal borderRadius = 5.0;

  if(isChecked())
  {
    backgroundColor = QColor(113, 113, 113);
    borderColor = QColor(200, 200, 200);
    fontColor = QColor(240, 240, 240);
  }
  
  if(m_Pressed && isChecked())
  {
    backgroundColor = QColor(200, 200, 200);
    fontColor = QColor(50, 50, 50);
    borderColor = QColor(120, 120, 120);

  }
  if(m_Pressed && !isChecked())
  {
    backgroundColor = QColor(120, 120, 120);
    fontColor = QColor(240, 240, 240);
    borderColor = QColor(200, 200, 200);
  }
  QRect rect = this->rect();

  QPainterPath buttonPath;
  buttonPath.addRoundedRect(rect, borderRadius, borderRadius);
  QPen pen(borderColor, 2);
  painter.setPen(pen);
  painter.fillPath(buttonPath, backgroundColor);
  painter.drawPath(buttonPath);

  painter.setPen(QPen(fontColor));
  font.setWeight(QFont::Bold);
  painter.setFont(font);

  int textYCoord = rect.y() + fontMargin + fontHeight;
  painter.drawText(m_TextMargin, textYCoord, text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatusBarButton::getTextMargin()
{
  return m_TextMargin;
}
