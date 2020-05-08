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
#pragma once

#include <memory>

#include <cstddef> // for nullptr

#include <QtCore/QObject> // for Q_OBJECT
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/NamedCollection.hpp"
#include "SIMPLib/DataContainers/IDataContainerBundle.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/IDataStructureContainerNode.hpp"
#include "SIMPLib/DataContainers/RenameDataPath.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/Montages/AbstractMontage.h"
#include "SIMPLib/Montages/GridMontage.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class DataContainer;
using DataContainerShPtr = std::shared_ptr<DataContainer>;

class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

class GridMontage;
using GridMontageShPtr = std::shared_ptr<GridMontage>;

/**
 * @class DataContainerArray DataContainerArray.h DREAM3DLib/Common/DataContainerArray.h
 * @brief  This class holds the list of filters that will be run. This should be
 * the class (or a subclass) that you use to build up a pipeline in order to run
 * it.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 28, 2011
 * @version 1.0
 */
class SIMPLib_EXPORT DataContainerArray : public QObject, public IDataStructureContainerNode<DataContainer>
{
  Q_OBJECT
  // clang-format off
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(DataContainerArray)
  PYB11_SHARED_POINTERS(DataContainerArray)
  PYB11_STATIC_NEW_MACRO(DataContainerArray)
  PYB11_METHOD(bool addOrReplaceDataContainer ARGS DataContainer)
  PYB11_METHOD(bool insertOrAssign ARGS DataContainer)
  PYB11_METHOD(DataContainer::Pointer getDataContainer OVERLOAD const.QString.&,Name CONST_METHOD)
  PYB11_METHOD(DataContainer::Pointer getDataContainer OVERLOAD const.DataArrayPath.&,Path CONST_METHOD)
  PYB11_METHOD(bool doesDataContainerExist OVERLOAD const.QString.&,Name CONST_METHOD)
  PYB11_METHOD(bool doesDataContainerExist OVERLOAD const.DataArrayPath.&,Path CONST_METHOD)
  PYB11_METHOD(DataContainer::Pointer removeDataContainer ARGS Name)
  PYB11_METHOD(bool renameDataContainer OVERLOAD const.QString.&,OldName const.QString.&,NewName)
  PYB11_METHOD(bool renameDataContainer OVERLOAD const.DataArrayPath.&,OldPath const.DataArrayPath.&,NewPath )
  PYB11_METHOD(void clearDataContainers)
  PYB11_METHOD(int getNumDataContainers)
  PYB11_METHOD(void duplicateDataContainer ARGS OldName, NewName)
  PYB11_METHOD(AttributeMatrix::Pointer getAttributeMatrix ARGS DataArrayPath)
  PYB11_METHOD(bool doesAttributeMatrixExist ARGS DataArrayPath)
  PYB11_METHOD(bool doesAttributeArrayExist ARGS DataArrayPath)
  PYB11_END_BINDINGS()
  // End Python bindings declarations
  // clang-format on

public:
  using Self = DataContainerArray;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for DataContainerArray
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for DataContainerArray
   */
  static QString ClassName();

  using Container = ChildCollection;
  using MontageCollection = NamedCollection<AbstractMontage>;

  ~DataContainerArray() override;

  /**
   * @brief Creates and returns an empty DataArrayPath
   * @return
   */
  DataArrayPath getDataArrayPath() const override;

  /**
   * @brief
   */
  bool addOrReplaceDataContainer(const DataContainerShPtr& f)
  {
    return insertOrAssign(f);
  }

  /**
   * @brief getDataContainer
   * @param path Uses the DataContainerName from the DataArrayPath to return a data container
   * @return
   */
  virtual DataContainerShPtr getDataContainer(const DataArrayPath& path) const;

  /**
   * @brief getDataContainer
   * @param name
   * @return
   */
  virtual DataContainerShPtr getDataContainer(const QString& name) const;

  /**
   * @brief getDataContainers
   * @return
   */
  Container getDataContainers() const;

  /**
   * @brief Returns if a DataContainer with the give name is in the array
   * @param name The name of the DataContiner to find
   * @return
   */
  virtual bool doesDataContainerExist(const QString& name) const;

  /**
   * @brief doesDataContainerExist
   * @param dap
   * @return
   */
  virtual bool doesDataContainerExist(const DataArrayPath& dap) const;

  /**
   * @brief DataContainerArray::doesAttributeMatrixExist
   * @param path
   * @return
   */
  virtual bool doesAttributeMatrixExist(const DataArrayPath& path) const;

  /**
   * @brief doesAttributeArrayExist
   * @param path
   * @return
   */
  virtual bool doesAttributeArrayExist(const DataArrayPath& path) const;

  /**
   * @brief
   */
  virtual DataContainerShPtr removeDataContainer(const QString& name);

  /**
   * @brief renameDataContainer
   * @param oldName
   * @param newName
   * @return
   */
  bool renameDataContainer(const QString& oldName, const QString& newName);

  /**
   * @brief renameDataContainer
   * @param oldName
   * @param newName
   * @return
   */
  bool renameDataContainer(const DataArrayPath& oldName, const DataArrayPath& newName);

  /**
   * @brief Removes all DataContainers from this DataContainerArray
   */
  virtual void clearDataContainers();

  /**
   * @brief getDataContainerNames
   * @return
   */
  NameList getDataContainerNames() const;

  /**
   * @brief Returns the number of DataContainers
   * @return
   */
  virtual int getNumDataContainers() const;

  /**
   * @brief duplicateDataContainer
   * @param name
   * @param newName
   */
  virtual void duplicateDataContainer(const QString& name, const QString& newName);

  //////////////////////  AttributeMatrix Functions //////////////////////////
  /**
   * @brief getAttributeMatrix
   * @param path
   * @return
   */
  virtual AttributeMatrix::Pointer getAttributeMatrix(const DataArrayPath& path) const;

  /**
   * @brief printDataContainerNames
   * @param out
   */
  virtual void printDataContainerNames(QTextStream& out) const;

  /**
   * @brief Reads desired the DataContainers from HDF5 file
   * @param preflight
   * @param dcaGid
   * @param dcaProxy
   * @param obs
   * @return
   */
  virtual int readDataContainersFromHDF5(bool preflight, hid_t dcaGid, DataContainerArrayProxy& dcaProxy, Observable* obs = nullptr);

  /**
   * @brief setDataContainerBundles
   * @param bundles
   */
  void setDataContainerBundles(QMap<QString, IDataContainerBundle::Pointer> bundles);

  /**
   * @brief getDataContainerBundles
   * @return
   */
  QMap<QString, IDataContainerBundle::Pointer>& getDataContainerBundles();

  /**
   * @brief getDataContainerBundle
   * @param name
   * @return
   */
  IDataContainerBundle::Pointer getDataContainerBundle(const QString& name) const;

  template <typename BundleType>
  typename BundleType::Pointer getDataContainerBundleAs(const QString& name) const
  {
    typename BundleType::Pointer dcb = std::dynamic_pointer_cast<BundleType>(getDataContainerBundle(name));
    return dcb;
  }

  /**
   * @brief addDataContainerBundle
   * @param dataContainer
   */
  void addDataContainerBundle(IDataContainerBundle::Pointer dataContainerBundle);

  /**
   * @brief removeDataContainerBundle
   * @param name
   * @return
   */
  int removeDataContainerBundle(const QString& name);

  /**
   * @brief renameDataContainerBundle
   * @param oldName
   * @param newName
   */
  bool renameDataContainerBundle(const QString& oldName, const QString& newName);

  /**
   * @brief removeDataContainerFromBundle
   * @param name
   */
  void removeDataContainerFromBundles(const QString& name);

  /**
   * @brief Returns a copy of the Montage collection.
   * @return
   */
  MontageCollection getMontageCollection() const;

  /**
   * @brief Attempts to add the given montage to the collection.
   * This will fail if a montage by the same name already exists in the collection.
   * Returns true if it succeded.  Returns false otherwise.
   * @param montage
   * @return
   */
  bool addMontage(const AbstractMontageShPtr& montage);

  /**
   * @brief Adds the given montage to the collection if no montages of the same name exist yet.
   * Otherwise, the current montage is replaced with the one provided.
   * @param montage
   */
  void addOrReplaceMontage(const AbstractMontageShPtr& montage);

  /**
   * @brief Removes any montage of the given name from the collection.
   * @param name
   */
  void removeMontage(const QString& name);

  /**
   * @brief Finds and returns a shared pointer to a montage with the given name.
   * If none are found, this returns nullptr.
   * @param name
   * @return
   */
  AbstractMontageShPtr getMontage(const QString& name) const;

  /**
   * @brief Returns a list of names for all montages in the collection.
   * @return
   */
  QStringList getMontageNames() const;

  /**
   * @brief createNonPrereqGridMontage
   * @param filter
   * @param montageName
   * @param size
   * @param dcNames
   * @param collectionType
   * @return
   */
  GridMontageShPtr createNonPrereqGridMontage(AbstractFilter* filter, const QString& montageName, SizeVec3Type size, const QStringList& dcNames = QStringList(),
                                              GridMontage::CollectionMethod collectionMethod = GridMontage::CollectionMethod::CombOrder);

  /**
   * @brief renameDataArrayPaths
   * @param renamePaths
   */
  void renameDataArrayPaths(DataArrayPath::RenameContainer renamePaths);

  DataContainerShPtr getPrereqDataContainer(AbstractFilter* filter, const DataArrayPath& dap, bool createIfNotExists = false);

  /**
   * @brief getPrereqMontage
   * @param filter
   * @param name
   */
  AbstractMontageShPtr getPrereqMontage(AbstractFilter* filter, const QString& name);

  /**
   * @brief getPrereqDataContainer
   * @param name
   * @param createIfNotExists
   * @return
   */
  DataContainerShPtr getPrereqDataContainer(AbstractFilter* filter, const QString& name, bool createIfNotExists = false);

  /**
   * @brief createNonPrereqDataContainer
   * @param filter
   * @param dap
   * @param id
   * @return
   */
  DataContainerShPtr createNonPrereqDataContainer(AbstractFilter* filter, const DataArrayPath& dap, RenameDataPath::DataID_t id = RenameDataPath::k_Invalid_ID);

  /**
   * @brief This function will create a new DataContainer of type <T>
   * @param dataContainerName The name of the DataContainer. Must not be empty or this method will ASSERT()
   * @return Valid DataContainer Object UNLESS the DataContainer with the given name already exists or the
   * dataContainerName is empty in which case a Null DataContainer will be returned.
   */
  DataContainerShPtr createNonPrereqDataContainer(AbstractFilter* filter, const QString& dataContainerName, RenameDataPath::DataID_t id = RenameDataPath::k_Invalid_ID);

  /**
   * @brief getPrereqGeometryFromDataContainer Returns an IGeometry object of the templated type
   * if it is available for the given DataContainer
   * @param filter
   * @param dcName
   * @return
   */
  template <typename GeometryType>
  typename GeometryType::Pointer getPrereqGeometryFromDataContainer(AbstractFilter* filter, const QString& dcName)
  {
    typename GeometryType::Pointer geom = GeometryType::NullPointer();
    DataContainerShPtr dc = getPrereqDataContainer(filter, dcName, false);
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
  typename GeometryType::Pointer getPrereqGeometryFromDataContainer(AbstractFilter* filter, const DataArrayPath& path)
  {
    typename GeometryType::Pointer geom = GeometryType::NullPointer();
    DataContainerShPtr dc = getPrereqDataContainer(filter, path.getDataContainerName(), false);
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
  AttributeMatrix::Pointer getPrereqAttributeMatrixFromPath(AbstractFilter* filter, const DataArrayPath& path, int err);

  /**
   * @brief getPrereqArrayFromPath
   * @param filter Instance of an AbstractFilter. Can be nullptr
   * @param path The path to the IDataArray
   * @param cDims The component dimensions of the IDataArray subclass
   * @return Valid or nullptr shared pointer based on availability of the array
   */
  template <class ArrayType>
  typename ArrayType::Pointer getPrereqArrayFromPath(AbstractFilter* filter, const DataArrayPath& path, const std::vector<size_t>& cDims = {}) const
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

    DataContainerShPtr dc = getDataContainer(dcName);
    if(nullptr == dc.get())
    {
      if(filter)
      {
        ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
        filter->setErrorCondition(-80002, ss);
      }
      return dataArray;
    }

    AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(amName);
    if(nullptr == attrMat.get())
    {
      if(filter)
      {
        ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
        filter->setErrorCondition(-80003, ss);
      }
      return dataArray;
    }

    dataArray = attrMat->getPrereqArray<ArrayType>(filter, daName, -80002, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    return dataArray;
  }

  /**
   * @brief getPrereqIDataArrayFromPath
   * @param filter
   * @param path
   * @return
   */
  IDataArray::Pointer getPrereqIDataArrayFromPath(AbstractFilter* filter, const DataArrayPath& path) const;

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
  template <class ArrayType>
  typename ArrayType::Pointer createNonPrereqArrayFromPath(AbstractFilter* filter, const DataArrayPath& path, typename ArrayType::value_type initValue, const std::vector<size_t>& compDims,
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

    DataContainerShPtr dc = getDataContainer(path.getDataContainerName());
    if(nullptr == dc.get())
    {
      if(filter)
      {
        ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(path.getDataContainerName());
        filter->setErrorCondition(-80002, ss);
      }
      return dataArray;
    }

    AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(path.getAttributeMatrixName());
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
    dataArray = attrMat->createNonPrereqArray<ArrayType>(filter, path.getDataArrayName(), initValue, compDims, id);
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
  bool validateNumberOfTuples(AbstractFilter* filter, const QVector<DataArrayPath>& paths) const;

  /**
   * @brief validateNumberOfTuples This method will validate that all of the DataArray
   * objects supplied are non-nullptr and that all are have the same number of tuples.
   * It will return false if and any of the checks fail, or
   * if the QVector of input DataArray objects has 0 or 1 element.
   * @param filter The filter calling the validation
   * @param paths The paths that should be checked
   * @return bool Validation check
   */
  bool validateNumberOfTuples(AbstractFilter* filter, QVector<IDataArrayShPtrType> dataArrays) const;

  /**
   * @brief deepCopy
   * @param dca
   * @return
   */
  DataContainerArray::Pointer deepCopy(bool forceNoAllocate = false) const;

protected:
  DataContainerArray();

private:
  QMap<QString, IDataContainerBundle::Pointer> m_DataContainerBundles;
  MontageCollection m_MontageCollection;

  /**
   * @brief setMontageTileFromDataContainerName
   * @param row
   * @param col
   * @param depth
   * @param dcName
   */
  void setMontageTileFromDataContainerName(AbstractFilter* filter, size_t row, size_t col, size_t depth, const GridMontage::Pointer& montage, const QString& dcName);

public:
  DataContainerArray(const DataContainerArray&) = delete;            // Copy Constructor Not Implemented
  DataContainerArray(DataContainerArray&&) = delete;                 // Move Constructor Not Implemented
  DataContainerArray& operator=(const DataContainerArray&) = delete; // Copy Assignment Not Implemented
  DataContainerArray& operator=(DataContainerArray&&) = delete;      // Move Assignment Not Implemented
};
Q_DECLARE_METATYPE(DataContainerArray::Pointer);
