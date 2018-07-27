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

#include <string>
#include <vector>

#include <QtCore/QString>

#include "H5Support/H5Lite.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @class StringDataArray StringDataArray.h DREAM3DLib/Common/StringDataArray.h
 * @brief Stores an array of QString objects
 *
 * @date Nov 13, 2012
 * @version 1.0
 */
class SIMPLib_EXPORT StringDataArray : public IDataArray
{
public:
  SIMPL_SHARED_POINTERS(StringDataArray)
  SIMPL_TYPE_MACRO_SUPER(StringDataArray, IDataArray)
  SIMPL_CLASS_VERSION(2)

  /**
   * @brief CreateArray
   * @param numTuples
   * @param name
   * @param allocate
   * @return
   */
  static Pointer CreateArray(size_t numTuples, const QString& name, bool allocate = true);

  /**
   * @brief CreateArray
   * @param numTuples
   * @param compDims
   * @param name
   * @param allocate
   * @return
   */
  static Pointer CreateArray(size_t numTuples, QVector<size_t> compDims, const QString& name, bool allocate = true);
  /**
   * @brief createNewArray
   * @param numElements
   * @param numComponents NOT USED. It is always 1.
   * @param name
   * @return
   */
  virtual IDataArray::Pointer createNewArray(size_t numElements, int rank, size_t* dims, const QString& name, bool allocate = true);

  /**
   * @brief createNewArray
   * @param numElements
   * @param dims
   * @param name
   * @param allocate
   * @return
   */
  virtual IDataArray::Pointer createNewArray(size_t numElements, std::vector<size_t> dims, const QString& name, bool allocate = true);

  /**
   * @brief createNewArray
   * @param numElements
   * @param dims
   * @param name
   * @param allocate
   * @return
   */
  virtual IDataArray::Pointer createNewArray(size_t numElements, QVector<size_t> dims, const QString& name, bool allocate = true);

  /**
   * @brief ~StringDataArray
   */
  virtual ~StringDataArray();

  /**
   * @brief isAllocated
   * @return
   */
  virtual bool isAllocated();

  /**
   * @brief Gives this array a human readable name
   * @param name The name of this array
   */
  virtual void setInitValue(const std::string& initValue);

  /**
   * @brief Gives this array a human readable name
   * @param name The name of this array
   */
  virtual void setInitValue(const QString& initValue);

  /**
   * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
   * can be a primitive like char, float, int or the name of a class.
   * @return
   */
  void getXdmfTypeAndSize(QString& xdmfTypeName, int& precision);
  /**
   * @brief getTypeAsString
   * @return
   */
  virtual QString getTypeAsString();

  /**
   * @brief Gives this array a human readable name
   * @param name The name of this array
   */
  void setName(const QString& name);

  /**
   * @brief Returns the human readable name of this array
   * @return
   */
  QString getName();

  /**
   * @brief
   */
  virtual void takeOwnership();

  /**
   * @brief
   */
  virtual void releaseOwnership();
  /**
   * @brief Returns a void pointer pointing to the index of the array. nullptr
   * pointers are entirely possible. No checks are performed to make sure
   * the index is with in the range of the internal data array.
   * @param i The index to have the returned pointer pointing to.
   * @return Void Pointer. Possibly nullptr.
   */
  virtual void* getVoidPointer(size_t i);

  /**
   * @brief Returns the number of Tuples in the array.
   */
  virtual size_t getNumberOfTuples();

  /**
   * @brief Return the number of elements in the array
   * @return
   */
  virtual size_t getSize();

  virtual int getNumberOfComponents();

  QVector<size_t> getComponentDimensions();

  // Description:
  // Set/Get the dimension (n) of the rank. Must be >= 1. Make sure that
  // this is set before allocation.
  void SetRank(int rnk);

  /**
   * @brief getRank
   * @return
   */
  int getRank();

  /**
   * @brief Returns the number of bytes that make up the data type.
   * 1 = char
   * 2 = 16 bit integer
   * 4 = 32 bit integer/Float
   * 8 = 64 bit integer/Double
   */
  virtual size_t getTypeSize();

  /**
   * @brief Removes Tuples from the Array. If the size of the vector is Zero nothing is done. If the size of the
   * vector is greater than or Equal to the number of Tuples then the Array is Resized to Zero. If there are
   * indices that are larger than the size of the original (before erasing operations) then an error code (-100) is
   * returned from the program.
   * @param idxs The indices to remove
   * @return error code.
   */
  virtual int eraseTuples(QVector<size_t>& idxs);

  /**
   * @brief Copies a Tuple from one position to another.
   * @param currentPos The index of the source data
   * @param newPos The destination index to place the copied data
   * @return
   */
  virtual int copyTuple(size_t currentPos, size_t newPos);

  // This line must be here, because we are overloading the copyData pure virtual function in IDataArray.
  // This is required so that other classes can call this version of copyData from the subclasses.
  using IDataArray::copyFromArray;

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
  bool copyFromArray(size_t destTupleOffset, IDataArray::Pointer sourceArray, size_t srcTupleOffset, size_t totalSrcTuples);

  /**
   * @brief Does Nothing
   * @param pos The index of the Tuple
   * @param value pointer to value
   */
  virtual void initializeTuple(size_t pos, void* value);

  /**
   * @brief Sets all the values to empty string.
   */
  virtual void initializeWithZeros();

  /**
   * @brief initializeWithValue
   * @param value
   */
  virtual void initializeWithValue(QString value);

  /**
   * @brief initializeWithValue
   * @param value
   */
  virtual void initializeWithValue(const std::string& value);

  /**
   * @brief deepCopy
   * @param forceNoAllocate
   * @return
   */
  virtual IDataArray::Pointer deepCopy(bool forceNoAllocate = false);

  /**
   * @brief Reseizes the internal array
   * @param size The new size of the internal array
   * @return 1 on success, 0 on failure
   */
  virtual int32_t resizeTotalElements(size_t size);

  /**
   * @brief Reseizes the internal array
   * @param size The new size of the internal array
   * @return 1 on success, 0 on failure
   */
  virtual int32_t resize(size_t numTuples);

  /**
   * @brief Initializes this class to zero bytes freeing any data that it currently owns
   */
  virtual void initialize();

  /**
   * @brief printTuple
   * @param out
   * @param i
   * @param delimiter
   */
  virtual void printTuple(QTextStream& out, size_t i, char delimiter = ',');

  /**
   * @brief printComponent
   * @param out
   * @param i
   * @param j
   */
  virtual void printComponent(QTextStream& out, size_t i, int j);

  /**
   * @brief getFullNameOfClass
   * @return
   */
  QString getFullNameOfClass();

  /**
   *
   * @param parentId
   * @return
   */
  virtual int writeH5Data(hid_t parentId, QVector<size_t> tDims);

  /**
   * @brief writeXdmfAttribute
   * @param out
   * @param volDims
   * @param hdfFileName
   * @param groupPath
   * @return
   */
  virtual int writeXdmfAttribute(QTextStream& out, int64_t* volDims, const QString& hdfFileName, const QString& groupPath, const QString& labelb);

  /**
   * @brief getInfoString
   * @return Returns a formatted string that contains general infomation about
   * the instance of the object.
   */
  virtual QString getInfoString(SIMPL::InfoStringFormat format);

  /**
   * @brief readH5Data
   * @param parentId
   * @return
   */
  virtual int readH5Data(hid_t parentId);

  /**
   * @brief setValue
   * @param i
   * @param value
   */
  void setValue(size_t i, const QString& value);

  /**
   * @brief getValue
   * @param i
   * @return
   */
  QString getValue(size_t i);

protected:
  /**
   * @brief Protected Constructor
   * @param numElements The number of elements in the internal array.
   * @param takeOwnership Will the class clean up the memory. Default=true
   */
  StringDataArray(size_t numTuples, const QString name, bool allocate = true);

  StringDataArray();

private:
  QString m_Name;
  QString m_InitValue;
  std::vector<QString> m_Array;
  bool _ownsData;

public:
  StringDataArray(const StringDataArray&) = delete;            // Copy Constructor Not Implemented
  StringDataArray(StringDataArray&&) = delete;                 // Move Constructor Not Implemented
  StringDataArray& operator=(const StringDataArray&) = delete; // Copy Assignment Not Implemented
  StringDataArray& operator=(StringDataArray&&) = delete;      // Move Assignment Not Implemented
};
