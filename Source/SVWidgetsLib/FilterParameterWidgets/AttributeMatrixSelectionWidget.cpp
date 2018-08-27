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

#include "AttributeMatrixSelectionWidget.h"

#include <QtCore/QList>
#include <QtCore/QMetaProperty>
#include <QtCore/QSignalMapper>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QMenu>

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetUtils.hpp"
#include "FilterParameterWidgetsDialogs.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionWidget::AttributeMatrixSelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionWidget::AttributeMatrixSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<AttributeMatrixSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "AttributeMatrixSelectionWidget can ONLY be used with an AttributeMatrixSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionWidget::~AttributeMatrixSelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::setupGui()
{
  if(getFilter() == nullptr)
  {
    return;
  }
  if(getFilterParameter() == nullptr)
  {
    return;
  }

  m_SelectedAttributeMatrixPath->setAttrMatrixRequirements(m_FilterParameter->getRequirements());
  m_SelectedAttributeMatrixPath->setFilter(getFilter());

  label->setText(getFilterParameter()->getHumanLabel());

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)), 
    this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  connect(this, SIGNAL(filterPathInput(DataArrayPath)), m_SelectedAttributeMatrixPath, SLOT(checkDragPath(DataArrayPath)));
  connect(this, SIGNAL(endViewPathRequirements()), m_SelectedAttributeMatrixPath, SLOT(clearPathFiltering()));
  connect(this, SIGNAL(endDataStructureRequirements()), m_SelectedAttributeMatrixPath, SLOT(endExternalFiltering()));
  connect(this, SIGNAL(applyPathToFilteringParameter(DataArrayPath)), m_SelectedAttributeMatrixPath, SLOT(setFilteredDataArrayPath(DataArrayPath)));

  connect(m_SelectedAttributeMatrixPath, SIGNAL(viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType)),
    this, SIGNAL(viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType)));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(endViewPaths()), this, SIGNAL(endViewPaths()));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(pathChanged()), this, SIGNAL(parametersChanged()));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(filterPath(DataArrayPath)), this, SIGNAL(filterPath(DataArrayPath)));

  connect(m_SelectedAttributeMatrixPath, SIGNAL(dataArrayPathSelectionLocked(QToolButton*)), this, SIGNAL(dataArrayPathSelectionLocked(QToolButton*)));
  connect(this, SIGNAL(lockDataArrayPathSelection(QToolButton*)), m_SelectedAttributeMatrixPath, SLOT(selectionWidgetLocked(QToolButton*)));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(dataArrayPathSelectionUnlocked(QToolButton*)), this, SIGNAL(dataArrayPathSelectionUnlocked(QToolButton*)));
  connect(this, SIGNAL(unlockDataArrayPathSelection(QToolButton*)), m_SelectedAttributeMatrixPath, SLOT(selectionWidgetUnlocked(QToolButton*)));

  DataArrayPath defaultPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();
  m_SelectedAttributeMatrixPath->setText(defaultPath.serialize(Detail::Delimiter));
  m_SelectedAttributeMatrixPath->setPropertyName(getFilterParameter()->getHumanLabel());

  changeStyleSheet(Style::FS_STANDARD_STYLE);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrixSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
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
bool AttributeMatrixSelectionWidget::eventFilter(QObject* obj, QEvent* event)
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
void AttributeMatrixSelectionWidget::updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath)
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
void AttributeMatrixSelectionWidget::attributeMatrixSelected(QString path)
{
  setSelectedPath(path);

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::setSelectedPath(QString path)
{
  DataArrayPath amPath = DataArrayPath::Deserialize(path, Detail::Delimiter);
  setSelectedPath(amPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::setSelectedPath(DataArrayPath amPath)
{
  if (amPath.isEmpty()) { return; }

  m_SelectedAttributeMatrixPath->setDataArrayPath(amPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }
  if(m_DidCausePreflight == true)
  {
    // std::cout << "***  AttributeMatrixSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  m_SelectedAttributeMatrixPath->beforePreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::afterPreflight()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (NULL == dca.get()) { return; }

  m_SelectedAttributeMatrixPath->afterPreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Geenerate the path to the AttributeArray
  DataArrayPath selectedPath = m_SelectedAttributeMatrixPath->getDataArrayPath();
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
