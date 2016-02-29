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

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigen>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SubtractionOperator::SubtractionOperator() :
CalculatorOperator()
{
  setPrecedenceId(0);
  setOperatorType(Binary);
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
    IDataArray::Pointer item1 = qSharedPointerDynamicCast<CalculatorArray>(executionStack.pop())->getArray();
    IDataArray::Pointer item2 = qSharedPointerDynamicCast<CalculatorArray>(executionStack.pop())->getArray();

    EXECUTE_FUNCTION_TWO_ARRAYS(filter, newArrayName, item1, item2, subtract)
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
QSharedPointer<CalculatorItem> SubtractionOperator::subtract(AbstractFilter* filter, const QString &newArrayName, IDataArray::Pointer subtrahend, IDataArray::Pointer minuend)
{ 
  typedef Eigen::Array<J, Eigen::Dynamic, 1> JEigenArrayType;
  typedef Eigen::Map<JEigenArrayType> JEigenArrayMapType;

  typedef Eigen::Array<K, Eigen::Dynamic, 1> KEigenArrayType;
  typedef Eigen::Map<KEigenArrayType> KEigenArrayMapType;

  typename DataArray<J>::Pointer subtrahendCast = std::dynamic_pointer_cast<DataArray<J> >(subtrahend);
  typename DataArray<K>::Pointer minuendCast = std::dynamic_pointer_cast<DataArray<K> >(minuend);

  DataArray<double>::Pointer newArray;
  if (subtrahendCast->getNumberOfTuples() > 1 && minuendCast->getNumberOfTuples() == 1)
  {
    double minuendNum = static_cast<double>(minuendCast->getValue(0));
    JEigenArrayMapType ac(subtrahendCast->getPointer(0), subtrahendCast->getNumberOfTuples());

    newArray = DataArray<double>::CreateArray(subtrahendCast->getNumberOfTuples(), newArrayName);
    Eigen::Map<Eigen::Array<double, Eigen::Dynamic, 1> > newArrayMap(newArray->getPointer(0), newArray->getNumberOfTuples());

    newArrayMap = minuendNum - ac. template cast<double>();
  }
  else if (subtrahendCast->getNumberOfTuples() == 1 && minuendCast->getNumberOfTuples() > 1)
  {
    double subtrahendNum = static_cast<double>(subtrahendCast->getValue(0));
    KEigenArrayMapType ac(minuendCast->getPointer(0), minuendCast->getNumberOfTuples());

    newArray = DataArray<double>::CreateArray(minuendCast->getNumberOfTuples(), newArrayName);
    Eigen::Map<Eigen::Array<double, Eigen::Dynamic, 1> > newArrayMap(newArray->getPointer(0), newArray->getNumberOfTuples());

    newArrayMap = ac. template cast<double>() - subtrahendNum;
  }
  else if (subtrahendCast->getNumberOfTuples() > 0 && minuendCast->getNumberOfTuples() > 0)
  {
    JEigenArrayMapType subtrahendAC(subtrahendCast->getPointer(0), subtrahendCast->getNumberOfTuples());
    KEigenArrayMapType minuendAC(minuendCast->getPointer(0), minuendCast->getNumberOfTuples());

    newArray = DataArray<double>::CreateArray(minuendCast->getNumberOfTuples(), newArrayName);
    Eigen::Map<Eigen::Array<double, Eigen::Dynamic, 1> > newArrayMap(newArray->getPointer(0), newArray->getNumberOfTuples());

    newArrayMap = minuendAC. template cast<double>() - subtrahendAC. template cast<double>();
  }
  else
  {
    Q_ASSERT(false);
  }

  QSharedPointer<CalculatorItem> newItem = QSharedPointer<CalculatorArray>(new CalculatorArray(newArray));
  return newItem;
}

