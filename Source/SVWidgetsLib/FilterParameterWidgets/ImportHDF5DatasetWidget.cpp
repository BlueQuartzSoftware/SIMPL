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

#include <cmath>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QMimeData>
#include <QtCore/QString>
#include <QtCore/QUrl>

#include <QtCore/QTextStream>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

#include "SIMPLib/CoreFilters/ImportHDF5Dataset.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/ImportHDF5DatasetFilterParameter.h"
#include "SIMPLib/Utilities/FilterCompatibility.hpp"
#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"
#include "SVWidgetsLib/QtSupport/QtSLineEdit.h"
#include "SVWidgetsLib/Widgets/ImportHDF5TreeModel.h"
#include "SVWidgetsLib/Widgets/ImportHDF5TreeModelItem.h"

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
  m_FilterParameter = SIMPL_FILTER_PARAMETER_COMPATIBILITY_CHECK(filter, parameter, ImportHDF5DatasetWidget, ImportHDF5DatasetFilterParameter);
  m_Filter = SIMPL_FILTER_COMPATIBILITY_CHECK(filter, parameter, ImportHDF5DatasetWidget, ImportHDF5Dataset);

  setupUi(this);
  setupGui();

  if(m_Filter != nullptr)
  {
    QString currentPath = m_Filter->property(PROPERTY_NAME_AS_CHAR).toString();
    if(!currentPath.isEmpty())
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
void ImportHDF5DatasetWidget::setIcon(const QPixmap& path)
{
  m_Icon = path;
  setupMenuField();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPixmap ImportHDF5DatasetWidget::getIcon()
{
  return m_Icon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportHDF5DatasetWidget::getCurrentFile() const
{
  return m_CurrentOpenFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::setupGui()
{
  setErrorText("");
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &ImportHDF5DatasetWidget::beforePreflight);

  // Catch when the filter is finished running the preflight
  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &ImportHDF5DatasetWidget::afterPreflight);

  // Catch when the filter wants its values updated
  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &ImportHDF5DatasetWidget::filterNeedsInputParameters);

  QtSFileCompleter* com = new QtSFileCompleter(this, false);
  value->setCompleter(com);
  QObject::connect(com, SIGNAL(activated(const QString&)), this, SLOT(on_value_textChanged(const QString&)));

  setupMenuField();

  // Update the widget when the data directory changes
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  connect(validator, &SIMPLDataPathValidator::dataDirectoryChanged, [=] {
    blockSignals(true);
    on_value_textChanged(value->text());
    on_value_fileDropped(value->text());
    on_value_returnPressed();
    blockSignals(false);

    Q_EMIT parametersChanged();
  });

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
    QString hdf5FilePath = m_Filter->getHDF5FilePath();

    if(!hdf5FilePath.isEmpty())
    {
      value->blockSignals(true);
      value->setText(hdf5FilePath);
      value->blockSignals(false);
      setValue(hdf5FilePath);

      QString currentPath = QString::fromStdString(m_CurrentHDFDataPath);
      cDimsLE->setText(m_ComponentDimsMap[currentPath], true);
    }
  }

  if(m_FilterParameter != nullptr)
  {
    selectBtn->setText(m_FilterParameter->getHumanLabel() + " ...");
  }

  m_CurrentText = value->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::keyPressEvent(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Escape)
  {
    SVStyle* style = SVStyle::Instance();
    value->setText(m_CurrentText);
    style->LineEditClearStyle(value);
    value->setToolTip("");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::setupMenuField()
{
  QFileInfo fi(value->text());

  QMenu* lineEditMenu = new QMenu(value);
  value->setButtonMenu(QtSLineEdit::Left, lineEditMenu);

  value->setButtonVisible(QtSLineEdit::Left, true);

  QPixmap pixmap(8, 8);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  painter.drawPixmap(0, (pixmap.height() - m_Icon.height()) / 2, m_Icon);
  value->setButtonPixmap(QtSLineEdit::Left, pixmap);

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

  if(!value->text().isEmpty() && fi.exists())
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
void ImportHDF5DatasetWidget::on_value_editingFinished()
{
  setValue(value->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::on_value_returnPressed()
{
  setValue(value->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::on_value_textChanged(const QString& text)
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);

  QFileInfo fi(text);
  if(fi.isRelative())
  {
    absPathLabel->setText(inputPath);
  }

  if(QtSFileUtils::HasValidFilePath(inputPath))
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
    style->LineEditBackgroundErrorStyle(value);
    value->setToolTip("Press the 'Return' key to apply your changes");
  }
  else
  {
    style->LineEditClearStyle(value);
    value->setToolTip("");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::on_value_fileDropped(const QString& text)
{
  setValue(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::initializeHDF5Paths()
{
  QList<ImportHDF5Dataset::DatasetImportInfo> importInfoList = m_Filter->getDatasetImportInfoList();
  for(const auto& importInfo : importInfoList)
  {
    QString hdf5Path = importInfo.dataSetPath;
    QString cDimsStr = importInfo.componentDimensions;
    m_ComponentDimsMap.insert(hdf5Path, cDimsStr);
  }

  // Automatically expand HDF groups in the viewer
  ImportHDF5TreeModel* treeModel = dynamic_cast<ImportHDF5TreeModel*>(hdfTreeView->model());
  if(treeModel != nullptr)
  {
    for(const auto& importInfo : importInfoList)
    {
      QString hdf5Path = importInfo.dataSetPath;
      QStringList hdf5PathTokens = hdf5Path.split("/", QSTRING_SKIP_EMPTY_PARTS);
      QModelIndex parentIdx = treeModel->index(0, 0);
      hdfTreeView->expand(parentIdx);
      treeModel->setData(parentIdx, Qt::Checked, Qt::CheckStateRole);
      while(!hdf5PathTokens.empty())
      {
        QString hdf5PathToken = hdf5PathTokens.front();
        QString dsetToken = hdf5PathTokens.back();
        hdf5PathTokens.pop_front();
        QModelIndexList idxList = treeModel->match(treeModel->index(0, 0, parentIdx), Qt::DisplayRole, hdf5PathToken);
        if(!idxList.empty())
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
void ImportHDF5DatasetWidget::on_selectBtn_clicked()
{
  QString s = QString("HDF5 Files (*.hdf5 *.h5);;All Files(*.*)");
  QString file = QFileDialog::getOpenFileName(this, tr("Select HDF5 File"), m_OpenDialogLastDirectory, s);

  if(file.isEmpty())
  {
    return;
  }

  file = QDir::toNativeSeparators(file);

  if(initWithFile(file))
  {
    value->setText(file);
    Q_EMIT parametersChanged();
  }

  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::dragEnterEvent(QDragEnterEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() != 0 ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  m_OpenDialogLastDirectory = parent.dirName();
  QFileInfo fi(file);
  QString ext = fi.suffix();
  if(fi.exists() && fi.isFile() && ((ext.compare("dream3d") != 0) || (ext.compare("h5") != 0) || (ext.compare("hdf5") != 0)))
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
  QString file = urls.count() != 0 ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  m_OpenDialogLastDirectory = parent.dirName();
  QFileInfo fi(file);
  QString ext = fi.suffix();
  file = QDir::toNativeSeparators(file);
  if(fi.exists() && fi.isFile() && ((ext.compare("h5") != 0) || (ext.compare("hdf5") != 0) || (ext.compare("dream3d") != 0)))
  {
    QDir parent(file);
    m_OpenDialogLastDirectory = parent.dirName();
    if(initWithFile(file))
    {
      Q_EMIT parametersChanged();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportHDF5DatasetWidget::initWithFile(const QString& hdf5File)
{
  if(hdf5File.isNull())
  {
    return false;
  }

  // If there is current file open, close it.
  if(m_FileId >= 0)
  {
    herr_t err = H5Utilities::closeFile(m_FileId);
    if(err < 0)
    {
      QString msg;
      QTextStream out(&msg);
      out << "Error closing HDF5 file: " << m_CurrentOpenFile;
      setErrorText(msg);
      return false;
    }
  }

  setErrorText("");
  m_FileId = H5Utilities::openFile(hdf5File.toStdString(), true);
  if(m_FileId < 0)
  {
    QString msg;
    QTextStream out(&msg);
    out << "Error Reading HDF5 file: " << hdf5File;
    setErrorText(msg);
    return false;
  }
  // Delete the old model
  QAbstractItemModel* oldModel = hdfTreeView->model();

  delete oldModel;

  m_ComponentDimsMap.clear();

  // Save the last place the user visited while opening the file
  QString nativeHdf5File = QDir::toNativeSeparators(hdf5File);

  QFileInfo fileInfo(nativeHdf5File);
  m_OpenDialogLastDirectory = fileInfo.path();
  m_CurrentOpenFile = nativeHdf5File;

  // Set the Window Title to the file name
  setWindowTitle(fileInfo.fileName());

  // Get the ImportHDF5TreeModel and set the Root Node
  ImportHDF5TreeModel* treeModel = new ImportHDF5TreeModel(m_FileId, hdfTreeView);
  connect(treeModel, SIGNAL(selectedHDF5PathsChanged()), this, SIGNAL(parametersChanged()));
  hdfTreeView->setModel(treeModel);
#if defined(Q_OS_MAC)
  hdfTreeView->setAttribute(Qt::WA_MacShowFocusRect, false);
#endif

  QModelIndex rootIndex = treeModel->index(0, 0);
  if(rootIndex.isValid())
  {
    hdfTreeView->setExpanded(rootIndex, true);
  }

  // Connect the Tree View selection Model to a method in this class
  connect(hdfTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(hdfTreeView_currentChanged(QModelIndex, QModelIndex)));

  attributesTable->horizontalHeader()->setStretchLastSection(true); // Stretch the last column to fit to the viewport

  initializeHDF5Paths();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::_updateViewFromHDFPath(const std::string& dataPath)
{
  m_CurrentHDFDataPath = dataPath;
  QString message("Current Dataset:");
  message.append(dataPath.c_str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::on_cDimsLE_valueChanged(const QString& text)
{
  m_ComponentDimsMap.insert(QString::fromStdString(m_CurrentHDFDataPath), text);

  Q_EMIT parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::hdfTreeView_currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  setErrorText("");
  cDimsLE->setText(QString());

  // Check to make sure we have a data file opened
  if(m_FileId < 0)
  {
    QString msg;
    QTextStream out(&msg);
    out << "No data file is opened";
    setErrorText(msg);
    return;
  }

  ImportHDF5TreeModel* model = static_cast<ImportHDF5TreeModel*>(hdfTreeView->model());
  QString path = model->indexToHDF5Path(current);

  m_CurrentHDFDataPath = path.toStdString();

  updateGeneralTable(path);
  updateAttributeTable(path);
  updateComponentDimensions(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t ImportHDF5DatasetWidget::updateGeneralTable(const QString& path)
{
  setErrorText("");
  std::string datasetPath = path.toStdString();
  std::string objName = H5Utilities::extractObjectName(datasetPath);
  QString objType;

  herr_t err = 0;
  if(m_FileId < 0)
  {
    QString msg;
    QTextStream out(&msg);
    out << "Error: FileId is Invalid: " << m_FileId;
    setErrorText(msg);
    return m_FileId;
  }
  // Test for Dataset Existance
  H5O_info_t statbuf;
  err = H5Oget_info_by_name(m_FileId, datasetPath.c_str(), &statbuf, H5P_DEFAULT);
  bool isGroup = false;
  if(err < 0)
  {
    generalTable->clearContents(); // Clear the attributes Table
    QString msg;
    QTextStream out(&msg);
    out << "Data Set Does NOT exist at Path: " << path << "\nFileId: " << m_FileId;
    setErrorText(msg);
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
    objType = "HDF5 Group";
    break;
  case H5O_TYPE_DATASET:
    isGroup = false;
    objType = "HDF5 Dataset";
    break;
  case H5O_TYPE_NAMED_DATATYPE:
    isGroup = false;
    objType = "Named DataType";
    break;
  default:
    isGroup = false;
    objType = "UNKNOWN";
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

  if(!isGroup)
  {
    generalTable->setRowCount(7);
    H5T_class_t classType;
    size_t classSize;
    std::vector<hsize_t> dims;
    err = H5Lite::getDatasetInfo(m_FileId, datasetPath, dims, classType, classSize);
    if(err < 0)
    {
      QString msg;
      QTextStream out(&msg);
      out << "Could not get dataset info for " << QString::fromStdString(datasetPath);
      setErrorText(msg);
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
    QString theType = QString::fromStdString(H5Lite::StringForHDFClassType(classType));
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
      QString msg;
      QTextStream out(&msg);
      out << "Error getting number of objects for group: " << grpId;
      setErrorText(msg);
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
  setErrorText("");
  herr_t err = 0;
  if(m_FileId < 0)
  {
    QString msg;
    QTextStream out(&msg);
    out << "Error: FileId is Invalid: " << m_FileId;
    setErrorText(msg);
    return m_FileId;
  }
  // Test for Dataset Existance
  H5O_info_t statbuf;
  err = H5Oget_info_by_name(m_FileId, path.toStdString().c_str(), &statbuf, H5P_DEFAULT);
  if(err < 0)
  {
    attributesTable->clearContents(); // Clear the attributes Table
    QString msg;
    QTextStream out(&msg);
    out << "Data Set Does NOT exist at Path: " << path << "\nFileId: " << m_FileId;
    setErrorText(msg);
    return err;
  }

  // Update the attributes table
  if(nullptr != attributesTable)
  {
    attributesTable->clearContents(); // Clear the attributes Table
    std::list<std::string> attributes;
    err = H5Utilities::getAllAttributeNames(m_FileId, path.toStdString(), attributes);
    if(err < 0)
    {
      QString msg;
      QTextStream out(&msg);
      out << "Error Reading Attributes for datasetPath: " << path;
      setErrorText(msg);
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
      // for(std::list<std::string>::iterator iter = attributes.begin(); iter != attributes.end(); iter++)
      for(const auto& keyStdStr : attributes)
      {
        QString key = QString::fromStdString(keyStdStr);

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
        H5T_class_t classType;
        size_t typeSize;
        hid_t attributeId;
        QH5Lite::getAttributeInfo(parentLoc, objName, key, dims, classType, typeSize, attributeId);

        QString strData = "";
        herr_t err = 0;

        if(classType == H5T_NO_CLASS)
        {
          strData = QString("H5T_NO_CLASS: Not Parsed");
        }
        if(classType == H5T_INTEGER)
        {
          std::vector<int32_t> data;
          err = QH5Lite::readVectorAttribute(parentLoc, objName, key, data);
          strData.clear();
          QTextStream ss(&strData);
          for(const auto& i : data)
          {
            ss << i << " ";
          }
        }
        if(classType == H5T_FLOAT)
        {
          std::vector<float> data;
          err = QH5Lite::readVectorAttribute(parentLoc, objName, key, data);
          strData.clear();
          QTextStream ss(&strData);
          for(const auto& i : data)
          {
            ss << i << " ";
          }
        }
        if(classType == H5T_TIME)
        {
          strData = QString("H5T_TIME: Not Parsed");
        }
        if(classType == H5T_STRING)
        {
          err = QH5Lite::readStringAttribute(parentLoc, objName, key, strData);
          if(err < 0)
          {
            strData = QString("H5T_STRING: Not Parsed");
          }
        }
        if(classType == H5T_BITFIELD)
        {
          strData = QString("H5T_BITFIELD: Not Parsed");
        }
        if(classType == H5T_OPAQUE)
        {
          strData = QString("H5T_OPAQUE: Not Parsed");
        }
        if(classType == H5T_COMPOUND)
        {
          strData = QString("H5T_COMPOUND: Not Parsed");
        }
        if(classType == H5T_REFERENCE)
        {
          strData = QString("H5T_REFERENCE: Not Parsed");
        }
        if(classType == H5T_ENUM)
        {
          strData = QString("H5T_ENUM: Not Parsed");
        }
        if(classType == H5T_VLEN)
        {
          strData = QString("H5T_VLEN: Not Parsed");
        }
        if(classType == H5T_ARRAY)
        {
          strData = QString("H5T_ARRAY: Not Parsed");
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
herr_t ImportHDF5DatasetWidget::updateComponentDimensions(const QString& path)
{
  herr_t err = 0;

  if(m_ComponentDimsMap.contains(path))
  {
    QString cDimsStr = m_ComponentDimsMap[path];
    cDimsLE->setText(cDimsStr);
  }
  else
  {
    QString cDimsStr;
    std::tie(err, cDimsStr) = bestGuessCDims(path);

    if(!cDimsStr.isEmpty() && err == 0)
    {
      cDimsLE->setText(cDimsStr);
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::tuple<herr_t, QString> ImportHDF5DatasetWidget::bestGuessCDims(const QString& path)
{
  setErrorText("");
  herr_t err = 0;
  QString cDimsStr = "";
  QString objName = QH5Utilities::extractObjectName(path);
  if(path.isEmpty())
  {
    return std::make_tuple(err, cDimsStr);
  }
  if(m_FileId < 0)
  {
    err = -1;
    QString msg;
    QTextStream out(&msg);
    out << "Error: File: " << path << "\nFileId is Invalid: " << m_FileId;
    setErrorText(msg);
    return std::make_tuple(err, cDimsStr);
  }

  // Test for Dataset Existance
  H5O_info_t statbuf;
  err = H5Oget_info_by_name(m_FileId, path.toStdString().c_str(), &statbuf, H5P_DEFAULT);
  if(err < 0)
  {
    QString msg;
    QTextStream out(&msg);
    out << "Data Set Does NOT exist at Path: " << path << "\nFileId: " << m_FileId;
    setErrorText(msg);
    return std::make_tuple(err, cDimsStr);
  }

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

  QVector<hsize_t> fileDims;
  H5T_class_t type_class;
  size_t type_size;
  QH5Lite::getDatasetInfo(parentLoc, objName, fileDims, type_class, type_size);

  DataArrayPath amPath = m_Filter->getSelectedAttributeMatrix();
  AttributeMatrix::Pointer am = m_Filter->getDataContainerArray()->getAttributeMatrix(amPath);
  if(am != nullptr)
  {
    std::vector<size_t> amTupleDims = am->getTupleDimensions();
    int fileDimsSize = fileDims.size();

    //      // Calculate the prime factors of the attribute matrix tuple dimensions
    //      QVector<int> amTupleDimsPFs;
    //      for (int i = 0; i < amTupleDims.size(); i++)
    //      {
    //        calculatePrimeFactors(amTupleDims[i], amTupleDimsPFs);
    //      }

    //      // Calculate the prime factors of the file dimensions
    //      QVector<int> fileDimsPFs;
    //      for (int i = 0; i < fileDims.size(); i++)
    //      {
    //        calculatePrimeFactors(fileDims[i], fileDimsPFs);
    //      }

    // If an attribute matrix tuple dimensions prime factor exists in the file dimensions prime factor list, remove it.
    for(int i = 0; i < amTupleDims.size(); i++)
    {
      int amTupleDim = amTupleDims[i];
      if(fileDims.contains(amTupleDim))
      {
        fileDims.removeOne(amTupleDim);
      }
      else
      {
        QH5Utilities::closeHDF5Object(parentLoc);
        return std::make_tuple(-1, "");
      }
    }

    if(!fileDims.empty())
    {
      cDimsStr = QString::number(fileDims[0]);
      for(int i = 1; i < fileDims.size(); i++)
      {
        cDimsStr.append(", ");
        cDimsStr.append(QString::number(fileDims[i]));
      }

      m_ComponentDimsMap.insert(path, cDimsStr);
    }
    else if(fileDimsSize == amTupleDims.size() && !amTupleDims.empty())
    {
      // If no dimensions remain, and the starting dimension sizes were the same, set component dimension to 1
      cDimsStr = QString::number(1);
      m_ComponentDimsMap.insert(path, cDimsStr);
    }
  }

  QH5Utilities::closeHDF5Object(parentLoc);

  return std::make_tuple(err, cDimsStr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::calculatePrimeFactors(int n, QVector<int>& primeFactors)
{
  // Find and store all the 2s that n is divisible by
  while(n % 2 == 0)
  {
    primeFactors.push_back(2);
    n = n / 2;
  }

  // At this point, n must be odd.  So we can skip one element (Note: i = i + 2)
  for(int i = 3; i <= std::sqrt(n); i = i + 2)
  {
    // Store i if n is divisible by i
    while(n % i == 0)
    {
      primeFactors.push_back(i);
      n = n / i;
    }
  }

  // Handle the edge case when the remaining n value is a prime number greater than 2
  if(n > 2)
  {
    primeFactors.push_back(n);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  Q_UNUSED(filter)

  m_Filter->setHDF5FilePath(value->text());

  ImportHDF5TreeModel* treeModel = dynamic_cast<ImportHDF5TreeModel*>(hdfTreeView->model());
  if(treeModel != nullptr)
  {
    QStringList dsetPaths = treeModel->getSelectedHDF5Paths();
    QList<ImportHDF5Dataset::DatasetImportInfo> importInfoList;
    for(int i = 0; i < dsetPaths.size(); i++)
    {
      ImportHDF5Dataset::DatasetImportInfo importInfo;
      importInfo.dataSetPath = dsetPaths[i];
      importInfo.componentDimensions = m_ComponentDimsMap[dsetPaths[i]];
      importInfoList.push_back(importInfo);
    }
    m_Filter->setDatasetImportInfoList(importInfoList);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::beforePreflight()
{
  ImportHDF5TreeModel* treeModel = dynamic_cast<ImportHDF5TreeModel*>(hdfTreeView->model());
  if(treeModel != nullptr)
  {
    for(int i = 0; i < m_CurrentPathsWithErrors.size(); i++)
    {
      QModelIndex index = treeModel->hdf5PathToIndex(m_CurrentPathsWithErrors[i]);
      treeModel->setData(index, false, ImportHDF5TreeModel::Roles::HasErrorsRole);
    }

    // Check for unspecified component dimensions and fill it in with the best guess
    QStringList selectedPaths = treeModel->getSelectedHDF5Paths();
    for(const QString& selectedPath : selectedPaths)
    {
      if(!m_ComponentDimsMap.contains(selectedPath))
      {
        herr_t err;
        QString bestCDimGuess;
        std::tie(err, bestCDimGuess) = bestGuessCDims(selectedPath);

        if(!bestCDimGuess.isEmpty() && (err == 0))
        {
          m_ComponentDimsMap[selectedPath] = bestCDimGuess;
        }
      }
    }
  }

  updateComponentDimensions(QString::fromStdString(m_CurrentHDFDataPath));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::afterPreflight()
{
  m_CurrentPathsWithErrors = m_Filter->getDatasetPathsWithErrors();
  ImportHDF5TreeModel* treeModel = dynamic_cast<ImportHDF5TreeModel*>(hdfTreeView->model());
  if(treeModel != nullptr)
  {
    for(int i = 0; i < m_CurrentPathsWithErrors.size(); i++)
    {
      QModelIndex index = treeModel->hdf5PathToIndex(m_CurrentPathsWithErrors[i]);
      treeModel->setData(index, true, ImportHDF5TreeModel::Roles::HasErrorsRole);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::setValue(const QString& text)
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);
  m_CurrentText = text;

  // Set/Remove the red outline if the file does exist
  if(!QtSFileUtils::VerifyPathExists(inputPath, value))
  {
    return;
  }

  QFileInfo fi(value->text());
  if(fi.isRelative())
  {
    absPathLabel->setText(inputPath);
    absPathLabel->show();
  }
  else
  {
    absPathLabel->hide();
  }

  QString file = QDir::toNativeSeparators(inputPath);

  if(initWithFile(file))
  {
    value->setText(file);
    // Store the last used directory into the private instance variable
    QFileInfo fi(file);
    m_OpenDialogLastDirectory = fi.path();
    setErrorText("");
    Q_EMIT parametersChanged(); // This should force the preflight to run because we are emitting a signal
  }
  errorLabel->update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportHDF5DatasetWidget::getValue()
{
  if(value->text().isEmpty())
  {
    return QDir::homePath();
  }
  return value->text();
}

// -----------------------------------------------------------------------------
void ImportHDF5DatasetWidget::setErrorText(const QString& value)
{
  errorLabel->setText(value);
  errorLabel->setVisible(!value.isEmpty());
}

// -----------------------------------------------------------------------------
QString ImportHDF5DatasetWidget::getErrorText() const
{
  return errorLabel->text();
}
