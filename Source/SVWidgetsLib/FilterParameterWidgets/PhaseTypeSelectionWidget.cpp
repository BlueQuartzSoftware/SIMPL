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

#include "PhaseTypeSelectionWidget.h"

#include <QtCore/QList>
#include <QtCore/QSignalMapper>

#include <QtWidgets/QComboBox>

#include "SIMPLib/Common/PhaseType.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/FilterParameters/PhaseTypeSelectionFilterParameter.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::PhaseTypeSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<PhaseTypeSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "PhaseTypeSelectionWidget can ONLY be used with a PhaseTypeSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::PhaseTypeSelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::~PhaseTypeSelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::setupGui()
{
  if(getFilter() == nullptr)
  {
    return;
  }

  label->setText(m_FilterParameter->getHumanLabel());

  // Set empty AttributeMatrix requirement type
  m_SelectedAttributeMatrixPath->setAttrMatrixRequirements(AttributeMatrixSelectionFilterParameter::RequirementType());
  m_SelectedAttributeMatrixPath->setFilter(getFilter());

  // m_MenuMapper = new QSignalMapper(this);
  // connect(m_MenuMapper, SIGNAL(mapped(QString)), this, SLOT(attributeMatrixSelected(QString)));

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &PhaseTypeSelectionWidget::beforePreflight);

  // Catch when the filter is finished running the preflight
  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &PhaseTypeSelectionWidget::afterPreflight);

  // Catch when the filter wants its values updated
  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &PhaseTypeSelectionWidget::filterNeedsInputParameters);

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)), this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  connect(this, SIGNAL(filterPathInput(DataArrayPath)), m_SelectedAttributeMatrixPath, SLOT(checkDragPath(DataArrayPath)));
  connect(this, SIGNAL(endViewPathRequirements()), m_SelectedAttributeMatrixPath, SLOT(clearPathFiltering()));
  connect(this, SIGNAL(endDataStructureRequirements()), m_SelectedAttributeMatrixPath, SLOT(endExternalFiltering()));
  connect(this, SIGNAL(applyPathToFilteringParameter(DataArrayPath)), m_SelectedAttributeMatrixPath, SLOT(setFilteredDataArrayPath(DataArrayPath)));

  connect(m_SelectedAttributeMatrixPath, SIGNAL(viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType)), this,
          SIGNAL(viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType)));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(endViewPaths()), this, SIGNAL(endViewPaths()));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(pathChanged()), this, SLOT(attributeMatrixUpdated()));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(filterPath(DataArrayPath)), this, SIGNAL(filterPath(DataArrayPath)));

  connect(m_SelectedAttributeMatrixPath, SIGNAL(dataArrayPathSelectionLocked(QToolButton*)), this, SIGNAL(dataArrayPathSelectionLocked(QToolButton*)));
  connect(this, SIGNAL(lockDataArrayPathSelection(QToolButton*)), m_SelectedAttributeMatrixPath, SLOT(selectionWidgetLocked(QToolButton*)));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(dataArrayPathSelectionUnlocked(QToolButton*)), this, SIGNAL(dataArrayPathSelectionUnlocked(QToolButton*)));
  connect(this, SIGNAL(unlockDataArrayPathSelection(QToolButton*)), m_SelectedAttributeMatrixPath, SLOT(selectionWidgetUnlocked(QToolButton*)));

  QString pathProp = m_FilterParameter->getAttributeMatrixPathProperty();
  DataArrayPath defaultPath = getFilter()->property(qPrintable(pathProp)).value<DataArrayPath>();
  m_SelectedAttributeMatrixPath->setText(defaultPath.serialize(Detail::Delimiter));
  m_SelectedAttributeMatrixPath->setPropertyName(getFilterParameter()->getHumanLabel());

  updatePhaseComboBoxes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhaseTypeSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
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
void PhaseTypeSelectionWidget::attributeMatrixSelected(QString path)
{
  setSelectedPath(path);

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PhaseTypeSelectionWidget::eventFilter(QObject* obj, QEvent* event)
{
  if(event->type() == QEvent::Show && obj == m_SelectedAttributeMatrixPath->menu())
  {
    QPoint pos = adjustedMenuPosition(m_SelectedAttributeMatrixPath);
    m_SelectedAttributeMatrixPath->menu()->move(pos);
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::updateDataArrayPath(QString propertyName, const DataArrayPath::RenameType& renamePath)
{
  if(propertyName.compare(PROPERTY_NAME_AS_CHAR) == 0)
  {
    QVariant var = getFilter()->property(PROPERTY_NAME_AS_CHAR);
    DataArrayPath updatedPath = var.value<DataArrayPath>();

    blockSignals(true);
    setSelectedPath(updatedPath);
    blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::attributeMatrixUpdated()
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::setSelectedPath(QString path)
{
  DataArrayPath amPath = DataArrayPath::Deserialize(path, Detail::Delimiter);
  setSelectedPath(amPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::setSelectedPath(DataArrayPath amPath)
{
  if(amPath.isEmpty())
  {
    return;
  }

  m_SelectedAttributeMatrixPath->setText("");
  m_SelectedAttributeMatrixPath->setToolTip("");

  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  if(dca->doesAttributeMatrixExist(amPath))
  {
    AttributeMatrix::Pointer am = dca->getAttributeMatrix(amPath);
    QString html = am->getInfoString(SIMPL::HtmlFormat);
    m_SelectedAttributeMatrixPath->setToolTip(html);
    m_SelectedAttributeMatrixPath->setText(amPath.serialize(Detail::Delimiter));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::updatePhaseComboBoxes()
{
  bool ok = false;
  // setup the list of choices for the widget
  QString countProp = m_FilterParameter->getPhaseTypeCountProperty();
  int phaseCount = getFilter()->property(countProp.toLatin1().constData()).toInt(&ok);
  QString phaseDataProp = m_FilterParameter->getPhaseTypeDataProperty();

  PhaseType::Types dataFromFilter = getFilter()->property(phaseDataProp.toLatin1().constData()).value<PhaseType::Types>();
  if(phaseCount < 0 && dataFromFilter.size() < 10) // there was an issue getting the phase Count from the Filter.
  {
    phaseCount = dataFromFilter.size(); // So lets just use the count from the actual phase data
  }

  // Get our list of predefined enumeration values
  QVector<PhaseType::Type> phaseTypeEnums;
  PhaseType::getPhaseTypeEnums(phaseTypeEnums);

  phaseListWidget->clear();
  // Get our list of Phase Type Strings
  QStringList phaseListChoices = m_FilterParameter->getPhaseListChoices();

  for(int i = 1; i < phaseCount; i++)
  {
    QComboBox* cb = new QComboBox(nullptr);
    for(int s = 0; s < phaseListChoices.size(); ++s)
    {
      cb->addItem((phaseListChoices[s]), static_cast<PhaseType::EnumType>(phaseTypeEnums[s]));
      cb->setItemData(static_cast<int>(s), static_cast<PhaseType::EnumType>(phaseTypeEnums[s]), Qt::UserRole);
    }

    QListWidgetItem* item = new QListWidgetItem(phaseListWidget);
    phaseListWidget->addItem(item);
    phaseListWidget->setItemWidget(item, cb);

    if(i < dataFromFilter.size())
    {
      cb->setCurrentIndex(static_cast<int>(dataFromFilter[i]));
    }
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(phaseTypeComboBoxChanged(int)));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::phaseTypeComboBoxChanged(int index)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::resetPhaseComboBoxes()
{
  int count = phaseListWidget->count();

  for(int i = 0; i < count; ++i)
  {
    QComboBox* cb = qobject_cast<QComboBox*>(phaseListWidget->itemWidget(phaseListWidget->item(i)));
    if(cb != nullptr)
    {
      cb->setCurrentIndex(-1);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }
  if(m_DidCausePreflight)
  {
    return;
  }

  m_SelectedAttributeMatrixPath->beforePreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::afterPreflight()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  updatePhaseComboBoxes();

  m_SelectedAttributeMatrixPath->afterPreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  int count = phaseListWidget->count();
  QVariant var;

  PhaseType::Types phaseTypes(count + 1, PhaseType::Type::Unknown);
  bool ok = false;
  phaseTypes[0] = PhaseType::Type::Unknown;
  for(int i = 0; i < count; ++i)
  {
    QComboBox* cb = qobject_cast<QComboBox*>(phaseListWidget->itemWidget(phaseListWidget->item(i)));
    phaseTypes[i + 1] = static_cast<PhaseType::Type>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
  }

  Q_UNUSED(filter)
  PhaseTypeSelectionFilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
  if(setter)
  {
    setter(phaseTypes);
  }
  else
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }

  DataArrayPath path = m_SelectedAttributeMatrixPath->getDataArrayPath();
  var.setValue(path);
  ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(m_FilterParameter->getAttributeMatrixPathProperty().toLatin1().constData(), var);
  if(!ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}
