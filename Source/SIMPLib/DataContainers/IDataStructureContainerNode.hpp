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

#include <vector>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "IDataStructureNode.h"

template <class DerivedChild_t> class IDataStructureContainerNode : public IDataStructureNode
{
public:
  using ChildShPtr = std::shared_ptr<DerivedChild_t>;
  using ChildCollection = std::vector<ChildShPtr>;
  using iterator = typename ChildCollection::iterator;
  using const_iterator =  typename ChildCollection::const_iterator;

private:
  ChildCollection m_Children;

  /**
   * @brief forceDerivedChildType throws a compile error if the child type is not an IDataStructureNode.
   * As a constexpr, this is run at compile time, and as noexcept, exceptions will cause termination.
   */
  constexpr void forceDerivedChildType() noexcept
  {
    IDataStructureNode* typeCheck = new DerivedChild_t();
  }

protected:
  /**
   * @brief Removes the given child from the children collection and returns its shared_ptr.
   * @param child
   * @return
   */
  IDataStructureNode::Pointer removeChild(const IDataStructureNode* rmChild) override
  {
    for(auto iter = m_Children.begin(); iter != m_Children.end(); ++iter)
    {
      if((*iter).get() == rmChild)
      {
        ChildShPtr ptr = *iter;
        m_Children.erase(iter);
        return ptr;
      }
    }

    return NullPointer();
  }

  //void setParent(const ParentType& parent) override
  //{
  //  // Remove from parent's children
  //  Pointer tempPtr;
  //  Pointer parentPtr = m_Parent.lock();
  //  if(nullptr != parentPtr)
  //  {
  //    tempPtr = parentPtr->removeChild(this);
  //  }

  //  m_Parent = parent;
  //}

public:
  IDataStructureContainerNode(const QString& name = "")
    : IDataStructureNode(name)
  {}
  IDataStructureContainerNode(IDataStructureNode::WeakPointer parent, const QString& name = "")
    : IDataStructureNode(parent, name)
  {}
  ~IDataStructureContainerNode() override = default;

  /**
   * @brief Returns a copy of the children collection.
   * @return
   */
  constexpr ChildCollection getChildren() const
  {
    return m_Children;
  }

  /**
   * @brief Returns an iterator pointing to the start of the children collection.
   * @return
   */
  constexpr iterator begin() noexcept
  {
    return m_Children.begin();
  }

  /**
   * @brief Returns a const iterator pointing to the start of the children collection.
   * @return
   */
  constexpr const_iterator begin() const noexcept
  {
    return m_Children.begin();
  }

  /**
   * @brief Returns a const iterator pointing to the start of the children collection.
   * @return
   */
  constexpr const_iterator cbegin() const noexcept
  {
    return m_Children.cbegin();
  }

  /**
   * @brief Returns an iterator pointing past the end of the children collection.
   * @return
   */
  constexpr iterator end() noexcept
  {
    return m_Children.end();
  }

  /**
   * @brief Returns a const iterator pointing past the end of the children collection.
   * @return
   */
  constexpr const_iterator end() const noexcept
  {
    return m_Children.end();
  }

  /**
   * @brief Returns a const iterator pointing past the end of the children collection.
   * @return
   */
  constexpr const_iterator cend() const noexcept
  {
    return m_Children.cend();
  }

  /**
   * @brief Returns true if the child collection is empty.  Returns false otherwise.
   * @return
   */
  constexpr bool empty() const
  {
    return m_Children.empty();
  }

  /**
   * @brief Returns the size of the children collection.
   * @return
   */
  constexpr size_t size() const
  {
    return m_Children.size();
  }

  /**
   * @brief Clears the children collection.  Items are not deleted unless this
   * was the last shared_ptr referencing them.
   */
  constexpr void clear() noexcept
  {
    m_Children.clear();
  }

  /**
   * @brief Returns the child node with the given name.  If no children nodes
   * are found with the given name, return nullptr.
   * @param name
   * @return
   */
  constexpr iterator find(const QString& name)
  {
    for(auto iter = begin(); iter != end(); iter++)
    {
      if((*iter)->getName() == name)
      {
        return iter;
      }
    }

    return {};
  }

  /**
   * @brief Returns the child node with the given name.  If no children nodes
   * are found with the given name, return nullptr.
   * @param name
   * @return
   */
  constexpr const_iterator find(const QString& name) const
  {
    for(auto iter = begin(); iter != end(); iter++)
    {
      if((*iter)->getName() == name)
      {
        return iter;
      }
    }

    return {};
  }

  /**
   * @brief Returns the child node at the given index.  If index is greater than
   * the specified index, throw out_of_range exception.
   * @param index
   * @return
   */
  constexpr ChildShPtr& operator[](size_t index)
  {
    if(index < m_Children.size())
    {
      const char msg[] = "Index is out of range for the IDataStructureContainerNode's children collection.";
      throw std::out_of_range(msg);
    }

    return m_Children[index];
  }

  /**
   * @brief Returns the child node with the given name.  If no children nodes
   * are found with the given name, return nullptr.
   * @param name
   * @return
   */
  constexpr ChildShPtr& operator[](const QString& name)
  {
    return *find(name);
  }

  /**
   * @brief Returns true if the container has a child by the given name.
   * Returns false otherwise.
   * @param name
   * @return
   */
  bool hasChildWithName(const QString& name) const
  {
    return *find(name) != nullptr;
  }

  /**
   * @brief Attempts to append the given IDataStructureNode as a child.
   * Returns true if the process succeeded.  Returns false otherwise.
   * @param value
   * @return success
   */
  constexpr bool push_back(const ChildShPtr& node)
  {
    if(std::dynamic_pointer_cast<ChildShPtr>(node) == nullptr)
    {
      return false;
    }
    if(*find(node->getName()) != nullptr)
    {
      return false;
    }

    m_Children.push_back(node);
  }

  constexpr void erase(iterator iter)
  {
    m_Children.erase(iter);
  }
  constexpr void erase(const_iterator iter)
  {
    m_Children.erase(iter);
  }
};
