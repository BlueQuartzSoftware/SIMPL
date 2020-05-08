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
 *    United States Air Force Prime Contract FA8650-15-D-5280
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "LinkedPathCreationWidget.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationWidget::LinkedPathCreationWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<LinkedPathCreationFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "LinkedPathCreationWidget can ONLY be used with a StringFilterParameter object");

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationWidget::~LinkedPathCreationWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedPathCreationWidget::setupGui()
{
  setupUi(this);

  blockSignals(true);
  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());

    QString str = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
    stringEdit->setText(str, true);
  }
  blockSignals(false);

  stringEdit->hideButtons();

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &LinkedPathCreationWidget::beforePreflight);

  // Catch when the filter is finished running the preflight
  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &LinkedPathCreationWidget::afterPreflight);

  // Catch when the filter wants its values updated
  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &LinkedPathCreationWidget::filterNeedsInputParameters);

  connect(stringEdit, SIGNAL(valueChanged(const QString&)), this, SIGNAL(parametersChanged()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedPathCreationWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedPathCreationWidget::afterPreflight()
{
  dataPathDisplay->setDataArrayPath(m_FilterParameter->getLinkedDataArrayPath());
  style()->unpolish(this);
  style()->polish(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathHelper::DataType LinkedPathCreationWidget::getDataType() const
{
  DataArrayPathHelper::DataType type = dataPathDisplay->getDataType();
  int typei = static_cast<int>(type);
  typei = (typei + 1) % 3;
  type = static_cast<DataArrayPathHelper::DataType>(typei);
  return type;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LinkedPathCreationWidget::getDataTypei() const
{
  return static_cast<int>(getDataType());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedPathCreationWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  Q_UNUSED(filter)
  LinkedPathCreationFilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
  if(setter)
  {
    setter(stringEdit->getText());
  }
  else
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}
