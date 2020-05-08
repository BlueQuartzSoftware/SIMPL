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

#include <memory>

#include "SIMPLib/DataArrays/DataArray.hpp"
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
#include "SIMPLib/DataContainers/DataContainer.h"

class SplitAttributeArrayTest
{

public:
  SplitAttributeArrayTest() = default;
  virtual ~SplitAttributeArrayTest() = default;
  SplitAttributeArrayTest(const SplitAttributeArrayTest&) = delete;            // Copy Constructor Not Implemented
  SplitAttributeArrayTest(SplitAttributeArrayTest&&) = delete;                 // Move Constructor Not Implemented
  SplitAttributeArrayTest& operator=(const SplitAttributeArrayTest&) = delete; // Copy Assignment Not Implemented
  SplitAttributeArrayTest& operator=(SplitAttributeArrayTest&&) = delete;      // Move Assignment Not Implemented

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer dc = DataContainer::New("DataContainer");
    AttributeMatrix::Pointer am1 = AttributeMatrix::New(std::vector<size_t>(1, 10), "AttributeMatrix", AttributeMatrix::Type::Any);

    UInt32ArrayType::Pointer mcArray1 = UInt32ArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array uint32_t", true);
    fillDataArray<uint32_t>(mcArray1);

    BoolArrayType::Pointer mcArray2 = BoolArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array bool", true);
    fillDataArray(mcArray2);

    UCharArrayType::Pointer mcArray3 = UCharArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array unsigned char", true);
    fillDataArray<unsigned char>(mcArray3);

    Int8ArrayType::Pointer mcArray4 = Int8ArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array int8_t", true);
    fillDataArray<int8_t>(mcArray4);

    UInt8ArrayType::Pointer mcArray5 = UInt8ArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array uint8_t", true);
    fillDataArray<uint8_t>(mcArray5);

    Int16ArrayType::Pointer mcArray6 = Int16ArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array int16_t", true);
    fillDataArray<int16_t>(mcArray6);

    UInt16ArrayType::Pointer mcArray7 = UInt16ArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array uint16_t", true);
    fillDataArray<uint16_t>(mcArray7);

    Int32ArrayType::Pointer mcArray8 = Int32ArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array int32_t", true);
    fillDataArray<int32_t>(mcArray8);

    Int64ArrayType::Pointer mcArray9 = Int64ArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array int64_t", true);
    fillDataArray<int64_t>(mcArray9);

    UInt64ArrayType::Pointer mcArray10 = UInt64ArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array uint64_t", true);
    fillDataArray<uint64_t>(mcArray10);

    FloatArrayType::Pointer mcArray11 = FloatArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array float", true);
    fillDataArray<float>(mcArray11);

    DoubleArrayType::Pointer mcArray12 = DoubleArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array double", true);
    fillDataArray<double>(mcArray12);

    SizeTArrayType::Pointer mcArray13 = SizeTArrayType::CreateArray(std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), "MultiComponent Array size_t", true);
    fillDataArray<size_t>(mcArray13);

    am1->insertOrAssign(mcArray1);
    am1->insertOrAssign(mcArray2);
    am1->insertOrAssign(mcArray3);
    am1->insertOrAssign(mcArray4);
    am1->insertOrAssign(mcArray5);
    am1->insertOrAssign(mcArray6);
    am1->insertOrAssign(mcArray7);
    am1->insertOrAssign(mcArray8);
    am1->insertOrAssign(mcArray9);
    am1->insertOrAssign(mcArray10);
    am1->insertOrAssign(mcArray11);
    am1->insertOrAssign(mcArray12);
    am1->insertOrAssign(mcArray13);
    dc->addOrReplaceAttributeMatrix(am1);
    dca->addOrReplaceDataContainer(dc);

    return dca;
  }

  void fillDataArray(BoolArrayType::Pointer inputArray)
  {
    typename BoolArrayType::Pointer daPtr = std::dynamic_pointer_cast<BoolArrayType>(inputArray);
    bool num = false;
    for(int i = 0; i < daPtr->getNumberOfTuples(); i++)
    {
      daPtr->setComponent(i, 0, num);
      daPtr->setComponent(i, 1, num);
      daPtr->setComponent(i, 2, num);
      num = true;
      daPtr->setComponent(i, 3, num);
      num = true;
      daPtr->setComponent(i, 4, num);
      num = true;
    }
  }

  template <typename T>
  void fillDataArray(IDataArray::Pointer inputArray)
  {
    typename DataArray<T>::Pointer daPtr = std::dynamic_pointer_cast<DataArray<T>>(inputArray);
    T num = 0;
    for(int i = 0; i < daPtr->getNumberOfTuples(); i++)
    {
      daPtr->setComponent(i, 0, num);
      num++;
      daPtr->setComponent(i, 1, num);
      num++;
      daPtr->setComponent(i, 2, num);
      num++;
      daPtr->setComponent(i, 3, num);
      num++;
      daPtr->setComponent(i, 4, num);
      num++;
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the SplitAttributeArrayTest Filter from the FilterManager
    QString filtName = "SplitAttributeArray";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "DREAM3D requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestSplitAttributeArrayTest()
  {
    DataContainerArray::Pointer dca = createDataContainerArray();
    FilterManager* fm = FilterManager::Instance();

    QString filtName = "SplitAttributeArray";
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr != filterFactory.get())
    {
      AbstractFilter::Pointer filter = filterFactory->create();

      TestSplitByType<uint32_t>(filter, dca, "uint32_t");
      TestSplitByType<bool>(filter, dca, "bool");
      TestSplitByType<unsigned char>(filter, dca, "unsigned char");
      TestSplitByType<int8_t>(filter, dca, "int8_t");
      TestSplitByType<uint8_t>(filter, dca, "uint8_t");
      TestSplitByType<int16_t>(filter, dca, "int16_t");
      TestSplitByType<uint16_t>(filter, dca, "uint16_t");
      TestSplitByType<int32_t>(filter, dca, "int32_t");
      TestSplitByType<int64_t>(filter, dca, "int64_t");
      TestSplitByType<uint64_t>(filter, dca, "uint64_t");
      TestSplitByType<float>(filter, dca, "float");
      TestSplitByType<double>(filter, dca, "double");
      TestSplitByType<size_t>(filter, dca, "size_t");
    }

    return EXIT_SUCCESS;
  }

  template <typename T>
  void TestSplitByType(AbstractFilter::Pointer filter, DataContainerArray::Pointer dca, QString dataType)
  {
    //    Observer obs;
    //    QObject::connect(
    //          filter.get(), &AbstractFilter::messageGenerated,
    //          &obs, &Observer::processPipelineMessage
    //          );

    DataArrayPath arrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType);
    int err = 0;
    bool propWasSet = false;
    QVariant var;

    filter->setDataContainerArray(dca);

    var.setValue(arrayPath);
    propWasSet = filter->setProperty("InputArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRED(err, >=, 0)

    dca = createDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRED(err, >=, 0)
    using DataArrayPtrType = std::shared_ptr<DataArray<T>>;
    DataArrayPtrType mcArray_original =
        filter->getDataContainerArray()->getPrereqArrayFromPath<DataArray<T>>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType));

    std::vector<DataArrayPtrType> mcArraysAfterSplit;
    DataArrayPtrType mcArray0 =
        filter->getDataContainerArray()->getPrereqArrayFromPath<DataArray<T>>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType + "Component0"));
    mcArraysAfterSplit.push_back(mcArray0);
    DataArrayPtrType mcArray1 =
        filter->getDataContainerArray()->getPrereqArrayFromPath<DataArray<T>>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType + "Component1"));
    mcArraysAfterSplit.push_back(mcArray1);
    DataArrayPtrType mcArray2 =
        filter->getDataContainerArray()->getPrereqArrayFromPath<DataArray<T>>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType + "Component2"));
    mcArraysAfterSplit.push_back(mcArray2);
    DataArrayPtrType mcArray3 =
        filter->getDataContainerArray()->getPrereqArrayFromPath<DataArray<T>>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType + "Component3"));
    mcArraysAfterSplit.push_back(mcArray3);
    DataArrayPtrType mcArray4 =
        filter->getDataContainerArray()->getPrereqArrayFromPath<DataArray<T>>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType + "Component4"));
    mcArraysAfterSplit.push_back(mcArray4);

    size_t numTuples = mcArray_original->getNumberOfTuples();
    int32_t numComps = mcArray_original->getNumberOfComponents();
    for(int i = 0; i < numTuples; i++)
    {
      for(int j = 0; j < numComps; j++)
      {
        T originalValue = mcArray_original->getComponent(i, j);
        T afterSplitValue = mcArraysAfterSplit.at(j)->getComponent(i, 0);
        DREAM3D_REQUIRE_EQUAL(originalValue, afterSplitValue);
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestSplitAttributeArrayTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }
};
