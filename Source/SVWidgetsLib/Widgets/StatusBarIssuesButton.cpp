#include "StatusBarIssuesButton.h"


#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QStyleOption>

#include "SVWidgetsLib/Widgets/SVStyle.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatusBarIssuesButton::StatusBarIssuesButton(QWidget* parent)
: StatusBarButton(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatusBarIssuesButton::~StatusBarIssuesButton() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarIssuesButton::updateMinimumWidth()
{
  if(m_ErrorBadgeCount + m_WarningBadgeCount > 0)
  {
    QFont font = SVStyle::Instance()->GetHumanLabelFont();

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
    StatusBarButton::updateMinimumWidth();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarIssuesButton::setErrorBadgeCount(int count)
{
  m_ErrorBadgeCount = count;
  updateMinimumWidth();
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarIssuesButton::setWarningBadgeCount(int count)
{
  m_WarningBadgeCount = count;
  updateMinimumWidth();
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarIssuesButton::paintEvent(QPaintEvent* event)
{
  StatusBarButton::paintEvent(event);

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

  QFontMetrics fontMetrics(font);
  int fontHeight = fontMetrics.height();

  painter.setFont(font);

  QColor badgeBgColor(120, 120, 120);
  QColor badgeFontColor(240, 240, 240);

  if(isChecked())
  {
    badgeBgColor = QColor(200, 200, 200);
    badgeFontColor = QColor(96, 96, 96);
  }
  
  QRect rect = this->rect();

  int btnCenterY = rect.height() / 2;

  if(m_ErrorBadgeCount + m_WarningBadgeCount > 0)
  {

    QString number = QString::number(m_ErrorBadgeCount + m_WarningBadgeCount);
    int fw = fontMetrics.width(number);

    QPainterPath badgePath;
    badgePath.addRoundedRect(rect.width() - m_BadgeWidth - getTextMargin(), // X
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

    painter.drawText(rect.width() - getTextMargin() - (m_BadgeWidth / 2) - (fw / 2), btnCenterY + (fontHeight / 3), number);
  }

  QString toolTip = QString("The current pipeline has %1 errors and %2 warnings.").arg(m_ErrorBadgeCount).arg(m_WarningBadgeCount);
  setToolTip(toolTip);
}
