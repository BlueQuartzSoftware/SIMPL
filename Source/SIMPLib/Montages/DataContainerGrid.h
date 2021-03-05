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

#pragma once

#include <vector>

#include <QtCore/QObject>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLArray.hpp"

class SIMPLib_EXPORT DataContainerGrid : public QObject
{
  Q_OBJECT

public:
  using NameCollectionType = std::vector<QString>;

  DataContainerGrid(SizeVec3Type dims = SizeVec3Type());
  DataContainerGrid(SizeVec3Type dims, const std::vector<QString>& dcList);
  virtual ~DataContainerGrid();

  /**
   * @brief Returns the number of items in the grid.
   * @return
   */
  size_t size() const;

  /**
   * @brief Returns the grid's dimensions.
   * @return
   */
  SizeVec3Type getDimensions() const;

  /**
   * @brief Resizes the grid to the given dimensions.
   * @param newSize
   */
  void resizeDimensions(SizeVec3Type newSize);

  /**
   * @brief Returns the DataContainer name at the given position.
   * @param pos
   * @return
   */
  QString getDataContainerName(SizeVec3Type pos) const;

  /**
   * @brief Sets the DataContainer name for the given position.
   * @param pos
   * @param name
   */
  void setDataContainerName(SizeVec3Type pos, const QString& name);

  /**
   * @brief Returns a list of DataContainer names.
   * @return
   */
  QStringList getDataContainerNames() const;

Q_SIGNALS:
  void dimensionsChanged(SizeVec3Type newDims);
  void dataContainerNameChanged();

private:
  /**
   * @brief Returns the vector offset for the given position
   * @param pos
   * @return
   */
  size_t getOffset(SizeVec3Type pos) const;

  SizeVec3Type m_Dims;
  NameCollectionType m_DataContainerNames;

public:
  DataContainerGrid(const DataContainerGrid& other);
  DataContainerGrid& operator=(const DataContainerGrid& other);
};

Q_DECLARE_METATYPE(DataContainerGrid)
