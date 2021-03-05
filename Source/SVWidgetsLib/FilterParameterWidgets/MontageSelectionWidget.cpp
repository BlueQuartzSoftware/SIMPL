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

#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/MontageSelectionFilterParameter.h"
#include "SIMPLib/Utilities/FilterCompatibility.hpp"

#include "SVWidgetsLib/ui_MontageSelectionWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSelectionWidget::MontageSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
, m_Ui(new Ui::MontageSelectionWidget)
{
  m_FilterParameter = SIMPL_FILTER_PARAMETER_COMPATIBILITY_CHECK(filter, parameter, MontageSelectionWidget, MontageSelectionFilterParameter);

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
  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &MontageSelectionWidget::beforePreflight);

  // Catch when the filter is finished running the preflight
  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &MontageSelectionWidget::afterPreflight);

  // Catch when the filter wants its values updated
  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &MontageSelectionWidget::filterNeedsInputParameters);

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)), this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  // Set FilterParameter values
  MontageSelection selection = dynamic_cast<MontageSelectionFilterParameter*>(getFilterParameter())->getGetterCallback()();
  m_Ui->prefixStringEdit->setText(selection.getPrefix());
  m_Ui->suffixStringEdit->setText(selection.getSuffix());
  m_Ui->paddingSpinBox->setValue(selection.getPadding());
  m_Ui->rowStartSpinBox->setValue(selection.getRowStart());
  m_Ui->rowEndSpinBox->setValue(selection.getRowEnd());
  m_Ui->colStartSpinBox->setValue(selection.getColStart());
  m_Ui->colEndSpinBox->setValue(selection.getColEnd());

  // Connect widget signals
  connect(m_Ui->prefixStringEdit, &QtSStringEdit::valueChanged, this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->suffixStringEdit, &QtSStringEdit::valueChanged, this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->paddingSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->rowStartSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->rowEndSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->colStartSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MontageSelectionWidget::causePreflight);
  connect(m_Ui->colEndSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MontageSelectionWidget::causePreflight);
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
  populateDataContainerTable();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelectionWidget::populateDataContainerTable()
{
  MontageSelection selection = getMontageSelection();
  const int rowCount = selection.getRowEnd() - selection.getRowStart() + 1;
  const int colCount = selection.getColEnd() - selection.getColStart() + 1;

  m_Ui->dataContainerTableWidget->clear();
  m_Ui->dataContainerTableWidget->setRowCount(rowCount);
  m_Ui->dataContainerTableWidget->setColumnCount(colCount);
  for(int row = 0; row < rowCount; row++)
  {
    for(int col = 0; col < colCount; col++)
    {
      QString dcName = selection.getDataContainerName(row + selection.getRowStart(), col + selection.getColStart());
      m_Ui->dataContainerTableWidget->setItem(row, col, new QTableWidgetItem(getDcIcon(dcName), dcName));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon MontageSelectionWidget::getDcIcon(const QString& dcName) const
{
  // Do not recreate the variables with every call, and these are not variables
  // that make sense to be available outside of this method.
  static QIcon dcFoundIcon = QIcon(":SIMPL/icons/images/bullet_ball_green.png");
  static QIcon dcNotFoundIcon = QIcon(":SIMPL/icons/images/bullet_ball_red.png");

  bool dcExists = (getFilter()->getDataContainerArray()->getDataContainer(dcName) != nullptr);
  return dcExists ? dcFoundIcon : dcNotFoundIcon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelectionWidget::causePreflight()
{
  m_DidCausePreflight = true;
  Q_EMIT parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  MontageSelection selection = getMontageSelection();

  Q_UNUSED(filter)
  MontageSelectionFilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
  if(setter)
  {
    setter(selection);
  }
  else
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}
