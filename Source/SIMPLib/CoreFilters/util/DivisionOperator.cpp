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

#include "DivisionOperator.h"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigen>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DivisionOperator::DivisionOperator() :
CalculatorOperator()
{
  setPrecedenceId(1);
  setOperatorType(Binary);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DivisionOperator::~DivisionOperator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSharedPointer<CalculatorItem> DivisionOperator::calculate(AbstractFilter* filter, const QString &newArrayName, QStack<QSharedPointer<CalculatorItem> > &executionStack)
{
  if (executionStack.size() >= 2)
  {
    IDataArray::Pointer item1 = qSharedPointerDynamicCast<CalculatorArray>(executionStack.pop())->getArray();
    IDataArray::Pointer item2 = qSharedPointerDynamicCast<CalculatorArray>(executionStack.pop())->getArray();

    EXECUTE_FUNCTION_TWO_ARRAYS(filter, newArrayName, item1, item2, divide)
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
template <typename J, typename K>
QSharedPointer<CalculatorItem> DivisionOperator::divide(AbstractFilter* filter, const QString &newArrayName, IDataArray::Pointer divisor, IDataArray::Pointer dividend)
{ 
  typedef Eigen::Array<J, Eigen::Dynamic, 1> JEigenArrayType;
  typedef Eigen::Map<JEigenArrayType> JEigenArrayMapType;

  typedef Eigen::Array<K, Eigen::Dynamic, 1> KEigenArrayType;
  typedef Eigen::Map<KEigenArrayType> KEigenArrayMapType;

  typename DataArray<J>::Pointer divisorCast = std::dynamic_pointer_cast<DataArray<J> >(divisor);
  typename DataArray<K>::Pointer dividendCast = std::dynamic_pointer_cast<DataArray<K> >(dividend);

  DataArray<double>::Pointer newArray;
  if (divisorCast->getNumberOfTuples() > 1 && dividendCast->getNumberOfTuples() == 1)
  {
    double dividendNum = static_cast<double>(dividendCast->getValue(0));
    JEigenArrayMapType ac(divisorCast->getPointer(0), divisorCast->getNumberOfTuples());

    newArray = DataArray<double>::CreateArray(divisorCast->getNumberOfTuples(), newArrayName);
    Eigen::Map<Eigen::Array<double, Eigen::Dynamic, 1> > newArrayMap(newArray->getPointer(0), newArray->getNumberOfTuples());

//    if (divisorNum == 0)
//    {
//      QString ss = QObject::tr("The chosen infix equation tried to divide by 0 (\"%1\", tuple %2).").arg(divisor->getName()).arg(QString::number(i));
//      filter->setErrorCondition(-4008);
//      filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
//      return QSharedPointer<CalculatorItem>();
//    }

    newArrayMap = dividendNum / ac. template cast<double>();
  }
  else if (divisorCast->getNumberOfTuples() == 1 && dividendCast->getNumberOfTuples() > 1)
  {
    double divisorNum = static_cast<double>(divisorCast->getValue(0));
    KEigenArrayMapType ac(dividendCast->getPointer(0), dividendCast->getNumberOfTuples());

    newArray = DataArray<double>::CreateArray(dividendCast->getNumberOfTuples(), newArrayName);
    Eigen::Map<Eigen::Array<double, Eigen::Dynamic, 1> > newArrayMap(newArray->getPointer(0), newArray->getNumberOfTuples());

//    if (divisorNum == 0)
//    {
//      QString ss = QObject::tr("The chosen infix equation tried to divide by 0.");
//      filter->setErrorCondition(-4008);
//      filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
//      return QSharedPointer<CalculatorItem>();
//    }

    newArrayMap = ac. template cast<double>() / divisorNum;
  }
  else if (divisorCast->getNumberOfTuples() > 0 && dividendCast->getNumberOfTuples() > 0)
  {
    JEigenArrayMapType divisorAC(divisorCast->getPointer(0), divisorCast->getNumberOfTuples());
    KEigenArrayMapType dividendAC(dividendCast->getPointer(0), dividendCast->getNumberOfTuples());

    newArray = DataArray<double>::CreateArray(dividendCast->getNumberOfTuples(), newArrayName);
    Eigen::Map<Eigen::Array<double, Eigen::Dynamic, 1> > newArrayMap(newArray->getPointer(0), newArray->getNumberOfTuples());

//    if (divisorNum == 0)
//    {
//      QString ss = QObject::tr("The chosen infix equation tried to divide by 0 (\"%1\", tuple %2).").arg(divisor->getName()).arg(QString::number(i));
//      filter->setErrorCondition(-4008);
//      filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
//      return QSharedPointer<CalculatorItem>();
//    }

    newArrayMap = dividendAC. template cast<double>() / divisorAC. template cast<double>();
  }
  else
  {
    Q_ASSERT(false);
  }

  QSharedPointer<CalculatorItem> newItem = QSharedPointer<CalculatorArray>(new CalculatorArray(newArray));
  return newItem;
}

