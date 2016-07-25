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

#include "FilterParameterWidgetsDialogs.h"
#include "FilterParameterWidgetUtils.hpp"

// Include the MOC generated file for this class
#include "moc_ComparisonSelectionWidget.cpp"

namespace Detail
{
  const QString Delimiter(" / ");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionWidget::ComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_ShowOperators(true),
  m_DidCausePreflight(false),
  m_ComparisonSelectionTableModel(NULL)
{
  m_FilterParameter = dynamic_cast<ComparisonSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "ComparisonSelectionWidget can ONLY be used with a ComparisonSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionWidget::~ComparisonSelectionWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInputs ComparisonSelectionWidget::getComparisonInputs()
{
  ComparisonInputs comps;
  if (m_ComparisonSelectionTableModel == NULL) { return comps; }

  int filterCount = m_ComparisonSelectionTableModel->rowCount();
  QVector<QString> featureNames;
  QVector<float> featureValues;
  QVector<int> featureOperators;
  m_ComparisonSelectionTableModel->getTableData(featureNames, featureValues, featureOperators);

  DataArrayPath amPath = DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter);


  for(int i = 0; i < filterCount; ++i)
  {
    ComparisonInput_t comp;
#if OLD_GUI
    comp.dataContainerName = dataContainerCombo->currentText();
    comp.attributeMatrixName = attributeMatrixCombo->currentText();
#else
    comp.dataContainerName = amPath.getDataContainerName();
    comp.attributeMatrixName = amPath.getAttributeMatrixName();
#endif
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
  if (getFilter() == NULL)
  {
    return;
  }
  if (getFilterParameter() == NULL)
  {
    return;
  }

  m_ComparisonSelectionTableModel = createComparisonModel();

#if OLD_GUI
  connect(dataContainerCombo, SIGNAL(currentIndexChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&)));

  connect(attributeMatrixCombo, SIGNAL(currentIndexChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&)));
#endif

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // Set the data into the TableModel
  ComparisonInputs comps = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<ComparisonInputs>();
  m_ComparisonSelectionTableModel->setTableData(comps);



  m_MenuMapper = new QSignalMapper(this);
  connect(m_MenuMapper, SIGNAL(mapped(QString)),
            this, SLOT(attributeMatrixSelected(QString)));

  createSelectionMenu();


#if 0
  // is the filter parameter tied to a boolean property of the Filter Instance, if it is then we need to make the check box visible
  if(getFilterParameter()->isConditional() == true)
  {
    bool boolProp = getFilter()->property(getFilterParameter()->getConditionalProperty().toLatin1().constData() ).toBool();
    conditionalCB->setChecked(boolProp);
    conditionalCB->setText(getFilterParameter()->getConditionalLabel());
    dataContainerList->setEnabled(boolProp);
    attributeMatrixList->setEnabled(boolProp);
    attributeArrayList->setEnabled(boolProp);
  }
  else
  {
    widgetLayout->removeWidget(conditionalCB);
    conditionalCB->deleteLater();
    widgetLayout->removeWidget(linkLeft);
    linkLeft->deleteLater();
    widgetLayout->removeWidget(linkRight);
    linkRight->deleteLater();
  }
#endif

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


#if OLD_GUI
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::populateComboBoxes()
{
//  std::cout << "ComparisonSelectionWidget::populateComboBoxes()" << std::endl;

  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  // Check to see if we have any DataContainers to actually populate drop downs with.
  if(dca->getDataContainers().size() == 0)
  {
    dataContainerCombo->clear();
    attributeMatrixCombo->clear();
    return;
  }

  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Populate the DataContainerArray Combo Box with all the DataContainers
  FilterParameterWidgetUtils::PopulateDataContainerComboBox<ComparisonSelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, m_DcaProxy);

  // Grab what is currently selected
  QString curDcName = dataContainerCombo->currentText();
  QString curAmName = attributeMatrixCombo->currentText();
  //  QString curDaName = attributeArrayList->currentText();

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
  //QStringList tokens = selectedPath.split(SIMPLView::PathSep);

  // Now to figure out which one of these to use. If this is the first time through then what we picked up from the
  // gui will be empty strings because nothing is there. If there is something in the filter then we should use that.
  // If there is something in both of them and they are NOT equal then we have a problem. Use the flag m_DidCausePreflight
  // to determine if the change from the GUI should over ride the filter or vice versa. there is a potential that in future
  // versions that something else is driving SIMPLView and pushing the changes to the filter and we need to reflect those
  // changes in the GUI, like a testing script?

  QString dcName = checkStringValues(curDcName, filtDcName);
  QString amName = checkStringValues(curAmName, filtAmName);
  // QString daName = checkStringValues(curDaName, filtDaName);

  bool didBlock = false;

  if (!dataContainerCombo->signalsBlocked()) { didBlock = true; }
  dataContainerCombo->blockSignals(true);
  int dcIndex = dataContainerCombo->findText(dcName);

  dataContainerCombo->setCurrentIndex(dcIndex);
  FilterParameterWidgetUtils::PopulateAttributeMatrixComboBox<ComparisonSelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, m_DcaProxy);

  if(didBlock) { dataContainerCombo->blockSignals(false); didBlock = false; }

  if(!attributeMatrixCombo->signalsBlocked()) { didBlock = true; }
  attributeMatrixCombo->blockSignals(true);

  if (dcIndex < 0)
  {
    attributeMatrixCombo->setCurrentIndex(-1);
    ComparisonInputs ci;
    m_ComparisonSelectionTableModel->setTableData(ci);
  }
  else
  {
    int amIndex = attributeMatrixCombo->findText(amName);

    // Set the selected index in the Attribute Matrix
    attributeMatrixCombo->setCurrentIndex(amIndex);

    if (amIndex < 0)
    {
      ComparisonInputs ci;
      m_ComparisonSelectionTableModel->setTableData(ci);
    }
    else
    {
      // Get the selected Data Container Name from the DataContainerList Widget
      QString currentDCName = dataContainerCombo->currentText();
      QString currentAttrMatName = attributeMatrixCombo->currentText();

      // Now based on that AttributeMatrix get a list the AttributeArrays
      QStringList possibleArrays = generateAttributeArrayList(currentDCName, currentAttrMatName);
      // Push that list into the Table Model
      m_ComparisonSelectionTableModel->setPossibleFeatures(possibleArrays);
      // Now that we have an updated list of names we need to Set the ItemDelegate for the table.
      QAbstractItemDelegate* aid = m_ComparisonSelectionTableModel->getItemDelegate();
      comparisonSelectionTableView->setItemDelegate(aid);

      // Now set the table data directly.
      m_ComparisonSelectionTableModel->setTableData(comps);
    }
  }

  if(didBlock) { attributeMatrixCombo->blockSignals(false); didBlock = false; }


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::populateAttributeMatrixList()
{
//  std::cout << "ComparisonSelectionWidget::populateAttributeMatrixList()" << std::endl;
  QString dcName = dataContainerCombo->currentText();

  // Clear the AttributeMatrix List
  attributeMatrixCombo->blockSignals(true);
  attributeMatrixCombo->clear();

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = m_DcaProxy.dataContainers.values();
  QListIterator<DataContainerProxy> containerIter(containers);
  while(containerIter.hasNext())
  {
    DataContainerProxy dc = containerIter.next();

    if(dc.name.compare(dcName) == 0 )
    {
      // We found the proper Data Container, now populate the AttributeMatrix List
      QMap<QString, AttributeMatrixProxy> attrMats = dc.attributeMatricies;
      QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
      while(attrMatsIter.hasNext() )
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        attributeMatrixCombo->addItem(amName);
      }
    }
  }

  attributeMatrixCombo->blockSignals(false);
}

#endif
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ComparisonSelectionWidget::generateAttributeArrayList(const QString& currentDCName, const QString &currentAttrMatName)
{
//  std::cout << "ComparisonSelectionWidget::generateAttributeArrayList()" << std::endl;
  QStringList attributeArrayList;


  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = m_DcaProxy.dataContainers.values();
  QListIterator<DataContainerProxy> containerIter(containers);
  while(containerIter.hasNext())
  {
    DataContainerProxy dc = containerIter.next();
    if(dc.name.compare(currentDCName) == 0 )
    {
      // We found the proper Data Container, now populate the AttributeMatrix List
      QMap<QString, AttributeMatrixProxy> attrMats = dc.attributeMatricies;
      QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
      while(attrMatsIter.hasNext() )
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        if(amName.compare(currentAttrMatName) == 0 )
        {

          // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
          AttributeMatrixProxy amProxy = attrMatsIter.value();
          QMap<QString, DataArrayProxy> dataArrays = amProxy.dataArrays;
          QMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
          while(dataArraysIter.hasNext() )
          {
            dataArraysIter.next();
            //DataArrayProxy daProxy = dataArraysIter.value();
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
  {return filtDcName;}
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == true)
  {return curDcName;}
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == false && m_DidCausePreflight == true)
  { return curDcName;}

  return filtDcName;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::tableDataWasChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::widgetChanged(const QString& text)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_addComparison_clicked()
{
  if (!m_ComparisonSelectionTableModel->insertRow(m_ComparisonSelectionTableModel->rowCount()))
  {
    qDebug() << "on_addComparison_clicked  Failed to add row";
    return;
  }

  QModelIndex index = m_ComparisonSelectionTableModel->index(m_ComparisonSelectionTableModel->rowCount() - 1, 0);
  comparisonSelectionTableView->setCurrentIndex(index);
  comparisonSelectionTableView->resizeColumnsToContents();
  comparisonSelectionTableView->scrollToBottom();
  comparisonSelectionTableView->setFocus();
//  m_DidCausePreflight = true;
//  emit parametersChanged();
//  m_DidCausePreflight = false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_removeComparison_clicked()
{
  QItemSelectionModel* selectionModel = comparisonSelectionTableView->selectionModel();

  if (!selectionModel->hasSelection()) { return; }
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) { return; }
  m_ComparisonSelectionTableModel->removeRow(index.row(), index.parent());
  if (m_ComparisonSelectionTableModel->rowCount() > 0)
  {
    comparisonSelectionTableView->resizeColumnsToContents();
  }
//  m_DidCausePreflight = true;
//  emit parametersChanged();
//  m_DidCausePreflight = false;
}

#if  OLD_GUI
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_dataContainerCombo_currentIndexChanged(int index)
{
  Q_UNUSED(index)
  populateAttributeMatrixList();

  // Do not select an attribute matrix from the list
  if (attributeMatrixCombo->count() > 0)
  {
    attributeMatrixCombo->setCurrentIndex(-1);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_attributeMatrixCombo_currentIndexChanged(int index)
{
  Q_UNUSED(index)
  // Get the selected Data Container Name from the DataContainerList Widget
  QString currentDCName = dataContainerCombo->currentText();
  QString currentAttrMatName = attributeMatrixCombo->currentText();

  QStringList possibleArrays = generateAttributeArrayList(currentDCName, currentAttrMatName);
  m_ComparisonSelectionTableModel->setPossibleFeatures(possibleArrays);
  // Set the ItemDelegate for the table.
  QAbstractItemDelegate* aid = m_ComparisonSelectionTableModel->getItemDelegate();
  comparisonSelectionTableView->setItemDelegate(aid);
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::setComparisons(QVector<ComparisonInput_t> comparisons)
{
  qint32 count = comparisons.size();

  QVector<QString> arrayNames(count);
  QVector<int>   compOperators(count);
  QVector<float> compValues(count);
  //bool ok = false;
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
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::beforePreflight()
{
  qDebug() << "ComparisonSelectionWidget::beforePreflight()";
  if (NULL == getFilter()) { return; }
  if(m_DidCausePreflight == true)
  {
    // std::cout << "***  ComparisonSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

//  dataContainerCombo->blockSignals(true);
//  attributeMatrixCombo->blockSignals(true);
//  m_ComparisonSelectionTableModel->blockSignals(true);
//  // Reset all the combo box widgets to have the default selection of the first index in the list
//  populateComboBoxes();
//  m_ComparisonSelectionTableModel->blockSignals(false);
//  dataContainerCombo->blockSignals(false);
//  attributeMatrixCombo->blockSignals(false);


  populateButtonText();
  createSelectionMenu();

  DataArrayPath amPath = DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter);
  if (amPath.isEmpty()) { return; }

  QStringList possibleArrays = generateAttributeArrayList(amPath.getDataContainerName(), amPath.getAttributeMatrixName());
  m_ComparisonSelectionTableModel->setPossibleFeatures(possibleArrays);
  // Set the ItemDelegate for the table.
  QAbstractItemDelegate* aid = m_ComparisonSelectionTableModel->getItemDelegate();
  comparisonSelectionTableView->setItemDelegate(aid);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::afterPreflight()
{
  // qDebug() << getFilter()->getNameOfClass() << " DataContainerArrayProxyWidget::afterPreflight()";
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
  if(NULL == dca.get()) { return; }

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
  if (event->type() == QEvent::Show && obj == m_SelectedAttributeMatrixPath->menu())
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
void ComparisonSelectionWidget::attributeMatrixSelected(QString path)
{
  qDebug() << "ComparisonSelectionWidget::attributeMatrixSelected(QString path)";
  setSelectedPath(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::setSelectedPath(QString path)
{
  DataArrayPath amPath = DataArrayPath::Deserialize(path, Detail::Delimiter);
  if (amPath.isEmpty()) { return; }

  m_SelectedAttributeMatrixPath->setText("");
  m_SelectedAttributeMatrixPath->setToolTip("");

  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  int err = 0;
  AttributeMatrix::Pointer attrMat = dca->getPrereqAttributeMatrixFromPath(getFilter(), amPath, err);
  if(nullptr != attrMat.get()) {
    QString html = attrMat->getInfoString(SIMPL::HtmlFormat);
    m_SelectedAttributeMatrixPath->setToolTip(html);
    m_SelectedAttributeMatrixPath->setText(path);
  }

  if(NULL != m_ComparisonSelectionTableModel)
  {
    delete m_ComparisonSelectionTableModel;
    m_ComparisonSelectionTableModel = createComparisonModel();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionTableModel* ComparisonSelectionWidget::createComparisonModel()
{
  qDebug() << "ComparisonSelectionWidget::createComparisonModel()";
  ComparisonSelectionTableModel* newModel = new ComparisonSelectionTableModel(m_ShowOperators);
  QAbstractItemModel* oldModel = comparisonSelectionTableView->model();
  if(NULL != oldModel) { delete oldModel; }

  comparisonSelectionTableView->setModel(newModel);
  newModel->setNumberOfPhases(1);
  comparisonSelectionTableView->resizeColumnsToContents();

  // Set the ItemDelegate for the table.
  QAbstractItemDelegate* aid = newModel->getItemDelegate();
  comparisonSelectionTableView->setItemDelegate(aid);

  // Now connect all the signals and slots
  connect(newModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex)),
          this, SLOT(tableDataWasChanged(const QModelIndex&, const QModelIndex&)));

  return newModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::createSelectionMenu()
{
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  // Get the menu and clear it out
  QMenu* menu = m_SelectedAttributeMatrixPath->menu();
  if(!menu)
  {
    menu = new QMenu();
    m_SelectedAttributeMatrixPath->setMenu(menu);
    menu->installEventFilter(this);
  }
  if(menu) {
    menu->clear();
  }

  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  QList<DataContainer::Pointer> containers = dca->getDataContainers();
  QVector<unsigned int> amTypes = m_FilterParameter->getDefaultAttributeMatrixTypes();
  QVector<unsigned int> geomTypes = m_FilterParameter->getDefaultGeometryTypes();



  QListIterator<DataContainer::Pointer> containerIter(containers);
  while(containerIter.hasNext())
  {
    DataContainer::Pointer dc = containerIter.next();

    IGeometry::Pointer geom = IGeometry::NullPointer();
    uint32_t geomType = 999;
    if (NULL != dc.get()) { geom = dc->getGeometry(); }
    if (NULL != geom.get()) { geomType = geom->getGeometryType(); }


    QMenu* dcMenu = new QMenu(dc->getName());
    dcMenu->setDisabled(false);
    menu->addMenu(dcMenu);
    if(geomTypes.isEmpty() == false && geomTypes.contains(geomType) == false )
    {
      dcMenu->setDisabled(true);
    }


    // We found the proper Data Container, now populate the AttributeMatrix List
    DataContainer::AttributeMatrixMap_t attrMats = dc->getAttributeMatrices();
    QMapIterator<QString, AttributeMatrix::Pointer> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext() )
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      AttributeMatrix::Pointer am = attrMatsIter.value();

      QAction* action = new QAction(amName, dcMenu);
      DataArrayPath daPath(dc->getName(), amName, "");
      QString path = daPath.serialize(Detail::Delimiter);
      action->setData(path);

      connect(action, SIGNAL(triggered(bool)), m_MenuMapper, SLOT(map()));
      m_MenuMapper->setMapping(action, path);
      dcMenu->addAction(action);

      bool amIsNotNull = (nullptr != am.get()) ? true : false;
      bool amValidType = (amTypes.isEmpty() == false && amTypes.contains(am->getType()) == false) ? true : false;

      if (amIsNotNull && amValidType)
      {
        action->setDisabled(true);
      }
    }
  }
}
