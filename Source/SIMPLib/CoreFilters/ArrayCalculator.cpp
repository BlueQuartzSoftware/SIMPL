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
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/CalculatorFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"

#include "util/CalculatorArray.h"
#include "util/LeftParenthesisSeparator.h"
#include "util/RightParenthesisSeparator.h"
#include "util/AdditionOperator.h"
#include "util/SubtractionOperator.h"
#include "util/MultiplicationOperator.h"
#include "util/DivisionOperator.h"
#include "util/ABSOperator.h"

// Include the MOC generated file for this class
#include "moc_ArrayCalculator.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArrayCalculator::ArrayCalculator() :
  AbstractFilter()
{
  setupFilterParameters();

  m_SymbolList.push_back("(");
  m_SymbolList.push_back(")");
  m_SymbolList.push_back("+");
  m_SymbolList.push_back("-");
  m_SymbolList.push_back("*");
  m_SymbolList.push_back("/");
  m_SymbolList.push_back("abs");
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

  QVector<QSharedPointer<CalculatorItem> > parsedInfix = parseInfixEquation(m_InfixEquation);
  if (parsedInfix.isEmpty() == true) { return; }

  /* Check for two operators in a row, or operators at the beginning or end of the expression.
     If this occurs, then this is not a valid expression */
  for (int i = 0; i < parsedInfix.size(); i++)
  {
    QSharedPointer<CalculatorItem> item = parsedInfix[i];
    if (NULL != qSharedPointerDynamicCast<CalculatorOperator>(item))
    {
      QSharedPointer<CalculatorOperator> operatorItem = qSharedPointerDynamicCast<CalculatorOperator>(item);
      if (operatorItem->getOperatorType() == CalculatorOperator::Binary)
      {
        if (i > parsedInfix.size() - 2 || NULL != qSharedPointerDynamicCast<CalculatorOperator>(parsedInfix[i+1])
          || i < 1 || NULL != qSharedPointerDynamicCast<CalculatorOperator>(parsedInfix[i - 1]))
        {
          QString ss = QObject::tr("The chosen infix equation is not a valid equation.");
          setErrorCondition(-4005);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          return;
        }
      }
    }
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

  // Parse the infix equation from the user interface
  QVector<QSharedPointer<CalculatorItem> > parsedInfix = parseInfixEquation(m_InfixEquation);

  // Convert the parsed infix equation into RPN
  QVector<QSharedPointer<CalculatorItem> > rpn = toRPN(parsedInfix);

  // Execute the RPN equation
  for (int i = 0; i < rpn.size(); i++)
  {
    QSharedPointer<CalculatorItem> rpnItem = rpn[i];
    if (NULL != qSharedPointerDynamicCast<CalculatorArray>(rpnItem))
    {
      m_ExecutionStack.push(rpnItem);
    }
    else if (NULL != qSharedPointerDynamicCast<CalculatorOperator>(rpnItem))
    {
      QSharedPointer<CalculatorOperator> rpnOperator = qSharedPointerDynamicCast<CalculatorOperator>(rpnItem);
      QSharedPointer<CalculatorItem> rpnCalculatedItem = rpnOperator->calculate(this, m_CalculatedArray.getDataArrayName(), m_ExecutionStack);
      if (NULL == rpnCalculatedItem) { return; }

      m_ExecutionStack.push(rpnCalculatedItem);
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
  if (NULL != qSharedPointerDynamicCast<CalculatorArray>(resultItem))
  {
    QSharedPointer<CalculatorArray> arrayItem = qSharedPointerDynamicCast<CalculatorArray>(resultItem);
    newArray = arrayItem->getArray();
  }
  else
  {
    QString ss = QObject::tr("Unexpected output item from chosen infix equation.  The output item must be an array."
                             "Please contact the DREAM3D developers for more information.");
    setErrorCondition(-4009);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataArrayPath amPath(m_CalculatedArray.getDataContainerName(), m_CalculatedArray.getAttributeMatrixName(), "");
  AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(amPath);
  if (NULL != am)
  {
    am->addAttributeArray(m_CalculatedArray.getDataArrayName(), newArray);
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
  AttributeMatrix::Pointer am = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, m_SelectedAttributeMatrix, err);
  if (NULL == am)
  {
    QString ss = QObject::tr("Could not find the attribute matrix \"%1\".").arg(m_SelectedAttributeMatrix.getAttributeMatrixName());
    setErrorCondition(-4001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return QVector<QSharedPointer<CalculatorItem> >();
  }

  // Remove spaces and place @ symbols around each item in the equation
  for (int i = 0; i < m_SymbolList.size(); i++)
  {
    QString symbolStr = m_SymbolList[i];

    equation.replace(symbolStr, "@" + symbolStr + "@");
  }

  QStringList aaNames = am->getAttributeArrayNames();
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
      IDataArray::Pointer ptr = DoubleArrayType::CreateArray(1, QVector<size_t>(1, 1), "NumberArray");
      ptr->initializeTuple(0, &num);
      itemPtr = QSharedPointer<CalculatorArray>(new CalculatorArray(ptr));
    }
    else if (listItem == "(")
    {
      itemPtr = QSharedPointer<LeftParenthesisSeparator>(new LeftParenthesisSeparator());
    }
    else if (listItem == ")")
    {
      itemPtr = QSharedPointer<RightParenthesisSeparator>(new RightParenthesisSeparator());
    }
    else if (listItem == "+")
    {
      itemPtr = QSharedPointer<AdditionOperator>(new AdditionOperator());
    }
    else if (listItem == "-")
    {
      itemPtr = QSharedPointer<SubtractionOperator>(new SubtractionOperator());
    }
    else if (listItem == "*")
    {
      itemPtr = QSharedPointer<MultiplicationOperator>(new MultiplicationOperator());
    }
    else if (listItem == "/")
    {
      itemPtr = QSharedPointer<DivisionOperator>(new DivisionOperator());
    }
    else if (listItem == "abs")
    {
      itemPtr = QSharedPointer<ABSOperator>(new ABSOperator());
    }
    else if (am->getAttributeArrayNames().contains(listItem))
    {
      IDataArray::Pointer dataArray = am->getAttributeArray(listItem);
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
      itemPtr = QSharedPointer<CalculatorArray>(new CalculatorArray(dataArray));
    }
    else
    {
      QString ss = QObject::tr("An unrecognized item \"%1\" was found in the chosen infix equation.").arg(listItem);
      setErrorCondition(-4002);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return QVector<QSharedPointer<CalculatorItem> >();
    }

    parsedInfix.push_back(itemPtr);
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
    if (NULL != qSharedPointerDynamicCast<CalculatorArray>(calcItem))
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
    rpnEquation.push_back(itemStack.pop());
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

