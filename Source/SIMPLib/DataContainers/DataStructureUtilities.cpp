/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5261
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "DataStructureUtilities.h"

#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

// -----------------------------------------------------------------------------
DataStructureUtilities::DataStructureUtilities() = default;
// -----------------------------------------------------------------------------
DataStructureUtilities::~DataStructureUtilities() = default;

// -----------------------------------------------------------------------------
DataContainerShPtr DataStructureUtilities::getPrereqDataContainer(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const DataArrayPath& dap, bool createIfNotExists)
{
  return getPrereqDataContainer(filter, dap.getDataContainerName(), createIfNotExists);
}

// -----------------------------------------------------------------------------
DataContainerShPtr DataStructureUtilities::getPrereqDataContainer(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const QString& name, bool createIfNotExists)
{
  DataContainerShPtr dc = getDataContainer(name);
  if(nullptr == dc.get() && !createIfNotExists)
  {
    if(filter != nullptr)
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
DataContainerShPtr DataStructureUtilities::createNonPrereqDataContainer(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const DataArrayPath& dap, RenameDataPath::DataID_t id)
{
  return createNonPrereqDataContainer(filter, dap.getDataContainerName(), id);
}

// -----------------------------------------------------------------------------
DataContainerShPtr DataStructureUtilities::createNonPrereqDataContainer(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const QString& dataContainerName, RenameDataPath::DataID_t id)
{
  if(dataContainerName.isEmpty())
  {
    if(filter != nullptr)
    {
      QString ss = QObject::tr("The DataContainer Object must have a name to be created.");
      filter->setErrorCondition(-887, ss);
      return DataContainer::NullPointer();
    }
  }

  if(dataContainerName.contains('/'))
  {
    if(filter != nullptr)
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
    if(filter != nullptr)
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
AttributeMatrixShPtrType DataStructureUtilities::getPrereqAttributeMatrixFromPath(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const DataArrayPath& path, int err)
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
bool DataStructureUtilities::validateNumberOfTuples(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const std::vector<DataArrayPath>& paths)
{
  if(paths.size() <= 1)
  {
    return false;
  }
  QVector<IDataArray::Pointer> dataArrays;
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
  IDataArray::Pointer array0 = getPrereqIDataArrayFromPath(filter, paths.at(0));
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
    IDataArray::Pointer nextArray = getPrereqIDataArrayFromPath(filter, paths.at(i));
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
bool DataStructureUtilities::validateNumberOfTuples(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, std::vector<IDataArray::Pointer>& dataArrays)
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
