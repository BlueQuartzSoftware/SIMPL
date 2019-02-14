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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

//class DsnIterator;
//class DsnConstIterator;

/**
 * @class IDataStructureNode IDataStructureNode.h SIMPLib/DataContainers/IDataStructureNode.h
 * @brief the IDataStructureNode class serves as the base class for data
 * structure classes from DataContainer to DataArray
 */
class IDataStructureNode
{
public:
  SIMPL_SHARED_POINTERS(IDataStructureNode)

  using children_collection = std::vector<Pointer>;
  // using parent_collection = std::vector<ParentType>;
  using iterator = children_collection::iterator;
  using const_iterator = children_collection::const_iterator;
  using ParentType = WeakPointer;

  IDataStructureNode(const QString& name = "");
  IDataStructureNode(IDataStructureNode::WeakPointer parent, const QString& name = "");
  virtual ~IDataStructureNode();

  /**
   * @brief Returns the node's name.
   * @return
   */
  QString getName() const;

  /**
   * @brief Attempts to rename the container.  Returns true if the operation succeeded.
   * Returns false otherwise.
   * @param newName
   * @return
   */
  bool setName(const QString& newName);

  /**
   * @brief Returns the parent node.  This currently only returns a single weak_ptr,
   * but as additional graph support is added, this will instead return a parent_collection.
   * @return
   */
  ParentType getParent() const;

  /**
   * @brief Returns true if a parent node exists.  Returns false otherwise.
   * @return
   */
  bool hasParent() const;

  /**
   * @brief Returns a copy of the children collection.
   * @return
   */
  children_collection getChildren() const;

  /**
   * @brief Returns an iterator pointing to the start of the children collection.
   * @return
   */
  iterator begin() noexcept;

  /**
   * @brief Returns a const iterator pointing to the start of the children collection.
   * @return
   */
  const_iterator begin() const noexcept;

  /**
   * @brief Returns a const iterator pointing to the start of the children collection.
   * @return
   */
  const_iterator cbegin() const noexcept;

  /**
   * @brief Returns an iterator pointing past the end of the children collection.
   * @return
   */
  iterator end() noexcept;

  /**
   * @brief Returns a const iterator pointing past the end of the children collection.
   * @return
   */
  const_iterator end() const noexcept;

  /**
   * @brief Returns a const iterator pointing past the end of the children collection.
   * @return
   */
  const_iterator cend() const noexcept;

  /**
   * @brief Returns true if the child collection is empty.  Returns false otherwise.
   * @return
   */
  bool empty() const;

  /**
   * @brief Returns the size of the children collection.
   * @return
   */
  size_t size() const;

  /**
   * @brief Clears the children collection.  Items are not deleted unless this
   * was the last shared_ptr referencing them.
   */
  void clear() noexcept;

  /**
   * @brief Returns the child node with the given name.  If no children nodes
   * are found with the given name, return nullptr.
   * @param name
   * @return
   */
  const_iterator find(const QString& name) const;

  /**
   * @brief Returns the child node at the given index.  If index is greater than
   * the specified index, throw out_of_range exception.
   * @param index
   * @return
   */
  Pointer& operator[](size_t index);

  /**
   * @brief Returns the child node with the given name.  If no children nodes
   * are found with the given name, return nullptr.
   * @param name
   * @return
   */
  inline Pointer& operator[](const QString& name) { return *find(name); }

  size_t size() const;

  /**
   * @brief Attempts to append the given IDataStructureNode as a child.
   * Returns true if the process succeeded.  Returns false otherwise.
   * @param value
   * @return success
   */
  virtual bool push_back(const Pointer& value);

  bool erase(iterator pos);
  bool erase(const_iterator pos);

  /**
   * @brief Returns true if the given type_info is acceptable as a child node.
   */
  virtual bool acceptableChildType(const std::type_info& type) = 0;

  /**
   * @brief Returns true if the given node is of the correct type of be added
   * as a child node.  This is an overloaded method.
   */
  inline bool acceptableChildType(const Pointer& node) { return acceptableChildType(typeid(node)); }

protected:
  /**
   * @brief Sets the parent weak_ptr.  As additional graph support is added,
   * this will instead be replaced with addParent(const ParentType&) and
   * removeParent(const ParentType&)
   */
  void setParent(const ParentType& parent);

  // void addParent(const ParentType& newParent);
  // bool removeParent(const ParentType& removedParent);

private:
  QString m_Name;
  ParentType m_Parent;
  children_collection m_Children;
};
