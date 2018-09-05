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

#include <QtCore/QMetaProperty>

#include "SIMPLib/FilterParameters/FloatVec2FilterParameter.h"

#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec2Widget::FloatVec2Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<FloatVec2FilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "FloatVec2Widget can ONLY be used with a FloatVec2FilterParameter object");

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
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  connect(xData, SIGNAL(textChanged(const QString&)), this, SLOT(widgetChanged(const QString&)));
  connect(yData, SIGNAL(textChanged(const QString&)), this, SLOT(widgetChanged(const QString&)));

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

    FloatVec2_t data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<FloatVec2_t>();

    xData->setText(loc.toString(data.x));
    yData->setText(loc.toString(data.y));
  }

  errorLabel->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec2Widget::widgetChanged(const QString& text)
{
  Q_UNUSED(text);

  QLineEdit* le = nullptr;
  (sender() == xData) ? le = xData : le = nullptr;
  (sender() == yData) ? le = yData : le = nullptr;

  errorLabel->hide();

  if(le)
  {
    if(le->text().isEmpty())
    {
      SVStyle::Instance()->LineEditErrorStyle(le);
      SVStyle::Instance()->SetErrorColor("QLabel", errorLabel);
      errorLabel->setText("No value entered. Filter will use default value of " + getFilterParameter()->getDefaultValue().toString());
      errorLabel->show();
    }
  }

  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec2Widget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = false;
  FloatVec2_t defValue = m_FilterParameter->getDefaultValue().value<FloatVec2_t>();
  FloatVec2_t data;

  QLocale loc;

  data.x = loc.toFloat(xData->text(), &ok);
  if(!ok)
  {
    SVStyle::Instance()->LineEditBackgroundErrorStyle(xData);
    SVStyle::Instance()->SetErrorColor("QLabel", errorLabel);    errorLabel->setText("X Value entered is beyond the representable range for a double.\nThe filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
    data.x = defValue.x;
  }

  data.y = loc.toFloat(yData->text(), &ok);
  if(!ok)
  {
    SVStyle::Instance()->LineEditBackgroundErrorStyle(yData);
    SVStyle::Instance()->SetErrorColor("QLabel", errorLabel);    errorLabel->setText("Y Value entered is beyond the representable range for a double.\nThe filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
    data.y = defValue.y;
  }

  QVariant v;
  v.setValue(data);
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);
  if(false == ok)
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
