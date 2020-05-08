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

#include <vector>

#include "SIMPLib/Geometry/IGeometry.h"
#include "SIMPLib/Montages/AbstractMontage.h"
#include "SIMPLib/Montages/GridTileIndex.h"

/**
 * @class GridMontage GridMontage.h SIMPLib/DataContainers/GridMontage.h
 * @brief The GridMontage class handles DataContainer montages arranged in a 3D grid.
 * Tiles are saved in a vector with GridTileIndices used as a means to lookup a DataContainer.
 * The montage does not assign the origin of each DataContainer geometry based on
 * the tile's position in the montage.
 */
class SIMPLib_EXPORT GridMontage : public AbstractMontage
{
public:
  using SizeType = SizeVec3Type;
  using TileIdType = SizeVec3Type;
  using DimensionsType = IVec3<double>;
  using BoundsType = IVec6<double>;

  using EnumType = uint32_t;
  enum class CollectionMethod : EnumType
  {
    CombOrder = 0,
    SnakeOrder
  };

  using Self = GridMontage;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Creates a new GridMontage with the specified name and size.
   * @param name
   * @param row
   * @param col
   * @param depth
   */
  static Pointer New(const QString& name, size_t row, size_t col, size_t depth = 1);

  /**
   * @brief Destructor
   */
  ~GridMontage() override;

  /**
   * @brief Checks and returns the validity of the montage.
   * Each tile must have a DataContainer and Geometry.
   * The units and geometry type must match between all geometries.
   * @return
   */
  bool isValid() const override;

  /**
   * @brief Returns the number of rows in the geometry.
   * @return
   */
  size_t getRowCount() const;

  /**
   * @brief Returns the number of columns in the geometry.
   * @return
   */
  size_t getColumnCount() const;

  /**
   * @brief Returns the depth of the geometry.
   * @return
   */
  size_t getDepthCount() const;

  /**
   * @brief Returns the size of the grid by the number of tiles.
   * @return
   */
  SizeType getGridSize() const;

  /**
   * @brief Returns the number of tiles in the montage.
   * @return
   */
  size_t getTileCount() const override;

  /**
   * @brief Resizes the montage to a new set of row, column, and depth values.
   * @param row
   * @param col
   * @param depth
   */
  void resizeTileDims(size_t row, size_t col, size_t depth);

  /**
   * @brief Returns the GridTileIndex for the specified position.
   * If the target position is invalid, return an invalid GridTileIndex.
   * @param row
   * @param col
   * @param depth
   * @return
   */
  GridTileIndex getTileIndex(size_t row, size_t col, size_t depth = 0) const;

  /**
   * @brief Searches for and returns the GridTileIndex for the specified DataContainer.
   * If the DataContainer could not be found, return an invalid GridTileIndex.
   * @param dc
   * @return
   */
  GridTileIndex getTileIndexForDataContainer(const DataContainerShPtr& dc) const;

  /**
   * @brief Returns a shared_ptr for the tile index for the given DataContainer.
   * @param dc
   * @return
   */
  AbstractTileIndexShPtr getTileIndexFor(const DataContainerShPtr& dc) const override;

  /**
   * @brief Returns a collection of DataContainers in the montage
   * @return
   */
  CollectionType getDataContainers() const override;

  /**
   * @brief Returns the DataContainer with the given GridTileIndex.
   * If the DataContainer is not found, return nullptr.
   * @param index
   * @return
   */
  DataContainerShPtr getDataContainer(GridTileIndex index) const;

  /**
   * @brief Sets the DataContainer at the given index.
   * @param index
   * @param dc
   */
  void setDataContainer(GridTileIndex index, const DataContainerShPtr& dc);

  /**
   * @brief Performs a deep copy of the current values.
   * @return
   */
  Pointer deepCopy() const;

  /**
   * @brief Creates a copy of the current montage and reassigns references
   * to the given DataContainerArray's values.
   * @param dca
   */
  AbstractMontage::Pointer propagate(const DataContainerArrayShPtr& dca) const override;

  /**
   * @brief Returns an HTML string with information about the montage with data laid out in a table.
   * @return
   */
  QString getInfoString() const override;

  /* -------- Begin iterator support -------- */
  /**
   * @brief Returns an iterator to the first item in the montage.
   * @return
   */
  Iterator begin() override;

  /**
   * @brief Returns a const iterator to the first item in the montage.
   * @return
   */
  ConstIterator begin() const override;

  /**
   * @brief Returns an iterator to the last item in the montage.
   * @return
   */
  Iterator end() override;

  /**
   * @brief Returns a const iterator to the last item in the montage.
   * @return
   */
  ConstIterator end() const override;
  /* -------- End iterator support -------- */

  /**
   * @brief Returns the length unit used by the montage
   * @return
   */
  IGeometry::LengthUnit getLengthUnit() const;

  /**
   * @brief Returns a QString representation of the length unit used by the montage.
   * @return
   */
  QString getLenthUnitString() const;

  /**
   * @brief Returns the montage's dimensions as { X, Y, Z } values
   * @return
   */
  DimensionsType getDimensions() const;

  /**
   * @brief Returns the montage's bounds as { minX, maxX, minY, maxY, minZ, maxZ } values.
   * @return
   */
  BoundsType getBounds() const;

  /**
   * @brief Write the montage to an HDF5 file.
   * @param groupId
   * @return error code
   */
  int writeH5Data(hid_t groupId) const override;

  /**
   * @brief Sets the DataContainer vector for the montage.
   * Returns true on success. Returns false otherwise.
   * @param dataContainers
   * @return
   */
  bool setDataContainers(std::vector<DataContainerShPtr> dataContainers);

protected:
  GridMontage(const QString& name, size_t row, size_t col, size_t depth);

  /**
   * @brief Returns a size_t index from the given TileIdType.
   * @param tileID
   * @return
   */
  size_t getOffsetFromTileId(TileIdType tileID) const;

private:
  SizeType m_Size;
  CollectionType m_DataContainers;
};
