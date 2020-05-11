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
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <array>

#include "SIMPLib/Common/SIMPLRange.h"
#include "SIMPLib/SIMPLib.h"

// SIMPLib.h MUST be included before this or the guard will block the include but not its uses below.
// This is consistent with previous behavior, only earlier parallelization split the includes between
// the corresponding .h and .cpp files.
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
// clang-format off
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
// clang-format on
#endif

/**
 * @brief The ParallelDataAlgorithm class handles parallelization across data-based algorithms.
 * A range is required, as well as an object with a matching function operator.  This class
 * utilizes TBB for parallelization and will fallback to non-parallelization if it is not
 * available or the parallelization is disabled.
 */
class SIMPLib_EXPORT ParallelDataAlgorithm
{
public:
  ParallelDataAlgorithm();
  virtual ~ParallelDataAlgorithm();

  /**
   * @brief Returns true if parallelization is enabled.  Returns false otherwise.
   * @return
   */
  bool getParallelizationEnabled() const;

  /**
   * @brief Sets whether parallelization is enabled.
   * @param doParallel
   */
  void setParallelizationEnabled(bool doParallel);

  /**
   * @brief Returns the range to operate over.
   * @return
   */
  SIMPLRange getRange() const;

  /**
   * @brief Sets the range to operate over.
   * @param range2D
   */
  void setRange(const SIMPLRange& range);

  /**
   * @brief Sets the range to operate over.
   * @param min
   * @param max
   */
  void setRange(size_t min, size_t max);

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  /**
   * @brief Sets the partitioner for parallelization.
   * @param partitioner
   */
  void setPartitioner(const tbb::auto_partitioner& partitioner);
#endif

  /**
   * @brief Runs the data algorithm.  Parallelization is used if appropriate.
   * @param body
   */
  template <typename Body>
  void execute(const Body& body)
  {
    bool doParallel = false;
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    doParallel = m_RunParallel;
    if(doParallel)
    {
      tbb::blocked_range<size_t> tbbRange(m_Range[0], m_Range[1]);
      tbb::parallel_for(tbbRange, body, m_Partitioner);
    }
#endif

    // Run non-parallel operation
    if(!doParallel)
    {
      body(m_Range);
    }
  }

private:
  SIMPLRange m_Range;
  bool m_RunParallel = false;
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::auto_partitioner m_Partitioner;
#endif
};
