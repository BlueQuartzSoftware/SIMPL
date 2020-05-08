///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, mjackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

//-- C++
#include <memory>
#include <vector>

#include <hdf5.h>

//--Qt Includes
#include <QtCore/QString>
#include <QtCore/QtDebug>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/IDataStructureNode.h"
#include "SIMPLib/Utilities/ToolTipGenerator.h"

class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

/**
 * @class IDataArray IDataArray.h PathToHeader/IDataArray.h
 * @brief This class holds a raw pointer to some allocated data that can be stored
 * into or read from an HDF5 data file.
 * The class design was borrowed heavily from the vtkDataArray class from www.vtk.org.
 * The work was all performed by those individuals. I have merely changed a few
 * methods to meet my specific needs.
 * @author mjackson
 * @date Jan 3, 2008
 * @version $Revision: 1.2 $
 */
class SIMPLib_EXPORT IDataArray : public IDataStructureNode
{

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(IDataArray)
  PYB11_SHARED_POINTERS(IDataArray)
  PYB11_PROPERTY(QString Name READ getName WRITE setName)
  PYB11_METHOD(QString getTypeAsString)
  PYB11_METHOD(std::vector<size_t> getComponentDimensions)
  PYB11_METHOD(size_t getNumberOfTuples)
  PYB11_METHOD(int32_t getNumberOfComponents)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = IDataArray;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for IDataArray
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for IDataArray
   */
  static QString ClassName();

  IDataArray(const QString& name = "");
  ~IDataArray() override;

  virtual Pointer createNewArray(size_t numElements, int32_t rank, const size_t* dims, const QString& name, bool allocate = true) const = 0;
  virtual Pointer createNewArray(size_t numElements, const std::vector<size_t>& dims, const QString& name, bool allocate = true) const = 0;
  // virtual Pointer createNewArray(size_t numElements, std::vector<size_t> dims, const QString& name, bool allocate = true) = 0;

  /**
   * @brief Creates and returns a DataArrayPath for the DataArray
   * @return
   */
  DataArrayPath getDataArrayPath() const override;

  virtual int32_t getClassVersion() const = 0;

  /**
   * @brief Has all the memory needed for this class been allocated?
   */
  virtual bool isAllocated() const = 0;

  /**
   * @brief Makes this class responsible for freeing the memory.
   */
  virtual void takeOwnership() = 0;

  /**
   * @brief This class will NOT free the memory associated with the internal pointer.
   * This can be useful if the user wishes to keep the data around after this
   * class goes out of scope.
   */
  virtual void releaseOwnership() = 0;

  /**
   * @brief Returns a void pointer pointing to the index of the array. nullptr
   * pointers are entirely possible. No checks are performed to make sure
   * the index is with in the range of the internal data array.
   * @param i The index to have the returned pointer pointing to.
   * @return Void Pointer. Possibly nullptr.
   */
  virtual void* getVoidPointer(size_t i) = 0;

  /**
   * @brief Returns the number of Tuples in the array.
   */
  virtual size_t getNumberOfTuples() const = 0;

  /**
   * @brief Return the number of elements in the array
   * @return
   */
  virtual size_t getSize() const = 0;

  virtual int32_t getNumberOfComponents() const = 0;
  virtual std::vector<size_t> getComponentDimensions() const = 0;

  /**
   * @brief Returns the number of bytes that make up the data type.
   * 1 = char
   * 2 = 16 bit integer
   * 4 = 32 bit integer/Float
   * 8 = 64 bit integer/Double
   */
  virtual size_t getTypeSize() const = 0;

  /**
   * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
   * can be a primitive like char, float, int or the name of a class.
   * @return
   */
  virtual void getXdmfTypeAndSize(QString& xdmfTypeName, int32_t& precision) const = 0;

  /**
   * @brief Erases tuples based on a list of specific Tuple indices
   * @param idxs The indices to erase
   * @return
   */
  virtual int32_t eraseTuples(const std::vector<size_t>& idxs) = 0;

  /**
   * @brief Copies a Tuple from one position to another.
   * @param currentPos The index of the source data
   * @param newPos The destination index to place the copied data
   * @return
   */
  virtual int32_t copyTuple(size_t currentPos, size_t newPos) = 0;

  /**
   * @brief copyData This method copies all data from the <b>sourceArray</b> into
   * the current array starting at the target destination tuple offset value.
   *
   * The method will check to ensure that various conditions are properly met
   * before attempting to copy the data into the array at the give offset to avoid
   * the possibility of walking off the end of the array into application memory
   * or causing a General protection fault by the OS.
   *
   * For example if the DataArray has 10 tuples and the destTupleOffset = 5 then
   * then source data will be copied into the destination array starting at
   * destination tuple 5. In psuedo code it would be the following:
   * @code
   *  destArray[5] = sourceArray[0];
   *  destArray[6] = sourceArray[1];
   *  .....
   * @endcode
   * @param destTupleOffset
   * @param sourceArray
   * @return
   */
  bool copyFromArray(size_t destTupleOffset, IDataArray::ConstPointer sourceArray);

  /**
   * @brief copyData This method copies the number of tuples specified by the
   * totalSrcTuples value starting from the source tuple offset value in <b>sourceArray</b>
   * into the current array starting at the target destination tuple offset value.
   *
   * For example if the DataArray has 10 tuples, the source DataArray has 10 tuples,
   *  the destTupleOffset = 5, the srcTupleOffset = 5, and the totalSrcTuples = 3,
   *  then tuples 5, 6, and 7 will be copied from the source into tuples 5, 6, and 7
   * of the destination array. In psuedo code it would be the following:
   * @code
   *  destArray[5] = sourceArray[5];
   *  destArray[6] = sourceArray[6];
   *  destArray[7] = sourceArray[7];
   *  .....
   * @endcode
   * @param destTupleOffset
   * @param sourceArray
   * @return
   */
  virtual bool copyFromArray(size_t destTupleOffset, IDataArray::ConstPointer sourceArray, size_t srcTupleOffset, size_t totalSrcTuples) = 0;

  /**
   * @brief Splats the same value c across all values in the Tuple
   * @param pos The index of the Tuple
   * @param value pointer to value
   */
  virtual void initializeTuple(size_t pos, const void* value) = 0;

  /**
   * @brief Sets all the values to zero.
   */
  virtual void initializeWithZeros() = 0;

  /**
   * @brief Resizes the internal array
   * @param size The new size of the internal array
   * @return 1 on success, 0 on failure
   */
  virtual int32_t resizeTotalElements(size_t size) = 0;

  /**
   * @brief resize
   * @param count
   */
  virtual void resizeTuples(size_t count) = 0;

  /**
   * @brief printTuple
   * @param out
   * @param i
   * @param delimiter
   */
  virtual void printTuple(QTextStream& out, size_t i, char delimiter = ',') const = 0;

  /**
   * @brief printComponent
   * @param out
   * @param i
   * @param j
   */
  virtual void printComponent(QTextStream& out, size_t i, int32_t j) const = 0;

  /**
   * @brief deepCopy
   * @param forceNoAllocate
   * @return
   */
  virtual IDataArray::Pointer deepCopy(bool forceNoAllocate = false) const = 0;

  /**
   * @brief writeH5Data
   * @param parentId
   * @param tDims
   * @return
   */
  virtual int32_t writeH5Data(hid_t parentId, const std::vector<size_t>& tDims) const = 0;

  /**
   * @brief readH5Data
   * @param parentId
   * @return
   */
  virtual int32_t readH5Data(hid_t parentId) = 0;

  /**
   * @brief writeXdmfAttribute
   * @param out
   * @param volDims
   * @param hdfFileName
   * @param groupPath
   * @param label
   * @return
   */
  virtual int32_t writeXdmfAttribute(QTextStream& out, const int64_t* volDims, const QString& hdfFileName, const QString& groupPath, const QString& label) const = 0;

  /**
   * @brief getTypeAsString
   * @return
   */
  virtual QString getTypeAsString() const = 0;

  /**
   * @brief getInfoString
   * @return Returns a formatted string that contains general infomation about
   * the instance of the object.
   */
  virtual QString getInfoString(SIMPL::InfoStringFormat format) const = 0;

  /**
   * @brief Returns a ToolTipGenerator for creating HTML tooltip tables
   * with values populated to match the current DataArray.
   * @return
   */
  virtual ToolTipGenerator getToolTipGenerator() const = 0;

protected:
private:
  IDataArray(const IDataArray&);     // Not Implemented
  void operator=(const IDataArray&); // Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
class CheckDataArrayType
{
public:
  CheckDataArrayType() = default;
  virtual ~CheckDataArrayType() = default;
  bool operator()(IDataArrayShPtrType p)
  {
    return (std::dynamic_pointer_cast<T>(p).get() != nullptr);
  }
};
