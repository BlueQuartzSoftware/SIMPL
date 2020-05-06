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

#include "SIMPLib/CoreFilters/CreateDataArray.h"
#include "SIMPLib/CoreFilters/CreateDataContainer.h"
#include "SIMPLib/CoreFilters/CreateAttributeMatrix.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"
#include "SIMPLib/DataContainers/DataContainerArray.h"

class CreateDataArrayTest
{
public:
  CreateDataArrayTest() = default;
  virtual ~CreateDataArrayTest() = default;
  static const int k_NoError = 0;
  static const int k_Int8Error = -4050;
  const size_t k_NumTuples = 5;
  const size_t k_NumComponents = 3;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestCreateDataContainerAndAttributeMatrix()
  {

    int err = 0;
    // bool ok = false;
    QVariant var;

    DataContainerArray::Pointer dca = DataContainerArray::New();
    {
      CreateDataContainer::Pointer filter = CreateDataContainer::New();
      filter->setDataContainerName({"Test Data Container", "", ""});
      filter->setDataContainerArray(dca);

      filter->preflight();
      err = filter->getErrorCode();
      DREAM3D_REQUIRED(err, >=, 0)
    }
    {
      CreateAttributeMatrix::Pointer filter = CreateAttributeMatrix::New();

      filter->setDataContainerArray(dca);
      filter->setCreatedAttributeMatrix({"Test Data Container", "Cell Attribute Matrix", ""});

      filter->preflight();
      err = filter->getErrorCode();
      DREAM3D_REQUIRED(err, >=, 0)
    }

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer CreateDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer m = DataContainer::New(SIMPL::Defaults::DataContainerName);
    dca->addOrReplaceDataContainer(m);
    AttributeMatrix::Pointer attrMatrix = AttributeMatrix::New({k_NumTuples}, SIMPL::Defaults::AttributeMatrixName, AttributeMatrix::Type::Generic);
    m->addOrReplaceAttributeMatrix(attrMatrix);
    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  void TestScalarType(CreateDataArray::Pointer filter, SIMPL::ScalarTypes::Type scalarType)
  {
    using DataArrayType = DataArray<T>;
    QString dsName = QString("TestArray-%1").arg(static_cast<int>(scalarType));
    DataArrayPath path = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::AttributeMatrixName, dsName);

    DataContainerArray::Pointer dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setScalarType(scalarType);
    filter->setNumberOfComponents(1);
    filter->setNewArray(path);
    filter->setInitializationType(0);
    /* ==== Test The Maximum Value for the primitive type ===== */
    T max = std::numeric_limits<T>::max();
    filter->setInitializationValue(QString::number(max));

    filter->preflight();
    int32_t err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, k_NoError);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, k_NoError);

    {
      IDataArray::Pointer testArrayPtr = dca->getDataContainer(SIMPL::Defaults::DataContainerName)->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
      typename DataArrayType::Pointer inputArray = std::dynamic_pointer_cast<DataArrayType>(testArrayPtr);
      DREAM3D_REQUIRE_VALID_POINTER(inputArray.get());
      T* inputArrayPtr = inputArray->getPointer(0); // pointer to the int array created from the filter
      DREAM3D_REQUIRE_EQUAL(inputArrayPtr[0], max)
    }

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);

    /* ==== Test The Minmum Value for the primitive type ===== */
    T min = std::numeric_limits<T>::min();
    filter->setInitializationValue(QString::number(min));

    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, k_NoError);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);

    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, k_NoError);

    {
      IDataArray::Pointer testArrayPtr = dca->getDataContainer(SIMPL::Defaults::DataContainerName)->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
      typename DataArrayType::Pointer inputArray = std::dynamic_pointer_cast<DataArrayType>(testArrayPtr);
      DREAM3D_REQUIRE_VALID_POINTER(inputArray.get());
      T* inputArrayPtr = inputArray->getPointer(0); // pointer to the int array created from the filter
      DREAM3D_REQUIRE_EQUAL(inputArrayPtr[0], min)
    }

    // test multiple components with multiple init values
    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setNumberOfComponents(k_NumComponents);
    filter->setInitializationValue("[2;3;4");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5458);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("2;3;4]");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5458);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("2;3;4");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5457);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[2;4]");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5459);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("2;3;hfi4");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5457);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[2;3;hfi4]");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5452);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[65;78;88]"); // we chose these values so the int8 and uint8 could print something if needed.
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, k_NoError);

    // Now verify the data was correctly written to the data array
    std::vector<T> exemplar = {static_cast<T>(65), static_cast<T>(78), static_cast<T>(88)};
    typename DataArrayType::Pointer output = dca->getPrereqArrayFromPath<DataArrayType>(nullptr, path, {3});
    size_t outNumTuples = output->getNumberOfTuples();
    for(size_t t = 0; t < outNumTuples; t++)
    {
      T* tPtr = output->getTuplePointer(t);
      int32_t cmp = std::memcmp(tPtr, exemplar.data(), sizeof(T) * k_NumComponents);
      if(cmp != 0)
      {
        std::cout << "##################################################################################################" << std::endl;
        std::cout << "CreateDataArrayTest Failure:(" << __LINE__ << ")" << std::endl;
        std::cout << output->getInfoString(SIMPL::MarkDown).toStdString() << std::endl;
        QString str;
        QTextStream out(&str);
        output->printTuple(out, t);
        std::cout << "Data:    " << str.toStdString() << std::endl;
        std::cout << "Exemplar:" << exemplar[0] << ", " << exemplar[1] << ", " << exemplar[2] << std::endl;
        std::cout << "##################################################################################################" << std::endl;
      }
      DREAM3D_REQUIRE_EQUAL(cmp, 0)
    }

    //-------------------------------------------------------------------------
    // Now try with muliple components but a single value in the string
    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[65]"); // we chose these values so the int8 and uint8 could print something if needed.
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, k_NoError);

    // Now verify the data was correctly written to the data array
    exemplar = {static_cast<T>(65), static_cast<T>(65), static_cast<T>(65)};
    output = dca->getPrereqArrayFromPath<DataArrayType>(nullptr, path, {3});
    outNumTuples = output->getNumberOfTuples();
    for(size_t t = 0; t < outNumTuples; t++)
    {
      T* tPtr = output->getTuplePointer(t);
      int32_t cmp = std::memcmp(tPtr, exemplar.data(), sizeof(T) * k_NumComponents);
      if(cmp != 0)
      {
        std::cout << "##################################################################################################" << std::endl;
        std::cout << "CreateDataArrayTest Failure:(" << __LINE__ << ")" << std::endl;
        std::cout << output->getInfoString(SIMPL::MarkDown).toStdString() << std::endl;
        QString str;
        QTextStream out(&str);
        output->printTuple(out, t);
        std::cout << "Data:    " << str.toStdString() << std::endl;
        std::cout << "Exemplar:" << exemplar[0] << ", " << exemplar[1] << ", " << exemplar[2] << std::endl;
        std::cout << "##################################################################################################" << std::endl;
      }
      DREAM3D_REQUIRE_EQUAL(cmp, 0)
    }

    //-------------------------------------------------------------------------
    // Now try with muliple components but a single value in the string
    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("65"); // we chose these values so the int8 and uint8 could print something if needed.
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, k_NoError);

    // Now verify the data was correctly written to the data array
    exemplar = {static_cast<T>(65), static_cast<T>(65), static_cast<T>(65)};
    output = dca->getPrereqArrayFromPath<DataArrayType>(nullptr, path, {3});
    outNumTuples = output->getNumberOfTuples();
    for(size_t t = 0; t < outNumTuples; t++)
    {
      T* tPtr = output->getTuplePointer(t);
      int32_t cmp = std::memcmp(tPtr, exemplar.data(), sizeof(T) * k_NumComponents);
      if(cmp != 0)
      {
        std::cout << "##################################################################################################" << std::endl;
        std::cout << "CreateDataArrayTest Failure:(" << __LINE__ << ")" << std::endl;
        std::cout << output->getInfoString(SIMPL::MarkDown).toStdString() << std::endl;
        QString str;
        QTextStream out(&str);
        output->printTuple(out, t);
        std::cout << "Data:    " << str.toStdString() << std::endl;
        std::cout << "Exemplar:" << exemplar[0] << ", " << exemplar[1] << ", " << exemplar[2] << std::endl;
        std::cout << "##################################################################################################" << std::endl;
      }
      DREAM3D_REQUIRE_EQUAL(cmp, 0)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestCreateDataArray()
  {

    CreateDataArray::Pointer filter = CreateDataArray::New();

    TestScalarType<int8_t>(filter, SIMPL::ScalarTypes::Type::Int8);
    TestScalarType<uint8_t>(filter, SIMPL::ScalarTypes::Type::UInt8);
    TestScalarType<int16_t>(filter, SIMPL::ScalarTypes::Type::Int16);
    TestScalarType<uint16_t>(filter, SIMPL::ScalarTypes::Type::UInt16);
    TestScalarType<int32_t>(filter, SIMPL::ScalarTypes::Type::Int32);
    TestScalarType<uint32_t>(filter, SIMPL::ScalarTypes::Type::UInt32);
    TestScalarType<int64_t>(filter, SIMPL::ScalarTypes::Type::Int64);
    TestScalarType<uint64_t>(filter, SIMPL::ScalarTypes::Type::UInt64);

#if 0
        // Test 2 set a bool array with a non-zero value and compare to 1
        filter->setDataContainerArray(dca);
        filter->getDataContainerArray()->getAttributeMatrix(path1)->removeAttributeArray(path1.getDataArrayName());
        QVariant var;
        var.setValue(10);
        propWasSet = filter->setProperty("ScalarType", var); // bool
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)

            double d = QString("0.000001").toDouble(&ok);
        var.setValue(d);
        propWasSet = filter->setProperty("InitializationValue", var); // initialize with d (9.9999999999999995e-007 Visual Studio)
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)

            filter->execute();
        err = filter->getErrorCode();
        DREAM3D_REQUIRE_EQUAL(err, k_NoError);

        IDataArray::Pointer boolArray = m->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
        DataArray<bool>::Pointer inputArrayBool = std::dynamic_pointer_cast<DataArray<bool>>(boolArray);
        bool* inputArrayPtrBool = inputArrayBool->getPointer(0); // pointer to the bool array created from the filter
        if (*inputArrayPtrBool != 1) // check the initialization value from the bool array
        {
          DREAM3D_REQUIRE_EQUAL(0, 1)
        }

        // Test 3 set int8 array with an initialization of 128 for out of range error
        filter->setDataContainerArray(dca);
        var.setValue(0);
        propWasSet = filter->setProperty("ScalarType", var); // int8
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)

            var.setValue(128);
        propWasSet = filter->setProperty("InitializationValue", var); // initialize with 128
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)

            filter->execute();
        err = filter->getErrorCode();
        DREAM3D_REQUIRE_EQUAL(err, k_Int8Error);

#endif

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### CreateDataArrayTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestCreateDataArray())
  }

private:
  CreateDataArrayTest(const CreateDataArrayTest&); // Copy Constructor Not Implemented
  void operator=(const CreateDataArrayTest&);      // Move assignment Not Implemented
};
