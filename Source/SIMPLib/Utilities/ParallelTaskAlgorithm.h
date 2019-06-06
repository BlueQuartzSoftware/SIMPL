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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
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

#include "SIMPLib/SIMPLib.h"

// SIMPLib.h MUST be included before this or the guard will block the include but not its uses below.
// This is consistent with previous behavior, only earlier parallelization split the includes between
// the corresponding .h and .cpp files.
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
// clang-format off
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
// clang-format on
#endif

/**
 * @brief The ParallelTaskAlgorithm class handles parallelization across task-based algorithms.
 * An object with a function operator is required to operate the task.  This class utilizes
 * TBB for parallelization and will fallback to non-parallelization if it is not available
 * or the parallelization is disabled.
 */
class SIMPLib_EXPORT ParallelTaskAlgorithm
{
public:
  ParallelTaskAlgorithm();
  virtual ~ParallelTaskAlgorithm();

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
   * @brief Return maximum threads to use for parallelization.  If Parallel Algorithms
   * is not enabled, the maximum hardware concurrency is returned instead.
   * @return
   */
  uint32_t getMaxThreads() const;

  /**
   * @brief Sets the maximum number of threads to use.  This amount is automatically
   * reduced to the max hardware concurrency.
   * @param threads
   */
  void setMaxThreads(uint32_t threads);

  /**
   * @brief Executes the given object's function operator.  If parallel algorithms
   * is enabled, this process is multi-threaded.  Otherwise, this process is done
   * in a single thread.
   * @param body
   */
  template <typename Body>
  void execute(const Body& body)
  {
    bool doParallel = false;
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    doParallel = m_Parallelization;
    if(doParallel)
    {
      m_TaskGroup->run(body);
      m_CurThreads++;
      if(m_CurThreads >= m_MaxThreads)
      {
        wait();
      }
    }
#else
    body();
#endif
  }

  /**
   * @brief Waits for the threads to finish and resets the current thread count.
   */
  void wait();

private:
  bool m_Parallelization = false;
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  uint32_t m_MaxThreads = 1;
  uint32_t m_CurThreads = 0;
  tbb::task_scheduler_init init;
  std::shared_ptr<tbb::task_group> m_TaskGroup;
#endif
};
