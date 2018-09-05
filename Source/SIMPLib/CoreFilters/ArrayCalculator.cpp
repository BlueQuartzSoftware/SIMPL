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
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/CalculatorFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/ScalarTypeFilterParameter.h"
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
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<DataArray<bool>>()(iDataArrayPtr))                                                                                                                           \
  {                                                                                                                                                                                                    \
    DataArray<bool>::Pointer arrayCast = std::dynamic_pointer_cast<DataArray<bool>>(iDataArrayPtr);                                                                                                    \
    itemPtr = CalculatorArray<bool>::New(arrayCast, ICalculatorArray::Array, !getInPreflight());                                                                                                       \
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArrayCalculator::ArrayCalculator()
: m_SelectedAttributeMatrix("", "", "")
, m_InfixEquation(QString())
, m_CalculatedArray("", "", "Output")
, m_Units(Radians)
, m_ScalarType(SIMPL::ScalarTypes::Type::Double)
{

  createSymbolMap();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArrayCalculator::~ArrayCalculator() = default;

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

  parameters.push_back(SIMPL_NEW_SCALARTYPE_FP("Scalar Type", ScalarType, FilterParameter::CreatedArray, ArrayCalculator));

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
void ArrayCalculator::writeFilterParameters(QJsonObject& obj) const
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
    setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::EMPTY_EQUATION));
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<CalculatorItem::Pointer> parsedInfix = parseInfixEquation();
  if(parsedInfix.isEmpty() == true)
  {
    return;
  }

  for(int i = 0; i < parsedInfix.size(); i++)
  {
    CalculatorItem::Pointer calcItem = parsedInfix[i];
    QString errMsg = "";
    CalculatorItem::ErrorCode err = calcItem->checkValidity(parsedInfix, i, errMsg);
    int errInt = static_cast<int>(err);
    if(errInt < 0)
    {
      setErrorCondition(errInt);
      notifyErrorMessage(getHumanLabel(), errMsg, getErrorCondition());
      return;
    }
  }

  if(false == m_CalculatedArray.isValid())
  {
    setErrorCondition(-4675);
    QString ss = QObject::tr("The output path must be valid");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataArrayPath calculatedAMPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");
  AttributeMatrix::Pointer calculatedAM = getDataContainerArray()->getAttributeMatrix(calculatedAMPath);
  AttributeMatrix::Pointer selectedAM = getDataContainerArray()->getAttributeMatrix(m_SelectedAttributeMatrix);

  QVector<size_t> cDims;
  ICalculatorArray::ValueType resultType = ICalculatorArray::ValueType::Unknown;

  for(int32_t i = 0; i < parsedInfix.size(); i++)
  {
    CalculatorItem::Pointer item1 = parsedInfix[i];
    if(item1->isICalculatorArray())
    {
      ICalculatorArray::Pointer array1 = std::dynamic_pointer_cast<ICalculatorArray>(item1);
      if (item1->isArray())
      {
        if (cDims.isEmpty() == false && resultType == ICalculatorArray::ValueType::Array && cDims != array1->getArray()->getComponentDimensions())
        {
          QString ss = QObject::tr("Attribute Array symbols in the infix expression have mismatching component dimensions");
          setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::INCONSISTENT_COMP_DIMS));
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          return;
        }

        resultType = ICalculatorArray::ValueType::Array;
        cDims = array1->getArray()->getComponentDimensions();
      }
      else if (resultType == ICalculatorArray::ValueType::Unknown)
      {
        resultType = ICalculatorArray::ValueType::Number;
        cDims = array1->getArray()->getComponentDimensions();
      }
    }
  }

  if(resultType == ICalculatorArray::ValueType::Unknown)
  {
    QString ss = QObject::tr("The expression does not have any arguments that simplify down to a number.");
    setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS));
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else if(resultType == ICalculatorArray::ValueType::Number)
  {
    QString ss = QObject::tr("The result of the chosen expression will be a numeric value or contain one tuple."
                             " This numeric value will be stored in an array with the number of tuples equal to 1");
    setWarningCondition(static_cast<int>(CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING));
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());

    if(calculatedAM->getNumberOfTuples() > 1)
    {
      QString ss = QObject::tr("The tuple count in the output attribute matrix at path '%1/%2' is greater than 1.  The current"
                               " expression evaluates to an array with a tuple count of 1, which does not match the output attribute matrix"
                               " tuple count.")
                       .arg(calculatedAMPath.getDataContainerName())
                       .arg(calculatedAMPath.getAttributeMatrixName());
      setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::INCORRECT_TUPLE_COUNT));
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
  else if(calculatedAM->getNumberOfTuples() != selectedAM->getNumberOfTuples())
  {
    QString ss = QObject::tr("The tuple count of the output Attribute Matrix is not equal to the tuple count of the selected Attribute Matrix");
    setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::INCORRECT_TUPLE_COUNT));
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<CalculatorItem::Pointer> rpn = toRPN(parsedInfix);
  if(rpn.isEmpty())
  {
    return;
  }

  switch(m_ScalarType)
  {
  case SIMPL::ScalarTypes::Type::Int8:
    getDataContainerArray()->createNonPrereqArrayFromPath<Int8ArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, cDims);
    break;
  case SIMPL::ScalarTypes::Type::UInt8:
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt8ArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, cDims);
    break;
  case SIMPL::ScalarTypes::Type::Int16:
    getDataContainerArray()->createNonPrereqArrayFromPath<Int16ArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, cDims);
    break;
  case SIMPL::ScalarTypes::Type::UInt16:
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt16ArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, cDims);
    break;
  case SIMPL::ScalarTypes::Type::Int32:
    getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, cDims);
    break;
  case SIMPL::ScalarTypes::Type::UInt32:
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, cDims);
    break;
  case SIMPL::ScalarTypes::Type::Int64:
    getDataContainerArray()->createNonPrereqArrayFromPath<Int64ArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, cDims);
    break;
  case SIMPL::ScalarTypes::Type::UInt64:
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt64ArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, cDims);
    break;
  case SIMPL::ScalarTypes::Type::Float:
    getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, cDims);
    break;
  case SIMPL::ScalarTypes::Type::Double:
    getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, cDims);
    break;
  case SIMPL::ScalarTypes::Type::Bool:
    getDataContainerArray()->createNonPrereqArrayFromPath<BoolArrayType, AbstractFilter, double>(this, m_CalculatedArray, 0, cDims);
    break;
  default:
    QString ss = QObject::tr("The output array type is not valid.  No DataArray could be created.");
    setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::InvalidOutputArrayType));
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
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

  // Parse the infix expression from the user interface
  QVector<CalculatorItem::Pointer> parsedInfix = parseInfixEquation();

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

    if(getCancel())
    {
      return;
    }
  }

  // Grab the result from the stack
  ICalculatorArray::Pointer arrayItem = ICalculatorArray::NullPointer();
  if(m_ExecutionStack.size() != 1)
  {
    QString ss = QObject::tr("The chosen infix equation is not a valid equation.");
    setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::INVALID_EQUATION));
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else if(!m_ExecutionStack.isEmpty())
  {
    arrayItem = m_ExecutionStack.pop();
  }

  if(arrayItem != ICalculatorArray::NullPointer())
  {
    IDataArray::Pointer resultArray = IDataArray::NullPointer();
    resultArray = arrayItem->getArray();

    IDataArray::Pointer resultTypeArray = IDataArray::NullPointer();
    resultTypeArray = convertArrayType(resultArray, m_ScalarType);

    DataArrayPath createdAMPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");
    AttributeMatrix::Pointer createdAM = getDataContainerArray()->getAttributeMatrix(createdAMPath);
    if(nullptr != createdAM)
    {
      resultTypeArray->setName(m_CalculatedArray.getDataArrayName());
      createdAM->addAttributeArray(resultTypeArray->getName(), resultTypeArray);
    }
  }
  else
  {
    QString ss = QObject::tr("Unexpected output item from chosen infix expression; the output item must be an array\n"
                             "Please contact the DREAM.3D developers for more information");
    setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::UNEXPECTED_OUTPUT));
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T> IDataArray::Pointer convertArray(DoubleArrayType::Pointer inputArray)
{
  if(nullptr == inputArray)
  {
    return nullptr;
  }

  double* rawInputarray = inputArray->getPointer(0);

  typename DataArray<T>::Pointer convertedArrayPtr = DataArray<T>::CreateArray(inputArray->getNumberOfTuples(), inputArray->getComponentDimensions(), inputArray->getName());
  T* rawOutputArray = convertedArrayPtr->getPointer(0);

  int count = inputArray->getSize();
  for(int i = 0; i < count; i++)
  {
    double val = rawInputarray[i];
    rawOutputArray[i] = val;
  }

  return convertedArrayPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer ArrayCalculator::convertArrayType(IDataArray::Pointer inputArray, SIMPL::ScalarTypes::Type scalarType)
{
  DoubleArrayType::Pointer inputDblArray = std::dynamic_pointer_cast<DoubleArrayType>(inputArray);
  if(nullptr == inputDblArray)
  {
    return nullptr;
  }

  IDataArray::Pointer castArray = nullptr;

  switch(scalarType)
  {
  case SIMPL::ScalarTypes::Type::Int8:
    castArray = convertArray<int8_t>(inputDblArray);
    break;
  case SIMPL::ScalarTypes::Type::UInt8:
    castArray = convertArray<uint8_t>(inputDblArray);
    break;
  case SIMPL::ScalarTypes::Type::Int16:
    castArray = convertArray<int16_t>(inputDblArray);
    break;
  case SIMPL::ScalarTypes::Type::UInt16:
    castArray = convertArray<uint16_t>(inputDblArray);
    break;
  case SIMPL::ScalarTypes::Type::Int32:
    castArray = convertArray<int32_t>(inputDblArray);
    break;
  case SIMPL::ScalarTypes::Type::UInt32:
    castArray = convertArray<uint32_t>(inputDblArray);
    break;
  case SIMPL::ScalarTypes::Type::Int64:
    castArray = convertArray<int64_t>(inputDblArray);
    break;
  case SIMPL::ScalarTypes::Type::UInt64:
    castArray = convertArray<uint64_t>(inputDblArray);
    break;
  case SIMPL::ScalarTypes::Type::Float:
    castArray = convertArray<float>(inputDblArray);
    break;
  case SIMPL::ScalarTypes::Type::Double:
    castArray = convertArray<double>(inputDblArray);
    break;
  case SIMPL::ScalarTypes::Type::Bool:
    castArray = convertArray<bool>(inputDblArray);
    break;
  default:
    break;
  }

  return castArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::createSymbolMap()
{
  // Insert all items into the symbol map to use during expression parsing
  {
    LeftParenthesisItem::Pointer symbol = LeftParenthesisItem::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    RightParenthesisItem::Pointer symbol = RightParenthesisItem::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    CommaSeparator::Pointer symbol = CommaSeparator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    AdditionOperator::Pointer symbol = AdditionOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    SubtractionOperator::Pointer symbol = SubtractionOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    MultiplicationOperator::Pointer symbol = MultiplicationOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    DivisionOperator::Pointer symbol = DivisionOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    PowOperator::Pointer symbol = PowOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    ABSOperator::Pointer symbol = ABSOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    SinOperator::Pointer symbol = SinOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    CosOperator::Pointer symbol = CosOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    TanOperator::Pointer symbol = TanOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    ASinOperator::Pointer symbol = ASinOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    ACosOperator::Pointer symbol = ACosOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    ATanOperator::Pointer symbol = ATanOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    SqrtOperator::Pointer symbol = SqrtOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    RootOperator::Pointer symbol = RootOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    Log10Operator::Pointer symbol = Log10Operator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    LogOperator::Pointer symbol = LogOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    ExpOperator::Pointer symbol = ExpOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    LnOperator::Pointer symbol = LnOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    FloorOperator::Pointer symbol = FloorOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
  {
    CeilOperator::Pointer symbol = CeilOperator::New();
    m_SymbolMap.insert(symbol->getInfixToken(), symbol);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<CalculatorItem::Pointer> ArrayCalculator::parseInfixEquation()
{
  int err = 0;

  AttributeMatrix::Pointer selectedAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedAttributeMatrix, err);

  QVector<QString> itemList = getRegularExpressionMatches();

  // Iterate through the QStringList and create the proper CalculatorItems
  QVector<CalculatorItem::Pointer> parsedInfix;

  for(int i = 0; i < itemList.size(); i++)
  {
    QString strItem = itemList[i];
    CalculatorItem::Pointer itemPtr;

    bool ok;
    double num = strItem.toDouble(&ok);
    if(ok == true)
    {
      // This is a numeric value
      parseNumericValue(strItem, parsedInfix, num);
    }
    else if(strItem == "-")
    {
      // This is a minus sign
      parseMinusSign(strItem, parsedInfix, i);
    }
    else if(strItem.contains("[") && strItem.contains("]"))
    {
      // This is an index operator
      if(!parseIndexOperator(strItem, parsedInfix))
      {
        return QVector<CalculatorItem::Pointer>();
      }
    }
    else
    {
      itemPtr = m_SymbolMap.value(strItem);

      if(nullptr != std::dynamic_pointer_cast<CommaSeparator>(itemPtr))
      {
        // This is a comma operator
        if(!parseCommaOperator(strItem, parsedInfix))
        {
          return QVector<CalculatorItem::Pointer>();
        }
      }
      else if(nullptr != itemPtr)
      {
        // This is another type of operator
        QString ss = QObject::tr("Item '%1' in the infix expression is the name of an array in the selected Attribute Matrix, but it is currently being used as a mathematical operator").arg(strItem);
        checkForAmbiguousArrayName(strItem, ss);

        parsedInfix.push_back(itemPtr);
      }
      else if(selectedAM->getAttributeArrayNames().contains(strItem) || (strItem.isEmpty() == false && strItem[0] == '\"' && strItem[strItem.size() - 1] == '\"'))
      {
        if(!parseArray(strItem, parsedInfix, selectedAM))
        {
          return QVector<CalculatorItem::Pointer>();
        }
      }
      else
      {
        QString ss = QObject::tr("An unrecognized item '%1' was found in the chosen infix expression").arg(strItem);
        setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::UNRECOGNIZED_ITEM));
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
QVector<QString> ArrayCalculator::getRegularExpressionMatches()
{
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

  return itemList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::parseMinusSign(QString token, QVector<CalculatorItem::Pointer>& parsedInfix, int loopIdx)
{
  CalculatorItem::Pointer itemPtr;

  // This could be either a negative sign or subtraction sign, so we need to figure out which one it is
  if(loopIdx == 0 || (((nullptr != std::dynamic_pointer_cast<CalculatorOperator>(parsedInfix.back()) &&
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
    itemPtr = m_SymbolMap.value(token);
    parsedInfix.push_back(itemPtr);
  }

  QString ss = QObject::tr("Item '%1' in the infix expression is the name of an array in the selected attribute matrix, but it is currently being used as a mathematical operator").arg(token);
  checkForAmbiguousArrayName(token, ss);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::parseNumericValue(QString token, QVector<CalculatorItem::Pointer>& parsedInfix, double number)
{
  // This is a number, so create an array with numOfTuples equal to 1 and set the value into it
  DoubleArrayType::Pointer ptr = DoubleArrayType::CreateArray(1, QVector<size_t>(1, 1), "INTERNAL_USE_ONLY_NumberArray");
  ptr->setValue(0, number);
  CalculatorItem::Pointer itemPtr = CalculatorArray<double>::New(ptr, ICalculatorArray::Number, !getInPreflight());
  parsedInfix.push_back(itemPtr);

  QString ss = QObject::tr("Item '%1' in the infix expression is the name of an array in the selected Attribute Matrix, but it is currently being used as a number").arg(token);
  checkForAmbiguousArrayName(token, ss);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ArrayCalculator::parseIndexOperator(QString token, QVector<CalculatorItem::Pointer>& parsedInfix)
{
  int idx = parsedInfix.size() - 1;

  QString errorMsg = QObject::tr("Index operator '%1' is not paired with a valid array name.").arg(token);
  int errCode = static_cast<int>(CalculatorItem::ErrorCode::ORPHANED_COMPONENT);
  if(idx < 0)
  {
    setErrorCondition(errCode);
    notifyErrorMessage(getHumanLabel(), errorMsg, getErrorCondition());
    return false;
  }
  else if (parsedInfix[idx]->isICalculatorArray() == false)
  {
    setErrorCondition(errCode);
    notifyErrorMessage(getHumanLabel(), errorMsg, getErrorCondition());
    return false;
  }
  else if (parsedInfix[idx]->isNumber())
  {
    setErrorCondition(errCode);
    notifyErrorMessage(getHumanLabel(), errorMsg, getErrorCondition());
    return false;
  }

  token.remove("[");
  token.remove("]");

  bool ok;
  int index = token.toInt(&ok);
  if(ok == false)
  {
    QString ss = QObject::tr("The chosen infix expression is not a valid expression");
    setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::INVALID_COMPONENT));
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return false;
  }

  ICalculatorArray::Pointer calcArray = std::dynamic_pointer_cast<ICalculatorArray>(parsedInfix.back());
  if(nullptr != calcArray && index >= calcArray->getArray()->getNumberOfComponents())
  {
    QString ss = QObject::tr("'%1' has an component index that is out of range").arg(calcArray->getArray()->getName());
    setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::COMPONENT_OUT_OF_RANGE));
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return false;
  }

  parsedInfix.pop_back();

  DoubleArrayType::Pointer reducedArray = calcArray->reduceToOneComponent(index, !getInPreflight());
  CalculatorItem::Pointer itemPtr;

  CREATE_CALCULATOR_ARRAY(itemPtr, reducedArray)
  parsedInfix.push_back(itemPtr);

  QString ss = QObject::tr("Item '%1' in the infix expression is the name of an array in the selected Attribute Matrix, but it is currently being used as an indexing operator").arg(token);
  checkForAmbiguousArrayName(token, ss);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ArrayCalculator::parseCommaOperator(QString token, QVector<CalculatorItem::Pointer>& parsedInfix)
{
  QString ss =
      QObject::tr("Item '%1' in the infix expression is the name of an array in the selected Attribute Matrix, but it is currently being detected as a comma in a mathematical operator").arg(token);
  checkForAmbiguousArrayName(token, ss);

  // Put parentheses around the entire term so that the RPN parser knows to evaluate the entire expression placed here
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

  CalculatorItem::Pointer itemPtr = m_SymbolMap.value(token);

  parsedInfix.push_back(itemPtr);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ArrayCalculator::parseArray(QString token, QVector<CalculatorItem::Pointer>& parsedInfix, AttributeMatrix::Pointer selectedAM)
{
  int firstArray_NumTuples = -1;
  QString firstArray_Name = "";

  token.remove("\"");
  if(selectedAM->getAttributeArrayNames().contains(token) == false)
  {
    QString ss = QObject::tr("The item '%1' is not the name of any valid array in the selected Attribute Matrix").arg(token);
    setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::INVALID_ARRAY_NAME));
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return false;
  }

  IDataArray::Pointer dataArray = selectedAM->getAttributeArray(token);
  if(firstArray_NumTuples < 0 && firstArray_Name.isEmpty() == true)
  {
    firstArray_NumTuples = dataArray->getNumberOfTuples();
    firstArray_Name = dataArray->getName();
  }
  else if(dataArray->getNumberOfTuples() != firstArray_NumTuples)
  {
    QString ss = QObject::tr("Arrays '%1' and '%2' in the infix expression have an inconsistent number of tuples").arg(firstArray_Name).arg(dataArray->getName());
    setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::INCONSISTENT_TUPLES));
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return false;
  }

  CalculatorItem::Pointer itemPtr;

  CREATE_CALCULATOR_ARRAY(itemPtr, dataArray)
  parsedInfix.push_back(itemPtr);
  return true;
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
      setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::MISMATCHED_PARENTHESES));
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

      setWarningCondition(static_cast<int>(CalculatorItem::WarningCode::AMBIGUOUS_NAME_WARNING));
      warningMsg.append("\nTo treat this item as an array name, please add double quotes around the item (i.e. \"" + strItem + "\").");
      notifyWarningMessage(getHumanLabel(), warningMsg, getWarningCondition());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ArrayCalculator::newFilterInstance(bool copyFilterParameters) const
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
const QString ArrayCalculator::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ArrayCalculator::getUuid()
{
  return QUuid("{7ff0ebb3-7b0d-5ff7-b9d8-5147031aca10}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getHumanLabel() const
{
  return "Attribute Array Calculator";
}
