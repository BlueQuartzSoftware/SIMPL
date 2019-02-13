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

class DnsIterator;
class DnsConstIterator;

/**
 * @class IDataStructureNode IDataStructureNode.h SIMPLib/DataContainers/IDataStructureNode.h
 * @brief the IDataStructureNode class serves as the base class for data
 * structure classes from DataContainer to DataArray
 */
class IDataStructureNode
{
public:
  SIMPL_SHARED_POINTERS(IDataStructureNode)

  using iterator = DnsIterator;
  using const_iterator = DnsConstIterator;
  using ParentType = WeakPointer;
  using children_collection = std::vector<Pointer>;
  //using parent_collection = std::vector<ParentType>;

  IDataStructureNode();
  IDataStructureNode(IDataStructureNode::WeakPointer parent);
  virtual ~IDataStructureNode();

  /**
   * @brief Returns the parent node.  This currently only returns a single weak_ptr,
   * but as additional graph support is added, this will instead return a parent_collection.
   * @return
   */
  ParentType getParent() const;

  bool hasParent() const;

  iterator begin() noexcept;
  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept;

  iterator end() noexcept;
  const_iterator end() const noexcept;
  const_iterator cend() const noexcept;

  bool empty() const;
  size_t size() const;
  size_t max_size() const;

  void clear() noexcept;
  virtual void push_back(const IDataStructureNode& value) = 0;

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
  ParentType m_Parent;
  children_collection m_Children;
};
