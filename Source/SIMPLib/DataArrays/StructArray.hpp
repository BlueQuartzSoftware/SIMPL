
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

#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/IDataArrayFilter.h"
#include "SIMPLib/SIMPLib.h"

template<typename T>
class StructArray : public IDataArray
{
  public:
    SIMPL_SHARED_POINTERS(StructArray<T> )
    SIMPL_TYPE_MACRO_SUPER(StructArray<T>, IDataArray)
    SIMPL_CLASS_VERSION(2)

    /**
     * @brief Static constructor
     * @param numElements The number of elements in the internal array.
     * @param name The name of the array
     * @return Std::Shared_Ptr wrapping an instance of StructArrayTemplate<T>
     */
    static Pointer CreateArray(size_t numElements, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      StructArray<T>* d = new StructArray<T> (numElements, true);
      if(allocate)
      {
        if (d->Allocate() < 0)
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
    IDataArray::Pointer createNewArray(size_t numElements, int rank, size_t* dims, const QString& name, bool allocate = true) override
    {
      IDataArray::Pointer p = StructArray<T>::CreateArray(numElements, name, allocate);
      return p;
    }

    IDataArray::Pointer createNewArray(size_t numElements, std::vector<size_t> dims, const QString& name, bool allocate = true) override
    {
      IDataArray::Pointer p = StructArray<T>::CreateArray(numElements, name, allocate);
      return p;
    }

    IDataArray::Pointer createNewArray(size_t numElements, QVector<size_t> dims, const QString& name, bool allocate = true) override
    {
      IDataArray::Pointer p = StructArray<T>::CreateArray(numElements, name, allocate);
      return p;
    }

    /**
     * @brief Destructor
     */
    ~StructArray() override
    {
      //qDebug() << "~StructArrayTemplate '" << m_Name << "'" ;
      if ((nullptr != m_Array) && (true == this->_ownsData))
      {
        _deallocate();
      }
    }

    /**
     * @brief isAllocated
     * @return
     */
    bool isAllocated() override
    {
      return m_IsAllocated;
    }


    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    void getXdmfTypeAndSize(QString& xdmfTypeName, int& precision) override
    {
      xdmfTypeName = getNameOfClass();
      precision = 0;
    }

    /**
     * @brief getTypeAsString
     * @return
     */
    QString getTypeAsString() override { return "struct"; }

    /**
    * @brief Returns the HDF Type for a given primitive value.
    * @param value A value to use. Can be anything. Just used to get the type info
    * from
    * @return The HDF5 native type for the value
    */
    virtual QString getFullNameOfClass()
    {
      QString theType = getTypeAsString();
      theType = "StructArray<" + theType + ">";
      return theType;
    }


    /**
     * @brief Gives this array a human readable name
     * @param name The name of this array
     */
    void setName(const QString& name) override
    {
      m_Name = name;
    }

    /**
     * @brief Returns the human readable name of this array
     * @return
     */
    QString getName() override
    {
      return m_Name;
    }

    /**
     * @brief Makes this class responsible for freeing the memory
     */
    void takeOwnership() override
    {
      this->_ownsData = true;
    }

    /**
     * @brief This class will NOT free the memory associated with the internal pointer.
     * This can be useful if the user wishes to keep the data around after this
     * class goes out of scope.
     */
    void releaseOwnership() override
    {
      this->_ownsData = false;
    }

    /**
     * @brief Allocates the memory needed for this class
     * @return 1 on success, -1 on failure
     */
    int32_t Allocate()
    {
      if ((nullptr != m_Array) && (true == this->_ownsData))
      {
        _deallocate();
      }
      m_Array = nullptr;
      this->_ownsData = true;
      this->m_IsAllocated = false;

      if (this->m_Size == 0)
      {
        initialize();
        return 1;
      }


      size_t newSize = this->m_Size;
#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
      Array = static_cast<T*>( _mm_malloc (newSize * sizeof(T), 16) );
#else
      m_Array = (T*)malloc(newSize * sizeof(T));
#endif
      if (!m_Array)
      {
        qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
        return -1;
      }
      this->m_IsAllocated = true;
      this->m_Size = newSize;
      return 1;
    }

    /**
     * @brief Initializes this class to zero bytes freeing any data that it currently owns
     */
    virtual void initialize()
    {
      if (nullptr != m_Array && true == this->_ownsData)
      {
        _deallocate();
      }
      m_Array = nullptr;
      this->m_Size = 0;
      this->_ownsData = true;
      this->m_MaxId = 0;
      this->m_IsAllocated = false;
    }

    /**
     * @brief Sets all the values to zero.
     */
    void initializeWithZeros() override
    {
      size_t typeSize = sizeof(T);
      ::memset(m_Array, 0, this->m_Size * typeSize);
    }

    /**
     * @brief Sets all the values to value.
     */
    void initializeWithValue(T value, size_t offset = 0)
    {
      for (size_t i = offset; i < this->m_Size; i++)
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
    int eraseTuples(QVector<size_t>& idxs) override
    {

      int err = 0;

      // If nothing is to be erased just return
      if(idxs.size() == 0)
      {
        return 0;
      }

      size_t idxs_size = static_cast<size_t>(idxs.size());
      if (idxs_size >= getNumberOfTuples() )
      {
        resize(0);
        return 0;
      }

      // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
      // off the end of the array and return an error code.
      for(QVector<size_t>::size_type i = 0; i < idxs.size(); ++i)
      {
        if (idxs[i] > this->m_MaxId) { return -100; }
      }

      // Calculate the new size of the array to copy into
      size_t newSize = (getNumberOfTuples() - idxs.size());
      T* currentSrc = nullptr;

      // Create a new Array to copy into
      T* newArray = (T*)malloc(newSize * sizeof(T));
      // Splat AB across the array so we know if we are copying the values or not
      ::memset(newArray, 0xAB, newSize * sizeof(T));

      // Keep the current Destination Pointer
      T* currentDest = newArray;
      size_t j = 0;
      int k = 0;
      // Find the first chunk to copy by walking the idxs array until we get an
      // index that is NOT a continuous increment from the start
      for (k = 0; k < idxs.size(); ++k)
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
        _deallocate(); // We are done copying - delete the current Array
        this->m_Size = newSize;
        m_Array = newArray;
        this->_ownsData = true;
        this->m_MaxId = newSize - 1;
        return 0;
      }

      QVector<size_t> srcIdx(idxs.size() + 1);
      QVector<size_t> destIdx(idxs.size() + 1);
      QVector<size_t> copyElements(idxs.size() + 1);
      srcIdx[0] = 0;
      destIdx[0] = 0;
      copyElements[0] = (idxs[0] - 0);

      for (int i = 1; i < srcIdx.size(); ++i)
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
      for (int i = 0; i < srcIdx.size(); ++i)
      {
        currentDest = newArray + destIdx[i];
        currentSrc = m_Array + srcIdx[i];
        size_t bytes = copyElements[i] * sizeof(T);
        ::memcpy(currentDest, currentSrc, bytes);
      }

      // We are done copying - delete the current Array
      _deallocate();

      // Allocation was successful.  Save it.
      this->m_Size = newSize;
      m_Array = newArray;
      // This object has now allocated its memory and owns it.
      this->_ownsData = true;

      this->m_MaxId = newSize - 1;

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
      size_t max =  ((this->m_MaxId + 1));
      if (currentPos >= max
          || newPos >= max )
      {return -1;}
      if (currentPos == newPos) { return 0; }
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
    bool copyFromArray(size_t destTupleOffset, IDataArray::Pointer sourceArray, size_t srcTupleOffset, size_t totalSrcTuples) override
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
      Self* source = dynamic_cast<Self*>(sourceArray.get());
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
    size_t getTypeSize() override
    {
      return sizeof(T);
    }

    /**
     * @brief Returns the number of elements in the internal array.
     */
    size_t getNumberOfTuples() override
    {
      if (m_Size == 0) { return 0; }
      return (this->m_MaxId + 1);
    }

    /**
     * @brief getSize
     * @return
     */
    size_t getSize() override
    {
      return m_Size;
    }

    /**
     * @brief getNumberOfComponents
     * @return
     */
    int getNumberOfComponents() override
    {
      return 1;
    }

    /**
     * @brief getNumberOfComponents
     * @return
     */
    QVector<size_t> getComponentDimensions() override
    {
      QVector<size_t> dims(1, 1);
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
    int getRank()
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
      if (i >= m_Size) { return nullptr;}

      return (void*)(&(m_Array[i]));
    }


    /**
     * @brief Returns the pointer to a specific index into the array. No checks are made
     * as to the correctness of the index being passed in. If you ask for an index off
     * then end of the array they you will likely cause your program to abort.
     * @param i The index to return the pointer to.
     * @return The pointer to the index
     */
    virtual T* getPointer(size_t i)
    {
#ifndef NDEBUG
      if (m_Size > 0) { Q_ASSERT(i < m_Size);}
#endif
      return (T*)(&(m_Array[i]));
    }

    /**
     * @brief Splats the same value c across all values in the Tuple
     * @param i The index of the Tuple
     * @param c The value to splat across all components in the tuple
     */
    void initializeTuple(size_t i, void* p) override
    {
#ifndef NDEBUG
      if (m_Size > 0) { Q_ASSERT(i < m_Size);}
#endif
      Q_ASSERT(false);
      //T c = static_cast<T>(p);
      //for (int j = 0; j < this->NumberOfComponents; ++j)
      {
        // Array[i] = c;
      }
    }

    IDataArray::Pointer deepCopy(bool forceNoAllocate = false) override
    {
      IDataArray::Pointer daCopy = createNewArray(getNumberOfTuples(), getComponentDimensions(), getName(), m_IsAllocated);
      if(m_IsAllocated == true && forceNoAllocate == false)
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
      if (this->resizeAndExtend(size) || size == 0)
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }

    /**
     * @brief Resize
     * @param numTuples
     * @return
     */
    int32_t resize(size_t numTuples) override
    {
      return resizeTotalElements(numTuples );
    }

    /**
     * @brief printTuple
     * @param out
     * @param i
     * @param delimiter
     */
    void printTuple(QTextStream& out, size_t i, char delimiter = ',') override
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
    void printComponent(QTextStream& out, size_t i, int j) override
    {
      Q_ASSERT(false);
      //        out << Array[i + j];
    }


    /**
     *
     * @param parentId
     * @return
     */
    int writeH5Data(hid_t parentId, QVector<size_t> tDims) override
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
    int writeXdmfAttribute(QTextStream& out, int64_t* volDims, const QString& hdfFileName,
                                   const QString& groupPath, const QString& labelb) override
    {
      out << "<!-- Xdmf is not supported for " << getNameOfClass() << " with type " << getTypeAsString() << " --> ";
      return -1;
    }

    /**
     * @brief getInfoString
     * @return Returns a formatted string that contains general infomation about
     * the instance of the object.
     */
    QString getInfoString(SIMPL::InfoStringFormat format) override
    {
      QString info;
      QTextStream ss (&info);
      if(format == SIMPL::HtmlFormat)
      {
        ss << "<html><head></head>\n";
        ss << "<body>\n";
        ss << "<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\">\n";
        ss << "<tbody>\n";
        ss << "<tr bgcolor=\"#FFFCEA\"><th colspan=2>Attribute Array Info</th></tr>";
        ss << "<tr bgcolor=\"#E9E7D6\"><th align=\"right\">Name:</th><td>" << getName() << "</td></tr>";
        ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Type:</th><td>" << getTypeAsString() << "</td></tr>";
        QLocale usa(QLocale::English, QLocale::UnitedStates);
        QString numStr = usa.toString(static_cast<qlonglong>(getNumberOfTuples()));
        ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Number of Tuples:</th><td>" << numStr << "</td></tr>";
        ss << "</tbody></table>\n";
        ss << "<br/>";
        ss << "</body></html>";
      }
      else
      {

      }
      return info;
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
    StructArray(size_t numElements, bool ownsData = true) :
      m_Array(nullptr),
      m_Size(numElements),
      _ownsData(ownsData),
      m_IsAllocated(false)
    {
      m_MaxId = (m_Size > 0) ? m_Size - 1 : m_Size;
      //  MUD_FLAP_0 = MUD_FLAP_1 = MUD_FLAP_2 = MUD_FLAP_3 = MUD_FLAP_4 = MUD_FLAP_5 = 0xABABABABABABABABul;
    }

    /**
     * @brief deallocates the memory block
     */
    void _deallocate()
    {
      // We are going to splat 0xABABAB across the first value of the array as a debugging aid
      unsigned char* cptr = reinterpret_cast<unsigned char*>(m_Array);
      if(nullptr != cptr)
      {
        if (m_Size > 0)
        {
          if (sizeof(T) >= 1) { cptr[0] = 0xAB; }
          if (sizeof(T) >= 2) { cptr[1] = 0xAB; }
          if (sizeof(T) >= 4) { cptr[2] = 0xAB; cptr[3] = 0xAB;}
          if (sizeof(T) >= 8) { cptr[4] = 0xAB; cptr[5] = 0xAB; cptr[6] = 0xAB; cptr[7] = 0xAB;}
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

#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
      _mm_free( this->m_buffer );
#else
      free(m_Array);
#endif
      m_Array = nullptr;
      this->m_IsAllocated = false;
    }

    /**
     * @brief resizes the internal array to be 'size' elements in length
     * @param size
     * @return Pointer to the internal array
     */
    virtual T* resizeAndExtend(size_t size)
    {
      T* newArray;
      size_t newSize;

      if (size > this->m_Size)
      {
        newSize = size;
      }
      else if (size == this->m_Size) // Requested size is equal to current size.  Do nothing.
      {
        return m_Array;
      }
      else // Requested size is smaller than current size.  Squeeze the memory.
      {
        newSize = size;
      }

      // Wipe out the array completely if new size is zero.
      if (newSize == 0)
      {
        this->initialize();
        return m_Array;
      }
      // OS X's realloc does not free memory if the new block is smaller.  This
      // is a very serious problem and causes huge amount of memory to be
      // wasted. Do not use realloc on the Mac.
      bool dontUseRealloc = false;
#if defined __APPLE__
      dontUseRealloc = true;
#endif

      // Allocate a new array if we DO NOT own the current array
      if ((nullptr != m_Array) && (false == this->_ownsData))
      {
        // The old array is owned by the user so we cannot try to
        // reallocate it.  Just allocate new memory that we will own.
        newArray = (T*)malloc(newSize * sizeof(T));
        if (!newArray)
        {
          qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
          return 0;
        }

        // Copy the data from the old array.
        memcpy(newArray, m_Array, (newSize < this->m_Size ? newSize : this->m_Size) * sizeof(T));
      }
      else if (!dontUseRealloc)
      {
        // Try to reallocate with minimal memory usage and possibly avoid copying.
        newArray = (T*)realloc(m_Array, newSize * sizeof(T));
        if (!newArray)
        {
          qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
          return 0;
        }
      }
      else
      {
        newArray = (T*)malloc(newSize * sizeof(T));
        if (!newArray)
        {
          qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
          return 0;
        }

        // Copy the data from the old array.
        if (m_Array != nullptr)
        {
          memcpy(newArray, m_Array, (newSize < this->m_Size ? newSize : this->m_Size) * sizeof(T));
        }
        // Free the old array
        _deallocate();
      }

      // Allocation was successful.  Save it.
      this->m_Size = newSize;
      m_Array = newArray;
      // This object has now allocated its memory and owns it.
      this->_ownsData = true;

      this->m_MaxId = newSize - 1;
      this->m_IsAllocated = true;

      return m_Array;
    }

  private:

    //  unsigned long long int MUD_FLAP_0;
    T* m_Array;
    //  unsigned long long int MUD_FLAP_1;
    size_t m_Size;
    //  unsigned long long int MUD_FLAP_4;
    bool _ownsData;
    //  unsigned long long int MUD_FLAP_2;
    size_t m_MaxId;

    bool m_IsAllocated;
    //   unsigned long long int MUD_FLAP_3;
    QString m_Name;
    //  unsigned long long int MUD_FLAP_5;

    StructArray(const StructArray&); //Not Implemented
    void operator=(const StructArray&); //Not Implemented


};



