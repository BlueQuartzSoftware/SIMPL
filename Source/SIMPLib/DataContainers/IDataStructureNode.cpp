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
IDataStructureNode::HashType IDataStructureNode::CreateStringHash(const QString& string)
{
  std::hash<std::string> hashFn;
  return hashFn(string.toStdString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::IDataStructureNode(const QString& name)
: m_Name(name)
, m_Parent(nullptr)
{
  updateNameHash();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::IDataStructureNode(ParentType* parent, const QString& name)
: m_Name(name)
, m_Parent(parent)
{
  updateNameHash();

  // Add to parent's children
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::~IDataStructureNode()
{
  if(m_Parent != nullptr)
  {
    m_Parent->removeChildNode(this);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IDataStructureNode::updateNameHash()
{
  m_NameHash = CreateStringHash(getName());
}

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
  if(nullptr == m_Parent)
  {
    m_Name = newName;
    updateNameHash();
    return true;
  }
  else if(!m_Parent->hasChildWithName(newName))
  {
    m_Name = newName;
    updateNameHash();
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataStructureNode::ParentType* IDataStructureNode::getParentNode() const
{
  return m_Parent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IDataStructureNode::setParentNode(ParentType* const parent)
{
  if(parent == m_Parent)
  {
    return;
  }

  // Remove from parent's children
  if(nullptr != m_Parent)
  {
    m_Parent->removeChildNode(this);
  }

  m_Parent = parent;
}

// -----------------------------------------------------------------------------
QString IDataStructureNode::getNameOfClass() const
{
  return QString("IDataStructureNode");
}

// -----------------------------------------------------------------------------
QString IDataStructureNode::ClassName()
{
  return QString("IDataStructureNode");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath IDataStructureNode::getParentPath() const
{
  if(!hasParent())
  {
    return DataArrayPath();
  }
  return getParentNode()->getDataArrayPath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractDataStructureContainer::createParentConnection(IDataStructureNode* child, AbstractDataStructureContainer* parent) const
{
  child->setParentNode(parent);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractDataStructureContainer::destroyParentConnection(IDataStructureNode* child) const
{
  child->clearParentNode();
}

// -----------------------------------------------------------------------------
IDataStructureNode::Pointer IDataStructureNode::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
