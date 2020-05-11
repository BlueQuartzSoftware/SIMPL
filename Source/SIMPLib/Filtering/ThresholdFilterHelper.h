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
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/IDataArrayFilter.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The ThresholdFilterHelper class
 */
class SIMPLib_EXPORT ThresholdFilterHelper : public IDataArrayFilter
{
public:
  ThresholdFilterHelper(SIMPL::Comparison::Enumeration compType, double compValue, BoolArrayType* output);

  ~ThresholdFilterHelper() override;

  /**
   *
   */
  template <typename T>
  void filterDataLessThan(const IDataArray::Pointer& m_Input)
  {
    size_t m_NumValues = m_Input->getNumberOfTuples();
    T v = static_cast<T>(comparisonValue);
    using DataArrayType = DataArray<T>;
    typename DataArrayType::Pointer dataPtr = std::dynamic_pointer_cast<DataArrayType>(m_Input);
    T* data = dataPtr->getTuplePointer(0);
    for(size_t i = 0; i < m_NumValues; ++i)
    {
      bool b = (data[i] < v);
      m_Output->setValue(i, b);
    }
  }

  /**
   *
   */
  template <typename T>
  void filterDataGreaterThan(const IDataArray::Pointer& m_Input)
  {
    size_t m_NumValues = m_Input->getNumberOfTuples();
    T v = static_cast<T>(comparisonValue);
    using DataArrayType = DataArray<T>;
    typename DataArrayType::Pointer dataPtr = std::dynamic_pointer_cast<DataArrayType>(m_Input);
    T* data = dataPtr->getTuplePointer(0);
    for(size_t i = 0; i < m_NumValues; ++i)
    {
      bool b = (data[i] > v);
      m_Output->setValue(i, b);
    }
  }

  /**
   *
   */
  template <typename T>
  void filterDataEqualTo(const IDataArray::Pointer& m_Input)
  {
    size_t m_NumValues = m_Input->getNumberOfTuples();
    T v = static_cast<T>(comparisonValue);
    using DataArrayType = DataArray<T>;
    typename DataArrayType::Pointer dataPtr = std::dynamic_pointer_cast<DataArrayType>(m_Input);
    T* data = dataPtr->getTuplePointer(0);
    for(size_t i = 0; i < m_NumValues; ++i)
    {
      bool b = (data[i] == v);
      m_Output->setValue(i, b);
    }
  }

  /**
   *
   */
  template <typename T>
  void filterDataNotEqualTo(const IDataArray::Pointer& m_Input)
  {
    size_t m_NumValues = m_Input->getNumberOfTuples();
    T v = static_cast<T>(comparisonValue);
    using DataArrayType = DataArray<T>;
    typename DataArrayType::Pointer dataPtr = std::dynamic_pointer_cast<DataArrayType>(m_Input);
    T* data = dataPtr->getTuplePointer(0);
    for(size_t i = 0; i < m_NumValues; ++i)
    {
      bool b = (data[i] != v);
      m_Output->setValue(i, b);
    }
  }

  /**
   * @brief execute
   * @param input
   * @param output
   * @return
   */
  int execute(const IDataArray::Pointer& input, IDataArray* output);

private:
  SIMPL::Comparison::Enumeration comparisonOperator;
  double comparisonValue;
  BoolArrayType* m_Output;

public:
  ThresholdFilterHelper(const ThresholdFilterHelper&) = delete;            // Copy Constructor Not Implemented
  ThresholdFilterHelper(ThresholdFilterHelper&&) = delete;                 // Move Constructor Not Implemented
  ThresholdFilterHelper& operator=(const ThresholdFilterHelper&) = delete; // Copy Assignment Not Implemented
  ThresholdFilterHelper& operator=(ThresholdFilterHelper&&) = delete;      // Move Assignment Not Implemented
};
