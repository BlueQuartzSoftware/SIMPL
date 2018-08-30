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

#include "SIMPLH5DataReader.h"

#include <sstream>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/DataContainers/DataContainerBundle.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "H5Support/QH5Utilities.h"
#include "H5Support/H5ScopedSentinel.h"

const QString Title = "HDF5 Read Error";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLH5DataReader::SIMPLH5DataReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLH5DataReader::~SIMPLH5DataReader()
{
  if (m_FileId >= 0)
  {
    closeFile();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLH5DataReader::openFile(const QString &filePath)
{
  if (m_FileId >= 0)
  {
    QString ss = QObject::tr("Error opening input file '%1' - A file is already open with this reader.").arg(filePath);
    emit errorGenerated(Title, ss, -148);
    return false;
  }

  m_FileId = QH5Utilities::openFile(filePath, true); // Open the file Read Only
  if(m_FileId < 0)
  {
    QString ss = QObject::tr("Error opening input file '%1'.").arg(filePath);
    emit errorGenerated(Title, ss, -149);
    m_FileId = -1;
    return false;
  }

  m_CurrentFilePath = filePath;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLH5DataReader::closeFile()
{
  herr_t err = QH5Utilities::closeFile(m_FileId); // Open the file Read Only
  if(err < 0)
  {
    QString ss = QObject::tr("Error closing input file '%1'").arg(m_CurrentFilePath);
    emit errorGenerated(Title, ss, -151);
    return false;
  }

  m_CurrentFilePath.clear();
  m_FileId = -1;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer SIMPLH5DataReader::readSIMPLDataUsingProxy(const DataContainerArrayProxy &proxy, bool preflight)
{
  if (m_FileId < 0)
  {
    QString ss = QObject::tr("File data unable to be read - file was not properly opened");
    emit errorGenerated(Title, ss, -249);
    return DataContainerArray::NullPointer();
  }

  QString ss;
  int32_t err = 0;
  QString m_FileVersion;
  float fVersion = 0.0f;
  bool check = false;
  DataContainerArray::Pointer dca = DataContainerArray::New();

  // Check to see if version of .dream3d file is prior to new data container names
  err = QH5Lite::readStringAttribute(m_FileId, "/", SIMPL::HDF5::FileVersionName, m_FileVersion);
  fVersion = m_FileVersion.toFloat(&check);
  if(fVersion < 5.0 || err < 0)
  {
    QH5Utilities::closeFile(m_FileId);
    m_FileId = QH5Utilities::openFile(m_CurrentFilePath, false); // Re-Open the file as Read/Write
    err = H5Lmove(m_FileId, "VoxelDataContainer", m_FileId, SIMPL::Defaults::DataContainerName.toLatin1().data(), H5P_DEFAULT, H5P_DEFAULT);
    err = H5Lmove(m_FileId, "SurfaceMeshDataContainer", m_FileId, SIMPL::Defaults::DataContainerName.toLatin1().data(), H5P_DEFAULT, H5P_DEFAULT);
    err = QH5Lite::writeStringAttribute(m_FileId, "/", SIMPL::HDF5::FileVersionName, "5.0");
    QH5Utilities::closeFile(m_FileId);
    m_FileId = QH5Utilities::openFile(m_CurrentFilePath, true); // Re-Open the file as Read Only
  }
  if (check == false)
  {
    QString ss = QObject::tr("File data unable to be read - file version could not be read");
    emit errorGenerated(Title, ss, -250);
    return DataContainerArray::NullPointer();
  }
  if(fVersion < 7.0)
  {
    ss = QObject::tr("File unable to be read - file structure older than 7.0");
    emit errorGenerated(Title, ss, -251);
    return DataContainerArray::NullPointer();
  }
  hid_t dcaGid = H5Gopen(m_FileId, SIMPL::StringConstants::DataContainerGroupName.toLatin1().data(), 0);
  if(dcaGid < 0)
  {
    QString ss = QObject::tr("Error attempting to open the HDF5 Group '%1'").arg(SIMPL::StringConstants::DataContainerGroupName);
    emit errorGenerated(Title, ss, -1923123);
    return DataContainerArray::NullPointer();
  }

  err = dca->readDataContainersFromHDF5(preflight, dcaGid, proxy, this);
  if(err < 0)
  {
    QString ss = QObject::tr("Error trying to read the DataContainers from the file '%1'").arg(m_CurrentFilePath);
    emit errorGenerated(Title, ss, err);
    return DataContainerArray::NullPointer();
  }

  err = H5Gclose(dcaGid);
  dcaGid = -1;

  bool result = readDataContainerBundles(m_FileId, dca);
  if(result == false)
  {
    QString ss = QObject::tr("Error trying to read the DataContainerBundles from the file '%1'").arg(m_CurrentFilePath);
    emit errorGenerated(Title, ss, err);
    return DataContainerArray::NullPointer();
  }

  return dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy SIMPLH5DataReader::readDataContainerArrayStructure(SIMPLH5DataReaderRequirements* req, int &err)
{
  DataContainerArrayProxy proxy;

  if (m_FileId < 0)
  {
    return DataContainerArrayProxy();
  }

  // Check the DREAM3D File Version to make sure we are reading the proper version
  QString d3dVersion;
  err = QH5Lite::readStringAttribute(m_FileId, "/", SIMPL::HDF5::DREAM3DVersion, d3dVersion);
  if(err < 0)
  {
    QString ss = QObject::tr("HDF5 Attribute '%1' was not found on the HDF5 root node and this is required").arg(SIMPL::HDF5::DREAM3DVersion);
    err = -72;
    emit errorGenerated(Title, ss, err);
    return DataContainerArrayProxy();
  }

  QString fileVersion;
  err = QH5Lite::readStringAttribute(m_FileId, "/", SIMPL::HDF5::FileVersionName, fileVersion);
  if(err < 0)
  {
    // std::cout << "Attribute '" << SIMPL::HDF5::FileVersionName.toStdString() << " was not found" << std::endl;
    QString ss = QObject::tr("HDF5 Attribute '%1' was not found on the HDF5 root node and this is required").arg(SIMPL::HDF5::FileVersionName);
    err = -73;
    emit errorGenerated(Title, ss, err);
    return DataContainerArrayProxy();
  }
  //  else {
  //    std::cout << SIMPL::HDF5::FileVersionName.toStdString() << ":" << fileVersion.toStdString() << std::endl;
  //  }

  hid_t dcArrayGroupId = H5Gopen(m_FileId, SIMPL::StringConstants::DataContainerGroupName.toLatin1().constData(), H5P_DEFAULT);
  if(dcArrayGroupId < 0)
  {
    QString ss = QObject::tr("Error opening HDF5 Group '%1'").arg(SIMPL::StringConstants::DataContainerGroupName);
    err = -74;
    emit errorGenerated(Title, ss, err);
    return DataContainerArrayProxy();
  }

  QString h5InternalPath = QString("/") + SIMPL::StringConstants::DataContainerGroupName;

  // Read the entire structure of the file into the proxy
  DataContainer::ReadDataContainerStructure(dcArrayGroupId, proxy, req, h5InternalPath);

  QH5Utilities::closeHDF5Object(dcArrayGroupId);
  return proxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLH5DataReader::readPipelineJson(QString &json)
{
  herr_t err = 0;

  // Check to see if version of .dream3d file is prior to new data container names
  QString fileVersionString = "";
  bool check = false;
  err = QH5Lite::readStringAttribute(m_FileId, "/", SIMPL::HDF5::FileVersionName, fileVersionString);
  float fVersion = fileVersionString.toFloat(&check);
  if (check == false)
  {
    QString ss = QObject::tr("Pipeline unable to be read - file version could not be read.");
    emit errorGenerated(Title, ss, -252);
    return false;
  }
  else if (fVersion < 7.0)
  {
    QString ss = QObject::tr("Pipeline unable to be read - file version older than 7.0.");
    emit errorGenerated(Title, ss, -253);
    return false;
  }

  // HDF5: Open the "Pipeline" Group
  hid_t pipelineGroupId = H5Gopen(m_FileId, SIMPL::StringConstants::PipelineGroupName.toLatin1().data(), H5P_DEFAULT);

  err = QH5Lite::readStringDataset(pipelineGroupId, "Pipeline", json);
  if (err < 0)
  {
    QString ss = QObject::tr("Pipeline unable to be read - JSON dataset could not be read.");
    emit errorGenerated(Title, ss, -254);
    json.clear();
    return false;
  }

  err = H5Gclose(pipelineGroupId);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLH5DataReader::readDataContainerBundles(hid_t fileId, DataContainerArray::Pointer dca)
{
  herr_t err = 0;
  hid_t dcbGroupId = H5Gopen(fileId, SIMPL::StringConstants::DataContainerBundleGroupName.toLatin1().constData(), H5P_DEFAULT);
  if(dcbGroupId < 0)
  {
    // NO Bundles are available to read so just return.
    QString ss = QObject::tr("Error opening HDF5 Group '%1' ").arg(SIMPL::StringConstants::DataContainerBundleGroupName);
    emit errorGenerated(Title, ss, -73);
    return false;
  }

  H5ScopedGroupSentinel sentinel(&dcbGroupId, false);

  QList<QString> groupNames;
  err = QH5Utilities::getGroupObjects(dcbGroupId, H5Utilities::H5Support_GROUP, groupNames);
  if(err < 0)
  {
    QString ss = QObject::tr("Error getting group objects from HDF5 group '%1' ").arg(SIMPL::StringConstants::DataContainerBundleGroupName);
    emit errorGenerated(Title, ss, -74);
    return false;
  }

  char sep = 0x1E;
  QListIterator<QString> iter(groupNames);
  while(iter.hasNext())
  {
    QString bundleName = iter.next();
    DataContainerBundle::Pointer bundle = DataContainerBundle::New(bundleName);

    hid_t bundleId = H5Gopen(dcbGroupId, bundleName.toLatin1().constData(), H5P_DEFAULT);
    sentinel.addGroupId(&bundleId); // Make sure this group gets closed

    // Read in the Data Container Names
    QString dcNames;
    err = QH5Lite::readStringDataset(bundleId, SIMPL::StringConstants::DataContainerNames, dcNames);
    if(err < 0)
    {
      QString ss = QObject::tr("Error reading DataContainer group names from HDF5 group '%1' ").arg(bundleName);
      emit errorGenerated(Title, ss, -75);
      return false;
    }
    QStringList dcNameList = dcNames.split(QString(sep));

    QStringListIterator nameIter(dcNameList);
    while(nameIter.hasNext())
    {
      QString dcName = nameIter.next();
      DataContainer::Pointer dc = dca->getDataContainer(dcName);
      if(nullptr == dc.get())
      {
        qDebug() << "Data Container '" << dcName << "' was nullptr"
                 << " " << __FILE__ << "(" << __LINE__ << ")";
      }
      bundle->addDataContainer(dc);
    }

    QString metaArrays;
    err = QH5Lite::readStringDataset(bundleId, SIMPL::StringConstants::MetaDataArrays, metaArrays);
    if(err < 0)
    {
      QString ss = QObject::tr("Error reading DataContainerBundle meta data arrays from HDF5 group '%1' ").arg(bundleName);
      emit errorGenerated(Title, ss, -76);
      return false;
    }
    QStringList metaNameList = metaArrays.split(QString(sep));
    bundle->setMetaDataArrays(metaNameList);

    dca->addDataContainerBundle(bundle);
  }

  H5Gclose(dcbGroupId);
  dcbGroupId = -1;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLH5DataReader::notifyErrorMessage(const QString& humanLabel, const QString& str, int code)
{
  Q_UNUSED(humanLabel)

  emit errorGenerated(Title, str, code);
}
