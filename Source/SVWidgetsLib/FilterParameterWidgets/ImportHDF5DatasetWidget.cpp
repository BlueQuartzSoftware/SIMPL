/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImportHDF5DatasetWidget.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QMimeData>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QVariant>

#include <QtGui/QCloseEvent>

#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "H5Support/QH5Utilities.h"

#include "SIMPLib/CoreFilters/ImportHDF5Dataset.h"
#include "SIMPLib/FilterParameters/ImportHDF5DatasetFilterParameter.h"

#include "SVWidgetsLib/Widgets/ImportHDF5TreeModel.h"
#include "SVWidgetsLib/Widgets/ImportHDF5TreeModelItem.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString ImportHDF5DatasetWidget::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5DatasetWidget::ImportHDF5DatasetWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_CurrentOpenFile("")
, m_CurrentHDFDataPath("")
, m_FileId(-1)
{
  m_FilterParameter = dynamic_cast<ImportHDF5DatasetFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "ImportHDF5DatasetWidget can ONLY be used with an ImportHDF5DatasetFilterParameter object");

  //  m_Filter = dynamic_cast<ImportHDF5Dataset*>(filter);
  //  Q_ASSERT_X(m_Filter != nullptr, "NULL Pointer", "ImportHDF5DatasetWidget can ONLY be used with an ImportHDF5Dataset filter");

  setupUi(this);
  setupGui();

  if(filter)
  {
    QString currentPath = filter->property(PROPERTY_NAME_AS_CHAR).toString();
    if(currentPath.isEmpty() == false)
    {
      currentPath = QDir::toNativeSeparators(currentPath);
      // Store the last used directory into the private instance variable
      QFileInfo fi(currentPath);
      m_OpenDialogLastDirectory = fi.path();
    }
    else
    {
      m_OpenDialogLastDirectory = QDir::homePath();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5DatasetWidget::~ImportHDF5DatasetWidget()
{
  if(m_FileId > 0)
  {
    H5Fclose(m_FileId);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // Allow the widget to fill the entire FilterInputWidget space
  setWidgetIsExpanding(true);

  QFont inputFileFont;
  inputFileFont.setBold(true);
  inputFileFont.setItalic(true);
  inputFileFont.setWeight(75);
  inputFileFont.setStyleStrategy(QFont::PreferAntialias);
#if defined(Q_OS_MAC)
  inputFileFont.setPointSize(12);
#elif defined(Q_OS_WIN)
  inputFileFont.setPointSize(9);
#else
  inputFileFont.setPointSize(10);
#endif

  value->setFont(inputFileFont);
  AbstractFilter* filter = getFilter();
  if(filter != nullptr)
  {
    ImportHDF5DatasetFilterParameter::FilePathGetterCallbackType filePathCallback = m_FilterParameter->getFilePathGetterCallback();
    QString hdf5FilePath = filePathCallback();

    if(hdf5FilePath.isEmpty() == false)
    {
      value->setText(hdf5FilePath);

      initWithFile(hdf5FilePath);
    }
  }

  if(m_FilterParameter != nullptr)
  {
    selectBtn->setText(m_FilterParameter->getHumanLabel() + " ...");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::initializeHDF5Paths()
{
  ImportHDF5DatasetFilterParameter::DatasetGetterCallbackType dSetCallback = m_FilterParameter->getDataSetGetterCallback();
  QStringList hdf5Paths = dSetCallback();

  // Automatically expand HDF groups in the viewer
  ImportHDF5TreeModel* treeModel = dynamic_cast<ImportHDF5TreeModel*>(hdfTreeView->model());
  if(treeModel != nullptr)
  {
    for(int i = 0; i < hdf5Paths.size(); i++)
    {
      QString hdf5Path = hdf5Paths[i];
      QStringList hdf5PathTokens = hdf5Path.split("/", QString::SkipEmptyParts);
      QModelIndex parentIdx = treeModel->index(0, 0);
      hdfTreeView->expand(parentIdx);
      treeModel->setData(parentIdx, Qt::Checked, Qt::CheckStateRole);
      while(hdf5PathTokens.size() > 0)
      {
        QString hdf5PathToken = hdf5PathTokens.front();
        QString dsetToken = hdf5PathTokens.back();
        hdf5PathTokens.pop_front();
        QModelIndexList idxList = treeModel->match(treeModel->index(0, 0, parentIdx), Qt::DisplayRole, hdf5PathToken);
        if(idxList.size() > 0)
        {
          QModelIndex foundIdx = idxList[0];
          hdfTreeView->expand(foundIdx);

          if(treeModel->data(foundIdx, Qt::DisplayRole).toString() == dsetToken)
          {
            treeModel->setData(foundIdx, Qt::Checked, Qt::CheckStateRole);
          }

          parentIdx = foundIdx;
        }
        else
        {
          hdf5PathTokens.clear();
        }
      }

      // Select the dataset
      hdfTreeView->setCurrentIndex(parentIdx);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportHDF5DatasetWidget::verifyPathExists(QString filePath, QtSFSDropLabel* lineEdit)
{
  QFileInfo fileinfo(filePath);
  if(false == fileinfo.exists())
  {
    lineEdit->changeStyleSheet(QtSFSDropLabel::FS_DOESNOTEXIST_STYLE);
  }
  else
  {
    lineEdit->changeStyleSheet(QtSFSDropLabel::FS_STANDARD_STYLE);
  }
  return fileinfo.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::on_value_fileDropped(const QString& text)
{
  m_OpenDialogLastDirectory = text;

  // Set/Remove the red outline if the file does exist
  if(verifyPathExists(text, value) == true)
  {
    if (initWithFile(text))
    {
      emit parametersChanged();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::on_selectBtn_clicked()
{
  QString s = QString("HDF5 Files (*.hdf5 *.h5);;All Files(*.*)");
  QString file = QFileDialog::getOpenFileName(this, tr("Select HDF5 File"), m_OpenDialogLastDirectory, s);

  if(true == file.isEmpty())
  {
    return;
  }

  file = QDir::toNativeSeparators(file);

  if (initWithFile(file))
  {
    emit parametersChanged();
  }

  value->setText(file);

  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::on_showLocationBtn_clicked()
{
  hasValidFilePath(value->text());
  showFileInFileSystem();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::dragEnterEvent(QDragEnterEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  m_OpenDialogLastDirectory = parent.dirName();
  QFileInfo fi(file);
  QString ext = fi.suffix();
  if(fi.exists() && fi.isFile() && (ext.compare("dream3d") || ext.compare("h5") || ext.compare("hdf5")))
  {
    e->accept();
  }
  else
  {
    e->ignore();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::dropEvent(QDropEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  m_OpenDialogLastDirectory = parent.dirName();
  QFileInfo fi(file);
  QString ext = fi.suffix();
  file = QDir::toNativeSeparators(file);
  if(fi.exists() && fi.isFile() && (ext.compare("h5") || ext.compare("hdf5") || ext.compare("dream3d")))
  {
    QDir parent(file);
    m_OpenDialogLastDirectory = parent.dirName();
    if (initWithFile(file) == true)
    {
      emit parametersChanged();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportHDF5DatasetWidget::initWithFile(QString hdf5File)
{
  if(true == hdf5File.isNull())
  {
    return false;
  }

  hid_t fileId = H5Utilities::openFile(hdf5File.toStdString(), true);
  if(fileId < 0)
  {
    std::cout << "Error Reading HDF5 file: " << hdf5File.toStdString() << std::endl;
    return false;
  }

  // Delete the old model
  QAbstractItemModel* oldModel = hdfTreeView->model();
  if(oldModel != nullptr)
  {
    delete oldModel;
  }

  if (m_FileId >= 0)
  {
    herr_t err = H5Utilities::closeFile(m_FileId);
    if (err < 0)
    {
      std::cout << "Error closing HDF5 file: " << m_CurrentOpenFile.toStdString() << std::endl;
      return false;
    }
  }

  m_FileId = fileId;

  // Save the last place the user visited while opening the file
  hdf5File = QDir::toNativeSeparators(hdf5File);

  QFileInfo fileInfo(hdf5File);
  m_OpenDialogLastDirectory = fileInfo.path();
  m_CurrentOpenFile = hdf5File;

  // Set the Window Title to the file name
  setWindowTitle(fileInfo.fileName());

  // Get the ImportHDF5TreeModel and set the Root Node
  ImportHDF5TreeModel* treeModel = new ImportHDF5TreeModel(m_FileId, hdfTreeView);
  connect(treeModel, SIGNAL(modelChanged()), this, SIGNAL(parametersChanged()));
  hdfTreeView->setModel(treeModel);
#if defined(Q_OS_MAC)
  hdfTreeView->setAttribute(Qt::WA_MacShowFocusRect, false);
#endif

  // Connect the Tree View selection Model to a method in this class
  connect(hdfTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(hdfTreeView_currentChanged(QModelIndex, QModelIndex)));

  attributesTable->horizontalHeader()->setStretchLastSection(true); // Stretch the last column to fit to the viewport

  initializeHDF5Paths();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::_updateViewFromHDFPath(std::string dataPath)
{
  m_CurrentHDFDataPath = dataPath;
  QString message("Current Dataset:");
  message.append(dataPath.c_str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::hdfTreeView_currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  // Check to make sure we have a data file opened
  if(m_FileId < 0)
  {
    std::cout << "\t No data file is opened" << std::endl;
    return;
  }

  ImportHDF5TreeModel* model = static_cast<ImportHDF5TreeModel*>(hdfTreeView->model());
  QString path = model->indexToHDF5Path(current);

  updateGeneralTable(path);
  updateAttributeTable(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t ImportHDF5DatasetWidget::updateGeneralTable(const QString& path)
{
  std::string datasetPath = path.toStdString();
  std::string objName = H5Utilities::extractObjectName(datasetPath);
  QString objType;

  herr_t err = 0;
  if(m_FileId < 0)
  {
    std::cout << "Error: FileId is Invalid: " << m_FileId << std::endl;
    return m_FileId;
  }
  // Test for Dataset Existance
  H5O_info_t statbuf;
  err = H5Oget_info_by_name(m_FileId, datasetPath.c_str(), &statbuf, H5P_DEFAULT);
  bool isGroup = false;
  if(err < 0)
  {
    generalTable->clearContents(); // Clear the attributes Table
    std::cout << "Data Set Does NOT exist at Path given: FileId: " << m_FileId << std::endl;
    return err;
  }
  generalTable->clearContents(); // Clear the attributes Table
  // Put the basic info in first
  generalTable->setRowCount(4);
  generalTable->setColumnCount(2);
  generalTable->verticalHeader()->hide();
  generalTable->horizontalHeader()->hide();

#if 0
  QStringList headerLabels;
  headerLabels.insert(0, tr("Key"));
  headerLabels.insert(1, tr("Value"));
  generalTable->setHorizontalHeaderLabels( headerLabels );
#endif

  switch(statbuf.type)
  {
  case H5O_TYPE_GROUP:
    isGroup = true;
    objName = "HDF5 Group";
    break;
  case H5O_TYPE_DATASET:
    objName = "HDF5 Dataset";
    isGroup = false;
    break;
  case H5O_TYPE_NAMED_DATATYPE:
    isGroup = false;
    objName = "Named DataType";
    break;
  default:
    isGroup = false;
    objName = "UNKNOWN";
  }
  // Update the attributes table
  int row = 0;

  addRow(generalTable, row, "Name", QString::fromStdString(objName));
  ++row;
  addRow(generalTable, row, "Path", path);
  ++row;
  addRow(generalTable, row, "Type", objType);
  ++row;

  QString objectIdString = QString::number(statbuf.addr);
  addRow(generalTable, row, "Object ID", objectIdString);
  ++row;

  if(isGroup == false)
  {
    generalTable->setRowCount(7);
    H5T_class_t attr_type;
    size_t attr_size;
    std::vector<hsize_t> dims;
    err = H5Lite::getDatasetInfo(m_FileId, datasetPath, dims, attr_type, attr_size);
    if(err < 0)
    {
      std::cout << "Could not get dataset info for " << datasetPath << std::endl;
      return err;
    }

    addRow(generalTable, row, "No. of Dimension(s)", QString::number(dims.size()));
    ++row;

    QString key;
    int index = 0;
    for(std::vector<hsize_t>::iterator vIter = dims.begin(); vIter != dims.end(); ++vIter)
    {
      if(index > 0)
      {
        key += " x ";
      }
      key += QString::number(*vIter);

      ++index;
    }
    addRow(generalTable, row, "Dimensions Size(s)", key);
    row++;
    hid_t typeId = H5Lite::getDatasetType(m_FileId, datasetPath);
    QString theType;
    if(attr_type == H5T_STRING)
    {
      theType = "H5T_STRING";
    }
    else
    {
      theType = QString::fromStdString(H5Lite::StringForHDFType(typeId));
    }
    err = H5Tclose(typeId);
    addRow(generalTable, row, "Data Type", theType);
    row++;
  }
  else
  {
    generalTable->setRowCount(5);
    hid_t grpId = H5Gopen(m_FileId, datasetPath.c_str(), H5P_DEFAULT);
    hsize_t num_objs = 0;

    H5G_info_t group_info;
    err = H5Gget_info(grpId, &group_info);
    if(err < 0)
    {
      std::cout << "Error getting number of objects for group: " << grpId << std::endl;
      return err;
    }
    num_objs = group_info.nlinks;

    addRow(generalTable, row, "Number of Members", QString::number(num_objs));
    ++row;

    H5Gclose(grpId);
  }

  generalTable->resizeColumnsToContents();
  generalTable->horizontalHeader()->setStretchLastSection(true);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::addRow(QTableWidget* table, int row, const QString& key, const QString& value)
{

  QTableWidgetItem* keyItem = new QTableWidgetItem(key);
  table->setItem(row, 0, keyItem);
  QTableWidgetItem* valueItem = new QTableWidgetItem(value);
  table->setItem(row, 1, valueItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t ImportHDF5DatasetWidget::updateAttributeTable(const QString& path)
{
  QString objName = QH5Utilities::extractObjectName(path);

  herr_t err = 0;
  if(m_FileId < 0)
  {
    std::cout << "Error: FileId is Invalid: " << m_FileId << std::endl;
    return m_FileId;
  }
  // Test for Dataset Existance
  H5O_info_t statbuf;
  err = H5Oget_info_by_name(m_FileId, path.toStdString().c_str(), &statbuf, H5P_DEFAULT);
  if(err < 0)
  {
    attributesTable->clearContents(); // Clear the attributes Table
    std::cout << "Data Set Does NOT exist at Path given: FileId: " << m_FileId << std::endl;
    return err;
  }

  // Update the attributes table
  if(NULL != attributesTable)
  {
    attributesTable->clearContents(); // Clear the attributes Table
    std::list<std::string> attributes;
    err = H5Utilities::getAllAttributeNames(m_FileId, path.toStdString(), attributes);
    if(err < 0)
    {
      std::cout << "Error Reading Attributes for datasetPath: " << path.toStdString() << std::endl;
    }
    else
    {
      attributesTable->setRowCount(attributes.size());
      attributesTable->setColumnCount(2);
      attributesTable->verticalHeader()->hide();
      QStringList headerLabels;
      headerLabels.insert(0, tr("Name"));
      headerLabels.insert(1, tr("Value"));
      attributesTable->setHorizontalHeaderLabels(headerLabels);
      qint32 row = 0;
      for(std::list<std::string>::iterator iter = attributes.begin(); iter != attributes.end(); iter++)
      {
        QString key = QString::fromStdString(*iter);

        QString parentPath = "";
        if(path == "/")
        {
          parentPath = path;
        }
        else
        {
          parentPath = QH5Utilities::getParentPath(path);
        }

        hid_t parentLoc = QH5Utilities::openHDF5Object(m_FileId, parentPath);

        QVector<hsize_t> dims;
        H5T_class_t type_class;
        size_t type_size;
        hid_t attr_type;
        QH5Lite::getAttributeInfo(parentLoc, objName, key, dims, type_class, type_size, attr_type);

        QString strData = "";
        herr_t err = 0;
        if(type_class == H5T_FLOAT)
        {
          float data = 0.0f;
          err = QH5Lite::readScalarAttribute(parentLoc, objName, key, data);
          strData = QString::number(data);
        }
        else if(type_class == H5T_INTEGER)
        {
          int data = 0;
          err = QH5Lite::readScalarAttribute(parentLoc, objName, key, data);
          strData = QString::number(data);
        }
        else if(type_class == H5T_STRING)
        {
          err = QH5Lite::readStringAttribute(parentLoc, objName, key, strData);
        }

        QH5Utilities::closeHDF5Object(parentLoc);

        QTableWidgetItem* keyItem = new QTableWidgetItem(key);
        attributesTable->setItem(row, 0, keyItem);

        QTableWidgetItem* valueItem = new QTableWidgetItem(strData);
        attributesTable->setItem(row, 1, valueItem);

        ++row;
      }
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  Q_UNUSED(filter)

  ImportHDF5DatasetFilterParameter::FilePathSetterCallbackType callback = m_FilterParameter->getFilePathSetterCallback();
  callback(value->text());

  ImportHDF5TreeModel* treeModel = dynamic_cast<ImportHDF5TreeModel*>(hdfTreeView->model());
  if(treeModel != nullptr)
  {
    ImportHDF5DatasetFilterParameter::DatasetSetterCallbackType callback = m_FilterParameter->getDataSetSetterCallback();
    callback(treeModel->getSelectedHDF5Paths());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::afterPreflight()
{
}
