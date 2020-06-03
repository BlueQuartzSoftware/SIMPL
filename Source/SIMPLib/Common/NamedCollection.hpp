/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>
#include <set>
#include <type_traits>

#include <QtCore/QString>

#include "SIMPLib/Common/INamedObject.h"

//#include "SIMPLib/Montages/AbstractMontage.h"

/**
 * @class NamedCollection NamedCollection.hpp SIMPLib/Common/NamedCollection.hpp
 * @brief NamedCollection is a templated set of named objects of a certain type.
 * By deriving from INamedCollection, the items in the collection can keep track
 * of what collections they are in.  This, along with the custom hashing in
 * INamedObject, allows the collection to be treated like a map without separating
 * the key and value.
 */
template <class T>
class NamedCollection : public INamedCollection
{
  using Self = NamedCollection<T>;

public:
  using ChildType = typename std::shared_ptr<T>;
  using Collection = typename std::set<ChildType>;
  using Iterator = typename Collection::iterator;
  // using ConstIterator = typename Collection::const_iterator;

private:
  Collection m_Items;

public:
  /* --------------- Begin STL iterator support ------------------ */
  /**
   * @brief Returns an iterator to the first item in the collection.
   * @return
   */
  Iterator begin()
  {
    return m_Items.begin();
  }

  /**
   * @brief Returns an iterator to the last item in the collection.
   * @return
   */
  Iterator end()
  {
    return m_Items.end();
  }

  /**
   * @brief Returns a const iterator to the first item in the collection.
   * @return
   */
  Iterator begin() const
  {
    return m_Items.begin();
  }

  /**
   * @brief Returns a const iterator to the last item in the collection.
   * @return
   */
  Iterator end() const
  {
    return m_Items.end();
  }
  /* --------------- End STL iterator support ------------------ */

  /**
   * @brief Removes all items from the collection.
   */
  void clear() override
  {
    for(const auto& item : m_Items)
    {
      handleRemovingObject(item.get());
    }
    m_Items.clear();
  }

  /**
   * @brief Default constructor
   */
  NamedCollection() = default;
  // NamedCollection(const NamedCollection<T>& other)
  //: m_Items(other.m_Items)
  //{
  //  for(const auto& item : *this)
  //  {
  //    handleAddingObject(item);
  //  }
  //}

  /**
   * @brief Remove all children from the collection before it is destroyed.
   */
  ~NamedCollection() override
  {
    clear();
  }

  /**
   * @brief Returns the size of the collection.
   * @return
   */
  size_t size() const override
  {
    return m_Items.size();
  }

  /**
   * @brief Returns true if the collection is empty.
   * @return
   */
  bool isEmpty() const override
  {
    return m_Items.empty();
  }

  /**
   * @brief Searches the collection for an object with the given name.
   * @param name
   * @return
   */
  Iterator find(const QString& name) const
  {
    // Hash the target name to reduce the time required for each item
    INamedObject::HashType targetHash = INamedObject::CreateHash(name);
    auto unaryPredicate = [name, targetHash](const ChildType& obj) {
      if(obj->getNameHash() == targetHash)
      {
        return true;
      }
      return false;
    };

    return std::find_if(begin(), end(), unaryPredicate);
  }

  /**
   * @brief Erases any item with the given name.
   * @param name
   */
  void erase(const QString& name) override
  {
    Iterator iter = find(name);
    if(iter == end())
    {
      return;
    }

    handleRemovingObject((*iter).get());
    m_Items.erase(iter);
  }

  /**
   * @brief Attempts to insert the given item into the collection.
   * This will fail if an item with the same name already exists in the collection.
   * @return
   */
  bool insert(const ChildType& newItem)
  {
    if(nullptr == newItem)
    {
      return false;
    }
    if(!m_Items.insert(newItem).second)
    {
      return false;
    }

    handleAddingObject(newItem.get());
    return true;
  }

  /**
   * @brief Adds the given item to the collection. If an item with the same name already exists, it is removed.
   * @param newItem
   */
  void addOrReplace(const ChildType& newItem)
  {
    erase(newItem->getName());
    insert(newItem);
  }

  /**
   * @brief Returns true if the collection contains an object with the given name.  Returns false otherwise.
   * @param name
   * @return
   */
  bool contains(const QString& name) const override
  {
    return find(name) != end();
  }

  /**
   * @brief Returns a list of all the names used in the collection.
   * @return
   */
  NameList getNameList() const override
  {
    NameList names;
    for(const auto& obj : m_Items)
    {
      names.push_back(obj->getName());
    }

    return names;
  }

  /**
   * @brief Lexicographically compares collections.  Returns true if the items in
   * this collection are less than the other's.  Returns false otherwise.
   * @param other
   * @return
   */
  bool operator<(const Self& other) const
  {
    return m_Items < other.m_Items;
  }

  /**
   * @brief Lexicographically compares collections.  Returns true if the items in
   * this collection are equal to the other's.  Returns false otherwise.
   * @param other
   * @return
   */
  bool operator==(const Self& other) const
  {
    return m_Items == other.m_Items;
  }
};
