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

#include "RangeWidget.h"

#include <QtCore/QMetaProperty>

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include <QtCore/QPropertyAnimation>
#include <QtCore/QSequentialAnimationGroup>

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RangeWidget::RangeWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<RangeFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "RangeWidget can ONLY be used with a RangeFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RangeWidget::~RangeWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RangeWidget::setupGui()
{
  blockSignals(true);
  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());

    QPair<double, double> range = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<QPair<double, double>>();
    minValue->setText(QString::number(range.first));
    maxValue->setText(QString::number(range.second));
  }
  blockSignals(false);

  minValue->setValidator(new QDoubleValidator(minValue));
  maxValue->setValidator(new QDoubleValidator(maxValue));

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RangeWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RangeWidget::afterPreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RangeWidget::on_minValue_textChanged(const QString& text)
{
  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RangeWidget::on_maxValue_textChanged(const QString& text)
{
  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RangeWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QPair<double, double> pair;
  pair.first = minValue->text().toDouble();
  pair.second = maxValue->text().toDouble();

  QVariant var;
  var.setValue(pair);
  QString propertyName = PROPERTY_NAME_AS_CHAR;
  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}
