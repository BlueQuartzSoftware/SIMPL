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

#include "SVPipelineFilterOutlineWidget.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include <QtWidgets/QMessageBox>

#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/QtSupport/QtSStyles.h"
#include "SVWidgetsLib/Widgets/SVPipelineFilterWidget.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineFilterOutlineWidget::SVPipelineFilterOutlineWidget(AbstractFilter* filter, QWidget* parent)
: QFrame(parent)
, m_Filter(filter)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineFilterOutlineWidget::~SVPipelineFilterOutlineWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterOutlineWidget::setupGui()
{
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterOutlineWidget::setFilter(AbstractFilter* filter)
{
  m_Filter = filter;
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterOutlineWidget::changeStyle()
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

  fontStringStream << "pt \"" << font.family()  << "\";";

#if defined(Q_OS_WIN)
  if(font.bold()) {
    fontStringStream << "font-weight: bold;";
  }
#endif

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
#if defined(Q_OS_LINUX)
  fontStringStream << "font-weight: bold;";
#endif
  filterIndexStyleStream << "QLabel\n{";
  filterIndexStyleStream << fontString;
  filterIndexStyleStream << "color: rgb(242, 242, 242);"; // Always have a white'ish font
  filterIndexStyleStream << "border-top-left-radius: 3px;";
  filterIndexStyleStream << "border-bottom-left-radius: 3px;";

  SVPipelineFilterWidget::WidgetState wState = SVPipelineFilterWidget::WidgetState::Ready;
  SVPipelineFilterWidget::PipelineState pState = SVPipelineFilterWidget::PipelineState::Stopped;
  SVPipelineFilterWidget::ErrorState eState = SVPipelineFilterWidget::ErrorState::Ok;

  QString widgetBackgroundColor;
  QString labelColor;
  QString indexBackgroundColor;
  QColor bgColor;
//  if(m_Filter)
//  {
//    bgColor = QtSStyles::ColorForFilterGroup(m_Filter->getGroupName());
//  }
//  else
  {
    bgColor = QColor(150, 150, 150);
  }

  switch(wState)
  {
    case SVPipelineFilterWidget::WidgetState::Ready:
      widgetBackgroundColor = QString("background-color: %1;").arg(bgColor.name());
      labelColor = "color: rgb(190, 190, 190);";
      indexBackgroundColor = "background-color: rgb(48, 48, 48);";
      break;
    case SVPipelineFilterWidget::WidgetState::Executing:
      widgetBackgroundColor = "background-color: rgb(130, 130, 130);";
      labelColor ="color: rgb(190, 190, 190);";
      indexBackgroundColor = "background-color: rgb(6, 140, 190);";
      break;
    case SVPipelineFilterWidget::WidgetState::Completed:
      widgetBackgroundColor = QString("background-color: %1;").arg(bgColor.name());
      labelColor ="color: rgb(190, 190, 190);";
      indexBackgroundColor = "background-color: rgb(6, 118, 6);";
      break;
    case SVPipelineFilterWidget::WidgetState::Disabled:
      break;
  }

  QColor selectedColor = QColor::fromHsv(bgColor.hue(), 100, 120);

  switch(pState)
  {
    case SVPipelineFilterWidget::PipelineState::Running:
      widgetBackgroundColor = QString("background-color: %1;").arg(selectedColor.name());
      labelColor = "color: rgb(190, 190, 190);";
      break;
    case SVPipelineFilterWidget::PipelineState::Stopped:
      widgetBackgroundColor = QString("background-color: %1;").arg(bgColor.name());
      labelColor = "color: rgb(0, 0, 0);";
      break;
    case SVPipelineFilterWidget::PipelineState::Paused:
      widgetBackgroundColor = "background-color: rgb(160, 160, 160);";
      labelColor = "color: rgb(0, 0, 0);";
      break;
  }

  switch(eState)
  {
    case SVPipelineFilterWidget::ErrorState::Ok:

      break;
    case SVPipelineFilterWidget::ErrorState::Error:
      indexBackgroundColor = "background-color: rgb(179, 2, 5);";
      break;
    case SVPipelineFilterWidget::ErrorState::Warning:
      indexBackgroundColor = "background-color: rgb(172, 168, 0);";
      break;
  }


  if(isSelected() == true)
  {
    QColor selectedColor = QColor::fromHsv(bgColor.hue(), 180, 150);

    svWidgetStyleStream << "border-top: 3px solid " <<  selectedColor.name() << ";";
    svWidgetStyleStream << "border-right: 3px solid " <<  selectedColor.name() << ";";
    svWidgetStyleStream << "border-bottom: 3px solid " <<  selectedColor.name() << ";";
    svWidgetStyleStream << "padding-left: 3px;";
    filterIndexStyleStream << "border-top: 3px solid " <<  selectedColor.name() << ";";
    filterIndexStyleStream << "border-left: 3px solid " <<  selectedColor.name() << ";";
    filterIndexStyleStream << "border-bottom: 3px solid " <<  selectedColor.name() << ";";
    filterIndexStyleStream << "padding-right: 3px;";
  }
  else if(isSelected() == false && hasRightClickTarget() == true)
  {
    svWidgetStyleStream << "border-top: 3px solid " <<  selectedColor.name() << ";";
    svWidgetStyleStream << "border-right: 3px solid " <<  selectedColor.name() << ";";
    svWidgetStyleStream << "border-bottom: 3px solid " <<  selectedColor.name() << ";";
    svWidgetStyleStream << "padding-left: 3px;";
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

#if defined(Q_OS_WIN)
  if (isSelected() == true)
  {
    filterName->setStyleSheet("color: rgb(242, 242, 242);");
  }
  else
  {
    filterName->setStyleSheet("");
  }
#endif

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVPipelineFilterOutlineWidget::isSelected()
{
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVPipelineFilterOutlineWidget::hasRightClickTarget()
{
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterOutlineWidget::setFilterTitle(QString name)
{
  filterName->setText(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterOutlineWidget::setFilterIndex(int i, int numFilters)
{
  if (numFilters < 10) { numFilters = 11; }
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
