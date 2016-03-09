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

#include "UnaryOperator.h"

#include "BinaryOperator.h"
#include "LeftParenthesisItem.h"
#include "RightParenthesisItem.h"
#include "CommaSeparator.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UnaryOperator::UnaryOperator() :
  CalculatorOperator(),
  m_NumOfArguments(-1)
{
  setPrecedence(Delta_Precedence);
  setOperatorType(Unary);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UnaryOperator::~UnaryOperator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double UnaryOperator::calculate(AbstractFilter* filter, const QString &newArrayName, QStack<ICalculatorArray::Pointer> &executionStack, int index)
{
  // This should never be executed
  return 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool UnaryOperator::checkValidity(QVector<CalculatorItem::Pointer> infixVector, int currentIndex)
{
  if (currentIndex - 1 >= 0 &&
    NULL == std::dynamic_pointer_cast<BinaryOperator>(infixVector[currentIndex-1])
    && NULL == std::dynamic_pointer_cast<LeftParenthesisItem>(infixVector[currentIndex - 1]))
  {
    return false;
  }

  int index = currentIndex + 1;
  int commaCount = 0;
  bool hasArray = false;
  if (index < infixVector.size() && NULL != std::dynamic_pointer_cast<LeftParenthesisItem>(infixVector[index]))
  {
    index++;

    // Iterate through the vector to find the matching right parenthesis
    for (; index < infixVector.size(); index++)
    {
      if (NULL != std::dynamic_pointer_cast<RightParenthesisItem>(infixVector[index]))
      {
        // We found the matching right parenthesis, so return true
        if (commaCount < m_NumOfArguments - 1
          || (index + 1 < infixVector.size() && NULL == std::dynamic_pointer_cast<BinaryOperator>(infixVector[index + 1])
          && NULL == std::dynamic_pointer_cast<RightParenthesisItem>(infixVector[index + 1])) || hasArray == false)
        {
          return false;
        }

        return true;
      }
      else if (NULL != std::dynamic_pointer_cast<LeftParenthesisItem>(infixVector[index]))
      {
        /* We found another left parenthesis, but we don't care what's inside this set of parentheses
           (other operators' checkValidity functions will take care of these values), so just iterate
           until we find the matching closing parenthesis for this opening parenthesis */
        while (index < infixVector.size() && NULL == std::dynamic_pointer_cast<RightParenthesisItem>(infixVector[index]))
        {
          if (NULL != std::dynamic_pointer_cast<ICalculatorArray>(infixVector[index]))
          {
            hasArray = true;
          }

          index++;
        }
      }
      else if (NULL != std::dynamic_pointer_cast<CommaSeparator>(infixVector[index]))
      {
        // We found a comma, so increase the comma count
        commaCount++;
        if (commaCount > m_NumOfArguments - 1)
        {
          // We found too many commas (meaning that there are too many arguments), so return false
          return false;
        }
      }
      else if (NULL != std::dynamic_pointer_cast<ICalculatorArray>(infixVector[index]))
      {
        hasArray = true;
      }
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int UnaryOperator::getNumberOfArguments()
{
  return m_NumOfArguments;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UnaryOperator::setNumberOfArguments(int numOfArguments)
{
  m_NumOfArguments = numOfArguments;
}

