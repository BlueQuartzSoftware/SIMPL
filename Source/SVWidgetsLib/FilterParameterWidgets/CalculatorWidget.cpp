/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "CalculatorWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QSequentialAnimationGroup>

#include <QtWidgets/QMenu>

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculatorWidget::CalculatorWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_ScalarsMenu(nullptr)
, m_VectorsMenu(nullptr)
, m_SelectedText("")
, m_SelectionStart(-1)
{
  m_Filter = dynamic_cast<ArrayCalculator*>(filter);
  Q_ASSERT_X(m_Filter != nullptr, "NULL Pointer", "CalculatorWidget can ONLY be used with an ArrayCalculator filter");

  m_FilterParameter = dynamic_cast<CalculatorFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "CalculatorWidget can ONLY be used with a CalculatorFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculatorWidget::~CalculatorWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::setupGui()
{
  blockSignals(true);
  if(getFilterParameter() != nullptr)
  {
    QString str = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
    equation->setText(str);
  }
  blockSignals(false);

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)),
    this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  connect(equation, SIGNAL(textChanged(const QString&)), this, SLOT(widgetChanged(const QString&)));

  connect(equation, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));

  // Unary Operator Buttons
  connect(absBtn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));
  connect(sinBtn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));
  connect(cosBtn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));
  connect(tanBtn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));
  connect(sqrtBtn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));
  connect(asinBtn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));
  connect(acosBtn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));
  connect(atanBtn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));
  connect(ceilBtn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));
  connect(floorBtn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));
  connect(log10Btn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));
  connect(lnBtn, SIGNAL(clicked()), this, SLOT(printUnaryButtonName()));

  // Other Buttons
  connect(leftBraceBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(rightBraceBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(commaBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(addBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(subtractBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(multiplyBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(divideBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(oneBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(twoBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(threeBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(fourBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(fiveBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(sixBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(sevenBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(eightBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(nineBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(zeroBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));
  connect(decimalBtn, SIGNAL(clicked()), this, SLOT(printButtonName()));

  applyChangesBtn->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::printUnaryButtonName()
{
  QPushButton* button = static_cast<QPushButton*>(sender());
  if(nullptr != button)
  {
    printStringToEquation(button->text() + "(");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::printButtonName()
{
  QPushButton* button = static_cast<QPushButton*>(sender());
  if(nullptr != button)
  {
    printStringToEquation(button->text());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::printActionName()
{
  QAction* action = static_cast<QAction*>(sender());
  if(nullptr != action)
  {
    printStringToEquation(action->text());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::printStringToEquation(QString str)
{
  QString equationText = equation->text();

  if(m_SelectedText.isEmpty() == false && m_SelectionStart >= 0)
  {
    equationText.replace(m_SelectionStart, m_SelectedText.size(), str);
    equation->setFocus();
    equation->setText(equationText);
    equation->setCursorPosition(m_SelectionStart + str.size());

    m_SelectedText.clear();
    m_SelectionStart = -1;
  }
  else
  {
    int pos = equation->cursorPosition();
    equationText.insert(pos, str);
    equation->setText(equationText);
    equation->setCursorPosition(pos + str.size());
    equation->setFocus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_clearBtn_clicked()
{
  equation->clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_backspaceBtn_clicked()
{
  QString equationText = equation->text();
  equationText.chop(1);
  equation->setText(equationText);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_radiansBtn_toggled(bool checked)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_piBtn_clicked()
{
  printStringToEquation("3.1415927");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_logBtn_clicked()
{
  printStringToEquation("log(");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_expBtn_clicked()
{
  printStringToEquation("exp(");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_powBtn_clicked()
{
  printStringToEquation("^");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_rootBtn_clicked()
{
  printStringToEquation("root(");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_scalarsBtn_clicked()
{
  if(nullptr != m_VectorsMenu)
  {
    delete m_ScalarsMenu;
    m_ScalarsMenu = nullptr;
  }

  // Create Scalars Menu
  m_ScalarsMenu = new QMenu(this);

  AttributeMatrix::Pointer am = m_Filter->getDataContainerArray()->getAttributeMatrix(m_Filter->getSelectedAttributeMatrix());
  if(nullptr == am)
  {
    return;
  }

  QStringList nameList = am->getAttributeArrayNames();

  for(int i = 0; i < nameList.size(); i++)
  {
    if(am->getAttributeArray(nameList[i])->getComponentDimensions()[0] == 1)
    {
      QAction* action = new QAction(nameList[i], m_ScalarsMenu);
      connect(action, SIGNAL(triggered()), this, SLOT(printActionName()));
      m_ScalarsMenu->addAction(action);
    }
  }

  scalarsBtn->setMenu(m_ScalarsMenu);

  scalarsBtn->showMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_vectorsBtn_clicked()
{
  if(nullptr != m_VectorsMenu)
  {
    delete m_VectorsMenu;
    m_VectorsMenu = nullptr;
  }

  // Create Vectors Menu
  m_VectorsMenu = new QMenu(this);

  AttributeMatrix::Pointer am = m_Filter->getDataContainerArray()->getAttributeMatrix(m_Filter->getSelectedAttributeMatrix());
  if(nullptr == am)
  {
    return;
  }

  QStringList nameList = am->getAttributeArrayNames();

  for(int i = 0; i < nameList.size(); i++)
  {
    if(am->getAttributeArray(nameList[i])->getComponentDimensions()[0] > 1)
    {
      QAction* action = new QAction(nameList[i], m_VectorsMenu);
      connect(action, SIGNAL(triggered()), this, SLOT(printActionName()));
      m_VectorsMenu->addAction(action);
    }
  }

  vectorsBtn->setMenu(m_VectorsMenu);

  vectorsBtn->showMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_equation_returnPressed()
{
  // qDebug() << "DataArrayCreationWidget::on_value_returnPressed() " << this;
  m_DidCausePreflight = true;
  on_applyChangesBtn_clicked();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::updateSelection()
{
  if(equation->hasFocus())
  {
    m_SelectedText = equation->selectedText();
    m_SelectionStart = equation->selectionStart();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::hideButton()
{
  equation->setToolTip("");
  applyChangesBtn->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::widgetChanged(const QString& text)
{
  equation->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
  equation->setToolTip("Press the 'Return' key to apply your changes");
  if(applyChangesBtn->isVisible() == false)
  {
    applyChangesBtn->setVisible(true);
    fadeInWidget(applyChangesBtn);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::afterPreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, equation->text());
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }

  ArrayCalculator* calculatorFilter = dynamic_cast<ArrayCalculator*>(filter);
  Q_ASSERT_X(calculatorFilter != nullptr, "NULL Pointer", "CalculatorWidget can ONLY be used with an ArrayCalculator filter");

  if(radiansBtn->isChecked())
  {
    calculatorFilter->setUnits(ArrayCalculator::Radians);
  }
  else
  {
    calculatorFilter->setUnits(ArrayCalculator::Degrees);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::on_applyChangesBtn_clicked()
{
  equation->setStyleSheet(QString(""));
  emit parametersChanged();

  QPointer<QtSFaderWidget> faderWidget = new QtSFaderWidget(applyChangesBtn);
  setFaderWidget(faderWidget);

  if(getFaderWidget())
  {
    faderWidget->close();
  }
  faderWidget = new QtSFaderWidget(applyChangesBtn);
  faderWidget->setFadeOut();
  connect(faderWidget, SIGNAL(animationComplete()), this, SLOT(hideButton()));
  faderWidget->start();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorWidget::updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  if(propertyName.compare(getFilterParameter()->getPropertyName()) == 0)
  {
    QString inputStr = equation->text();
    inputStr.replace(oldPath.getDataArrayName(), newPath.getDataArrayName());
    equation->setText(inputStr);
  }
}
