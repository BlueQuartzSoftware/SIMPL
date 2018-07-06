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

#include <QtCore/QObject>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

class SIMPLib_EXPORT CalculatorItem
{
  public:
    SIMPL_SHARED_POINTERS(CalculatorItem)

    virtual ~CalculatorItem();

    using EnumType = int;

    enum class ErrorCode : EnumType
    {
      SUCCESS = 0,
      INVALID_EQUATION = -4009,
      INVALID_COMPONENT = -4010,
      EMPTY_EQUATION = -4011,
      EMPTY_CAL_ARRAY = -4012,
      EMPTY_SEL_MATRIX = -4013,
      LOST_ATTR_MATRIX = -4014,
      INCORRECT_TUPLE_COUNT = -4015,
      INCONSISTENT_TUPLES = -4016,
      UNRECOGNIZED_ITEM = -4017,
      MISMATCHED_PARENTHESES = -4018,
      UNEXPECTED_OUTPUT = -4019,
      COMPONENT_OUT_OF_RANGE = -4020,
      INVALID_ARRAY_NAME = -4022,
      INCONSISTENT_INDEXING = -4023,
      INCONSISTENT_COMP_DIMS = -4024,
      ATTRARRAY_ZEROTUPLES_WARNING = -4025,
      ORPHANED_COMPONENT = -4026,
      OPERATOR_NO_LEFT_VALUE = -4027,
      OPERATOR_NO_RIGHT_VALUE = -4028,
      OPERATOR_NO_OPENING_PAREN = -4029,
      OPERATOR_NO_CLOSING_PAREN = -4030,
      NO_NUMERIC_ARGUMENTS = -4031,
      MISSING_ARGUMENTS = -4032,
      NOT_ENOUGH_ARGUMENTS = -4033,
      TOO_MANY_ARGUMENTS = -4034,
      INVALID_SYMBOL = -4035,
      NO_PRECEDING_UNARY_OPERATOR = -4036,
      InvalidOutputArrayType = -4037
    };

    enum class WarningCode : EnumType
    {
      NONE = 0,
      NUMERIC_VALUE_WARNING = -5010,
      AMBIGUOUS_NAME_WARNING = -5011
    };

    QString getInfixToken();

    virtual CalculatorItem::ErrorCode checkValidity(QVector<CalculatorItem::Pointer> infixVector, int currentIndex, QString& msg) = 0;

    bool isICalculatorArray();

    bool isArray();

    bool isNumber();

  protected:
    CalculatorItem();

    void setInfixToken(const QString& token);

  private:
    QString m_InfixToken = "";

    CalculatorItem(const CalculatorItem&) = delete; // Copy Constructor Not Implemented
    void operator=(const CalculatorItem&) = delete; // Move assignment Not Implemented
};

