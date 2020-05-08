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

#include "FloatVec3Widget.h"

#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"

#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Widget::FloatVec3Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<FloatVec3FilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "FloatVec3Widget can ONLY be used with a FloatVec3FilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Widget::~FloatVec3Widget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec3Widget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &FloatVec3Widget::beforePreflight);

  // Catch when the filter is finished running the preflight
  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &FloatVec3Widget::afterPreflight);

  // Catch when the filter wants its values updated
  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &FloatVec3Widget::filterNeedsInputParameters);

  connect(xData, SIGNAL(textChanged(const QString&)), this, SLOT(xDataChanged(const QString&)));
  connect(yData, SIGNAL(textChanged(const QString&)), this, SLOT(yDataChanged(const QString&)));
  connect(zData, SIGNAL(textChanged(const QString&)), this, SLOT(zDataChanged(const QString&)));

  QLocale loc = QLocale::system();

  QDoubleValidator* xVal = new QDoubleValidator(xData);
  xData->setValidator(xVal);
  xVal->setLocale(loc);

  QDoubleValidator* yVal = new QDoubleValidator(yData);
  yData->setValidator(yVal);
  yVal->setLocale(loc);

  QDoubleValidator* zVal = new QDoubleValidator(zData);
  zData->setValidator(zVal);
  zVal->setLocale(loc);
  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());

    FloatVec3Type data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<FloatVec3Type>();

    xData->setText(loc.toString(data[0]));
    yData->setText(loc.toString(data[1]));
    zData->setText(loc.toString(data[2]));
  }

  errorLabel->hide();
}

// -----------------------------------------------------------------------------
void FloatVec3Widget::xDataChanged(const QString& value)
{
  widgetChanged(xData, value);
}

// -----------------------------------------------------------------------------
void FloatVec3Widget::yDataChanged(const QString& value)
{
  widgetChanged(yData, value);
}

// -----------------------------------------------------------------------------
void FloatVec3Widget::zDataChanged(const QString& value)
{
  widgetChanged(zData, value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec3Widget::widgetChanged(QLineEdit* le, const QString& msg)
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

  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec3Widget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = false;
  FloatVec3Type defValue = m_FilterParameter->getDefaultValue().value<FloatVec3Type>();
  FloatVec3Type data;

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

  data[2] = loc.toFloat(zData->text(), &ok);
  if(!ok)
  {
    SVStyle::Instance()->LineEditBackgroundErrorStyle(zData);
    SVStyle::Instance()->SetErrorColor("QLabel", errorLabel);
    errorLabel->setText("Z Value entered is beyond the representable range for a float.\nThe filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
    data[2] = defValue[2];
  }

  Q_UNUSED(filter)
  FloatVec3FilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
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
void FloatVec3Widget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec3Widget::afterPreflight()
{
}
