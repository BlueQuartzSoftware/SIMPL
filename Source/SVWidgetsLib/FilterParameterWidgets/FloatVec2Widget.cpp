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

#include "FloatVec2Widget.h"

#include "SIMPLib/FilterParameters/FloatVec2FilterParameter.h"
#include "SIMPLib/Utilities/FilterCompatibility.hpp"

#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec2Widget::FloatVec2Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = SIMPL_FILTER_PARAMETER_COMPATIBILITY_CHECK(filter, parameter, FloatVec2Widget, FloatVec2FilterParameter);

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec2Widget::~FloatVec2Widget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec2Widget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &FloatVec2Widget::beforePreflight);

  // Catch when the filter is finished running the preflight
  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &FloatVec2Widget::afterPreflight);

  // Catch when the filter wants its values updated
  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &FloatVec2Widget::filterNeedsInputParameters);

  connect(xData, SIGNAL(textChanged(const QString&)), this, SLOT(xDataChanged(const QString&)));
  connect(yData, SIGNAL(textChanged(const QString&)), this, SLOT(yDataChanged(const QString&)));

  QLocale loc = QLocale::system();

  QDoubleValidator* xVal = new QDoubleValidator(xData);
  xData->setValidator(xVal);
  xVal->setLocale(loc);

  QDoubleValidator* yVal = new QDoubleValidator(yData);
  yData->setValidator(yVal);
  yVal->setLocale(loc);

  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());

    FloatVec2Type data = m_FilterParameter->getGetterCallback()();

    xData->setText(loc.toString(data[0]));
    yData->setText(loc.toString(data[1]));
  }

  errorLabel->hide();
}

// -----------------------------------------------------------------------------
void FloatVec2Widget::xDataChanged(const QString& value)
{
  widgetChanged(xData, value);
}

// -----------------------------------------------------------------------------
void FloatVec2Widget::yDataChanged(const QString& value)
{
  widgetChanged(yData, value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec2Widget::widgetChanged(QLineEdit* le, const QString& msg)
{
  Q_UNUSED(msg);
  if(nullptr == le)
  {
    return;
  }
  errorLabel->hide();

  if(le->text().isEmpty())
  {
    SVStyle::Instance()->LineEditBackgroundErrorStyle(le);
    SVStyle::Instance()->SetErrorColor("QLabel", errorLabel);
    errorLabel->setText("No value entered. Filter will use default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
  }
  else
  {
    SVStyle::Instance()->LineEditClearStyle(le);
  }

  Q_EMIT parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec2Widget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = false;
  FloatVec2Type defValue = m_FilterParameter->getDefaultValue().value<FloatVec2Type>();
  FloatVec2Type data;

  QLocale loc;

  data[0] = loc.toFloat(xData->text(), &ok);
  if(!ok)
  {
    SVStyle::Instance()->LineEditBackgroundErrorStyle(xData);
    SVStyle::Instance()->SetErrorColor("QLabel", errorLabel);
    errorLabel->setText("X Value entered is beyond the representable range for a float.\nThe filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
    data[0] = defValue[0];
  }

  data[1] = loc.toFloat(yData->text(), &ok);
  if(!ok)
  {
    SVStyle::Instance()->LineEditBackgroundErrorStyle(yData);
    SVStyle::Instance()->SetErrorColor("QLabel", errorLabel);
    errorLabel->setText("Y Value entered is beyond the representable range for a float.\nThe filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
    data[1] = defValue[1];
  }

  Q_UNUSED(filter)
  FloatVec2FilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
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
void FloatVec2Widget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec2Widget::afterPreflight()
{
}
