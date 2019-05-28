/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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

#include "MontageSelectionWidget.h"

#include "SIMPLib/FilterParameters/MontageSelectionFilterParameter.h"

#include "SVWidgetsLib/ui_MontageSelectionWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSelectionWidget::MontageSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
, m_Ui(new Ui::MontageSelectionWidget)
{
  m_FilterParameter = dynamic_cast<MontageSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "MontageSelectionWidget can ONLY be used with a MontageSelectionFilterParameter object");

  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSelectionWidget::MontageSelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelectionWidget::setupGui()
{
  // Sanity Check the filter and the filter parameter
  if(getFilter() == nullptr)
  {
    return;
  }
  if(getFilterParameter() == nullptr)
  {
    return;
  }

  // Lastly, hook up the filter's signals and slots to our own signals and slots
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)), this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  connect(m_Ui->prefixStringEdit, &QtSStringEdit::valueChanged, this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->suffixStringEdit, &QtSStringEdit::valueChanged, this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->paddingSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->rowStartSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->rowEndSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->colStartSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->colEndSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &MontageSelectionWidget::causePreflight);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSelection MontageSelectionWidget::getMontageSelection() const
{
  MontageSelection montage;
  montage.setPrefix(m_Ui->prefixStringEdit->getText());
  montage.setSuffix(m_Ui->suffixStringEdit->getText());
  montage.setPadding(m_Ui->paddingSpinBox->value());
  montage.setRowStart(m_Ui->rowStartSpinBox->value());
  montage.setRowEnd(m_Ui->rowEndSpinBox->value());
  montage.setColStart(m_Ui->colStartSpinBox->value());
  montage.setColEnd(m_Ui->colEndSpinBox->value());
  return montage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelectionWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelectionWidget::afterPreflight()
{
  MontageSelection selection = getMontageSelection();
  QStringList dcNames = selection.getDataContainerNames();
  m_Ui->dataContainerListWidget->clear();
  m_Ui->dataContainerListWidget->addItems(dcNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelectionWidget::causePreflight()
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  MontageSelection selection = getMontageSelection();

  QVariant var;
  var.setValue(selection);
  bool ok = false;

  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(!ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}
