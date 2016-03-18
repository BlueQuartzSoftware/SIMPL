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

#include <QtCore/QMapIterator>
#include <QtCore/QRegularExpression>
#include <QtCore/QVectorIterator>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/CalculatorFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"

#include "util/CalculatorArray.hpp"
#include "util/LeftParenthesisItem.h"
#include "util/RightParenthesisItem.h"
#include "util/CommaSeparator.h"
#include "util/AdditionOperator.h"
#include "util/SubtractionOperator.h"
#include "util/MultiplicationOperator.h"
#include "util/DivisionOperator.h"
#include "util/NegativeOperator.h"
#include "util/ABSOperator.h"
#include "util/SinOperator.h"
#include "util/CosOperator.h"
#include "util/TanOperator.h"
#include "util/ASinOperator.h"
#include "util/ACosOperator.h"
#include "util/ATanOperator.h"
#include "util/SqrtOperator.h"
#include "util/Log10Operator.h"
#include "util/LogOperator.h"
#include "util/PowOperator.h"
#include "util/RootOperator.h"
#include "util/ExpOperator.h"
#include "util/LnOperator.h"
#include "util/FloorOperator.h"
#include "util/CeilOperator.h"

// Include the MOC generated file for this class
#include "moc_ArrayCalculator.cpp"

#define CREATE_CALCULATOR_ARRAY(itemPtr, iDataArrayPtr)\
    if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(iDataArrayPtr))\
    {\
      FloatArrayType::Pointer arrayCast = std::dynamic_pointer_cast<FloatArrayType>(iDataArrayPtr);\
      itemPtr = CalculatorArray<float>::New(arrayCast, !getInPreflight());\
    }\
    else if(TemplateHelpers::CanDynamicCast<DoubleArrayType>()(iDataArrayPtr))\
    {\
      DoubleArrayType::Pointer arrayCast = std::dynamic_pointer_cast<DoubleArrayType>(iDataArrayPtr);\
      itemPtr = CalculatorArray<double>::New(arrayCast, !getInPreflight());\
    }\
    else if(TemplateHelpers::CanDynamicCast<Int8ArrayType>()(iDataArrayPtr))\
    {\
      Int8ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int8ArrayType>(iDataArrayPtr);\
      itemPtr = CalculatorArray<int8_t>::New(arrayCast, !getInPreflight());\
    }\
    else if(TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(iDataArrayPtr))\
    {\
      UInt8ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt8ArrayType>(iDataArrayPtr);\
      itemPtr = CalculatorArray<uint8_t>::New(arrayCast, !getInPreflight());\
    }\
    else if(TemplateHelpers::CanDynamicCast<Int16ArrayType>()(iDataArrayPtr))\
    {\
      Int16ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int16ArrayType>(iDataArrayPtr);\
      itemPtr = CalculatorArray<int16_t>::New(arrayCast, !getInPreflight());\
    }\
    else if(TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(iDataArrayPtr))\
    {\
      UInt16ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt16ArrayType>(iDataArrayPtr);\
      itemPtr = CalculatorArray<uint16_t>::New(arrayCast, !getInPreflight());\
    }\
    else if(TemplateHelpers::CanDynamicCast<Int32ArrayType>()(iDataArrayPtr))\
    {\
      Int32ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int32ArrayType>(iDataArrayPtr);\
      itemPtr = CalculatorArray<int32_t>::New(arrayCast, !getInPreflight());\
    }\
    else if(TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(iDataArrayPtr))\
    {\
      UInt32ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt32ArrayType>(iDataArrayPtr);\
      itemPtr = CalculatorArray<uint32_t>::New(arrayCast, !getInPreflight());\
    }\
    else if(TemplateHelpers::CanDynamicCast<Int64ArrayType>()(iDataArrayPtr))\
    {\
      Int64ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int64ArrayType>(iDataArrayPtr);\
      itemPtr = CalculatorArray<int64_t>::New(arrayCast, !getInPreflight());\
    }\
    else if(TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(iDataArrayPtr))\
    {\
      UInt64ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt64ArrayType>(iDataArrayPtr);\
      itemPtr = CalculatorArray<uint64_t>::New(arrayCast, !getInPreflight());\
    }\

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArrayCalculator::ArrayCalculator() :
  AbstractFilter(),
  m_Units(Radians)
{
  setupFilterParameters();

  // Insert all items into the symbol map to use during equation parsing
  m_SymbolMap.insert("(", LeftParenthesisItem::New());
  m_SymbolMap.insert(")", RightParenthesisItem::New());
  m_SymbolMap.insert(",", CommaSeparator::New());
  m_SymbolMap.insert("+", AdditionOperator::New());
  m_SymbolMap.insert("-", SubtractionOperator::New());
  m_SymbolMap.insert("*", MultiplicationOperator::New());
  m_SymbolMap.insert("/", DivisionOperator::New());
  m_SymbolMap.insert("^", PowOperator::New());
  m_SymbolMap.insert("abs", ABSOperator::New());
  m_SymbolMap.insert("sin", SinOperator::New());
  m_SymbolMap.insert("cos", CosOperator::New());
  m_SymbolMap.insert("tan", TanOperator::New());
  m_SymbolMap.insert("asin", ASinOperator::New());
  m_SymbolMap.insert("acos", ACosOperator::New());
  m_SymbolMap.insert("atan", ATanOperator::New());
  m_SymbolMap.insert("sqrt", SqrtOperator::New());
  m_SymbolMap.insert("root", RootOperator::New());
  m_SymbolMap.insert("log10", Log10Operator::New());
  m_SymbolMap.insert("log", LogOperator::New());
  m_SymbolMap.insert("exp", ExpOperator::New());
  m_SymbolMap.insert("ln", LnOperator::New());
  m_SymbolMap.insert("floor", FloorOperator::New());
  m_SymbolMap.insert("ceil", CeilOperator::New());
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
  setUnits(static_cast<ArrayCalculator::AngleUnits>(reader->readValue("Units", static_cast<int>(getUnits()))));
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
  SIMPL_FILTER_WRITE_PARAMETER(Units)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::dataCheck()
{
  int err = 0;
  setErrorCondition(0);
  setWarningCondition(0);

  getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, QVector<size_t>(1, 1));
  if (getErrorCondition() < 0)
  {
    return;
  }

  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedAttributeMatrix, err);
  if (getErrorCondition() < 0)
  {
    return;
  }

  if (m_InfixEquation.isEmpty() == true || m_InfixEquation.split(" ", QString::SkipEmptyParts).size() <= 0)
  {
    QString ss = QObject::tr("The infix equation is empty.");
    setErrorCondition(EMPTY_EQUATION);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<CalculatorItem::Pointer> parsedInfix = parseInfixEquation(m_InfixEquation);
  if (parsedInfix.isEmpty() == true) { return; }

  bool hasArrayGreaterThan1 = false;
  bool hasValue = false;
  for (int i=0; i<parsedInfix.size(); i++)
  {
    CalculatorItem::Pointer currentItem = parsedInfix[i];
    if (NULL != std::dynamic_pointer_cast<CalculatorOperator>(currentItem))
    {
      bool result = std::dynamic_pointer_cast<CalculatorOperator>(currentItem)->checkValidity(parsedInfix, i);
      if (result == false)
      {
        QString ss = QObject::tr("The chosen infix equation is not a valid equation.");
        setErrorCondition(INVALID_EQUATION);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
    else if (NULL != std::dynamic_pointer_cast<ICalculatorArray>(currentItem))
    {
      hasValue = true;
      if (std::dynamic_pointer_cast<ICalculatorArray>(currentItem)->getArray()->getNumberOfTuples() > 1)
      {
        hasArrayGreaterThan1 = true;
      }
    }
  }

  if (hasValue == false)
  {
    QString ss = QObject::tr("The chosen infix equation is not a valid equation.");
    setErrorCondition(INVALID_EQUATION);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataArrayPath calculatedAMPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");
  AttributeMatrix::Pointer calculatedAM = getDataContainerArray()->getAttributeMatrix(calculatedAMPath);

  if (hasArrayGreaterThan1 == false)
  {
    QString ss = QObject::tr("The result of the chosen equation will be a numeric value, not an array."
                             "This numeric value will be stored in an array with the number of tuples equal to 1.");
    setWarningCondition(NUMERIC_VALUE_WARNING);
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());

    if (calculatedAM->getNumTuples() != 1)
    {
      QString ss = QObject::tr("The tuple count of the calculated attribute matrix is not equal to 1.");
      setErrorCondition(INCORRECT_TUPLE_COUNT);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
  else
  {
    AttributeMatrix::Pointer selectedAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, m_SelectedAttributeMatrix, err);
    if (NULL != selectedAM && calculatedAM->getNumTuples() != selectedAM->getNumTuples())
    {
      QString ss = QObject::tr("The tuple count of the calculated attribute matrix is not equal to the tuple count of the selected attribute matrix.");
      setErrorCondition(INCORRECT_TUPLE_COUNT);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }

  QVector<CalculatorItem::Pointer> rpn = toRPN(parsedInfix);
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
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataArrayPath calculatedAMPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");
  AttributeMatrix::Pointer calculatedAM = getDataContainerArray()->getAttributeMatrix(calculatedAMPath);

  // Parse the infix equation from the user interface
  QVector<CalculatorItem::Pointer> parsedInfix = parseInfixEquation(m_InfixEquation);

  // Convert the parsed infix equation into RPN
  QVector<CalculatorItem::Pointer> rpn = toRPN(parsedInfix);

  // Execute the RPN equation
  int totalItems = rpn.size();
  for (int rpnCount = 0; rpnCount < totalItems; rpnCount++)
  {
    notifyStatusMessage(getHumanLabel(), QString::number(rpnCount + 1) + "/" + QString::number(totalItems) + ": " + QString::number(0) + "%");

    CalculatorItem::Pointer rpnItem = rpn[rpnCount];
    ICalculatorArray::Pointer calcArray = std::dynamic_pointer_cast<ICalculatorArray>(rpnItem);
    if (NULL != calcArray)
    {
      // This is an array
      m_ExecutionStack.push(calcArray);
    }
    else
    {
      // This is an operator
      CalculatorOperator::Pointer rpnOperator = std::dynamic_pointer_cast<CalculatorOperator>(rpnItem);
      DoubleArrayType::Pointer newArray = DoubleArrayType::CreateArray(calculatedAM->getNumTuples(), QVector<size_t>(1, 1), m_CalculatedArray.getDataArrayName());

      int tuplePercentNum = newArray->getNumberOfTuples() / 100;
      int percent = 0;
      int tupleThresh = tuplePercentNum;
      for (int i=0; i<newArray->getNumberOfTuples(); i++)
      {
        double value = rpnOperator->calculate(this, m_CalculatedArray.getDataArrayName(), m_ExecutionStack, i);

        if (getErrorCondition() < 0)
        {
          return;
        }

        newArray->setValue(i, value);

        if (i == tupleThresh)
        {
          percent++;
          QString msg = QString::number(rpnCount + 1) + "/" + QString::number(totalItems) + ": " + QString::number(percent) + "%";
          notifyStatusMessage(getHumanLabel(), msg);
          tupleThresh = i + tuplePercentNum;
        }
      }

      m_ExecutionStack.pop();
      if (rpnOperator->getOperatorType() == CalculatorOperator::Binary)
      {
        m_ExecutionStack.pop();
      }

      m_ExecutionStack.push(CalculatorArray<double>::New(newArray, true));
    }

    if (getCancel() == true) { return; }
  }

  // Grab the result from the stack
  ICalculatorArray::Pointer arrayItem = m_ExecutionStack.pop();

  IDataArray::Pointer resultArray = IDataArray::NullPointer();
  if (NULL != arrayItem)
  {
    resultArray = arrayItem->getArray();

    DataArrayPath createdAMPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");
    AttributeMatrix::Pointer createdAM = getDataContainerArray()->getAttributeMatrix(createdAMPath);
    if (NULL != createdAM)
    {
      resultArray->setName(m_CalculatedArray.getDataArrayName());
      createdAM->addAttributeArray(resultArray->getName(), resultArray);
    }
  }
  else
  {
    QString ss = QObject::tr("Unexpected output item from chosen infix equation.  The output item must be an array."
                             "Please contact the DREAM3D developers for more information.");
    setErrorCondition(UNEXPECTED_OUTPUT);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<CalculatorItem::Pointer> ArrayCalculator::parseInfixEquation(QString equation)
{
  int err = 0;

  AttributeMatrix::Pointer selectedAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedAttributeMatrix, err);

  // Parse all the items into a QVector of strings using a regular expression
  QVector<QString> itemList;
  QRegularExpression regExp("\\d+(\\.\\d+)?|\\.\\d+|\\w{2,}\\d{0,2}|\\+|\\-|\\*|\\/|\\(|\\)|\\,|\\^");
  QRegularExpressionMatchIterator iter = regExp.globalMatch(m_InfixEquation);
  while (iter.hasNext())
  {
    QRegularExpressionMatch match = iter.next();
    itemList.push_back(match.captured());
  }

  // Iterate through the QStringList and create the proper CalculatorItems
  QVector<CalculatorItem::Pointer> parsedInfix;
  int numTuples = -1;
  QString firstArray = "";
  for (int i = 0; i < itemList.size(); i++)
  {
    QString strItem = itemList[i];
    CalculatorItem::Pointer itemPtr;

    bool ok;
    double num = strItem.toDouble(&ok);
    if (ok == true)
    {
      // This is a number, so create an array with numOfTuples equal to 1 and set the value into it
      DoubleArrayType::Pointer ptr = DoubleArrayType::CreateArray(1, QVector<size_t>(1, 1), m_CalculatedArray.getDataArrayName());
      ptr->setValue(0, num);
      itemPtr = CalculatorArray<double>::New(ptr, !getInPreflight());
      parsedInfix.push_back(itemPtr);
    }
    else if (strItem == "-")
    {
      // This could be either a negative sign or subtraction sign, so we need to figure out which one it is
      if (
           i == 0 ||
           (
             (
               (NULL != std::dynamic_pointer_cast<CalculatorOperator>(parsedInfix.back()) && std::dynamic_pointer_cast<CalculatorOperator>(parsedInfix.back())->getOperatorType() == CalculatorOperator::Binary)
               || NULL != std::dynamic_pointer_cast<LeftParenthesisItem>(parsedInfix.back())
             )
             && NULL == std::dynamic_pointer_cast<RightParenthesisItem>(parsedInfix.back())
           )
         )
      {
        // By context, this is a negative sign
        itemPtr = NegativeOperator::New();
        parsedInfix.push_back(itemPtr);
      }
      else
      {
        // By context, this is a subtraction sign
        itemPtr = m_SymbolMap.value(strItem);
        parsedInfix.push_back(itemPtr);
      }
    }
    else if (selectedAM->getAttributeArrayNames().contains(strItem))
    {
      // This is an array, so create the array item
      IDataArray::Pointer dataArray = selectedAM->getAttributeArray(strItem);
      if (numTuples < 0 && firstArray.isEmpty() == true)
      {
        numTuples = dataArray->getNumberOfTuples();
        firstArray = dataArray->getName();
      }
      else if (dataArray->getNumberOfTuples() != numTuples)
      {
        QString ss = QObject::tr("Arrays \"%1\" and \"%2\" in the infix equation have an inconsistent number of tuples.").arg(firstArray).arg(dataArray->getName());
        setErrorCondition(INCONSISTENT_TUPLES);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return QVector<CalculatorItem::Pointer>();
      }

      CREATE_CALCULATOR_ARRAY(itemPtr, dataArray)
        parsedInfix.push_back(itemPtr);
    }
    else
    {
      itemPtr = m_SymbolMap.value(strItem);
      if (NULL == itemPtr)
      {
        QString ss = QObject::tr("An unrecognized item \"%1\" was found in the chosen infix equation.").arg(strItem);
        setErrorCondition(UNRECOGNIZED_ITEM);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return QVector<CalculatorItem::Pointer>();
      }
      else if (NULL != std::dynamic_pointer_cast<CommaSeparator>(itemPtr))
      {
        // This is a comma, so make sure that this comma has a valid unary operator before it
        {
          QVectorIterator<CalculatorItem::Pointer> iter(parsedInfix);
          iter.toBack();
          bool foundUnaryOperator = false;
          while (iter.hasPrevious())
          {
            CalculatorItem::Pointer item = iter.previous();
            if (NULL != std::dynamic_pointer_cast<UnaryOperator>(item))
            {
              foundUnaryOperator = true;
            }
          }

          if (foundUnaryOperator == false)
          {
            QString ss = QObject::tr("The chosen infix equation is not a valid equation.");
            setErrorCondition(INVALID_EQUATION);
            notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
            return QVector<CalculatorItem::Pointer>();
          }
        }

        // This is a comma, so we need to put parentheses around the entire term so that the RPN parser knows to evaluate the entire expression placed here
        // For example, if we have root( 4*4, 2*3 ), then we need it to be root( (4*4), (2*3) )
        parsedInfix.push_back(RightParenthesisItem::New());

        QVector<CalculatorItem::Pointer>::iterator iter = parsedInfix.end();
        iter--;
        while (iter != parsedInfix.begin())
        {
          if (NULL != std::dynamic_pointer_cast<CommaSeparator>(*iter)
            || NULL != std::dynamic_pointer_cast<LeftParenthesisItem>(*iter))
          {
            iter++;
            parsedInfix.insert(iter, LeftParenthesisItem::New());
            break;
          }

          iter--;
        }
      }

      parsedInfix.push_back(itemPtr);
    }
  }

  // Return the parsed infix equation as a vector of CalculatorItems
  return parsedInfix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<CalculatorItem::Pointer> ArrayCalculator::toRPN(QVector<CalculatorItem::Pointer> infixEquation)
{
  QStack<CalculatorItem::Pointer> itemStack;
  QVector<CalculatorItem::Pointer> rpnEquation;

  // Iterate through the infix equation items
  for (int i = 0; i < infixEquation.size(); i++)
  {
    CalculatorItem::Pointer calcItem = infixEquation[i];
    if (NULL != std::dynamic_pointer_cast<ICalculatorArray>(calcItem))
    {
      // This is a number or array, so push it onto the rpn equation output
      rpnEquation.push_back(calcItem);
    }
    else if (NULL != std::dynamic_pointer_cast<LeftParenthesisItem>(calcItem))
    {
      // This is a left parenthesis, so push it onto the item stack
      itemStack.push_back(calcItem);
    }
    else if (NULL != std::dynamic_pointer_cast<RightParenthesisItem>(calcItem))
    {
      // This is a right parenthesis, so push operators from the item stack onto the rpn equation output until we get to the left parenthesis
      while (itemStack.isEmpty() == false && NULL == std::dynamic_pointer_cast<LeftParenthesisItem>(itemStack.top()))
      {
        rpnEquation.push_back(itemStack.pop());
      }

      if (itemStack.isEmpty() == true)
      {
        QString ss = QObject::tr("One or more parentheses are mismatched in the chosen infix equation \"%1\".").arg(m_InfixEquation);
        setErrorCondition(MISMATCHED_PARENTHESES);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return QVector<CalculatorItem::Pointer>();
      }

      // Discard the left parenthesis that we found
      itemStack.pop();
    }
    else if (NULL != std::dynamic_pointer_cast<CalculatorSeparator>(calcItem))
    {
      // This is a comma, so we want to continue without adding it to anything
      continue;
    }
    else
    {
      // This is an operator
      CalculatorOperator::Pointer incomingOperator = std::dynamic_pointer_cast<CalculatorOperator>(calcItem);
      if (itemStack.isEmpty() == false)
      {
        /* If the operator's precedence is lower than the precedence of the operator on top of the item stack, push the operator at the top
           of the item stack onto the rpn equation output.  Keeping doing this until there isn't another operator at the top of the item
           stack or the operator has a higher precedence than the one currently on top of the stack */
        CalculatorOperator::Pointer topOperator = std::dynamic_pointer_cast<CalculatorOperator>(itemStack.top());
        while (NULL != topOperator && incomingOperator->hasHigherPrecedence(topOperator) == false)
        {
          rpnEquation.push_back(itemStack.pop());
          if (itemStack.isEmpty() == false)
          {
            topOperator = std::dynamic_pointer_cast<CalculatorOperator>(itemStack.top());
          }
          else
          {
            topOperator = NULL;
          }
        }
      }

      // Push the operator onto the rpn equation output.
      itemStack.push_back(calcItem);
    }
  }

  /* After we are done iterating through the infix equation items, keep transferring items from the item stack to the
     rpn equation output until the stack is empty. */
  while (itemStack.isEmpty() == false)
  {
    CalculatorItem::Pointer item = itemStack.pop();
    if (NULL != std::dynamic_pointer_cast<LeftParenthesisItem>(item))
    {
      QString ss = QObject::tr("One or more parentheses are mismatched in the chosen infix equation \"%1\".").arg(m_InfixEquation);
      setErrorCondition(MISMATCHED_PARENTHESES);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return QVector<CalculatorItem::Pointer>();
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
  filter->setUnits(getUnits());
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

