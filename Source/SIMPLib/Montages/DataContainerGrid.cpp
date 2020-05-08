/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DataContainerGrid.h"

#include <algorithm>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGrid::DataContainerGrid(SizeVec3Type dims)
: m_Dims(dims)
, m_DataContainerNames(dims[0] * dims[1] * dims[2])
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGrid::DataContainerGrid(SizeVec3Type dims, const std::vector<QString>& dcList)
: m_Dims(dims)
, m_DataContainerNames(dims[0] * dims[1] * dims[2])
{
  size_t size = std::min(dcList.size(), m_DataContainerNames.size());
  for(size_t i = 0; i < size; i++)
  {
    m_DataContainerNames[i] = dcList[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGrid::DataContainerGrid(const DataContainerGrid& other)
: m_Dims(other.m_Dims)
, m_DataContainerNames(other.m_DataContainerNames)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGrid::~DataContainerGrid() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t getOffsetForSize(SizeVec3Type pos, SizeVec3Type size)
{
  size_t row = pos[0];
  size_t col = pos[1];
  size_t depth = pos[2];

  size_t rowCount = size[0];
  size_t colCount = size[1];

  size_t offset = row + col * rowCount + depth * rowCount * colCount;
  return offset;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t DataContainerGrid::getOffset(SizeVec3Type pos) const
{
  return getOffsetForSize(pos, m_Dims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t DataContainerGrid::size() const
{
  return m_Dims[0] * m_Dims[1] * m_Dims[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SizeVec3Type DataContainerGrid::getDimensions() const
{
  return m_Dims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGrid::resizeDimensions(SizeVec3Type newSize)
{
  const SizeVec3Type oldSize = m_Dims;
  const NameCollectionType oldNames = m_DataContainerNames;

  m_Dims = newSize;
  m_DataContainerNames.clear();
  m_DataContainerNames.resize(newSize[0] * newSize[1] * newSize[2]);

  for(size_t row = 0; row < newSize[0] && row < oldSize[0]; row++)
  {
    for(size_t col = 0; col < newSize[1] && col < oldSize[1]; col++)
    {
      for(size_t depth = 0; depth < newSize[2] && depth < oldSize[2]; depth++)
      {
        SizeVec3Type pos(row, col, depth);
        size_t oldOffset = getOffsetForSize(pos, oldSize);
        size_t newOffset = getOffsetForSize(pos, newSize);
        m_DataContainerNames[newOffset] = oldNames[oldOffset];
      }
    }
  }

  emit dimensionsChanged(newSize);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerGrid::getDataContainerName(SizeVec3Type pos) const
{
  size_t offset = getOffset(pos);
  return m_DataContainerNames[offset];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGrid::setDataContainerName(SizeVec3Type pos, const QString& name)
{
  size_t offset = getOffset(pos);
  m_DataContainerNames[offset] = name;

  emit dataContainerNameChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataContainerGrid::getDataContainerNames() const
{
  QStringList dcNames;

  for(size_t depth = 0; depth < m_Dims[2]; depth++)
  {
    for(size_t row = 0; row < m_Dims[0]; row++)
    {
      for(size_t col = 0; col < m_Dims[1]; col++)
      {
        size_t offset = getOffset(SizeVec3Type(row, col, depth));
        dcNames.push_back(m_DataContainerNames[offset]);
      }
    }
  }

  return dcNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGrid& DataContainerGrid::operator=(const DataContainerGrid& other)
{
  m_Dims = other.m_Dims;
  m_DataContainerNames = other.m_DataContainerNames;

  emit dimensionsChanged(m_Dims);
  return *this;
}
