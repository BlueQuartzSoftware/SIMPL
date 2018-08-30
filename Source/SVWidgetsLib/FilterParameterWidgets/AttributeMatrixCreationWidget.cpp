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

#include "AttributeMatrixCreationWidget.h"

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
AttributeMatrixCreationWidget::AttributeMatrixCreationWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<AttributeMatrixCreationFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "AttributeMatrixCreationWidget can ONLY be used with a AttributeMatrixCreationFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixCreationWidget::AttributeMatrixCreationWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixCreationWidget::~AttributeMatrixCreationWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::setupGui()
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

  // Do not allow the user to put a forward slash into the attributeMatrixName line edit
  stringEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[^/]*"), this));

  DataContainerSelectionFilterParameter::RequirementType req;
  m_SelectedDataContainerPath->setDataContainerRequirements(req);
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

  connect(stringEdit, SIGNAL(valueChanged(const QString&)), this, SIGNAL(parametersChanged()));

  m_SelectedDataContainerPath->blockSignals(true);
  DataArrayPath amPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();
  m_SelectedDataContainerPath->setText(amPath.getDataContainerName());
  m_SelectedDataContainerPath->setPropertyName(getFilterParameter()->getHumanLabel());
  stringEdit->setText(amPath.getAttributeMatrixName(), true);
  m_SelectedDataContainerPath->blockSignals(false);

  changeStyleSheet(Style::FS_STANDARD_STYLE);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrixCreationWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  if(curDcName.isEmpty() == true && filtDcName.isEmpty() == false)
  {
    return filtDcName;
  }
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == true)
  {
    return curDcName;
  }
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == false && m_DidCausePreflight == true)
  {
    return curDcName;
  }

  return filtDcName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrixCreationWidget::eventFilter(QObject* obj, QEvent* event)
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
void AttributeMatrixCreationWidget::updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath)
{
  if(propertyName.compare(PROPERTY_NAME_AS_CHAR) == 0)
  {
    QVariant var = getFilter()->property(PROPERTY_NAME_AS_CHAR);
    DataArrayPath updatedPath = var.value<DataArrayPath>();
    QString amName = updatedPath.getAttributeMatrixName();
    updatedPath.setAttributeMatrixName("");

    blockSignals(true);
    setSelectedPath(updatedPath);
    stringEdit->setText(amName);
    blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::dataContainerSelected(QString path)
{
  setSelectedPath(path);

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::setSelectedPath(QString path)
{
  DataArrayPath dcPath = DataArrayPath::Deserialize(path, Detail::Delimiter);
  setSelectedPath(dcPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::setSelectedPath(DataArrayPath dcPath)
{
  if (dcPath.isEmpty()) { return; }

  m_SelectedDataContainerPath->setDataArrayPath(dcPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }
  if(m_DidCausePreflight == true)
  {
    // std::cout << "***  AttributeMatrixCreationWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  m_SelectedDataContainerPath->beforePreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::afterPreflight()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (NULL == dca.get()) { return; }

  m_SelectedDataContainerPath->afterPreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Generate the path to the AttributeArray
  DataArrayPath selectedPath = m_SelectedDataContainerPath->getDataArrayPath();
  selectedPath.setAttributeMatrixName(stringEdit->getText());
  QVariant var;
  var.setValue(selectedPath);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}
