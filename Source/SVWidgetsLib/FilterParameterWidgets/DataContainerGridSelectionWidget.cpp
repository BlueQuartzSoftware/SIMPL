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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DataContainerGridSelectionWidget.h"

#include <QtCore/QSignalBlocker>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/FilterParameters/DataContainerGridSelectionFilterParameter.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetUtils.hpp"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"
#include "SVWidgetsLib/FilterParameterWidgets/DataContainerGridModel.h"
#include "SVWidgetsLib/ui_DataContainerGridSelectionWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridSelectionWidget::DataContainerGridSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_Ui(new Ui::DataContainerGridSelectionWidget)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<DataContainerGridSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "DataContainerGridSelectionWidget can ONLY be used with a DataContainerGridSelectionFilterParameter object");

  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridSelectionWidget::DataContainerGridSelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_Ui(new Ui::DataContainerGridSelectionWidget)
, m_DidCausePreflight(false)
{
  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridSelectionWidget::~DataContainerGridSelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionWidget::setupGui()
{
  if(getFilter() == nullptr)
  {
    return;
  }

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)), this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  if(getFilterParameter() == nullptr)
  {
    return;
  }
  m_Ui->propertyLabel->setText(getFilterParameter()->getHumanLabel());

  m_Ui->dataContainerGridTableView->setModel(new DataContainerGridModel(m_FilterParameter->getGetterCallback()()));

  changeStyleSheet(Style::FS_STANDARD_STYLE);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerGridSelectionWidget::checkStringValues(const QString& curDcName, const QString& filtDcName)
{
  if(curDcName.isEmpty() && !filtDcName.isEmpty())
  {
    return filtDcName;
  }
  if(!curDcName.isEmpty() && filtDcName.isEmpty())
  {
    return curDcName;
  }
  if(!curDcName.isEmpty() && !filtDcName.isEmpty() && m_DidCausePreflight)
  {
    return curDcName;
  }

  return filtDcName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionWidget::updateDataArrayPath(const QString& propertyName, const DataArrayPath::RenameType& renamePath)
{
  DataContainerGridSelectionFilterParameter::GetterCallbackType getter = m_FilterParameter->getGetterCallback();
  if(getter)
  {
    QSignalBlocker blocker(this);
    //setSelectedPath(getter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }
  if(m_DidCausePreflight)
  {
    // std::cout << "***  DataContainerGridSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionWidget::afterPreflight()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  DataContainerGrid dcg;
  DataContainerGridModel* model = dynamic_cast<DataContainerGridModel*>(m_Ui->dataContainerGridTableView->model());
  model->getTableData(dcg);

  DataContainerGridSelectionFilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
  if(setter)
  {
    setter(dcg);
  }
}
