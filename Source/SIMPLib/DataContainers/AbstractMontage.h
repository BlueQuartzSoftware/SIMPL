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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
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

#include <QtCore/QString>

#include "SIMPLib/Common/INamedObject.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/SIMPLib.h"

class AbstractTileIndex;
using AbstractTileIndexShPtr = std::shared_ptr<AbstractTileIndex>;

class DataContainer;
using DataContainerShPtr = std::shared_ptr<DataContainer>;

class DataContainerArray;
using DataContainerArrayShPtr = std::shared_ptr<DataContainerArray>;

/**
 * @brief The AbstractMontage class is the base for the various types of montages and
 * contains the common API between the types.  It does not have access to individual tiles,
 * as those require Montage-specific Index types.  AbstractMontage is derived from
 * INamedObject in order to take advantage of NamedCollection<AbstractMontage>.  This
 * custom collection allows items to be searched for by name and prevents multiple items
 * from having the same name.
 */
class SIMPLib_EXPORT AbstractMontage : public INamedObject
{
public:
  SIMPL_SHARED_POINTERS(AbstractMontage)

  using CollectionType = std::vector<DataContainerShPtr>;

  virtual ~AbstractMontage();

  /**
   * @brief Checks if the montage's values are valid.
   * @return
   */
  virtual bool isValid() const = 0;

  /**
   * @brief Returns the number of tiles in the montage.
   * @return
   */
  virtual size_t getTileCount() const = 0;

  /**
   * @brief Returns the collection of DataContainers used by the montage.
   * @return
   */
  virtual CollectionType getDataContainers() const = 0;

  /**
   * @brief Returns a QStringList of the DataContainer names in the montage.
   * @return
   */
  QStringList getDataContainerNames() const;

  /**
   * @brief Returns a vector of DataArrayPaths for the Montage.
   * @return
   */
  std::vector<DataArrayPath> getDataArrayPaths() const;

  /**
   * @brief Returns an HTML string containing information about the montage.
   * @return
   */
  virtual QString getInfoString() const = 0;

  /**
   * @brief Returns a shared_ptr for the tile index for the given DataContainer.
   * @param dc
   * @return
   */
  virtual AbstractTileIndexShPtr getTileIndexFor(const DataContainerShPtr& dc) const = 0;

  /**
   * @brief Creates and returns a copy of the montage with all DataContainer references
   * updated to the provided DataContainerArray.  If a DataContainer cannot be found,
   * it is set to nullptr.
   * @param dca
   * @return
   */
  virtual Pointer propagate(const DataContainerArrayShPtr& dca) const = 0;

protected:
  /**
   * @brief Constructor is only available to derived classes.
   * @param name
   */
  AbstractMontage(const QString& name);
};
