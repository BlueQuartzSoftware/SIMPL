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

#include "IntVec3Widget.h"

#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3Widget::IntVec3Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<IntVec3FilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "IntVec3Widget can ONLY be used with a IntVec3FilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3Widget::~IntVec3Widget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec3Widget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &IntVec3Widget::beforePreflight);

  // Catch when the filter is finished running the preflight
  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &IntVec3Widget::afterPreflight);

  // Catch when the filter wants its values updated
  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &IntVec3Widget::filterNeedsInputParameters);

  connect(xData, SIGNAL(textChanged(const QString&)), this, SLOT(widgetChanged(const QString&)));
  connect(yData, SIGNAL(textChanged(const QString&)), this, SLOT(widgetChanged(const QString&)));
  connect(zData, SIGNAL(textChanged(const QString&)), this, SLOT(widgetChanged(const QString&)));

  QIntValidator* xVal = new QIntValidator(xData);
  xData->setValidator(xVal);
  QIntValidator* yVal = new QIntValidator(yData);
  yData->setValidator(yVal);
  QIntValidator* zVal = new QIntValidator(zData);
  zData->setValidator(zVal);

  if(getFilterParameter() != nullptr)
  {
    IntVec3WidgetLabel->setText(getFilterParameter()->getHumanLabel());

    IntVec3Type data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<IntVec3Type>();
    xData->setText(QString::number(data[0]));
    yData->setText(QString::number(data[1]));
    zData->setText(QString::number(data[2]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec3Widget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec3Widget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = false;
  IntVec3Type data;
  data[0] = xData->text().toInt(&ok);
  data[1] = yData->text().toInt(&ok);
  data[2] = zData->text().toInt(&ok);

  Q_UNUSED(filter)
  IntVec3FilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
  if(setter)
  {
    setter(data);
  }
  else
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec3Widget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec3Widget::afterPreflight()
{
}
