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
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/CalculatorFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

#include "util/ABSOperator.h"
#include "util/ACosOperator.h"
#include "util/ASinOperator.h"
#include "util/ATanOperator.h"
#include "util/AdditionOperator.h"
#include "util/CalculatorArray.hpp"
#include "util/CeilOperator.h"
#include "util/CommaSeparator.h"
#include "util/CosOperator.h"
#include "util/DivisionOperator.h"
#include "util/ExpOperator.h"
#include "util/FloorOperator.h"
#include "util/IndexOperator.h"
#include "util/LeftParenthesisItem.h"
#include "util/LnOperator.h"
#include "util/Log10Operator.h"
#include "util/LogOperator.h"
#include "util/MultiplicationOperator.h"
#include "util/NegativeOperator.h"
#include "util/PowOperator.h"
#include "util/RightParenthesisItem.h"
#include "util/RootOperator.h"
#include "util/SinOperator.h"
#include "util/SqrtOperator.h"
#include "util/SubtractionOperator.h"
#include "util/TanOperator.h"

// Include the MOC generated file for this class
#include "moc_ArrayCalculator.cpp"

#define CREATE_CALCULATOR_ARRAY(itemPtr, iDataArrayPtr)                                                                                                                                                \
  if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(iDataArrayPtr))                                                                                                                                 \
  {                                                                                                                                                                                                    \
    FloatArrayType::Pointer arrayCast = std::dynamic_pointer_cast<FloatArrayType>(iDataArrayPtr);                                                                                                      \
    itemPtr = CalculatorArray<float>::New(arrayCast, ICalculatorArray::Array, !getInPreflight());                                                                                                      \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<DoubleArrayType>()(iDataArrayPtr))                                                                                                                           \
  {                                                                                                                                                                                                    \
    DoubleArrayType::Pointer arrayCast = std::dynamic_pointer_cast<DoubleArrayType>(iDataArrayPtr);                                                                                                    \
    itemPtr = CalculatorArray<double>::New(arrayCast, ICalculatorArray::Array, !getInPreflight());                                                                                                     \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int8ArrayType>()(iDataArrayPtr))                                                                                                                             \
  {                                                                                                                                                                                                    \
    Int8ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int8ArrayType>(iDataArrayPtr);                                                                                                        \
    itemPtr = CalculatorArray<int8_t>::New(arrayCast, ICalculatorArray::Array, !getInPreflight());                                                                                                     \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(iDataArrayPtr))                                                                                                                            \
  {                                                                                                                                                                                                    \
    UInt8ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt8ArrayType>(iDataArrayPtr);                                                                                                      \
    itemPtr = CalculatorArray<uint8_t>::New(arrayCast, ICalculatorArray::Array, !getInPreflight());                                                                                                    \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int16ArrayType>()(iDataArrayPtr))                                                                                                                            \
  {                                                                                                                                                                                                    \
    Int16ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int16ArrayType>(iDataArrayPtr);                                                                                                      \
    itemPtr = CalculatorArray<int16_t>::New(arrayCast, ICalculatorArray::Array, !getInPreflight());                                                                                                    \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(iDataArrayPtr))                                                                                                                           \
  {                                                                                                                                                                                                    \
    UInt16ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt16ArrayType>(iDataArrayPtr);                                                                                                    \
    itemPtr = CalculatorArray<uint16_t>::New(arrayCast, ICalculatorArray::Array, !getInPreflight());                                                                                                   \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int32ArrayType>()(iDataArrayPtr))                                                                                                                            \
  {                                                                                                                                                                                                    \
    Int32ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int32ArrayType>(iDataArrayPtr);                                                                                                      \
    itemPtr = CalculatorArray<int32_t>::New(arrayCast, ICalculatorArray::Array, !getInPreflight());                                                                                                    \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(iDataArrayPtr))                                                                                                                           \
  {                                                                                                                                                                                                    \
    UInt32ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt32ArrayType>(iDataArrayPtr);                                                                                                    \
    itemPtr = CalculatorArray<uint32_t>::New(arrayCast, ICalculatorArray::Array, !getInPreflight());                                                                                                   \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int64ArrayType>()(iDataArrayPtr))                                                                                                                            \
  {                                                                                                                                                                                                    \
    Int64ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<Int64ArrayType>(iDataArrayPtr);                                                                                                      \
    itemPtr = CalculatorArray<int64_t>::New(arrayCast, ICalculatorArray::Array, !getInPreflight());                                                                                                    \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(iDataArrayPtr))                                                                                                                           \
  {                                                                                                                                                                                                    \
    UInt64ArrayType::Pointer arrayCast = std::dynamic_pointer_cast<UInt64ArrayType>(iDataArrayPtr);                                                                                                    \
    itemPtr = CalculatorArray<uint64_t>::New(arrayCast, ICalculatorArray::Array, !getInPreflight());                                                                                                   \
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArrayCalculator::ArrayCalculator()
: AbstractFilter()
, m_SelectedAttributeMatrix("", "", "")
, m_InfixEquation(QString())
, m_CalculatedArray("", "", "Output")
, m_Units(Radians)
{
  setupFilterParameters();

  // Insert all items into the symbol map to use during expression parsing
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
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Any, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", SelectedAttributeMatrix, FilterParameter::Parameter, ArrayCalculator, req));
  }

  parameters.push_back(SIMPL_NEW_CALC_FP("Infix Expression", InfixEquation, FilterParameter::Parameter, ArrayCalculator));

  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Any, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Calculated Array", CalculatedArray, FilterParameter::CreatedArray, ArrayCalculator, req));
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
void ArrayCalculator::readFilterParameters(QJsonObject& obj)
{
  AbstractFilter::readFilterParameters(obj);
  setUnits(static_cast<ArrayCalculator::AngleUnits>(obj["Units"].toInt()));
}

// FP: Check why these values are not connected to a filter parameter!

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::writeFilterParameters(QJsonObject& obj)
{
  AbstractFilter::writeFilterParameters(obj);
  obj["Units"] = static_cast<int>(getUnits());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::initialize()
{
  m_ExecutionStack.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedAttributeMatrix, -301);
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(m_InfixEquation.isEmpty() == true || m_InfixEquation.split(" ", QString::SkipEmptyParts).size() <= 0)
  {
    QString ss = QObject::tr("The infix expression is empty");
    setErrorCondition(EMPTY_EQUATION);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<CalculatorItem::Pointer> parsedInfix = parseInfixEquation(m_InfixEquation);
  if(parsedInfix.isEmpty() == true)
  {
    return;
  }

  bool hasArrayGreaterThan1 = false;
  bool hasValue = false;
  bool checkCompDims = true;
  QVector<size_t> compDims(1, 1);
  QVector<size_t> tmpCompDims(1, 1);
  for(int32_t i = 0; i < parsedInfix.size(); i++)
  {
    CalculatorItem::Pointer currentItem = parsedInfix[i];
    if(nullptr != std::dynamic_pointer_cast<CalculatorOperator>(currentItem))
    {
      bool result = std::dynamic_pointer_cast<CalculatorOperator>(currentItem)->checkValidity(parsedInfix, i);
      if(result == false)
      {
        QString ss = QObject::tr("The chosen infix expression is not a valid expression");
        setErrorCondition(INVALID_EQUATION);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
    else if(nullptr != std::dynamic_pointer_cast<ICalculatorArray>(currentItem))
    {
      hasValue = true;
      ICalculatorArray::Pointer calcArray = std::dynamic_pointer_cast<ICalculatorArray>(currentItem);
      if(calcArray->getArray()->getNumberOfTuples() > 1)
      {
        hasArrayGreaterThan1 = true;
      }

      // If the calculator array items are truly arrays, verify that they
      // all have the same component dimensions
      if(calcArray->getType() == ICalculatorArray::Array)
      {
        if(checkCompDims)
        {
          tmpCompDims = calcArray->getArray()->getComponentDimensions();
          checkCompDims = false;
        }
        else
        {
          compDims = calcArray->getArray()->getComponentDimensions();
          if(tmpCompDims != compDims)
          {
            QString ss = QObject::tr("All Attribute Arrays in the infix expression must have the same component dimensions");
            setErrorCondition(INCONSISTENT_COMP_DIMS);
            notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
            return;
          }
        }
      }
    }
  }

  // Ensure the output component dims are correct if we happened to only have
  // one array in the infix expression
  compDims = tmpCompDims;

  if(hasValue == false)
  {
    QString ss = QObject::tr("The chosen infix expression is not a valid expression");
    setErrorCondition(INVALID_EQUATION);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataArrayPath calculatedAMPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");

  if(hasArrayGreaterThan1 == false)
  {
    QString ss = QObject::tr("The result of the chosen expression will be a numeric value or contain one tuple\n"
                             "This numeric value will be stored in an array with the number of tuples equal to 1");
    setWarningCondition(NUMERIC_VALUE_WARNING);
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());

    AttributeMatrix::Pointer calculatedAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, calculatedAMPath, -301);
    if(getErrorCondition() < 0)
    {
      return;
    }

    if(calculatedAM->getNumberOfTuples() != 1)
    {
      QString ss = QObject::tr("The tuple count of the output Attribute Matrix is not equal to 1");
      setErrorCondition(INCORRECT_TUPLE_COUNT);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
  else
  {
    AttributeMatrix::Pointer selectedAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedAttributeMatrix, -301);
    AttributeMatrix::Pointer calculatedAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, calculatedAMPath, -301);
    if(getErrorCondition() < 0)
    {
      return;
    }
    if(nullptr != selectedAM && calculatedAM->getNumberOfTuples() != selectedAM->getNumberOfTuples())
    {
      QString ss = QObject::tr("The tuple count of the output Attribute Matrix is not equal to the tuple count of the selected Attribute Matrix");
      setErrorCondition(INCORRECT_TUPLE_COUNT);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }

  QVector<CalculatorItem::Pointer> rpn = toRPN(parsedInfix);
  if(rpn.isEmpty() == true)
  {
    return;
  }

  getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, compDims);
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
  if(getErrorCondition() < 0)
  {
    return;
  }
  initialize();

  DataArrayPath calculatedAMPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");
  AttributeMatrix::Pointer calculatedAM = getDataContainerArray()->getAttributeMatrix(calculatedAMPath);

  // Parse the infix expression from the user interface
  QVector<CalculatorItem::Pointer> parsedInfix = parseInfixEquation(m_InfixEquation);

  // Convert the parsed infix expression into RPN
  QVector<CalculatorItem::Pointer> rpn = toRPN(parsedInfix);

  // Execute the RPN expression
  int totalItems = rpn.size();
  for(int rpnCount = 0; rpnCount < totalItems; rpnCount++)
  {
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Computing Operator " + QString::number(rpnCount + 1) + "/" + QString::number(totalItems));

    CalculatorItem::Pointer rpnItem = rpn[rpnCount];
    ICalculatorArray::Pointer calcArray = std::dynamic_pointer_cast<ICalculatorArray>(rpnItem);
    if(nullptr != calcArray)
    {
      // This is an array
      m_ExecutionStack.push(calcArray);
    }
    else
    {
      // This is an operator
      CalculatorOperator::Pointer rpnOperator = std::dynamic_pointer_cast<CalculatorOperator>(rpnItem);

      rpnOperator->calculate(this, m_CalculatedArray, m_ExecutionStack);
      if(getErrorCondition() < 0)
      {
        return;
      }
    }

    if(getCancel() == true)
    {
      return;
    }
  }

  // Grab the result from the stack
  ICalculatorArray::Pointer arrayItem = ICalculatorArray::NullPointer();
  if(!m_ExecutionStack.isEmpty())
  {
    arrayItem = m_ExecutionStack.pop();
  }

  if(nullptr != arrayItem)
  {
    IDataArray::Pointer resultArray = IDataArray::NullPointer();
    resultArray = arrayItem->getArray();

    DataArrayPath createdAMPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");
    AttributeMatrix::Pointer createdAM = getDataContainerArray()->getAttributeMatrix(createdAMPath);
    if(nullptr != createdAM)
    {
      resultArray->setName(m_CalculatedArray.getDataArrayName());
      createdAM->addAttributeArray(resultArray->getName(), resultArray);
    }
  }
  else
  {
    QString ss = QObject::tr("Unexpected output item from chosen infix expression; the output item must be an array\n"
                             "Please contact the DREAM.3D developers for more information");
    setErrorCondition(UNEXPECTED_OUTPUT);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<CalculatorItem::Pointer> ArrayCalculator::parseInfixEquation(QString expression)
{
  int err = 0;

  AttributeMatrix::Pointer selectedAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedAttributeMatrix, err);

  // Parse all the items into a QVector of strings using a regular expression
  QVector<QString> itemList;
  // Match all array names that start with two alphabetical characters and have spaces.  Match all numbers, decimal or integer.
  // Match one letter array names.  Match all special character operators.
  QRegularExpression regExp("(\"((\\[)?\\d+(\\.\\d+)?(\\])?|(\\[)?\\.\\d+(\\])?|\\w{1,1}((\\w|\\s|\\d)*(\\w|\\d){1,1})?|\\S)\")|(((\\[)?\\d+(\\.\\d+)?(\\])?|(\\[)?\\.\\d+(\\])?|\\w{1,1}((\\w|\\s|\\d)"
                            "*(\\w|\\d){1,1})?|\\S))");
  QRegularExpressionMatchIterator iter = regExp.globalMatch(m_InfixEquation);
  while(iter.hasNext())
  {
    QRegularExpressionMatch match = iter.next();
    itemList.push_back(match.captured());
  }

  // Iterate through the QStringList and create the proper CalculatorItems
  QVector<CalculatorItem::Pointer> parsedInfix;
  int firstArray_NumTuples = -1;
  QString firstArray_Name = "";
  for(int i = 0; i < itemList.size(); i++)
  {
    QString strItem = itemList[i];
    CalculatorItem::Pointer itemPtr;

    bool ok;
    double num = strItem.toDouble(&ok);
    if(ok == true)
    {
      // This is a number, so create an array with numOfTuples equal to 1 and set the value into it
      DoubleArrayType::Pointer ptr = DoubleArrayType::CreateArray(1, QVector<size_t>(1, 1), "INTERNAL_USE_ONLY_NumberArray");
      ptr->setValue(0, num);
      itemPtr = CalculatorArray<double>::New(ptr, ICalculatorArray::Number, !getInPreflight());
      parsedInfix.push_back(itemPtr);

      QString ss = QObject::tr("Item '%1' in the infix expression is the name of an array in the selected Attribute Matrix, but it is currently being used as a number").arg(strItem);
      checkForAmbiguousArrayName(strItem, ss);
    }
    else if(strItem == "-")
    {
      // This could be either a negative sign or subtraction sign, so we need to figure out which one it is
      if(i == 0 || (((nullptr != std::dynamic_pointer_cast<CalculatorOperator>(parsedInfix.back()) &&
                      std::dynamic_pointer_cast<CalculatorOperator>(parsedInfix.back())->getOperatorType() == CalculatorOperator::Binary) ||
                     nullptr != std::dynamic_pointer_cast<LeftParenthesisItem>(parsedInfix.back())) &&
                    nullptr == std::dynamic_pointer_cast<RightParenthesisItem>(parsedInfix.back())))
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

      QString ss = QObject::tr("Item '%1' in the infix expression is the name of an array in the selected attribute matrix, but it is currently being used as a mathematical operator").arg(strItem);
      checkForAmbiguousArrayName(strItem, ss);
    }
    else if(strItem.contains("[") && strItem.contains("]"))
    {
      // This is an array index, so create an index operator
      strItem.remove("[");
      strItem.remove("]");

      bool ok;
      int index = strItem.toInt(&ok);
      if(ok == false)
      {
        QString ss = QObject::tr("The chosen infix expression is not a valid expression");
        setErrorCondition(INVALID_EQUATION);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return QVector<CalculatorItem::Pointer>();
      }

      ICalculatorArray::Pointer calcArray = std::dynamic_pointer_cast<ICalculatorArray>(parsedInfix.back());
      if(nullptr != calcArray && index >= calcArray->getArray()->getNumberOfComponents())
      {
        QString ss = QObject::tr("'%1' has an component index that is out of range").arg(calcArray->getArray()->getName());
        setErrorCondition(COMPONENT_OUT_OF_RANGE);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return QVector<CalculatorItem::Pointer>();
      }

      itemPtr = IndexOperator::New(index);
      parsedInfix.push_back(itemPtr);

      QString ss = QObject::tr("Item '%1' in the infix expression is the name of an array in the selected Attribute Matrix, but it is currently being used as an indexing operator").arg(strItem);
      checkForAmbiguousArrayName(strItem, ss);
    }
    else
    {
      itemPtr = m_SymbolMap.value(strItem);

      if(nullptr != std::dynamic_pointer_cast<CommaSeparator>(itemPtr))
      {
        QString ss = QObject::tr("Item '%1' in the infix expression is the name of an array in the selected Attribute Matrix, but it is currently being detected as a comma in a mathematical operator")
                         .arg(strItem);
        checkForAmbiguousArrayName(strItem, ss);

        // This is a comma, so make sure that this comma has a valid unary operator before it
        {
          QVectorIterator<CalculatorItem::Pointer> iter(parsedInfix);
          iter.toBack();
          bool foundUnaryOperator = false;
          while(iter.hasPrevious())
          {
            CalculatorItem::Pointer item = iter.previous();
            if(nullptr != std::dynamic_pointer_cast<UnaryOperator>(item))
            {
              foundUnaryOperator = true;
            }
          }

          if(foundUnaryOperator == false)
          {
            QString ss = QObject::tr("The chosen infix expression is not a valid expression");
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
        while(iter != parsedInfix.begin())
        {
          if(nullptr != std::dynamic_pointer_cast<CommaSeparator>(*iter) || nullptr != std::dynamic_pointer_cast<LeftParenthesisItem>(*iter))
          {
            iter++;
            parsedInfix.insert(iter, LeftParenthesisItem::New());
            break;
          }

          iter--;
        }

        parsedInfix.push_back(itemPtr);
      }
      else if(nullptr != itemPtr)
      {
        QString ss = QObject::tr("Item '%1' in the infix expression is the name of an array in the selected Attribute Matrix, but it is currently being used as a mathematical operator").arg(strItem);
        checkForAmbiguousArrayName(strItem, ss);

        parsedInfix.push_back(itemPtr);
      }
      else if(selectedAM->getAttributeArrayNames().contains(strItem) || (strItem.isEmpty() == false && strItem[0] == '\"' && strItem[strItem.size() - 1] == '\"'))
      {
        strItem.remove("\"");
        if(selectedAM->getAttributeArrayNames().contains(strItem) == false)
        {
          QString ss = QObject::tr("The item '%1' is not the name of any valid array in the selected Attribute Matrix").arg(strItem);
          setErrorCondition(INVALID_ARRAY_NAME);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          return QVector<CalculatorItem::Pointer>();
        }

        IDataArray::Pointer dataArray = selectedAM->getAttributeArray(strItem);
        if(firstArray_NumTuples < 0 && firstArray_Name.isEmpty() == true)
        {
          firstArray_NumTuples = dataArray->getNumberOfTuples();
          firstArray_Name = dataArray->getName();
        }
        else if(dataArray->getNumberOfTuples() != firstArray_NumTuples)
        {
          QString ss = QObject::tr("Arrays '%1' and '%2' in the infix expression have an inconsistent number of tuples").arg(firstArray_Name).arg(dataArray->getName());
          setErrorCondition(INCONSISTENT_TUPLES);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          return QVector<CalculatorItem::Pointer>();
        }

        CREATE_CALCULATOR_ARRAY(itemPtr, dataArray)
        parsedInfix.push_back(itemPtr);
      }
      else
      {
        QString ss = QObject::tr("An unrecognized item '%1' was found in the chosen infix expression").arg(strItem);
        setErrorCondition(UNRECOGNIZED_ITEM);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return QVector<CalculatorItem::Pointer>();
      }
    }
  }

  // Return the parsed infix expression as a vector of CalculatorItems
  return parsedInfix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<CalculatorItem::Pointer> ArrayCalculator::toRPN(QVector<CalculatorItem::Pointer> infixEquation)
{
  QStack<CalculatorItem::Pointer> itemStack;
  QVector<CalculatorItem::Pointer> rpnEquation;

  bool* oneComponent = nullptr;

  // Iterate through the infix expression items
  for(int i = 0; i < infixEquation.size(); i++)
  {
    CalculatorItem::Pointer calcItem = infixEquation[i];
    if(nullptr != std::dynamic_pointer_cast<ICalculatorArray>(calcItem))
    {
      ICalculatorArray::Pointer arrayItem = std::dynamic_pointer_cast<ICalculatorArray>(calcItem);

      // This is a number or array, so push it onto the rpn expression output
      rpnEquation.push_back(arrayItem);

      if(arrayItem->getType() == ICalculatorArray::Array)
      {
        if(i + 1 < infixEquation.size() && nullptr != std::dynamic_pointer_cast<IndexOperator>(infixEquation[i + 1]) && nullptr == oneComponent)
        {
          oneComponent = new bool(true);
        }
        else if((i + 1 >= infixEquation.size() || nullptr == std::dynamic_pointer_cast<IndexOperator>(infixEquation[i + 1])) && nullptr == oneComponent)
        {
          oneComponent = new bool(false);
        }
        else if(((i + 1 >= infixEquation.size() || nullptr == std::dynamic_pointer_cast<IndexOperator>(infixEquation[i + 1])) && *oneComponent == true) ||
                ((i + 1 < infixEquation.size() && nullptr != std::dynamic_pointer_cast<IndexOperator>(infixEquation[i + 1])) && *oneComponent == false))
        {
          QString ss = QObject::tr("Not all arrays have a component index. All arrays must specify a component index (i.e. %1[0]), or none at all").arg(arrayItem->getArray()->getName());
          setErrorCondition(INCONSISTENT_INDEXING);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          return QVector<CalculatorItem::Pointer>();
        }
      }
    }
    else if(nullptr != std::dynamic_pointer_cast<LeftParenthesisItem>(calcItem))
    {
      // This is a left parenthesis, so push it onto the item stack
      itemStack.push_back(calcItem);
    }
    else if(nullptr != std::dynamic_pointer_cast<RightParenthesisItem>(calcItem))
    {
      // This is a right parenthesis, so push operators from the item stack onto the rpn expression output until we get to the left parenthesis
      while(itemStack.isEmpty() == false && nullptr == std::dynamic_pointer_cast<LeftParenthesisItem>(itemStack.top()))
      {
        rpnEquation.push_back(itemStack.pop());
      }

      if(itemStack.isEmpty() == true)
      {
        QString ss = QObject::tr("One or more parentheses are mismatched in the chosen infix expression '%1'").arg(m_InfixEquation);
        setErrorCondition(MISMATCHED_PARENTHESES);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return QVector<CalculatorItem::Pointer>();
      }

      // Discard the left parenthesis that we found
      itemStack.pop();
    }
    else if(nullptr != std::dynamic_pointer_cast<CalculatorSeparator>(calcItem))
    {
      // This is a comma, so we want to continue without adding it to anything
      continue;
    }
    else
    {
      // This is an operator
      CalculatorOperator::Pointer incomingOperator = std::dynamic_pointer_cast<CalculatorOperator>(calcItem);

      if(itemStack.isEmpty() == false)
      {
        /* If the operator's precedence is lower than the precedence of the operator on top of the item stack, push the operator at the top
           of the item stack onto the rpn expression output.  Keeping doing this until there isn't another operator at the top of the item
           stack or the operator has a higher precedence than the one currently on top of the stack */
        CalculatorOperator::Pointer topOperator = std::dynamic_pointer_cast<CalculatorOperator>(itemStack.top());
        while(nullptr != topOperator && incomingOperator->hasHigherPrecedence(topOperator) == false)
        {
          rpnEquation.push_back(itemStack.pop());
          if(itemStack.isEmpty() == false)
          {
            topOperator = std::dynamic_pointer_cast<CalculatorOperator>(itemStack.top());
          }
          else
          {
            topOperator = nullptr;
          }
        }
      }

      // Push the operator onto the rpn expression output.
      itemStack.push_back(calcItem);
    }
  }

  /* After we are done iterating through the infix expression items, keep transferring items from the item stack to the
     rpn expression output until the stack is empty. */
  while(itemStack.isEmpty() == false)
  {
    CalculatorItem::Pointer item = itemStack.pop();
    if(nullptr != std::dynamic_pointer_cast<LeftParenthesisItem>(item))
    {
      QString ss = QObject::tr("One or more parentheses are mismatched in the chosen infix expression '%1'").arg(m_InfixEquation);
      setErrorCondition(MISMATCHED_PARENTHESES);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return QVector<CalculatorItem::Pointer>();
    }

    rpnEquation.push_back(item);
  }

  if(oneComponent != nullptr)
  {
    delete oneComponent;
  }

  return rpnEquation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::checkForAmbiguousArrayName(QString strItem, QString warningMsg)
{
  if(getInPreflight())
  {
    int err = 0;
    AttributeMatrix::Pointer selectedAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedAttributeMatrix, err);
    if(getErrorCondition() < 0)
    {
      return;
    }
    if(selectedAM->getAttributeArrayNames().contains(strItem))
    {
      IDataArray::Pointer dataArray = selectedAM->getAttributeArray(strItem);

      setWarningCondition(AMBIGUOUS_NAME_WARNING);
      warningMsg.append("\nTo treat this item as an array name, please add double quotes around the item (i.e. \"" + strItem + "\").");
      notifyWarningMessage(getHumanLabel(), warningMsg, getErrorCondition());
    }
  }
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
{
  return Core::CoreBaseName;
}

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
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getGroupName()
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getSubGroupName()
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getHumanLabel()
{
  return "Attribute Array Calculator";
}
