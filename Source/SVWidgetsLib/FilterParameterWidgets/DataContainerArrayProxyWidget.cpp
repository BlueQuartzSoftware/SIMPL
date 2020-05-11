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

#include <cassert>

#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtCore/QDebug>

#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxyWidget::DataContainerArrayProxyWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
, m_EmptyDcProxy(DataContainerProxy())
, m_EmptyAmProxy(AttributeMatrixProxy())
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

  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &DataContainerArrayProxyWidget::beforePreflight);

  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &DataContainerArrayProxyWidget::afterPreflight);

  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &DataContainerArrayProxyWidget::filterNeedsInputParameters);

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)), this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  // setStyleSheet("QColumnView { text-decoration-color: red; }");

  connect(dataContainerList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));
  connect(attributeMatrixList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));
  connect(dataArrayList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));

  connect(dataContainerList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));
  connect(attributeMatrixList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));
  connect(dataArrayList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));

  connect(selectAllDataContainer, SIGNAL(clicked(bool)), this, SLOT(selectAllDataContainersClicked(bool)));
  connect(selectAllAttributeMatrix, SIGNAL(clicked(bool)), this, SLOT(selectAllAttributeMatricesClicked(bool)));
  connect(selectAllDataArray, SIGNAL(clicked(bool)), this, SLOT(selectAllDataArraysClicked(bool)));

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
  if(nullptr == item)
  {
    return;
  }

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
  if(nullptr == item)
  {
    return;
  }

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
      DataContainerArrayProxy::StorageType dcMap = m_DcaProxy.getDataContainers();
      if(dcMap.find(name) != dcMap.end())
      {
        DataContainerProxy dcProxy = dcMap.value(name);
        dcProxy.setFlag(item->checkState());
        dcMap[name] = dcProxy;
        m_DcaProxy.setDataContainers(dcMap);
      }
    }
    else if(attributeMatrixList == widget)
    {
      DataContainerProxy dcProxy = getDataContainerProxy();
      DataContainerProxy::StorageType amMap = dcProxy.getAttributeMatricies();
      if(amMap.find(name) != amMap.end())
      {
        AttributeMatrixProxy amProxy = amMap.value(name);
        amProxy.setFlag(item->checkState());
        amMap[name] = amProxy;
        dcProxy.setAttributeMatricies(amMap);
        m_DcaProxy.insertDataContainer(dcProxy.getName(), dcProxy);
      }
    }
    else if(dataArrayList == widget)
    {
      getAttributeMatrixProxy().getDataArrays().find(name).value().setFlag(item->checkState());
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

  if(!m_DcName.isEmpty())
  {
    dcChecked = (getDataContainerProxy().getFlag() == Qt::Checked);
  }
  if(!m_AmName.isEmpty())
  {
    amChecked = (getAttributeMatrixProxy().getFlag() == Qt::Checked);
  }

  if(item->listWidget() == dataContainerList)
  {
    return item->checkState() == Qt::Checked;
  }
  if(item->listWidget() == attributeMatrixList)
  {
    return dcChecked || item->checkState() == Qt::Checked;
  }
  if(item->listWidget() == dataArrayList)
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
  else if(!static_cast<bool>(item->checkState()))
  {
    item->setBackground(defaultBrush);
  }
  item->setFont(font);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::updateDataArrayPath(QString propertyName, const DataArrayPath::RenameType& renamePath)
{
  blockSignals(true);

  // Update private values
  m_DcaProxy.updatePath(renamePath);

  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  if(oldPath.getDataContainerName() == m_DcName)
  {
    m_DcName = newPath.getDataContainerName();

    if(oldPath.getAttributeMatrixName() == m_AmName)
    {
      m_AmName = newPath.getAttributeMatrixName();
    }
  }

  applyDataContainerArrayProxy(m_DcaProxy);

  //// Update widget values
  // bool oldDcSelected = dataContainerList->currentItem() && dataContainerList->currentItem()->text() == oldPath.getDataContainerName();
  // bool oldAmSelected = oldDcSelected && attributeMatrixList->currentItem() && attributeMatrixList->currentItem()->text() == oldPath.getAttributeMatrixName();

  //// Selected DataContainer Renamed
  // if(oldDcSelected)
  //{
  //  // Selected AttributeMatrix Renamed
  //  if(oldAmSelected)
  //  {
  //    // Update DataArray list names
  //    QList<QListWidgetItem*> daItems = dataContainerList->findItems(oldPath.getDataArrayName(), Qt::MatchFlag::MatchCaseSensitive);
  //    for(QListWidgetItem* item : daItems)
  //    {
  //      item->setText(newPath.getDataArrayName());
  //    }
  //  } // End DataArray

  //  // Update AttributeMatrix list names
  //  QList<QListWidgetItem*> amItems = dataContainerList->findItems(oldPath.getAttributeMatrixName(), Qt::MatchFlag::MatchCaseSensitive);
  //  for(QListWidgetItem* item : amItems)
  //  {
  //    item->setText(newPath.getAttributeMatrixName());
  //  } // End AttributeMatrix
  //}

  //// Update DataContainer list names
  // QList<QListWidgetItem*> dcItems = dataContainerList->findItems(oldPath.getDataContainerName(), Qt::MatchFlag::MatchCaseSensitive);
  // for(QListWidgetItem* item : dcItems)
  //{
  //  item->setText(newPath.getDataContainerName());
  //}

  blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  Q_UNUSED(filter)
  DataContainerArrayProxyFilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
  if(setter)
  {
    setter(m_DcaProxy);
  }
  else
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
void DataContainerArrayProxyWidget::selectDataContainer(const QString& name)
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
void DataContainerArrayProxyWidget::selectAttributeMatrix(const QString& name)
{
  if(getDataContainerProxy().getAttributeMatricies().contains(name))
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
  if(m_DcaProxy.getDataContainers().contains(m_DcName))
  {
    return m_DcaProxy.getDataContainers().find(m_DcName).value();
  }

  return m_EmptyDcProxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy& DataContainerArrayProxyWidget::getAttributeMatrixProxy()
{
  if(getDataContainerProxy().getAttributeMatricies().contains(m_AmName))
  {
    return getDataContainerProxy().getAttributeMatricies().find(m_AmName).value();
  }

  return m_EmptyAmProxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QListWidgetItem*> DataContainerArrayProxyWidget::getAllItems(QListWidget* listWidget, bool ignoreSelectAll)
{
  QList<QListWidgetItem*> listItems;

  for(int i = 0; listWidget->item(i) != nullptr; i++)
  {
    if((!ignoreSelectAll) || (listWidget->item(i)->text() != "Select All"))
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
  for(QListWidgetItem* item : listItems)
  {
    item->setCheckState(state);
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
  for(QListWidgetItem* item : listItems)
  {
    if(item->checkState() == Qt::Checked)
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

  if(selectAll != nullptr)
  {
    selectAll->setCheckState(state);
  }

  return state;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::applyDataContainerArrayProxy(DataContainerArrayProxy& proxy)
{
  QListWidget* listWidget = dataContainerList;
  listWidget->clear();

  DataContainerArrayProxy::StorageType dcMap = proxy.getDataContainers();
  for(const auto& dcProxy : dcMap)
  {
    QListWidgetItem* item = new QListWidgetItem(dcProxy.getName(), listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(static_cast<Qt::CheckState>(dcProxy.getFlag()));
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

  if(!m_DcName.isEmpty())
  {
    listWidget->blockSignals(true);
    DataContainerProxy proxy = getDataContainerProxy();
    DataContainerProxy::StorageType amMap = proxy.getAttributeMatricies();
    for(auto iter = amMap.begin(); iter != amMap.end(); iter++)
    {
      QListWidgetItem* item = new QListWidgetItem(iter.key(), listWidget);
      item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
      item->setCheckState(static_cast<Qt::CheckState>(iter.value().getFlag()));

      toggleStrikeOutFont(item, static_cast<Qt::CheckState>(proxy.getFlag()));
    }

    listWidget->blockSignals(false);
    listWidget->update();
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

  if(!m_AmName.isEmpty())
  {
    listWidget->blockSignals(true);
    AttributeMatrixProxy proxy = getAttributeMatrixProxy();
    AttributeMatrixProxy::StorageType daMap = proxy.getDataArrays();
    for(auto iter = daMap.begin(); iter != daMap.end(); iter++)
    {
      QListWidgetItem* item = new QListWidgetItem(iter.key(), listWidget);
      item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
      item->setCheckState(static_cast<Qt::CheckState>(iter.value().getFlag()));

      Qt::CheckState checkState = static_cast<Qt::CheckState>(proxy.getFlag());
      toggleStrikeOutFont(item, checkState);
    }

    listWidget->blockSignals(false);
    listWidget->update();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void transferDataContainFlags(const DataContainerProxy& source, DataContainerArrayProxy& dest)
{
  QMap<QString, DataContainerProxy> dcProxies = dest.getDataContainers();
  for(auto& dcProxy : dcProxies)
  {
    if(dcProxy.getName() == source.getName())
    {
      // we have the correct DataContainer, so transfer the flags
      dcProxy.setFlag(source.getFlag());
    }
  }
  dest.setDataContainers(dcProxies);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void transferAttributeMatrixFlags(const QString& dcName, const AttributeMatrixProxy& source, DataContainerArrayProxy& dest)
{
  QMap<QString, DataContainerProxy>& dcProxies = dest.getDataContainers();
  for(QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
  {
    DataContainerProxy& dcProxy = dcIter.value();
    if(dcProxy.getName() == dcName)
    {
      QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.getAttributeMatricies();
      for(QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter)
      {
        const QString& amName = amIter.key();
        if(amName.compare(source.getName()) == 0)
        {
          AttributeMatrixProxy& attrProxy = amIter.value();
          attrProxy.setFlag(source.getFlag());
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void transferDataArrayFlags(const QString& dc_name, const QString& am_name, const DataArrayProxy& source, DataContainerArrayProxy& dest)
{
  QMap<QString, DataContainerProxy>& dcProxies = dest.getDataContainers();
  for(QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
  {
    DataContainerProxy& dcProxy = dcIter.value();
    if(dcProxy.getName() == dc_name)
    {
      // we have the correct DataContainer, so transfer the flags
      // dcProxy.flag = source.flag;
      QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.getAttributeMatricies();
      for(QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter)
      {
        const QString& amName = amIter.key();
        if(amName.compare(am_name) == 0)
        {
          AttributeMatrixProxy& attrProxy = amIter.value();
          // attrProxy.flag = source.flag;

          QMap<QString, DataArrayProxy>& daProxies = attrProxy.getDataArrays();
          for(QMap<QString, DataArrayProxy>::iterator daIter = daProxies.begin(); daIter != daProxies.end(); ++daIter)
          {
            const QString& daName = daIter.key();
            if(daName == source.getName())
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

  DataContainerArrayProxy::StorageType dcMap = current.getDataContainers();
  for(auto& dcProxy : dcMap)
  {
    // We have a DataContainer from the DataContainerArrayProxy, transfer any flags from this DataContainerProxy to
    // the same one in the incoming DataContainerArrayProxy
    transferDataContainFlags(dcProxy, incoming);

    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.getAttributeMatricies();
    for(auto& attrProxy : attrMats)
    {
      transferAttributeMatrixFlags(dcProxy.getName(), attrProxy, incoming);

      // Loop over the current AttributeMatrixProxy and see if we need to transfer any flags.
      const QMap<QString, DataArrayProxy>& dataArrays = attrProxy.getDataArrays();
      QMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
      while(dataArraysIter.hasNext())
      {
        dataArraysIter.next();
        DataArrayProxy daProxy = dataArraysIter.value();

        transferDataArrayFlags(dcProxy.getName(), attrProxy.getName(), daProxy, incoming);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::beforePreflight()
{
  if(!m_DidCausePreflight)
  {
    //  qDebug() << getFilter()->getNameOfClass() << " DataContainerArrayProxyWidget::beforePreflight()";
    // Get the DataContainerArray from the Filter instance. This will have what will become the choices for the user
    // to select from.
    DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
    DataContainerArrayProxy incomingProxy = DataContainerArrayProxy(dca.get());
    incomingProxy.setFlags(m_FilterParameter->getDefaultFlagValue());
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
