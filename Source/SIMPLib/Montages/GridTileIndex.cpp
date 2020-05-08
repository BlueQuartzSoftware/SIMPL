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

#include "GridTileIndex.h"

#include "SIMPLib/Montages/GridMontage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridTileIndex::GridTileIndex()
: AbstractTileIndex()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridTileIndex::GridTileIndex(const GridMontage* montage, SizeVec3Type pos)
: AbstractTileIndex(montage)
, m_TilePos(pos)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridTileIndex::~GridTileIndex() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t GridTileIndex::getRow() const
{
  return m_TilePos[0];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t GridTileIndex::getCol() const
{
  return m_TilePos[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t GridTileIndex::getDepth() const
{
  return m_TilePos[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SizeVec3Type GridTileIndex::getTilePos() const
{
  return m_TilePos;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerShPtr GridTileIndex::getDataContainer() const
{
  const GridMontage* montage = dynamic_cast<const GridMontage*>(getMontage());
  if(nullptr == montage)
  {
    return nullptr;
  }

  return montage->getDataContainer(*this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GridTileIndex::isValid() const
{
  const GridMontage* montage = dynamic_cast<const GridMontage*>(getMontage());
  if(nullptr == montage)
  {
    return false;
  }

  SizeVec3Type gridSize = montage->getGridSize();
  bool sizeCheck = true;
  for(size_t i = 0; i < 3; i++)
  {
    sizeCheck &= m_TilePos[i] <= gridSize[i];
  }

  return AbstractTileIndex::isValid() && sizeCheck;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ToolTipGenerator GridTileIndex::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;

  SizeVec3Type tilePos = getTilePos();

  toolTipGen.addTitle("Grid Tile Index");
  toolTipGen.addValue("Index", "(" + QString::number(tilePos[0]) + ", " + QString::number(tilePos[1]) + ", " + QString::number(tilePos[2]) + ")");

  return toolTipGen;
}
