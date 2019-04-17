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

#include <iterator>
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
class DsnIterator : public std::iterator<std::random_access_iterator_tag, std::shared_ptr<IDataStructureNode>, size_t> // std::iterator_traits<std::shared_ptr<IDataStructureNode>>
{
  using self_type = DsnIterator;
  using difference_type = size_t;
  using value_type = std::shared_ptr<IDataStructureNode>;
  using pointer = IDataStructureNode*;
  using reference = value_type&;
  using collection_type = std::vector<value_type>;
  using iterator_category = std::random_access_iterator_tag;

public:
  /**
   * @brief Default constructor creates an invalid iterator
   */
  DsnIterator() = default;

  /**
   * @brief Creates an iterator for the given collection and index
   * @param collection
   * @param index
   */
  DsnIterator(const collection_type& collection, size_t index)
  : m_Collection(collection)
  , m_Index(index)
  {
  }

  /**
   * @brief Copy iterator
   * @param other
   */
  DsnIterator(const self_type& other)
  : m_Collection(other.m_Collection)
  , m_Index(other.m_Index)
  {
  }

  /**
   * @brief Destructor
   */
  ~DsnIterator() = default;

  /**
   * @brief Returns the iterator's index.
   * @return
   */
  constexpr difference_type position() const
  {
    return m_Index;
  }

  /**
   * @brief Returns the next iterator in the collection.
   * @return
   */
  constexpr self_type& operator++(int)
  {
    m_Index++;
    return *this;
  }

  /**
   * @brief Returns the previous iterator in the collection.
   * @return
   */
  constexpr self_type& operator--(int)
  {
    m_Index--;
    return *this;
  }

  /**
   * @brief Proceeds n positions.
   * @param n
   * @return
   */
  constexpr self_type& operator+(difference_type n)
  {
    m_Index += n;
    return *this;
  }

  /**
   * @brief Regresses n positions.
   * @param n
   * @return
   */
  constexpr self_type& operator-(difference_type n)
  {
    if(n > m_Index)
    {
      throw std::out_of_range("Iterator out of range");
    }
    m_Index -= n;
    return *this;
  }

  /**
   * @brief Returns the difference in position with the given iterator.
   * @param other
   * @return
   */
  constexpr difference_type operator-(const self_type& other)
  {
    return m_Index - other.m_Index;
  }

  /**
   * @brief Returns the stored value.
   * @return
   */
  constexpr reference operator*()
  {
    return m_Collection[m_Index];
  }

  /**
   * @brief Returns the stored value.
   * @return
   */
  constexpr pointer operator->()
  {
    return m_Collection[m_Index].get();
  }

  /**
   * @brief Returns true if the given iterator comes later in the collection.
   * Returns false otherwise.
   * @param other
   * @return
   */
  constexpr bool operator<(const self_type& other) const
  {
    return m_Index < other.m_Index;
  }

  /**
   * @brief Returns true if the given iterator comes later or equal to the iterator in the collection.
   * Returns false otherwise.
   * @param other
   * @return
   */
  constexpr bool operator<=(const self_type& other) const
  {
    return m_Index <= other.m_Index;
  }

  /**
   * @brief Returns true if the given iterator comes earlier in the collection.
   * Returns false otherwise.
   * @param other
   * @return
   */
  constexpr bool operator>(const self_type& other) const
  {
    return m_Index > other.m_Index;
  }

  /**
   * @brief Returns true if the given iterator comes earlier or equal to the iterator in the collection.
   * Returns false otherwise.
   * @param other
   * @return
   */
  constexpr bool operator>=(const self_type& other) const
  {
    return m_Index >= other.m_Index;
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

  /**
   * @brief Swaps values with the given iterator.
   * @param other
   */
  inline void swap(self_type& other) noexcept
  {
    const collection_type tempCollection = m_Collection;
    m_Collection = other.m_Collection;
    other.m_Collection = tempCollection;

    const difference_type tempIndex = m_Index;
    m_Index = other.m_Index;
    other.m_Index = tempIndex;
  }

private:
  collection_type m_Collection;
  difference_type m_Index;
};

/**
 * @brief The DsnConstIterator is a bidirectional const iterator over data structure types
 * belonging to the same level.  DataContainerArray iterators only iterate over
 * DataContainers.  DataContainer iterators only iterate over their AttributeMatrices.
 * AttributeMatrix iterators only iterate over their DataArrays.
 * This was made constexpr for the sake of creating faster runtime operations.
 */
class DsnConstIterator : public std::iterator<std::random_access_iterator_tag, const std::shared_ptr<IDataStructureNode>, size_t> // std::iterator_traits<const std::shared_ptr<IDataStructureNode>>
{
  using self_type = DsnConstIterator;
  using difference_type = size_t;
  using value_type = const std::shared_ptr<IDataStructureNode>;
  using pointer = const IDataStructureNode*;
  using reference = value_type&;
  using collection_type = std::vector<std::shared_ptr<IDataStructureNode>>;
  using iterator_category = std::random_access_iterator_tag;

public:
  /**
   * @brief Calling the default constructor will return an invalid iterator.
   */
  DsnConstIterator() = default;

  /**
   * @brief Constructor for the given collection and index
   * @param collection
   * @param index
   */
  DsnConstIterator(const collection_type& collection, size_t index)
  : m_Collection(collection)
  , m_Index(index)
  {
  }

  /**
   * @brief Copy constructor
   * @param other
   */
  DsnConstIterator(const self_type& other)
  : m_Collection(other.m_Collection)
  , m_Index(other.m_Index)
  {
  }

  /**
   * @brief Destructor
   */
  ~DsnConstIterator() = default;

  /**
   * @brief Returns the iterator's index.
   * @return
   */
  constexpr difference_type position() const
  {
    return m_Index;
  }

  /**
   * @brief Returns the next iterator in the collection
   * @return
   */
  constexpr self_type& operator++()
  {
    m_Index++;
    return *this;
  }

  /**
   * @brief Returns the previous iterator in the collection
   * @return
   */
  constexpr self_type& operator--(int)
  {
    m_Index--;
    return *this;
  }

  /**
   * @brief Proceeds n positions.
   * @param n
   * @return
   */
  constexpr self_type& operator+(difference_type n)
  {
    m_Index += n;
    return *this;
  }

  /**
   * @brief Regresses n positions
   * @param n
   * @return
   */
  constexpr self_type& operator-(difference_type n)
  {
    if(n > m_Index)
    {
      throw std::out_of_range("Iterator out of range");
    }
    m_Index -= n;
    return *this;
  }

  /**
   * @brief Returns the difference in position with the given iterator.
   * @param other
   * @return
   */
  constexpr difference_type operator-(const self_type& other)
  {
    return m_Index - other.m_Index;
  }

  /**
   * @brief Returns a the stored value as const.
   * @return
   */
  constexpr reference operator*() const
  {
    return m_Collection[m_Index];
  }

  /**
   * @brief Returns the value as const.
   * @return
   */
  constexpr pointer operator->() const
  {
    return m_Collection[m_Index].get();
  }

  /**
   * @brief Returns true if the given iterator comes later in the collection.
   * Returns false otherwise.
   * @param other
   * @return
   */
  constexpr bool operator<(const self_type& other) const
  {
    return m_Index < other.m_Index;
  }

  /**
   * @brief Returns true if the given iterator comes later or equal to the iterator in the collection.
   * Returns false otherwise.
   * @param other
   * @return
   */
  constexpr bool operator<=(const self_type& other) const
  {
    return m_Index <= other.m_Index;
  }

  /**
   * @brief Returns true if the given iterator comes earlier in the collection.
   * Returns false otherwise.
   * @param other
   * @return
   */
  constexpr bool operator>(const self_type& other) const
  {
    return m_Index > other.m_Index;
  }

  /**
   * @brief Returns true if the given iterator comes earlier or equal to the iterator in the collection.
   * Returns false otherwise.
   * @param other
   * @return
   */
  constexpr bool operator>=(const self_type& other) const
  {
    return m_Index >= other.m_Index;
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

  /**
   * @brief Swaps values with the given iterator.
   * @param other
   */
  inline void swap(self_type& other) noexcept
  {
    const collection_type tempCollection = m_Collection;
    m_Collection = other.m_Collection;
    other.m_Collection = tempCollection;

    const difference_type tempIndex = m_Index;
    m_Index = other.m_Index;
    other.m_Index = tempIndex;
  }

private:
  collection_type m_Collection;
  size_t m_Index;
};
