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
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Montages/AbstractMontage.h"
#include "SIMPLib/Montages/GridMontage.h"

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
int DataContainerArray::getNumDataContainers() const
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
DataContainer::Pointer DataContainerArray::getDataContainer(const QString& name) const
{
  return getChildByName(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainerArray::getDataContainer(const DataArrayPath& path) const
{
  QString dcName = path.getDataContainerName();
  return getDataContainer(dcName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainerArray::getAttributeMatrix(const DataArrayPath& path) const
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
DataContainerArray::NameList DataContainerArray::getDataContainerNames() const
{
  return getNamesOfChildren();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Container DataContainerArray::getDataContainers() const
{
  return getChildren();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::printDataContainerNames(QTextStream& out) const
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
bool DataContainerArray::doesAttributeMatrixExist(const DataArrayPath& path) const
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
bool DataContainerArray::doesAttributeArrayExist(const DataArrayPath& path) const
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
IDataContainerBundle::Pointer DataContainerArray::getDataContainerBundle(const QString& name) const
{
  IDataContainerBundle::Pointer f = IDataContainerBundle::NullPointer();

  auto match = std::find_if(m_DataContainerBundles.cbegin(), m_DataContainerBundles.cend(), [&name](auto bundle) { return bundle->getName() == name; });

  if(match != m_DataContainerBundles.cend())
  {
    f = *match;
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
DataContainerArray::MontageCollection DataContainerArray::getMontageCollection() const
{
  return m_MontageCollection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::addMontage(const AbstractMontageShPtr& montage)
{
  return m_MontageCollection.insert(montage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::addOrReplaceMontage(const AbstractMontageShPtr& montage)
{
  m_MontageCollection.erase(montage->getName());
  m_MontageCollection.insert(montage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::removeMontage(const QString& name)
{
  m_MontageCollection.erase(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractMontageShPtr DataContainerArray::getMontage(const QString& name) const
{
  auto iter = m_MontageCollection.find(name);
  if(m_MontageCollection.end() == iter)
  {
    return nullptr;
  }
  return (*iter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataContainerArray::getMontageNames() const
{
  QStringList names;
  for(const auto& montage : m_MontageCollection)
  {
    names.push_back(montage->getName());
  }

  return names;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractMontageShPtr DataContainerArray::getPrereqMontage(AbstractFilter* filter, const QString& name)
{
  AbstractMontageShPtr montage = getMontage(name);
  if(nullptr == montage)
  {
    filter->setErrorCondition(-4950, "The prerequisite montage '" + name + "' could not be found.");
  }
  return montage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontageShPtr DataContainerArray::createNonPrereqGridMontage(AbstractFilter* filter, const QString& montageName, SizeVec3Type size, const QStringList& dcNames,
                                                                GridMontage::CollectionMethod collectionMethod)
{
  GridMontage::Pointer montage = GridMontage::New(montageName, size[0], size[1], size[2]);

  if(nullptr == montage)
  {
    filter->setErrorCondition(-4965, "The montage '" + montageName + "' could not be instantiated");
    return nullptr;
  }

  QStringListIterator dcNameIter(dcNames);

  switch(collectionMethod)
  {
  case GridMontage::CollectionMethod::CombOrder:
    for(size_t depth = 0; depth < size[2] && dcNameIter.hasNext(); depth++)
    {
      for(size_t row = 0; row < size[0] && dcNameIter.hasNext(); row++)
      {
        for(size_t col = 0; col < size[1] && dcNameIter.hasNext(); col++)
        {
          setMontageTileFromDataContainerName(filter, row, col, depth, montage, dcNameIter.next());
        }
      }
    }
    break;
  case GridMontage::CollectionMethod::SnakeOrder:
    for(size_t depth = 0; depth < size[2] && dcNameIter.hasNext(); depth++)
    {
      for(size_t row = 0; row < size[0] && dcNameIter.hasNext(); row++)
      {
        if(row % 2 == 0)
        {
          for(size_t col = 0; col < size[1] && dcNameIter.hasNext(); col++)
          {
            setMontageTileFromDataContainerName(filter, row, col, depth, montage, dcNameIter.next());
          }
        }
        else
        {
          for(int64_t col = static_cast<int64_t>(size[1] - 1); col >= 0 && dcNameIter.hasNext(); col--)
          {
            setMontageTileFromDataContainerName(filter, row, col, depth, montage, dcNameIter.next());
          }
        }
      }
    }
    break;
  }

  if(!addMontage(montage))
  {
    filter->setErrorCondition(-4960, "The montage '" + montageName + "' could not be added to the Data Structure");
    return nullptr;
  }

  return montage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::setMontageTileFromDataContainerName(AbstractFilter* filter, size_t row, size_t col, size_t depth, const GridMontage::Pointer& montage, const QString& dcName)
{
  GridTileIndex index = montage->getTileIndex(row, col, depth);
  DataContainer::Pointer dc = getPrereqDataContainer(filter, dcName);
  montage->setDataContainer(index, dc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer DataContainerArray::deepCopy(bool forceNoAllocate) const
{
  DataContainerArray::Pointer dcaCopy = DataContainerArray::New();
  const Container dcs = getDataContainers();
  for(const auto& dc : dcs)
  {
    DataContainer::Pointer dcCopy = dc->deepCopy(forceNoAllocate);

    // End add DataContainer
    dcaCopy->push_back(dcCopy);
  }

  const MontageCollection montageCollection = getMontageCollection();
  MontageCollection mccopy = MontageCollection();
  for(const auto& montage : montageCollection)
  {
    AbstractMontage::Pointer montageCopy = montage->propagate(dcaCopy);
    dcaCopy->addMontage(montageCopy);
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
QString DataContainerArray::getNameOfClass() const
{
  return QString("DataContainerArray");
}

// -----------------------------------------------------------------------------
QString DataContainerArray::ClassName()
{
  return QString("DataContainerArray");
}

// -----------------------------------------------------------------------------
DataContainerShPtr DataContainerArray::getPrereqDataContainer(AbstractFilter* filter, const DataArrayPath& dap, bool createIfNotExists)
{
  return getPrereqDataContainer(filter, dap.getDataContainerName(), createIfNotExists);
}

// -----------------------------------------------------------------------------
DataContainerShPtr DataContainerArray::getPrereqDataContainer(AbstractFilter* filter, const QString& name, bool createIfNotExists)
{
  DataContainerShPtr dc = getDataContainer(name);
  if(nullptr == dc.get() && !createIfNotExists)
  {
    if(filter)
    {
      QString ss = "The DataContainer Object with the specific name '" + name + "' was not available.";
      filter->setErrorCondition(-999, ss);
    }
    return dc;
  }

  if(nullptr != dc && createIfNotExists)
  {
    DataContainerShPtr dataContainer = DataContainer::New(name); // Create a new Data Container
    addOrReplaceDataContainer(dataContainer);                    // Put the new DataContainer into the array
    return dataContainer;                                        // Return the wrapped pointer
  }
  // The DataContainer we asked for was present and NON Null so return that.
  return dc;
}

// -----------------------------------------------------------------------------
DataContainerShPtr DataContainerArray::createNonPrereqDataContainer(AbstractFilter* filter, const DataArrayPath& dap, RenameDataPath::DataID_t id)
{
  return createNonPrereqDataContainer(filter, dap.getDataContainerName(), id);
}

// -----------------------------------------------------------------------------
DataContainerShPtr DataContainerArray::createNonPrereqDataContainer(AbstractFilter* filter, const QString& dataContainerName, RenameDataPath::DataID_t id)
{
  if(dataContainerName.isEmpty())
  {
    if(filter)
    {
      QString ss = QObject::tr("The DataContainer Object must have a name to be created.");
      filter->setErrorCondition(-887, ss);
      return DataContainer::NullPointer();
    }
  }

  if(dataContainerName.contains('/'))
  {
    if(filter)
    {
      QString ss = QObject::tr("The DataContainer Object has forward slashes in its name.");
      filter->setErrorCondition(-888, ss);
      return DataContainer::NullPointer();
    }
  }

  DataContainerShPtr dataContainer = DataContainer::New(dataContainerName);
  bool dcExists = !push_back(dataContainer);
  if(dcExists)
  {
    if(filter)
    {
      QString ss = QObject::tr("The DataContainer Object with the specific name '%1' already exists.").arg(dataContainerName);
      filter->setErrorCondition(-889, ss);
      return DataContainer::NullPointer();
    }
  }

  RenameDataPath::AlertFilterCreatedPath(filter, id, DataArrayPath(dataContainerName, "", ""));
  return dataContainer;
}

// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainerArray::getPrereqAttributeMatrixFromPath(AbstractFilter* filter, const DataArrayPath& path, int err)
{
  // First try to get the Parent DataContainer. If an error occurs the error message will have been set
  // so just return a nullptr shared pointer
  DataContainerShPtr dc = getPrereqDataContainer(filter, path.getDataContainerName(), false);
  if(nullptr == dc)
  {
    return AttributeMatrix::NullPointer();
  }

  // Now just return what ever the DataContainer gives us. if the AttributeMatrix was not available then an
  // error message and code will have been set into the "filter" object if that object was non-null itself.
  return dc->getPrereqAttributeMatrix(filter, path.getAttributeMatrixName(), err);
}

// -----------------------------------------------------------------------------
bool DataContainerArray::validateNumberOfTuples(AbstractFilter* filter, const QVector<DataArrayPath>& paths) const
{
  if(paths.size() <= 1)
  {
    return false;
  }
  QVector<IDataArrayShPtrType> dataArrays;
  bool valid = true;
  QString ss;
  if(!paths.at(0).isValid() && nullptr != filter)
  {
    ss = QObject::tr("DataContainerArray::validateNumberOfTuples Error at line %1. The DataArrayPath object was not valid meaning one of the strings in the object is empty. The path is %2")
             .arg(__LINE__)
             .arg(paths.at(0).serialize());
    filter->setErrorCondition(-10000, ss);
    valid = false;
    return valid;
  }
  IDataArrayShPtrType array0 = getPrereqIDataArrayFromPath(filter, paths.at(0));
  if(nullptr == array0.get() && nullptr != filter)
  {
    ss = QObject::tr("DataContainerArray::validateNumberOfTuples Error at line %1. The DataArray object was not available. The path is %2").arg(__LINE__).arg(paths.at(0).serialize());
    filter->setErrorCondition(-10100, ss);
    valid = false;
    return valid;
  }

  dataArrays.push_back(array0);
  for(int32_t i = 1; i < paths.size(); i++)
  {
    if(!paths.at(i).isValid() && nullptr != filter)
    {
      ss = QObject::tr("DataContainerArray::validateNumberOfTuples Error at line %1. The DataArrayPath object was not valid meaning one of the strings in the object is empty. The path is %2")
               .arg(__LINE__)
               .arg(paths.at(i).serialize());
      filter->setErrorCondition(-10000, ss);
      valid = false;
      return valid;
    }
    IDataArrayShPtrType nextArray = getPrereqIDataArrayFromPath(filter, paths.at(i));
    if(nullptr == nextArray.get() && nullptr != filter)
    {
      ss = QObject::tr("DataContainerArray::validateNumberOfTuples Error at line %1. The DataArray object was not available. The path is %2").arg(__LINE__).arg(paths.at(i).serialize());
      filter->setErrorCondition(-10100, ss);
      valid = false;
      return valid;
    }

    dataArrays.push_back(nextArray);
  }
  size_t numTuples = dataArrays[0]->getNumberOfTuples();
  for(int32_t i = 1; i < dataArrays.size(); i++)
  {
    if(numTuples != dataArrays[i]->getNumberOfTuples() && nullptr != filter)
    {
      ss = QObject::tr("The number of tuples for the DataArray %1 is %2 and for DataArray %3 is %4. The number of tuples must match.")
               .arg(dataArrays[0]->getName())
               .arg(dataArrays[0]->getNumberOfTuples())
               .arg(dataArrays[i]->getName())
               .arg(dataArrays[i]->getNumberOfTuples());
      filter->setErrorCondition(-10200, ss);
      valid = false;
    }
  }
  return valid;
}

// -----------------------------------------------------------------------------
bool DataContainerArray::validateNumberOfTuples(AbstractFilter* filter, QVector<IDataArrayShPtrType> dataArrays) const
{
  if(dataArrays.size() <= 1)
  {
    return false;
  }
  bool valid = true;
  QString ss;
  for(const auto& dataArray : dataArrays)
  {
    if(nullptr == dataArray && nullptr != filter)
    {
      ss = QObject::tr("DataContainerArray::validateNumberOfTuples Error at line %1. The DataArray object was not available").arg(__LINE__);
      filter->setErrorCondition(-10100, ss);
      valid = false;
    }
  }
  size_t numTuples = dataArrays[0]->getNumberOfTuples();
  for(int32_t i = 1; i < dataArrays.size(); i++)
  {
    if(numTuples != dataArrays[i]->getNumberOfTuples() && nullptr != filter)
    {
      ss = QObject::tr("The number of tuples for the DataArray %1 is %2 and for DataArray %3 is %4. The number of tuples must match.")
               .arg(dataArrays[i - 1]->getName())
               .arg(dataArrays[i - 1]->getNumberOfTuples())
               .arg(dataArrays[i]->getName())
               .arg(dataArrays[i]->getNumberOfTuples());
      filter->setErrorCondition(-10200, ss);
      valid = false;
    }
  }
  return valid;
}

// -----------------------------------------------------------------------------
IDataArray::Pointer DataContainerArray::getPrereqIDataArrayFromPath(AbstractFilter* filter, const DataArrayPath& path) const
{
  QString ss;
  IDataArray::Pointer dataArray = nullptr;

  if(path.isEmpty())
  {
    if(filter)
    {
      ss = QObject::tr("DataContainerArray::getPrereqIDataArrayFromPath Error at line %1. The DataArrayPath object was empty").arg(__LINE__);
      filter->setErrorCondition(-90000, ss);
    }
    return dataArray;
  }

  if(!path.isValid())
  {
    if(filter)
    {
      ss = QObject::tr("DataContainerArray::getPrereqIDataArrayFromPath Error at line %1. The DataArrayPath object was not valid meaning one of the strings in the object is empty. The path is %2")
               .arg(__LINE__)
               .arg(path.serialize());
      filter->setErrorCondition(-90001, ss);
    }
    return dataArray;
  }

  QString dcName = path.getDataContainerName();
  QString amName = path.getAttributeMatrixName();
  QString daName = path.getDataArrayName();

  DataContainerShPtr dc = getDataContainer(dcName);
  if(nullptr == dc.get())
  {
    if(filter)
    {
      ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
      filter->setErrorCondition(-999, ss);
    }
    return dataArray;
  }

  AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(amName);
  if(nullptr == attrMat.get())
  {
    if(filter)
    {
      ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
      filter->setErrorCondition(-307020, ss);
    }
    return dataArray;
  }

  dataArray = attrMat->getPrereqIDataArray(filter, daName, -90002); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  return dataArray;
}
