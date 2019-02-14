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

#include <sstream>
#include <stdexcept>
#include <string>

//#include "SIMPLib/DataContainers/DsnIterators.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::IDataStructureNode(const QString& name)
: m_Name(name)
{
  m_Parent = NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::IDataStructureNode(IDataStructureNode::WeakPointer parent, const QString& name)
: m_Name(name)
, m_Parent(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::~IDataStructureNode() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString IDataStructureNode::getName() const
{
  return m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool IDataStructureNode::setName(const QString& newName)
{
  Pointer parent = m_Parent.lock();
  if(nullptr != parent)
  {
    if(nullptr != (*parent)[newName])
    {
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::ParentType IDataStructureNode::getParent() const
{
  return m_Parent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IDataStructureNode::setParent(const ParentType& parent)
{
  m_Parent = parent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool IDataStructureNode::hasParent() const
{
  return !m_Parent.expired();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::children_collection IDataStructureNode::getChildren() const
{
  return m_Children;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::iterator IDataStructureNode::begin() noexcept
{
  return m_Children.begin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::const_iterator IDataStructureNode::begin() const noexcept
{
  return m_Children.begin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::const_iterator IDataStructureNode::cbegin() const noexcept
{
  return m_Children.cbegin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::iterator IDataStructureNode::end() noexcept
{
  return m_Children.end();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::const_iterator IDataStructureNode::end() const noexcept
{
  return m_Children.end();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::const_iterator IDataStructureNode::cend() const noexcept
{
  return m_Children.cend();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool IDataStructureNode::empty() const
{
  return m_Children.empty();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t IDataStructureNode::size() const
{
  return m_Children.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IDataStructureNode::clear() noexcept
{
  m_Children.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::const_iterator IDataStructureNode::find(const QString& name) const
{
  for(auto& iter = begin(); iter != end(); iter++)
  {
    if((*iter)->getName() == name)
    {
      return iter;
    }
  }

  return {};
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::Pointer& IDataStructureNode::operator[](size_t index)
{
  if(index < m_Children.size())
  {
    std::ostringstream ss;
    ss << "Index " << index << " is out of range for " << getName().toStdString()
      << "'s children collection.";
    throw std::out_of_range(ss.str());
  }

  return m_Children[index];
  return NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t IDataStructureNode::size() const
{
  return m_Children.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool IDataStructureNode::push_back(const Pointer& node)
{
  if(node == nullptr)
  {
    return false;
  }
  if(!acceptableChildType(node))
  {
    return false;
  }
  if(*find(node->getName()) != nullptr)
  {
    return false;
  }

  m_Children.push_back(node);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool IDataStructureNode::erase(iterator iter)
{
  m_Children.erase(iter);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool IDataStructureNode::erase(const_iterator iter)
{
  m_Children.erase(iter);
  return true;
}
