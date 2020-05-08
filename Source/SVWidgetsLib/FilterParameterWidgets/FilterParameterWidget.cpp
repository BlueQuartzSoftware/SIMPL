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
#include "FilterParameterWidget.h"

#include <QtCore/QPropertyAnimation>
#include <QtCore/QTimer>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QGraphicsOpacityEffect>
#include <QtWidgets/QMenu>

#include <QtGui/QScreen>

#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

namespace detail
{
static const float Max = 1.0;
static const int Duration = 400;
} // namespace detail

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameterWidget::FilterParameterWidget(QWidget* parent)
: QFrame(parent)
, animation(nullptr)
, effect(nullptr)
{
  fadeIn = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameterWidget::FilterParameterWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: QFrame(parent)
, m_Filter(filter)
, m_FilterParameter(parameter)
, animation(nullptr)
, effect(nullptr)
{
  fadeIn = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameterWidget::~FilterParameterWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::setLinkedConditionalState(int state)
{
  bool boolProp = (state == Qt::Checked);
  fadeWidget(this, boolProp);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::setLinkedComboBoxState(int groupId)
{
  if(m_FilterParameter != nullptr)
  {
    int groupIndex = m_FilterParameter->getGroupIndex();
    int state = (groupIndex == groupId) ? Qt::Checked : Qt::Unchecked;
    {
      setLinkedConditionalState(state);
    }
  }
}

#if 1
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPointer<QtSFaderWidget> FilterParameterWidget::getFaderWidget() const
{
  return m_FaderWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::setFaderWidget(QPointer<QtSFaderWidget> faderWidget)
{
  m_FaderWidget = faderWidget;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::fadeInWidget(QWidget* widget)
{
#if 0
  if (m_FaderWidget)
  {
    m_FaderWidget->close();
  }
  m_FaderWidget = new QtSFaderWidget(widget);
  m_FaderWidget->start();
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::setupGui()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterParameterWidget::wrapStringInHtml(const QString& message)
{
  QString html;
  QTextStream ss(&html);
  ss << "<html><head></head>\n";
  ss << "<body>\n";
  ss << "<b>" << message << "</b>\n";
  ss << "<br/>";
  ss << "</body></html>";
  return html;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::changeStyleSheet(Style style)
{
  QString styleSheet;
  QTextStream ss(&styleSheet);

  ss << "QFrame {";
#if 0
#if defined(Q_OS_WIN)
  ss << "font: italic 9 pt \"" << SVStyle::GetUIFont() << "\";";
#elif defined(Q_OS_MAC)
  ss << "font: italic 12 pt \"" << SVStyle::GetUIFont() << "\";";
#else
  ss << "font: italic 10 pt \"" << SVStyle::GetUIFont() << "\";";
#endif
#endif

  if(style == Style::FS_STANDARD_STYLE)
  {
  }
  else if(style == Style::FS_DRAGGING_STYLE)
  {
    ss << "border: 2px solid rgb(34, 120, 46);";
    ss << "border-radius: 10px;";
  }
  else if(style == Style::FS_DOESNOTEXIST_STYLE)
  {
    ss << "color: rgb(200, 50, 50); font: bold;";
    ss << "font: bold;";

    // ss << "background-color: rgb(230, 150, 150);";
  }
  else if(style == Style::FS_WARNING_STYLE)
  {
    ss << "color: rgb(255, 140, 0); font: bold;";
  }

  ss << "}";

  ss << "QLabel#label { border: 0px solid rgb(255, 120, 46); }";

  ss << " QToolTip {\
              border: 2px solid #434343;\
              padding: 2px;\
              border-radius: 3px;\
              opacity: 255;\
              background-color: #FFFCEA;\
              color: #000000;\
              }";

  setStyleSheet(styleSheet);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPoint FilterParameterWidget::adjustedMenuPosition(QToolButton* pushButton)
{
  // Calculate the actual virtual desktop QRect.
  QList<QScreen*> screens = QGuiApplication::screens();
  int xMin = std::numeric_limits<int>::max();
  int yMin = std::numeric_limits<int>::max();
  int xMax = std::numeric_limits<int>::min();
  int yMax = std::numeric_limits<int>::min();
  QRect virtDesktopRect;
  for(const auto& screen : screens)
  {
    QRect rect = screen->availableGeometry();
    if(rect.x() < xMin)
    {
      xMin = rect.x();
    }
    if(rect.y() < yMin)
    {
      yMin = rect.y();
    }
    if(rect.x() + rect.width() > xMax)
    {
      xMax = rect.x() + rect.width();
    }
    if(rect.y() + rect.height() > yMax)
    {
      yMax = rect.y() + rect.height();
    }
  }

  virtDesktopRect.setTopLeft(QPoint(xMin, yMin));
  virtDesktopRect.setBottomRight(QPoint(xMax, yMax));

  QSize menuSize = pushButton->menu()->sizeHint();
  QPoint point = QCursor::pos();

  // Move the x position to the left by half the width of the menu so the menu
  // is centered up under the mouse
  point.setX(point.x() - menuSize.width() / 2);

  // If the menu is going to go off the screen in the X-axis, reposition it until it's completely on the screen
  if(point.x() + menuSize.width() > virtDesktopRect.right())
  {
    //  int diffX = point.x() + menuSize.width() - virtDesktopRect.right();
    point.setX(virtDesktopRect.right() - menuSize.width());
  }

  // Make sure the menu will not get positioned off the left side of the desktop
  if(point.x() - 0.5 * menuSize.width() < virtDesktopRect.left())
  {
    point.setX(virtDesktopRect.left() + 2); //
  }

  // Find the "Y" Position that the menu should be displayed at. We want the menu
  // to appear just below the button so the button and it's text are not obscurred.
  QPoint localButtonCoords = pushButton->geometry().bottomLeft();
  QPoint globalButtonCoords = mapToGlobal(localButtonCoords);

  point.setY(globalButtonCoords.y());

  int screenNum = QApplication::desktop()->screenNumber(pushButton);
  int desktopHeight = screens.at(screenNum)->availableGeometry().height();

  if(point.y() > desktopHeight)
  {
    localButtonCoords = pushButton->geometry().topLeft();
    globalButtonCoords = mapToGlobal(localButtonCoords);
    point.setY(globalButtonCoords.y() - menuSize.height());
  }

  return point;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::fadeWidget(QWidget* widget, bool in)
{
#if 0
  if (m_FaderWidget)
  {
    m_FaderWidget->close();
  }
  m_FaderWidget = new QtSFaderWidget(widget);
  if(in)
  {
    setVisible(true);
    m_FaderWidget->setFadeIn();
    connect(m_FaderWidget, SIGNAL(animationComplete() ),
            this, SLOT(show()));
    connect(m_FaderWidget, SIGNAL(animationComplete()),
            this, SLOT(showBorder()));

    widget->setStyleSheet("border: 2px solid MediumBlue;");
  }
  else
  {
    m_FaderWidget->setFadeOut();
    connect(m_FaderWidget, SIGNAL(animationComplete() ),
            this, SLOT(hide()));
  }

  m_FaderWidget->setStartColor(palette().color(QWidget::backgroundRole()));
  m_FaderWidget->start();
#else
  fadeIn = in;

  QString styleSheet;
  QTextStream ss(&styleSheet);
  ss << "QFrame#" << this->objectName() << " { ";
  ss << "background-color: " << SVStyle::Instance()->getFader_color().name() << ";";
  ss << "border: 0px Solid;";
  ss << "border-color: " << SVStyle::Instance()->getFader_color().name() << ";";
  ss << "border-radius: 5px;";
  ss << "}";

  if(!fadeIn)
  {
    startValue = detail::Max;
    endValue = 0;
  }
  else
  {
    widget->show();
    widget->setStyleSheet(styleSheet);
    // qDebug() << styleSheet();

    fadeIn = in;
    startValue = 0;
    endValue = detail::Max;
  }

  QRect rect = geometry();
  QLinearGradient alphaGradient(rect.topLeft(), rect.bottomLeft());
  alphaGradient.setColorAt(0.5, Qt::black);
  alphaGradient.setColorAt(1.0, Qt::black);

  if(effect == nullptr)
  {
    effect = new QGraphicsOpacityEffect(this);
    effect->setOpacityMask(alphaGradient);
    // this->setGraphicsEffect(effect);
  }

  if(animation == nullptr)
  {
    animation = new QPropertyAnimation(effect, "opacity", this);
    animation->setDuration(detail::Duration);
    connect(animation, SIGNAL(finished()), this, SLOT(animationFinished()));
    connect(animation, SIGNAL(finished()), this, SLOT(showBorder()));
  }

  animation->setStartValue(startValue);
  animation->setEndValue(endValue);

  animation->start();

#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::animationFinished()
{
  if(!fadeIn)
  {
    this->hide();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::showBorder()
{
  if(nullptr != m_Timer)
  {
    delete m_Timer;
    m_Timer = nullptr;
  }
  m_Timer = new QTimer(this);
  connect(m_Timer, SIGNAL(timeout()), this, SLOT(hideBorder()));
  m_Timer->start(2000);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::hideBorder()
{
  setStyleSheet("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::setValidFilePath(const QString& filePath)
{
  m_CurrentlyValidPath = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::showFileInFileSystem()
{
  QFileInfo fi(m_CurrentlyValidPath);
  QString path;
  if(fi.isFile())
  {
    path = fi.absoluteFilePath();
  }
  else
  {
    path = fi.absolutePath();
  }

  QtSFileUtils::ShowPathInGui(this, fi.absoluteFilePath());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::loadData()
{
}

// -----------------------------------------------------------------------------
void FilterParameterWidget::setFilter(AbstractFilter* value)
{
  m_Filter = value;
}

// -----------------------------------------------------------------------------
AbstractFilter* FilterParameterWidget::getFilter() const
{
  return m_Filter;
}
// -----------------------------------------------------------------------------
void FilterParameterWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = value;
}

// -----------------------------------------------------------------------------
FilterParameter* FilterParameterWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
void FilterParameterWidget::setWidgetIsExpanding(bool value)
{
  m_WidgetIsExpanding = value;
}

// -----------------------------------------------------------------------------
bool FilterParameterWidget::isWidgetExpanding() const
{
  return m_WidgetIsExpanding;
}
