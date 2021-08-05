/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <QtCore/QString>

#include "H5Support/H5SupportTypeDefs.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/IDataArray.h"

/**
 * @class NeighborList NeighborList.hpp DREAM3DLib/Common/NeighborList.hpp
 * @brief Template class for wrapping raw arrays of data.
 * @author mjackson
 * @date July 3, 2008
 * @version 1.0
 */
template <typename T>
class NeighborList : public IDataArray
{
public:
  using Self = NeighborList<T>;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;

  static Pointer NullPointer();

  using value_type = T;

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName();

  /**
   * @brief Returns the version of this class.
   * @return
   */
  int32_t getClassVersion() const override;

  void setNumNeighborsArrayName(const QString& name);

  QString getNumNeighborsArrayName();

  static Pointer New();

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
   * @param name
   * @param allocate
   * @return
   */
  static Pointer CreateArray(size_t numTuples, const std::string& name, bool allocate = true);

  /**
   * @brief CreateArray
   * @param numTuples
   * @param rank
   * @param dims
   * @param name
   * @param allocate
   * @return
   */
  static Pointer CreateArray(size_t numTuples, int rank, const size_t* dims, const QString& name, bool allocate = true);

  /**
   * @brief CreateArray
   * @param numTuples
   * @param cDims
   * @param name
   * @param allocate
   * @return
   */
  static Pointer CreateArray(size_t numTuples, const std::vector<size_t>& cDims, const QString& name, bool allocate = true);

  /**
   * @brief CreateArray
   * @param tDims
   * @param cDims
   * @param name
   * @param allocate
   * @return
   */
  static Pointer CreateArray(const std::vector<size_t>& tDims, const std::vector<size_t>& cDims, const QString& name, bool allocate);

  /**
   * @brief createNewArray
   * @param numElements
   * @param rank
   * @param dims
   * @param name
   * @return
   */
  IDataArray::Pointer createNewArray(size_t numElements, int rank, const size_t* dims, const QString& name, bool allocate = true) const override;

  /**
   * @brief createNewArray
   * @param numElements
   * @param dims
   * @param name
   * @return
   */
  IDataArray::Pointer createNewArray(size_t numElements, const std::vector<size_t>& dims, const QString& name, bool allocate = true) const override;

  using VectorType = std::vector<T>;
  using SharedVectorType = std::shared_ptr<VectorType>;

  // -----------------------------------------------------------------------------
  ~NeighborList() override = default;

  /**
   * @brief isAllocated
   * @return
   */
  bool isAllocated() const override;

  /**
   * @brief Gives this array a human readable name
   * @param name The name of this array
   */
  virtual void setInitValue(T initValue);

  /**
   * @brief Sets all the values to value.
   */
  virtual void initializeWithValue(T initValue, size_t offset = 0);

  /**
   * @brief getXdmfTypeAndSize Returns the XDMF Type Name and precision.
   * @return Returns UNKNOWN and 0 Since NeighborLists are not written to XDMF
   */
  void getXdmfTypeAndSize(QString& xdmfTypeName, int& precision) const override;

  /**
   * @brief Returns the HDF Type for a given primitive value.
   * @param value A value to use. Can be anything. Just used to get the type info
   * from
   * @return The HDF5 native type for the value
   */
  QString getFullNameOfClass() const;

  /**
   * @brief getTypeAsString
   * @return
   */
  QString getTypeAsString() const override;

  /**
   * @brief takeOwnership
   */
  void takeOwnership() override;

  /**
   * @brief releaseOwnership
   */
  void releaseOwnership() override;

  /**
   * @brief getVoidPointer
   * @param i
   * @return
   */
  void* getVoidPointer(size_t i) override;

  /**
   * @brief Removes Tuples from the Array. If the size of the vector is Zero nothing is done. If the size of the
   * vector is greater than or Equal to the number of Tuples then the Array is Resized to Zero. If there are
   * indices that are larger than the size of the original (before erasing operations) then an error code (-100) is
   * returned from the program.
   * @param idxs The indices to remove
   * @return error code.
   */
  int eraseTuples(const std::vector<size_t>& idxs) override;

  /**
   * @brief copyTuple
   * @param currentPos
   * @param newPos
   * @return
   */
  int copyTuple(size_t currentPos, size_t newPos) override;

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
  bool copyFromArray(size_t destTupleOffset, IDataArray::ConstPointer sourceArray, size_t srcTupleOffset, size_t totalSrcTuples) override;

  /**
   * @brief Splats the same value c across all values in the Tuple
   * @param i The index of the Tuple
   * @param c The value to splat across all components in the tuple
   */
  void initializeTuple(size_t i, const void* p) override;
  /**
   * @brief Returns the number of elements in the internal array.
   */
  size_t getNumberOfTuples() const override;

  /**
   * @brief getSize Returns the total number of data items that are being stored. This is the sum of all the sizes
   * of the internal storage arrays for this class.
   * @return
   */
  size_t getSize() const override;

  /**
   * @brief setNumberOfComponents
   * @param nc
   */
  void setNumberOfComponents(int nc);

  /**
   * @brief getNumberOfComponents
   * @return
   */
  int getNumberOfComponents() const override;

  /**
   * @brief getComponentDimensions
   * @return
   */
  std::vector<size_t> getComponentDimensions() const override;

  /**
   * @brief SetRank
   * @param rnk
   */
  void SetRank(int rnk);

  /**
   * @brief getRank
   * @return
   */
  int getRank() const;

  /**
   * @brief getTypeSize
   * @return
   */
  size_t getTypeSize() const override;

  /**
   * @brief initializeWithZeros
   */
  void initializeWithZeros() override;

  /**
   * @brief deepCopy
   * @return
   */
  IDataArray::Pointer deepCopy(bool forceNoAllocate = false) const override;

  /**
   * @brief resizeTotalElements
   * @param size
   * @return
   */
  int32_t resizeTotalElements(size_t size) override;

  /**
   * @brief Resizes the internal array to accomondate numTuples
   * @param numTuples
   */
  void resizeTuples(size_t numTuples) override;

  // FIXME: These need to be implemented
  void printTuple(QTextStream& out, size_t i, char delimiter = ',') const override;

  /**
   * @brief printComponent
   * @param out
   * @param i
   * @param j
   */
  void printComponent(QTextStream& out, size_t i, int j) const override;

  /**
   *
   * @param parentId
   * @return
   */
  int writeH5Data(hid_t parentId, const std::vector<size_t>& tDims) const override;

  /**
   * @brief writeXdmfAttribute
   * @param out
   * @param volDims
   * @param hdfFileName
   * @param groupPath
   * @return
   */
  int writeXdmfAttribute(QTextStream& out, const int64_t* volDims, const QString& hdfFileName, const QString& groupPath, const QString& label) const override;

  /**
   * @brief Returns a ToolTipGenerator for creating HTML tooltip tables
   * with values populated to match the current DataArray.
   * @return
   */
  ToolTipGenerator getToolTipGenerator() const override;

  /**
   * @brief getInfoString
   * @return Returns a formatted string that contains general infomation about
   * the instance of the object.
   */
  QString getInfoString(SIMPL::InfoStringFormat format) const override;
  /**
   *
   * @param parentId
   * @return
   */
  int readH5Data(hid_t parentId) override;

  /**
   * @brief addEntry
   * @param grainId
   * @param value
   */
  void addEntry(int grainId, T value);

  /**
   * @brief clearAllLists
   */
  void clearAllLists();

  /**
   * @brief setList
   * @param grainId
   * @param neighborList
   */
  void setList(int grainId, SharedVectorType neighborList);

  /**
   * @brief getValue
   * @param grainId
   * @param index
   * @param ok
   * @return
   */
  T getValue(int grainId, int index, bool& ok) const;

  /**
   * @brief getNumberOfLists
   * @return
   */
  int getNumberOfLists() const;

  /**
   * @brief getListSize
   * @param grainId
   * @return
   */
  int getListSize(int grainId) const;

  VectorType& getListReference(int grainId) const;

  /**
   * @brief getList
   * @param grainId
   * @return
   */
  SharedVectorType getList(int grainId) const;

  /**
   * @brief copyOfList
   * @param grainId
   * @return
   */
  VectorType copyOfList(int grainId) const;

  /**
   * @brief operator []
   * @param grainId
   * @return
   */
  VectorType& operator[](int grainId);

  /**
   * @brief operator []
   * @param grainId
   * @return
   */
  VectorType& operator[](size_t grainId);

protected:
  /**
   * @brief NeighborList
   */
  NeighborList(size_t numTuples, const QString name);

private:
  QString m_NumNeighborsArrayName;
  std::vector<SharedVectorType> m_Array;
  size_t m_NumTuples;
  bool m_IsAllocated;
  T m_InitValue;

public:
  NeighborList(const NeighborList&) = delete;            // Copy Constructor Not Implemented
  NeighborList(NeighborList&&) = delete;                 // Move Constructor Not Implemented
  NeighborList& operator=(const NeighborList&) = delete; // Copy Assignment Not Implemented
  NeighborList& operator=(NeighborList&&) = delete;      // Move Assignment Not Implemented
};

using Int32NeighborListType = NeighborList<int32_t>;
using FloatNeighborListType = NeighborList<float>;

// -----------------------------------------------------------------------------
// Declare our extern templates

extern template class NeighborList<char>;
extern template class NeighborList<unsigned char>;

extern template class NeighborList<int8_t>;
extern template class NeighborList<uint8_t>;
extern template class NeighborList<int16_t>;
extern template class NeighborList<uint16_t>;
extern template class NeighborList<int32_t>;
extern template class NeighborList<uint32_t>;
extern template class NeighborList<int64_t>;
extern template class NeighborList<uint64_t>;

extern template class NeighborList<float>;
extern template class NeighborList<double>;

extern template class NeighborList<size_t>;
