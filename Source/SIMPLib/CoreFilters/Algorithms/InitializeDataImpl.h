/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <array>
#include <memory>

#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/FilterParameters/RangeFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class InitializeData;
class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

/**
 * @brief The InitializeDataImpl class.
 */
class InitializeDataImpl
{
public:
  InitializeDataImpl(InitializeData* filter, IDataArrayShPtrType p, const std::array<int64_t, 3>& dims, const std::array<int64_t, 6>& bounds, int initType, bool invertData, double initValue,
                     FPRangePair initRange);

  virtual ~InitializeDataImpl();

  /**
   * @brief operator ()
   */
  void operator()() const;

private:
  InitializeData* m_Filter;
  IDataArrayShPtrType m_TargetArray;
  const std::array<int64_t, 3> m_Dims;
  const std::array<int64_t, 6> m_Bounds;
  int m_InitType;
  bool m_InvertData;
  double m_InitValue;
  FPRangePair m_InitRange;

  /**
   * @brief getRange Gets the range needed for the uniform distribution.
   */
  template <typename T>
  std::pair<T, T> getRange() const;

  /**
   * @brief initializeArrayWithInts Initializes the array p with integers, either from the
   * manual value entered in the filter, or with a random number.  This function does not
   * check that the template type actually is an integer, so it will most likely cause
   * unexpected results when passing anything other than an integer as a template parameter.
   * @param p The array that will be initialized
   * @param dims The dimensions of the array p
   */
  template <typename T>
  void initializeArrayWithInts() const;

  /**
   * @brief initializeArrayWithReals Initializes the array p with real numbers, either from the
   * manual value entered in the filter, or with a random number.  This function does not
   * check that the template type actually is a non-integer, so it will most likely cause
   * unexpected results when passing anything other than a float or double as a template
   * parameter.
   * @param p The array that will be initialized
   * @param dims The dimensions of the array p
   */
  template <typename T>
  void initializeArrayWithReals() const;

  /**
   * @brief initializeArrayWithBools Initializes the array p with booleans, either from the
   * manual value entered in the filter, or with a random boolean value.
   * @param p The array that will be initialized
   * @param dims The dimensions of the array p
   */
  void initializeArrayWithBools() const;
};
