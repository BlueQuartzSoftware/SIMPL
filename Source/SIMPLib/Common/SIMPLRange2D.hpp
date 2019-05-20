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

#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Common/SIMPLArray.hpp"
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range2d.h>
#endif

class SIMPLib_EXPORT SIMPLRange2D
{
public:
  using RangeType = std::array<size_t, 4>; // { init row, init col, final row, final col }

  SIMPLRange2D()
  : m_Range({ 0,0,0,0 })
  {
  }
  SIMPLRange2D(size_t initRow, size_t initCol, size_t endRow, size_t endCol)
  : m_Range({ initRow, initCol, endRow, endCol })
  {
  }
  template<typename Type>
  SIMPLRange2D(IVec2<Type> array)
  : m_Range({ 0, array[0], 0, array[1] })
  {
  }
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  SIMPLRange2D(tbb::blocked_range2d<size_t, size_t>& r)
  : m_Range({ r.rows().begin(), r.cols().begin(), r.rows().end(), r.cols().end() })
  {
  }
#endif

  /**
   * @brief Returns an array representation of the range.
   * @return
   */
  RangeType getRange() const
  {
    return m_Range;
  }

  /**
   * @brief Returns the minimum row index in the range.
   * @return
   */
  size_t minRow() const
  {
    return m_Range[0];
  }

  /**
   * @brief Returns the minimum column index in the range.
   * @return
   */
  size_t minCol() const
  {
    return m_Range[1];
  }

  /**
   * @brief Returns the maximum row index in the range.
   * @return
   */
  size_t maxRow() const
  {
    return m_Range[2];
  }

  /**
   * @brief Returns the maximum column index in the range.
   * @return
   */
  size_t maxCol() const
  {
    return m_Range[3];
  }

  /**
   * @brief Returns the number of rows in the range.
   * @return
   */
  size_t numRows() const
  {
    return maxRow() - minRow();
  }

  /**
   * @brief Returns the number of columns in the range.
   * @return
   */
  size_t numCols() const
  {
    return maxCol() - minCol();
  }

  /**
   * @brief Returns the number of indices in the range.
   * @return
   */
  size_t size() const
  {
    return numRows() * numCols();
  }

  /**
   * @brief Returns true if the range is empty.  Returns false otherwise.
   * @return
   */
  bool empty() const
  {
    const bool emptyRows = (m_Range[0] == m_Range[2]) && (m_Range[0] == 0);
    const bool emptyCols = (m_Range[1] == m_Range[3]) && (m_Range[1] == 0);
    return emptyRows && emptyCols;
  }

  /**
   * @brief Returns the range based on the specified index.  The range is
   * organized as [min, max]
   */
  size_t operator[](size_t index) const
  {
    if(index < 4)
    {
      return m_Range[index];
    }
    throw std::range_error("Range out of bounds");
  }

private:
  RangeType m_Range;
};
