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

#include "ComparisonSelectionWidget.h"

#include <QtCore/QSignalMapper>

#include <QtWidgets/QMenu>

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetUtils.hpp"
#include "FilterParameterWidgetsDialogs.h"

#include "ComparisonSelectionItemDelegate.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionWidget::ComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_ShowOperators(true)
, m_DidCausePreflight(false)
, m_ComparisonSelectionTableModel(nullptr)
{
  m_FilterParameter = dynamic_cast<ComparisonSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "ComparisonSelectionWidget can ONLY be used with a ComparisonSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionWidget::~ComparisonSelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInputs ComparisonSelectionWidget::getComparisonInputs()
{
  ComparisonInputs comps;
  if(m_ComparisonSelectionTableModel == nullptr)
  {
    return comps;
  }

  int filterCount = m_ComparisonSelectionTableModel->rowCount();
  QVector<QString> featureNames;
  QVector<float> featureValues;
  QVector<int> featureOperators;
  m_ComparisonSelectionTableModel->getTableData(featureNames, featureValues, featureOperators);

  DataArrayPath amPath = DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter);

  for(int i = 0; i < filterCount; ++i)
  {
    ComparisonInput_t comp;
    comp.dataContainerName = amPath.getDataContainerName();
    comp.attributeMatrixName = amPath.getAttributeMatrixName();
    comp.attributeArrayName = featureNames[i];
    comp.compOperator = featureOperators[i];
    comp.compValue = featureValues[i];
    comps.addInput(comp);
  }
  return comps;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::setupGui()
{
  if(getFilter() == nullptr)
  {
    return;
  }
  if(getFilterParameter() == nullptr)
  {
    return;
  }

  AttributeMatrixSelectionFilterParameter::RequirementType reqs;
  m_SelectedAttributeMatrixPath->setAttrMatrixRequirements(reqs);
  m_SelectedAttributeMatrixPath->setFilter(getFilter());

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)),
    this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  connect(this, SIGNAL(filterPathInput(DataArrayPath)), m_SelectedAttributeMatrixPath, SLOT(checkDragPath(DataArrayPath)));
  connect(this, SIGNAL(endViewPathRequirements()), m_SelectedAttributeMatrixPath, SLOT(clearPathFiltering()));
  connect(this, SIGNAL(endDataStructureRequirements()), m_SelectedAttributeMatrixPath, SLOT(endExternalFiltering()));
  connect(this, SIGNAL(applyPathToFilteringParameter(DataArrayPath)), m_SelectedAttributeMatrixPath, SLOT(setFilteredDataArrayPath(DataArrayPath)));

  connect(m_SelectedAttributeMatrixPath, SIGNAL(viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType)), this, SIGNAL(viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType)));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(endViewPaths()), this, SIGNAL(endViewPaths()));
  //connect(m_SelectedAttributeMatrixPath, SIGNAL(pathChanged()), this, SIGNAL(parametersChanged()));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(filterPath(DataArrayPath)), this, SIGNAL(filterPath(DataArrayPath)));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(pathChanged()), this, SLOT(attributeMatrixUpdated()));

  connect(m_SelectedAttributeMatrixPath, SIGNAL(dataArrayPathSelectionLocked(QToolButton*)), this, SIGNAL(dataArrayPathSelectionLocked(QToolButton*)));
  connect(this, SIGNAL(lockDataArrayPathSelection(QToolButton*)), m_SelectedAttributeMatrixPath, SLOT(selectionWidgetLocked(QToolButton*)));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(dataArrayPathSelectionUnlocked(QToolButton*)), this, SIGNAL(dataArrayPathSelectionUnlocked(QToolButton*)));
  connect(this, SIGNAL(unlockDataArrayPathSelection(QToolButton*)), m_SelectedAttributeMatrixPath, SLOT(selectionWidgetUnlocked(QToolButton*)));

  // Create the table model
  m_ComparisonSelectionTableModel = createComparisonModel();

  // Set the data into the TableModel
  ComparisonInputs comps = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<ComparisonInputs>();
  m_ComparisonSelectionTableModel->setTableData(comps);

  DataArrayPath defaultPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();
  m_SelectedAttributeMatrixPath->setText(defaultPath.serialize(Detail::Delimiter));
  QString matrixPropertyName = getFilterParameter()->getHumanLabel();
  matrixPropertyName = matrixPropertyName.replace("Arrays", "Matrix");
  matrixPropertyName = matrixPropertyName.replace("arrays", "matrix");
  m_SelectedAttributeMatrixPath->setPropertyName(matrixPropertyName);

  addComparison->setStyleSheet(SVStyle::Instance()->StyleSheetForButton(addComparison->objectName(), SVWidgets::Styles::PushButtonStyleSheet, SVWidgets::Styles::AddImagePath));
  removeComparison->setStyleSheet(SVStyle::Instance()->StyleSheetForButton(removeComparison->objectName(), SVWidgets::Styles::PushButtonStyleSheet, SVWidgets::Styles::DeleteImagePath));
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_conditionalCB_stateChanged(int state)
{
  bool boolProp = conditionalCB->isChecked();
  dataContainerList->setEnabled(boolProp);
  dataContainerList->setVisible(boolProp);
  attributeMatrixList->setEnabled(boolProp);
  attributeMatrixList->setVisible(boolProp);
  attributeArrayList->setEnabled(boolProp);
  attributeArrayList->setVisible(boolProp);

  label->setVisible(boolProp);
  linkLeft->setVisible(boolProp);
  linkRight->setVisible(boolProp);
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;

}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ComparisonSelectionWidget::generateAttributeArrayList(const QString& currentDCName, const QString& currentAttrMatName)
{
  //  std::cout << "ComparisonSelectionWidget::generateAttributeArrayList()" << std::endl;
  QStringList attributeArrayList;

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = m_DcaProxy.dataContainers.values();
  QListIterator<DataContainerProxy> containerIter(containers);
  while(containerIter.hasNext())
  {
    DataContainerProxy dc = containerIter.next();
    if(dc.name.compare(currentDCName) == 0)
    {
      // We found the proper Data Container, now populate the AttributeMatrix List
      QMap<QString, AttributeMatrixProxy> attrMats = dc.attributeMatricies;
      QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
      while(attrMatsIter.hasNext())
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        if(amName.compare(currentAttrMatName) == 0)
        {

          // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
          AttributeMatrixProxy amProxy = attrMatsIter.value();
          QMap<QString, DataArrayProxy> dataArrays = amProxy.dataArrays;
          QMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
          while(dataArraysIter.hasNext())
          {
            dataArraysIter.next();
            // DataArrayProxy daProxy = dataArraysIter.value();
            QString daName = dataArraysIter.key();
            attributeArrayList << daName;
          }
        }
      }
    }
  }

  return attributeArrayList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComparisonSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
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
void ComparisonSelectionWidget::tableDataWasChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  Q_UNUSED(topLeft)
  Q_UNUSED(bottomRight)
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::widgetChanged(const QString& text)
{
  Q_UNUSED(text)
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_addComparison_clicked()
{
  int rowCount = m_ComparisonSelectionTableModel->rowCount();
  if(!m_ComparisonSelectionTableModel->insertRow(rowCount))
  {
    return;
  }

  QModelIndex index = m_ComparisonSelectionTableModel->index(m_ComparisonSelectionTableModel->rowCount() - 1, 0);
  comparisonSelectionTableView->setCurrentIndex(index);
  comparisonSelectionTableView->resizeColumnsToContents();
  comparisonSelectionTableView->scrollToBottom();
  comparisonSelectionTableView->setFocus();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_removeComparison_clicked()
{
  QItemSelectionModel* selectionModel = comparisonSelectionTableView->selectionModel();

  if(!selectionModel->hasSelection())
  {
    return;
  }
  QModelIndex index = selectionModel->currentIndex();
  if(!index.isValid())
  {
    return;
  }
  m_ComparisonSelectionTableModel->removeRow(index.row(), index.parent());
  if(m_ComparisonSelectionTableModel->rowCount() > 0)
  {
    comparisonSelectionTableView->resizeColumnsToContents();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::setComparisons(QVector<ComparisonInput_t> comparisons)
{
  qint32 count = comparisons.size();

  QVector<QString> arrayNames(count);
  QVector<int> compOperators(count);
  QVector<float> compValues(count);
  // bool ok = false;
  for(int i = 0; i < count; ++i)
  {
    arrayNames[i] = (comparisons[i].attributeArrayName);
    compOperators[i] = comparisons[i].compOperator;
    compValues[i] = comparisons[i].compValue;
  }
  m_ComparisonSelectionTableModel->setTableData(arrayNames, compValues, compOperators);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QVariant var;
  ComparisonInputs comps = getComparisonInputs();
  var.setValue(comps);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }
  if(m_DidCausePreflight == true)
  {
    // std::cout << "***  ComparisonSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  if (m_SelectedAttributeMatrixPath->text().isEmpty())
  {
    populateButtonText();
  }

  m_SelectedAttributeMatrixPath->beforePreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::afterPreflight()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (NULL == dca.get()) { return; }

  // Update the selectable feature arrays
  DataArrayPath amPath = m_SelectedAttributeMatrixPath->getDataArrayPath();
  QStringList possibleArrays = generateAttributeArrayList(amPath.getDataContainerName(), amPath.getAttributeMatrixName());
  m_ComparisonSelectionTableModel->setPossibleFeatures(possibleArrays);

  ComparisonSelectionItemDelegate* aid = dynamic_cast<ComparisonSelectionItemDelegate*>(comparisonSelectionTableView->itemDelegate());
  if(aid)
  {
    aid->setFeatureList(possibleArrays);
  }
  else
  {
    QAbstractItemDelegate* delegate = m_ComparisonSelectionTableModel->getItemDelegate();
    comparisonSelectionTableView->setItemDelegate(delegate);
  }

  m_SelectedAttributeMatrixPath->afterPreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::populateButtonText()
{
  //  std::cout << "ComparisonSelectionWidget::populateComboBoxes()" << std::endl;

  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  // Check to see if we have any DataContainers to actually populate drop downs with.
  if(dca->getDataContainers().size() == 0)
  {
    return;
  }

  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Grab what is currently selected
  QString curDcName = "";
  QString curAmName = "";

  // Get what is in the filter
  ComparisonInputs comps = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<ComparisonInputs>();
  // Split the path up to make sure we have a valid path separated by the "|" character
  ComparisonInput_t comp;
  if(comps.size() > 0)
  {
    comp = comps[0]; // Get the first threshold value;
  }
  QString filtDcName = comp.dataContainerName;
  QString filtAmName = comp.attributeMatrixName;
  QString filtDaName = comp.attributeArrayName;

  QString dcName = checkStringValues(curDcName, filtDcName);
  QString amName = checkStringValues(curAmName, filtAmName);

  m_SelectedAttributeMatrixPath->setText(dcName + Detail::Delimiter + amName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonSelectionWidget::eventFilter(QObject* obj, QEvent* event)
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
void ComparisonSelectionWidget::attributeMatrixUpdated()
{
  DataArrayPath path = m_SelectedAttributeMatrixPath->getDataArrayPath();
  attributeMatrixSelected(path.serialize(Detail::Delimiter));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::attributeMatrixSelected(QString path)
{
  setSelectedPath(path);

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::setSelectedPath(QString path)
{
  DataArrayPath amPath = DataArrayPath::Deserialize(path, Detail::Delimiter);
  setSelectedPath(amPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::setSelectedPath(DataArrayPath amPath)
{
  if (amPath.isEmpty()) { return; }

  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  if(nullptr != m_ComparisonSelectionTableModel)
  {
    m_ComparisonSelectionTableModel = createComparisonModel();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionTableModel* ComparisonSelectionWidget::createComparisonModel()
{
  ComparisonSelectionTableModel* newModel = new ComparisonSelectionTableModel(m_ShowOperators);
  QAbstractItemModel* oldModel = comparisonSelectionTableView->model();
  if(nullptr != oldModel)
  {
    delete oldModel;
  }

  comparisonSelectionTableView->setModel(newModel);
  newModel->setNumberOfPhases(1);
  comparisonSelectionTableView->resizeColumnsToContents();

  // Now connect all the signals and slots
  connect(newModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex)), this, SLOT(tableDataWasChanged(const QModelIndex&, const QModelIndex&)));

  DataArrayPath amPath = DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter);
  if(amPath.isEmpty())
  {
    return newModel;
  }

  QStringList possibleArrays = generateAttributeArrayList(amPath.getDataContainerName(), amPath.getAttributeMatrixName());
  newModel->setPossibleFeatures(possibleArrays);

  // Set the ItemDelegate for the table.
  QAbstractItemDelegate* aid = newModel->getItemDelegate();
  comparisonSelectionTableView->setItemDelegate(aid);

  return newModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath)
{
  if(propertyName.compare(getFilterParameter()->getPropertyName()) == 0)
  {
    DataArrayPath oldPath;
    DataArrayPath newPath;
    std::tie(oldPath, newPath) = renamePath;

    QVariant var = getFilter()->property(PROPERTY_NAME_AS_CHAR);
    ComparisonInputs inputs = var.value<ComparisonInputs>();
    ComparisonInput_t input = inputs.getInput(0);
    DataArrayPath amPath(input.dataContainerName, input.attributeMatrixName, "");

    blockSignals(true);
    // Update the AttributeMatrix path
    {
      DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
      if(nullptr == dca.get())
      {
        return;
      }

      m_SelectedAttributeMatrixPath->setText(amPath.serialize(Detail::Delimiter));
    }

    // Update the DataArray choices
    {
      QAbstractItemModel* model = comparisonSelectionTableView->model();
      ComparisonSelectionTableModel* compModel = dynamic_cast<ComparisonSelectionTableModel*>(model);
      if(compModel)
      {
        compModel->blockSignals(true);
        compModel->updateFeatureName(oldPath.getDataArrayName(), newPath.getDataArrayName());
        compModel->blockSignals(false);
      }
    }
    blockSignals(false);
  }
}
