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
#include "SIMPLib/FilterParameters/DataContainerGridSelectionFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "SVWidgetsLib/FilterParameterWidgets/DataContainerGridModel.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetUtils.hpp"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"
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

  QIntValidator* intValidator = new QIntValidator(this);
  intValidator->setBottom(1);
  m_Ui->xDimEdit->setValidator(intValidator);
  m_Ui->yDimEdit->setValidator(intValidator);
  m_Ui->zDimEdit->setValidator(intValidator);
  connect(m_Ui->xDimEdit, &QLineEdit::textChanged, this, &DataContainerGridSelectionWidget::updateDimensions);
  connect(m_Ui->yDimEdit, &QLineEdit::textChanged, this, &DataContainerGridSelectionWidget::updateDimensions);
  connect(m_Ui->zDimEdit, &QLineEdit::textChanged, this, &DataContainerGridSelectionWidget::updateDimensions);

  connect(m_Ui->upBtn, &QPushButton::clicked, this, &DataContainerGridSelectionWidget::increaseDepth);
  connect(m_Ui->downBtn, &QPushButton::clicked, this, &DataContainerGridSelectionWidget::decreaseDepth);

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), &AbstractFilter::dataArrayPathUpdated, this, &DataContainerGridSelectionWidget::updateDataArrayPath);

  if(getFilterParameter() == nullptr)
  {
    return;
  }
  m_Ui->propertyLabel->setText(getFilterParameter()->getHumanLabel());

  m_GridModel = new DataContainerGridModel(m_FilterParameter->getGetterCallback()());
  m_Ui->dataContainerGridTableView->setModel(m_GridModel);
  checkDepth();

  connect(m_GridModel, SIGNAL(modelChanged()), this, SIGNAL(parametersChanged()));

  DataContainerGrid grid;
  m_GridModel->getGridData(grid);
  QSignalBlocker xDimBlocker(m_Ui->xDimEdit);
  QSignalBlocker yDimBlocker(m_Ui->yDimEdit);
  QSignalBlocker zDimBlocker(m_Ui->zDimEdit);
  m_Ui->xDimEdit->setText(QString::number(grid.getDimensions()[0]));
  m_Ui->yDimEdit->setText(QString::number(grid.getDimensions()[1]));
  m_Ui->zDimEdit->setText(QString::number(grid.getDimensions()[2]));

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
    // setSelectedPath(getter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionWidget::updateDimensions()
{
  SizeVec3Type dims;
  dims[0] = m_Ui->xDimEdit->text().toInt();
  dims[1] = m_Ui->yDimEdit->text().toInt();
  dims[2] = m_Ui->zDimEdit->text().toInt();

  DataContainerGrid grid;
  m_GridModel->getGridData(grid);
  grid.resizeDimensions(dims);
  m_GridModel->setGridData(grid);

  checkDepth();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionWidget::checkDepth()
{
  int depth = m_GridModel->getCurrentDepth();
  const int maxDepth = m_GridModel->depthCount() - 1;

  if(depth >= maxDepth)
  {
    depth = maxDepth;
    m_GridModel->setCurrentDepth(depth);
  }

  m_Ui->zDepthLabel->setText(QString::number(depth + 1));

  bool isMaxDepth = (depth >= maxDepth);
  m_Ui->upBtn->setEnabled(!isMaxDepth);

  bool isMinDepth = (depth <= 0);
  m_Ui->downBtn->setEnabled(!isMinDepth);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionWidget::increaseDepth()
{
  int depth = m_GridModel->getCurrentDepth() + 1;
  m_GridModel->setCurrentDepth(depth);

  checkDepth();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionWidget::decreaseDepth()
{
  int depth = m_GridModel->getCurrentDepth() - 1;
  m_GridModel->setCurrentDepth(depth);

  checkDepth();
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
  model->getGridData(dcg);

  DataContainerGridSelectionFilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
  if(setter)
  {
    setter(dcg);
  }
}
