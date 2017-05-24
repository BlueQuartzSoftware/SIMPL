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
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/IFilterFactory.hpp"
#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/FilterParameters/DataContainerReaderFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/InputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"

#include "SVWidgetsLib/Core/FilterWidgetManager.h"
#include "SVWidgetsLib/FilterParameterWidgets/ChoiceWidget.h"
#include "SVWidgetsLib/FilterParameterWidgets/LinkedBooleanWidget.h"
#include "SVWidgetsLib/Widgets/DataContainerArrayWidget.h"
#include "SVWidgetsLib/Widgets/SVPipelineViewWidget.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_SVPipelineFilterWidget.cpp"

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

  setupUi(this);

  // Set the Name of the filter into the FilterWidget
  AbstractFilter::Pointer filter = getFilter();
  if(nullptr != filter.get())
  {
    filterName->setText(getFilter()->getHumanLabel());

    QString filterGroup;
    QTextStream groupStream(&filterGroup);
    groupStream << "Group: " << filter->getGroupName() << "\n";
    groupStream << "Subgroup: " << filter->getSubGroupName();
    filterName->setToolTip(filterGroup);

    connect(filter.get(), SIGNAL(filterCompleted()), this, SLOT(toCompletedState()));
    connect(filter.get(), SIGNAL(filterInProgress()), this, SLOT(toExecutingState()));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::setSelected(bool s)
{
  PipelineFilterObject::setSelected(s);
  changeStyle();
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
  changeStyle();
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
  filterName->setText(title);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::setFilterIndex(int i, int numFilters)
{
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

  filterIndex->setText(numStr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::changeStyle()
{
  QFont font = QtSStyles::GetHumanLabelFont();
  QString fontString;
  QTextStream fontStringStream(&fontString);

  fontStringStream << "font: " << font.weight() << " ";
#if defined(Q_OS_MAC)
  fontStringStream << font.pointSize() - 4;
#elif defined(Q_OS_WIN)
  fontStringStream << font.pointSize() - 3;
#else
  fontStringStream << font.pointSize() - 1;
#endif

  fontStringStream << "pt \"" << font.family() << "\";";

  // Style the over all widget
  QString svWidgetStyle;
  QTextStream svWidgetStyleStream(&svWidgetStyle);
  svWidgetStyleStream << "QFrame#frame {\n";
  svWidgetStyleStream << "border-radius: 0 3 3 0px;";
  svWidgetStyleStream << "padding: 0 0 0 0px;";

  QString labelStyle;
  QTextStream labelStyleStream(&labelStyle);
  labelStyleStream << "QLabel {";
  labelStyleStream << fontString;
  labelStyleStream << "padding: 2 2 2 2px;";

  //----------------------------------------------------
  QString filterIndexStyle;
  QTextStream filterIndexStyleStream(&filterIndexStyle);
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
  fontStringStream << "font-weight: bold;";
#endif
  filterIndexStyleStream << "QLabel\n{";
  filterIndexStyleStream << fontString;
  filterIndexStyleStream << "color: rgb(242, 242, 242);"; // Always have a white'ish font
  filterIndexStyleStream << "border-top-left-radius: 3px;";
  filterIndexStyleStream << "border-bottom-left-radius: 3px;";

  WidgetState wState = getWidgetState();
  PipelineState pState = getPipelineState();
  ErrorState eState = getErrorState();

  QString widgetBackgroundColor;
  QString labelColor;
  QString indexBackgroundColor;
  QColor bgColor = getGroupColor();

  switch(wState)
  {
  case WidgetState::Ready:
    widgetBackgroundColor = QString("background-color: %1;").arg(bgColor.name());
    labelColor = "color: rgb(190, 190, 190);";
    indexBackgroundColor = "background-color: rgb(48, 48, 48);";
    break;
  case WidgetState::Executing:
    widgetBackgroundColor = "background-color: rgb(130, 130, 130);";
    labelColor = "color: rgb(190, 190, 190);";
    indexBackgroundColor = "background-color: rgb(6, 140, 190);";
    break;
  case WidgetState::Completed:
    widgetBackgroundColor = QString("background-color: %1;").arg(bgColor.name());
    labelColor = "color: rgb(190, 190, 190);";
    indexBackgroundColor = "background-color: rgb(6, 118, 6);";
    break;
  }

  QColor selectedColor = QColor::fromHsv(bgColor.hue(), 100, 120);

  switch(pState)
  {
  case PipelineState::Running:
    widgetBackgroundColor = QString("background-color: %1;").arg(selectedColor.name());
    labelColor = "color: rgb(190, 190, 190);";
    break;
  case PipelineState::Stopped:
    widgetBackgroundColor = QString("background-color: %1;").arg(bgColor.name());
    labelColor = "color: rgb(0, 0, 0);";
    break;
  case PipelineState::Paused:
    widgetBackgroundColor = "background-color: rgb(160, 160, 160);";
    labelColor = "color: rgb(0, 0, 0);";
    break;
  }

  switch(eState)
  {
  case ErrorState::Ok:

    break;
  case ErrorState::Error:
    indexBackgroundColor = "background-color: rgb(179, 2, 5);";
    break;
  case ErrorState::Warning:
    indexBackgroundColor = "background-color: rgb(215, 197, 1);";
    break;
  }

  if(isSelected() == true)
  {
    QColor selectedColor = QColor::fromHsv(bgColor.hue(), 180, 150);

    svWidgetStyleStream << "border-top: 3px solid " << selectedColor.name() << ";";
    svWidgetStyleStream << "border-right: 3px solid " << selectedColor.name() << ";";
    svWidgetStyleStream << "border-bottom: 3px solid " << selectedColor.name() << ";";
    svWidgetStyleStream << "padding-left: 3px;";
    filterIndexStyleStream << "border-top: 3px solid " << selectedColor.name() << ";";
    filterIndexStyleStream << "border-left: 3px solid " << selectedColor.name() << ";";
    filterIndexStyleStream << "border-bottom: 3px solid " << selectedColor.name() << ";";
    filterIndexStyleStream << "padding-right: 3px;";
    labelColor = "color: rgb(235, 235, 235);";
  }
  else if(isSelected() == false && hasRightClickTarget() == true)
  {
    svWidgetStyleStream << "border-top: 3px solid " << selectedColor.name() << ";";
    svWidgetStyleStream << "border-right: 3px solid " << selectedColor.name() << ";";
    svWidgetStyleStream << "border-bottom: 3px solid " << selectedColor.name() << ";";
    svWidgetStyleStream << "padding-left: 3px;";
    filterIndexStyleStream << "border-top: 3px solid " << selectedColor.name() << ";";
    filterIndexStyleStream << "border-left: 3px solid " << selectedColor.name() << ";";
    filterIndexStyleStream << "border-bottom: 3px solid " << selectedColor.name() << ";";
    filterIndexStyleStream << "padding-right: 3px;";
    labelColor = "color: rgb(235, 235, 235);";
  }
  else
  {
    svWidgetStyleStream << "padding: 3px;";
    filterIndexStyleStream << "padding: 3px;";
  }

  svWidgetStyleStream << widgetBackgroundColor;
  labelStyleStream << labelColor;
  filterIndexStyleStream << indexBackgroundColor;

  svWidgetStyleStream << "}\n";
  labelStyleStream << "}";
  filterIndexStyleStream << "}\n";

  // Set the Style Sheet
  frame->setStyleSheet(svWidgetStyle + labelStyle);
  filterIndex->setStyleSheet(filterIndexStyle);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::on_deleteBtn_clicked()
{
  emit filterWidgetRemoved(this);
  emit filterWidgetPressed(nullptr, qApp->queryKeyboardModifiers());
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
void SVPipelineFilterWidget::mouseReleaseEvent(QMouseEvent* event)
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
void SVPipelineFilterWidget::getGuiParametersFromFilter(AbstractFilter* filt)
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
  getFilterInputWidget()->toRunningState();
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toExecutingState()
{
  PipelineFilterObject::toExecutingState();
  getFilterInputWidget()->toRunningState();
  changeStyle();
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
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toRunningState()
{
  PipelineFilterObject::toRunningState();
  getFilterInputWidget()->toRunningState();
  deleteBtn->setDisabled(true);
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toStoppedState()
{
  PipelineFilterObject::toStoppedState();
  getFilterInputWidget()->toIdleState();
  deleteBtn->setEnabled(true);
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toPausedState()
{
  PipelineFilterObject::toPausedState();
  getFilterInputWidget()->toIdleState();
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toOkState()
{
  PipelineFilterObject::toOkState();
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toErrorState()
{
  PipelineFilterObject::toErrorState();
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toWarningState()
{
  PipelineFilterObject::toWarningState();
  changeStyle();
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
