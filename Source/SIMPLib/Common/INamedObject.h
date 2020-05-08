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

#pragma once

#include <functional>
#include <set>

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "SIMPLib/Common/INamedCollection.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @class INamedObject INamedObject.h SIMPLib/Common/INamedObject.h
 * @brief The INamedObject is a base class for named objects in SIMPL, allowing
 * them to be placed in special containers that key off of the object's name
 * instead of requiring the key and value to be updated separately.  An added
 * bonus is the ability to keep track of any naming conflicts that could occur
 * when renaming an object when it is contained by multiple collections. This
 * also offers the ability to track parent collections in a bi-directional graph.
 */
class SIMPLib_EXPORT INamedObject
{
  friend INamedCollection;

public:
  using Self = INamedObject;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  using HashType = size_t;
  using ParentCollectionType = std::set<INamedCollection*>;

  /**
   * @brief Remove this object from its parent containers upon deletion.
   */
  virtual ~INamedObject();

  /**
   * @brief Method for creating hashes from QString values.
   * @param string
   * @return
   */
  static HashType CreateHash(const QString& string);

  /**
   * @brief Returns the object's name value.
   * @return
   */
  QString getName() const;

  /**
   * @brief Attempts to set the object's name value.
   * Returns true if the operation succeded.  Returns false otherwise.
   * @param newName
   * @return
   */
  bool setName(const QString& newName);

  /**
   * @brief Returns the hash value for the current name.
   * This value should be set when the name is changed so that it does not
   * have to be calculated every time this method is called.
   * @return
   */
  HashType getNameHash() const;

  /**
   * @brief Returns a set with all parent collections.
   * @return
   */
  ParentCollectionType getParentCollections() const;

  /**
   * @brief Compares the hash value between two INamedObjects and returns if this
   * object's value is less than the other.
   * @param other
   * @return
   */
  bool operator<(const INamedObject& other) const;

  /**
   * @brief Compares the hash value of two INamedObjects for equality.
   * Returns true if both are equal.  Returns false otherwise.
   * @param other
   * @return
   */
  bool operator==(const INamedObject& other) const;

protected:
  INamedObject(const QString& name);

private:
  /**
   * @brief Adds the collection to the list of parents.
   * @param collection
   */
  void addToCollection(INamedCollection* collection);

  /**
   * @brief Removed the collection from the list of parents.
   * @param collection
   */
  void removeFromCollection(INamedCollection* collection);

  ParentCollectionType m_ParentCollctions;
  QString m_Name;
  HashType m_NameHash;
};

/**
 * @brief Compares two INamedObject shared pointers based on the less than operator of their base pointers.
 * @param lhs
 * @param rhs
 * @return
 */
inline bool operator<(const INamedObject::Pointer& lhs, const INamedObject::Pointer& rhs)
{
  return (*lhs) < (*rhs);
}

/**
 * @brief Compares two INamedObject shared pointers based on the equality operator of their base pointers.
 * @param lhs
 * @param rhs
 * @return
 */
inline bool operator==(const INamedObject::Pointer& lhs, const INamedObject::Pointer& rhs)
{
  return (*lhs) == (*rhs);
}
