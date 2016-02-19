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

#include "util/CalculatorNumber.h"
#include "util/LeftParenthesisSeparator.h"
#include "util/RightParenthesisSeparator.h"
#include "util/AdditionOperator.h"
#include "util/SubtractionOperator.h"
#include "util/MultiplicationOperator.h"
#include "util/DivisionOperator.h"

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
  AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyAttributeMatrix, SIMPL::Defaults::AnyGeometry);
  parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Attribute Matrix", "SelectedAttributeMatrix", getSelectedAttributeMatrix(), FilterParameter::Parameter, req));

  parameters.push_back(CalculatorFilterParameter::New("Infix Equation", "InfixEquation", getInfixEquation(), FilterParameter::Parameter));
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
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::dataCheck()
{
  setErrorCondition(0);

  // Do Stuff
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
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
    if (NULL != qSharedPointerDynamicCast<CalculatorNumber>(rpnItem) || NULL != qSharedPointerDynamicCast<CalculatorArray>(rpnItem))
    {
      m_ExecutionStack.push(rpnItem);
    }
    else if (NULL != qSharedPointerDynamicCast<CalculatorOperator>(rpnItem))
    {
      QSharedPointer<CalculatorOperator> rpnOperator = qSharedPointerDynamicCast<CalculatorOperator>(rpnItem);
      QSharedPointer<CalculatorItem> rpnCalculatedItem = rpnOperator->calculate(m_ExecutionStack);
      m_ExecutionStack.push(rpnCalculatedItem);
    }
    else
    {
      // ERROR: Unrecognized item in the RPN vector
    }
  }

  // Grab the result from the stack
  QSharedPointer<CalculatorItem> resultItem = m_ExecutionStack.pop();
  QSharedPointer<CalculatorNumber> numberItem = qSharedPointerDynamicCast<CalculatorNumber>(resultItem);
  double num = numberItem->getNumber();

  if (getCancel() == true) { return; }

  //if (getErrorCondition() < 0)
  //{
  //  QString ss = QObject::tr("Some error message");
  //  setErrorCondition(-99999999);
  //  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  //  return;
  //}

  //notifyStatusMessage(getHumanLabel(), "Complete");
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
    // ERROR: Could not find the attribute matrix
  }

  // Remove all spaces
  equation.remove(" ");

  // Place @ symbols around each item in the equation
  for (int i = 0; i < m_SymbolList.size(); i++)
  {
    QString symbolStr = m_SymbolList[i];

    equation.replace(symbolStr, "@" + symbolStr + "@");
  }
  
  // Now split the equation up into a QStringList of items
  QStringList list = equation.split("@", QString::SkipEmptyParts);

  // Iterate through the QStringList and create the proper CalculatorItems
  QVector<QSharedPointer<CalculatorItem> > parsedInfix;
  for (int i = 0; i < list.size(); i++)
  {
    QString listItem = list[i];
    QSharedPointer<CalculatorItem> itemPtr;

    bool ok;
    double num = listItem.toDouble(&ok);
    if (ok == true)
    {
      itemPtr = QSharedPointer<CalculatorNumber>(new CalculatorNumber(num));
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
    if (NULL != qSharedPointerDynamicCast<CalculatorNumber>(calcItem))
    {
      // This is a number, so push it onto the rpn equation output
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
        // ERROR: Could not find matching left parenthesis
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
          topOperator = qSharedPointerDynamicCast<CalculatorOperator>(itemStack.top());
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

