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
  void TestBasicParsing(SIMPL::ScalarTypes::Type scalarType)
  {
    CreateDataArray::Pointer filter = CreateDataArray::New();
    int32_t err = 0;
    DataContainerArray::Pointer dca = DataContainerArray::NullPointer();

    QString dsName = QString("TestArray-%1").arg(static_cast<int>(scalarType));
    std::cout << "    Testing Basic Parsing" << dsName.toStdString() << std::endl;
    DataArrayPath path = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::AttributeMatrixName, dsName);

    filter->setScalarType(scalarType);
    filter->setNewArray(path);
    filter->setInitializationType(0);
    filter->setNumberOfComponents(k_NumComponents);

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
    DREAM3D_REQUIRE_EQUAL(err, -5400);
  }
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  void TestUnsignedIntegers(SIMPL::ScalarTypes::Type scalarType)
  {
    CreateDataArray::Pointer filter = CreateDataArray::New();
    int32_t err = 0;
    using DataArrayType = DataArray<T>;
    DataContainerArray::Pointer dca = DataContainerArray::NullPointer();

    QString dsName = QString("TestArray-%1").arg(static_cast<int>(scalarType));
    std::cout << "    Testing Unsigned Integer" << dsName.toStdString() << std::endl;
    DataArrayPath path = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::AttributeMatrixName, dsName);

    filter->setScalarType(scalarType);
    filter->setNewArray(path);
    filter->setInitializationType(0);
    filter->setNumberOfComponents(k_NumComponents);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[65.333;78.666;88.432347]");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5400);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[-65;78;88]");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5400);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[65;78;88]");
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
    filter->setInitializationValue("[-65]");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5400);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[65]");
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
    filter->setInitializationValue("-65");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5457);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[-65]");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5400);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("65");
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
  template <typename T>
  void TestSignedIntegers(SIMPL::ScalarTypes::Type scalarType)
  {
    CreateDataArray::Pointer filter = CreateDataArray::New();
    int32_t err = 0;
    using DataArrayType = DataArray<T>;
    DataContainerArray::Pointer dca = DataContainerArray::NullPointer();

    QString dsName = QString("TestArray-%1").arg(static_cast<int>(scalarType));
    std::cout << "    Testing Signed Integer" << dsName.toStdString() << std::endl;
    DataArrayPath path = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::AttributeMatrixName, dsName);

    filter->setScalarType(scalarType);
    filter->setNewArray(path);
    filter->setInitializationType(0);
    filter->setNumberOfComponents(k_NumComponents);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[65.333;78.666;88.432347]");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -5400);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[-65;78;88]");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, k_NoError);

    // Now verify the data was correctly written to the data array
    std::vector<T> exemplar = {static_cast<T>(-65), static_cast<T>(78), static_cast<T>(88)};
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
    filter->setInitializationValue("[-65]");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, k_NoError);

    // Now verify the data was correctly written to the data array
    exemplar = {static_cast<T>(-65), static_cast<T>(-65), static_cast<T>(-65)};
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
    filter->setInitializationValue("65");
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
  template <typename T>
  void TestReals(SIMPL::ScalarTypes::Type scalarType)
  {
    CreateDataArray::Pointer filter = CreateDataArray::New();
    int32_t err = 0;
    using DataArrayType = DataArray<T>;
    DataContainerArray::Pointer dca = DataContainerArray::NullPointer();

    QString dsName = QString("TestArray-%1").arg(static_cast<int>(scalarType));
    std::cout << "    Testing Reals" << dsName.toStdString() << std::endl;
    DataArrayPath path = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::AttributeMatrixName, dsName);

    filter->setScalarType(scalarType);
    filter->setNewArray(path);
    filter->setInitializationType(0);
    filter->setNumberOfComponents(k_NumComponents);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[65.333;78.666;88.432347]");
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setInitializationValue("[65;78;88]");
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
    filter->setInitializationValue("[65]");
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
    filter->setInitializationValue("65");
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
    TestBasicParsing<int8_t>(SIMPL::ScalarTypes::Type::Int8);
    TestBasicParsing<uint8_t>(SIMPL::ScalarTypes::Type::UInt8);
    TestBasicParsing<int16_t>(SIMPL::ScalarTypes::Type::Int16);
    TestBasicParsing<uint16_t>(SIMPL::ScalarTypes::Type::UInt16);
    TestBasicParsing<int32_t>(SIMPL::ScalarTypes::Type::Int32);
    TestBasicParsing<uint32_t>(SIMPL::ScalarTypes::Type::UInt32);
    TestBasicParsing<int64_t>(SIMPL::ScalarTypes::Type::Int64);
    TestBasicParsing<uint64_t>(SIMPL::ScalarTypes::Type::UInt64);
    TestBasicParsing<float>(SIMPL::ScalarTypes::Type::Float);
    TestBasicParsing<double>(SIMPL::ScalarTypes::Type::Double);

    TestSignedIntegers<int8_t>(SIMPL::ScalarTypes::Type::Int8);
    TestSignedIntegers<int16_t>(SIMPL::ScalarTypes::Type::Int16);
    TestSignedIntegers<int32_t>(SIMPL::ScalarTypes::Type::Int32);
    TestSignedIntegers<int64_t>(SIMPL::ScalarTypes::Type::Int64);

    TestUnsignedIntegers<uint8_t>(SIMPL::ScalarTypes::Type::UInt8);
    TestUnsignedIntegers<uint16_t>(SIMPL::ScalarTypes::Type::UInt16);
    TestUnsignedIntegers<uint32_t>(SIMPL::ScalarTypes::Type::UInt32);
    TestUnsignedIntegers<uint64_t>(SIMPL::ScalarTypes::Type::UInt64);

    TestReals<float>(SIMPL::ScalarTypes::Type::Float);
    TestReals<double>(SIMPL::ScalarTypes::Type::Double);

    //    //   TestIntegers<bool>(filter, SIMPL::ScalarTypes::Type::Bool);
    //    TestIntegers<size_t>(SIMPL::ScalarTypes::Type::SizeT);
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
