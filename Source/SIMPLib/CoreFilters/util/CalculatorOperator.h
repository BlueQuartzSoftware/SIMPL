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
#include "CalculatorNumber.h"

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

#define EXECUTE_ARRAY_NUMBER_OPERATIONS(newArrayName, item1, item2, func)\
  if (NULL != qSharedPointerDynamicCast<CalculatorNumber>(item1) && NULL != qSharedPointerDynamicCast<CalculatorNumber>(item2))\
  {\
    double number1 = qSharedPointerDynamicCast<CalculatorNumber>(item1)->getNumber();\
    double number2 = qSharedPointerDynamicCast<CalculatorNumber>(item2)->getNumber();\
    return func(newArrayName, number1, number2);\
  }\
  if (NULL != qSharedPointerDynamicCast<CalculatorArray>(item1) && NULL != qSharedPointerDynamicCast<CalculatorNumber>(item2))\
  {\
    IDataArray::Pointer array1 = qSharedPointerDynamicCast<CalculatorArray>(item1)->getArray();\
    double number2 = qSharedPointerDynamicCast<CalculatorNumber>(item2)->getNumber();\
    return func(newArrayName, array1, number2);\
  }\
  if (NULL != qSharedPointerDynamicCast<CalculatorNumber>(item1) && NULL != qSharedPointerDynamicCast<CalculatorArray>(item2))\
  {\
    double number1 = qSharedPointerDynamicCast<CalculatorNumber>(item1)->getNumber();\
    IDataArray::Pointer array2 = qSharedPointerDynamicCast<CalculatorArray>(item2)->getArray();\
    return func(newArrayName, array2, number1);\
  }\
  if (NULL != qSharedPointerDynamicCast<CalculatorArray>(item1) && NULL != qSharedPointerDynamicCast<CalculatorArray>(item2))\
  {\
    IDataArray::Pointer array1 = qSharedPointerDynamicCast<CalculatorArray>(item1)->getArray();\
    IDataArray::Pointer array2 = qSharedPointerDynamicCast<CalculatorArray>(item2)->getArray();\
    EXECUTE_OPERATOR_FUNCTION(newArrayName, array1, array2, add)\
  }\


#define EXECUTE_OPERATOR_FUNCTION(newArrayName, ptr1, ptr2, func)\
    if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<FloatArrayType, FloatArrayType>(newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<FloatArrayType, DoubleArrayType>(newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<FloatArrayType, Int8ArrayType>(newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<FloatArrayType, UInt8ArrayType>(newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<FloatArrayType, Int16ArrayType>(newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<FloatArrayType, UInt16ArrayType>(newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<FloatArrayType, Int32ArrayType>(newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<FloatArrayType, UInt32ArrayType>(newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<FloatArrayType, Int64ArrayType>(newArrayName, ptr1, ptr2);\
    }\
    else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<FloatArrayType, UInt64ArrayType>(newArrayName, ptr1, ptr2);\
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<DoubleArrayType, FloatArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<DoubleArrayType, DoubleArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<DoubleArrayType, Int8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<DoubleArrayType, UInt8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<DoubleArrayType, Int16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<DoubleArrayType, UInt16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<DoubleArrayType, Int32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<DoubleArrayType, UInt32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<DoubleArrayType, Int64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<DoubleArrayType, UInt64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<Int8ArrayType, FloatArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<Int8ArrayType, DoubleArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<Int8ArrayType, Int8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<Int8ArrayType, UInt8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<Int8ArrayType, Int16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<Int8ArrayType, UInt16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<Int8ArrayType, Int32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<Int8ArrayType, UInt32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<Int8ArrayType, Int64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<Int8ArrayType, UInt64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<UInt8ArrayType, FloatArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<UInt8ArrayType, DoubleArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<UInt8ArrayType, Int8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<UInt8ArrayType, UInt8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<UInt8ArrayType, Int16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<UInt8ArrayType, UInt16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<UInt8ArrayType, Int32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<UInt8ArrayType, UInt32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<UInt8ArrayType, Int64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<UInt8ArrayType, UInt64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<Int16ArrayType, FloatArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<Int16ArrayType, DoubleArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<Int16ArrayType, Int8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<Int16ArrayType, UInt8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<Int16ArrayType, Int16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<Int16ArrayType, UInt16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<Int16ArrayType, Int32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<Int16ArrayType, UInt32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<Int16ArrayType, Int64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<Int16ArrayType, UInt64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<UInt16ArrayType, FloatArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<UInt16ArrayType, DoubleArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<UInt16ArrayType, Int8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<UInt16ArrayType, UInt8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<UInt16ArrayType, Int16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<UInt16ArrayType, UInt16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<UInt16ArrayType, Int32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<UInt16ArrayType, UInt32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<UInt16ArrayType, Int64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<UInt16ArrayType, UInt64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<Int32ArrayType, FloatArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<Int32ArrayType, DoubleArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<Int32ArrayType, Int8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<Int32ArrayType, UInt8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<Int32ArrayType, Int16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<Int32ArrayType, UInt16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<Int32ArrayType, Int32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<Int32ArrayType, UInt32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<Int32ArrayType, Int64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<Int32ArrayType, UInt64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<UInt32ArrayType, FloatArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<UInt32ArrayType, DoubleArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<UInt32ArrayType, Int8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<UInt32ArrayType, UInt8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<UInt32ArrayType, Int16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<UInt32ArrayType, UInt16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<UInt32ArrayType, Int32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<UInt32ArrayType, UInt32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<UInt32ArrayType, Int64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<UInt32ArrayType, UInt64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<Int64ArrayType, FloatArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<Int64ArrayType, DoubleArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<Int64ArrayType, Int8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<Int64ArrayType, UInt8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<Int64ArrayType, Int16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<Int64ArrayType, UInt16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<Int64ArrayType, Int32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<Int64ArrayType, UInt32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<Int64ArrayType, Int64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<Int64ArrayType, UInt64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<FloatArrayType>()(ptr2))\
    {\
      return func<UInt64ArrayType, FloatArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<DoubleArrayType>()(ptr2))\
    {\
      return func<UInt64ArrayType, DoubleArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int8ArrayType>()(ptr2))\
    {\
      return func<UInt64ArrayType, Int8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(ptr2))\
    {\
      return func<UInt64ArrayType, UInt8ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int16ArrayType>()(ptr2))\
    {\
      return func<UInt64ArrayType, Int16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(ptr2))\
    {\
      return func<UInt64ArrayType, UInt16ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int32ArrayType>()(ptr2))\
    {\
      return func<UInt64ArrayType, Int32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(ptr2))\
    {\
      return func<UInt64ArrayType, UInt32ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<Int64ArrayType>()(ptr2))\
    {\
      return func<UInt64ArrayType, Int64ArrayType>(newArrayName, ptr1, ptr2); \
    }\
    else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr1) && TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(ptr2))\
    {\
      return func<UInt64ArrayType, UInt64ArrayType>(newArrayName, ptr1, ptr2); \
    }\

#endif /* _CalculatorOperator_H_ */
