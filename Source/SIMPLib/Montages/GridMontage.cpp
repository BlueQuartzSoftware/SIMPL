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

#include "GridMontage.h"

#include <memory>
#include <typeinfo>

#include "H5Support/QH5Lite.h"

#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/IGeometry3D.h"
#include "SIMPLib/Geometry/IGeometryGrid.h"
#include "SIMPLib/Utilities/ToolTipGenerator.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::GridMontage(const QString& name, size_t row, size_t col, size_t depth)
: AbstractMontage(name)
, m_Size(SizeVec3Type(row, col, depth))
{
  m_DataContainers.resize(getTileCount());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::Pointer GridMontage::New(const QString& name, size_t row, size_t col, size_t depth)
{
  if(name.isEmpty() || row == 0 || col == 0 || depth == 0)
  {
    return nullptr;
  }

  Pointer ptr = Pointer(new GridMontage(name, row, col, depth));
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::~GridMontage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GridMontage::isValid() const
{
  const size_t tileCount = getTileCount();
  if(tileCount == 0)
  {
    return false;
  }

  IGeometry::LengthUnit lengthUnit;
  IGeometry* geomType = nullptr;
  for(size_t i = 0; i < tileCount; i++)
  {
    if(nullptr == m_DataContainers[i])
    {
      return false;
    }
    // Geometries Required
    if(nullptr == m_DataContainers[i]->getGeometry())
    {
      return false;
    }
    // Identical length units and GeometryType required
    if(i == 0)
    {
      lengthUnit = m_DataContainers[i]->getGeometry()->getUnits();
      geomType = m_DataContainers[i]->getGeometry().get();
    }
    else if(lengthUnit != m_DataContainers[i]->getGeometry()->getUnits())
    {
      return false;
    }
    else if(typeid(geomType) != typeid(m_DataContainers[i]->getGeometry().get()))
    {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t GridMontage::getRowCount() const
{
  return m_Size[0];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t GridMontage::getColumnCount() const
{
  return m_Size[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t GridMontage::getDepthCount() const
{
  return m_Size[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::SizeType GridMontage::getGridSize() const
{
  return m_Size;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t GridMontage::getTileCount() const
{
  return getRowCount() * getColumnCount() * getDepthCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridTileIndex GridMontage::getTileIndex(size_t row, size_t col, size_t depth) const
{
  if(row > m_Size[0])
  {
    return GridTileIndex();
  }
  if(col > m_Size[1])
  {
    return GridTileIndex();
  }
  if(depth > m_Size[2])
  {
    return GridTileIndex();
  }

  TileIdType tileID(row, col, depth);
  return GridTileIndex(this, tileID);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridTileIndex GridMontage::getTileIndexForDataContainer(const DataContainerShPtr& dc) const
{
  const size_t rowCount = getRowCount();
  const size_t colCount = getColumnCount();
  const size_t depthCount = getDepthCount();

  for(size_t row = 0; row < rowCount; row++)
  {
    for(size_t col = 0; col < colCount; col++)
    {
      for(size_t depth = 0; depth < depthCount; depth++)
      {
        GridTileIndex index = getTileIndex(row, col, depth);
        DataContainer::Pointer d = getDataContainer(index);
        if(dc == d)
        {
          return index;
        }
      }
    }
  }

  return GridTileIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractTileIndexShPtr GridMontage::getTileIndexFor(const DataContainerShPtr& dc) const
{
  GridTileIndex gridIndex = getTileIndexForDataContainer(dc);
  GridTileIndex* indexPtr = new GridTileIndex(gridIndex);
  return AbstractTileIndexShPtr(indexPtr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractMontage::CollectionType GridMontage::getDataContainers() const
{
  return m_DataContainers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer GridMontage::getDataContainer(GridTileIndex index) const
{
  if(!index.isValid())
  {
    return nullptr;
  }

  size_t offset = getOffsetFromTileId(index.getTilePos());
  if(offset >= m_DataContainers.size())
  {
    return nullptr;
  }
  return m_DataContainers[offset];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GridMontage::setDataContainer(GridTileIndex index, const DataContainerShPtr& dc)
{
  if(!index.isValid())
  {
    return;
  }

  size_t offset = getOffsetFromTileId(index.getTilePos());
  if(offset >= m_DataContainers.size())
  {
    return;
  }
  m_DataContainers[offset] = dc;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool checkTileIdValid(GridMontage::TileIdType tileID, GridMontage::SizeType gridSize)
{
  bool sizeCheck = true;
  for(size_t i = 0; i < 3; i++)
  {
    sizeCheck &= tileID[i] <= gridSize[i];
  }

  return sizeCheck;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t getOffsetFromTileIdAndSize(GridMontage::TileIdType tileID, GridMontage::SizeType size)
{
  size_t row = tileID[0];
  size_t col = tileID[1];
  size_t depth = tileID[2];

  size_t rowCount = size[0];
  size_t colCount = size[1];

  size_t offset = row + col * rowCount + depth * rowCount * colCount;
  return offset;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t GridMontage::getOffsetFromTileId(TileIdType tileID) const
{
  return getOffsetFromTileIdAndSize(tileID, getGridSize());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GridMontage::resizeTileDims(size_t row, size_t col, size_t depth)
{
  const SizeType oldSize = getGridSize();
  const CollectionType oldCollection = m_DataContainers;

  m_DataContainers.resize(row * col * depth);
  std::fill(m_DataContainers.begin(), m_DataContainers.end(), nullptr);

  const SizeType newSize = getGridSize();
  for(size_t r = 0; r < oldSize[0]; r++)
  {
    for(size_t c = 0; c < oldSize[1]; c++)
    {
      for(size_t d = 0; d < oldSize[2]; d++)
      {
        TileIdType oldId(r, c, d);
        if(checkTileIdValid(oldId, newSize))
        {
          size_t oldOffset = getOffsetFromTileIdAndSize(oldId, oldSize);
          size_t newOffset = getOffsetFromTileId(oldId);
          m_DataContainers[newOffset] = oldCollection[oldOffset];
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::Pointer GridMontage::deepCopy() const
{
  Pointer ptr = Pointer(new GridMontage(getName(), getRowCount(), getColumnCount(), getDepthCount()));
  const size_t tileCount = m_DataContainers.size();
  for(size_t i = 0; i < tileCount; i++)
  {
    ptr->m_DataContainers[i] = m_DataContainers[i];
  }

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractMontage::Pointer GridMontage::propagate(const DataContainerArrayShPtr& dca) const
{
  Pointer ptr = deepCopy();

  // Use the DataContainer of the same name in the new DataContainerArray
  const size_t tileCount = m_DataContainers.size();
  for(size_t i = 0; i < tileCount; i++)
  {
    if(nullptr == m_DataContainers[i])
    {
      continue;
    }
    QString dcName = m_DataContainers[i]->getName();
    ptr->m_DataContainers[i] = dca->getDataContainer(dcName);
  }

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::Iterator GridMontage::begin()
{
  return m_DataContainers.begin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::ConstIterator GridMontage::begin() const
{
  return m_DataContainers.begin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::Iterator GridMontage::end()
{
  return m_DataContainers.end();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::ConstIterator GridMontage::end() const
{
  return m_DataContainers.end();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::LengthUnit GridMontage::getLengthUnit() const
{
  for(const auto& dc : m_DataContainers)
  {
    if(nullptr == dc || nullptr == dc->getGeometry())
    {
      continue;
    }
    return dc->getGeometry()->getUnits();
  }

  return IGeometry::LengthUnit::Unknown;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GridMontage::getLenthUnitString() const
{
  return IGeometry::LengthUnitToString(getLengthUnit());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::DimensionsType GridMontage::getDimensions() const
{
  BoundsType bounds = getBounds();
  DimensionsType dims;
  dims[0] = bounds[1] - bounds[0];
  dims[1] = bounds[3] - bounds[2];
  dims[2] = bounds[5] - bounds[4];
  return dims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::BoundsType GridMontage::getBounds() const
{
  double minX = 0.0;
  double maxX = 0.0;
  double minY = 0.0;
  double maxY = 0.0;
  double minZ = 0.0;
  double maxZ = 0.0;
  bool hasValues = false;

  for(const auto& dc : m_DataContainers)
  {
    if(nullptr == dc)
    {
      continue;
    }
    IGeometry::Pointer geom = dc->getGeometry();
    if(nullptr == geom)
    {
      continue;
    }

    if(nullptr != std::dynamic_pointer_cast<IGeometryGrid>(geom))
    {
      IGeometryGrid::Pointer grid = std::dynamic_pointer_cast<IGeometryGrid>(geom);
      size_t xPoints = grid->getXPoints();
      size_t yPoints = grid->getYPoints();
      size_t zPoints = grid->getZPoints();

      double coords[3];

      // float halfRes[3] = { 0, 0, 0 };
      // float spacing[3] = { 1, 1, 1 };

      // calculate origin point
      {
        grid->getCoords(0, 0, 0, coords);

        if(!hasValues)
        {
          minX = coords[0];
          minY = coords[1];
          minZ = coords[2];

          maxX = coords[0];
          maxY = coords[1];
          maxZ = coords[2];

          hasValues = true;
        }
        else
        {
          // Find min
          minX = std::min(minX, coords[0]);
          minY = std::min(minY, coords[1]);
          minZ = std::min(minZ, coords[2]);
          // Find max
          maxX = std::max(maxX, coords[0]);
          maxY = std::max(maxY, coords[1]);
          maxZ = std::max(maxZ, coords[2]);
        }
      }

      // calculate opposite corner
      {
        grid->getCoords(xPoints, yPoints, zPoints, coords);

        // Find min
        minX = std::min(minX, coords[0]);
        minY = std::min(minY, coords[1]);
        minZ = std::min(minZ, coords[2]);
        // Find max
        maxX = std::max(maxX, coords[0]);
        maxY = std::max(maxY, coords[1]);
        maxZ = std::max(maxZ, coords[2]);
      }

      // minX -= halfRes[0];
      // maxX -= halfRes[0];
      // minY -= halfRes[1];
      // maxY -= halfRes[1];
      // minZ -= halfRes[2];
      // minZ -= halfRes[2];
    }
  }

  BoundsType extent;
  extent[0] = minX;
  extent[1] = maxX;
  extent[2] = minY;
  extent[3] = maxY;
  extent[4] = minZ;
  extent[5] = maxZ;

  return extent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GridMontage::getInfoString() const
{
  QLocale usa(QLocale::English, QLocale::UnitedStates);

  QString info;
  QTextStream ss(&info);

  ToolTipGenerator htmlGenerator;

  htmlGenerator.addTitle("Montage Info");
  htmlGenerator.addValue("Name", getName());
  htmlGenerator.addValue("Type", "Grid");
  htmlGenerator.addValue("Number of Tiles", usa.toString(static_cast<quint64>(getTileCount())));
  htmlGenerator.addSpacer();
  htmlGenerator.addTitle("Tile Dimensions");
  htmlGenerator.addValue("Rows", usa.toString(static_cast<quint64>(getRowCount())));
  htmlGenerator.addValue("Columns", usa.toString(static_cast<quint64>(getColumnCount())));
  htmlGenerator.addValue("Depth", usa.toString(static_cast<quint64>(getDepthCount())));

  htmlGenerator.addSpacer();

  htmlGenerator.addTitle("Geometry Info");
  const QString lengthUnitStr = getLenthUnitString();
  htmlGenerator.addValue("Length Unit", lengthUnitStr);
  const BoundsType bounds = getBounds();
  const DimensionsType dims = getDimensions();
  htmlGenerator.addValue("X", usa.toString(bounds[0]) + " to " + usa.toString(bounds[1]) + " (dimension: " + usa.toString(dims[0]) + ")");
  htmlGenerator.addValue("Y", usa.toString(bounds[2]) + " to " + usa.toString(bounds[3]) + " (dimension: " + usa.toString(dims[1]) + ")");
  htmlGenerator.addValue("Z", usa.toString(bounds[4]) + " to " + usa.toString(bounds[5]) + " (dimension: " + usa.toString(dims[2]) + ")");

  return htmlGenerator.generateHTML();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GridMontage::writeH5Data(hid_t groupId) const
{
  hid_t montageId = QH5Utilities::createGroup(groupId, getName());
  // This object will make sure the HDF5 Group id is closed when it goes out of scope.
  H5GroupAutoCloser bundleIdClose(&montageId);

  int err = QH5Lite::writeStringDataset(montageId, "Montage Type", "GridMontage");
  if(err < 0)
  {
    return err;
  }

  err = QH5Lite::writeStringDataset(montageId, SIMPL::StringConstants::Name, getName());
  if(err < 0)
  {
    return err;
  }

  // Write Dimensions
  const SizeVec3Type dims = getGridSize();
  err = QH5Lite::writeScalarDataset(montageId, "X Dim", dims[0]);
  if(err < 0)
  {
    return err;
  }
  err = QH5Lite::writeScalarDataset(montageId, "Y Dim", dims[1]);
  if(err < 0)
  {
    return err;
  }
  err = QH5Lite::writeScalarDataset(montageId, "Z Dim", dims[2]);
  if(err < 0)
  {
    return err;
  }

  // size_t count = static_cast<size_t>(m_DataContainers.size());
  QStringList dcNameList;

  for(const auto& dc : m_DataContainers)
  {
    if(nullptr == dc)
    {
      dcNameList << "";
    }
    else
    {
      dcNameList << dc->getName();
    }
  }

  char sep = 0x1E; // Use the ASCII 'record separator' value (Decimal value 30) to separate the names
  // Write the Names of the Data Containers that this bundle holds
  QString nameList = dcNameList.join(QString(sep));
  err = QH5Lite::writeStringDataset(montageId, SIMPL::StringConstants::DataContainerNames, nameList);
  if(err < 0)
  {
    return err;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GridMontage::setDataContainers(std::vector<DataContainerShPtr> dataContainers)
{
  if(dataContainers.size() != m_DataContainers.size())
  {
    return false;
  }
  m_DataContainers = dataContainers;
  return true;
}

// -----------------------------------------------------------------------------
GridMontage::Pointer GridMontage::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
