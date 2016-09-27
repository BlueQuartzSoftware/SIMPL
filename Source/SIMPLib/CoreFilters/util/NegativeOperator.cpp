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

#include "NegativeOperator.h"

#include <math.h>

#include "SIMPLib/CoreFilters/ArrayCalculator.h"

#include "BinaryOperator.h"
#include "CalculatorArray.hpp"
#include "LeftParenthesisItem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NegativeOperator::NegativeOperator()
: CalculatorOperator()
{
  setPrecedence(D_Precedence);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NegativeOperator::~NegativeOperator()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NegativeOperator::calculate(AbstractFilter* filter, DataArrayPath calculatedArrayPath, QStack<ICalculatorArray::Pointer>& executionStack)
{
  if(executionStack.size() >= 1)
  {
    ICalculatorArray::Pointer arrayPtr = executionStack.pop();

    DoubleArrayType::Pointer newArray = createNewArray(filter, calculatedArrayPath, arrayPtr);

    int numComps = newArray->getNumberOfComponents();
    for(int i = 0; i < newArray->getNumberOfTuples(); i++)
    {
      for(int c = 0; c < newArray->getNumberOfComponents(); c++)
      {
        int index = numComps * i + c;
        double num = arrayPtr->getValue(index);
        newArray->setValue(index, -1 * num);
      }
    }

    executionStack.push(CalculatorArray<double>::New(newArray, arrayPtr->getType(), true));
    return;
  }

  QString ss = QObject::tr("The chosen infix equation is not a valid equation.");
  filter->setErrorCondition(ArrayCalculator::INVALID_EQUATION);
  filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool NegativeOperator::checkValidity(QVector<CalculatorItem::Pointer> infixVector, int currentIndex)
{
  if(currentIndex - 1 < 0 || (currentIndex - 1 >= 0 && (nullptr != std::dynamic_pointer_cast<BinaryOperator>(infixVector[currentIndex - 1]) ||
                                                        nullptr != std::dynamic_pointer_cast<LeftParenthesisItem>(infixVector[currentIndex - 1]))))
  {
    if(currentIndex + 1 < infixVector.size() &&
       (nullptr != std::dynamic_pointer_cast<ICalculatorArray>(infixVector[currentIndex + 1]) || nullptr != std::dynamic_pointer_cast<LeftParenthesisItem>(infixVector[currentIndex + 1]) ||
        nullptr != std::dynamic_pointer_cast<UnaryOperator>(infixVector[currentIndex + 1])))
    {
      return true;
    }
  }

  return false;
}
