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



#include "ArrayCalculator.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/CalculatorFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"

#include "util/CalculatorArray.hpp"
#include "util/LeftParenthesisSeparator.h"
#include "util/RightParenthesisSeparator.h"
#include "util/AdditionOperator.h"
#include "util/SubtractionOperator.h"
#include "util/MultiplicationOperator.h"
#include "util/DivisionOperator.h"
#include "util/ABSOperator.h"
#include "util/SinOperator.h"
#include "util/CosOperator.h"
#include "util/TanOperator.h"
#include "util/SqrtOperator.h"
#include "util/Log10Operator.h"
#include "util/PowOperator.h"

// Include the MOC generated file for this class
#include "moc_ArrayCalculator.cpp"

#define CREATE_CALCULATOR_ARRAY(itemPtr, iDataArrayPtr)\
    if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(iDataArrayPtr))\
    {\
      FloatArrayType::Pointer arrayCast = std::dynamic_pointer_cast<FloatArrayType>(iDataArrayPtr);\
      itemPtr = QSharedPointer<CalculatorArray<float> >(new CalculatorArray<float>(arrayCast));\
    }\
    else if(TemplateHelpers::CanDynamicCast<DoubleArrayType>()(iDataArrayPtr))\
    {\
      DoubleArrayType::Pointer arrayCast = std::dynamic_pointer_cast<DoubleArrayType>(iDataArrayPtr);\
      itemPtr = QSharedPointer<CalculatorArray<double> >(new CalculatorArray<double>(arrayCast));\
    }\
    else if(TemplateHelpers::CanDynamicCast<Int8ArrayType>()(iDataArrayPtr))\
    {\
      Int8ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int8ArrayType>(iDataArrayPtr);\
      itemPtr = QSharedPointer<CalculatorArray<int8_t> >(new CalculatorArray<int8_t>(arrayCast));\
    }\
    else if(TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(iDataArrayPtr))\
    {\
      UInt8ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt8ArrayType>(iDataArrayPtr);\
      itemPtr = QSharedPointer<CalculatorArray<uint8_t> >(new CalculatorArray<uint8_t>(arrayCast));\
    }\
    else if(TemplateHelpers::CanDynamicCast<Int16ArrayType>()(iDataArrayPtr))\
    {\
      Int16ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int16ArrayType>(iDataArrayPtr);\
      itemPtr = QSharedPointer<CalculatorArray<int16_t> >(new CalculatorArray<int16_t>(arrayCast));\
    }\
    else if(TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(iDataArrayPtr))\
    {\
      UInt16ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt16ArrayType>(iDataArrayPtr);\
      itemPtr = QSharedPointer<CalculatorArray<uint16_t> >(new CalculatorArray<uint16_t>(arrayCast));\
    }\
    else if(TemplateHelpers::CanDynamicCast<Int32ArrayType>()(iDataArrayPtr))\
    {\
      Int32ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int32ArrayType>(iDataArrayPtr);\
      itemPtr = QSharedPointer<CalculatorArray<int32_t> >(new CalculatorArray<int32_t>(arrayCast));\
    }\
    else if(TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(iDataArrayPtr))\
    {\
      UInt32ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt32ArrayType>(iDataArrayPtr);\
      itemPtr = QSharedPointer<CalculatorArray<uint32_t> >(new CalculatorArray<uint32_t>(arrayCast));\
    }\
    else if(TemplateHelpers::CanDynamicCast<Int64ArrayType>()(iDataArrayPtr))\
    {\
      Int64ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int64ArrayType>(iDataArrayPtr);\
      itemPtr = QSharedPointer<CalculatorArray<int64_t> >(new CalculatorArray<int64_t>(arrayCast));\
    }\
    else if(TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(iDataArrayPtr))\
    {\
      UInt64ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt64ArrayType>(iDataArrayPtr);\
      itemPtr = QSharedPointer<CalculatorArray<uint64_t> >(new CalculatorArray<uint64_t>(arrayCast));\
    }\

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArrayCalculator::ArrayCalculator() :
  AbstractFilter()
{
  setupFilterParameters();

  // Insert all items into the symbol map to use during equation parsing
  m_SymbolMap.insert("(", QSharedPointer<LeftParenthesisSeparator>(new LeftParenthesisSeparator()));
  m_SymbolMap.insert(")", QSharedPointer<RightParenthesisSeparator>(new RightParenthesisSeparator()));
  m_SymbolMap.insert("+", QSharedPointer<AdditionOperator>(new AdditionOperator()));
  m_SymbolMap.insert("-", QSharedPointer<SubtractionOperator>(new SubtractionOperator()));
  m_SymbolMap.insert("*", QSharedPointer<MultiplicationOperator>(new MultiplicationOperator()));
  m_SymbolMap.insert("/", QSharedPointer<DivisionOperator>(new DivisionOperator()));
  m_SymbolMap.insert("abs", QSharedPointer<ABSOperator>(new ABSOperator()));
  m_SymbolMap.insert("sin", QSharedPointer<SinOperator>(new SinOperator()));
  m_SymbolMap.insert("cos", QSharedPointer<CosOperator>(new CosOperator()));
  m_SymbolMap.insert("tan", QSharedPointer<TanOperator>(new TanOperator()));
  m_SymbolMap.insert("sqrt", QSharedPointer<SqrtOperator>(new SqrtOperator()));
  m_SymbolMap.insert("log10", QSharedPointer<Log10Operator>(new Log10Operator()));
  m_SymbolMap.insert("^", QSharedPointer<PowOperator>(new PowOperator()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArrayCalculator::~ArrayCalculator()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyAttributeMatrix, SIMPL::Defaults::AnyGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Attribute Matrix", "SelectedAttributeMatrix", getSelectedAttributeMatrix(), FilterParameter::Parameter, req));
  }

  parameters.push_back(CalculatorFilterParameter::New("Infix Equation", "InfixEquation", getInfixEquation(), FilterParameter::Parameter));

  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(SIMPL::Defaults::AnyAttributeMatrix, SIMPL::Defaults::AnyGeometry);
    parameters.push_back(DataArrayCreationFilterParameter::New("Calculated Array", "CalculatedArray", getCalculatedArray(), FilterParameter::CreatedArray, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedAttributeMatrix(reader->readDataArrayPath("SelectedAttributeMatrix", getSelectedAttributeMatrix()));
  setInfixEquation(reader->readString("InfixEquation", getInfixEquation()));
  setCalculatedArray(reader->readDataArrayPath("CalculatedArray", getCalculatedArray()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ArrayCalculator::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(SelectedAttributeMatrix)
  SIMPL_FILTER_WRITE_PARAMETER(InfixEquation)
  SIMPL_FILTER_WRITE_PARAMETER(CalculatedArray)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::dataCheck()
{
  setErrorCondition(0);

  if (m_InfixEquation.isEmpty() == true)
  {
    QString ss = QObject::tr("The infix equation is empty.");
    setErrorCondition(-4011);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_CalculatedArray.isEmpty() == true)
  {
    QString ss = QObject::tr("A calculated array has not been chosen.");
    setErrorCondition(-4012);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  AttributeMatrix::Pointer selectedAM = getDataContainerArray()->getAttributeMatrix(m_SelectedAttributeMatrix);
  if (NULL == selectedAM)
  {
    QString ss = QObject::tr("Could not find the attribute matrix \"%1\".").arg(m_SelectedAttributeMatrix.getAttributeMatrixName());
    setErrorCondition(-4013);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataArrayPath calculatedAMPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");
  AttributeMatrix::Pointer calculatedAM = getDataContainerArray()->getAttributeMatrix(calculatedAMPath);
  if (NULL == calculatedAM)
  {
    QString ss = QObject::tr("Could not find the attribute matrix \"%1\".").arg(m_CalculatedArray.getAttributeMatrixName());
    setErrorCondition(-4014);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<QSharedPointer<CalculatorItem> > parsedInfix = parseInfixEquation(m_InfixEquation);
  if (parsedInfix.isEmpty() == true) { return; }

  bool hasArrayGreaterThan1 = false;
  bool hasArray = false;
  for (int i=0; i<parsedInfix.size(); i++)
  {
    QSharedPointer<CalculatorItem> currentItem = parsedInfix[i];
    if (NULL != qSharedPointerDynamicCast<CalculatorOperator>(currentItem))
    {
      bool result = qSharedPointerDynamicCast<CalculatorOperator>(currentItem)->checkValidity(parsedInfix, i);
      if (result == false)
      {
        QString ss = QObject::tr("The chosen infix equation is not a valid equation.");
        setErrorCondition(-4018);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
    else if (NULL != qSharedPointerDynamicCast<ICalculatorArray>(currentItem))
    {
      hasArray = true;
      if (qSharedPointerDynamicCast<ICalculatorArray>(currentItem)->getArray()->getNumberOfTuples() > 1)
      {
        hasArrayGreaterThan1 = true;
      }
    }
  }

  if (hasArray == true)
  {
    if (hasArrayGreaterThan1 == false)
    {
      QString ss = QObject::tr("The result of the chosen equation will be a numeric value, not an array."
        "This numeric value will be stored in an array with the number of tuples equal to 1.").arg(m_SelectedAttributeMatrix.getAttributeMatrixName());
      notifyWarningMessage(getHumanLabel(), ss, -4015);

      if (calculatedAM->getNumTuples() != 1)
      {
        QString ss = QObject::tr("The tuple count of the calculated attribute matrix is not equal to 1.").arg(m_SelectedAttributeMatrix.getAttributeMatrixName());
        setErrorCondition(-4016);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
    else if (calculatedAM->getNumTuples() != selectedAM->getNumTuples())
    {
      QString ss = QObject::tr("The tuple count of the calculated attribute matrix is not equal to the tuple count of the selected attribute matrix.").arg(m_SelectedAttributeMatrix.getAttributeMatrixName());
      setErrorCondition(-4017);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
  else
  {
    QString ss = QObject::tr("The chosen infix equation does not have any numeric values or arrays in it.");
    setErrorCondition(-4019);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<QSharedPointer<CalculatorItem> > rpn = toRPN(parsedInfix);
  if (rpn.isEmpty() == true) { return; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataArrayPath calculatedAMPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");
  AttributeMatrix::Pointer calculatedAM = getDataContainerArray()->getAttributeMatrix(calculatedAMPath);

  // Parse the infix equation from the user interface
  QVector<QSharedPointer<CalculatorItem> > parsedInfix = parseInfixEquation(m_InfixEquation);

  // Convert the parsed infix equation into RPN
  QVector<QSharedPointer<CalculatorItem> > rpn = toRPN(parsedInfix);

  // Execute the RPN equation
  for (int i = 0; i < rpn.size(); i++)
  {
    QSharedPointer<CalculatorItem> rpnItem = rpn[i];
    if (NULL != qSharedPointerDynamicCast<ICalculatorArray>(rpnItem))
    {
      m_ExecutionStack.push(rpnItem);
    }
    else if (NULL != qSharedPointerDynamicCast<CalculatorOperator>(rpnItem))
    {
      QSharedPointer<CalculatorOperator> rpnOperator = qSharedPointerDynamicCast<CalculatorOperator>(rpnItem);
      DoubleArrayType::Pointer newArray = DoubleArrayType::CreateArray(calculatedAM->getNumTuples(), QVector<size_t>(1, 1), m_CalculatedArray.getDataArrayName());

      for (int i=0; i<newArray->getNumberOfTuples(); i++)
      {
        double value = rpnOperator->calculate(this, m_CalculatedArray.getDataArrayName(), m_ExecutionStack, i);

        if (getErrorCondition() < 0)
        {
          return;
        }

        newArray->setValue(i, value);
      }

      m_ExecutionStack.pop();
      if (rpnOperator->getOperatorType() == CalculatorOperator::Binary)
      {
        m_ExecutionStack.pop();
      }

      m_ExecutionStack.push(QSharedPointer<CalculatorArray<double> >(new CalculatorArray<double>(newArray)));
    }
    else
    {
      // ERROR: Unrecognized item in the RPN vector
    }

    if (getCancel() == true) { return; }
  }

  // Grab the result from the stack
  QSharedPointer<CalculatorItem> resultItem = m_ExecutionStack.pop();

  IDataArray::Pointer newArray = IDataArray::NullPointer();
  if (NULL != qSharedPointerDynamicCast<ICalculatorArray>(resultItem))
  {
    QSharedPointer<ICalculatorArray> arrayItem = qSharedPointerDynamicCast<ICalculatorArray>(resultItem);
    newArray = arrayItem->getArray();

    DataArrayPath createdAMPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");
    AttributeMatrix::Pointer createdAM = getDataContainerArray()->getAttributeMatrix(createdAMPath);
    if (NULL != createdAM)
    {
      createdAM->addAttributeArray(m_CalculatedArray.getDataArrayName(), newArray);
    }
  }
  else
  {
    QString ss = QObject::tr("Unexpected output item from chosen infix equation.  The output item must be an array."
                             "Please contact the DREAM3D developers for more information.");
    setErrorCondition(-4009);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QSharedPointer<CalculatorItem> > ArrayCalculator::parseInfixEquation(QString equation)
{
  if (equation.isEmpty())
  {
    return QVector<QSharedPointer<CalculatorItem> >();
  }

  int err = 0;
  AttributeMatrix::Pointer selectedAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, m_SelectedAttributeMatrix, err);
  if (NULL == selectedAM)
  {
    QString ss = QObject::tr("Could not find the attribute matrix \"%1\".").arg(m_SelectedAttributeMatrix.getAttributeMatrixName());
    setErrorCondition(-4001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return QVector<QSharedPointer<CalculatorItem> >();
  }

  // Remove spaces and place @ symbols around each item in the equation
  for (QMap<QString, QSharedPointer<CalculatorItem> >::iterator iter = m_SymbolMap.begin(); iter != m_SymbolMap.end(); iter++)
  {
    QString symbolStr = iter.key();

    equation.replace(symbolStr, "@" + symbolStr + "@");
  }

  QStringList aaNames = selectedAM->getAttributeArrayNames();
  for (int i = 0; i < aaNames.size(); i++)
  {
    QString aaName = aaNames[i];
    equation.replace(aaName, "@[" + QString::number(i) + "]@");
  }

  equation.remove(" ");

  for (int i = 0; i < aaNames.size(); i++)
  {
    QString aaName = aaNames[i];
    equation.replace("[" + QString::number(i) + "]", aaName);
  }
  
  // Now split the equation up into a QStringList of items
  QStringList list = equation.split("@", QString::SkipEmptyParts);

  // Iterate through the QStringList and create the proper CalculatorItems
  QVector<QSharedPointer<CalculatorItem> > parsedInfix;
  int numTuples = -1;
  QString firstArray = "";
  for (int i = 0; i < list.size(); i++)
  {
    QString listItem = list[i];
    QSharedPointer<CalculatorItem> itemPtr;

    bool ok;
    double num = listItem.toDouble(&ok);
    if (ok == true)
    {
      // This is a number, so create an array with numOfTuples equal to 1 and set the value into it
      DoubleArrayType::Pointer ptr = DoubleArrayType::CreateArray(1, QVector<size_t>(1, 1), "NumberArray");
      ptr->setValue(0, num);
      itemPtr = QSharedPointer<CalculatorArray<double> >(new CalculatorArray<double>(ptr));
      parsedInfix.push_back(itemPtr);
    }
    else if (listItem == "-")
    {
      // This could be either a negative sign or subtraction sign, so we need to figure out which one it is
      if (
           i == 0 ||
           (
             (
               (NULL != qSharedPointerDynamicCast<CalculatorOperator>(parsedInfix.back()) && qSharedPointerDynamicCast<CalculatorOperator>(parsedInfix.back())->getOperatorType() == CalculatorOperator::Binary)
               || NULL != qSharedPointerDynamicCast<LeftParenthesisSeparator>(parsedInfix.back())
             )
             && NULL == qSharedPointerDynamicCast<RightParenthesisSeparator>(parsedInfix.back())
           )
         )
      {
        // By context, this is a negative sign, so we need to insert a -1 array and a multiplication operator
        DoubleArrayType::Pointer ptr = DoubleArrayType::CreateArray(1, QVector<size_t>(1, 1), "NumberArray");
        ptr->setValue(0, -1);
        itemPtr = QSharedPointer<CalculatorArray<double> >(new CalculatorArray<double>(ptr));
        parsedInfix.push_back(itemPtr);

        itemPtr = QSharedPointer<MultiplicationOperator>(new MultiplicationOperator());
        parsedInfix.push_back(itemPtr);
      }
      else
      {
        // By context, this is a subtraction sign
        itemPtr = m_SymbolMap.value(listItem);
        parsedInfix.push_back(itemPtr);
      }
    }
    else if (selectedAM->getAttributeArrayNames().contains(listItem))
    {
      // This is an array, so create the array item
      IDataArray::Pointer dataArray = selectedAM->getAttributeArray(listItem);
      if (numTuples < 0 && firstArray.isEmpty() == true)
      {
        numTuples = dataArray->getNumberOfTuples();
        firstArray = dataArray->getName();
      }
      else if (dataArray->getNumberOfTuples() != numTuples)
      {
        QString ss = QObject::tr("Arrays \"%1\" and \"%2\" in the infix equation have an inconsistent number of tuples.").arg(firstArray).arg(dataArray->getName());
        setErrorCondition(-4007);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return QVector<QSharedPointer<CalculatorItem> >();
      }

      CREATE_CALCULATOR_ARRAY(itemPtr, dataArray)
      parsedInfix.push_back(itemPtr);
    }
    else
    {
      itemPtr = m_SymbolMap.value(listItem);
      if (itemPtr.isNull())
      {
        QString ss = QObject::tr("An unrecognized item \"%1\" was found in the chosen infix equation.").arg(listItem);
        setErrorCondition(-4002);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return QVector<QSharedPointer<CalculatorItem> >();
      }
      else
      {
        parsedInfix.push_back(itemPtr);
      }
    }
  }

  // Return the parsed infix equation as a vector of CalculatorItems
  return parsedInfix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QSharedPointer<CalculatorItem> > ArrayCalculator::toRPN(QVector<QSharedPointer<CalculatorItem> > infixEquation)
{
  QStack<QSharedPointer<CalculatorItem> > itemStack;
  QVector<QSharedPointer<CalculatorItem> > rpnEquation;

  // Iterate through the infix equation items
  for (int i = 0; i < infixEquation.size(); i++)
  {
    QSharedPointer<CalculatorItem> calcItem = infixEquation[i];
    if (NULL != qSharedPointerDynamicCast<ICalculatorArray>(calcItem))
    {
      // This is a number or array, so push it onto the rpn equation output
      rpnEquation.push_back(calcItem);
    }
    else if (NULL != qSharedPointerDynamicCast<LeftParenthesisSeparator>(calcItem))
    {
      // This is a left parenthesis, so push it onto the item stack
      itemStack.push_back(calcItem);
    }
    else if (NULL != qSharedPointerDynamicCast<RightParenthesisSeparator>(calcItem))
    {
      // This is a right parenthesis, so push operators from the item stack onto the rpn equation output until we get to the left parenthesis
      while (itemStack.isEmpty() == false && NULL == qSharedPointerDynamicCast<LeftParenthesisSeparator>(itemStack.top()))
      {
        rpnEquation.push_back(itemStack.pop());
      }

      if (itemStack.isEmpty() == true)
      {
        QString ss = QObject::tr("One or more parentheses are mismatched in the chosen infix equation \"%1\".").arg(m_InfixEquation);
        setErrorCondition(-4003);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return QVector<QSharedPointer<CalculatorItem> >();
      }

      // Discard the left parenthesis that we found
      itemStack.pop();
    }
    else if (NULL != qSharedPointerDynamicCast<CalculatorOperator>(calcItem))
    {
      // This is an operator
      QSharedPointer<CalculatorOperator> incomingOperator = qSharedPointerDynamicCast<CalculatorOperator>(calcItem);
      if (itemStack.isEmpty() == false)
      {
        /* If the operator's precedence is lower than the precedence of the operator on top of the item stack, push the operator at the top
           of the item stack onto the rpn equation output.  Keeping doing this until there isn't another operator at the top of the item
           stack or the operator has a higher precedence than the one currently on top of the stack */
        QSharedPointer<CalculatorOperator> topOperator = qSharedPointerDynamicCast<CalculatorOperator>(itemStack.top());
        while (NULL != topOperator && incomingOperator->hasHigherPrecedence(topOperator) == false)
        {
          rpnEquation.push_back(itemStack.pop());
          if (itemStack.isEmpty() == false)
          {
            topOperator = qSharedPointerDynamicCast<CalculatorOperator>(itemStack.top());
          }
          else
          {
            topOperator.clear();
          }
        }
      }

      // Push the operator onto the rpn equation output.
      itemStack.push_back(calcItem);
    }
    else
    {
      QString ss = QObject::tr("An unrecognized character was found in the chosen infix equation \"%1\".").arg(m_InfixEquation);
      setErrorCondition(-4004);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return QVector<QSharedPointer<CalculatorItem> >();
    }
  }

  /* After we are done iterating through the infix equation items, keep transferring items from the item stack to the 
     rpn equation output until the stack is empty. */
  while (itemStack.isEmpty() == false)
  {
    QSharedPointer<CalculatorItem> item = itemStack.pop();
    if (NULL != qSharedPointerDynamicCast<LeftParenthesisSeparator>(item))
    {
      QString ss = QObject::tr("One or more parentheses are mismatched in the chosen infix equation \"%1\".").arg(m_InfixEquation);
      setErrorCondition(-4010);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return QVector<QSharedPointer<CalculatorItem> >();
    }

    rpnEquation.push_back(item);
  }

  return rpnEquation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ArrayCalculator::newFilterInstance(bool copyFilterParameters)
{
  ArrayCalculator::Pointer filter = ArrayCalculator::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getGroupName()
{ return SIMPL::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getHumanLabel()
{ return "Array Calculator"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getSubGroupName()
{ return SIMPL::FilterSubGroups::GenerationFilters; }

