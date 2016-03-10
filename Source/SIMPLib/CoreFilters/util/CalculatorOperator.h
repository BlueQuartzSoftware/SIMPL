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

#define _USE_MATH_DEFINES
#include <math.h>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/DataArrays/IDataArray.h"

#include "CalculatorArray.hpp"

class SIMPLib_EXPORT CalculatorOperator : public CalculatorItem
{
  public:
    enum OperatorType
    {
      Unary,
      Binary
    };

    SIMPL_SHARED_POINTERS(CalculatorOperator)

    static Pointer New()
    {
      return Pointer(new CalculatorOperator());
    }

    static double toDegrees(double radians);
    static double toRadians(double degrees);

    virtual ~CalculatorOperator();

    bool hasHigherPrecedence(CalculatorOperator::Pointer other);

    virtual double calculate(AbstractFilter* filter, const QString &newArrayName, QStack<ICalculatorArray::Pointer> &executionStack, int index);

    virtual bool checkValidity(QVector<CalculatorItem::Pointer> infixVector, int currentIndex);

    OperatorType getOperatorType();

  protected:
    CalculatorOperator();

    enum Precedence
    {
      Unknown_Precedence,
      Alpha_Precedence,
      Bravo_Precedence,
      Charlie_Precedence,
      Delta_Precedence,
      Episilon_Precedence
    };

    double root(double base, double root);

    Precedence getPrecedence();
    void setPrecedence(Precedence precedence);

    void setOperatorType(OperatorType type);

  private:
    Precedence                                      m_Precedence;
    OperatorType                                    m_OperatorType;

    CalculatorOperator(const CalculatorOperator&); // Copy Constructor Not Implemented
    void operator=(const CalculatorOperator&); // Operator '=' Not Implemented
};

#endif /* _CalculatorOperator_H_ */
