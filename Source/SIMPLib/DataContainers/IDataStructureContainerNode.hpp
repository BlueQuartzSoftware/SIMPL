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

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "IDataStructureNode.h"

template <class DerivedChild_t>
class IDataStructureContainerNode : public AbstractDataStructureContainer
{
public:
  using Self = IDataStructureContainerNode<DerivedChild_t>;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;

  using ChildShPtr = std::shared_ptr<DerivedChild_t>;
  using ChildCollection = std::vector<ChildShPtr>;
  using iterator = typename ChildCollection::iterator;
  using const_iterator = typename ChildCollection::const_iterator;
  using NameList = QList<QString>;

private:
  ChildCollection m_ChildrenNodes;

protected:
public:
  IDataStructureContainerNode(const QString& name = "")
  : AbstractDataStructureContainer(name)
  {
  }
  IDataStructureContainerNode(ParentType* parent, const QString& name = "")
  : AbstractDataStructureContainer(parent, name)
  {
  }
  ~IDataStructureContainerNode() override
  {
    clear();
  }

  /**
   * @brief Returns a copy of the children collection.
   * @return
   */
  constexpr const ChildCollection& getChildren() const
  {
    return m_ChildrenNodes;
  }

  /**
   * @brief Returns the names of all children nodes.
   * @return
   */
  NameList getNamesOfChildren() const
  {
    NameList names;
    for(const auto& child : m_ChildrenNodes)
    {
      names.push_back(child->getName());
    }
    return names;
  }

  /**
   * @brief Returns the DataArrayPaths of all descendants.
   * @return
   */
  DataArrayPathList getDescendantPaths() const
  {
    DataArrayPathList paths;

    for(const auto& child : m_ChildrenNodes)
    {
      paths.push_back(child->getDataArrayPath());
      // Check if child is a container node
      auto childContainer = std::dynamic_pointer_cast<IDataStructureContainerNode>(child);
      if(nullptr != childContainer)
      {
        paths.merge(childContainer->getDescendantPaths());
      }
    }

    return paths;
  }

  /**
   * @brief Returns a list of DataArrayPaths from both itself and all its descendants.
   * @return
   */
  DataArrayPathList getAllPaths() const
  {
    DataArrayPathList list = getDescendantPaths();
    list.push_front(getDataArrayPath());
    return list;
  }

  /**
   * @brief Returns an iterator pointing to the start of the children collection.
   * @return
   */
  constexpr iterator begin() noexcept
  {
    return m_ChildrenNodes.begin();
  }

  /**
   * @brief Returns a const iterator pointing to the start of the children collection.
   * @return
   */
  constexpr const_iterator begin() const noexcept
  {
    return m_ChildrenNodes.begin();
  }

  /**
   * @brief Returns a const iterator pointing to the start of the children collection.
   * @return
   */
  constexpr const_iterator cbegin() const noexcept
  {
    return m_ChildrenNodes.cbegin();
  }

  /**
   * @brief Returns an iterator pointing past the end of the children collection.
   * @return
   */
  constexpr iterator end() noexcept
  {
    return m_ChildrenNodes.end();
  }

  /**
   * @brief Returns a const iterator pointing past the end of the children collection.
   * @return
   */
  constexpr const_iterator end() const noexcept
  {
    return m_ChildrenNodes.end();
  }

  /**
   * @brief Returns a const iterator pointing past the end of the children collection.
   * @return
   */
  constexpr const_iterator cend() const noexcept
  {
    return m_ChildrenNodes.cend();
  }

  /**
   * @brief Returns true if the child collection is empty.  Returns false otherwise.
   * @return
   */
  constexpr bool empty() const
  {
    return m_ChildrenNodes.empty();
  }

  /**
   * @brief Returns the size of the children collection.
   * @return
   */
  constexpr size_t size() const
  {
    return m_ChildrenNodes.size();
  }

  /**
   * @brief Clears the children collection.  Items are not deleted unless this
   * was the last shared_ptr referencing them.
   */
  constexpr void clear() noexcept
  {
    auto children = getChildren();
    for(auto& child : children)
    {
      if(child != nullptr)
      {
        destroyParentConnection(child.get());
      }
    }
    m_ChildrenNodes.clear();
  }

  /**
   * @brief Returns the child node with the given name.  If no children nodes
   * are found with the given name, return nullptr.
   * @param name
   * @return
   */
  constexpr iterator find(const QString& name)
  {
    const auto hash = CreateStringHash(name);
    for(auto iter = begin(); iter != end(); iter++)
    {
      if((*iter)->checkNameHash(hash))
      {
        return iter;
      }
    }

    return end();
  }

  /**
   * @brief Returns the child node with the given name.  If no children nodes
   * are found with the given name, return nullptr.
   * @param name
   * @return
   */
  constexpr const_iterator find(const QString& name) const
  {
    const auto hash = CreateStringHash(name);
    for(auto iter = begin(); iter != end(); iter++)
    {
      if((*iter)->checkNameHash(hash))
      {
        return iter;
      }
    }

    return cend();
  }

  /**
   * @brief Returns the child with the given name as a shared_ptr.
   * If no child is found, return nullptr.
   * @param name
   * @return
   */
  constexpr ChildShPtr getChildByName(const QString& name) const
  {
    HashType nameHash = CreateStringHash(name);

    const auto& children = getChildren();
    for(const auto& child : children)
    {
      if(child->checkNameHash(nameHash))
      {
        return child;
      }
    }
    return nullptr;
  }

  /**
   * @brief Returns true if the container has a child node with the given name.
   * Returns false otherwise.
   * @param name
   * @return
   */
  constexpr bool contains(const QString& name) const
  {
    return getChildByName(name) != nullptr;
  }

  /**
   * @brief Returns true if the given object is one of the container's children.
   * Returns false otherwise.
   * @param obj
   * @return
   */
  constexpr bool contains(const ChildShPtr& obj) const
  {
    const auto& children = getChildren();
    for(const auto& child : children)
    {
      if(child == obj)
      {
        return true;
      }
    }
    return false;
  }

  /**
   * @brief Returns the index for the child with the given name.
   * Returns -1 if there is no child with the given name.
   * @param name
   * @return
   */
  constexpr int64_t getIndex(const QString& name) const
  {
    auto iter = find(name);
    if(iter == cend())
    {
      return -1;
    }
    return iter - begin();
  }

  /**
   * @brief Returns the child node at the given index.  If index is greater than
   * the specified index, throw out_of_range exception.
   * @param index
   * @return
   */
  constexpr ChildShPtr& operator[](size_t index)
  {
    if(index < 0 || index > m_ChildrenNodes.size())
    {
      const char msg[] = "Index is out of range for the IDataStructureContainerNode's children collection.";
      throw std::out_of_range(msg);
    }

    return m_ChildrenNodes[index];
  }

  /**
   * @brief Returns the child node with the given name.  If no children nodes
   * are found with the given name, return nullptr.
   * @param name
   * @return
   */
  constexpr ChildShPtr& operator[](const QString& name)
  {
    return operator[](getIndex(name));
  }

  /**
   * @brief Returns true if the container has a child by the given name.
   * Returns false otherwise.
   * @param name
   * @return
   */
  bool hasChildWithName(const QString& name) const override
  {
    return contains(name);
  }

  /**
   * @brief Attempts to append the given IDataStructureNode as a child.
   * Returns true if the process succeeded.  Returns false otherwise.
   * @param value
   * @return success
   */
  bool push_back(const ChildShPtr& node)
  {
    if(node.get() == nullptr)
    {
      return false;
    }
    if(contains(node->getName()))
    {
      return false;
    }
    typename ChildCollection::size_type size = m_ChildrenNodes.size();
    m_ChildrenNodes.push_back(node);

    createParentConnection(node.get(), this);
    return (size != m_ChildrenNodes.size());
  }

  /**
   * @brief Sets the given IDataStructureNode as a child.  If a child already
   * exists with the given name, it is removed.
   * @param node
   * @return success
   */
  bool insertOrAssign(const ChildShPtr& node)
  {
    // Can not insert a null IDataArray
    if(node.get() == nullptr)
    {
      return false;
    }

    auto iter = find(node->getName());
    if(iter != end())
    {
      if((*iter) != node)
      {
        erase(iter);
      }
      else
      {
        return true;
      }
    }

    m_ChildrenNodes.push_back(node);
    createParentConnection(node.get(), this);
    return true;
  }

  /**
   * @brief Erases the child at the given iterator
   * @param iter
   */
  void erase(iterator iter)
  {
    ChildShPtr child = (*iter);
    m_ChildrenNodes.erase(iter);
    destroyParentConnection(child.get());
  }

  /**
   * @brief Removes the given child from the children collection and returns its shared_ptr.
   * @param child
   * @return
   */
  IDataStructureNode::Pointer removeChildNode(const IDataStructureNode* rmChild) override
  {
    if(rmChild == nullptr)
    {
      return NullPointer();
    }

    for(auto iter = m_ChildrenNodes.begin(); iter != m_ChildrenNodes.end(); ++iter)
    {
      if((*iter).get() == rmChild)
      {
        ChildShPtr ptr = *iter;
        m_ChildrenNodes.erase(iter);
        return ptr;
      }
    }

    return NullPointer();
  }
};
