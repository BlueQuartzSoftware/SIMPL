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



#include <QtCore/QTimer>
#include <QtCore/QResource>
#include <QtCore/QDir>
#include <QtCore/QMimeData>

#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGroupBox>


#include "SVWidgetsLib/QtSupport/QtSStyles.h"

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/IFilterFactory.hpp"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/DocRequestManager.h"
#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerReaderFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/InputPathFilterParameter.h"

#include "SVWidgetsLib/Core/FilterWidgetManager.h"
#include "SVWidgetsLib/FilterParameterWidgets/LinkedBooleanWidget.h"
#include "SVWidgetsLib/FilterParameterWidgets/ChoiceWidget.h"
#include "SVWidgetsLib/Widgets/SVPipelineViewWidget.h"
#include "SVWidgetsLib/Widgets/DataContainerArrayWidget.h"



#define PADDING 5
#define BORDER 2
#define IMAGE_WIDTH 17
#define IMAGE_HEIGHT 17

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_SVPipelineFilterWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineFilterWidget::SVPipelineFilterWidget(QWidget* parent) :
  QFrame(parent),
  PipelineFilterObject(AbstractFilter::NullPointer()),
  m_Observer(NULL),
  m_FilterInputWidget(NULL)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineFilterWidget::SVPipelineFilterWidget(AbstractFilter::Pointer filter, IObserver* observer, QWidget* parent) :
  QFrame(parent),
  PipelineFilterObject(filter),
  m_Observer(observer),
  m_FilterInputWidget(NULL)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::initialize()
{
  setContextMenuPolicy(Qt::CustomContextMenu);
//FIXME: This needs to be pushed up to the PipelineViewWidget level
  //connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), dream3dApp, SLOT(on_pipelineFilterWidget_contextMenuRequested(const QPoint&)));

  connect(getFilterInputWidget(), SIGNAL(filterParametersChanged()), this, SLOT(filterInputWidget_filterParametersChanged()));
  connect(getFilterInputWidget(), SIGNAL(errorSettingFilterParameter(const QString&)), this, SLOT(displayFilterParameterWidgetError(const QString&)));

  setupUi(this);

  m_DeleteRect.setX(PADDING + BORDER);
  m_DeleteRect.setY(PADDING + BORDER);
  m_DeleteRect.setWidth(IMAGE_WIDTH);
  m_DeleteRect.setHeight(IMAGE_HEIGHT);

  // Set the Name of the filter into the FilterWidget
  filterName->setText(getFilter()->getHumanLabel() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::adjustLayout(QWidget* w, int state)
{
  if (state == Qt::Checked)
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
    PipelineMessage pm("Filter Paramter Widget", msg, -1, PipelineMessage::Error);
    m_Observer->processPipelineMessage(pm);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineFilterWidget::~SVPipelineFilterWidget()
{

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
void SVPipelineFilterWidget::changeStyle()
{
  QString style;
  QTextStream ss(&style);

  if(getHasPreflightWarnings())
  {
    ss << "border: 2px solid rgb(172, 168, 0);";
  }
  else if(getIsSelected() == true && getHasFocus() == true)
  {
    ss << "border: 2px solid MediumBlue;";
  }
  else if (getIsSelected() == true && getHasFocus() == false)
  {
    ss << "border: 2px solid DarkSlateGray;";
  }
  else
  {
    ss << "border: 1px solid #515151;";
    ss << "margin: 1px;";
  }
  setBorderColorStyle(style);
  updateWidgetStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::updateWidgetStyle()
{
  QString style;
  QTextStream ss(&style);

  ss << "SVPipelineFilterWidget {\n";


  if (getHasPreflightErrors() == true)
  {
    //ss << "background-color: rgb(200, 75, 75);\ncolor: rgb(255, 255, 255);";
    ss << "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(180, 55, 55, 255), stop:0.5 rgba(235, 110, 110, 255), stop:1 rgba(180, 55, 55, 255));\n";
  }
  else
  {
    ss << "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(185, 185, 185, 255), stop:0.5 rgba(216, 216, 216, 255), stop:1 rgba(170, 170, 170, 255));\n";
  }

  ss << "background-position: top ;\n background-repeat: repeat-x;";

  ss << getBorderColorStyle();

  ss << "border-radius: 8px;";
  ss << "padding: 0 0 0 0px;";
  ss << "}\n";

  ss << "QLabel\n {\n";

#if defined(Q_OS_WIN)
  ss << "font: 9pt \"" << QtSStyles::GetUIFont() << "\";";
#elif defined(Q_OS_MAC)
  ss << "font: 100 12pt \"" << QtSStyles::GetUIFont() << "\";";
#else
  ss << "font: 100 9pt \"" << QtSStyles::GetUIFont() << "\";";
  ss << "font-weight: bold;";
#endif

  if (getHasPreflightErrors() == true)
  {
    ss << "color: rgb(240, 240, 240);";
  }
  ss << "}\n";


  setStyleSheet(style);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::mousePressEvent ( QMouseEvent* event )
{
  if(event->button() == Qt::RightButton)
  {
    QFrame::mousePressEvent(event);
  }
  else if(event->button() == Qt::LeftButton)
  {
    // Only if we are inside the delete checkbox/image then pass up to the superclass
    if(m_DeleteRect.contains(event->pos()))
    {
      QFrame::mousePressEvent(event);
    }
    else
    {
      dragStartPosition = event->pos();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if(event->button() != Qt::LeftButton)
  {
    event->ignore();
    return;
  }
  // Only if we are inside the delete checkbox/image then pass up to the superclass
  if(m_DeleteRect.contains(event->pos()))
  {
    QFrame::mouseReleaseEvent(event);
  }
  else
  {
    emit filterWidgetPressed(this, qApp->queryKeyboardModifiers());
  }
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
  if((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
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
  Q_ASSERT("SVPipelineFilterWidget::getGuiParametersFromFilter executed");    // Code should never enter this function
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::on_deleteBtn_clicked()
{
  emit filterWidgetRemoved(this, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::filterInputWidget_filterParametersChanged()
{
  emit parametersChanged(QUuid());
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
void SVPipelineFilterWidget::toRunningState()
{
  m_FilterInputWidget->getVariablesTabContentsWidget()->setDisabled(true);
  deleteBtn->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterWidget::toIdleState()
{
  m_FilterInputWidget->getVariablesTabContentsWidget()->setEnabled(true);
  deleteBtn->setEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject* SVPipelineFilterWidget::deepCopy()
{
  SVPipelineFilterWidget* newWidget = new SVPipelineFilterWidget(getFilter()->newFilterInstance(true), NULL, dynamic_cast<QWidget*>(parent()));
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
