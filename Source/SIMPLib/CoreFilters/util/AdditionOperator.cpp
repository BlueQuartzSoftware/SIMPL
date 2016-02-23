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

#include "AdditionOperator.h"

#include "SIMPLib/Common/TemplateHelpers.hpp"

#include "CalculatorNumber.h"
#include "CalculatorArray.h"

#define ADD_NUMBER_TO_ARRAY(iDataPtr, dataType, newArray, numberVal)\
  dataType::Pointer arrayCast = std::dynamic_pointer_cast<dataType>(iDataPtr);\
  for (int i = 0; i < arrayCast->getNumberOfTuples(); i++)\
    {\
      double dblValue = static_cast<double>(arrayCast->getValue(i)) + static_cast<double>(numberVal);\
      newArray->initializeTuple(i, &dblValue);\
    }\

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdditionOperator::AdditionOperator() :
  CalculatorOperator()
{
  setPrecedenceId(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdditionOperator::~AdditionOperator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSharedPointer<CalculatorItem> AdditionOperator::calculate(AbstractFilter* filter, const QString &newArrayName, QStack<QSharedPointer<CalculatorItem> > &executionStack)
{
  if (executionStack.size() >= 2)
  {
    EXECUTE_ARRAY_NUMBER_OPERATIONS(filter, newArrayName, executionStack.pop(), executionStack.pop(), add)
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
QSharedPointer<CalculatorItem> AdditionOperator::add(AbstractFilter* filter, const QString &newArrayName, IDataArray::Pointer dataArray, double number)
{
  DataArray<double>::Pointer newArray = DataArray<double>::CreateArray(dataArray->getNumberOfTuples(), newArrayName);

  if (TemplateHelpers::CanDynamicCast<FloatArrayType>()(dataArray))
  {
    ADD_NUMBER_TO_ARRAY(dataArray, FloatArrayType, newArray, number)
  }
  else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(dataArray))
  {
    ADD_NUMBER_TO_ARRAY(dataArray, DoubleArrayType, newArray, number)
  }
  else if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(dataArray))
  {
    ADD_NUMBER_TO_ARRAY(dataArray, Int8ArrayType, newArray, number)
  }
  else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(dataArray))
  {
    ADD_NUMBER_TO_ARRAY(dataArray, UInt8ArrayType, newArray, number)
  }
  else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(dataArray))
  {
    ADD_NUMBER_TO_ARRAY(dataArray, Int16ArrayType, newArray, number)
  }
  else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(dataArray))
  {
    ADD_NUMBER_TO_ARRAY(dataArray, UInt16ArrayType, newArray, number)
  }
  else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(dataArray))
  {
    ADD_NUMBER_TO_ARRAY(dataArray, Int32ArrayType, newArray, number)
  }
  else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(dataArray))
  {
    ADD_NUMBER_TO_ARRAY(dataArray, UInt32ArrayType, newArray, number)
  }
  else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(dataArray))
  {
    ADD_NUMBER_TO_ARRAY(dataArray, Int64ArrayType, newArray, number)
  }
  else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(dataArray))
  {
    ADD_NUMBER_TO_ARRAY(dataArray, UInt64ArrayType, newArray, number)
  }

  QSharedPointer<CalculatorItem> newItem = QSharedPointer<CalculatorArray>(new CalculatorArray(newArray));
  return newItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename J, typename K>
QSharedPointer<CalculatorItem> AdditionOperator::add(AbstractFilter* filter, const QString &newArrayName, IDataArray::Pointer dataArray1, IDataArray::Pointer dataArray2)
{
  J::Pointer arrayCast1 = std::dynamic_pointer_cast<J>(dataArray1);
  K::Pointer arrayCast2 = std::dynamic_pointer_cast<K>(dataArray2);

  DataArray<double>::Pointer newArray = DataArray<double>::CreateArray(arrayCast1->getNumberOfTuples(), newArrayName);

  for (int i = 0; i < newArray->getNumberOfTuples(); i++)
  {
    double value = static_cast<double>(arrayCast1->getValue(i)) + static_cast<double>(arrayCast2->getValue(i));
    newArray->initializeTuple(i, &value);
  }

  QSharedPointer<CalculatorItem> newItem = QSharedPointer<CalculatorArray>(new CalculatorArray(newArray));
  return newItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSharedPointer<CalculatorItem> AdditionOperator::add(AbstractFilter* filter, const QString &newArrayName, double number1, double number2)
{
  double newNumber = number1 + number2;

  DataArray<double>::Pointer newArray = DataArray<double>::CreateArray(1, newArrayName);
  newArray->initializeTuple(0, &newNumber);

  QSharedPointer<CalculatorItem> newItem = QSharedPointer<CalculatorArray>(new CalculatorArray(newArray));
  return newItem;
}

