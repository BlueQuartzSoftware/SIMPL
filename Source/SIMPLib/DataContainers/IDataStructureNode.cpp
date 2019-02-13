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

#include "IDataStructureNode.h"

IDataStructureNode::IDataStructureNode()
{
  m_Parent = NullPointer();
}
IDataStructureNode::IDataStructureNode(IDataStructureNode::WeakPointer parent)
  : m_Parent(parent)
{
}
IDataStructureNode::~IDataStructureNode() = default;

IDataStructureNode::WeakPointer IDataStructureNode::getParent() const;
void IDataStructureNode::setParent(IDataStructureNode::Pointer parent);
bool IDataStructureNode::hasParent() const
{
  return !m_Parent.expired();
}

IDataStructureNode::iterator IDataStructureNode::begin() noexcept;
IDataStructureNode::const_iterator IDataStructureNode::begin() const noexcept;
IDataStructureNode::const_iterator IDataStructureNode::cbegin() const noexcept;

IDataStructureNode::iterator IDataStructureNode::end() noexcept;
IDataStructureNode::const_iterator IDataStructureNode::end() const noexcept;
IDataStructureNode::const_iterator IDataStructureNode::cend() const noexcept;

IDataStructureNode::bool IDataStructureNode::empty() const;
IDataStructureNode::size_t IDataStructureNode::size() const;
IDataStructureNode::size_t IDataStructureNode::max_size() const;

void IDataStructureNode::clear() noexcept;