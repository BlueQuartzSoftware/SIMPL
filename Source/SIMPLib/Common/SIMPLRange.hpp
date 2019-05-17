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
#include <tbb/blocked_range.h>
#endif

class SIMPLib_EXPORT SIMPLRange
{
public:
  using RangeType = std::array<size_t, 2>;

  SIMPLRange()
  : m_Range({0,0})
  {
  }
  SIMPLRange(size_t begin, size_t end)
  : m_Range({ begin, end })
  {
  }
  template<typename Type, size_t Dimension>
  SIMPLRange(SIMPLArray<Type, Dimension> array)
  : m_Range({ 0, Dimension })
  {
  }
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  SIMPLRange(tbb::blocked_range<size_t>& r)
  : m_Range({ r.begin(), r.end() })
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
   * @brief Returns the minimum index in the range.
   * @return
   */
  size_t min() const
  {
    return m_Range[0];
  }

  /**
   * @brief Returns the maximum index in the range.
   * @return
   */
  size_t max() const
  {
    return m_Range[1];
  }

  /**
   * @brief Returns the number of indices in the range.
   * @return
   */
  size_t size() const
  {
    return m_Range[1] - m_Range[0];
  }

  /**
   * @brief Returns true if the range is empty.  Returns false otherwise.
   * @return
   */
  bool empty() const
  {
    return size() == 0;
  }

  /**
   * @brief Returns the range based on the specified index.  The range is
   * organized as [min, max]
   */
  size_t operator[](size_t index) const
  {
    switch(index)
    {
    case 0:
      return min();
    case 1:
      return max();
    default:
      throw std::range_error("Range must be 0 or 1");
    }
  }

private:
  RangeType m_Range;
};
