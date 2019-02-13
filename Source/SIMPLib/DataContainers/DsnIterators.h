/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>
#include <vector>

class IDataStructureNode;

/**
 * @brief The DsnIterator is a bidirectional iterator over data structure types
 * belonging to the same level.  DataContainerArray iterators only iterate over
 * DataContainers.  DataContainer iterators only iterate over their AttributeMatrices.
 * AttributeMatrix iterators only iterate over their DataArrays.
 * This was made constexpr for the sake of creating faster runtime operations.
 */
class DsnIterator
{
  using self_type = DsnIterator;
  using value_type = IDataStructureNode*;
  using reference = value_type&;
  using collection = std::vector<std::shared_ptr<IDataStructureNode>>;

public:
  DsnIterator(const collection& col, size_t index)
  : m_Collection(col)
  , m_Index(index)
  {}
  ~DsnIterator() = default;

  /**
   * @brief Returns the next iterator in the collection.
   * @return
   */
  constexpr self_type& operator++(int) const
  {
    return self_type(m_Collection, m_Index + 1);
  }

  /**
   * @brief Returns the previous iterator in the collection.
   * @return
   */
  constexpr self_type& operator--(int) const
  {
    return self_type(m_Collection, m_Index - 1);
  }

  /**
   * @brief Returns the stored value.
   * @return
   */
  constexpr value_type operator*() const
  {
    return m_Collection[m_Index].get();
  }

  /**
   * @brief Returns the stored value.
   * @return
   */
  constexpr value_type operator->() const
  {
    return m_Collection[m_Index].get();
  }

  /**
    * @brief Returns true if other iterator points to the same index in same collection.
    * @param rhs right hand side
    * @return
    */
  constexpr bool operator==(const self_type& rhs) const
  {
    return (m_Index == rhs.m_Index) && (m_Collection == rhs.m_Collection);
  }

  /**
   * @brief Returns true if the other iterator does not point to the same index and collection.
   * @param rhs right hand side
   * @return
   */
  bool operator!=(const self_type& rhs) const
  {
    return !operator==(rhs);
  }

private:
  collection m_Collection;
  const size_t m_Index;
};


/**
 * @brief The DsnConstIterator is a bidirectional const iterator over data structure types
 * belonging to the same level.  DataContainerArray iterators only iterate over
 * DataContainers.  DataContainer iterators only iterate over their AttributeMatrices.
 * AttributeMatrix iterators only iterate over their DataArrays.
 * This was made constexpr for the sake of creating faster runtime operations.
 */
class DsnConstIterator
{
  using self_type = DsnConstIterator;
  using value_type = const IDataStructureNode*;
  using collection = std::vector<std::shared_ptr<IDataStructureNode>>;

public:
  DsnConstIterator(const collection& col, size_t index)
    : m_Collection(col)
    , m_Index(index)
  {}
  ~DsnConstIterator() = default;

  /**
   * @brief Returns the next iterator in the collection
   * @return
   */
  constexpr self_type& operator++(int) const
  {
    return self_type(m_Collection, m_Index + 1);
  }

  /**
   * @brief Returns the previous iterator in the collection
   * @return
   */
  constexpr self_type& operator--(int) const
  {
    return self_type(m_Collection, m_Index - 1);
  }

  /**
   * @brief Returns a the stored value as const.
   * @return
   */
  constexpr value_type operator*() const
  {
    return m_Collection[m_Index].get();
  }

  /**
   * @brief Returns the value as const.
   * @return
   */
  constexpr value_type operator->() const
  {
    return m_Collection[m_Index].get();
  }

  /**
   * @brief Returns true if the other iterator points to the same index and collection.
   * Returns false otherwise.
   * @param rhs right hand side
   * @return
   */
  constexpr bool operator==(const self_type& rhs) const
  {
    return (m_Index == rhs.m_Index) && (m_Collection == rhs.m_Collection);
  }

  /**
   * @brief Returns true if the other iterator does not point to the same index and collection.
   * Returns false otherwise.
   * @param rhs right hand side
   * @return
   */
  bool operator!=(const self_type& rhs) const
  {
    return !operator==(rhs);
  }

private:
  const collection m_Collection;
  const size_t m_Index;
};
