
//
//  This code was written under United States Air Force Contract number
//                           FA8650-07-D-5800
//
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

#include <cstring>

#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/IDataArrayFilter.h"

template <typename T>
class StructArray : public IDataArray
{
public:
  using Self = StructArray<T>;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  QString getNameOfClass() const override
  {
    return QString("StructArray<T>");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("StructArray<T>");
  }

  /**
   * @brief Returns the version of this class.
   * @return
   */
  int32_t getClassVersion() const override
  {
    return 2;
  }

  /**
   * @brief Static constructor
   * @param numElements The number of elements in the internal array.
   * @param name The name of the array
   * @return Std::Shared_Ptr wrapping an instance of StructArrayTemplate<T>
   */
  static Pointer CreateArray(size_t numElements, const QString& name, bool allocate = true)
  {
    if(name.isEmpty())
    {
      return NullPointer();
    }
    StructArray<T>* d = new StructArray<T>(numElements, true);
    if(allocate)
    {
      if(d->Allocate() < 0)
      {
        // Could not allocate enough memory, reset the pointer to null and return
        delete d;
        return StructArray<T>::NullPointer();
      }
    }
    d->setName(name);
    Pointer ptr(d);
    return ptr;
  }

  /**
   * @brief Static Method to create a DataArray from a QVector through a deep copy of the data
   * contained in the vector. The number of components will be set to 1.
   * @param vec The vector to copy the data from
   * @param name The name of the array
   * @return Std::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
   */
  IDataArray::Pointer createNewArray(size_t numElements, int rank, const size_t* dims, const QString& name, bool allocate = true) const override
  {
    IDataArray::Pointer p = StructArray<T>::CreateArray(numElements, name, allocate);
    return p;
  }

  IDataArray::Pointer createNewArray(size_t numElements, const std::vector<size_t>& dims, const QString& name, bool allocate = true) const override
  {
    IDataArray::Pointer p = StructArray<T>::CreateArray(numElements, name, allocate);
    return p;
  }

  /**
   * @brief Destructor
   */
  ~StructArray() override
  {
    // qDebug() << "~StructArrayTemplate '" << m_Name << "'" ;
    if((nullptr != m_Array) && (true == m_OwnsData))
    {
      deallocate();
    }
  }

  /**
   * @brief isAllocated
   * @return
   */
  bool isAllocated() const override
  {
    return m_IsAllocated;
  }

  /**
   * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
   * can be a primitive like char, float, int or the name of a class.
   * @return
   */
  void getXdmfTypeAndSize(QString& xdmfTypeName, int& precision) const override
  {
    xdmfTypeName = getNameOfClass();
    precision = 0;
  }

  /**
   * @brief getTypeAsString
   * @return
   */
  QString getTypeAsString() const override
  {
    return "struct";
  }

  /**
   * @brief Returns the HDF Type for a given primitive value.
   * @param value A value to use. Can be anything. Just used to get the type info
   * from
   * @return The HDF5 native type for the value
   */
  virtual QString getFullNameOfClass() const
  {
    QString theType = getTypeAsString();
    theType = "StructArray<" + theType + ">";
    return theType;
  }

  /**
   * @brief Makes this class responsible for freeing the memory
   */
  void takeOwnership() override
  {
    m_OwnsData = true;
  }

  /**
   * @brief This class will NOT free the memory associated with the internal pointer.
   * This can be useful if the user wishes to keep the data around after this
   * class goes out of scope.
   */
  void releaseOwnership() override
  {
    m_OwnsData = false;
  }

  /**
   * @brief Allocates the memory needed for this class
   * @return 1 on success, -1 on failure
   */
  int32_t Allocate()
  {
    if((nullptr != m_Array) && (true == m_OwnsData))
    {
      deallocate();
    }
    m_Array = nullptr;
    m_OwnsData = true;
    m_IsAllocated = false;

    if(m_Size == 0)
    {
      initialize();
      return 1;
    }

    size_t newSize = m_Size;
    m_Array = new T[newSize]();

    if(!m_Array)
    {
      qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. ";
      return -1;
    }
    m_IsAllocated = true;
    m_Size = newSize;
    return 1;
  }

  /**
   * @brief Initializes this class to zero bytes freeing any data that it currently owns
   */
  virtual void initialize()
  {
    if(nullptr != m_Array && true == m_OwnsData)
    {
      deallocate();
    }
    m_Array = nullptr;
    m_Size = 0;
    m_OwnsData = true;
    m_MaxId = 0;
    m_IsAllocated = false;
  }

  /**
   * @brief Sets all the values to zero.
   */
  void initializeWithZeros() override
  {
    size_t typeSize = sizeof(T);
    ::memset(m_Array, 0, m_Size * typeSize);
  }

  /**
   * @brief Sets all the values to value.
   */
  void initializeWithValue(T value, size_t offset = 0)
  {
    for(size_t i = offset; i < m_Size; i++)
    {
      m_Array[i] = value;
    }
  }

  /**
   * @brief Removes Tuples from the Array. If the size of the vector is Zero nothing is done. If the size of the
   * vector is greater than or Equal to the number of Tuples then the Array is Resized to Zero. If there are
   * indices that are larger than the size of the original (before erasing operations) then an error code (-100) is
   * returned from the program.
   * @param idxs The indices to remove
   * @return error code.
   */
  int eraseTuples(const std::vector<size_t>& idxs) override
  {

    int err = 0;

    // If nothing is to be erased just return
    if(idxs.empty())
    {
      return 0;
    }

    size_t idxs_size = static_cast<size_t>(idxs.size());
    if(idxs_size >= getNumberOfTuples())
    {
      resizeTuples(0);
      return 0;
    }

    // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
    // off the end of the array and return an error code.
    for(std::vector<size_t>::size_type i = 0; i < idxs.size(); ++i)
    {
      if(idxs[i] > m_MaxId)
      {
        return -100;
      }
    }

    // Calculate the new size of the array to copy into
    size_t newSize = (getNumberOfTuples() - idxs.size());
    T* currentSrc = nullptr;

    // Create a new Array to copy into
    T* newArray = new T[newSize]();
    // Splat AB across the array so we know if we are copying the values or not
    ::memset(newArray, 0xAB, newSize * sizeof(T));

    // Keep the current Destination Pointer
    T* currentDest = newArray;
    size_t j = 0;
    size_t k = 0;
    // Find the first chunk to copy by walking the idxs array until we get an
    // index that is NOT a continuous increment from the start
    for(k = 0; k < idxs.size(); ++k)
    {
      if(j == idxs[k])
      {
        ++j;
      }
      else
      {
        break;
      }
    }

    if(k == idxs.size()) // Only front elements are being dropped
    {
      currentSrc = m_Array + (j);
      ::memcpy(currentDest, currentSrc, (getNumberOfTuples() - idxs.size()) * sizeof(T));
      deallocate(); // We are done copying - delete the current Array
      m_Size = newSize;
      m_Array = newArray;
      m_OwnsData = true;
      m_MaxId = newSize - 1;
      return 0;
    }

    std::vector<size_t> srcIdx(idxs.size() + 1);
    std::vector<size_t> destIdx(idxs.size() + 1);
    std::vector<size_t> copyElements(idxs.size() + 1);
    srcIdx[0] = 0;
    destIdx[0] = 0;
    copyElements[0] = (idxs[0] - 0);

    for(size_t i = 1; i < srcIdx.size(); ++i)
    {
      srcIdx[i] = (idxs[i - 1] + 1);

      if(i < srcIdx.size() - 1)
      {
        copyElements[i] = (idxs[i] - idxs[i - 1] - 1);
      }
      else
      {
        copyElements[i] = (getNumberOfTuples() - idxs[i - 1] - 1);
      }
      destIdx[i] = copyElements[i - 1] + destIdx[i - 1];
    }

    // Copy the data
    for(int i = 0; i < srcIdx.size(); ++i)
    {
      currentDest = newArray + destIdx[i];
      currentSrc = m_Array + srcIdx[i];
      size_t bytes = copyElements[i] * sizeof(T);
      ::memcpy(currentDest, currentSrc, bytes);
    }

    // We are done copying - delete the current Array
    deallocate();

    // Allocation was successful.  Save it.
    m_Size = newSize;
    m_Array = newArray;
    // This object has now allocated its memory and owns it.
    m_OwnsData = true;

    m_MaxId = newSize - 1;

    return err;
  }

  /**
   * @brief
   * @param currentPos
   * @param newPos
   * @return
   */
  int copyTuple(size_t currentPos, size_t newPos) override
  {
    size_t max = ((m_MaxId + 1));
    if(currentPos >= max || newPos >= max)
    {
      return -1;
    }
    if(currentPos == newPos)
    {
      return 0;
    }
    T* src = m_Array + (currentPos);
    T* dest = m_Array + (newPos);
    size_t bytes = sizeof(T);
    ::memcpy(dest, src, bytes);
    return 0;
  }

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
  bool copyFromArray(size_t destTupleOffset, IDataArray::ConstPointer sourceArray, size_t srcTupleOffset, size_t totalSrcTuples) override
  {
    if(!m_IsAllocated)
    {
      return false;
    }
    if(nullptr == m_Array)
    {
      return false;
    }
    if(destTupleOffset >= m_MaxId)
    {
      return false;
    }
    if(!sourceArray->isAllocated())
    {
      return false;
    }
    const Self* source = dynamic_cast<const Self*>(sourceArray.get());
    if(source == nullptr)
    {
      return false;
    }
    if(nullptr == source->getPointer(0))
    {
      return false;
    }

    if(srcTupleOffset + totalSrcTuples > sourceArray->getNumberOfTuples())
    {
      return false;
    }

    if(totalSrcTuples + destTupleOffset > m_Size)
    {
      return false;
    }

    T* src = source->getPointer(srcTupleOffset);
    T* dest = m_Array + destTupleOffset;
    size_t bytes = (totalSrcTuples) * sizeof(T);
    ::memcpy(dest, src, bytes);

    return true;
  }

  /**
   * @brief Returns the number of bytes that make up the data type.
   * 1 = char
   * 2 = 16 bit integer
   * 4 = 32 bit integer/Float
   * 8 = 64 bit integer/Double
   */
  size_t getTypeSize() const override
  {
    return sizeof(T);
  }

  /**
   * @brief Returns the number of elements in the internal array.
   */
  size_t getNumberOfTuples() const override
  {
    if(m_Size == 0)
    {
      return 0;
    }
    return (m_MaxId + 1);
  }

  /**
   * @brief getSize
   * @return
   */
  size_t getSize() const override
  {
    return m_Size;
  }

  /**
   * @brief getNumberOfComponents
   * @return
   */
  int getNumberOfComponents() const override
  {
    return 1;
  }

  /**
   * @brief getNumberOfComponents
   * @return
   */
  std::vector<size_t> getComponentDimensions() const override
  {
    std::vector<size_t> dims(1, 1);
    return dims;
  }

  // Description:
  // Set/Get the dimension (n) of the rank. Must be >= 1. Make sure that
  // this is set before allocation.
  void SetRank(int rnk)
  {
  }

  /**
   * @brief getRank
   * @return
   */
  int getRank() const
  {
    return 1;
  }

  /**
   * @brief Returns a void pointer pointing to the index of the array. nullptr
   * pointers are entirely possible. No checks are performed to make sure
   * the index is with in the range of the internal data array.
   * @param i The index to have the returned pointer pointing to.
   * @return Void Pointer. Possibly nullptr.
   */
  void* getVoidPointer(size_t i) override
  {
    if(i >= m_Size)
    {
      return nullptr;
    }

    return reinterpret_cast<void*>(m_Array + i);
  }

  /**
   * @brief Returns the pointer to a specific index into the array. No checks are made
   * as to the correctness of the index being passed in. If you ask for an index off
   * then end of the array they you will likely cause your program to abort.
   * @param i The index to return the pointer to.
   * @return The pointer to the index
   */
  virtual T* getPointer(size_t i) const
  {
#ifndef NDEBUG
    if(m_Size > 0)
    {
      Q_ASSERT(i < m_Size);
    }
#endif
    return reinterpret_cast<T*>(m_Array + i);
  }

  /**
   * @brief Splats the same value c across all values in the Tuple
   * @param i The index of the Tuple
   * @param c The value to splat across all components in the tuple
   */
  void initializeTuple(size_t i, const void* p) override
  {
#ifndef NDEBUG
    if(m_Size > 0)
    {
      Q_ASSERT(i < m_Size);
    }
#endif
    Q_ASSERT(false);
    // T c = static_cast<T>(p);
    // for (int j = 0; j < this->NumberOfComponents; ++j)
    {
      // Array[i] = c;
    }
  }

  IDataArray::Pointer deepCopy(bool forceNoAllocate = false) const override
  {
    bool allocate = m_IsAllocated;
    if(forceNoAllocate)
    {
      allocate = false;
    }
    IDataArray::Pointer daCopy = createNewArray(getNumberOfTuples(), getComponentDimensions(), getName(), allocate);
    if(m_IsAllocated && !forceNoAllocate)
    {
      T* src = getPointer(0);
      void* dest = daCopy->getVoidPointer(0);
      size_t totalBytes = (getNumberOfTuples() * getNumberOfComponents() * sizeof(T));
      ::memcpy(dest, src, totalBytes);
    }
    return daCopy;
  }

  /**
   * @brief Reseizes the internal array
   * @param size The new size of the internal array
   * @return 1 on success, 0 on failure
   */
  int32_t resizeTotalElements(size_t size) override
  {
    if(resizeAndExtend(size) || size == 0)
    {
      return 1;
    }
    return 0;
  }

  /**
   * @brief Resizes the internal array to accomondate numTuples
   * @param numTuples
   */
  void resizeTuples(size_t numTuples) override
  {
    resizeTotalElements(numTuples);
  }

  /**
   * @brief printTuple
   * @param out
   * @param i
   * @param delimiter
   */
  void printTuple(QTextStream& out, size_t i, char delimiter = ',') const override
  {
    Q_ASSERT(false);
    //        for(int j = 0; j < NumberOfComponents; ++j)
    //        {
    //          if (j != 0) { out << delimiter; }
    //          out << Array[i + j];
    //        }
  }

  /**
   * @brief printComponent
   * @param out
   * @param i
   * @param j
   */
  void printComponent(QTextStream& out, size_t i, int j) const override
  {
    Q_ASSERT(false);
    //        out << Array[i + j];
  }

  /**
   *
   * @param parentId
   * @return
   */
  int writeH5Data(hid_t parentId, const std::vector<size_t>& tDims) const override
  {
    Q_ASSERT(false);
    return -1;
  }

  /**
   * @brief writeXdmfAttribute
   * @param out
   * @param volDims
   * @param hdfFileName
   * @param groupPath
   * @return
   */
  int writeXdmfAttribute(QTextStream& out, const int64_t* volDims, const QString& hdfFileName, const QString& groupPath, const QString& labelb) const override
  {
    out << "<!-- Xdmf is not supported for " << getNameOfClass() << " with type " << getTypeAsString() << " --> ";
    return -1;
  }

  /**
   * @brief Returns a ToolTipGenerator for creating HTML tooltip tables
   * with values populated to match the current DataArray.
   * @return
   */
  ToolTipGenerator getToolTipGenerator() const override
  {
    ToolTipGenerator toolTipGen;
    QLocale usa(QLocale::English, QLocale::UnitedStates);

    toolTipGen.addTitle("Attribute Array Info");
    toolTipGen.addValue("Name", getName());
    toolTipGen.addValue("Type", getTypeAsString());
    toolTipGen.addValue("Number of Tuples", usa.toString(static_cast<qlonglong>(getNumberOfTuples())));

    return toolTipGen;
  }

  /**
   * @brief getInfoString
   * @return Returns a formatted string that contains general infomation about
   * the instance of the object.
   */
  QString getInfoString(SIMPL::InfoStringFormat format) const override
  {
    if(format == SIMPL::HtmlFormat)
    {
      return getToolTipGenerator().generateHTML();
    }
    else
    {
    }
    return QString();
  }

  /**
   * @brief
   * @param parentId
   * @return
   */
  int readH5Data(hid_t parentId) override
  {
    Q_ASSERT(false);
    int err = -1;

    return err;
  }

  /**
   * @brief operator []
   * @param i
   * @return
   */
  inline T& operator[](size_t i)
  {
    Q_ASSERT(i < m_Size);
    return m_Array[i];
  }

protected:
  /**
   * @brief Protected Constructor
   * @param numElements The number of elements in the internal array.
   * @param takeOwnership Will the class clean up the memory. Default=true
   */
  StructArray(size_t numElements, bool ownsData = true)
  : m_Array(nullptr)
  , m_Size(numElements)
  , m_OwnsData(ownsData)
  , m_IsAllocated(false)
  {
    m_MaxId = (m_Size > 0) ? m_Size - 1 : m_Size;
    //  MUD_FLAP_0 = MUD_FLAP_1 = MUD_FLAP_2 = MUD_FLAP_3 = MUD_FLAP_4 = MUD_FLAP_5 = 0xABABABABABABABABul;
  }

  /**
   * @brief deallocates the memory block
   */
  void deallocate()
  {
    // We are going to splat 0xABABAB across the first value of the array as a debugging aid
    unsigned char* cptr = reinterpret_cast<unsigned char*>(m_Array);
    if(nullptr != cptr)
    {
      if(m_Size > 0)
      {
        if(sizeof(T) >= 1)
        {
          cptr[0] = 0xAB;
        }
        if(sizeof(T) >= 2)
        {
          cptr[1] = 0xAB;
        }
        if(sizeof(T) >= 4)
        {
          cptr[2] = 0xAB;
          cptr[3] = 0xAB;
        }
        if(sizeof(T) >= 8)
        {
          cptr[4] = 0xAB;
          cptr[5] = 0xAB;
          cptr[6] = 0xAB;
          cptr[7] = 0xAB;
        }
      }
    }
#if 0
      if (MUD_FLAP_0 != 0xABABABABABABABABul
          || MUD_FLAP_1 != 0xABABABABABABABABul
          || MUD_FLAP_2 != 0xABABABABABABABABul
          || MUD_FLAP_3 != 0xABABABABABABABABul
          || MUD_FLAP_4 != 0xABABABABABABABABul
          || MUD_FLAP_5 != 0xABABABABABABABABul)
      {
        Q_ASSERT(false);
      }
#endif

    delete[](m_Array);

    m_Array = nullptr;
    m_IsAllocated = false;
  }

  /**
   * @brief resizes the internal array to be 'size' elements in length
   * @param size
   * @return Pointer to the internal array
   */
  virtual T* resizeAndExtend(size_t size)
  {
    T* newArray = nullptr;
    size_t newSize = 0;

    if(size == m_Size) // Requested size is equal to current size.  Do nothing.
    {
      return m_Array;
    }
    newSize = size;

    // Wipe out the array completely if new size is zero.
    if(newSize == 0)
    {
      initialize();
      return m_Array;
    }

    newArray = new T[newSize]();
    if(!newArray)
    {
      qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. ";
      return nullptr;
    }

    // Copy the data from the old array.
    if(m_Array != nullptr)
    {
      std::memcpy(newArray, m_Array, (newSize < m_Size ? newSize : m_Size) * sizeof(T));
    }

    // Allocate a new array if we DO NOT own the current array
    if((nullptr != m_Array) && m_OwnsData)
    {
      // Free the old array
      deallocate();
    }

    // Allocation was successful.  Save it.
    m_Size = newSize;
    m_Array = newArray;
    // This object has now allocated its memory and owns it.
    m_OwnsData = true;

    m_MaxId = newSize - 1;
    m_IsAllocated = true;

    return m_Array;
  }

private:
  //  unsigned long long int MUD_FLAP_0;
  T* m_Array;
  //  unsigned long long int MUD_FLAP_1;
  size_t m_Size;
  //  unsigned long long int MUD_FLAP_4;
  bool m_OwnsData;
  //  unsigned long long int MUD_FLAP_2;
  size_t m_MaxId;

  bool m_IsAllocated;
  //   unsigned long long int MUD_FLAP_3;
  //  unsigned long long int MUD_FLAP_5;

  StructArray(const StructArray&);    // Not Implemented
  void operator=(const StructArray&); // Not Implemented
};
