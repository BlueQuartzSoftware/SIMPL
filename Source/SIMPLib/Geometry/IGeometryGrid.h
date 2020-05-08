/* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include <tuple>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/Geometry/IGeometry.h"

/**
 * @brief The IGeometryGrid class extends IGeometry for grid type geometries
 */
class SIMPLib_EXPORT IGeometryGrid : public IGeometry
{
  // This class needs to be wrapped so Python/Pybind11 can do the proper casting
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(IGeometryGrid SUPERCLASS IGeometry)
  PYB11_SHARED_POINTERS(IGeometryGrid)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = IGeometryGrid;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for IGeometryGrid
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for IGeometryGrid
   */
  static QString ClassName();

  IGeometryGrid();
  ~IGeometryGrid() override;

  virtual void setDimensions(const SizeVec3Type& dims) = 0;
  virtual SizeVec3Type getDimensions() const = 0;

  virtual size_t getXPoints() const = 0;
  virtual size_t getYPoints() const = 0;
  virtual size_t getZPoints() const = 0;

  virtual void getPlaneCoords(size_t idx[3], float coords[3]) const = 0;
  virtual void getPlaneCoords(size_t x, size_t y, size_t z, float coords[3]) const = 0;
  virtual void getPlaneCoords(size_t idx, float coords[3]) const = 0;

  virtual void getPlaneCoords(size_t idx[3], double coords[3]) const = 0;
  virtual void getPlaneCoords(size_t x, size_t y, size_t z, double coords[3]) const = 0;
  virtual void getPlaneCoords(size_t idx, double coords[3]) const = 0;

  virtual void getCoords(size_t idx[3], float coords[3]) const = 0;
  virtual void getCoords(size_t x, size_t y, size_t z, float coords[3]) const = 0;
  virtual void getCoords(size_t idx, float coords[3]) const = 0;

  virtual void getCoords(size_t idx[3], double coords[3]) const = 0;
  virtual void getCoords(size_t x, size_t y, size_t z, double coords[3]) const = 0;
  virtual void getCoords(size_t idx, double coords[3]) const = 0;

public:
  IGeometryGrid(const IGeometryGrid&) = delete;            // Copy Constructor Not Implemented
  IGeometryGrid(IGeometryGrid&&) = delete;                 // Move Constructor Not Implemented
  IGeometryGrid& operator=(const IGeometryGrid&) = delete; // Copy Assignment Not Implemented
  IGeometryGrid& operator=(IGeometryGrid&&) = delete;      // Move Assignment Not Implemented
};
