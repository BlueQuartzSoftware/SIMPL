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

#include "SVWidgetsLib/QtSupport/QtSStyles.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_SVPipelineFilterOutlineWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineFilterOutlineWidget::SVPipelineFilterOutlineWidget(QWidget* parent)
: QWidget(parent)
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
  updateWidgetStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterOutlineWidget::updateWidgetStyle()
{
  QString style;
  QTextStream ss(&style);

  ss << "QWidget#SVPipelineFilterOutlineWidget {\n";

  ss << "border: 2px solid rgb(149, 195, 255);\n";
  ss << "border-radius: 3px;\n";
  ss << "padding: 0 0 0 0px;\n";
  ss << "font: 100 14pt \"DejuVu Sans\";\n";
  ss << "font-weight: bold;\n";
  ss << "}\n";

  ss << "QLabel\n {\n";

#if defined(Q_OS_WIN)
  ss << "font: 9pt \"" << QtSStyles::GetUIFont() << "\";";
#elif defined(Q_OS_MAC)
  ss << "font: 100 14pt \"" << QtSStyles::GetUIFont() << "\";";
#else
  ss << "font: 100 9pt \"" << QtSStyles::GetUIFont() << "\";";
  ss << "font-weight: bold;";
#endif

  ss << "}\n";

  setStyleSheet(style);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineFilterOutlineWidget::setFilterName(QString name)
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
