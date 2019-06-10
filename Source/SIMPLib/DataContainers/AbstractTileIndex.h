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

#include <memory>

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"

class AbstractMontage;

class DataContainer;
using DataContainerShPtr = std::shared_ptr<DataContainer>;

/**
 * @class AbstractTileIndex AbstractTileIndex.h SIMPLib/DataContainers/AbstractTileIndex.h
 * @brief The AbstractTileIndex class is the base class for other index classes used
 * by each of the Montage collections.  The common requirements between these is the
 * ability to get the montage the index is for, the DataContainer it points to, and
 * a validity check.
 */
class SIMPLib_EXPORT AbstractTileIndex
{
public:
  /**
   * @brief The default constructor creates invalid indices.
   */
  AbstractTileIndex();

  /**
   * @brief Destructor
   */
  virtual ~AbstractTileIndex();

  /**
   * @brief Returns a const pointer to the Montage this index is valid for.
   * If no montage was assigned, this returns nullptr.
   * @return
   */
  const AbstractMontage* getMontage() const;

  /**
   * @brief Returns the DataContainer this index points to.  If the index is invalid, this returns nullptr.
   * @return
   */
  virtual DataContainerShPtr getDataContainer() const = 0;

  /**
   * @brief Returns true if this index is valid.  Returns false otherwise.
   * @return
   */
  virtual bool isValid() const;

protected:
  AbstractTileIndex(const AbstractMontage* montage);

private:
  const AbstractMontage* m_Montage;
};
