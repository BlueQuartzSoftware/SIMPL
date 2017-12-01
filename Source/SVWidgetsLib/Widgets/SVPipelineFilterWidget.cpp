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

#include "SVPipelineFilterWidget.h"

#include <QtCore/QDir>
#include <QtCore/QMimeData>
#include <QtCore/QResource>
#include <QtCore/QTimer>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGroupBox>

#include "SVWidgetsLib/QtSupport/QtSStyles.h"

#include "SIMPLib/Common/DocRequestManager.h"
#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/FilterParameters/DataContainerReaderFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/InputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"

#include "SVWidgetsLib/FilterParameterWidgets/ChoiceWidget.h"
#include "SVWidgetsLib/FilterParameterWidgets/LinkedBooleanWidget.h"
#include "SVWidgetsLib/Widgets/DataContainerArrayWidget.h"
#include "SVWidgetsLib/Widgets/SVPipelineViewWidget.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineFilterWidget::SVPipelineFilterWidget(QWidget* parent)
: QFrame(parent)
, PipelineFilterObject(AbstractFilter::NullPointer())
, m_Observer(nullptr)
, m_HasRightClickTarget(false)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineFilterWidget::SVPipelineFilterWidget(AbstractFilter::Pointer filter, IObserver* observer, QWidget* parent)
: QFrame(parent)
, PipelineFilterObject(filter)
, m_Observer(observer)
, m_HasRightClickTarget(false)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineFilterWidget::~SVPipelineFilterWidget()
{
  if(m_Observer != nullptr)
  {
    delete m_Observer;
    m_Observer = nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::initialize()
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(getFilterInputWidget(), SIGNAL(filterParametersChanged()), this, SLOT(filterInputWidget_filterParametersChanged()));
  connect(getFilterInputWidget(), SIGNAL(errorSettingFilterParameter(const QString&)), this, SLOT(displayFilterParameterWidgetError(const QString&)));

  connect(&m_AnimationTimer, SIGNAL(timeout()), this, SLOT(updateBorderThickness()));

  /******** CALL THIS EARLY, this initializes all the widgets  ***************/
  setupUi(this);

  deleteBtn->setVisible(false);
  disableBtn->setVisible(false);
  // connect(disableBtn, SIGNAL(clicked(bool)), this, SLOT(setIsEnabled(bool)));

  // Set the Name of the filter into the FilterWidget
  AbstractFilter::Pointer filter = getFilter();
  if(nullptr != filter.get())
  {
    m_FilterHumanLabel = getFilter()->getHumanLabel();

    QString filterGroup;
    QTextStream groupStream(&filterGroup);
    groupStream << "Group: " << filter->getGroupName() << "\n";
    groupStream << "Subgroup: " << filter->getSubGroupName();
    setToolTip(filterGroup);

    if(false == filter->getEnabled())
    {
      toDisabledState();
    }

    connect(filter.get(), SIGNAL(filterCompleted()), this, SLOT(toCompletedState()));
    connect(filter.get(), SIGNAL(filterInProgress()), this, SLOT(toExecutingState()));
  }
  
  deleteBtn->installEventFilter(this);
  disableBtn->installEventFilter(this);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::setSelected(bool s)
{
  PipelineFilterObject::setSelected(s);
  m_BorderThickness = 0;
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::on_disableBtn_clicked(bool checked)
{
  Q_UNUSED(checked)
  if(disableBtn->isChecked())
  {
    disableBtn->setIcon(QIcon(":/ban_red.png"));
  }
  else
  {
    disableBtn->setIcon(QIcon(":/ban.png"));
  }
  
  setIsEnabled(!disableBtn->isChecked());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::setIsEnabled(bool enabled)
{
  setFilterEnabled(enabled);
  if(getPipelineState() == PipelineState::Stopped)
  {
    QUuid uuid;
    PipelineFilterObject::setIsEnabled(enabled);
    emit parametersChanged(uuid);
    update();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVPipelineFilterWidget::hasRightClickTarget()
{
  return m_HasRightClickTarget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::setHasRightClickTarget(bool value)
{
  m_HasRightClickTarget = value;
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::adjustLayout(QWidget* w, int state)
{
  if(state == Qt::Checked)
  {
    w->show();
  }
  else
  {
    w->hide();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::displayFilterParameterWidgetError(const QString& msg)
{
  if(m_Observer)
  {
    PipelineMessage pm("Filter Paramter Widget", msg, -1, PipelineMessage::MessageType::Error);
    m_Observer->processPipelineMessage(pm);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::setFilterTitle(const QString title)
{
  m_FilterHumanLabel = title;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::setFilterIndex(int i, int numFilters)
{

  if(numFilters < 10)
  {
    numFilters = 11;
    m_MaxFilterCount = 99;
  }
  QString numStr = QString::number(i);

  if(numFilters > 9)
  {
    int mag = 0;
    int max = numFilters;
    m_MaxFilterCount = 1;
    while(max > 0)
    {
      mag++;
      max = max / 10;
      m_MaxFilterCount *= 10;
    }
    numStr = "";             // Clear the string
    QTextStream ss(&numStr); // Create a QTextStream to set up the padding
    ss.setFieldWidth(mag);
    ss.setPadChar('0');
    ss << i;
    m_MaxFilterCount -= 1;
  }
  m_PaddedIndex = numStr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::paintEvent(QPaintEvent*)
{
  QStyleOption opt;
  opt.init(this);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  WidgetState wState = getWidgetState();
  PipelineState pState = getPipelineState();
  ErrorState eState = getErrorState();

  QColor widgetBackgroundColor;
  QColor labelColor;
  QColor indexBackgroundColor;
  QColor bgColor = getGroupColor();
  QColor disabledBgColor = QColor(124, 124, 124);

  QColor hoveredColor = getGroupColor();
  hoveredColor.setRedF((hoveredColor.redF() * 1.10 > 1.0) ? 1.0 : hoveredColor.redF() * 1.10);
  hoveredColor.setGreenF((hoveredColor.greenF() * 1.10 > 1.0) ? 1.0 : hoveredColor.greenF() * 1.10);
  hoveredColor.setBlueF((hoveredColor.blueF() * 1.10 > 1.0) ? 1.0 : hoveredColor.blueF() * 1.10);
  if(m_HoverState)
  {
    bgColor = hoveredColor;
  }
  switch(wState)
  {
  case WidgetState::Ready:
    widgetBackgroundColor = bgColor;
    labelColor = QColor(190, 190, 190);
    indexBackgroundColor = QColor(48, 48, 48);
    break;
  case WidgetState::Executing:
    widgetBackgroundColor = QColor(130, 130, 130);
    labelColor = QColor(190, 190, 190);
    indexBackgroundColor = QColor(6, 140, 190);
    break;
  case WidgetState::Completed:
    widgetBackgroundColor = bgColor.name();
    labelColor = QColor(190, 190, 190);
    indexBackgroundColor = QColor(6, 118, 6);
    break;
  case WidgetState::Disabled:
    bgColor = disabledBgColor;
    widgetBackgroundColor = disabledBgColor.name();
    labelColor = QColor(190, 190, 190);
    indexBackgroundColor = QColor(96, 96, 96);
    break;
  }
  QColor selectedColor = QColor::fromHsv(bgColor.hue(), 100, 120);

  // Do not change the background color if the widget is disabled.
  if(wState != WidgetState::Disabled)
  {
    switch(pState)
    {
    case PipelineState::Running:
      widgetBackgroundColor = selectedColor.name();
      labelColor = QColor(190, 190, 190);
      break;
    case PipelineState::Stopped:
      widgetBackgroundColor = bgColor.name();
      labelColor = QColor(0, 0, 0);
      break;
    case PipelineState::Paused:
      widgetBackgroundColor = QColor(160, 160, 160);
      labelColor = QColor(0, 0, 0);
      break;
    }
  }

  switch(eState)
  {
  case ErrorState::Ok:

    break;
  case ErrorState::Error:
    indexBackgroundColor = QColor(179, 2, 5);
    break;
  case ErrorState::Warning:
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

  int maxHeight = maximumHeight();
  QFontMetrics fontMetrics(font);
  int fontHeight = fontMetrics.height();
  int fontMargin = ((maxHeight - fontHeight) / 2) - 1;

  int indexFontWidth = fontMetrics.width(QString::number(m_MaxFilterCount));

  painter.setFont(font);

  // back fill with RED so we know if we missed something
  // painter.fillRect(rect(), QColor(255, 0, 0));

  // Draw the Index area
  QRect indexRect = rect();
  indexRect.setWidth(2 * m_TextMargin + indexFontWidth);
  painter.fillRect(indexRect, indexBackgroundColor);

  // Draw the Title area
  QRect coloredRect(2 * m_TextMargin + indexFontWidth, rect().y(), rect().width() - (2 * m_TextMargin + indexFontWidth), rect().height()); // +4? without it it does not paint to the edge
  painter.fillRect(coloredRect, widgetBackgroundColor);

  // Draw the Index number
  painter.setPen(QPen(indexFontColor));
  QString number = m_PaddedIndex; // format the index number with a leading zero
  painter.drawText(rect().x() + m_TextMargin, rect().y() + fontMargin + fontHeight, number);

  // Compute the Width to draw the text based on the visibility of the various buttons
  int fullWidth = width() - m_IndexBoxWidth;
  int allowableWidth = fullWidth;
  if(deleteBtn->isVisible())
  {
    allowableWidth -= deleteBtn->width();
  }
  if(disableBtn->isVisible())
  {
    allowableWidth -= disableBtn->width();
  }
  // QString elidedHumanLabel = fontMetrics.elidedText(m_FilterHumanLabel, Qt::ElideRight, allowableWidth);
  int humanLabelWidth = fontMetrics.width(m_FilterHumanLabel);

  // Draw the filter human label
  painter.setPen(QPen(labelColor));
  font.setWeight(QFont::Normal);
  painter.setFont(font);

  // Compute a Fade out of the text if it is too long to fit in the widget
  if(humanLabelWidth > allowableWidth)
  {
    QRect fadedRect = coloredRect;
    fadedRect.setWidth(fullWidth);
    if(m_HoverState)
    {
      fadedRect.setWidth(allowableWidth);
    }

    QLinearGradient gradient(fadedRect.topLeft(), fadedRect.topRight());
    gradient.setColorAt(0.8, labelColor);
    gradient.setColorAt(1.0, QColor(0, 0, 0, 10));

    QPen pen;
    pen.setBrush(QBrush(gradient));
    painter.setPen(pen);
  }

  painter.drawText(rect().x() + m_IndexBoxWidth + m_TextMargin, rect().y() + fontMargin + fontHeight, m_FilterHumanLabel);

  // If the filter widget is selected, draw a border around it.
  if(isSelected() == true)
  {
    QColor selectedColor = QColor::fromHsv(bgColor.hue(), 180, 150);
    QPainterPath path;
    path.addRect(rect());
    QPen pen(selectedColor, m_BorderThickness);
    painter.setPen(pen);
    painter.drawPath(path);
    if(m_BorderThickness < 1.0)
    {
      m_AnimationTimer.start(33);
    }
  }
  else
  {
    m_AnimationTimer.stop();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::updateBorderThickness()
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
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::changeStyle()
{
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::on_deleteBtn_clicked()
{
  if(getPipelineState() == PipelineState::Stopped)
  {
    emit filterWidgetRemoved(this);
    emit filterWidgetPressed(nullptr, qApp->queryKeyboardModifiers());
    update();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVPipelineFilterWidget::eventFilter(QObject* watched, QEvent* event)
{
  QPushButton* button = qobject_cast<QPushButton*>(watched);
  if (deleteBtn == button) 
  {

    if (event->type() == QEvent::Enter) {
      // The push button is hovered by mouse
      deleteBtn->setIcon(QIcon(":/trash_hover.png"));
      return true;
    }
    
    if (event->type() == QEvent::Leave) {
      // The push button is not hovered by mouse
      deleteBtn->setIcon(QIcon(":/trash.png"));
      return true;
    }
  }
  
  if (disableBtn == button) 
  {
    if(disableBtn->isChecked())
    {
      if (event->type() == QEvent::Enter) {
        // The push button is hovered by mouse
        disableBtn->setIcon(QIcon(":/ban_red.png"));
        return true;
      }
      if (event->type() == QEvent::Leave) {
        // The push button is not hovered by mouse
        disableBtn->setIcon(QIcon(":/ban_red.png"));
        return true;
      }
    }
    else
    {
      if (event->type() == QEvent::Enter) {
        // The push button is hovered by mouse
        disableBtn->setIcon(QIcon(":/ban_hover.png"));
        return true;
      }
      if (event->type() == QEvent::Leave) {
        // The push button is not hovered by mouse
        disableBtn->setIcon(QIcon(":/ban.png"));
        return true;
      }
    }
  }
  
  
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::RightButton)
  {
    QFrame::mousePressEvent(event);
  }
  else if(event->button() == Qt::LeftButton)
  {
    m_DragStartPosition = event->pos();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::mouseReleaseEvent(QMouseEvent* /* event */)
{
  emit filterWidgetPressed(this, qApp->queryKeyboardModifiers());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::mouseMoveEvent(QMouseEvent* event)
{
  if(!(event->buttons() & Qt::LeftButton))
  {
    return;
  }
  if((event->pos() - m_DragStartPosition).manhattanLength() < QApplication::startDragDistance())
  {
    return;
  }

  emit dragStarted(event, this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::enterEvent(QEvent* /* event */)
{
  if(getPipelineState() == PipelineState::Stopped)
  {
    m_HoverState = true;
    disableBtn->setVisible(true);
    deleteBtn->setVisible(true);
    update();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::leaveEvent(QEvent* /* event */)
{
  m_HoverState = false;
  disableBtn->setVisible(false);
  deleteBtn->setVisible(false);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::focusInEvent(QFocusEvent* event)
{
  emit focusInEventStarted(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::focusOutEvent(QFocusEvent* event)
{
  emit focusOutEventStarted(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::getGuiParametersFromFilter(AbstractFilter* /* event */)
{
  Q_ASSERT("SVPipelineFilterWidget::getGuiParametersFromFilter executed"); // Code should never enter this function
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::filterInputWidget_filterParametersChanged()
{
  emit parametersChanged(QUuid());
  emit parametersChanged1(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::launchHelpForItem()
{
  QString className = getFilterClassName();

  DocRequestManager* docRequester = DocRequestManager::Instance();
  docRequester->requestFilterDocs(className);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toReadyState()
{
  PipelineFilterObject::toReadyState();
  getFilterInputWidget()->toIdleState();
  disableBtn->setChecked(false);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toExecutingState()
{
  PipelineFilterObject::toExecutingState();
  getFilterInputWidget()->toRunningState();
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toCompletedState()
{
  PipelineFilterObject::toCompletedState();
  getFilterInputWidget()->toRunningState();
  if(getFilter()->getWarningCondition() < 0)
  {
    setErrorState(ErrorState::Warning);
  }
  if(getFilter()->getErrorCondition() < 0)
  {
    setErrorState(ErrorState::Error);
  }
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toDisabledState()
{
  PipelineFilterObject::toDisabledState();
  getFilterInputWidget()->toIdleState();
  disableBtn->setChecked(true);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toRunningState()
{
  PipelineFilterObject::toRunningState();
  getFilterInputWidget()->toRunningState();
  deleteBtn->setDisabled(true);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toStoppedState()
{
  PipelineFilterObject::toStoppedState();
  getFilterInputWidget()->toIdleState();
  deleteBtn->setEnabled(true);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toPausedState()
{
  PipelineFilterObject::toPausedState();
  getFilterInputWidget()->toIdleState();
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toOkState()
{
  PipelineFilterObject::toOkState();
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toErrorState()
{
  PipelineFilterObject::toErrorState();
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toWarningState()
{
  PipelineFilterObject::toWarningState();
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject* SVPipelineFilterWidget::deepCopy()
{
  SVPipelineFilterWidget* newWidget = new SVPipelineFilterWidget(getFilter()->newFilterInstance(true), nullptr, dynamic_cast<QWidget*>(parent()));
  newWidget->setHasPreflightErrors(getHasPreflightErrors());
  newWidget->setHasPreflightWarnings(getHasPreflightWarnings());

  return newWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHBoxLayout* SVPipelineFilterWidget::getHorizontalLayout()
{
  return horizontalLayout;
}
