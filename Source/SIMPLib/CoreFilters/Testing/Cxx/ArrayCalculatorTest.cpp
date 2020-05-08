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
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/CoreFilters/ArrayCalculator.h"
#include "SIMPLib/CoreFilters/CreateDataArray.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"
#include "SIMPLib/CoreFilters/util/CalculatorOperator.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

class DummyObserver : public Observer
{
public:
  DummyObserver() = default;
  ~DummyObserver() override = default;

  /**
   * @brief Returns the name of the class for DummyObserver
   */
  QString getNameOfClass() const
  {
    return QString("DummyObserver");
  }

  /**
   * @brief Returns the name of the class for DummyObserver
   */
  QString ClassName()
  {
    return QString("DummyObserver");
  }

  void processPipelineMessage(const AbstractMessage::Pointer& pm) override
  {
    // Don't do anything...this class only exists to stifle the messages being dumped to the console
  }

public:
  DummyObserver(const DummyObserver&) = delete;            // Copy Constructor Not Implemented
  DummyObserver(DummyObserver&&) = delete;                 // Move Constructor Not Implemented
  DummyObserver& operator=(const DummyObserver&) = delete; // Copy Assignment Not Implemented
  DummyObserver& operator=(DummyObserver&&) = delete;      // Move Assignment Not Implemented
};

class ArrayCalculatorTest
{
public:
  ArrayCalculatorTest() = default;
  virtual ~ArrayCalculatorTest() = default;
  /**
   * @brief Returns the name of the class for ArrayCalculatorTest
   */
  /**
   * @brief Returns the name of the class for ArrayCalculatorTest
   */
  QString getNameOfClass() const
  {
    return QString("ArrayCalculatorTest");
  }

  /**
   * @brief Returns the name of the class for ArrayCalculatorTest
   */
  QString ClassName()
  {
    return QString("ArrayCalculatorTest");
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer dc = DataContainer::New("DataContainer");
    AttributeMatrix::Pointer am1 = AttributeMatrix::New(std::vector<size_t>(1, 10), "AttributeMatrix", AttributeMatrix::Type::Cell);
    AttributeMatrix::Pointer am2 = AttributeMatrix::New(std::vector<size_t>(1, 1), "NumericMatrix", AttributeMatrix::Type::Cell);
    FloatArrayType::Pointer array1 = FloatArrayType::CreateArray(10, "InputArray1", true);
    array1->initializeWithValue(-12);
    UInt32ArrayType::Pointer array2 = UInt32ArrayType::CreateArray(10, "InputArray2", true);
    array2->initializeWithValue(10);
    UInt32ArrayType::Pointer sArray = UInt32ArrayType::CreateArray(10, "Spaced Array", true);
    sArray->initializeWithValue(2);

    UInt32ArrayType::Pointer mcArray1 = UInt32ArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 3), "MultiComponent Array1", true);
    int num = 0;
    for(int i = 0; i < mcArray1->getNumberOfTuples() * mcArray1->getNumberOfComponents(); i++)
    {
      mcArray1->setValue(i, num);
      num++;
    }

    UInt32ArrayType::Pointer mcArray2 = UInt32ArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 3), "MultiComponent Array2", true);
    num = 0;
    for(int i = 0; i < mcArray2->getNumberOfTuples() * mcArray2->getNumberOfComponents(); i++)
    {
      mcArray2->setValue(i, num);
      num++;
    }

    IDataArray::Pointer numberArray = mcArray2->deepCopy();
    numberArray->setName("4");

    IDataArray::Pointer signArray = mcArray1->deepCopy();
    signArray->setName("*");

    am1->insertOrAssign(array1);
    am1->insertOrAssign(array2);
    am1->insertOrAssign(sArray);
    am1->insertOrAssign(mcArray1);
    am1->insertOrAssign(mcArray2);
    am1->insertOrAssign(numberArray);
    am1->insertOrAssign(signArray);
    dc->addOrReplaceAttributeMatrix(am1);
    dc->addOrReplaceAttributeMatrix(am2);
    dca->addOrReplaceDataContainer(dc);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  AbstractFilter::Pointer createArrayCalculatorFilter(const DataArrayPath& calculatedPath)
  {
    QString filtName = "ArrayCalculator";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr);

    AbstractFilter::Pointer filter = factory->create();
    DREAM3D_REQUIRE(filter.get() != nullptr);

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
  void runTest(const QString& equation, const DataArrayPath& targetArrayPath, CalculatorItem::ErrorCode expectedErrorCondition, CalculatorItem::WarningCode expectedWarningCondition,
               const int* expectedNumberOfTuples = nullptr, const double* expectedValue = nullptr, ArrayCalculator::AngleUnits units = ArrayCalculator::Radians)
  {
    std::cout << "  Testing equation: ==>" << equation.toStdString() << "<==" << std::endl;

    bool propWasSet = false;

    AbstractFilter::Pointer filter = createArrayCalculatorFilter(targetArrayPath);

    propWasSet = filter->setProperty("InfixEquation", equation);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);
    propWasSet = filter->setProperty("Units", units);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);
    filter->execute();
    if(filter->getErrorCode() < 0 && expectedErrorCondition == CalculatorItem::ErrorCode::SUCCESS)
    {
      std::cout << "    Error " << filter->getErrorCode() << " executing filter." << std::endl;
    }

    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(expectedErrorCondition));
    DREAM3D_REQUIRE_EQUAL(filter->getWarningCode(), static_cast<int>(expectedWarningCondition));

    DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(filter.get(), targetArrayPath);

    if(nullptr != expectedNumberOfTuples)
    {
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);
    }

    if(nullptr != expectedValue && nullptr != expectedNumberOfTuples)
    {
      double value = *expectedValue;

      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == 1);

      for(size_t i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        DREAM3D_REQUIRED(SIMPLibMath::closeEnough<double>(arrayPtr->getValue(i), value, 0.01), ==, true);
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void MultiComponentArrayCalculatorTest()
  {
    DataArrayPath numericArrayPath("DataContainer", "NumericMatrix", "NewArray");
    DataArrayPath arrayPath("DataContainer", "AttributeMatrix", "NewArray");

    bool propWasSet = false;

    // Multi-Component Array Tests
    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      UInt32ArrayType::Pointer mcArray1 =
          filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array1"));

      UInt32ArrayType::Pointer mcArray2 =
          filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array2"));

      propWasSet = filter->setProperty("InfixEquation", "MultiComponent Array1 + MultiComponent Array2");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(CalculatorItem::ErrorCode::SUCCESS));
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == mcArray1->getNumberOfTuples());
      DREAM3D_REQUIRE(arrayPtr->getNumberOfComponents() == mcArray1->getNumberOfComponents());
      for(int t = 0; t < arrayPtr->getNumberOfTuples(); t++)
      {
        for(int c = 0; c < arrayPtr->getNumberOfComponents(); c++)
        {
          int index = arrayPtr->getNumberOfComponents() * t + c;
          DREAM3D_REQUIRE(arrayPtr->getValue(index) == mcArray1->getValue(index) + mcArray2->getValue(index));
        }
      }
    }

    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      UInt32ArrayType::Pointer mcArray1 =
          filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array1"));

      UInt32ArrayType::Pointer mcArray2 =
          filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array2"));

      propWasSet = filter->setProperty("InfixEquation", "MultiComponent Array1[1] + MultiComponent Array2[0]");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(CalculatorItem::ErrorCode::SUCCESS));
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == mcArray1->getNumberOfTuples());
      DREAM3D_REQUIRE(arrayPtr->getNumberOfComponents() == 1);
      for(int t = 0; t < arrayPtr->getNumberOfTuples(); t++)
      {
        int index1 = mcArray1->getNumberOfComponents() * t + 1;
        int index2 = mcArray2->getNumberOfComponents() * t + 0;
        int arrayIndex = arrayPtr->getNumberOfComponents() * t + 0;
        DREAM3D_REQUIRE(arrayPtr->getValue(arrayIndex) == mcArray1->getValue(index1) + mcArray2->getValue(index2));
      }
    }

    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      UInt32ArrayType::Pointer nArray = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "4"));

      UInt32ArrayType::Pointer sArray = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "*"));

      propWasSet = filter->setProperty("InfixEquation", "\"4\" + 2");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(CalculatorItem::ErrorCode::SUCCESS));
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == nArray->getNumberOfTuples());
      DREAM3D_REQUIRE(arrayPtr->getNumberOfComponents() == nArray->getNumberOfComponents());
      for(int t = 0; t < arrayPtr->getNumberOfTuples(); t++)
      {
        for(int c = 0; c < arrayPtr->getNumberOfComponents(); c++)
        {
          int index = nArray->getNumberOfComponents() * t + c;
          DREAM3D_REQUIRE(arrayPtr->getValue(index) == nArray->getValue(index) + 2);
        }
      }
    }

    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      UInt32ArrayType::Pointer nArray = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "4"));

      UInt32ArrayType::Pointer sArray = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "*"));

      propWasSet = filter->setProperty("InfixEquation", "\"4\" + \"*\"");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(CalculatorItem::ErrorCode::SUCCESS));
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == nArray->getNumberOfTuples());
      DREAM3D_REQUIRE(arrayPtr->getNumberOfComponents() == nArray->getNumberOfComponents());
      for(int t = 0; t < arrayPtr->getNumberOfTuples(); t++)
      {
        for(int c = 0; c < arrayPtr->getNumberOfComponents(); c++)
        {
          int index = nArray->getNumberOfComponents() * t + c;
          DREAM3D_REQUIRE(arrayPtr->getValue(index) == nArray->getValue(index) + sArray->getValue(index));
        }
      }
    }

    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      UInt32ArrayType::Pointer nArray = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "4"));

      UInt32ArrayType::Pointer sArray = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "*"));

      propWasSet = filter->setProperty("InfixEquation", "\"4\"[0] + \"*\"[1]");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(CalculatorItem::ErrorCode::SUCCESS));
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == nArray->getNumberOfTuples());
      DREAM3D_REQUIRE(arrayPtr->getNumberOfComponents() == 1);
      for(int t = 0; t < arrayPtr->getNumberOfTuples(); t++)
      {
        int nIndex = nArray->getNumberOfComponents() * t + 0;
        int sIndex = sArray->getNumberOfComponents() * t + 1;
        DREAM3D_REQUIRE(arrayPtr->getValue(t) == nArray->getValue(nIndex) + sArray->getValue(sIndex));
      }
    }

    // Inconsistent indexing
    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      UInt32ArrayType::Pointer nArray = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "4"));

      UInt32ArrayType::Pointer sArray = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "*"));

      propWasSet = filter->setProperty("InfixEquation", "\"4\" + \"*\"[1]");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(CalculatorItem::ErrorCode::INCONSISTENT_COMP_DIMS));
    }

    // Out of bounds error
    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      UInt32ArrayType::Pointer nArray = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "4"));

      UInt32ArrayType::Pointer sArray = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "*"));

      propWasSet = filter->setProperty("InfixEquation", "\"4\"[0] + \"*\"[3]");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(CalculatorItem::ErrorCode::COMPONENT_OUT_OF_RANGE));
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void SingleComponentArrayCalculatorTest()
  {
    DataArrayPath numericArrayPath("DataContainer", "NumericMatrix", "NewArray");
    DataArrayPath arrayPath("DataContainer", "AttributeMatrix", "NewArray");

    bool propWasSet = false;

    // Empty Tests
    {
      runTest("", numericArrayPath, CalculatorItem::ErrorCode::EMPTY_EQUATION, CalculatorItem::WarningCode::NONE);
      runTest("          ", numericArrayPath, CalculatorItem::ErrorCode::EMPTY_EQUATION, CalculatorItem::WarningCode::NONE);
    }

    // Single Value Tests
    {
      int numTuple = 1;
      double value = -3;
      runTest("-3", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      numTuple = 1;
      value = 14;
      runTest("14", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      numTuple = 1;
      value = 0.345;
      runTest(".345", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Mismatched Parentheses Tests
    {
      int numTuple = 1;
      double value = 12;
      runTest("(3*4)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
      runTest("(3*4", numericArrayPath, CalculatorItem::ErrorCode::MISMATCHED_PARENTHESES, CalculatorItem::WarningCode::NONE);
      runTest("3*4)", numericArrayPath, CalculatorItem::ErrorCode::MISMATCHED_PARENTHESES, CalculatorItem::WarningCode::NONE);
    }

    // Nested Unary Operator Test
    {
      int numTuple = 1;
      double value = sin(pow(fabs(cos(fabs(static_cast<double>(3)) / 4) + 7), 2));
      runTest("sin( abs( cos( abs(3)/4) + 7)^2)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Single Array Tests (Force "Incorrect Tuple Count" Error)
    {
      runTest("-InputArray1", numericArrayPath, CalculatorItem::ErrorCode::INCORRECT_TUPLE_COUNT, CalculatorItem::WarningCode::NONE);
      runTest("InputArray2", numericArrayPath, CalculatorItem::ErrorCode::INCORRECT_TUPLE_COUNT, CalculatorItem::WarningCode::NONE);
    }

    // Unrecognized Item Tests
    {
      runTest("-foo", numericArrayPath, CalculatorItem::ErrorCode::UNRECOGNIZED_ITEM, CalculatorItem::WarningCode::NONE);
      runTest("InputArray3", numericArrayPath, CalculatorItem::ErrorCode::UNRECOGNIZED_ITEM, CalculatorItem::WarningCode::NONE);
      runTest("sin(InputArray 2)", numericArrayPath, CalculatorItem::ErrorCode::UNRECOGNIZED_ITEM, CalculatorItem::WarningCode::NONE);
    }

    // Single Array Tests
    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      FloatArrayType::Pointer inputArray1 = filter->getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "InputArray1"));

      propWasSet = filter->setProperty("InfixEquation", "-InputArray1");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(CalculatorItem::ErrorCode::SUCCESS));
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == inputArray1->getNumberOfTuples());
      for(int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == inputArray1->getValue(i) * -1);
      }
    }
    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      UInt32ArrayType::Pointer inputArray2 = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "InputArray2"));

      propWasSet = filter->setProperty("InfixEquation", "InputArray2");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(CalculatorItem::ErrorCode::SUCCESS));
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == inputArray2->getNumberOfTuples());
      for(int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == inputArray2->getValue(i));
      }
    }

    // Multiple Array Tests
    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      FloatArrayType::Pointer inputArray1 = filter->getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "InputArray1"));

      UInt32ArrayType::Pointer spacedArray = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "Spaced Array"));

      propWasSet = filter->setProperty("InfixEquation", "Spaced Array + InputArray1");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(CalculatorItem::ErrorCode::SUCCESS));
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCode(), static_cast<int>(CalculatorItem::WarningCode::NONE));
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == spacedArray->getNumberOfTuples());
      for(int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == inputArray1->getValue(i) + spacedArray->getValue(i));
      }
    }
    {
      AbstractFilter::Pointer filter = createArrayCalculatorFilter(arrayPath);

      FloatArrayType::Pointer inputArray1 = filter->getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "InputArray1"));

      UInt32ArrayType::Pointer inputArray2 = filter->getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "InputArray2"));

      propWasSet = filter->setProperty("InfixEquation", "sqrt((InputArray1^2)+(InputArray2^2))");
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), static_cast<int>(CalculatorItem::ErrorCode::SUCCESS));
      DREAM3D_REQUIRE_EQUAL(filter->getWarningCode(), static_cast<int>(CalculatorItem::WarningCode::NONE));
      DoubleArrayType::Pointer arrayPtr = filter->getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(filter.get(), arrayPath);
      DREAM3D_REQUIRE(arrayPtr->getNumberOfTuples() == inputArray2->getNumberOfTuples());
      for(int i = 0; i < arrayPtr->getNumberOfTuples(); i++)
      {
        double value = pow(inputArray1->getValue(i), 2) + pow(inputArray2->getValue(i), 2);
        value = sqrt(value);
        DREAM3D_REQUIRE(arrayPtr->getValue(i) == value);
      }
    }

    // Operator Tests

    // Addition Operator
    {
      runTest("+", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_LEFT_VALUE, CalculatorItem::WarningCode::NONE);
      runTest("3 +", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_RIGHT_VALUE, CalculatorItem::WarningCode::NONE);
      runTest("+ 12.5", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_LEFT_VALUE, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 18;
      runTest("12 + 6", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -6;
      runTest("-12 + 6", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
      runTest("6 + -12", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Subtraction Operator
    {
      runTest("-89.2 -", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_RIGHT_VALUE, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 43;
      runTest("97 - 54", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -34;
      runTest("-32 - 2", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 19;
      runTest("7 - -12", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Multiplication Operator
    {
      runTest("*", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_LEFT_VALUE, CalculatorItem::WarningCode::NONE);
      runTest("3 *", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_RIGHT_VALUE, CalculatorItem::WarningCode::NONE);
      runTest("* 12.5", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_LEFT_VALUE, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 72;
      runTest("12 * 6", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -72;
      runTest("-12 * 6", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
      runTest("6 * -12", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Division Operator
    {
      runTest("/", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_LEFT_VALUE, CalculatorItem::WarningCode::NONE);
      runTest("3 /", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_RIGHT_VALUE, CalculatorItem::WarningCode::NONE);
      runTest("/ 12.5", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_LEFT_VALUE, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 2;
      runTest("12 / 6", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -2;
      runTest("-12 / 6", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -0.5;
      runTest("6 / -12", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Pow Operator
    {
      runTest("^", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_LEFT_VALUE, CalculatorItem::WarningCode::NONE);
      runTest("3 ^", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_RIGHT_VALUE, CalculatorItem::WarningCode::NONE);
      runTest("^ 12.5", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_LEFT_VALUE, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 125;
      runTest("5 ^ 3", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -8;
      runTest("-2 ^ 3", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 0.25;
      runTest("2 ^ -2", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Abs Operator
    {
      runTest("abs", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("abs(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("abs)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("abs()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 2;
      runTest("abs(2)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 4.3;
      runTest("abs(-4.3)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 6.7;
      runTest("abs(abs(6.7))", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Sin Operator
    {
      runTest("sin", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("sin(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("sin)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("sin()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 1;
      runTest("sin(90)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 0;
      runTest("sin(-180)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 0.5;
      runTest("sin(" + QString::number(M_PI) + "/6)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value,
              ArrayCalculator::Radians);

      value = 1;
      runTest("sin(" + QString::number(M_PI) + "/2)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value,
              ArrayCalculator::Radians);
    }

    // Cos Operator
    {
      runTest("cos", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("cos(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("cos)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("cos()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 0;
      runTest("cos(90)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = -1;
      runTest("cos(-180)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 0.5;
      runTest("cos(" + QString::number(M_PI) + "/3)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value,
              ArrayCalculator::Radians);

      value = -0.5;
      runTest("cos(2*" + QString::number(M_PI) + "/3)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value,
              ArrayCalculator::Radians);
    }

    // Tan Operator
    {
      runTest("tan", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("tan(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("tan)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("tan()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 1;
      runTest("tan(45)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = sqrt(3);
      runTest("tan(60)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 1;
      runTest("tan(" + QString::number(M_PI) + "/4)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value,
              ArrayCalculator::Radians);

      value = -sqrt(static_cast<double>(1) / static_cast<double>(3));
      runTest("tan(5*" + QString::number(M_PI) + "/6)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value,
              ArrayCalculator::Radians);
    }

    // ASin Operator
    {
      runTest("asin", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("asin(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("asin)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("asin()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 30;
      runTest("asin(0.5)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 45;
      runTest("asin(sqrt(2)/2)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = M_PI / 3;
      runTest("asin(sqrt(3)/2)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);

      value = M_PI / 2;
      runTest("asin(1)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);
    }

    // ACos Operator
    {
      runTest("acos", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("acos(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("acos)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("acos()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 60;
      runTest("acos(0.5)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = 45;
      runTest("acos(sqrt(2)/2)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = M_PI / 6;
      runTest("acos(sqrt(3)/2)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);

      value = 0;
      runTest("acos(1)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);
    }

    // ATan Operator
    {
      runTest("atan", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("atan(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("atan)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("atan()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = -45;
      runTest("atan(-1)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = -60;
      runTest("atan(-sqrt(3))", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Degrees);

      value = M_PI / 6;
      runTest("atan(1/sqrt(3))", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);

      value = M_PI / 3;
      runTest("atan(sqrt(3))", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value, ArrayCalculator::Radians);
    }

    // Sqrt Operator
    {
      runTest("sqrt", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("sqrt(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("sqrt)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("sqrt()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("sqrt(1, 3)", numericArrayPath, CalculatorItem::ErrorCode::TOO_MANY_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 3;
      runTest("sqrt(9)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 4;
      runTest("sqrt(4*4)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 3;
      runTest("sqrt(3^2)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Root Operator
    {
      runTest("root", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("root(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("root)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("root()", numericArrayPath, CalculatorItem::ErrorCode::NOT_ENOUGH_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("root(1)", numericArrayPath, CalculatorItem::ErrorCode::NOT_ENOUGH_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("root(,)", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 3;
      runTest("root(9, 2)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 4;
      runTest("root(4*4, 2)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 4;
      runTest("root(4*4+0, 1*2+0)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 4;
      runTest("root(64, 3)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Log10 Operator
    {
      runTest("log10", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("log10(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("log10)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("log10()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("log10(1, 3)", numericArrayPath, CalculatorItem::ErrorCode::TOO_MANY_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("log10(,)", numericArrayPath, CalculatorItem::ErrorCode::TOO_MANY_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = log10(10);
      runTest("log10(10)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = log10(40);
      runTest("log10(40)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Log Operator
    {
      runTest("log", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("log(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("log)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("log()", numericArrayPath, CalculatorItem::ErrorCode::NOT_ENOUGH_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("log(1)", numericArrayPath, CalculatorItem::ErrorCode::NOT_ENOUGH_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("log(,)", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = log(5) / log(2);
      runTest("log(2, 5)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 2;
      runTest("log(10, 100)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Exp Operator
    {
      runTest("exp", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("exp(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("exp)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("exp()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("exp(1, 5)", numericArrayPath, CalculatorItem::ErrorCode::TOO_MANY_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("exp(,)", numericArrayPath, CalculatorItem::ErrorCode::TOO_MANY_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = M_E;
      runTest("exp(1)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 1;
      runTest("exp(0)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Ln Operator
    {
      runTest("ln", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("ln(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("ln)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("ln()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("ln(1, 5)", numericArrayPath, CalculatorItem::ErrorCode::TOO_MANY_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("ln(,)", numericArrayPath, CalculatorItem::ErrorCode::TOO_MANY_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = log(1);
      runTest("ln(1)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = log(7);
      runTest("ln(7)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Floor Operator
    {
      runTest("floor", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("floor(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("floor)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("floor()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("floor(1, 5)", numericArrayPath, CalculatorItem::ErrorCode::TOO_MANY_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("floor(,)", numericArrayPath, CalculatorItem::ErrorCode::TOO_MANY_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 12;
      runTest("floor(12.4564)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -83;
      runTest("floor(-82.789367)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Ceil Operator
    {
      runTest("ceil", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("ceil(", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_CLOSING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("ceil)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_OPENING_PAREN, CalculatorItem::WarningCode::NONE);
      runTest("ceil()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("ceil(1, 5)", numericArrayPath, CalculatorItem::ErrorCode::TOO_MANY_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("ceil(,)", numericArrayPath, CalculatorItem::ErrorCode::TOO_MANY_ARGUMENTS, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = 1;
      runTest("ceil(.4564)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -82;
      runTest("ceil(-82.789367)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }

    // Negative Operator
    {
      runTest("-", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_RIGHT_VALUE, CalculatorItem::WarningCode::NONE);

      runTest("-(", numericArrayPath, CalculatorItem::ErrorCode::MISMATCHED_PARENTHESES, CalculatorItem::WarningCode::NONE);
      runTest("-)", numericArrayPath, CalculatorItem::ErrorCode::OPERATOR_NO_RIGHT_VALUE, CalculatorItem::WarningCode::NONE);
      runTest("-()", numericArrayPath, CalculatorItem::ErrorCode::NO_NUMERIC_ARGUMENTS, CalculatorItem::WarningCode::NONE);
      runTest("-(1, 5)", numericArrayPath, CalculatorItem::ErrorCode::NO_PRECEDING_UNARY_OPERATOR, CalculatorItem::WarningCode::NONE);
      runTest("-(,)", numericArrayPath, CalculatorItem::ErrorCode::NO_PRECEDING_UNARY_OPERATOR, CalculatorItem::WarningCode::NONE);

      int numTuple = 1;
      double value = -9;
      runTest("- 9", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = -0.4564;
      runTest("-(.4564)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);

      value = 1;
      runTest("-(3-4)", numericArrayPath, CalculatorItem::ErrorCode::SUCCESS, CalculatorItem::WarningCode::NUMERIC_VALUE_WARNING, &numTuple, &value);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ArrayCalculatorTest Starting ####" << std::endl;
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(SingleComponentArrayCalculatorTest())
    DREAM3D_REGISTER_TEST(MultiComponentArrayCalculatorTest())
  }

private:
  ArrayCalculatorTest(const ArrayCalculatorTest&); // Copy Constructor Not Implemented
  void operator=(const ArrayCalculatorTest&);      // Move assignment Not Implemented
};
