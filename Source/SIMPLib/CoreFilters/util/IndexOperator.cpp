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

#include "IndexOperator.h"

#include <math.h>

#include "SIMPLib/CoreFilters/ArrayCalculator.h"

#include "CalculatorArray.hpp"
#include "BinaryOperator.h"
#include "LeftParenthesisItem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IndexOperator::IndexOperator(int index) :
CalculatorOperator(),
m_Index(index)
{
  setPrecedence(E_Precedence);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IndexOperator::~IndexOperator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IndexOperator::calculate(AbstractFilter* filter, DataArrayPath calculatedArrayPath, QStack<ICalculatorArray::Pointer> &executionStack)
{
  if (executionStack.size() >= 1)
  {
    ICalculatorArray::Pointer arrayPtr = executionStack.pop();

    int err = 0;
    DataArrayPath calculatedAMPath(calculatedArrayPath.getDataContainerName(), calculatedArrayPath.getAttributeMatrixName(), "");
    AttributeMatrix::Pointer calculatedAM = filter->getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(filter, calculatedAMPath, err);
    DoubleArrayType::Pointer newArray = DoubleArrayType::CreateArray(calculatedAM->getNumTuples(), QVector<size_t>(1, 1), calculatedArrayPath.getDataArrayName());

    int numComps = arrayPtr->getArray()->getNumberOfComponents();
    for (int i = 0; i < newArray->getNumberOfTuples(); i++)
    {
      int index = numComps * i + m_Index;
      double num = arrayPtr->getValue(index);
      newArray->setValue(i, num);
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
bool IndexOperator::checkValidity(QVector<CalculatorItem::Pointer> infixVector, int currentIndex)
{
  if (currentIndex - 1 >= 0 && NULL != std::dynamic_pointer_cast<ICalculatorArray>(infixVector[currentIndex-1]))
  {
    return true;
  }

  return false;
}


