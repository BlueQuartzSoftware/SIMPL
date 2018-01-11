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
#include <QtCore/QDir>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/CreateDataArray.h"
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

class CreateDataArrayTest
{
public:
  CreateDataArrayTest()
  {
  }
  virtual ~CreateDataArrayTest()
  {
  }

  enum ErrorCodes
  {
    NO_ERROR = 0,
    INT8_ERROR = -4050
  };

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the CreateDataArray Filter from the FilterManager
    QString filtName = "CreateDataArray";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The CreateDataArrayTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    // Now instantiate the CreateDataContainer Filter from the FilterManager
    filtName = "CreateDataContainer";
    filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The CreateDataArrayTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    // Now instantiate the CreateDataContainer Filter from the FilterManager
    filtName = "CreateAttributeMatrix";
    fm = FilterManager::Instance();
    filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The CreateDataArrayTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestCreateDataContainerAndAttributeMatrix()
  {

    int err = 0;
    bool propWasSet = false;
    // bool ok = false;
    QVariant var;

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Now instantiate the CreateDataArray Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();

    QString filtName = "CreateDataContainer";
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();

      filter->setDataContainerArray(dca);

      // Test 1 set int32 array with an initialization of -5 and read value for comparison
      var.setValue(QString::fromLatin1("Test Data Container"));
      propWasSet = filter->setProperty("CreatedDataContainer", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      filter->preflight();
      err = filter->getErrorCondition();
      DREAM3D_REQUIRED(err, >=, 0)
    }

    filtName = "CreateAttributeMatrix";
    filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();

      filter->setDataContainerArray(dca);

      var.setValue(QString::fromLatin1("Test Data Container"));
      propWasSet = filter->setProperty("CreatedDataContainer", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(QString::fromLatin1("Cell Attribute Matrix"));
      propWasSet = filter->setProperty("CreatedAttributeMatrix", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      filter->preflight();
      err = filter->getErrorCondition();
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
    dca->addDataContainer(m);
    AttributeMatrix::Pointer attrMatrix = AttributeMatrix::New(QVector<size_t>(1, 1), SIMPL::Defaults::AttributeMatrixName, AttributeMatrix::Type::Generic);
    m->addAttributeMatrix(SIMPL::Defaults::AttributeMatrixName, attrMatrix);
    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void TestScalarType(AbstractFilter::Pointer filter, SIMPL::ScalarTypes::Type scalarType)
  {

    QString dsName = QString("TestArray-%1").arg(static_cast<int>(scalarType));
    DataArrayPath path = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::AttributeMatrixName, dsName);

    DataContainerArray::Pointer dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);

    QVariant var;
    int err = 0;
    bool propWasSet = false;

    var.setValue(scalarType);
    propWasSet = filter->setProperty("ScalarType", var); //
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(1);
    propWasSet = filter->setProperty("NumberOfComponents", var); // 1 component
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(path);
    propWasSet = filter->setProperty("NewArray", var); // array path
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    /* ==== Test The Maximum Value for the primitive type ===== */
    T max = std::numeric_limits<T>::max();
    var.setValue(QString::number(max));
    propWasSet = filter->setProperty("InitializationValue", var); // initialize with
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    filter->preflight();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->execute();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

    {
      IDataArray::Pointer testArrayPtr = dca->getDataContainer(SIMPL::Defaults::DataContainerName)->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
      typedef DataArray<T> DataArrayType;
      typename DataArrayType::Pointer inputArray = std::dynamic_pointer_cast<DataArrayType>(testArrayPtr);
      DREAM3D_REQUIRE_VALID_POINTER(inputArray.get());
      T* inputArrayPtr = inputArray->getPointer(0); // pointer to the int array created from the filter
      DREAM3D_REQUIRE_EQUAL(inputArrayPtr[0], max)
    }

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);

    /* ==== Test The Minmum Value for the primitive type ===== */
    T min = std::numeric_limits<T>::min();
    var.setValue(QString::number(min));
    propWasSet = filter->setProperty("InitializationValue", var); // initialize with
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->preflight();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

    dca = CreateDataContainerArray();
    filter->setDataContainerArray(dca);

    filter->execute();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

    {
      IDataArray::Pointer testArrayPtr = dca->getDataContainer(SIMPL::Defaults::DataContainerName)->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
      typedef DataArray<T> DataArrayType;
      typename DataArrayType::Pointer inputArray = std::dynamic_pointer_cast<DataArrayType>(testArrayPtr);
      DREAM3D_REQUIRE_VALID_POINTER(inputArray.get());
      T* inputArrayPtr = inputArray->getPointer(0); // pointer to the int array created from the filter
      DREAM3D_REQUIRE_EQUAL(inputArrayPtr[0], min)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestCreateDataArray()
  {
    //  bool propWasSet = false;
    //  bool ok = false;
    //  int err = 0;

    // Now instantiate the CreateDataArray Filter from the FilterManager
    QString filtName = "CreateDataArray";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();
      // DataArrayPath path1 = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::AttributeMatrixName, "testArray");

      TestScalarType<uint64_t>(filter, SIMPL::ScalarTypes::Type::UInt64);
      TestScalarType<int8_t>(filter, SIMPL::ScalarTypes::Type::Int8);
      TestScalarType<uint8_t>(filter, SIMPL::ScalarTypes::Type::UInt8);
      TestScalarType<int16_t>(filter, SIMPL::ScalarTypes::Type::Int16);
      TestScalarType<uint16_t>(filter, SIMPL::ScalarTypes::Type::UInt16);
      TestScalarType<int32_t>(filter, SIMPL::ScalarTypes::Type::Int32);
      TestScalarType<uint32_t>(filter, SIMPL::ScalarTypes::Type::UInt32);
      TestScalarType<int64_t>(filter, SIMPL::ScalarTypes::Type::Int64);

//    TestScalarType<float>(filter, 8);
//    TestScalarType<double>(filter, path1, 9);

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
        err = filter->getErrorCondition();
        DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

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
        err = filter->getErrorCondition();
        DREAM3D_REQUIRE_EQUAL(err, INT8_ERROR);

#endif
    }
    else
    {
      QString ss = QObject::tr("CreateDataArrayTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### CreateDataArrayTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestCreateDataArray())
  }

private:
  CreateDataArrayTest(const CreateDataArrayTest&); // Copy Constructor Not Implemented
  void operator=(const CreateDataArrayTest&);      // Operator '=' Not Implemented
};
