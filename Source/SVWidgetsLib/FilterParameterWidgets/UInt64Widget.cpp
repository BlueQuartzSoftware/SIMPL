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

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt64Widget::UInt64Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<UInt64FilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "UInt64Widget can ONLY be used with a UInt64FilterParameter object");

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

  QIntValidator* xVal = new QIntValidator(value);
  value->setValidator(xVal);

  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());

    QString str = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
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
  int defValue = getFilterParameter()->getDefaultValue().toInt();
  uint64_t i = static_cast<uint64_t>(defValue);

  // Next make sure there is something in the
  if(!value->text().isEmpty())
  {
    i = value->text().toULongLong(&ok);
    //  make sure we can convert the entered value to a 32 bit signed int
    if(!ok)
    {
      SVStyle::Instance()->LineEditBackgroundErrorStyle(value);
      SVStyle::Instance()->SetErrorColor("QLabel", errorLabel);
      errorLabel->setText("Value entered is beyond the representable range for a 64 bit integer. The filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
      errorLabel->show();
      i = static_cast<uint64_t>(defValue);
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
