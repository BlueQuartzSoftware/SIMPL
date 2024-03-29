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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <cmath>

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/CoreFilters/ReadASCIIData.h"
#include "SIMPLib/CoreFilters/util/ASCIIWizardData.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

const QString DataContainerName = "DataContainer";
const QString AttributeMatrixName = "AttributeMatrix";
const QString DataArrayName = "Array1";

const std::vector<int> inputIntVector({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
const std::vector<int> inputHexVector({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A});
const std::vector<int> inputOctVector({001, 002, 003, 004, 005, 006, 007});
const std::vector<double> inputDoubleVector({1.5, 2.2, 3.65, 4.34, 5.76, 6.534, 7.0, 8.342, 9.8723, 10.89});
const std::vector<QString> inputCharErrorVector({"sdstrg", "2.2", "3.65", "&(^$#", "5.76", "sjtyr", "7.0", "8.342", "&*^#F", "youikjhgf"});
const std::vector<double> inputScientificNotation({0.15e1, 2.2e0, 3.65, 43.4e-1, 0.576e1, 653.4e-2, 7.0, 8.342, 9.8723, 10.89});

const std::vector<double> outputDoubleVector({1.5, 2.2, 3.65, 4.34, 5.76, 6.534, 7.0, 8.342, 9.8723, 10.89});
const std::vector<double> outputIntAsDoubleVector({1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0});

class ReadASCIIDataTest
{
public:
  ReadASCIIDataTest() = default;
  virtual ~ReadASCIIDataTest() = default;
  /**
   * @brief Returns the name of the class for ReadASCIIDataTest
   */
  /**
   * @brief Returns the name of the class for ReadASCIIDataTest
   */
  QString getNameOfClass() const
  {
    return QString("ReadASCIIDataTest");
  }

  /**
   * @brief Returns the name of the class for ReadASCIIDataTest
   */
  QString ClassName()
  {
    return QString("ReadASCIIDataTest");
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::ReadASCIIDataTest::TestFile1);
    QFile::remove(UnitTest::ReadASCIIDataTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the ReadASCIIData Filter from the FilterManager
    QString filtName = "ReadASCIIData";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The ReadASCIIDataTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  void CreateFile(const QString& filePath, std::vector<T> values, char /* delimiter */)
  {
    QFile data(filePath);
    if(data.open(QFile::WriteOnly))
    {
      QTextStream out(&data);

      for(size_t row = 0; row < values.size(); row++)
      {
        out << values[row];
        if(row + 1 < values.size())
        {
          out << "\n";
        }
      }
      data.close();
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  AbstractFilter::Pointer PrepFilter(ASCIIWizardData data)
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer dc = DataContainer::New(DataContainerName);
    AttributeMatrix::Pointer am = AttributeMatrix::New(std::vector<size_t>(1, data.numberOfLines), AttributeMatrixName, AttributeMatrix::Type::Cell);
    dc->addOrReplaceAttributeMatrix(am);
    dca->addOrReplaceDataContainer(dc);

    // Now instantiate the DxWriter Filter from the FilterManager
    QString filtName = "ReadASCIIData";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer importASCIIData = filterFactory->create();
      importASCIIData->preflight();

      int err = importASCIIData->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, ReadASCIIData::EMPTY_FILE)

      QVariant var;
      var.setValue(data);
      bool propWasSet = importASCIIData->setProperty("WizardData", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      importASCIIData->preflight();
      err = importASCIIData->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, ReadASCIIData::EMPTY_ATTR_MATRIX)

      importASCIIData->setDataContainerArray(dca);

      return importASCIIData;
    }

    return AbstractFilter::NullPointer();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  void ConvertType()
  {
    char delimiter = '\t';
    using DataArrayType = DataArray<T>;
    typename DataArrayType::Pointer output = DataArrayType::CreateArray(1, std::string("OutputArray"), false);
    QString outputType = output->getTypeAsString();

    ASCIIWizardData data;
    data.automaticAM = false;
    data.beginIndex = 1;
    data.consecutiveDelimiters = false;
    data.dataHeaders.push_back(DataArrayName);
    data.dataTypes.push_back(outputType);
    data.delimiters.push_back(delimiter);
    data.inputFilePath = UnitTest::ReadASCIIDataTest::TestFile1;
    data.numberOfLines = 10;
    data.selectedPath = DataArrayPath(DataContainerName, AttributeMatrixName, "");
    data.tupleDims = std::vector<size_t>(1, 10);

    // Test Using Expected Input - Double/Float
    {
      CreateFile(UnitTest::ReadASCIIDataTest::TestFile1, inputDoubleVector, delimiter);

      AbstractFilter::Pointer importASCIIData = PrepFilter(data);
      DREAM3D_REQUIRE_VALID_POINTER(importASCIIData.get())

      importASCIIData->execute();
      int err = importASCIIData->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, 0)

      AttributeMatrix::Pointer am = importASCIIData->getDataContainerArray()->getAttributeMatrix(DataArrayPath(DataContainerName, AttributeMatrixName, ""));
      typename DataArrayType::Pointer results = std::dynamic_pointer_cast<DataArrayType>(am->getAttributeArray(DataArrayName));

      DREAM3D_REQUIRE_EQUAL(results->getSize(), data.numberOfLines)

      T* resultsRaw = results->getPointer(0);
      for(size_t i = 0; i < results->getSize(); i++)
      {
        if(outputType == SIMPL::TypeNames::Double || outputType == SIMPL::TypeNames::Float)
        {
          double absValue = std::fabs(outputDoubleVector[i] - static_cast<double>(resultsRaw[i]));
          DREAM3D_REQUIRED(absValue, <, 0.01)
        }
        else
        {
          DREAM3D_REQUIRE_EQUAL(resultsRaw[i], inputIntVector[i])
        }
      }
    }

    RemoveTestFiles();

    // Test Using Expected Input - Integers
    {
      CreateFile(UnitTest::ReadASCIIDataTest::TestFile1, inputIntVector, delimiter);

      AbstractFilter::Pointer importASCIIData = PrepFilter(data);
      DREAM3D_REQUIRE_VALID_POINTER(importASCIIData.get())

      importASCIIData->execute();
      int err = importASCIIData->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, 0)

      AttributeMatrix::Pointer am = importASCIIData->getDataContainerArray()->getAttributeMatrix(DataArrayPath(DataContainerName, AttributeMatrixName, ""));
      typename DataArrayType::Pointer results = std::dynamic_pointer_cast<DataArrayType>(am->getAttributeArray(DataArrayName));

      DREAM3D_REQUIRE_EQUAL(results->getSize(), data.numberOfLines)

      T* resultsRaw = results->getPointer(0);
      for(size_t i = 0; i < results->getSize(); i++)
      {
        if(outputType == SIMPL::TypeNames::Double || outputType == SIMPL::TypeNames::Float)
        {
          DREAM3D_REQUIRE_EQUAL(resultsRaw[i], outputIntAsDoubleVector[i])
        }
        else
        {
          DREAM3D_REQUIRE_EQUAL(resultsRaw[i], inputIntVector[i])
        }
      }
    }

    RemoveTestFiles();

    // Test Using Unexpected Input - Alphabetical Characters, Special Characters, etc.
    {
      CreateFile(UnitTest::ReadASCIIDataTest::TestFile1, inputCharErrorVector, delimiter);

      AbstractFilter::Pointer importASCIIData = PrepFilter(data);
      DREAM3D_REQUIRE_VALID_POINTER(importASCIIData.get())

      importASCIIData->execute();
      int err = importASCIIData->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, ReadASCIIData::CONVERSION_FAILURE)
    }

    RemoveTestFiles();

    // Scientific Notation Test - Only if the output is a double or float
    if(outputType == SIMPL::TypeNames::Double || outputType == SIMPL::TypeNames::Float)
    {
      CreateFile(UnitTest::ReadASCIIDataTest::TestFile1, inputScientificNotation, delimiter);

      AbstractFilter::Pointer importASCIIData = PrepFilter(data);
      DREAM3D_REQUIRE_VALID_POINTER(importASCIIData.get())

      importASCIIData->execute();
      int err = importASCIIData->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, 0)

      AttributeMatrix::Pointer am = importASCIIData->getDataContainerArray()->getAttributeMatrix(DataArrayPath(DataContainerName, AttributeMatrixName, ""));
      typename DataArrayType::Pointer results = std::dynamic_pointer_cast<DataArrayType>(am->getAttributeArray(DataArrayName));

      DREAM3D_REQUIRE_EQUAL(results->getSize(), inputScientificNotation.size())

      T* resultsRaw = results->getPointer(0);
      for(size_t i = 0; i < results->getSize(); i++)
      {
        if(outputType == SIMPL::TypeNames::Double || outputType == SIMPL::TypeNames::Float)
        {
          double absValue = fabs(outputDoubleVector[i] - static_cast<double>(resultsRaw[i]));
          DREAM3D_REQUIRED(absValue, <, 0.01)
        }
        else
        {
          DREAM3D_REQUIRE_EQUAL(resultsRaw[i], inputIntVector[i])
        }
      }
    }

    RemoveTestFiles();

    // Hexadecimal Test - Only if the output is an integer
    if(outputType != SIMPL::TypeNames::Double && outputType != SIMPL::TypeNames::Float)
    {
      CreateFile(UnitTest::ReadASCIIDataTest::TestFile1, inputHexVector, delimiter);

      AbstractFilter::Pointer importASCIIData = PrepFilter(data);
      DREAM3D_REQUIRE_VALID_POINTER(importASCIIData.get())

      importASCIIData->execute();
      int err = importASCIIData->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, 0)

      AttributeMatrix::Pointer am = importASCIIData->getDataContainerArray()->getAttributeMatrix(DataArrayPath(DataContainerName, AttributeMatrixName, ""));
      typename DataArrayType::Pointer results = std::dynamic_pointer_cast<DataArrayType>(am->getAttributeArray(DataArrayName));

      DREAM3D_REQUIRE_EQUAL(results->getSize(), inputHexVector.size())

      T* resultsRaw = results->getPointer(0);
      for(int i = 0; i < results->getSize(); i++)
      {
        DREAM3D_REQUIRE_EQUAL(resultsRaw[i], inputIntVector[i])
      }
    }

    RemoveTestFiles();

    // Octal Test - Only if the output is an integer
    if(outputType != SIMPL::TypeNames::Double && outputType != SIMPL::TypeNames::Float)
    {
      CreateFile(UnitTest::ReadASCIIDataTest::TestFile1, inputOctVector, delimiter);

      data.numberOfLines = 7;
      data.tupleDims = std::vector<size_t>(1, 7);
      AbstractFilter::Pointer importASCIIData = PrepFilter(data);
      DREAM3D_REQUIRE_VALID_POINTER(importASCIIData.get())

      importASCIIData->execute();
      int err = importASCIIData->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, 0)

      AttributeMatrix::Pointer am = importASCIIData->getDataContainerArray()->getAttributeMatrix(DataArrayPath(DataContainerName, AttributeMatrixName, ""));
      typename DataArrayType::Pointer results = std::dynamic_pointer_cast<DataArrayType>(am->getAttributeArray(DataArrayName));

      DREAM3D_REQUIRE_EQUAL(results->getSize(), inputOctVector.size())

      T* resultsRaw = results->getPointer(0);
      for(int i = 0; i < results->getSize(); i++)
      {
        DREAM3D_REQUIRE_EQUAL(resultsRaw[i], inputIntVector[i])
      }
    }

    // Max Overflow Test
    {
      QString maxValue = QString::number(std::numeric_limits<T>().max());
      maxValue = maxValue.append('0');
      std::vector<QString> inputMaxVector({maxValue});
      CreateFile(UnitTest::ReadASCIIDataTest::TestFile1, inputMaxVector, delimiter);

      data.numberOfLines = 1;
      data.tupleDims = std::vector<size_t>(1, 1);
      AbstractFilter::Pointer importASCIIData = PrepFilter(data);
      DREAM3D_REQUIRE_VALID_POINTER(importASCIIData.get())

      importASCIIData->execute();
      int err = importASCIIData->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, ReadASCIIData::CONVERSION_FAILURE)
    }

    // Min Overflow Test
    {
      // We must hard-code these three types because there is no way to store them otherwise...
      QString minValue;
      if(outputType == SIMPL::TypeNames::Float)
      {
        minValue = "-3.4e39";
      }
      else if(outputType == SIMPL::TypeNames::Double)
      {
        minValue = "-1.7e309";
      }
      else if(outputType == SIMPL::TypeNames::Int64)
      {
        minValue = "-2e32";
      }
      else
      {
        int64_t store = std::numeric_limits<T>().min();
        store = store - 1;
        minValue = QString::number(store);
      }

      std::vector<QString> inputMinVector({minValue});
      CreateFile(UnitTest::ReadASCIIDataTest::TestFile1, inputMinVector, delimiter);

      data.numberOfLines = 1;
      data.tupleDims = std::vector<size_t>(1, 1);
      AbstractFilter::Pointer importASCIIData = PrepFilter(data);
      DREAM3D_REQUIRE_VALID_POINTER(importASCIIData.get())

      importASCIIData->execute();
      int err = importASCIIData->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, ReadASCIIData::CONVERSION_FAILURE)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    ConvertType<int8_t>();
    ConvertType<int16_t>();
    ConvertType<int32_t>();
    ConvertType<int64_t>();
    ConvertType<uint8_t>();
    ConvertType<uint16_t>();
    ConvertType<uint32_t>();
    ConvertType<uint64_t>();
    ConvertType<float>();
    ConvertType<double>();

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ReadASCIIDataTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(RemoveTestFiles()) // In case the previous test asserted or stopped prematurely

    DREAM3D_REGISTER_TEST(RunTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  ReadASCIIDataTest(const ReadASCIIDataTest&); // Copy Constructor Not Implemented
  void operator=(const ReadASCIIDataTest&);    // Move assignment Not Implemented
};
