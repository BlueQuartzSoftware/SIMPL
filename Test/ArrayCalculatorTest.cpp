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

#include <math.h>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
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

    am1->addAttributeArray("InputArray1", array1);
    am1->addAttributeArray("InputArray2", array2);
    dc->addAttributeMatrix("AttributeMatrix", am1);
    dc->addAttributeMatrix("NumericMatrix", am2);
    dca->addDataContainer(dc);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestArrayCalculatorFilter()
  {
    QString filtName = "ArrayCalculator";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != NULL);

    AbstractFilter::Pointer filter = factory->create();
    DREAM3D_REQUIRE(filter.get() != NULL);

    DataContainerArray::Pointer dca = createDataContainerArray();
    filter->setDataContainerArray(dca);

    FloatArrayType::Pointer inputArray1 = dca->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(filter.get(),
      DataArrayPath("DataContainer", "AttributeMatrix", "InputArray1"));

    UInt32ArrayType::Pointer inputArray2 = dca->getPrereqIDataArrayFromPath<UInt32ArrayType, AbstractFilter>(filter.get(), 
      DataArrayPath("DataContainer", "AttributeMatrix", "InputArray2"));

    QVariant var;
    bool propWasSet = false;

    // Set the calculated array using the NumericMatrix path
    DataArrayPath calculatedArrayPath = DataArrayPath("DataContainer", "NumericMatrix", "NewArray");
    var.setValue(calculatedArrayPath);
    propWasSet = filter->setProperty("CalculatedArray", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    // Set the selected attribute matrix
    DataArrayPath selectedAMPath = DataArrayPath("DataContainer", "AttributeMatrix", "");
    var.setValue(selectedAMPath);
    propWasSet = filter->setProperty("SelectedAttributeMatrix", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    {
      // Empty Tests
      propWasSet = filter->setProperty("InfixEquation", "");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::EMPTY_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "          ");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::EMPTY_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // Single Value Tests
      propWasSet = filter->setProperty("InfixEquation", "-3");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      DoubleArrayType::Pointer arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == -3);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "14");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == 14);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // Mismatched Parentheses Tests
      propWasSet = filter->setProperty("InfixEquation", "(3*4)");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == 12);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "(3*4");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::MISMATCHED_PARENTHESES);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "3*4)");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::MISMATCHED_PARENTHESES);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // Nested Unary Operator Tests
      propWasSet = filter->setProperty("InfixEquation", "abs( abs( abs( abs(3)/4) + 7.34)^2)");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "abs( cos( log10( abs(3)/4) - 1)^2)*5");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // Single Array Tests (Force "Incorrect Tuple Count" Error)
      propWasSet = filter->setProperty("InfixEquation", "-InputArray1");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INCORRECT_TUPLE_COUNT);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "InputArray2");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INCORRECT_TUPLE_COUNT);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // Set the calculated array path
      calculatedArrayPath = DataArrayPath("DataContainer", "AttributeMatrix", "NewArray");
      var.setValue(calculatedArrayPath);
      propWasSet = filter->setProperty("CalculatedArray", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      // Unrecognized Item Tests
      propWasSet = filter->setProperty("InfixEquation", "-foo");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::UNRECOGNIZED_ITEM);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "InputArray3");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::UNRECOGNIZED_ITEM);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "sin(InputArray 2)");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::UNRECOGNIZED_ITEM);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // Single Array Tests
      propWasSet = filter->setProperty("InfixEquation", "-InputArray1");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == inputArray1->getNumberOfTuples());
      for (int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == static_cast<double>(inputArray1->getValue(i) * -1));
      }

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "InputArray2");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == inputArray2->getNumberOfTuples());
      for (int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == static_cast<double>(inputArray2->getValue(i)));
      }

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // Multiple Array Tests
      propWasSet = filter->setProperty("InfixEquation", "InputArray1 + InputArray2");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), 0);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == inputArray2->getNumberOfTuples());
      for (int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == static_cast<double>(inputArray1->getValue(i)) + static_cast<double>(inputArray2->getValue(i)));
      }

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "sqrt((InputArray1^2)+(InputArray2^2))");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), 0);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == inputArray2->getNumberOfTuples());
      for (int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        double value = pow(static_cast<double>(inputArray1->getValue(i)), 2) + pow(static_cast<double>(inputArray2->getValue(i)), 2);
        value = sqrt(value);
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == value);
      }

      // Operator Tests

      // Set the calculated array path
      calculatedArrayPath = DataArrayPath("DataContainer", "NumericMatrix", "NewArray");
      var.setValue(calculatedArrayPath);
      propWasSet = filter->setProperty("CalculatedArray", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // Addition Operator
      propWasSet = filter->setProperty("InfixEquation", "+");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INVALID_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "3 +");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INVALID_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "+ 12.5");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INVALID_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "12 + 6");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == 18);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "-12 + 6");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == -6);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "6 + -12");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == -6);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // Subtraction Operator
      propWasSet = filter->setProperty("InfixEquation", "-");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::NO_NUMERIC_VALUES);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "-89.2 -");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INVALID_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "- 9");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);    // This one doesn't fail because the filter treats the minus sign as a negative sign
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == -9);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "97 - 54");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == 43);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "-32 - 2");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == -34);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "7 - -12");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == 19);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // Multiplication Operator
      propWasSet = filter->setProperty("InfixEquation", "*");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INVALID_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "3 *");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INVALID_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "* 12.5");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INVALID_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "12 * 6");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == 72);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "-12 * 6");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == -72);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "6 * -12");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == -72);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // Division Operator
      propWasSet = filter->setProperty("InfixEquation", "/");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INVALID_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "3 /");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INVALID_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "/ 12.5");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), ArrayCalculator::INVALID_EQUATION);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "12 / 6");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == 2);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "-12 / 6");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == -2);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      propWasSet = filter->setProperty("InfixEquation", "6 / -12");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCondition(), ArrayCalculator::NUMERIC_VALUE_WARNING);
      arrayPtr = dca->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), calculatedArrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
      DREAM3D_REQUIRE(arrayPtr->getValue(0) == -0.5);

      dca = createDataContainerArray();
      filter->setDataContainerArray(dca);

      // 1. checkValidity for NegativeOperator (doesn't really fit unary or binary operator functions)
      // 2. Write operator tests

      //PowOperator
      //ABSOperator
      //SinOperator
      //CosOperator
      //TanOperator
      //ASinOperator
      //ACosOperator
      //ATanOperator
      //SqrtOperator
      //RootOperator
      //Log10Operator
      //LogOperator
      //ExpOperator
      //LnOperator
      //FloorOperator
      //CeilOperator
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

