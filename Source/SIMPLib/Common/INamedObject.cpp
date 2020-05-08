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

#include "INamedObject.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
INamedObject::HashType INamedObject::CreateHash(const QString& string)
{
  return std::hash<std::string>()(string.toStdString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
INamedObject::INamedObject(const QString& name)
: m_Name(name)
, m_NameHash(CreateHash(name))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
INamedObject::~INamedObject()
{
  ParentCollectionType collections = getParentCollections();
  for(const auto& collection : collections)
  {
    collection->erase(getName());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString INamedObject::getName() const
{
  return m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool INamedObject::setName(const QString& name)
{
  for(const auto& collection : m_ParentCollctions)
  {
    // If collection does not exist, something has else gone wrong and requires attention.
    if(collection->contains(name))
    {
      return false;
    }
  }

  m_NameHash = CreateHash(name);
  m_Name = name;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
INamedObject::HashType INamedObject::getNameHash() const
{
  return m_NameHash;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
INamedObject::ParentCollectionType INamedObject::getParentCollections() const
{
  return m_ParentCollctions;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INamedObject::addToCollection(INamedCollection* collection)
{
  m_ParentCollctions.insert(collection);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INamedObject::removeFromCollection(INamedCollection* collection)
{
  m_ParentCollctions.erase(collection);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool INamedObject::operator<(const INamedObject& other) const
{
  return getNameHash() < other.getNameHash();
}

bool INamedObject::operator==(const INamedObject& other) const
{
  return getNameHash() == other.getNameHash();
}

// -----------------------------------------------------------------------------
INamedObject::Pointer INamedObject::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
