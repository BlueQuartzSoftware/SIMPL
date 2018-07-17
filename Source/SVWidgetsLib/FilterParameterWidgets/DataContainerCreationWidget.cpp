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

#include "DataContainerCreationWidget.h"

#include <QtCore/QMetaProperty>

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include <QtCore/QPropertyAnimation>
#include <QtCore/QSequentialAnimationGroup>

#include "FilterParameterWidgetUtils.hpp"
#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerCreationWidget::DataContainerCreationWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<DataContainerCreationFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "DataContainerCreationWidget can ONLY be used with a DataContainerCreationFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerCreationWidget::~DataContainerCreationWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerCreationWidget::setupGui()
{
  blockSignals(true);
  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());

    QString str = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
    stringEdit->setText(str, true);
  }
  blockSignals(false);

  stringEdit->hideButtons();

  // Do not allow the user to put a forward slash into the dataContainerName line edit
  stringEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[^/]*"), this));

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its dataContainerNames updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)),
    this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  connect(stringEdit, SIGNAL(valueChanged(const QString&)), this, SIGNAL(parametersChanged()));
  changeStyleSheet(Style::FS_STANDARD_STYLE);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerCreationWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerCreationWidget::afterPreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerCreationWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, stringEdit->getText());
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerCreationWidget::updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath)
{
  if(propertyName.compare(PROPERTY_NAME_AS_CHAR) == 0)
  {
    QVariant var = getFilter()->property(PROPERTY_NAME_AS_CHAR);
    DataArrayPath updatedPath = var.value<DataArrayPath>();
    QString dcName = updatedPath.getDataContainerName();
    updatedPath.setDataContainerName("");

    blockSignals(true);
    stringEdit->setText(dcName);
    blockSignals(false);
  }
}
