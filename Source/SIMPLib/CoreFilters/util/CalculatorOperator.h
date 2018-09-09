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

#pragma once

#include <QtCore/QSharedPointer>
#include <QtCore/QVector>
#include <QtCore/QStack>

#include "SIMPLib/SIMPLib.h"

#include "CalculatorArray.hpp"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class SIMPLib_EXPORT CalculatorOperator : public CalculatorItem
{
  public:
    enum OperatorType
    {
      Unary,
      Binary
    };

    SIMPL_SHARED_POINTERS(CalculatorOperator)

    static double toDegrees(double radians);
    static double toRadians(double degrees);

    ~CalculatorOperator() override;

    bool hasHigherPrecedence(CalculatorOperator::Pointer other);

    virtual void calculate(AbstractFilter* filter, DataArrayPath calculatedArrayPath, QStack<ICalculatorArray::Pointer>& executionStack) = 0;

    OperatorType getOperatorType();

  protected:
    CalculatorOperator();

    enum Precedence
    {
      Unknown_Precedence,
      A_Precedence,
      B_Precedence,
      C_Precedence,
      D_Precedence,
      E_Precedence
    };

    double root(double base, double root);

    Precedence getPrecedence();
    void setPrecedence(Precedence precedence);

    void setOperatorType(OperatorType type);

  private:
    Precedence                                      m_Precedence;
    OperatorType                                    m_OperatorType;

    CalculatorOperator(const CalculatorOperator&) = delete; // Copy Constructor Not Implemented
    void operator=(const CalculatorOperator&) = delete;     // Move assignment Not Implemented
};

#define CREATE_NEW_ARRAY_TWO_ARGUMENTS(filter, calculatedArrayPath, executionStack, func)                                                                                                              \
  ArrayCalculator* calculatorFilter = dynamic_cast<ArrayCalculator*>(filter);                                                                                                                          \
                                                                                                                                                                                                       \
  if(executionStack.size() >= 2 && nullptr != executionStack.top() && nullptr != calculatorFilter)                                                                                                     \
  {                                                                                                                                                                                                    \
    ICalculatorArray::Pointer array1 = executionStack.pop();                                                                                                                                           \
    ICalculatorArray::Pointer array2 = executionStack.pop();                                                                                                                                           \
                                                                                                                                                                                                       \
    DoubleArrayType::Pointer newArray;                                                                                                                                                                 \
    if(array1->getType() == ICalculatorArray::Array)                                                                                                                                                   \
    {                                                                                                                                                                                                  \
      newArray = DoubleArrayType::CreateArray(array1->getArray()->getNumberOfTuples(), array1->getArray()->getComponentDimensions(), calculatedArrayPath.getDataArrayName());                          \
    }                                                                                                                                                                                                  \
    else                                                                                                                                                                                               \
    {                                                                                                                                                                                                  \
      newArray = DoubleArrayType::CreateArray(array2->getArray()->getNumberOfTuples(), array2->getArray()->getComponentDimensions(), calculatedArrayPath.getDataArrayName());                          \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    int numComps = newArray->getNumberOfComponents();                                                                                                                                                  \
    for(int i = 0; i < static_cast<int>(newArray->getNumberOfTuples()); i++)                                                                                                                           \
    {                                                                                                                                                                                                  \
      for(int c = 0; c < newArray->getNumberOfComponents(); c++)                                                                                                                                       \
      {                                                                                                                                                                                                \
        int index = numComps * i + c;                                                                                                                                                                  \
        double num1 = array1->getValue(index);                                                                                                                                                         \
        double num2 = array2->getValue(index);                                                                                                                                                         \
        newArray->setValue(index, func(num2, num1));                                                                                                                                                   \
      }                                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    if(array1->getType() == ICalculatorArray::Array || array2->getType() == ICalculatorArray::Array)                                                                                                   \
    {                                                                                                                                                                                                  \
      executionStack.push(CalculatorArray<double>::New(newArray, ICalculatorArray::Array, true));                                                                                                      \
    }                                                                                                                                                                                                  \
    else                                                                                                                                                                                               \
    {                                                                                                                                                                                                  \
      executionStack.push(CalculatorArray<double>::New(newArray, ICalculatorArray::Number, true));                                                                                                     \
    }                                                                                                                                                                                                  \
    return;                                                                                                                                                                                            \
  }

