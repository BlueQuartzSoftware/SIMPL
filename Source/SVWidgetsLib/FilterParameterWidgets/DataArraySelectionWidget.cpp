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

#include "DataArraySelectionWidget.h"


#include <QtCore/QSignalMapper>
#include <QtCore/QMetaProperty>
#include <QtCore/QList>
#include <QtGui/QStandardItemModel>
#include <QtGui/QPainter>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>


#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"


#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/DataContainerArrayWidget.h"

#include "FilterParameterWidgetsDialogs.h"
#include "FilterParameterWidgetUtils.hpp"

// Include the MOC generated file for this class
#include "moc_DataArraySelectionWidget.cpp"
namespace Detail {
  const QString Delimiter(" / ");
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArraySelectionWidget::DataArraySelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<DataArraySelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "DataArraySelectionWidget can ONLY be used with a DataArraySelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArraySelectionWidget::DataArraySelectionWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArraySelectionWidget::~DataArraySelectionWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  //qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " DataArraySelectionWidget::initializeWidget";
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::setupGui()
{
  // Sanity Check the filter and the filter parameter
  if(getFilter() == NULL)
  {
    return;
  }
  if (getFilterParameter() == NULL)
  {
    return;
  }

  // Generate the text for the QLabel
  label->setText(getFilterParameter()->getHumanLabel() );

  // Get the default path from the Filter instance to cache
  m_DefaultPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();

#if DASW_NEW_GUI
  m_MenuMapper = new QSignalMapper(this);
  connect(m_MenuMapper, SIGNAL(mapped(QString)),
            this, SLOT(dataArraySelected(QString)));

  delete dataContainerCombo;
  dataContainerCombo = nullptr;
  delete attributeMatrixCombo;
  attributeMatrixCombo = nullptr;
  delete attributeArrayCombo;
  attributeArrayCombo = nullptr;

  dataArraySelected(m_DefaultPath.serialize(Detail::Delimiter));
  createSelectionMenu();

#else

  delete m_SelectedDataArrayPath;
  m_SelectedDataArrayPath = nullptr;
//  delete m_SelectedDataArrayPath;
//  m_SelectedDataArrayPath = nullptr;

  // Block Signals from the ComboBoxes while we clear them
  dataContainerCombo->blockSignals(true);
  attributeMatrixCombo->blockSignals(true);
  attributeArrayCombo->blockSignals(true);

  dataContainerCombo->clear();
  attributeMatrixCombo->clear();
  attributeArrayCombo->clear();

  dataContainerCombo->blockSignals(false);
  attributeMatrixCombo->blockSignals(false);
  attributeArrayCombo->blockSignals(false);

  populateComboBoxes();

#endif
  // Lastly, hook up the filter's signals and slots to our own signals and slots
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

}

#if DASW_NEW_GUI

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::createSelectionMenu()
{

  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }


  // Get the menu and clear it out
  QMenu* menu = m_SelectedDataArrayPath->menu();
  if(!menu)
  {
    menu = new QMenu();
    m_SelectedDataArrayPath->setMenu(menu);
  }
  if(menu) {
    menu->clear();
  }

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  QList<DataContainer::Pointer> containers = dca->getDataContainers();
  QVector<QString> daTypes = m_FilterParameter->getDefaultAttributeArrayTypes();
  QVector< QVector<size_t> > cDims = m_FilterParameter->getDefaultComponentDimensions();
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

      QMenu* amMenu = new QMenu(amName);
      dcMenu->addMenu(amMenu);

      if (NULL != am.get() && amTypes.isEmpty() == false && amTypes.contains(am->getType()) == false)
      {
        amMenu->setDisabled(true);
      }


      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the menus
      QList<QString> attrArrayNames = am->getAttributeArrayNames();
      QListIterator<QString> dataArraysIter(attrArrayNames);
      while(dataArraysIter.hasNext() )
      {
        QString attrArrayName = dataArraysIter.next();
        IDataArray::Pointer da = am->getAttributeArray(attrArrayName);
        QAction* action = new QAction(attrArrayName, amMenu);
        DataArrayPath daPath(dc->getName(), amName, attrArrayName);
        QString path = daPath.serialize(Detail::Delimiter);
        action->setData(path);

        connect(action, SIGNAL(triggered(bool)), m_MenuMapper, SLOT(map()));
        m_MenuMapper->setMapping(action, path);
        amMenu->addAction(action);

        bool daIsNotNull = (nullptr != da.get()) ? true : false;
        bool daValidType = (daTypes.isEmpty() == false && daTypes.contains(da->getTypeAsString()) == false) ? true : false;
        bool daValidDims = (cDims.isEmpty() == false && cDims.contains(da->getComponentDimensions()) == false) ? true : false;

        if (daIsNotNull && (daValidType || daValidDims) )
        {
          action->setDisabled(true);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::dataArraySelected(QString path)
{
  m_SelectedDataArrayPath->setText(path);

  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }


  IDataArray::Pointer attrArray = dca->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(getFilter(), DataArrayPath::Deserialize(path, Detail::Delimiter));
  if(nullptr != attrArray.get()) {
    QString html = attrArray->getInfoString(SIMPL::HtmlFormat);
    m_SelectedDataArrayPath->setToolTip(html);
  }

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

#else
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::populateComboBoxes()
{
  //qDebug() << "-----------------------------------------------";
  //qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " DataArraySelectionWidget::populateComboBoxes()";
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  //qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel();
  // Grab what is currently selected
  QString curDcName = dataContainerCombo->currentText();
  QString curAmName = attributeMatrixCombo->currentText();
  QString curDaName = attributeArrayCombo->currentText();
  //qDebug() << "Current ComboBox Value: " << curDcName << "::" << curAmName << "::" << curDaName;

  // Check to see if we have any DataContainers to actually populate drop downs with.
  if(dca->getDataContainers().size() == 0)
  {
    dataContainerCombo->clear();
    attributeMatrixCombo->clear();
    attributeArrayCombo->clear();
    return;
  }
  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Populate the DataContainer ComboBox
  FilterPararameterWidgetUtils::PopulateDataContainerComboBox<DataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, m_DcaProxy);

  // Get what is in the filter
  DataArrayPath selectedPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();

  // Split the path up to make sure we have a valid path separated by the "|" character
  QString filtDcName = selectedPath.getDataContainerName();
  QString filtAmName = selectedPath.getAttributeMatrixName();
  QString filtDaName = selectedPath.getDataArrayName();

  QString dcName;
  QString amName;
  QString daName;

  // If EVERYTHING is empty, then try the default value
  if(filtDcName.isEmpty() && filtAmName.isEmpty() && filtDaName.isEmpty()
      && curDcName.isEmpty() && curAmName.isEmpty() && curDaName.isEmpty() )
  {
    DataArrayPath daPath = getFilterParameter()->getDefaultValue().value<DataArrayPath>();
    dcName = daPath.getDataContainerName();
    amName = daPath.getAttributeMatrixName();
    daName = daPath.getDataArrayName();
  }
  else
  {
    // Now to figure out which one of these to use. If this is the first time through then what we picked up from the
    // gui will be empty strings because nothing is there. If there is something in the filter then we should use that.
    // If there is something in both of them and they are NOT equal then we have a problem. Use the flag m_DidCausePreflight
    // to determine if the change from the GUI should over ride the filter or vice versa. there is a potential that in future
    // versions that something else is driving SIMPLView and pushing the changes to the filter and we need to reflect those
    // changes in the GUI, like a testing script?

    dcName = checkStringValues(curDcName, filtDcName);
    if( !dca->doesDataContainerExist(dcName) ) { dcName = ""; }
    amName = checkStringValues(curAmName, filtAmName);
    if ( !dca->doesAttributeMatrixExist(DataArrayPath(dcName, amName, "") ) ) { amName = ""; }
    daName = checkStringValues(curDaName, filtDaName);
    if ( !dca->doesAttributeArrayExist(DataArrayPath(dcName, amName, daName) )) { daName = ""; }
  }

  bool didBlock = false;

  if (!dataContainerCombo->signalsBlocked()) { didBlock = true; }
  dataContainerCombo->blockSignals(true);
  int dcIndex = dataContainerCombo->findText(dcName);
  dataContainerCombo->setCurrentIndex(dcIndex);

  FilterPararameterWidgetUtils::PopulateAttributeMatrixComboBox<DataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, m_DcaProxy);

  if(didBlock) { dataContainerCombo->blockSignals(false); didBlock = false; }
  if(!attributeMatrixCombo->signalsBlocked()) { didBlock = true; }
  attributeMatrixCombo->blockSignals(true);

  int amIndex = -1;
  if (dcIndex < 0)
  {
    attributeMatrixCombo->setCurrentIndex(-1);
    attributeArrayCombo->setCurrentIndex(-1);
  }
  else
  {
    amIndex = attributeMatrixCombo->findText(amName);
    attributeMatrixCombo->setCurrentIndex(amIndex);
    FilterPararameterWidgetUtils::PopulateAttributeArrayComboBox<DataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, attributeArrayCombo, m_DcaProxy);
  }

  if(didBlock) { attributeMatrixCombo->blockSignals(false); didBlock = false; }

  if(!attributeArrayCombo->signalsBlocked()) { didBlock = true; }
  attributeArrayCombo->blockSignals(true);

  if (amIndex < 0)
  {
    attributeArrayCombo->setCurrentIndex(-1);
  }
  else
  {
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(attributeArrayCombo->model());
    if (NULL != model)
    {
      int daIndex = attributeArrayCombo->findText(daName);
      // The DataArray Name was empty, lets instantiate the filter and get the default value and try that
      if (daIndex < 0)
      {
        QVariant var = getFilterParameter()->getDefaultValue();
        DataArrayPath path = var.value<DataArrayPath>();
        daName = path.getDataArrayName(); // Pick up the DataArray Name from a Default instantiation of the filter
        daIndex = attributeArrayCombo->findText(daName);
      }

      QStandardItem* item = model->item(daIndex);
      if (NULL != item)
      {
        if (item->isEnabled())
        {
          attributeArrayCombo->setCurrentIndex(daIndex); // we set the selection but we are NOT triggering anything so we should
        }
      }
    }
  }

  if(didBlock) { attributeArrayCombo->blockSignals(false); didBlock = false; }// not be triggering an infinte recursion of preflights


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::on_dataContainerCombo_currentIndexChanged(int index)
{
  FilterPararameterWidgetUtils::PopulateAttributeMatrixComboBox<DataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, m_DcaProxy);

  // Do not select an attribute matrix from the list
  if (attributeMatrixCombo->count() > 0)
  {
    attributeMatrixCombo->setCurrentIndex(-1);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::on_attributeMatrixCombo_currentIndexChanged(int index)
{
  FilterPararameterWidgetUtils::PopulateAttributeArrayComboBox<DataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, attributeArrayCombo, m_DcaProxy);

  // Do not select an attribute array from the list
  if (attributeArrayCombo->count() > 0)
  {
    attributeArrayCombo->setCurrentIndex(-1);
  }

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//blockSignals(false);
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::on_attributeArrayCombo_currentIndexChanged(int index)
{
  //qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " DataArraySelectionWidget::on_attributeArrayCombo_currentIndexChanged(int index)";
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArraySelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  ////qDebug() << "    checkStringValues(...)" << curDcName << "  " << filtDcName;
  if (curDcName.isEmpty() == true && filtDcName.isEmpty() == false)
  {
    return filtDcName;
  }
  else if (curDcName.isEmpty() == false && filtDcName.isEmpty() == true)
  {
    return curDcName;
  }
  else if (curDcName.isEmpty() == false && filtDcName.isEmpty() == false && m_DidCausePreflight == true)
  {
    return curDcName;
  }

  return filtDcName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::setSelectedPath(QString dcName, QString attrMatName, QString attrArrName)
{
  //dataContainerCombo->blockSignals(true);
  // Set the correct DataContainer
  int count = dataContainerCombo->count();
  for(int i = 0; i < count; i++)
  {
    if (dataContainerCombo->itemText(i).compare(dcName) == 0 )
    {
      dataContainerCombo->setCurrentIndex(i); // This will fire the currentItemChanged(...) signal
      break;
    }
  }

  // Set the correct AttributeMatrix
  count = attributeMatrixCombo->count();
  for(int i = 0; i < count; i++)
  {
    if (attributeMatrixCombo->itemText(i).compare(attrMatName) == 0 )
    {
      attributeMatrixCombo->setCurrentIndex(i); // This will fire the currentItemChanged(...) signal
      break;
    }
  }

  attributeArrayCombo->blockSignals(true);
  // Set the correct Data Array (if Possible). It may have changed since we started
  count = attributeArrayCombo->count();
  for(int i = 0; i < count; i++)
  {
    if (attributeArrayCombo->itemText(i).compare(attrArrName) == 0 )
    {
      attributeArrayCombo->setCurrentIndex(i);
      break;
    }
  }
  attributeArrayCombo->blockSignals(false);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::beforePreflight()
{
  if (NULL == getFilter()) { return; }
  if(m_DidCausePreflight == true)
  {
    // std::cout << "***  DataArraySelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

#if DASW_NEW_GUI
    createSelectionMenu();
#else
  dataContainerCombo->blockSignals(true);
  attributeMatrixCombo->blockSignals(true);
  attributeArrayCombo->blockSignals(true);

  populateComboBoxes();

  dataContainerCombo->blockSignals(false);
  attributeMatrixCombo->blockSignals(false);
  attributeArrayCombo->blockSignals(false);
#endif

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::afterPreflight()
{
#if DASW_NEW_GUI
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }


  if(dca->doesAttributeArrayExist(DataArrayPath::Deserialize(m_SelectedDataArrayPath->text(), Detail::Delimiter)))
  {
    IDataArray::Pointer attrArray = dca->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(getFilter(), DataArrayPath::Deserialize(m_SelectedDataArrayPath->text(), Detail::Delimiter));
    if(nullptr != attrArray.get()) {
      QString html = attrArray->getInfoString(SIMPL::HtmlFormat);
      m_SelectedDataArrayPath->setToolTip(html);
    }
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Generate the path to the AttributeArray
#if DASW_NEW_GUI
  DataArrayPath selectedPath = DataArrayPath::Deserialize(m_SelectedDataArrayPath->text(), Detail::Delimiter);
  QString dc = selectedPath.getDataContainerName();
  QString am = selectedPath.getAttributeMatrixName();
  QString da = selectedPath.getDataArrayName();
#else
  QString dc = dataContainerCombo->currentText();
  QString am = attributeMatrixCombo->currentText();
  QString da = attributeArrayCombo->currentText();
#endif
 // qDebug() << "++++++++++++++++++++++++++++++++++++++++++++";
 // qDebug() << getFilterParameter()->getHumanLabel() << ":" << dc << Detail::Delimiter << am << Detail::Delimiter << da << "   m_DidCausePreflight:" << (int)(m_DidCausePreflight) << " " << (int)(attributeArrayCombo->signalsBlocked());
 // qDebug() << "++++++++++++++++++++++++++++++++++++++++++++";
  DataArrayPath path(dc, am, da);
  QVariant var;
  var.setValue(path);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

}
