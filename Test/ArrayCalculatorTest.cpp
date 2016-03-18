/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <QtCore/QCoreApplication>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"

#include "SIMPLib/CoreFilters/ArrayCalculator.h"
#include "SIMPLib/CoreFilters/CreateDataArray.h"

#include "SIMPLTestFileLocations.h"

#include "SIMPLib/CoreFilters/util/CalculatorOperator.h"

class DummyObserver : public Observer
{
public:
  DummyObserver() : Observer() {}
  virtual ~DummyObserver() {}
  SIMPL_TYPE_MACRO(DummyObserver)

  void processPipelineMessage(const PipelineMessage& pm)
  {
    // Don't do anything...this class only exists to stifle the messages being dumped to the console
  }

private:
  DummyObserver(const DummyObserver&); // Copy Constructor Not Implemented
  void operator=(const DummyObserver&); // Operator '=' Not Implemented
};

class ArrayCalculatorTest
{
public:
  ArrayCalculatorTest() {}
  virtual ~ArrayCalculatorTest() {}
  SIMPL_TYPE_MACRO(ArrayCalculatorTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer dc = DataContainer::New("DataContainer");
    AttributeMatrix::Pointer am1 = AttributeMatrix::New(QVector<size_t>(1, 10), "AttributeMatrix", 3);
    AttributeMatrix::Pointer am2 = AttributeMatrix::New(QVector<size_t>(1, 1), "NumericMatrix", 3);
    FloatArrayType::Pointer array1 = FloatArrayType::CreateArray(10, "InputArray1");
    array1->initializeWithValue(-12, 0);
    UInt32ArrayType::Pointer array2 = UInt32ArrayType::CreateArray(10, "InputArray2");
    array2->initializeWithValue(10, 0);
    UInt32ArrayType::Pointer array3 = UInt32ArrayType::CreateArray(10, "Spaced Array");
    array3->initializeWithValue(2, 0);

    am1->addAttributeArray("InputArray1", array1);
    am1->addAttributeArray("InputArray2", array2);
    am1->addAttributeArray("Spaced Array", array3);
    dc->addAttributeMatrix("AttributeMatrix", am1);
    dc->addAttributeMatrix("NumericMatrix", am2);
    dca->addDataContainer(dc);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  AbstractFilter::Pointer createArrayCalculatorFilter(DataArrayPath calculatedPath)
  {
    QString filtName = "ArrayCalculator";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != NULL);

    AbstractFilter::Pointer filter = factory->create();
    DREAM3D_REQUIRE(filter.get() != NULL);

    QVariant var;
    bool propWasSet = false;

    // Set the calculated array using the NumericMatrix path
    var.setValue(calculatedPath);
    propWasSet = filter->setProperty("CalculatedArray", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    // Set the selected attribute matrix
    DataArrayPath selectedAMPath = DataArrayPath("DataContainer", "AttributeMatrix", "");
    var.setValue(selectedAMPath);
    propWasSet = filter->setProperty("SelectedAttributeMatrix", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    DataContainerArray::Pointer dca = createDataContainerArray();
    filter->setDataContainerArray(dca);

    return filter;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void runTest(QString equation, DataArrayPath targetArrayPath, int expectedErrorCondition, int expectedWarningCondition, int* expectedNumberOfTuples = NULL, double* expectedValue = NULL, ArrayCalculator::AngleUnits units = ArrayCalculator::Radians)
  {
    bool propWasSet = false;

    AbstractFilter::Pointer filter = createArrayCalculatorFilter(targetArrayPath);

    propWasSet = filter->setProperty("InfixEquation", equation);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);
    propWasSet = filter->setProperty("Units", units);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), expectedErrorCondition);
    DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), expectedWarningCondition);

    DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), targetArrayPath);

    if (NULL != expectedNumberOfTuples)
    {
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
    }

    if (NULL != expectedValue && NULL != expectedNumberOfTuples)
    {
      int expectedTuples = *expectedNumberOfTuples;
      double value = *expectedValue;

      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);

      for (int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        double val1 = arrayPtr->getValue(i);
        DREAM3D_REQUIRE(SIMPLibMath::closeEnough<double>(arrayPtr->getValue(i), value, 0.01) == true);
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestArrayCalculatorFilter()
  {
    DataArrayPath numericArrayPath("DataContainer", "NumericMatrix", "NewArray");
    DataArrayPath arrayPath("DataContainer", "AttributeMatrix", "NewArray");

    QVariant var;
    bool propWasSet = false;

    // Empty Tests
    {
      runTest("", numericArrayPath, ArrayCalculator::EMPTY_EQUATION, 0);
      runTest("          ", numericArrayPath, ArrayCalculator::EMPTY_EQUATION, 0);
    }

    // Single Value Tests
    {
      int numTuple = 1;
      double value = -3;
      runTest("-3", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      numTuple = 1;
      value = 14;
      runTest("14", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      numTuple = 1;
      value = 0.345;
      runTest(".345", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Mismatched Parentheses Tests
    {
      int numTuple = 1;
      double value = 12;
      runTest("(3*4)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
      runTest("(3*4", numericArrayPath, ArrayCalculator::MISMATCHED_PARENTHESES, ArrayCalculator::NUMERIC_VALUE_WARNING);
      runTest("3*4)", numericArrayPath, ArrayCalculator::MISMATCHED_PARENTHESES, ArrayCalculator::NUMERIC_VALUE_WARNING);
    }

    // Nested Unary Operator Test
    {
      int numTuple = 1;
      double value = sin(pow(fabs(cos(fabs(static_cast<double>(3)) / 4) + 7), 2));
      runTest("sin( abs( cos( abs(3)/4) + 7)^2)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Single Array Tests (Force "Incorrect Tuple Count" Error)
    {
      runTest("-InputArray1", numericArrayPath, ArrayCalculator::INCORRECT_TUPLE_COUNT, 0);
      runTest("InputArray2", numericArrayPath, ArrayCalculator::INCORRECT_TUPLE_COUNT, 0);
    }

    // Unrecognized Item Tests
    {
      runTest("-foo", numericArrayPath, ArrayCalculator::UNRECOGNIZED_ITEM, 0);
      runTest("InputArray3", numericArrayPath, ArrayCalculator::UNRECOGNIZED_ITEM, 0);
      runTest("sin(InputArray 2)", numericArrayPath, ArrayCalculator::UNRECOGNIZED_ITEM, 0);
    }

    // Single Array Tests
    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      FloatArrayType::Pointer inputArray1 = filter->getDataContainerArray()->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(filter.get(),
        DataArrayPath("DataContainer", "AttributeMatrix", "InputArray1"));

      propWasSet = filter->setProperty("InfixEquation", "-InputArray1");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == inputArray1->getNumberOfTuples());
      for (int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == inputArray1->getValue(i) * -1);
      }
    }
    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      UInt32ArrayType::Pointer inputArray2 = filter->getDataContainerArray()->getPrereqIDataArrayFromPath<UInt32ArrayType, AbstractFilter>(filter.get(),
        DataArrayPath("DataContainer", "AttributeMatrix", "InputArray2"));

      propWasSet = filter->setProperty("InfixEquation", "InputArray2");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == inputArray2->getNumberOfTuples());
      for (int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == inputArray2->getValue(i));
      }
    }

    // Multiple Array Tests
    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      FloatArrayType::Pointer inputArray1 = filter->getDataContainerArray()->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(filter.get(),
        DataArrayPath("DataContainer", "AttributeMatrix", "InputArray1"));

      UInt32ArrayType::Pointer spacedArray = filter->getDataContainerArray()->getPrereqIDataArrayFromPath<UInt32ArrayType, AbstractFilter>(filter.get(),
        DataArrayPath("DataContainer", "AttributeMatrix", "Spaced Array"));

      propWasSet = filter->setProperty("InfixEquation", "Spaced Array + InputArray1");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), 0);
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == spacedArray->getNumberOfTuples());
      for (int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == inputArray1->getValue(i) + spacedArray->getValue(i));
      }
    }
    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      FloatArrayType::Pointer inputArray1 = filter->getDataContainerArray()->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(filter.get(),
        DataArrayPath("DataContainer", "AttributeMatrix", "InputArray1"));

      UInt32ArrayType::Pointer inputArray2 = filter->getDataContainerArray()->getPrereqIDataArrayFromPath<UInt32ArrayType, AbstractFilter>(filter.get(),
        DataArrayPath("DataContainer", "AttributeMatrix", "InputArray2"));

      propWasSet = filter->setProperty("InfixEquation", "sqrt((InputArray1^2)+(InputArray2^2))");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), 0);
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == inputArray2->getNumberOfTuples());
      for (int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        double value = pow(inputArray1->getValue(i), 2) + pow(inputArray2->getValue(i), 2);
        value = sqrt(value);
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == value);
      }
    }

    // Operator Tests

    // Addition Operator
    {
      runTest("+", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("3 +", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("+ 12.5", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 18;
      runTest("12 + 6", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -6;
      runTest("-12 + 6", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
      runTest("6 + -12", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Subtraction Operator
    {
      runTest("-", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("-89.2 -", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = -9;
      runTest("- 9", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 43;
      runTest("97 - 54", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -34;
      runTest("-32 - 2", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 19;
      runTest("7 - -12", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Multiplication Operator
    {
      runTest("*", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("3 *", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("* 12.5", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 72;
      runTest("12 * 6", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -72;
      runTest("-12 * 6", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
      runTest("6 * -12", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Division Operator
    {
      runTest("/", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("3 /", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("/ 12.5", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 2;
      runTest("12 / 6", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -2;
      runTest("-12 / 6", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -0.5;
      runTest("6 / -12", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Pow Operator
    {
      runTest("^", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("3 ^", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("^ 12.5", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 125;
      runTest("5 ^ 3", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -8;
      runTest("-2 ^ 3", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 0.25;
      runTest("2 ^ -2", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Abs Operator
    {
      runTest("abs", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("abs(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("abs)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("abs()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 2;
      runTest("abs(2)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 4.3;
      runTest("abs(-4.3)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 6.7;
      runTest("abs(abs(6.7))", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Sin Operator
    {
      runTest("sin", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("sin(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("sin)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("sin()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 1;
      runTest("sin(90)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 0;
      runTest("sin(-180)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 0.5;
      runTest("sin(" + QString::number(M_PI) + "/6)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);

      value = 1;
      runTest("sin(" + QString::number(M_PI) + "/2)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);
    }

    // Cos Operator
    {
      runTest("cos", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("cos(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("cos)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("cos()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 0;
      runTest("cos(90)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = -1;
      runTest("cos(-180)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 0.5;
      runTest("cos(" + QString::number(M_PI) + "/3)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);

      value = -0.5;
      runTest("cos(2*" + QString::number(M_PI) + "/3)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);
    }

    // Tan Operator
    {
      runTest("tan", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("tan(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("tan)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("tan()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 1;
      runTest("tan(45)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = sqrt(3);
      runTest("tan(60)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 1;
      runTest("tan(" + QString::number(M_PI) + "/4)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);

      value = -sqrt(static_cast<double>(1) / static_cast<double>(3));
      runTest("tan(5*" + QString::number(M_PI) + "/6)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);
    }

    // ASin Operator
    {
      runTest("asin", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("asin(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("asin)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("asin()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 30;
      runTest("asin(0.5)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 45;
      runTest("asin(sqrt(2)/2)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = M_PI / 3;
      runTest("asin(sqrt(3)/2)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);

      value = M_PI / 2;
      runTest("asin(1)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);
    }

    // ACos Operator
    {
      runTest("acos", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("acos(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("acos)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("acos()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 60;
      runTest("acos(0.5)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 45;
      runTest("acos(sqrt(2)/2)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = M_PI / 6;
      runTest("acos(sqrt(3)/2)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);

      value = 0;
      runTest("acos(1)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);
    }

    // ATan Operator
    {
      runTest("atan", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("atan(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("atan)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("atan()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = -45;
      runTest("atan(-1)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = -60;
      runTest("atan(-sqrt(3))", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = M_PI / 6;
      runTest("atan(1/sqrt(3))", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);

      value = M_PI / 3;
      runTest("atan(sqrt(3))", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);
    }

    // Sqrt Operator
    {
      runTest("sqrt", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("sqrt(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("sqrt)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("sqrt()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("sqrt(1, 3)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 3;
      runTest("sqrt(9)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 4;
      runTest("sqrt(4*4)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 3;
      runTest("sqrt(3^2)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Root Operator
    {
      runTest("root", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("root(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("root)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("root()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("root(1)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("root(,)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 3;
      runTest("root(9, 2)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 4;
      runTest("root(4*4, 2)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 4;
      runTest("root(64, 3)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Log10 Operator
    {
      runTest("log10", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("log10(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("log10)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("log10()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("log10(1, 3)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("log10(,)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = log10(10);
      runTest("log10(10)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = log10(40);
      runTest("log10(40)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Log Operator
    {
      runTest("log", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("log(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("log)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("log()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("log(1)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("log(,)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = log(5) / log(2);
      runTest("log(2, 5)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 2;
      runTest("log(10, 100)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Exp Operator
    {
      runTest("exp", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("exp(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("exp)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("exp()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("exp(1, 5)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("exp(,)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = M_E;
      runTest("exp(1)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 1;
      runTest("exp(0)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Ln Operator
    {
      runTest("ln", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("ln(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("ln)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("ln()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("ln(1, 5)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("ln(,)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = log(1);
      runTest("ln(1)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = log(7);
      runTest("ln(7)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Floor Operator
    {
      runTest("floor", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("floor(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("floor)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("floor()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("floor(1, 5)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("floor(,)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 12;
      runTest("floor(12.4564)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -83;
      runTest("floor(-82.789367)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Ceil Operator
    {
      runTest("ceil", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("ceil(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("ceil)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("ceil()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("ceil(1, 5)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("ceil(,)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = 1;
      runTest("ceil(.4564)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -82;
      runTest("ceil(-82.789367)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Negative Operator
    {
      runTest("-", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("-(", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("-)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("-()", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("-(1, 5)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);
      runTest("-(,)", numericArrayPath, ArrayCalculator::INVALID_EQUATION, 0);

      int numTuple = 1;
      double value = -0.4564;
      runTest("-(.4564)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 1;
      runTest("-(3-4)", numericArrayPath, 0, ArrayCalculator::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ArrayCalculatorTest Starting ####" << std::endl;
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestArrayCalculatorFilter())
  }

private:
  ArrayCalculatorTest(const ArrayCalculatorTest&); // Copy Constructor Not Implemented
  void operator=(const ArrayCalculatorTest&); // Operator '=' Not Implemented
};

