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

#include <memory>

#include "SIMPLib/SIMPLib.h"

class INamedObject;

/**
 * @class INamedCollection INamedCollection.h SIMPLib/Common/INamedCollection.h
 * @brief The INamedCollection class is used as an interface for INamedObject collections.
 * Adding to or removing from the collection should alert the INamedObject to the action
 * so that the INamedObject can check if it can be renamed.  This also adds extended support
 * for graph theory implementations as INamedObjects can have and keep track of multiple parents.
 */
class SIMPLib_EXPORT INamedCollection
{
public:
  using Self = INamedCollection;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  INamedCollection();
  virtual ~INamedCollection();

  INamedCollection(const INamedCollection&) = default;            // Copy Constructor Not Implemented
  INamedCollection(INamedCollection&&) = default;                 // Move Constructor Not Implemented
  INamedCollection& operator=(const INamedCollection&) = default; // Copy Assignment Not Implemented
  INamedCollection& operator=(INamedCollection&&) = default;      // Move Assignment Not Implemented

  using NameList = QStringList;

  /**
   * @brief Returns the number of items in the collection.
   * @return
   */
  virtual size_t size() const = 0;

  /**
   * @brief Returns true if the collection is empty.  Returns false otherwise.
   * @return
   */
  virtual bool isEmpty() const = 0;

  /**
   * @brief Clears the collection of all items.
   */
  virtual void clear() = 0;

  /**
   * @brief Erases any item with the given name.
   * @param name
   */
  virtual void erase(const QString& name) = 0;

  /**
   * @brief Returns true if the collection contains an item with the given name.
   * Returns false otherwise.
   * @param name
   * @return
   */
  virtual bool contains(const QString& name) const = 0;

  /**
   * @brief Returns a list of all names in the collection.
   * @return
   */
  virtual NameList getNameList() const = 0;

protected:
  /**
   * @brief Alerts the INamedObject that it has been added to a collection.
   * @param obj
   */
  void handleAddingObject(INamedObject* obj);

  /**
   * @brief Alerts the INamedObject that it has been removed from a collection.
   * @param obj
   */
  void handleRemovingObject(INamedObject* obj);

private:
};
