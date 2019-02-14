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

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/DataContainers/DataContainerProxy.h"

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
void DataContainerArray::addDataContainer(DataContainer::Pointer f)
{
  push_back(f);
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
  DataContainer::Pointer f = DataContainer::NullPointer();
  for(auto iter = begin(); iter != end(); ++iter)
  {
    DataContainer::Pointer dc = std::dynamic_pointer_cast<DataContainer>(*iter);
    if(dc->getName().compare(name) == 0)
    {
      f = dc;
      erase(iter);
      return f;
    }
  }

  // DO NOT return a NullPointer for any reason other than "DataContainer was not found"
  return f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::renameDataContainer(const QString& oldName, const QString& newName)
{
  auto iter = find(oldName);
  if(iter == end())
  {
    return false;
  }
  return (*iter)->setName(newName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainerArray::getDataContainer(const QString& name)
{
  auto iter = find(name);
  if(iter == end())
  {
    return DataContainer::NullPointer();
  }
  return std::dynamic_pointer_cast<DataContainer>(*iter);
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
  addDataContainer(new_f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DataContainerArray::getDataContainerNames()
{
  QList<QString> names;
  for(auto iter = begin(); iter != end(); ++iter)
  {
    names.push_back((*iter)->getName());
  }
  return names;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Container DataContainerArray::getDataContainers()
{
  Container container;
  for(auto iter : getChildren())
  {
    container.push_back(std::dynamic_pointer_cast<DataContainer>(iter));
  }
  return container;
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
int DataContainerArray::readDataContainersFromHDF5(bool preflight, hid_t dcaGid, 
                                                   const DataContainerArrayProxy &dcaProxy, 
                                                   Observable* obs)
{
  int err = 0;
  QList<DataContainerProxy> dcsToRead = dcaProxy.dataContainers.values();
  QListIterator<DataContainerProxy> dcIter(dcsToRead);
  while(dcIter.hasNext()) // DataContainerLevel
  {
    const DataContainerProxy& dcProxy = dcIter.next();
    if(dcProxy.flag == Qt::Unchecked)
    {
      continue;
    }
    if(this->doesDataContainerExist(dcProxy.name))
    {
      if(nullptr != obs)
      {
        QString ss =
            QObject::tr("A Data Container with name %1 already exists in Memory. Reading a Data Container with the same name would over write the one in memory. Currently this is not allowed.")
                .arg(dcProxy.name);
        obs->notifyErrorMessage(getNameOfClass(), ss, -198745600);
      }
      return -198745600;
    }
    DataContainer::Pointer dc = DataContainer::New(dcProxy.name);
    this->addDataContainer(dc);

    // Now open the DataContainer Group in the HDF5 file
    hid_t dcGid = H5Gopen(dcaGid, dcProxy.name.toLatin1().data(), H5P_DEFAULT);
    if(dcGid < 0)
    {
      if(nullptr != obs)
      {
        QString ss = QObject::tr("Error opening Group '%1'").arg(dcProxy.name);
        obs->notifyErrorMessage(getNameOfClass(), ss, -198745602);
      }
      return -198745602;
    }

    err = this->getDataContainer(dcProxy.name)->readMeshDataFromHDF5(dcGid, preflight);
    if(err < 0)
    {
      if(nullptr != obs)
      {
        QString ss = QObject::tr("Error reading Mesh Data from '%1'").arg(dcProxy.name);
        obs->notifyErrorMessage(getNameOfClass(), ss, -198745603);
      }
      return -198745603;
    }
    err = this->getDataContainer(dcProxy.name)->readAttributeMatricesFromHDF5(preflight, dcGid, dcProxy);
    if(err < 0)
    {
      if(nullptr != obs)
      {
        QString ss = QObject::tr("Error reading AttributeMatrix Data from '%1'").arg(dcProxy.name);
        obs->notifyErrorMessage(getNameOfClass(), ss, -198745604);
      }
      return -198745604;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::doesDataContainerExist(const QString& name)
{
  return find(name) != end();
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
    if((*it)->getName().compare(name) == 0)
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
  Container dcs = getDataContainers();
  for(DataContainer::Pointer dc : dcs)
  {
    DataContainer::Pointer dcCopy = dc->deepCopy(forceNoAllocate);
#if 0

    // Deep copy geometry if applicable
    if(nullptr != dcs[i]->getGeometry().get())
    {
      dcCopy->setGeometry(dcs[i]->getGeometry()->deepCopy());
    }

    // Add AttributeMatrix copies
    QMap<QString, AttributeMatrix::Pointer> ams = dcs[i]->getAttributeMatrices();
    for(QMap<QString, AttributeMatrix::Pointer>::Iterator iter = ams.begin(); iter != ams.end(); iter++)
    {
      QVector<size_t> dims = (*iter)->getTupleDimensions();
      AttributeMatrix::Pointer amCopy = AttributeMatrix::New(dims, (*iter)->getName(), (*iter)->getType());

      // Add DataArray copies without allocating memory
      QList<QString> daNames = (*iter)->getAttributeArrayNames();
      for(int i = 0; i < daNames.size(); i++)
      {
        // Copy and add DataArray to AttributeMatrix copy without allocating memory
        IDataArray::Pointer daCopy = (*iter)->getAttributeArray(daNames[i])->deepCopy(true);
        amCopy->addAttributeArray(daNames[i], daCopy);
      }

      // End add AttributeMatrix
      dcCopy->addAttributeMatrix(amCopy->getName(), amCopy);
    }
#endif
    // End add DataContainer
    dcaCopy->addDataContainer(dcCopy);
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
