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
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/SIMPLib.h"

class AbstractDataStructureContainer;

/**
 * @class IDataStructureNode IDataStructureNode.h SIMPLib/DataContainers/IDataStructureNode.h
 * @brief the IDataStructureNode class serves as the base class for data
 * structure classes from DataContainer to DataArray
 */
class SIMPLib_EXPORT IDataStructureNode
{
public:
  SIMPL_SHARED_POINTERS(IDataStructureNode)

  using ParentType = AbstractDataStructureContainer;
  // using ParentWkPtr = std::weak_ptr<ParentType>;
  // using parent_collection = std::vector<ParentWkPtr>;
  using DataArrayPathList = std::list<DataArrayPath>;
  using HashType = size_t;

  friend AbstractDataStructureContainer;

private:
  QString m_Name;
  ParentType* m_Parent = nullptr;
  HashType m_NameHash = 0;

  /**
   * @brief Updates the name hash variable based on the current name.
   */
  void updateNameHash();

  /**
   * @brief Sets the raw parent pointer.  As additional graph support is added,
   * this will instead be replaced with addParentNode(const ParentType&) and
   * removeParentNode(const ParentType&).  This should only be called from
   * IDataStructureContainerNode::push_back, insertOrAssign, or
   * IDataStructureNode::clearParentNode.  THIS DOES NOT ADD THE NODE TO THE TARGET PARENT!
   * @param parent
   */
  void setParentNode(ParentType* const parent);

  /**
   * @brief Clears the parent pointer and removes this from its parent constainer.
   */
  inline void clearParentNode()
  {
    setParentNode(nullptr);
  }

  // inline void addParentNode(const ParentType& newParent);
  // inline bool removeParentNode(const ParentType& removedParent);

protected:
  /**
   * @brief Inline method for creating hashes from QString values.
   * @param string
   * @return
   */
  static HashType CreateStringHash(const QString& string);

public:
  IDataStructureNode(const QString& name = "");
  IDataStructureNode(ParentType* parent, const QString& name = "");

  virtual ~IDataStructureNode();

  /**
   * @brief Checks the name hash for equality.  Returns true if they are equal.
   * Returns false otherwise.
   * @param nameHash
   * @return
   */
   bool checkNameHash(size_t nameHash) const
  {
    return m_NameHash == nameHash;
  }

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
   * @brief Returns the parent node.  This currently only returns a single pointer,
   * but as additional graph support is added, this will instead return a parent_collection.
   * @return
   */
  ParentType* getParentNode() const;

  /**
   * @brief Returns the data structure node's DataArrayPath.
   * @return
   */
  virtual DataArrayPath getDataArrayPath() const = 0;

  /**
   * @brief Returns the parent node's DataArrayPath.  If no parent node exists, return an empty path;
   * @return
   */
  DataArrayPath getParentPath() const;

  /**
   * @brief Returns true if a parent node exists.  Returns false otherwise.
   * @return
   */
   bool hasParent() const
  {
    return m_Parent != nullptr;
  }
};

/**
 * @class AbstractDataStructureContainer IDataStructureNode.h SIMPLib/DataContainers/IDataStructureNode.h
 * @brief The AbstractDataStructureContainer is an interface created so that IDataStructureNode can call required methods in IDataStructureContainerNode.
 */
class SIMPLib_EXPORT AbstractDataStructureContainer : public IDataStructureNode
{
public:
  AbstractDataStructureContainer(const QString& name = "")
  : IDataStructureNode(name)
  {
  }
  AbstractDataStructureContainer(ParentType* parent, const QString& name = "")
  : IDataStructureNode(parent, name)
  {
  }
  ~AbstractDataStructureContainer() override = default;

  /**
   * @brief Returns true if the container has a child node with the given name.
   * @param name
   * @return
   */
  virtual bool hasChildWithName(const QString& name) const = 0;

  /**
   * @brief Removes the target node from the container's list of children.
   * Returns the removed node as a shared pointer.
   * @param rmChild
   * @return
   */
  virtual IDataStructureNode::Pointer removeChildNode(const IDataStructureNode* rmChild) = 0;

protected:
  /**
   * @brief Sets the child's parent container.  This does not add the child to the parent's collection.
   * THIS METHOD IS ONLY USED BY IDataStructureNode<T> AND SHOULD NOT BE USED BY ANY CLASS THAT DERIVES FROM IT.
   * @param child
   * @param parent
   */
  void createParentConnection(IDataStructureNode* child, AbstractDataStructureContainer* parent) const;
  
  /**
   * @brief Clears the child's parent pointer.  This does not remove the child from the parent's collection.
   * THIS METHOD IS ONLY USED BY IDataStructureNode<T> AND SHOULD NOT BE USED BY ANY CLASS THAT DERIVES FROM IT.
   * @param child
   */
  void destroyParentConnection(IDataStructureNode* child) const;
};
