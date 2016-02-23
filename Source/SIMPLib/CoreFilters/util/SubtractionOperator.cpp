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

#include "SubtractionOperator.h"

#include "CalculatorNumber.h"

#define SUBTRACT_NUMBER_FROM_ARRAY(iDataPtr, dataType, newArray, numberVal)\
  dataType::Pointer arrayCast = std::dynamic_pointer_cast<dataType>(iDataPtr);\
  for (int i = 0; i < arrayCast->getNumberOfTuples(); i++)\
  {\
    double dblValue = static_cast<double>(arrayCast->getValue(i)) - static_cast<double>(numberVal);\
    newArray->initializeTuple(i, &dblValue);\
  }\

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SubtractionOperator::SubtractionOperator() :
CalculatorOperator()
{
  setPrecedenceId(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SubtractionOperator::~SubtractionOperator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSharedPointer<CalculatorItem> SubtractionOperator::calculate(AbstractFilter* filter, const QString &newArrayName, QStack<QSharedPointer<CalculatorItem> > &executionStack)
{
  if (executionStack.size() >= 2)
  {
    EXECUTE_ARRAY_NUMBER_OPERATIONS(filter, newArrayName, executionStack.pop(), executionStack.pop(), subtract)
  }

  // If the execution gets down here, then we have an error
  QString ss = QObject::tr("The chosen infix equation is not a valid equation.");
  filter->setErrorCondition(-4005);
  filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  return QSharedPointer<CalculatorItem>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSharedPointer<CalculatorItem> SubtractionOperator::subtract(AbstractFilter* filter, const QString &newArrayName, IDataArray::Pointer dataArray, double subtrahend)
{
  DataArray<double>::Pointer newArray = DataArray<double>::CreateArray(dataArray->getNumberOfTuples(), newArrayName);

  if (TemplateHelpers::CanDynamicCast<FloatArrayType>()(dataArray))
  {
    SUBTRACT_NUMBER_FROM_ARRAY(dataArray, FloatArrayType, newArray, subtrahend)
  }
  else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(dataArray))
  {
    SUBTRACT_NUMBER_FROM_ARRAY(dataArray, DoubleArrayType, newArray, subtrahend)
  }
  else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(dataArray))
  {
    SUBTRACT_NUMBER_FROM_ARRAY(dataArray, Int8ArrayType, newArray, subtrahend)
  }
  else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(dataArray))
  {
    SUBTRACT_NUMBER_FROM_ARRAY(dataArray, UInt8ArrayType, newArray, subtrahend)
  }
  else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(dataArray))
  {
    SUBTRACT_NUMBER_FROM_ARRAY(dataArray, Int16ArrayType, newArray, subtrahend)
  }
  else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(dataArray))
  {
    SUBTRACT_NUMBER_FROM_ARRAY(dataArray, UInt16ArrayType, newArray, subtrahend)
  }
  else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(dataArray))
  {
    SUBTRACT_NUMBER_FROM_ARRAY(dataArray, Int32ArrayType, newArray, subtrahend)
  }
  else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(dataArray))
  {
    SUBTRACT_NUMBER_FROM_ARRAY(dataArray, UInt32ArrayType, newArray, subtrahend)
  }
  else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(dataArray))
  {
    SUBTRACT_NUMBER_FROM_ARRAY(dataArray, Int64ArrayType, newArray, subtrahend)
  }
  else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(dataArray))
  {
    SUBTRACT_NUMBER_FROM_ARRAY(dataArray, UInt64ArrayType, newArray, subtrahend)
  }

  QSharedPointer<CalculatorItem> newItem = QSharedPointer<CalculatorArray>(new CalculatorArray(newArray));
  return newItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename J, typename K>
QSharedPointer<CalculatorItem> SubtractionOperator::subtract(AbstractFilter* filter, const QString &newArrayName, IDataArray::Pointer subtrahend, IDataArray::Pointer minuend)
{
  J::Pointer subtrahendCast = std::dynamic_pointer_cast<J>(subtrahend);
  K::Pointer minuendCast = std::dynamic_pointer_cast<K>(minuend);

  DataArray<double>::Pointer newArray = DataArray<double>::CreateArray(minuendCast->getNumberOfTuples(), newArrayName);

  for (int i = 0; i < newArray->getNumberOfTuples(); i++)
  {
    double value = static_cast<double>(minuendCast->getValue(i)) - static_cast<double>(subtrahendCast->getValue(i));
    newArray->initializeTuple(i, &value);
  }

  QSharedPointer<CalculatorItem> newItem = QSharedPointer<CalculatorArray>(new CalculatorArray(newArray));
  return newItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSharedPointer<CalculatorItem> SubtractionOperator::subtract(AbstractFilter* filter, const QString &newArrayName, double subtrahend, double minuend)
{
  double newNumber = minuend - subtrahend;

  DataArray<double>::Pointer newArray = DataArray<double>::CreateArray(1, newArrayName);
  newArray->initializeTuple(0, &newNumber);

  QSharedPointer<CalculatorItem> newItem = QSharedPointer<CalculatorArray>(new CalculatorArray(newArray));
  return newItem;
}

