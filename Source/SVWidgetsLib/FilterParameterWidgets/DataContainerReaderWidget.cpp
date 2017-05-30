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

#include "DataContainerReaderWidget.h"

#include <assert.h>

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaProperty>

#include <QtCore/QItemSelectionModel>
#include <QtGui/QStandardItemModel>
#include <QtGui/QDesktopServices>
#include <QtGui/QPainter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMenu>

#include "SIMPLib/CoreFilters/DataContainerReader.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

#include "FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString DataContainerReaderWidget::m_OpenDialogLastFilePath = "";

namespace Detail
{
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QStandardItem*> findChildItems(QStandardItem* parent, QString text)
{
  QList<QStandardItem*> list;
  if(parent->hasChildren() == false)
  {
    return list;
  } // No children, nothing to find
  int childCount = parent->rowCount();

  for(int i = 0; i < childCount; i++)
  {
    QStandardItem* item = parent->child(i);
    if(text.compare(item->text()) == 0)
    {
      list.push_back(item);
    }
  }
  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void removeNonExistantChildren(QStandardItem* parent, QStringList possibleNames)
{
  int childCount = parent->rowCount();

  // iterate from back to front as we may pop values out of the model which would screw up the index
  for(int i = childCount - 1; i >= 0; i--)
  {
    QStandardItem* item = parent->child(i);
    QStringList list = possibleNames.filter(item->text());
    if(list.size() == 0) // the name is in the model but NOT in the proxy so we need to remove it
    {
      // qDebug() << "!! Removing " << item->text();
      parent->removeRow(i);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> QStandardItem* getColumnItem(QStandardItem* parent, QString name, T& proxy)
{
  QStandardItem* item = nullptr;
  QList<QStandardItem*> items = findChildItems(parent, name);
  if(items.count() == 0)
  {
    // Create a new item because we did not find this item already
    item = new QStandardItem(proxy.name);
    item->setCheckState((proxy.flag == 2 ? Qt::Checked : Qt::Unchecked));
    item->setCheckable(true);
    parent->appendRow(item);
  }
  else if(items.count() > 1)
  {
    item = nullptr;
  }
  else
  {
    item = items.at(0);
    item->setCheckState((proxy.flag == 2 ? Qt::Checked : Qt::Unchecked));
    item->setCheckable(true);
  }

  return item;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> QStandardItem* updateProxyItem(QStandardItem* parent, QString name, T& proxy)
{
  QStandardItem* item = nullptr;
  if(nullptr == parent)
  {
    return item;
  }
  QList<QStandardItem*> items = findChildItems(parent, name);
  if(items.count() == 1)
  {
    item = items.at(0);
    //   qDebug() << parent->text() << " | " << item->text() << " ::"  << proxy.flag << " (Going to Change to) " << item->checkState();
    proxy.flag = item->checkState();
  }

  return item;
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
          AttributeMatrixProxy& amProxy = amIter.value();
          // attrProxy.flag = source.flag;

          QMap<QString, DataArrayProxy>& daProxies = amProxy.dataArrays;
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

} /* end namespace Detail */

// Include the MOC generated file for this class
#include "moc_DataContainerReaderWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReaderWidget::DataContainerReaderWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<DataContainerReaderFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "DataContainerReaderWidget can ONLY be used with a DataContainerReaderFilterParameter object");
  m_Filter = dynamic_cast<DataContainerReader*>(filter);
  Q_ASSERT_X(getFilter() != nullptr, "NULL Pointer", "DataContainerReaderWidget can ONLY be used with a DataContainerReader object");

  if(m_OpenDialogLastFilePath.isEmpty())
  {
    m_OpenDialogLastFilePath = QDir::homePath();
  }

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReaderWidget::DataContainerReaderWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_Filter(nullptr)
, m_FilterParameter(nullptr)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReaderWidget::~DataContainerReaderWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::setFilter(AbstractFilter* value)
{
  m_Filter = dynamic_cast<DataContainerReader*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter* DataContainerReaderWidget::getFilter() const
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = dynamic_cast<DataContainerReaderFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* DataContainerReaderWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::setupGui()
{

  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // Put in a QStandardItemModel
  QAbstractItemModel* oldModel = dcaProxyView->model();
  QStandardItemModel* model = new QStandardItemModel;
  dcaProxyView->setModel(model);
  delete oldModel;

  //  void activated(const QModelIndex& index);
  connect(dcaProxyView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(itemActivated(const QModelIndex)));

  setupMenuField();

  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());
  }

  if(getFilter() != nullptr)
  {
    QString path = m_Filter->getInputFile();

    m_LineEdit->setText(path);
    on_m_LineEdit_fileDropped(path);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::setupMenuField()
{
  QFileInfo fi(m_LineEdit->text());

  QMenu* lineEditMenu = new QMenu(m_LineEdit);
  m_LineEdit->setButtonMenu(QtSLineEdit::Left, lineEditMenu);
  QLatin1String iconPath = QLatin1String(":/caret-bottom.png");

  m_LineEdit->setButtonVisible(QtSLineEdit::Left, true);

  QPixmap pixmap(8, 8);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  const QPixmap mag = QPixmap(iconPath);
  painter.drawPixmap(0, (pixmap.height() - mag.height()) / 2, mag);
  m_LineEdit->setButtonPixmap(QtSLineEdit::Left, pixmap);

  {
    m_ShowFileAction = new QAction(lineEditMenu);
    m_ShowFileAction->setObjectName(QString::fromUtf8("showFileAction"));
#if defined(Q_OS_WIN)
  m_ShowFileAction->setText("Show in Windows Explorer");
#elif defined(Q_OS_MAC)
  m_ShowFileAction->setText("Show in Finder");
#else
  m_ShowFileAction->setText("Show in File System");
#endif
    lineEditMenu->addAction(m_ShowFileAction);
    connect(m_ShowFileAction, SIGNAL(triggered()), this, SLOT(showFileInFileSystem()));
  }


  if (m_LineEdit->text().isEmpty() == false && fi.exists())
  {
    m_ShowFileAction->setEnabled(true);
  }
  else
  {
    m_ShowFileAction->setDisabled(true);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::itemActivated(const QModelIndex& index)
{
  m_DidCausePreflight = true;
  updateProxyFromModel();
  m_Filter->setInputFileDataContainerArrayProxy(m_DcaProxy); // Set the new proxy into filter

  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::updateModelFromProxy(DataContainerArrayProxy& proxy)
{
  m_DcaProxy = proxy;
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
  if(!model)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }
  QStandardItem* rootItem = model->invisibleRootItem();

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = proxy.dataContainers.values();
  QListIterator<DataContainerProxy> containerIter(containers);
  QStringList dcList;
  while(containerIter.hasNext())
  {
    DataContainerProxy dcProxy = containerIter.next();
    dcList.push_back(dcProxy.name);
    QStandardItem* dcItem = Detail::getColumnItem<DataContainerProxy>(rootItem, dcProxy.name, dcProxy);
    assert(dcItem != nullptr);
    //    qDebug() << "**  " << dcProxy.name;
    // We found the proper Data Container, now populate the AttributeMatrix List
    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.attributeMatricies;
    QMutableMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext())
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      AttributeMatrixProxy& attrProxy = attrMatsIter.value();
      QStandardItem* amItem = Detail::getColumnItem<AttributeMatrixProxy>(dcItem, amName, attrProxy);
      assert(amItem != nullptr);

      //  qDebug() << "@@@ " << amName;
      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QMap<QString, DataArrayProxy>& dataArrays = attrProxy.dataArrays;
      QMutableMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
      while(dataArraysIter.hasNext())
      {
        dataArraysIter.next();
        DataArrayProxy& daProxy = dataArraysIter.value();
        QString daName = dataArraysIter.key();
        //   qDebug() << "#### " << daName;
        QStandardItem* daItem = Detail::getColumnItem<DataArrayProxy>(amItem, daName, daProxy);
        if(nullptr == daItem)
        {
          Q_ASSERT_X(daItem != nullptr, "daItem was nullptr. This can not happen", "");
        }
      }

      // Now remove those items that are still in the model but NOT the proxy. This can happen if a filter upstream
      // renames something
      Detail::removeNonExistantChildren(amItem, dataArrays.keys());
    }
    // Now remove any nonexistant AttributeMatrix objects
    Detail::removeNonExistantChildren(dcItem, attrMats.keys());
  }
  // Remove any Data Containers from the model
  Detail::removeNonExistantChildren(rootItem, dcList);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::updateProxyFromModel()
{
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
  if(!model)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }
  //
  QStandardItem* rootItem = model->invisibleRootItem();
  // Loop over the data containers until we find the proper data container
  QMap<QString, DataContainerProxy>& dcProxies = m_DcaProxy.dataContainers;
  for(QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
  {
    DataContainerProxy& dcProxy = dcIter.value();
    //  dcList.push_back(dcProxy.name);
    QStandardItem* dcItem = Detail::updateProxyItem<DataContainerProxy>(rootItem, dcProxy.name, dcProxy);

    //    qDebug() << "**  " << dcProxy.name;
    // We found the proper Data Container, now populate the AttributeMatrix List
    QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.attributeMatricies;
    for(QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter)
    {
      QString amName = amIter.key();
      AttributeMatrixProxy& amProxy = amIter.value();
      QStandardItem* amItem = Detail::updateProxyItem<AttributeMatrixProxy>(dcItem, amName, amProxy);

      //   qDebug() << "@@@ " << amName;
      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QMap<QString, DataArrayProxy>& daProxies = amProxy.dataArrays;
      for(QMap<QString, DataArrayProxy>::iterator daIter = daProxies.begin(); daIter != daProxies.end(); ++daIter)
      {
        DataArrayProxy& daProxy = daIter.value();
        QString daName = daIter.key();
        //    qDebug() << "#### " << daName;
        QStandardItem* daItem = Detail::updateProxyItem<DataArrayProxy>(amItem, daName, daProxy);
        Q_UNUSED(daItem)
      }
    }
  }

  //  m_DcaProxy.print("updateProxy AFTER Updating");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::updateProxyFromProxy(DataContainerArrayProxy& current, DataContainerArrayProxy& incoming)
{
  //  qDebug() << getFilter()->getNameOfClass() << " DataContainerReaderWidget::mergeProxies";

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
    Detail::transferDataContainFlags(dcProxy, incoming);

    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.attributeMatricies;
    QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext())
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      const AttributeMatrixProxy attrProxy = attrMatsIter.value();

      Detail::transferAttributeMatrixFlags(dcProxy.name, attrProxy, incoming);

      //   qDebug() << "@@@ " << amName;
      // Loop over the current AttributeMatrixProxy and see if we need to transfer any flags.
      const QMap<QString, DataArrayProxy> dataArrays = attrProxy.dataArrays;
      QMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
      while(dataArraysIter.hasNext())
      {
        dataArraysIter.next();
        DataArrayProxy daProxy = dataArraysIter.value();

        Detail::transferDataArrayFlags(dcProxy.name, attrProxy.name, daProxy, incoming);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::beforePreflight()
{
#if 0
  if (m_DidCausePreflight == false)
  {
    //  qDebug() << getFilter()->getNameOfClass() << " DataContainerReaderWidget::beforePreflight()";
    // Get the DataContainerArray from the Filter instance. This will have what will become the choices for the user
    // to select from.
    DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
    DataContainerArrayProxy incomingProxy = DataContainerArrayProxy(dca.get() );
    incomingProxy.setAllFlags(getFilterParameter()->getDefaultFlagValue());
    //incomingProxy.print("BeforePreflight INCOMING");
    //Now the idea becomes to save the selections that the user has made and transfer those changes to the incoming
    // proxy object
    updateProxyFromProxy(m_DcaProxy, incomingProxy);
    //proxy.print("'proxy' beforePreflight AFTER updateProxyFromProxy()");

    m_DcaProxy = incomingProxy;
    // Now that the proxy was updated with our selections, make the updated incoming proxy into our cache
    // Now update the Model
    updateModelFromProxy(m_DcaProxy);
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // qDebug() << "DataContainerReaderWidget::filterNeedsInputParameters()";
  m_Filter->setInputFile(m_LineEdit->text());
  updateProxyFromModel(); // Will update m_DcaProxy with the latest selections from the Model
  m_Filter->setInputFileDataContainerArrayProxy(m_DcaProxy);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::afterPreflight()
{
  // Make sure the model has the latest data from the proxy
  DataContainerArrayProxy p = m_Filter->getInputFileDataContainerArrayProxy();
  updateModelFromProxy(p);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::showFileInFileSystem()
{
  QFileInfo fi(m_CurrentlyValidPath);
  QString path;
  if (fi.isFile())
  {
    path = fi.absoluteFilePath();
  }
  else
  {
    path = fi.absolutePath();
  }

  QtSFileUtils::ShowPathInGui(this, path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerReaderWidget::verifyPathExists(QString m_LineEdit, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(m_LineEdit);
  if(false == fileinfo.exists())
  {
    lineEdit->setStyleSheet("QLineEdit { border: 1px solid red; }");
  }
  else
  {
    lineEdit->setStyleSheet("");
  }
  return fileinfo.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_m_LineEdit_editingFinished()
{
  m_LineEdit->setStyleSheet(QString(""));
  m_CurrentText = m_LineEdit->text();
  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_m_LineEdit_returnPressed()
{
  on_m_LineEdit_editingFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerReaderWidget::hasValidFilePath(const QString &filePath)
{
  QStringList pathParts = filePath.split(QDir::separator());
  if (pathParts.size() <= 0) { return false; }

  QString pathBuildUp;
  QFileInfo fi(filePath);

  /* This block of code figures out, based on the current OS, how the built-up path should begin.
   * For Mac and Linux, it should start with a separator for absolute paths or a path part for relative paths.
   * For Windows, it should start with a path part for both absolute and relative paths.
   * A "path part" is defined as a portion of string that is delimited by separators in a typical path. */
  {
#if defined(Q_OS_WIN)
  /* If there is at least one part, then add it to the pathBuildUp variable.
    A valid Windows path, absolute or relative, has to have at least one part. */
  if (pathParts[0].isEmpty() == false)
  {
    pathBuildUp.append(pathParts[0]);
  }
  else
  {
    return false;
  }
#else
  /* If the first part is empty and the filePath is absolute, then that means that
   * we are starting with the root directory and need to add it to our pathBuildUp */
  if (pathParts[0].isEmpty() && fi.isAbsolute())
  {
    pathBuildUp.append(QDir::separator());
  }
  /* If the first part is empty and the filePath is relative, then that means that
   * we are starting with the first folder part and need to add that to our pathBuildUp */
  else if (pathParts[0].isEmpty() == false && fi.isRelative())
  {
    pathBuildUp.append(pathParts[0] + QDir::separator());
  }
  else
  {
    return false;
  }
#endif
  }

  /* Now that we have started our built-up path, continue adding to the built-up path
   * until either the built-up path is invalid, or until we have processed all remaining path parts. */
  bool valid = false;

  QFileInfo buildingFi(pathBuildUp);
  size_t pathPartsIdx = 1; // We already processed the first path part above
  while (buildingFi.exists() == true && pathPartsIdx <= pathParts.size())
  {
    valid = true;
    m_CurrentlyValidPath = pathBuildUp; // Save the most current, valid built-up path

    // If there's another path part to add, add it to the end of the built-up path
    if (pathPartsIdx < pathParts.size())
    {
      /* If the built-up path doesn't already have a separator on the end, add one. */
      if (pathBuildUp[pathBuildUp.size() - 1] != QDir::separator())
      {
        pathBuildUp.append(QDir::separator());
      }

      pathBuildUp.append(pathParts[pathPartsIdx]);  // Add the next path part to the built-up path
      buildingFi.setFile(pathBuildUp);
    }
    pathPartsIdx++;
  }

  return valid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_m_LineEdit_textChanged(const QString& text)
{
  if (hasValidFilePath(text) == true)
  {
    m_ShowFileAction->setEnabled(true);
  }
  else
  {
    m_ShowFileAction->setDisabled(true);
  }

  if (text != m_CurrentText)
  {
    m_LineEdit->setStyleSheet(QString::fromLatin1("QLineEdit { color: rgb(255, 0, 0); }"));
    m_LineEdit->setToolTip("Press the 'Return' key to apply your changes");
  }
  else
  {
    m_LineEdit->setStyleSheet("");
    m_LineEdit->setToolTip("");
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_m_LineEdit_fileDropped(const QString& text)
{
  DataContainerArrayProxy proxy;

  setOpenDialogLastFilePath(text);
  // Set/Remove the red outline if the file does exist

  if(verifyPathExists(text, m_LineEdit) == true)
  {
    if(getFilter() != nullptr)
    {
      if(m_Filter->getLastFileRead().compare(text) != 0)
      {
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
        if(nullptr != model)
        {
          model->clear();
        }

        if(m_Filter->getInputFileDataContainerArrayProxy().dataContainers.size() > 0 && (text == m_Filter->getLastFileRead() || m_Filter->getLastFileRead().isEmpty()))
        {
          proxy = m_Filter->getInputFileDataContainerArrayProxy();
        }
        else
        {
          proxy = m_Filter->readDataContainerArrayStructure(text);
          m_Filter->setLastRead(QDateTime::currentDateTime());
        }

        m_Filter->setLastFileRead(text); // Update the cached file path in the filter
      }
      else
      {
        proxy = m_Filter->getInputFileDataContainerArrayProxy();
      }

      updateModelFromProxy(proxy);
    }
    emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_selectBtn_clicked()
{
  QString currentPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
  QString Ftype = "";        // getFilterParameter()->getFileType();
  QString ext = "*.dream3d"; // getFilterParameter()->getFileExtension();
  QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
  QString defaultName = m_OpenDialogLastFilePath;
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"), defaultName, s);

  if(true == file.isEmpty())
  {
    return;
  }
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastFilePath = fi.filePath();
  m_LineEdit->setText(file);
  on_m_LineEdit_fileDropped(file);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_dcaProxyView_updatePreviewWidget(const QModelIndex& index)
{
  //    QVariant var = index.data();
  //    QString name = var.toString();

  //    DataArrayInformationDisplayWidget::Pointer widget = DataArrayInformationDisplayWidget::New();
  //    widget->setNameText(name);

  //    // Create widget and add to dcaProxyView
  //    dcaProxyView->setPreviewWidget(widget.get());
}
