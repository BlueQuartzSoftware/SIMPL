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

#ifndef _dynamicListArray_H_
#define _dynamicListArray_H_

#include <vector>

//-- DREAM3D Includes
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The MeshFaceNeighbors class contains arrays of Faces for each Node in the mesh. This allows quick query to the node
 * to determine what Cells the node is a part of.
 */
template <typename T, typename K> class DynamicListArray
{
public:
  SIMPL_SHARED_POINTERS(DynamicListArray)
  SIMPL_STATIC_NEW_MACRO(DynamicListArray)
  SIMPL_TYPE_MACRO(DynamicListArray)

  class ElementList
  {
  public:
    T ncells;
    K* cells;
  };

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  virtual ~DynamicListArray()
  {
    // This makes sure we deallocate any lists that have been created
    for(size_t i = 0; i < this->m_Size; i++)
    {
      if(this->m_Array[i].cells != nullptr)
      {
        delete[] this->m_Array[i].cells;
      }
    }
    // Now delete all the "NeighborLists" structures
    if(this->m_Array != nullptr)
    {
      delete[] this->m_Array;
    }
  }

  /**
   * @brief size
   * @return
   */
  size_t size()
  {
    return m_Size;
  }

  /**
   * @brief deepCopy
   * @param forceNoAllocate
   * @return
   */
  Pointer deepCopy(bool forceNoAllocate = false)
  {
    DynamicListArray::Pointer copy = DynamicListArray::New();
    std::vector<T> linkCounts(m_Size, 0);
    if(forceNoAllocate)
    {
      copy->allocateLists(linkCounts);
      return copy;
    }

    // Figure out how many entries, and for each entry, how many cells
    for(size_t ptId = 0; ptId < m_Size; ptId++)
    {
      linkCounts[ptId] = this->m_Array[ptId].ncells;
    }
    // Allocate all that in the copy
    copy->allocateLists(linkCounts);
    // Copy the data from the original to the new
    for(size_t ptId = 0; ptId < m_Size; ptId++)
    {
      ElementList& elementList = getElementList(ptId);
      copy->setElementList(ptId, elementList);
    }
    return copy;
  }

  /**
   * @brief insertCellReference
   * @param ptId
   * @param pos
   * @param cellId
   */
  inline void insertCellReference(size_t ptId, size_t pos, size_t cellId)
  {
    this->m_Array[ptId].cells[pos] = cellId;
  }

  /**
   * @brief Get a link structure given a point id.
   * @param ptId
   * @return
   */
  ElementList& getElementList(size_t ptId)
  {
    return this->m_Array[ptId];
  }

  /**
   * @brief setElementList
   * @param ptId
   * @param nCells
   * @param data
   * @return
   */
  bool setElementList(size_t ptId, T nCells, K* data)
  {
    if(ptId >= m_Size)
    {
      return false;
    }
    if(nullptr != m_Array[ptId].cells && m_Array[ptId].ncells > 0)
    {
      m_Array[ptId].cells = nullptr;
      m_Array[ptId].ncells = 0;
    }
    m_Array[ptId].ncells = nCells;
    // If nCells is huge then there could be problems with this
    this->m_Array[ptId].cells = new K[nCells];
    ::memcpy(m_Array[ptId].cells, data, sizeof(K) * nCells);
    return true;
  }

  /**
   * @brief setElementList
   * @param ptId
   * @param list
   * @return
   */
  bool setElementList(size_t ptId, ElementList& list)
  {
    T nCells = list.ncells;
    K* data = list.cells;
    if(ptId >= m_Size)
    {
      return false;
    }
    if(nullptr != m_Array[ptId].cells && m_Array[ptId].ncells > 0)
    {
      m_Array[ptId].cells = nullptr;
      m_Array[ptId].ncells = 0;
    }
    m_Array[ptId].ncells = nCells;
    // If nCells is huge then there could be problems with this
    this->m_Array[ptId].cells = new K[nCells];
    ::memcpy(m_Array[ptId].cells, data, sizeof(K) * nCells);
    return true;
  }

  /**
   * @brief Get the number of cells using the point specified by ptId.
   * @param ptId
   * @return
   */
  T getNumberOfElements(size_t ptId)
  {
    return this->m_Array[ptId].ncells;
  }

  /**
   * @brief Return a list of cell ids using the point.
   * @param ptId
   * @return
   */
  K* getElementListPointer(size_t ptId)
  {
    return this->m_Array[ptId].cells;
  }

  /**
   * @brief deserializeLinks
   * @param buffer
   * @param nElements
   */
  void deserializeLinks(QVector<uint8_t>& buffer, size_t nElements)
  {
    size_t offset = 0;
    allocate(nElements); // Allocate all the links with 0 and nullptr;
    uint8_t* bufPtr = buffer.data();

    // Walk the array and allocate all the array links to Zero and nullptr
    T* ncells = nullptr;
    for(size_t i = 0; i < nElements; ++i)
    {
      ncells = reinterpret_cast<T*>(bufPtr + offset);
      this->m_Array[i].ncells = *ncells; // Set the number of cells in this link
      offset += 2;
      this->m_Array[i].cells = new K[(*ncells)];                                // Allocate a new chunk of memory to store the list
      ::memcpy(this->m_Array[i].cells, bufPtr + offset, (*ncells) * sizeof(K)); // Copy from the buffer into the new list memory
      offset += (*ncells) * sizeof(K);                                          // Increment the offset
    }
  }

  /**
   * @brief deserializeLinks
   * @param buffer
   * @param nElements
   */
  void deserializeLinks(std::vector<uint8_t>& buffer, size_t nElements)
  {
    size_t offset = 0;
    allocate(nElements); // Allocate all the links with 0 and nullptr;
    uint8_t* bufPtr = &(buffer.front());

    // Walk the array and allocate all the array links to Zero and nullptr
    T* ncells = nullptr;
    // int32_t* cells = nullptr;
    for(size_t i = 0; i < nElements; ++i)
    {
      ncells = reinterpret_cast<T*>(bufPtr + offset);
      this->m_Array[i].ncells = *ncells; // Set the number of cells in this link
      offset += 2;
      this->m_Array[i].cells = new K[(*ncells)];                                // Allocate a new chunk of memory to store the list
      ::memcpy(this->m_Array[i].cells, bufPtr + offset, (*ncells) * sizeof(K)); // Copy from the buffer into the new list memory
      offset += (*ncells) * sizeof(K);                                          // Increment the offset
    }
  }

  /**
   * @brief allocateLists
   * @param linkCounts
   */
  void allocateLists(QVector<T>& linkCounts)
  {
    allocate(linkCounts.size());
    for(typename std::vector<T>::size_type i = 0; i < linkCounts.size(); i++)
    {
      this->m_Array[i].ncells = linkCounts[i];
      this->m_Array[i].cells = new K[this->m_Array[i].ncells];
    }
  }

  /**
   * @brief allocateLists
   * @param linkCounts
   */
  void allocateLists(std::vector<T>& linkCounts)
  {
    allocate(linkCounts.size());
    for(typename std::vector<T>::size_type i = 0; i < linkCounts.size(); i++)
    {
      this->m_Array[i].ncells = linkCounts[i];
      this->m_Array[i].cells = new K[this->m_Array[i].ncells];
    }
  }

protected:
  DynamicListArray()
  : m_Array(nullptr)
  , m_Size(0)
  {
  }

  //----------------------------------------------------------------------------
  // This will allocate memory to hold all the NeighborList structures where each
  // structure is initialized to Zero Entries and a nullptr Pointer
  void allocate(size_t sz, size_t ext = 1000)
  {
    static typename DynamicListArray<T, K>::ElementList linkInit = {0, nullptr};

    // This makes sure we deallocate any lists that have been created
    for(size_t i = 0; i < this->m_Size; i++)
    {
      if(this->m_Array[i].cells != nullptr)
      {
        delete[] this->m_Array[i].cells;
      }
    }
    // Now delete all the "NeighborLists" structures
    if(this->m_Array != nullptr)
    {
      delete[] this->m_Array;
    }

    this->m_Size = sz;
    // Allocate a whole new set of structures
    this->m_Array = new typename DynamicListArray<T, K>::ElementList[sz];

    // Initialize each structure to have 0 entries and nullptr pointer.
    for(size_t i = 0; i < sz; i++)
    {
      this->m_Array[i] = linkInit;
    }
  }

private:
  ElementList* m_Array; // pointer to data
  size_t m_Size;
};

typedef DynamicListArray<int32_t, int32_t> Int32Int32DynamicListArray;
typedef DynamicListArray<uint16_t, int64_t> UInt16Int64DynamicListArray;
typedef DynamicListArray<int64_t, int64_t> Int64Int64DynamicListArray;

#endif /* _dynamicListArray_H_ */
