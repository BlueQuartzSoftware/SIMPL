/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#ifndef _CalculatorOperator_H_
#define _CalculatorOperator_H_

#include <QtCore/QSharedPointer>
#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/DataArrays/IDataArray.h"

#include "CalculatorArray.h"

class SIMPLib_EXPORT CalculatorOperator : public CalculatorItem
{
  public:
    CalculatorOperator();
    virtual ~CalculatorOperator();

    bool hasHigherPrecedence(QSharedPointer<CalculatorOperator> other);

    virtual QSharedPointer<CalculatorItem> calculate(AbstractFilter* filter, const QString &newArrayName, QStack<QSharedPointer<CalculatorItem> > &executionStack);

  protected:
    int getPrecedenceId();
    void setPrecedenceId(int id);

  private:
    int                                             m_PrecedenceId;

    CalculatorOperator(const CalculatorOperator&); // Copy Constructor Not Implemented
    void operator=(const CalculatorOperator&); // Operator '=' Not Implemented
};

#define EXECUTE_FUNCTION_TWO_ARRAYS(filter, newArrayName, ptr1, ptr2, func)\
    if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<float, float>(filter, newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<float, double>(filter, newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<float, int8_t>(filter, newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<float, uint8_t>(filter, newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<float, int16_t>(filter, newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<float, uint16_t>(filter, newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<float, int32_t>(filter, newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<float, uint32_t>(filter, newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<float, int64_t>(filter, newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<float, uint64_t>(filter, newArrayName, ptr1, ptr2);\
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<double, float>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<double, double>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<double, int8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<double, uint8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<double, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<double, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<double, int32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<double, uint32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<double, int64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<double, uint64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<int8_t, float>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<int8_t, double>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<int8_t, int8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<int8_t, uint8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<int8_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<int8_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<int8_t, int32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<int8_t, uint32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<int8_t, int64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<int8_t, uint64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<uint8_t, float>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<uint8_t, double>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<uint8_t, int8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<uint8_t, uint8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<uint8_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<uint8_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<uint8_t, int32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<uint8_t, uint32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<uint8_t, int64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<uint8_t, uint64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<int16_t, float>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<int16_t, double>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<int16_t, int8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<int16_t, uint8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<int16_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<int16_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<int16_t, int32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<int16_t, uint32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<int16_t, int64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<int16_t, uint64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<uint16_t, float>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<uint16_t, double>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<uint16_t, int8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<uint16_t, uint8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<uint16_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<uint16_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<uint16_t, int32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<uint16_t, uint32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<uint16_t, int64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<uint16_t, uint64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<int32_t, float>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<int32_t, double>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<int32_t, int8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<int32_t, uint8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<int32_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<int32_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<int32_t, int32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<int32_t, uint32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<int32_t, int64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<int32_t, uint64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<uint32_t, float>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<uint32_t, double>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<uint32_t, int8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<uint32_t, uint8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<uint32_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<uint32_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<uint32_t, int32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<uint32_t, uint32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<uint32_t, int64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<uint32_t, uint64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<int64_t, float>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<int64_t, double>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<int64_t, int8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<int64_t, uint8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<int64_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<int64_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<int64_t, int32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<int64_t, uint32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<int64_t, int64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<int64_t, uint64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<uint64_t, float>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<uint64_t, double>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<uint64_t, int8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<uint64_t, uint8_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<uint64_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<uint64_t, int16_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<uint64_t, int32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<uint64_t, uint32_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<uint64_t, int64_t>(filter, newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<uint64_t, uint64_t>(filter, newArrayName, ptr1, ptr2); \
    }\

#endif /* _CalculatorOperator_H_ */
