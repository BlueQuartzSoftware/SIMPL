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
#pragma once

#include <memory>
#include <vector>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/SIMPLib.h"

class DataContainerArray;
using DataContainerArrayShPtrType = std::shared_ptr<DataContainerArray>;

class DataContainer;
using DataContainerShPtrType = std::shared_ptr<DataContainer>;

class AttributeMatrix;
using AttributeMatrixShPtrType = std::shared_ptr<AttributeMatrix>;

class AbstractFilter;

class SIMPLib_EXPORT DataStructureUtilities
{
public:
  DataStructureUtilities();
  ~DataStructureUtilities();

  /**
   * @brief getPrereqDataContainer
   * @param filter
   * @param dap
   * @param createIfNotExists
   * @return
   */
  ADataContainerShPtrType getPrereqDataContainer(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const DataArrayPath& dap, bool createIfNotExists = false);

  /**
   * @brief getPrereqDataContainer
   * @param name
   * @param createIfNotExists
   * @return
   */
  ADataContainerShPtrType getPrereqDataContainer(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const QString& name, bool createIfNotExists = false);

  /**
   * @brief createNonPrereqDataContainer
   * @param filter
   * @param dap
   * @param id
   * @return
   */
  ADataContainerShPtrType createNonPrereqDataContainer(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const DataArrayPath& dap,
                                                       RenameDataPath::DataID_t id = RenameDataPath::k_Invalid_ID);

  /**
   * @brief This function will create a new DataContainer of type <T>
   * @param dataContainerName The name of the DataContainer. Must not be empty or this method will ASSERT()
   * @return Valid DataContainer Object UNLESS the DataContainer with the given name already exists or the
   * dataContainerName is empty in which case a Null DataContainer will be returned.
   */
  ADataContainerShPtrType createNonPrereqDataContainer(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const QString& dataContainerName,
                                                       RenameDataPath::DataID_t id = RenameDataPath::k_Invalid_ID);

  /**
   * @brief getPrereqGeometryFromDataContainer Returns an IGeometry object of the templated type
   * if it is available for the given DataContainer
   * @param filter
   * @param dcName
   * @return
   */
  template <typename GeometryType>
  typename GeometryType::Pointer getPrereqGeometryFromDataContainer(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const QString& dcName)
  {
    typename GeometryType::Pointer geom = GeometryType::NullPointer();
    ADataContainerShPtrType dc = DataStructureUtilites::getPrereqDataContainer(filter, dcName, false);
    if(nullptr == dc)
    {
      return geom;
    }

    return dc->getPrereqGeometry<GeometryType>(filter);
  }

  /**
   * @brief getPrereqGeometryFromDataContainer Returns an IGeometry object of the templated type
   * if it is available for the given DataContainer
   * @param filter
   * @param path
   * @return
   */
  template <typename GeometryType>
  typename GeometryType::Pointer getPrereqGeometryFromDataContainer(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const DataArrayPath& path)
  {
    typename GeometryType::Pointer geom = GeometryType::NullPointer();
    ADataContainerShPtrType dc = DataStructureUtilites::getPrereqDataContainer(filter, path.getDataContainerName(), false);
    if(nullptr == dc)
    {
      return geom;
    }

    return dc->getPrereqGeometry<GeometryType>(filter);
  }

  /**
   * @brief getPrereqAttributeMatrixFromPath This function will return an AttributeMatrix if it is availabe
   * at the path
   * @param filter An AbstractFilter or subclass where error messages and error codes can be sent
   * @param path The DataArrayPath object that has the path to the AttributeMatrix
   * @param err The error code to display to the user
   * @return
   */
  AttributeMatrixShPtrType getPrereqAttributeMatrixFromPath(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const DataArrayPath& path, int err);

  /**
   * @brief getPrereqArrayFromPath
   * @param filter Instance of an AbstractFilter. Can be nullptr
   * @param path The path to the IDataArray
   * @param cDims The component dimensions of the IDataArray subclass
   * @return Valid or nullptr shared pointer based on availability of the array
   */
  template <class ArrayType, class Filter>
  typename ArrayType::Pointer getPrereqArrayFromPath(const DataContainerArrayShPtrType& dca, Filter* filter, const DataArrayPath& path, QVector<size_t> cDims)
  {

    QString ss;
    typename ArrayType::Pointer dataArray = ArrayType::NullPointer();

    if(path.isEmpty())
    {
      if(filter)
      {
        ss = QObject::tr("DataContainerArray::getPrereqArrayFromPath Error at line %1. The DataArrayPath object was empty").arg(__LINE__);
        filter->setErrorCondition(-80000, ss);
      }
      return dataArray;
    }

    if(!path.isValid())
    {
      if(filter)
      {
        ss = QObject::tr("DataContainerArray::getPrereqArrayFromPath Error at line %1. The DataArrayPath object was not valid meaning one of the strings in the object is empty. The path is %2")
                 .arg(__LINE__)
                 .arg(path.serialize());
        filter->setErrorCondition(-80001, ss);
      }
      return dataArray;
    }

    QString dcName = path.getDataContainerName();
    QString amName = path.getAttributeMatrixName();
    QString daName = path.getDataArrayName();

    ADataContainerShPtrType dc = dca->getDataContainer(dcName);
    if(nullptr == dc.get())
    {
      if(filter)
      {
        ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
        filter->setErrorCondition(-80002, ss);
      }
      return dataArray;
    }

    AttributeMatrixShPtrType attrMat = dc->getAttributeMatrix(amName);
    if(nullptr == attrMat.get())
    {
      if(filter)
      {
        ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
        filter->setErrorCondition(-80003, ss);
      }
      return dataArray;
    }

    dataArray = attrMat->getPrereqArray<ArrayType, Filter>(filter, daName, -80002, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    return dataArray;
  }

  /**
   * @brief getPrereqIDataArrayFromPath
   * @param filter
   * @param path
   * @return
   */
  template <class ArrayType, class Filter>
  typename ArrayType::Pointer getPrereqIDataArrayFromPath(const DataContainerArrayShPtrType& dca, Filter* filter, const DataArrayPath& path)
  {

    QString ss;
    typename ArrayType::Pointer dataArray = ArrayType::NullPointer();

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

    ADataContainerShPtrType dc = getDataContainer(dcName);
    if(nullptr == dc.get())
    {
      if(filter)
      {
        ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
        filter->setErrorCondition(-999, ss);
      }
      return dataArray;
    }

    AttributeMatrixShPtrType attrMat = dc->getAttributeMatrix(amName);
    if(nullptr == attrMat.get())
    {
      if(filter)
      {
        ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
        filter->setErrorCondition(-307020, ss);
      }
      return dataArray;
    }

    dataArray = std::dynamic_pointer_cast<ArrayType>(attrMat->getPrereqIDataArray(filter, daName, -90002)); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    return dataArray;
  }

  /**
   * @brief createNonPrereqArray This method will create a new DataArray in the AttributeMatrix. The conditions for this
   * method to work properly include: a valid DataArrayPath is supplied, the name of the attribute array is not empty,
   * and an array with the same name cannot already exist
   * @param filter The instance of the filter the filter that is requesting the new array
   * @param attributeArrayName The name of the AttributeArray to create
   * @param initValue The initial value of all the elements of the array
   * @param size The number of tuples in the Array
   * @param dims The dimensions of the components of the AttributeArray
   * @return A Shared Pointer to the newly created array
   */
  template <class ArrayType, class Filter, typename T>
  typename ArrayType::Pointer createNonPrereqArrayFromPath(const DataContainerArrayShPtrType& dca, Filter* filter, const DataArrayPath& path, T initValue, QVector<size_t> compDims,
                                                           const QString& property = "", RenameDataPath::DataID_t id = RenameDataPath::k_Invalid_ID)
  {
    typename ArrayType::Pointer dataArray = ArrayType::NullPointer();
    QString ss;
    if(!path.isValid())
    {
      if(filter)
      {
        ss = QObject::tr("Property '%1': The DataArrayPath is invalid because one of the elements was empty.\n  DataContainer: %2\n  AttributeMatrix: %3\n  DataArray: %4")
                 .arg(property)
                 .arg(path.getDataContainerName())
                 .arg(path.getAttributeMatrixName())
                 .arg(path.getDataArrayName());
        filter->setErrorCondition(-80010, ss);
      }
      return dataArray;
    }

    if(path.getDataContainerName().contains('/'))
    {
      if(filter)
      {
        ss = QObject::tr("The DataContainer '%1' has forward slashes in its name").arg(path.getDataContainerName());
        filter->setErrorCondition(-80005, ss);
      }
      return dataArray;
    }

    if(path.getAttributeMatrixName().contains('/'))
    {
      if(filter)
      {
        ss = QObject::tr("The AttributeMatrix '%1' has forward slashes in its name").arg(path.getAttributeMatrixName());
        filter->setErrorCondition(-80006, ss);
      }
      return dataArray;
    }

    if(path.getDataArrayName().contains('/'))
    {
      if(filter)
      {
        ss = QObject::tr("The DataArray '%1' has forward slashes in its name").arg(path.getDataArrayName());
        filter->setErrorCondition(-80007, ss);
      }
      return dataArray;
    }

    if(path.getDataContainerName().contains('/'))
    {
      if(filter)
      {
        ss = QObject::tr("The DataContainer '%1' has forward slashes in its name").arg(path.getDataContainerName());
        filter->setErrorCondition(-80004, ss);
      }
      return dataArray;
    }

    ADataContainerShPtrType dc = getDataContainer(path.getDataContainerName());
    if(nullptr == dc.get())
    {
      if(filter)
      {
        ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(path.getDataContainerName());
        filter->setErrorCondition(-80002, ss);
      }
      return dataArray;
    }

    AttributeMatrixShPtrType attrMat = dc->getAttributeMatrix(path.getAttributeMatrixName());
    if(nullptr == attrMat.get())
    {
      if(filter)
      {
        ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(path.getAttributeMatrixName()).arg(path.getDataContainerName());
        filter->setErrorCondition(-80003, ss);
      }
      return dataArray;
    }

    // If something goes wrong at this point the error message will be directly set in the 'filter' object so we just
    // simply return what ever is given to us.
    dataArray = attrMat->createNonPrereqArray<ArrayType, Filter, T>(filter, path.getDataArrayName(), initValue, compDims, id);
    return dataArray;
  }

  /**
   * @brief validateNumberOfTuples This method will validate that all of the DataArray
   * paths supplied are valid, return non-nullptr DataArray pointers, and that all have the
   * same number of tuples.  It will return false if and any of the checks fail, or
   * if the QVector of input paths has 0 or 1 element.
   * @param filter The filter calling the validation
   * @param paths The paths that should be checked
   * @return bool Validation check
   */
  bool validateNumberOfTuples(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const std::vector<DataArrayPath>& paths);

  /**
   * @brief validateNumberOfTuples This method will validate that all of the DataArray
   * objects supplied are non-nullptr and that all are have the same number of tuples.
   * It will return false if and any of the checks fail, or
   * if the QVector of input DataArray objects has 0 or 1 element.
   * @param filter The filter calling the validation
   * @param paths The paths that should be checked
   * @return bool Validation check
   */
  bool validateNumberOfTuples(const DataContainerArrayShPtrType& dca, AbstractFilter* filter, const std::vector<IDataArray::Pointer>& dataArrays);
};
