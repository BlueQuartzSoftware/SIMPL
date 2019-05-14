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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/SIMPLib.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#endif

// =============================================================================
// This file defines macros used to run algorithms in parallel without breaking
// implementations where SIMPL_USE_PARALLEL_ALGORITHMS is not defined.
// This removes the requirements to copy and paste the contents of these macros
// in every class using parallel algorithms.
// =============================================================================

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
/**
 * @brief Create variables required for parallel algorithms
 */
#define SIMPL_INIT_PARALLEL_ALGORITHMS()                                                                                                                                                               \
  tbb::task_scheduler_init init;                                                                                                                                                                       \
  std::shared_ptr<tbb::task_group> g(new tbb::task_group);                                                                                                                                             \
  int32_t nthreads = static_cast<int32_t>(std::thread::hardware_concurrency()); /* Returns ZERO if not defined on this platform */                                                                     \
  int32_t threadCount = 0;
#else
/**
 * @brief Create variables required for parallel algorithms
 */
#define SIMPL_INIT_PARALLEL_ALGORITHMS()                                                                                                                                                               \
  {                                                                                                                                                                                                    \
  }
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
/**
 * @brief Sets the number of threads to use for parallel algorithms.
 * @param NumThreads
 */
#define SIMPL_SET_NUM_ALGORITHM_THREADS(NumThreads) nthreads = NumThreads;
#else
/**
 * @brief Sets the number of threads to use for parallel algorithms.
 * @param NumThreads
 */
#define SIMPL_SET_NUM_ALGORITHM_THREADS(NumThreads)                                                                                                                                                    \
  {                                                                                                                                                                                                    \
  }
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
/**
 * @brief Run the given function in parallel if SIMPL_USE_PARALLEL_ALGORITHMS is defined.  Otherwise, simply run the given function.
 * @param Func
 */
#define SIMPL_RUN_PARALLEL_ALGORITHM(Func)                                                                                                                                                             \
  g->run(findFFTConvolution(Func));                                                                                                                                                                    \
  threadCount++;                                                                                                                                                                                       \
  if(threadCount == nthreads)                                                                                                                                                                          \
  {                                                                                                                                                                                                    \
    g->wait();                                                                                                                                                                                         \
    threadCount = 0;                                                                                                                                                                                   \
  }
#else
/**
 * @brief Run the given function in parallel if SIMPL_USE_PARALLEL_ALGORITHMS is defined.  Otherwise, simply run the given function.
 * @param Func
 */
#define SIMPL_RUN_PARALLEL_ALGORITHM(Func) Funct;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
/**
 * @brief This will spill over if the number of files to process does not divide evenly by the number of threads.
 */
#define SIMPL_END_PARALLEL_ALGORITHMS()                                                                                                                                                                \
  g->wait();                                                                                                                                                                                           \
  threadCount = 0;
#else
/**
 * @brief This will spill over if the number of files to process does not divide evenly by the number of threads.
 */
#define SIMPL_END_PARALLEL_ALGORITHMS()                                                                                                                                                                \
  {                                                                                                                                                                                                    \
  }
#endif