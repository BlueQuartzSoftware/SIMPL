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

#include "DataContainerArrayProxyWidget.h"

#include <assert.h>

#include <QtCore/QMetaProperty>

#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QFont>

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxyWidget::DataContainerArrayProxyWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<DataContainerArrayProxyFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "DataContainerArrayProxyWidget can ONLY be used with a DataContainerArrayProxyFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxyWidget::DataContainerArrayProxyWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxyWidget::~DataContainerArrayProxyWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = dynamic_cast<DataContainerArrayProxyFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* DataContainerArrayProxyWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::setupGui()
{

  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // setStyleSheet("QColumnView { text-decoration-color: red; }");

  connect(dataContainerList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));
  connect(attributeMatrixList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));
  connect(dataArrayList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));

  connect(dataContainerList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));
  connect(attributeMatrixList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));
  connect(dataArrayList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));

  connect(selectAllDataContainer, SIGNAL(clicked(bool)),
    this, SLOT(selectAllDataContainersClicked(bool)));
  connect(selectAllAttributeMatrix, SIGNAL(clicked(bool)),
    this, SLOT(selectAllAttributeMatricesClicked(bool)));
  connect(selectAllDataArray, SIGNAL(clicked(bool)),
    this, SLOT(selectAllDataArraysClicked(bool)));

  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());

    // If the filter is just being dragged into the pipeline then the filter is going to have an empty
    // Proxy object at which point nothing is going to be put into the lists. But if the Filter was
    // initialized from a pipeline file then it might actually have a proxy so we need to do something
    // with that proxy?
    m_DcaProxy = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataContainerArrayProxy>();
    // m_DcaProxy.print("DataContainerArrayProxyWidget::setupGui()");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::selectAllDataContainersClicked(bool checked)
{
  selectAllDataContainer->setTristate(false);

  m_DidCausePreflight = true;

  dataContainerList->blockSignals(true);
  checkAllItems(dataContainerList, selectAllDataContainer->checkState());
  applyDataContainerArrayProxy(m_DcaProxy);
  dataContainerList->blockSignals(false);

  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::selectAllAttributeMatricesClicked(bool checked)
{
  selectAllAttributeMatrix->setTristate(false);

  m_DidCausePreflight = true;

  attributeMatrixList->blockSignals(true);
  checkAllItems(attributeMatrixList, selectAllAttributeMatrix->checkState());
  applyDataContainerProxy();
  attributeMatrixList->blockSignals(false);

  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::selectAllDataArraysClicked(bool checked)
{
  selectAllDataArray->setTristate(false);

  m_DidCausePreflight = true;

  dataArrayList->blockSignals(true);
  checkAllItems(dataArrayList, selectAllDataArray->checkState());
  applyAttributeMatrixProxy();
  dataArrayList->blockSignals(false);

  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::itemChanged(QListWidgetItem* item)
{
  m_DidCausePreflight = true;

  item->listWidget()->blockSignals(true);
  updateProxyChecked(item);
  item->listWidget()->blockSignals(false);

  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::itemSelected(QListWidgetItem* item)
{
  QString name = item->text();

  if(item->listWidget() == dataContainerList)
  {
    selectDataContainer(name);
  }
  else if(item->listWidget() == attributeMatrixList)
  {
    selectAttributeMatrix(name);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::updateProxyChecked(QListWidgetItem* item, bool updateAll)
{
  QString name = item->text();
  QListWidget* widget = item->listWidget();

  if(item->text() == "Select All")
  {
    if(item->checkState() != Qt::PartiallyChecked)
    {
      checkAllItems(widget, item->checkState());
    }
  }
  else
  {
    if(dataContainerList == widget)
    {
      m_DcaProxy.dataContainers.find(name).value().flag = item->checkState();
    }
    else if(attributeMatrixList == widget)
    {
      getDataContainerProxy().attributeMatricies.find(name).value().flag = item->checkState();
    }
    else if(dataArrayList == widget)
    {
      getAttributeMatrixProxy().dataArrays.find(name).value().flag = item->checkState();
    }
  }

  Qt::CheckState state = shouldStrikeOutItem(item) ? Qt::Checked : Qt::Unchecked;
  toggleStrikeOutFont(item, state);

  if(updateAll)
  {
    updateSelectAllState(widget);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArrayProxyWidget::shouldStrikeOutItem(QListWidgetItem* item)
{
  bool dcChecked = false;
  bool amChecked = false;

  if(false == m_DcName.isEmpty())
  {
    dcChecked = (getDataContainerProxy().flag == Qt::Checked);
  }
  if(false == m_AmName.isEmpty())
  {
    amChecked = (getAttributeMatrixProxy().flag == Qt::Checked);
  }

  if(item->listWidget() == dataContainerList)
  {
    return item->checkState() == Qt::Checked;
  }
  else if(item->listWidget() == attributeMatrixList)
  {
    return dcChecked || item->checkState() == Qt::Checked;
  }
  else if(item->listWidget() == dataArrayList)
  {
    return dcChecked || amChecked || item->checkState() == Qt::Checked;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QListWidgetItem*> DataContainerArrayProxyWidget::getChildItems(QListWidgetItem* item, QList<QListWidgetItem*> otherItems)
{
  QListWidget* listWidget = item->listWidget();

  if(listWidget == dataContainerList)
  {
    for(int i = 0; attributeMatrixList->item(i) != nullptr; i++)
    {
      otherItems.append(getChildItems(attributeMatrixList->item(i), otherItems));
    }
  }
  else if(listWidget == attributeMatrixList)
  {
    for(int i = 0; dataArrayList->item(i) != nullptr; i++)
    {
      otherItems.append(getChildItems(dataArrayList->item(i), otherItems));
    }
  }

  return otherItems;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::toggleStrikeOutFont(QListWidgetItem* item, Qt::CheckState state)
{
  QFont font = item->font();

  QColor errorColor(255, 191, 193);
  QBrush errorBrush(errorColor);

  QColor defaultColor(Qt::white);
  QBrush defaultBrush(defaultColor);

  if(state == Qt::Checked)
  {
    item->setBackground(errorBrush);
  }
  else if(item->checkState() == false)
  {
    item->setBackground(defaultBrush);
  }
  item->setFont(font);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // qDebug() << "DataContainerArrayProxyWidget::filterNeedsInputParameters(AbstractFilter* filter)";

  QVariant var;
  var.setValue(m_DcaProxy);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }

  //  if(getFilterParameter()->isConditional() )
  //  {
  //    var.setValue(conditionalCB->isChecked());
  //    ok = filter->setProperty(getFilterParameter()->getConditionalProperty().toLatin1().constData(), var);
  //    if(false == ok)
  //    {
  //      FilterParameterWidgetsDialogs::ShowCouldNotSetConditionalFilterParameter(getFilter(), getFilterParameter());
  //    }
  //  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerArrayProxyWidget::getDataContainerName()
{
  QList<QListWidgetItem*> selectedItems = dataContainerList->selectedItems();

  if(selectedItems.size() != 1)
  {
    return "";
  }

  if(selectedItems[0]->text() != "Select All")
  {
    return selectedItems[0]->text();
  }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerArrayProxyWidget::getAttrMatrixName()
{
  QList<QListWidgetItem*> selectedItems = attributeMatrixList->selectedItems();

  if(selectedItems.size() != 1)
  {
    return "";
  }

  if(selectedItems[0]->text() != "Select All")
  {
    return selectedItems[0]->text();
  }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::selectDataContainer(QString name)
{
  if(m_DcaProxy.contains(name))
  {
    m_DcName = name;
  }
  else
  {
    m_DcName = "";
  }
  m_AmName = "";

  applyDataContainerProxy();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::selectAttributeMatrix(QString name)
{
  if(getDataContainerProxy().attributeMatricies.contains(name))
  {
    m_AmName = name;
  }
  else
  {
    m_AmName = "";
  }

  applyAttributeMatrixProxy();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerProxy& DataContainerArrayProxyWidget::getDataContainerProxy()
{
  return m_DcaProxy.dataContainers.find(m_DcName).value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy& DataContainerArrayProxyWidget::getAttributeMatrixProxy()
{
  return getDataContainerProxy().attributeMatricies.find(m_AmName).value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QListWidgetItem*> DataContainerArrayProxyWidget::getAllItems(QListWidget* listWidget, bool ignoreSelectAll)
{
  QList<QListWidgetItem*> listItems;

  for(int i = 0; listWidget->item(i) != nullptr; i++)
  {
    if((false == ignoreSelectAll) || (listWidget->item(i)->text() != "Select All"))
    {
      listItems.append(listWidget->item(i));
    }
  }

  return listItems;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::checkAllItems(QListWidget* listWidget, Qt::CheckState state)
{
  QList<QListWidgetItem*> listItems = getAllItems(listWidget);

  for(int i = 0; i < listItems.size(); i++)
  {
    QListWidgetItem* item = listItems[i];
    listItems[i]->setCheckState(state);

    updateProxyChecked(item, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::CheckState DataContainerArrayProxyWidget::updateSelectAllState(QListWidget* listWidget)
{
  QList<QListWidgetItem*> listItems = getAllItems(listWidget);

  int numChecked = 0;
  for(int i = 0; i < listItems.size(); i++)
  {
    if(listItems[i]->checkState() == Qt::Checked)
    {
      numChecked++;
    }
  }

  Qt::CheckState state;
  if(numChecked == listItems.size())
  {
    state = Qt::CheckState::Checked;
  }
  else if(numChecked == 0)
  {
    state = Qt::CheckState::Unchecked;
  }
  else
  {
    state = Qt::CheckState::PartiallyChecked;
  }

  QCheckBox* selectAll = nullptr;
  if(listWidget == dataContainerList)
  {
    selectAll = selectAllDataContainer;
  }
  else if(listWidget == attributeMatrixList)
  {
    selectAll = selectAllAttributeMatrix;
  }
  else if(listWidget == dataArrayList)
  {
    selectAll = selectAllDataArray;
  }

  if(selectAll)
  {
    selectAll->setCheckState(state);
  }

  return state;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::applyDataContainerArrayProxy(DataContainerArrayProxy proxy)
{
  QListWidget* listWidget = dataContainerList;
  listWidget->clear();

  for(auto iter = proxy.dataContainers.begin(); iter != proxy.dataContainers.end(); iter++)
  {
    QListWidgetItem* item = new QListWidgetItem(iter.key(), listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(static_cast<Qt::CheckState>(iter.value().flag));
  }

  m_DcaProxy = proxy;
  applyDataContainerProxy();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::applyDataContainerProxy()
{
  QListWidget* listWidget = attributeMatrixList;
  listWidget->clear();

  if(false == m_DcName.isEmpty())
  {
    DataContainerProxy proxy = getDataContainerProxy();
    for(auto iter = proxy.attributeMatricies.begin();
      iter != proxy.attributeMatricies.end(); iter++)
    {
      QListWidgetItem* item = new QListWidgetItem(iter.key(), listWidget);
      item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
      item->setCheckState(static_cast<Qt::CheckState>(iter.value().flag));
      
      toggleStrikeOutFont(item, static_cast<Qt::CheckState>(proxy.flag));
    }
  }

  applyAttributeMatrixProxy();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::applyAttributeMatrixProxy()
{
  QListWidget* listWidget = dataArrayList;
  listWidget->clear();

  if(false == m_AmName.isEmpty())
  {
    AttributeMatrixProxy proxy = getAttributeMatrixProxy();
    for(auto iter = proxy.dataArrays.begin();
      iter != proxy.dataArrays.end(); iter++)
    {
      QListWidgetItem* item = new QListWidgetItem(iter.key(), listWidget);
      item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
      item->setCheckState(static_cast<Qt::CheckState>(iter.value().flag));

      Qt::CheckState checkState = static_cast<Qt::CheckState>(proxy.flag);
      toggleStrikeOutFont(item, checkState);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void transferDataContainFlags(const DataContainerProxy& source, DataContainerArrayProxy& dest)
{
  QMap<QString, DataContainerProxy>& dcProxies = dest.dataContainers;
  for(QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
  {
    DataContainerProxy& dcProxy = dcIter.value();
    if(dcProxy.name.compare(source.name) == 0)
    {
      // we have the correct DataContainer, so transfer the flags
      dcProxy.flag = source.flag;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void transferAttributeMatrixFlags(const QString dcName, const AttributeMatrixProxy& source, DataContainerArrayProxy& dest)
{
  QMap<QString, DataContainerProxy>& dcProxies = dest.dataContainers;
  for(QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
  {
    DataContainerProxy& dcProxy = dcIter.value();
    if(dcProxy.name.compare(dcName) == 0)
    {
      // we have the correct DataContainer, so transfer the flags
      //      dcProxy.flag = source.flag;
      QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.attributeMatricies;
      for(QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter)
      {
        QString amName = amIter.key();
        if(amName.compare(source.name) == 0)
        {
          AttributeMatrixProxy& attrProxy = amIter.value();
          attrProxy.flag = source.flag;
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void transferDataArrayFlags(const QString dc_name, const QString am_name, const DataArrayProxy& source, DataContainerArrayProxy& dest)
{
  QMap<QString, DataContainerProxy>& dcProxies = dest.dataContainers;
  for(QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
  {
    DataContainerProxy& dcProxy = dcIter.value();
    if(dcProxy.name.compare(dc_name) == 0)
    {
      // we have the correct DataContainer, so transfer the flags
      // dcProxy.flag = source.flag;
      QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.attributeMatricies;
      for(QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter)
      {
        QString amName = amIter.key();
        if(amName.compare(am_name) == 0)
        {
          AttributeMatrixProxy& attrProxy = amIter.value();
          // attrProxy.flag = source.flag;

          QMap<QString, DataArrayProxy>& daProxies = attrProxy.dataArrays;
          for(QMap<QString, DataArrayProxy>::iterator daIter = daProxies.begin(); daIter != daProxies.end(); ++daIter)
          {
            QString daName = daIter.key();
            if(daName.compare(source.name) == 0)
            {
              DataArrayProxy& daProxy = daIter.value();
              daProxy = source;
            }
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::updateProxyFromProxy(DataContainerArrayProxy& current, DataContainerArrayProxy& incoming)
{
  //  qDebug() << getFilter()->getNameOfClass() << " DataContainerArrayProxyWidget::mergeProxies";

  // Loop over the current model and only worry about getting the flag for each proxy from the current and transfering
  // that flag to the incoming. This allows us to save the selections but also update the model later on with this new
  // proxy which will have selection flags set appropriately.

  QList<DataContainerProxy> containers = current.dataContainers.values();
  QListIterator<DataContainerProxy> containerIter(containers);
  //  QStringList dcList;
  while(containerIter.hasNext())
  {
    DataContainerProxy dcProxy = containerIter.next();

    // We have a DataContainer from the DataContainerArrayProxy, transfer any flags from this DataContainerProxy to
    // the same one in the incoming DataContainerArrayProxy
    transferDataContainFlags(dcProxy, incoming);

    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.attributeMatricies;
    QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext())
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      const AttributeMatrixProxy attrProxy = attrMatsIter.value();

      transferAttributeMatrixFlags(dcProxy.name, attrProxy, incoming);

      //   qDebug() << "@@@ " << amName;
      // Loop over the current AttributeMatrixProxy and see if we need to transfer any flags.
      const QMap<QString, DataArrayProxy> dataArrays = attrProxy.dataArrays;
      QMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
      while(dataArraysIter.hasNext())
      {
        dataArraysIter.next();
        DataArrayProxy daProxy = dataArraysIter.value();

        transferDataArrayFlags(dcProxy.name, attrProxy.name, daProxy, incoming);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::beforePreflight()
{
  if(m_DidCausePreflight == false)
  {
    //  qDebug() << getFilter()->getNameOfClass() << " DataContainerArrayProxyWidget::beforePreflight()";
    // Get the DataContainerArray from the Filter instance. This will have what will become the choices for the user
    // to select from.
    DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
    DataContainerArrayProxy incomingProxy = DataContainerArrayProxy(dca.get());
    incomingProxy.setAllFlags(m_FilterParameter->getDefaultFlagValue());
    // incomingProxy.print("BeforePreflight INCOMING");
    // Now the idea becomes to save the selections that the user has made and transfer those changes to the incoming
    // proxy object
    updateProxyFromProxy(m_DcaProxy, incomingProxy);
    // proxy.print("'proxy' beforePreflight AFTER updateProxyFromProxy()");

    m_DcaProxy = incomingProxy;
    // Now that the proxy was updated with our selections, make the updated incoming proxy into our cache
    // Now update the Widget
    // ... and don't cause three more preflights from "changing" each of the QListWidgets
    blockSignals(true);
    applyDataContainerArrayProxy(m_DcaProxy);
    blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::afterPreflight()
{
  // qDebug() << getFilter()->getNameOfClass() << " DataContainerArrayProxyWidget::afterPreflight()";
}
