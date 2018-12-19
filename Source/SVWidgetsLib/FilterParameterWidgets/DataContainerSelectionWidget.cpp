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

#include "DataContainerSelectionWidget.h"

#include <QtCore/QList>
#include <QtCore/QMetaProperty>
#include <QtCore/QSignalMapper>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QMenu>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetUtils.hpp"
#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerSelectionWidget::DataContainerSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<DataContainerSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "DataContainerSelectionWidget can ONLY be used with a DataContainerSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerSelectionWidget::DataContainerSelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerSelectionWidget::~DataContainerSelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::setupGui()
{
  if(getFilter() == nullptr)
  {
    return;
  }

  m_SelectedDataContainerPath->setDataContainerRequirements(m_FilterParameter->getRequirements());
  m_SelectedDataContainerPath->setFilter(getFilter());

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)),
    this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  connect(this, SIGNAL(filterPathInput(DataArrayPath)), m_SelectedDataContainerPath, SLOT(checkDragPath(DataArrayPath)));
  connect(this, SIGNAL(endViewPathRequirements()), m_SelectedDataContainerPath, SLOT(clearPathFiltering()));
  connect(this, SIGNAL(endDataStructureRequirements()), m_SelectedDataContainerPath, SLOT(endExternalFiltering()));
  connect(this, SIGNAL(applyPathToFilteringParameter(DataArrayPath)), m_SelectedDataContainerPath, SLOT(setFilteredDataArrayPath(DataArrayPath)));

  connect(m_SelectedDataContainerPath, SIGNAL(viewPathsMatchingReqs(DataContainerSelectionFilterParameter::RequirementType)), this, SIGNAL(viewPathsMatchingReqs(DataContainerSelectionFilterParameter::RequirementType)));
  connect(m_SelectedDataContainerPath, SIGNAL(endViewPaths()), this, SIGNAL(endViewPaths()));
  connect(m_SelectedDataContainerPath, SIGNAL(pathChanged()), this, SIGNAL(parametersChanged()));
  connect(m_SelectedDataContainerPath, SIGNAL(filterPath(DataArrayPath)), this, SIGNAL(filterPath(DataArrayPath)));

  connect(m_SelectedDataContainerPath, SIGNAL(dataArrayPathSelectionLocked(QToolButton*)), this, SIGNAL(dataArrayPathSelectionLocked(QToolButton*)));
  connect(this, SIGNAL(lockDataArrayPathSelection(QToolButton*)), m_SelectedDataContainerPath, SLOT(selectionWidgetLocked(QToolButton*)));
  connect(m_SelectedDataContainerPath, SIGNAL(dataArrayPathSelectionUnlocked(QToolButton*)), this, SIGNAL(dataArrayPathSelectionUnlocked(QToolButton*)));
  connect(this, SIGNAL(unlockDataArrayPathSelection(QToolButton*)), m_SelectedDataContainerPath, SLOT(selectionWidgetUnlocked(QToolButton*)));

  if(getFilterParameter() == nullptr)
  {
    return;
  }
  label->setText(getFilterParameter()->getHumanLabel());

  m_SelectedDataContainerPath->setStyleSheet(SVStyle::Instance()->QToolSelectionButtonStyle(false));

  QString dcName = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<QString>();
  m_SelectedDataContainerPath->setText(dcName);
  m_SelectedDataContainerPath->setPropertyName(getFilterParameter()->getHumanLabel());


  changeStyleSheet(Style::FS_STANDARD_STYLE);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
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
bool DataContainerSelectionWidget::eventFilter(QObject* obj, QEvent* event)
{
  if(event->type() == QEvent::Show && obj == m_SelectedDataContainerPath->menu())
  {
    QPoint pos = adjustedMenuPosition(m_SelectedDataContainerPath);
    m_SelectedDataContainerPath->menu()->move(pos);
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath)
{
  if(propertyName.compare(PROPERTY_NAME_AS_CHAR) == 0)
  {
    QVariant var = getFilter()->property(PROPERTY_NAME_AS_CHAR);
    QString updatedPath = var.value<QString>();

    blockSignals(true);
    setSelectedPath(updatedPath);
    blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::dataContainerSelected(QString path)
{
  setSelectedPath(path);

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::setSelectedPath(QString path)
{
  DataArrayPath dcPath = DataArrayPath::Deserialize(path, Detail::Delimiter);
  setSelectedPath(dcPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::setSelectedPath(DataArrayPath dcPath)
{
  if (dcPath.isEmpty()) { return; }

  m_SelectedDataContainerPath->setDataArrayPath(dcPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }
  if(m_DidCausePreflight)
  {
    // std::cout << "***  DataContainerSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  m_SelectedDataContainerPath->beforePreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::afterPreflight()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  m_SelectedDataContainerPath->afterPreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Generate the path to the AttributeArray
  // DataArrayPath path(dataContainerList->currentText(), attributeMatrixList->currentText(), attributeArrayList->currentText());
  QVariant var(m_SelectedDataContainerPath->text());
  // var.setValue(path);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(!ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}
