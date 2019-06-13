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
#include "DataContainerArray.h"

#include <QtCore/QTextStream>

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"

#include "SIMPLib/DataContainers/DataContainerProxy.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::DataContainerArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::~DataContainerArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataContainerArray::getDataArrayPath() const
{
  return DataArrayPath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerArray::getNumDataContainers()
{
  return static_cast<int>(size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::clearDataContainers()
{
  clear();
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::insert(size_t index, DataContainer::Pointer f)
{
  QList<DataContainer::Pointer>::iterator it = m_Array.begin();
  for(size_t i = 0; i < index; ++i)
  {
    ++it;
  }
  m_Array.insert(it, f);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::erase(size_t index)
{
  QList<DataContainer::Pointer>::iterator it = m_Array.begin();
  for(size_t i = 0; i < index; ++i)
  {
    ++it;
  }
  QString dcName = (*it)->getName();
  removeDataContainerFromBundles(dcName);
  m_Array.erase(it);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::clear()
{
  m_Array.clear();
  m_DataContainerBundles.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::empty()
{
  return m_Array.isEmpty();
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainerArray::removeDataContainer(const QString& name)
{
  removeDataContainerFromBundles(name);
  auto it = find(name);
  if(it == end())
  {
    // DO NOT return a NullPointer for any reason other than "DataContainer was not found"
    return DataContainer::NullPointer();
  }
  DataContainer::Pointer p = (*it);
  erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::renameDataContainer(const QString& oldName, const QString& newName)
{
  DataContainer::Pointer dc = getChildByName(oldName);
  return dc && dc->setName(newName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::renameDataContainer(const DataArrayPath& oldName, const DataArrayPath& newName)
{
  DataContainer::Pointer dc = getChildByName(oldName.getDataContainerName());
  return dc && dc->setName(newName.getDataContainerName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainerArray::getDataContainer(const QString& name)
{
  return getChildByName(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainerArray::getDataContainer(const DataArrayPath& path)
{
  QString dcName = path.getDataContainerName();
  return getDataContainer(dcName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainerArray::getAttributeMatrix(const DataArrayPath& path)
{
  DataContainer::Pointer dc = getDataContainer(path);
  if(nullptr == dc.get())
  {
    return AttributeMatrix::NullPointer();
  }

  return dc->getAttributeMatrix(path.getAttributeMatrixName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::duplicateDataContainer(const QString& name, const QString& newName)
{
  DataContainer::Pointer f = getDataContainer(name);
  if(f == nullptr)
  {
    return;
  }

  DataContainer::Pointer new_f = f->deepCopy(false);
  new_f->setName(newName);
  addOrReplaceDataContainer(new_f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::NameList DataContainerArray::getDataContainerNames()
{
  return getNamesOfChildren();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Container DataContainerArray::getDataContainers()
{
  return getChildren();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::printDataContainerNames(QTextStream& out)
{
  out << "---------------------------------------------------------------------";
  Container dcArray = getDataContainers();
  for(DataContainer::Pointer dc : dcArray)
  {
    out << dc->getNameOfClass();
  }
  out << "---------------------------------------------------------------------";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerArray::readDataContainersFromHDF5(bool preflight, hid_t dcaGid, DataContainerArrayProxy& dcaProxy, Observable* obs)
{
  int err = 0;

  DataContainerArrayProxy::StorageType dcMap = dcaProxy.getDataContainers();

  for(auto& dcProxy : dcMap)
  {
    if(dcProxy.getFlag() == Qt::Unchecked)
    {
      continue;
    }
    if(this->doesDataContainerExist(dcProxy.getName()))
    {
      if(nullptr != obs)
      {
        QString ss =
            QObject::tr("A Data Container with name %1 already exists in Memory. Reading a Data Container with the same name would over write the one in memory. Currently this is not allowed.")
                .arg(dcProxy.getName());
        obs->setErrorCondition(-198745600, ss);
      }
      return -198745600;
    }
    DataContainer::Pointer dc = DataContainer::New(dcProxy.getName());
    this->addOrReplaceDataContainer(dc);

    // Now open the DataContainer Group in the HDF5 file
    hid_t dcGid = H5Gopen(dcaGid, dcProxy.getName().toLatin1().data(), H5P_DEFAULT);
    if(dcGid < 0)
    {
      if(nullptr != obs)
      {
        QString ss = QObject::tr("Error opening Group '%1'").arg(dcProxy.getName());
        obs->setErrorCondition(-198745602, ss);
      }
      return -198745602;
    }

    err = this->getDataContainer(dcProxy.getName())->readMeshDataFromHDF5(dcGid, preflight);
    if(err < 0)
    {
      if(nullptr != obs)
      {
        QString ss = QObject::tr("Error reading Mesh Data from '%1'").arg(dcProxy.getName());
        obs->setErrorCondition(-198745603, ss);
      }
      return -198745603;
    }
    err = this->getDataContainer(dcProxy.getName())->readAttributeMatricesFromHDF5(preflight, dcGid, dcProxy);
    if(err < 0)
    {
      if(nullptr != obs)
      {
        QString ss = QObject::tr("Error reading AttributeMatrix Data from '%1'").arg(dcProxy.getName());
        obs->setErrorCondition(-198745604, ss);
      }
      return -198745604;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::doesDataContainerExist(const DataArrayPath& dap) const
{
  return contains(dap.getDataContainerName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::doesDataContainerExist(const QString& name) const
{
  return contains(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::doesAttributeMatrixExist(const DataArrayPath& path)
{
  if(!doesDataContainerExist(path.getDataContainerName()))
  {
    return false;
  }

  DataContainer::Pointer dc = getDataContainer(path);
  return dc->doesAttributeMatrixExist(path.getAttributeMatrixName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::doesAttributeArrayExist(const DataArrayPath& path)
{
  if(!doesDataContainerExist(path.getDataContainerName()))
  {
    return false;
  }
  if(!doesAttributeMatrixExist(path))
  {
    return false;
  }
  AttributeMatrix::Pointer attrMat = getAttributeMatrix(path);

  return attrMat->doesAttributeArrayExist(path.getDataArrayName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::setDataContainerBundles(QMap<QString, IDataContainerBundle::Pointer> bundles)
{
  m_DataContainerBundles = bundles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, IDataContainerBundle::Pointer>& DataContainerArray::getDataContainerBundles()
{
  return m_DataContainerBundles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataContainerBundle::Pointer DataContainerArray::getDataContainerBundle(const QString& name)
{
  IDataContainerBundle::Pointer f = IDataContainerBundle::NullPointer();
  for(QMap<QString, IDataContainerBundle::Pointer>::iterator it = m_DataContainerBundles.begin(); it != m_DataContainerBundles.end(); ++it)
  {
    if((*it)->getName() == name)
    {
      f = *it;
      break;
    }
  }

  return f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::addDataContainerBundle(IDataContainerBundle::Pointer dataContainerBundle)
{
  m_DataContainerBundles[dataContainerBundle->getName()] = dataContainerBundle;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerArray::removeDataContainerBundle(const QString& name)
{
  return m_DataContainerBundles.remove(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::removeDataContainerFromBundles(const QString& name)
{
  for(QMap<QString, IDataContainerBundle::Pointer>::iterator iter = m_DataContainerBundles.begin(); iter != m_DataContainerBundles.end(); ++iter)
  {
    IDataContainerBundle::Pointer dcbPtr = iter.value();
    if(dcbPtr.get() != nullptr)
    {
      QVector<QString> dcbNames = dcbPtr->getDataContainerNames();
      for(qint32 i = 0; i < dcbNames.size(); i++)
      {
        if(dcbNames[i].compare(name) == 0)
        {
          dcbPtr->removeDataContainer(name);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::renameDataContainerBundle(const QString& oldName, const QString& newName)
{
  // Make sure we do not already have a DataContainerBundle with the newname
  QMap<QString, IDataContainerBundle::Pointer>::iterator iter = m_DataContainerBundles.find(newName);

  // Now rename the DataContainerBundle
  if(iter == m_DataContainerBundles.end())
  {
    iter = m_DataContainerBundles.find(oldName);
    if(iter == m_DataContainerBundles.end())
    {
      return false;
    }

    m_DataContainerBundles.insert(newName, iter.value());
    iter.value()->setName(newName);
    m_DataContainerBundles.remove(oldName);
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer DataContainerArray::deepCopy(bool forceNoAllocate)
{
  DataContainerArray::Pointer dcaCopy = DataContainerArray::New();
  const Container dcs = getDataContainers();
  for(const auto& dc : dcs)
  {
    DataContainer::Pointer dcCopy = dc->deepCopy(forceNoAllocate);

    // End add DataContainer
    dcaCopy->push_back(dcCopy);
  }

  return dcaCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::renameDataArrayPaths(DataArrayPath::RenameContainer renamePaths)
{
  for(DataArrayPath::RenameType renameType : renamePaths)
  {
    DataArrayPath oldPath;
    DataArrayPath newPath;
    std::tie(oldPath, newPath) = renameType;

    if(oldPath.getDataContainerName() != newPath.getDataContainerName())
    {
      renameDataContainer(oldPath.getDataContainerName(), newPath.getDataContainerName());
    }
    if(oldPath.getAttributeMatrixName() != newPath.getAttributeMatrixName())
    {
      DataContainer::Pointer dc = getDataContainer(newPath);
      if(dc)
      {
        dc->renameAttributeMatrix(oldPath.getAttributeMatrixName(), newPath.getAttributeMatrixName());
      }
    }
    if(oldPath.getDataArrayName() != newPath.getDataArrayName())
    {
      AttributeMatrix::Pointer am = getAttributeMatrix(newPath);
      if(am)
      {
        am->renameAttributeArray(oldPath.getDataArrayName(), newPath.getDataArrayName());
      }
    }
  }
}

// -----------------------------------------------------------------------------
DataContainerArray::Pointer DataContainerArray::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
DataContainerArray::Pointer DataContainerArray::New()
{
  Pointer sharedPtr(new(DataContainerArray));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
const QString DataContainerArray::getNameOfClass() const
{
  return QString("DataContainerArray");
}

// -----------------------------------------------------------------------------
QString DataContainerArray::ClassName()
{
  return QString("DataContainerArray");
}
