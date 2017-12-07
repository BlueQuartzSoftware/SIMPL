#include "StatusBarButton.h"


#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QStyleOption>

#include "SVWidgetsLib/QtSupport/QtSStyles.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatusBarButton::StatusBarButton(QWidget* parent)
: QToolButton (parent)
{
  //  QFont font = QtSStyles::GetHumanLabelFont();

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

  if(m_ErrorBadgeCount + m_WarningBadgeCount > 0)
  {
    QFont font = QtSStyles::GetHumanLabelFont();

#if defined(Q_OS_MAC)
    font.setPointSize(font.pointSize() - 4);
#elif defined(Q_OS_WIN)
    font.setPointSize(font.pointSize() - 3);
#else
    font.setPointSize(font.pointSize() - 1);
#endif

    QFontMetrics fontMetrics(font);
    QString number = QString::number(m_ErrorBadgeCount + m_WarningBadgeCount);
    int fw = fontMetrics.width(number);

    m_BadgeWidth = fw + (2 * m_BadgeMargin);
    setMinimumWidth(110 + m_BadgeWidth);
  }
  else
  {
    setMinimumWidth(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarButton::setErrorBadgeCount(int count)
{
  m_ErrorBadgeCount = count;
  updateMinimumWidth();
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarButton::setWarningBadgeCount(int count)
{
  m_WarningBadgeCount = count;
  updateMinimumWidth();
  update();
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
  QFont font = QtSStyles::GetHumanLabelFont();

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
  int fontMargin = ((maxHeight - fontHeight) / 2) - 1;

  painter.setFont(font);

  QColor backgroundColor(200, 200, 200);
  QColor borderColor(120, 120, 120);
  QColor fontColor(50, 50, 50);
  QColor badgeBgColor(120, 120, 120);
  QColor badgeFontColor(240, 240, 240);
  qreal borderRadius = 5.0;

  if(isChecked())
  {
    backgroundColor = QColor(113, 113, 113);
    borderColor = QColor(200, 200, 200);
    fontColor = QColor(240, 240, 240);
    badgeBgColor = QColor(200, 200, 200);
    badgeFontColor = QColor(96, 96, 96);
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

  int btnCenterY = rect.height() / 2;

  if(m_ErrorBadgeCount + m_WarningBadgeCount > 0)
  {

    QString number = QString::number(m_ErrorBadgeCount + m_WarningBadgeCount);
    int fw = fontMetrics.width(number);

    QPainterPath badgePath;
    badgePath.addRoundedRect(rect.width() - m_BadgeWidth - m_TextMargin, // X
                             4,                                          // Y
                             m_BadgeWidth,                               // Width
                             minimumHeight() - 8,                        // Height
                             7.0, 7.0);                                  // X & Y Radius

    QPen pen(badgeBgColor, 2);
    painter.setPen(pen);
    painter.fillPath(badgePath, badgeBgColor);
    painter.drawPath(badgePath);

    painter.setPen(QPen(badgeFontColor));
    font.setWeight(QFont::Bold);
    painter.setFont(font);

    painter.drawText(rect.width() - m_TextMargin - (m_BadgeWidth / 2) - (fw / 2), btnCenterY + (fontHeight / 3), number);
  }

  QString toolTip = QString("The current pipeline has %1 errors and %2 warnings.").arg(m_ErrorBadgeCount).arg(m_WarningBadgeCount);
  setToolTip(toolTip);
}
