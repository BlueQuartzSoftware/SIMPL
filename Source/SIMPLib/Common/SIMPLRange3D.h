/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <array>

#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/SIMPLib.h"

// SIMPLib.h MUST be included before this or the guard will block the include but not its uses below.
// This is consistent with previous behavior, only earlier parallelization split the includes between
// the corresponding .h and .cpp files.
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
// clang-format off
#include <tbb/blocked_range3d.h>
// clang-format on
#endif

/**
 * @class SIMPLRange3D SIMPLRange3D.h SIMPLib/Common/SIMPLRange3D.h
 * @brief The SIMPLRange3D class defines a range between set of minimum and
 * maximum values. The purpose of this class is mainly to allow a more unified
 * control flow during parallelization between builds using TBB and those that
 * do not.  Because tbb::blocked_range is used in an implicit conversion constructor,
 * a single operator accepting a SIMPLRange can be used TBB parallelized and
 * non-paralleled versions without a branching code base.
 */
class SIMPLib_EXPORT SIMPLRange3D
{
public:
  using RangeType = std::array<size_t, 6>;
  using DimensionRange = std::array<size_t, 2>;

  SIMPLRange3D();
  SIMPLRange3D(size_t x, size_t y, size_t z);
  SIMPLRange3D(size_t xMin, size_t xMax, size_t yMin, size_t yMax, size_t zMin, size_t zMax);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  SIMPLRange3D(const tbb::blocked_range3d<size_t>& r);
#endif

  /**
   * @brief Returns an array representation of the range.
   * @return
   */
  RangeType getRange() const;

  /**
   * @brief Returns the range along the X dimension
   * @return
   */
  DimensionRange getXRange() const;

  /**
   * @brief Returns the range along the Y dimension
   * @return
   */
  DimensionRange getYRange() const;

  /**
   * @brief Returns the range along the Z dimension
   * @return
   */
  DimensionRange getZRange() const;

  /**
   * @brief Returns true if the range is empty.  Returns false otherwise.
   * @return
   */
  bool empty() const;

  /**
   * @brief Returns the specified part of the range.  The range is organized as
   * [xMin, xMax, yMin, yMax, zMin, zMax].
   * @param index
   * @return
   */
  size_t operator[](size_t index) const;

private:
  RangeType m_Range;
};
