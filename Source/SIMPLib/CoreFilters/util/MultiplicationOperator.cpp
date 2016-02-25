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

#include "MultiplicationOperator.h"

#include "CalculatorNumber.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiplicationOperator::MultiplicationOperator() :
CalculatorOperator()
{
  setPrecedenceId(1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiplicationOperator::~MultiplicationOperator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSharedPointer<CalculatorItem> MultiplicationOperator::calculate(AbstractFilter* filter, const QString &newArrayName, QStack<QSharedPointer<CalculatorItem> > &executionStack)
{
  if (executionStack.size() >= 2)
  {
    QSharedPointer<CalculatorItem> item1 = executionStack.pop();
    QSharedPointer<CalculatorItem> item2 = executionStack.pop();
    EXECUTE_ARRAY_NUMBER_OPERATIONS(filter, newArrayName, item1, item2, multiply)
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
template <typename T>
QSharedPointer<CalculatorItem> MultiplicationOperator::multiply(AbstractFilter* filter, const QString &newArrayName, IDataArray::Pointer dataArray, double multiplier)
{
  DataArray<double>::Pointer newArray = DataArray<double>::CreateArray(dataArray->getNumberOfTuples(), newArrayName);

  typename T::Pointer arrayCast = std::dynamic_pointer_cast<T>(dataArray);
  for (int i = 0; i < arrayCast->getNumberOfTuples(); i++)
  {
    double dblValue = static_cast<double>(arrayCast->getValue(i)) * static_cast<double>(multiplier);
    newArray->initializeTuple(i, &dblValue);
  }

  QSharedPointer<CalculatorItem> newItem = QSharedPointer<CalculatorArray>(new CalculatorArray(newArray));
  return newItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename J, typename K>
QSharedPointer<CalculatorItem> MultiplicationOperator::multiply(AbstractFilter* filter, const QString &newArrayName, IDataArray::Pointer multiplier, IDataArray::Pointer multiplicand)
{
  typename J::Pointer multiplierCast = std::dynamic_pointer_cast<J>(multiplier);
  typename K::Pointer multiplicandCast = std::dynamic_pointer_cast<K>(multiplicand);

  DataArray<double>::Pointer newArray = DataArray<double>::CreateArray(multiplicandCast->getNumberOfTuples(), newArrayName);

  for (int i = 0; i < newArray->getNumberOfTuples(); i++)
  {
    double value = static_cast<double>(multiplicandCast->getValue(i)) * static_cast<double>(multiplierCast->getValue(i));
    newArray->initializeTuple(i, &value);
  }

  QSharedPointer<CalculatorItem> newItem = QSharedPointer<CalculatorArray>(new CalculatorArray(newArray));
  return newItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSharedPointer<CalculatorItem> MultiplicationOperator::multiply(AbstractFilter* filter, const QString &newArrayName, double multiplier, double multiplicand)
{
  double newNumber = multiplicand * multiplier;

  QSharedPointer<CalculatorItem> newItem = QSharedPointer<CalculatorNumber>(new CalculatorNumber(newNumber));
  return newItem;
}

