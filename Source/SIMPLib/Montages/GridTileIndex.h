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

#include "AbstractTileIndex.h"

#include "SIMPLib/Common/SIMPLArray.hpp"

class GridMontage;

/**
 * @class GridTileIndex GridTileIndex.h SIMPLib/DataContainers/GridTileIndex.h
 * @brief The GridTileIndex class is used to reference specific DataContainers
 * inside a GridMontage using the tile's row, column, and depth.
 */
class SIMPLib_EXPORT GridTileIndex : public AbstractTileIndex
{
  friend GridMontage;

public:
  /**
   * @brief The default constructor creates invalid indices.
   */
  GridTileIndex();

  /**
   * @brief Destructor
   */
  ~GridTileIndex() override;

  /**
   * @brief Returns the tile row within the target montage.
   * @return
   */
  size_t getRow() const;

  /**
   * @brief Returns the tile column within the target montage.
   * @return
   */
  size_t getCol() const;

  /**
   * @brief Returns the tile depth within the target montage.
   * @return
   */
  size_t getDepth() const;

  /**
   * @brief Returns the tile position within the target montage as { Row, Column, Depth }
   * @return
   */
  SizeVec3Type getTilePos() const;

  /**
   * @brief Retuns the target DataContainer.
   * Returns nullptr if it could not be found or the index is invalid.
   * @return
   */
  DataContainerShPtr getDataContainer() const override;

  /**
   * @brief Checks if the index is valid.
   * @return
   */
  bool isValid() const override;

  /**
   * @brief Creates and returns a ToolTipGenerator with the relevant values set for the tile index.
   * @return
   */
  ToolTipGenerator getToolTipGenerator() const override;

protected:
  /**
   * @param "Valid" constructor is only accessible from within GridMontage.
   * @param montage
   * @param pos
   */
  GridTileIndex(const GridMontage* montage, SizeVec3Type pos);

private:
  SizeVec3Type m_TilePos;
};
