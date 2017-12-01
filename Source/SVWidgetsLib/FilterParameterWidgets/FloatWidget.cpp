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

#include "FloatWidget.h"

#include <QtCore/QMetaProperty>

#include "SVWidgetsLib/QtSupport/QtSStyles.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatWidget::FloatWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<FloatFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "FloatWidget can ONLY be used with a FloatFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatWidget::~FloatWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatWidget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  connect(value, SIGNAL(textChanged(const QString&)), this, SLOT(widgetChanged(const QString&)));

  QLocale loc = QLocale::system();

  QDoubleValidator* xVal = new QDoubleValidator(value);
  value->setValidator(xVal);
  xVal->setLocale(loc);

  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());
    bool ok = false;

    float d = getFilter()->property(PROPERTY_NAME_AS_CHAR).toFloat(&ok);
    QString str = loc.toString(d);

    //  QString str = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
    value->setText(str);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatWidget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = true;
  float defValue = getFilterParameter()->getDefaultValue().toFloat();
  float i = defValue;
  QLocale loc;
  // Next make sure there is something in the
  if(!value->text().isEmpty())
  {
    i = loc.toFloat(value->text(), &ok);
    // i = value->text().toFloat(&ok);
    //  make sure we can convert the entered value to a 32 bit signed int
    if(!ok)
    {
      errorLabel->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
      errorLabel->setText("Value entered is beyond the representable range for a float.\nThe filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
      errorLabel->show();
      QtSStyles::LineEditErrorStyle(value);
      i = defValue;
    }
    else
    {
      errorLabel->hide();
      QtSStyles::LineEditClearStyle(value);
    }
  }
  else
  {
    QtSStyles::LineEditErrorStyle(value);
    errorLabel->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
    errorLabel->setText("No value entered. Filter will use default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
  }

  QVariant v(i);
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatWidget::afterPreflight()
{
}
