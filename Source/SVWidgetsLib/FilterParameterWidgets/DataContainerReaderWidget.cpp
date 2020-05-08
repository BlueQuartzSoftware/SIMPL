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

#include <cassert>

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include <QtGui/QStandardItemModel>
#include <QtGui/QPainter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

#include <QtCore/QDebug>

#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString DataContainerReaderWidget::m_OpenDialogLastFilePath = "";

namespace Detail
{
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QStandardItem*> findChildItems(QStandardItem* parent, const QString& text)
{
  QList<QStandardItem*> list;
  if(!parent->hasChildren())
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
void removeNonExistantChildren(QStandardItem* parent, const QStringList& possibleNames)
{
  int childCount = parent->rowCount();

  // iterate from back to front as we may pop values out of the model which would screw up the index
  for(int i = childCount - 1; i >= 0; i--)
  {
    QStandardItem* item = parent->child(i);
    QStringList list = possibleNames.filter(item->text());
    if(list.empty()) // the name is in the model but NOT in the proxy so we need to remove it
    {
      // qDebug() << "!! Removing " << item->text();
      parent->removeRow(i);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
QStandardItem* getColumnItem(QStandardItem* parent, QString name, T& proxy)
{
  QStandardItem* item = nullptr;
  QList<QStandardItem*> items = findChildItems(parent, name);
  if(items.count() == 0)
  {
    // Create a new item because we did not find this item already
    item = new QStandardItem(proxy.getName());
    item->setCheckState((proxy.getFlag() == 2 ? Qt::Checked : Qt::Unchecked));
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
    item->setCheckState((proxy.getFlag() == 2 ? Qt::Checked : Qt::Unchecked));
    item->setCheckable(true);
  }

  return item;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
QStandardItem* updateProxyItem(QStandardItem* parent, QString name, T& proxy)
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
    proxy.setFlag(item->checkState());
  }

  return item;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void transferDataContainFlags(const DataContainerProxy& source, DataContainerArrayProxy& dest)
{
  QMap<QString, DataContainerProxy>& dcProxies = dest.getDataContainers();
  for(QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
  {
    DataContainerProxy& dcProxy = dcIter.value();
    if(dcProxy.getName() == source.getName())
    {
      // we have the correct DataContainer, so transfer the flags
      dcProxy.setFlag(source.getFlag());
    }
  }
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
      // we have the correct DataContainer, so transfer the flags
      //      dcProxy.flag = source.flag;
      QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.getAttributeMatricies();
      for(QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter)
      {
        const QString& amName = amIter.key();
        if(amName == source.getName())
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
          AttributeMatrixProxy& amProxy = amIter.value();
          // attrProxy.flag = source.flag;

          QMap<QString, DataArrayProxy>& daProxies = amProxy.getDataArrays();
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

} /* end namespace Detail */

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
, m_FilterParameter(nullptr)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReaderWidget::~DataContainerReaderWidget() = default;

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

  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &DataContainerReaderWidget::beforePreflight);

  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &DataContainerReaderWidget::afterPreflight);

  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &DataContainerReaderWidget::filterNeedsInputParameters);

  // Put in a QStandardItemModel
  QAbstractItemModel* oldModel = dcaProxyView->model();
  QStandardItemModel* model = new QStandardItemModel;
  dcaProxyView->setModel(model);
  delete oldModel;

  //  void activated(const QModelIndex& index);
  connect(dcaProxyView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(itemActivated(const QModelIndex)));

  setupMenuField();

  absPathLabel->hide();

  // Update the widget when the data directory changes
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  connect(validator, &SIMPLDataPathValidator::dataDirectoryChanged, [=] {
    blockSignals(true);
    updateStylingForPath(m_LineEdit->text());
    updateDCAProxy(m_LineEdit->text());
    checkFilePath(m_LineEdit->text());
    blockSignals(false);

    emit parametersChanged();
  });

  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());
  }

  if(getFilter() != nullptr)
  {
    QString path = m_Filter->getInputFile();

    m_LineEdit->setText(path);
    setValidFilePath(m_LineEdit->text());

    checkFilePath(path);
    updateStylingForPath(path);

    emit parametersChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::setupMenuField()
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString path = validator->convertToAbsolutePath(m_LineEdit->text());

  QFileInfo fi(path);

  QMenu* lineEditMenu = new QMenu(m_LineEdit);
  m_LineEdit->setButtonMenu(QtSLineEdit::Left, lineEditMenu);
  QLatin1String iconPath = QLatin1String(":/SIMPL/icons/images/caret-bottom.png");

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

  if(!m_LineEdit->text().isEmpty() && fi.exists())
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
  if(model == nullptr)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }
  QStandardItem* rootItem = model->invisibleRootItem();
  // Loop over the data containers until we find the proper data container
  DataContainerArrayProxy::StorageType& dcMap = proxy.getDataContainers();
  QStringList dcList;
  for(auto& dcProxy : dcMap)
  {
    dcList.push_back(dcProxy.getName());
    QStandardItem* dcItem = Detail::getColumnItem<DataContainerProxy>(rootItem, dcProxy.getName(), dcProxy);
    assert(dcItem != nullptr);
    //    qDebug() << "**  " << dcProxy.name;
    // We found the proper Data Container, now populate the AttributeMatrix List
    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.getAttributeMatricies();
    for(auto& attrProxy : attrMats)
    {
      QString amName = attrProxy.getName();
      QStandardItem* amItem = Detail::getColumnItem<AttributeMatrixProxy>(dcItem, amName, attrProxy);
      assert(amItem != nullptr);
      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QMap<QString, DataArrayProxy>& dataArrays = attrProxy.getDataArrays();
      for(auto& daProxy : dataArrays)
      {
        QString daName = daProxy.getName();
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
  if(model == nullptr)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }
  //
  QStandardItem* rootItem = model->invisibleRootItem();
  // Loop over the data containers until we find the proper data container
  QMap<QString, DataContainerProxy>& dcProxies = m_DcaProxy.getDataContainers();
  for(auto& dcProxy : dcProxies)
  {
    QStandardItem* dcItem = Detail::updateProxyItem<DataContainerProxy>(rootItem, dcProxy.getName(), dcProxy);

    //    qDebug() << "**  " << dcProxy.name;
    // We found the proper Data Container, now populate the AttributeMatrix List
    QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.getAttributeMatricies();
    for(auto& amProxy : amProxies)
    {
      QStandardItem* amItem = Detail::updateProxyItem<AttributeMatrixProxy>(dcItem, amProxy.getName(), amProxy);
      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QMap<QString, DataArrayProxy>& daProxies = amProxy.getDataArrays();
      for(auto& daProxy : daProxies)
      {
        QStandardItem* daItem = Detail::updateProxyItem<DataArrayProxy>(amItem, daProxy.getName(), daProxy);
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

  DataContainerArrayProxy::StorageType& currentDcMap = current.getDataContainers();
  for(auto& dcProxy : currentDcMap)
  {
    // We have a DataContainer from the DataContainerArrayProxy, transfer any flags from this DataContainerProxy to
    // the same one in the incoming DataContainerArrayProxy
    Detail::transferDataContainFlags(dcProxy, incoming);

    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.getAttributeMatricies();
    for(auto& attrProxy : attrMats)
    {
      Detail::transferAttributeMatrixFlags(dcProxy.getName(), attrProxy, incoming);

      //   qDebug() << "@@@ " << amName;
      // Loop over the current AttributeMatrixProxy and see if we need to transfer any flags.
      const QMap<QString, DataArrayProxy> dataArrays = attrProxy.getDataArrays();
      for(auto& daProxy : dataArrays)
      {
        Detail::transferDataArrayFlags(dcProxy.getName(), attrProxy.getName(), daProxy, incoming);
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
  Q_UNUSED(filter)

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString path = validator->convertToAbsolutePath(m_LineEdit->text());

  m_Filter->setInputFile(path);
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
void DataContainerReaderWidget::checkFilePath(const QString& text)
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString path = validator->convertToAbsolutePath(text);

  QFileInfo fi(text);
  if(fi.isRelative())
  {
    absPathLabel->setText(path);
    absPathLabel->show();
    setValidFilePath(path);
  }
  else
  {
    absPathLabel->hide();
  }
  SVStyle* style = SVStyle::Instance();
  style->LineEditClearStyle(m_LineEdit);
  m_CurrentText = text;

  updateDCAProxy(text);
  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::updateDCAProxy(const QString& text)
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString path = validator->convertToAbsolutePath(text);

  DataContainerArrayProxy proxy;

  setOpenDialogLastFilePath(path);
  // Set/Remove the red outline if the file does exist

  if(QtSFileUtils::VerifyPathExists(path, m_LineEdit))
  {
    if(getFilter() != nullptr)
    {
      if(m_Filter->getLastFileRead().compare(path) != 0)
      {
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
        if(nullptr != model)
        {
          model->clear();
        }

        if(!m_Filter->getInputFileDataContainerArrayProxy().getDataContainers().empty() && (path == m_Filter->getLastFileRead() || m_Filter->getLastFileRead().isEmpty()))
        {
          proxy = m_Filter->getInputFileDataContainerArrayProxy();
        }
        else
        {
          proxy = m_Filter->readDataContainerArrayStructure(path);
          m_Filter->setLastRead(QDateTime::currentDateTime());
        }

        m_Filter->setLastFileRead(path); // Update the cached file path in the filter
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
void DataContainerReaderWidget::updateStylingForPath(const QString& text)
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString path = validator->convertToAbsolutePath(text);

  if(QtSFileUtils::HasValidFilePath(path))
  {
    m_ShowFileAction->setEnabled(true);
  }
  else
  {
    m_ShowFileAction->setDisabled(true);
  }

  SVStyle* style = SVStyle::Instance();

  if(text != m_CurrentText)
  {
    style->LineEditBackgroundErrorStyle(m_LineEdit);
    m_LineEdit->setToolTip("Press the 'Return' key to apply your changes");
  }
  else
  {
    style->LineEditClearStyle(m_LineEdit);
    m_LineEdit->setToolTip("");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_m_LineEdit_editingFinished()
{
  checkFilePath(m_LineEdit->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_m_LineEdit_returnPressed()
{
  checkFilePath(m_LineEdit->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_m_LineEdit_textChanged(const QString& text)
{
  updateStylingForPath(text);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_m_LineEdit_fileDropped(const QString& text)
{
  updateDCAProxy(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_selectBtn_clicked()
{
  QString Ftype = "";        // getFilterParameter()->getFileType();
  QString ext = "*.dream3d"; // getFilterParameter()->getFileExtension();
  QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
  QString defaultName = m_OpenDialogLastFilePath;
  QString filePath = QFileDialog::getOpenFileName(this, tr("Select Input File"), defaultName, s);

  if(filePath.isEmpty())
  {
    return;
  }
  filePath = QDir::toNativeSeparators(filePath);
  // Store the last used directory into the private instance variable
  QFileInfo fi(filePath);
  m_OpenDialogLastFilePath = fi.filePath();
  m_LineEdit->setText(filePath);
  setValidFilePath(m_LineEdit->text());
  updateDCAProxy(filePath);
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
