/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "UInt64Widget.h"

#include "SIMPLib/FilterParameters/UInt64FilterParameter.h"
#include "SIMPLib/Utilities/FilterCompatibility.hpp"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt64Widget::UInt64Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = SIMPL_FILTER_PARAMETER_COMPATIBILITY_CHECK(filter, parameter, UInt64Widget, UInt64FilterParameter);

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt64Widget::~UInt64Widget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UInt64Widget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &UInt64Widget::beforePreflight);

  // Catch when the filter is finished running the preflight
  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &UInt64Widget::afterPreflight);

  // Catch when the filter wants its values updated
  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &UInt64Widget::filterNeedsInputParameters);

  connect(value, SIGNAL(textChanged(const QString&)), this, SLOT(widgetChanged(const QString&)));

  QLocale loc = QLocale::system();
  loc.setNumberOptions(QLocale::NumberOption::RejectGroupSeparator | QLocale::NumberOption::OmitGroupSeparator);

  QIntValidator* xVal = new QIntValidator(value);
  xVal->setLocale(loc);
  value->setValidator(xVal);

  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());
    uint64_t ui64 = m_FilterParameter->getGetterCallback()();
    QString str = loc.toString(ui64);
    value->setText(str);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UInt64Widget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UInt64Widget::filterNeedsInputParameters(AbstractFilter* filter)
{
  Q_UNUSED(filter)

  bool ok = true;
  uint64_t defValue = getFilterParameter()->getDefaultValue().toULongLong();
  uint64_t i = defValue;

  // Next make sure there is something in the
  if(!value->text().isEmpty())
  {
    i = value->locale().toULongLong(value->text(), &ok);
    //  make sure we can convert the entered value to a 32 bit signed int
    if(!ok)
    {
      SVStyle::Instance()->LineEditBackgroundErrorStyle(value);
      SVStyle::Instance()->SetErrorColor("QLabel", errorLabel);
      errorLabel->setText("Value entered is beyond the representable range for a 64 bit integer. The filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
      errorLabel->show();
      i = defValue;
    }
    else
    {
      errorLabel->hide();
      SVStyle::Instance()->LineEditClearStyle(value);
    }
  }
  else
  {
    SVStyle::Instance()->LineEditBackgroundErrorStyle(value);
    SVStyle::Instance()->SetErrorColor("QLabel", errorLabel);
    errorLabel->setText("No value entered. Filter will use default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
  }

  UInt64FilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
  if(setter)
  {
    setter(i);
  }
  else
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UInt64Widget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UInt64Widget::afterPreflight()
{
}
