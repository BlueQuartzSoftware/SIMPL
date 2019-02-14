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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

// class DsnIterator;
// class DsnConstIterator;
class IDataStructureNode;
#include "SIMPLib/DataContainers/IDataStructureContainerNode.hpp"

/**
 * @class IDataStructureNode IDataStructureNode.h SIMPLib/DataContainers/IDataStructureNode.h
 * @brief the IDataStructureNode class serves as the base class for data
 * structure classes from DataContainer to DataArray
 */
class IDataStructureNode
{
public:
  SIMPL_SHARED_POINTERS(IDataStructureNode)

  using ParentType = IDataStructureContainerNode<IDataStructureNode>;
  using ParentPointer = std::shared_ptr<ParentType>;
  using ParentWkPtr = std::weak_ptr<ParentType>;
  // using parent_collection = std::vector<ParentType>;

private:
  QString m_Name;
  ParentWkPtr m_Parent;

protected:
  inline virtual Pointer removeChild(const IDataStructureNode& rmChild)
  {
    return NullPointer();
  }

  /**
   * @brief Sets the parent weak_ptr.  As additional graph support is added,
   * this will instead be replaced with addParent(const ParentType&) and
   * removeParent(const ParentType&)
   */
  void setParent(const ParentWkPtr& parent);

  // constexpr void addParent(const ParentType& newParent);
  // constexpr bool removeParent(const ParentType& removedParent);

public:
  IDataStructureNode(const QString& name = "");
  IDataStructureNode(ParentWkPtr parent, const QString& name = "");

  virtual ~IDataStructureNode() = default;

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
  ParentWkPtr getParent() const;

  /**
   * @brief Returns true if a parent node exists.  Returns false otherwise.
   * @return
   */
  constexpr bool hasParent() const
  {
    return !m_Parent.expired();
  }
};
