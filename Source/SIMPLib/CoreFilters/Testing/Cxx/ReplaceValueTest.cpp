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

#include <QtCore/QDebug>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

#ifdef CREATE_DATA_ARRAY
#undef CREATE_DATA_ARRAY
#endif

#define CREATE_DATA_ARRAY(type, attrMat, tDims, cDims, initVal, comps, err)                                                                                                                            \
  DataArray<type>::Pointer _##type##_##comps##_##attrMat##Array = DataArray<type>::CreateArray(tDims, cDims, #type #comps, true);                                                                      \
  err = attrMat->insertOrAssign(_##type##_##comps##_##attrMat##Array);                                                                                                                                 \
  _##type##_##comps##_##attrMat##Array->initializeWithValue(initVal);                                                                                                                                  \
  DREAM3D_REQUIRE(err >= 0);

#ifdef SET_PROPERTIES_AND_CHECK_NE
#undef SET_PROPERTIES_AND_CHECK_NE
#endif

#define SET_PROPERTIES_AND_CHECK_NE(filter, removeValue, replaceValue, selectedArray, errVal)                                                                                                          \
  var.setValue(selectedArray);                                                                                                                                                                         \
  propWasSet = filter->setProperty("SelectedArray", var);                                                                                                                                              \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property SelectedArray";                                                                                                                                                \
  }                                                                                                                                                                                                    \
  var.setValue(removeValue);                                                                                                                                                                           \
  propWasSet = filter->setProperty("RemoveValue", var);                                                                                                                                                \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property RemoveValue";                                                                                                                                                  \
  }                                                                                                                                                                                                    \
  var.setValue(replaceValue);                                                                                                                                                                          \
  propWasSet = filter->setProperty("ReplaceValue", var);                                                                                                                                               \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property ReplaceValue";                                                                                                                                                 \
  }                                                                                                                                                                                                    \
  filter->execute();                                                                                                                                                                                   \
  err = filter->getErrorCode();                                                                                                                                                                        \
  DREAM3D_REQUIRE_EQUAL(err, errVal);

#ifdef SET_PROPERTIES_AND_CHECK_EQ
#undef SET_PROPERTIES_AND_CHECK_EQ
#endif

#define SET_PROPERTIES_AND_CHECK_EQ(filter, removeValue, replaceValue, selectedArray, dataArray, type)                                                                                                 \
  var.setValue(selectedArray);                                                                                                                                                                         \
  propWasSet = filter->setProperty("SelectedArray", var);                                                                                                                                              \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property SelectedArray";                                                                                                                                                \
  }                                                                                                                                                                                                    \
  var.setValue(removeValue);                                                                                                                                                                           \
  propWasSet = filter->setProperty("RemoveValue", var);                                                                                                                                                \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property RemoveValue";                                                                                                                                                  \
  }                                                                                                                                                                                                    \
  var.setValue(replaceValue);                                                                                                                                                                          \
  propWasSet = filter->setProperty("ReplaceValue", var);                                                                                                                                               \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property ReplaceValue";                                                                                                                                                 \
  }                                                                                                                                                                                                    \
  filter->execute();                                                                                                                                                                                   \
  err = filter->getErrorCode();                                                                                                                                                                        \
  DREAM3D_REQUIRE_EQUAL(err, 0);                                                                                                                                                                       \
  dataArray = dc->getAttributeMatrix(selectedArray.getAttributeMatrixName())->getAttributeArray(selectedArray.getDataArrayName());                                                                     \
  DREAM3D_REQUIRE_EQUAL(err, 0);                                                                                                                                                                       \
  validateReplacedValues<type>(dataArray);

class ReplaceValueTest
{
public:
  ReplaceValueTest() = default;
  virtual ~ReplaceValueTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FindDifferenceMapTest Filter from the FilterManager
    QString filtName = "ReplaceValueInArray";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The replaceValueTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer initializeDataContainerArray()
  {
    int err = 0;

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer m = DataContainer::New("ReplaceValueTest");

    // Create Attribute Matrices with different tDims to test validation of tuple compatibility
    std::vector<size_t> tDims(1, 100);
    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, "ReplaceValueAttrMat", AttributeMatrix::Type::Cell);

    m->addOrReplaceAttributeMatrix(attrMat);

    dca->addOrReplaceDataContainer(m);

    std::vector<size_t> cDims(1, 3);
    int32_t initVal = 10;
    tDims[0] = 100;

    CREATE_DATA_ARRAY(uint8_t, attrMat, tDims, cDims, initVal, 3, err);
    CREATE_DATA_ARRAY(int8_t, attrMat, tDims, cDims, initVal, 3, err);
    CREATE_DATA_ARRAY(uint16_t, attrMat, tDims, cDims, initVal, 3, err);
    CREATE_DATA_ARRAY(int16_t, attrMat, tDims, cDims, initVal, 3, err);
    CREATE_DATA_ARRAY(uint32_t, attrMat, tDims, cDims, initVal, 3, err);
    CREATE_DATA_ARRAY(int32_t, attrMat, tDims, cDims, initVal, 3, err);
    CREATE_DATA_ARRAY(uint64_t, attrMat, tDims, cDims, initVal, 3, err);
    CREATE_DATA_ARRAY(int64_t, attrMat, tDims, cDims, initVal, 3, err);
    CREATE_DATA_ARRAY(double, attrMat, tDims, cDims, initVal, 3, err);
    CREATE_DATA_ARRAY(float, attrMat, tDims, cDims, initVal, 3, err);
    CREATE_DATA_ARRAY(bool, attrMat, tDims, cDims, true, 3, err);

    cDims[0] = 1;

    CREATE_DATA_ARRAY(uint8_t, attrMat, tDims, cDims, initVal, 1, err);
    CREATE_DATA_ARRAY(int8_t, attrMat, tDims, cDims, initVal, 1, err);
    CREATE_DATA_ARRAY(uint16_t, attrMat, tDims, cDims, initVal, 1, err);
    CREATE_DATA_ARRAY(int16_t, attrMat, tDims, cDims, initVal, 1, err);
    CREATE_DATA_ARRAY(uint32_t, attrMat, tDims, cDims, initVal, 1, err);
    CREATE_DATA_ARRAY(int32_t, attrMat, tDims, cDims, initVal, 1, err);
    CREATE_DATA_ARRAY(uint64_t, attrMat, tDims, cDims, initVal, 1, err);
    CREATE_DATA_ARRAY(int64_t, attrMat, tDims, cDims, initVal, 1, err);
    CREATE_DATA_ARRAY(double, attrMat, tDims, cDims, initVal, 1, err);
    CREATE_DATA_ARRAY(float, attrMat, tDims, cDims, initVal, 1, err);
    CREATE_DATA_ARRAY(bool, attrMat, tDims, cDims, true, 1, err);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  void validateReplacedValues(IDataArray::Pointer iArray)
  {
    typename DataArray<T>::Pointer dataArrayPtr = std::dynamic_pointer_cast<DataArray<T>>(iArray);
    T* dataArray = dataArrayPtr->getPointer(0);
    size_t numTuples = dataArrayPtr->getNumberOfTuples();

    for(size_t i = 0; i < numTuples; i++)
    {
      DREAM3D_REQUIRE_EQUAL(dataArray[i], 5)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void removeArrayFromDataContainerArray(DataContainerArray::Pointer dca, DataArrayPath path)
  {
    dca->getDataContainer(path.getDataContainerName())->getAttributeMatrix(path.getAttributeMatrixName())->removeAttributeArray(path.getDataArrayName());
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void validateReplaceValue(AbstractFilter::Pointer filter, DataContainerArray::Pointer dca)
  {
    QVariant var;
    bool propWasSet;
    int err = 0;

    DataContainer::Pointer dc = dca->getDataContainer("ReplaceValueTest");
    IDataArray::Pointer dataArray;

    DataArrayPath attrMat_uint8_3("ReplaceValueTest", "ReplaceValueAttrMat", "uint8_t3");
    DataArrayPath attrMat_int8_3("ReplaceValueTest", "ReplaceValueAttrMat", "int8_t3");
    DataArrayPath attrMat_uint16_3("ReplaceValueTest", "ReplaceValueAttrMat", "uint16_t3");
    DataArrayPath attrMat_int16_3("ReplaceValueTest", "ReplaceValueAttrMat", "int16_t3");
    DataArrayPath attrMat_uint32_3("ReplaceValueTest", "ReplaceValueAttrMat", "uint32_t3");
    DataArrayPath attrMat_int32_3("ReplaceValueTest", "ReplaceValueAttrMat", "int32_t3");
    DataArrayPath attrMat_uint64_3("ReplaceValueTest", "ReplaceValueAttrMat", "uint64t3");
    DataArrayPath attrMat_int64_3("ReplaceValueTest", "ReplaceValueAttrMat", "int64_t3");
    DataArrayPath attrMat_float_3("ReplaceValueTest", "ReplaceValueAttrMat", "float3");
    DataArrayPath attrMat_double_3("ReplaceValueTest", "ReplaceValueAttrMat", "double3");
    DataArrayPath attrMat_bool_3("ReplaceValueTest", "ReplaceValueAttrMat", "bool3");

    DataArrayPath attrMat_uint8_1("ReplaceValueTest", "ReplaceValueAttrMat", "uint8_t1");
    DataArrayPath attrMat_int8_1("ReplaceValueTest", "ReplaceValueAttrMat", "int8_t1");
    DataArrayPath attrMat_uint16_1("ReplaceValueTest", "ReplaceValueAttrMat", "uint16_t1");
    DataArrayPath attrMat_int16_1("ReplaceValueTest", "ReplaceValueAttrMat", "int16_t1");
    DataArrayPath attrMat_uint32_1("ReplaceValueTest", "ReplaceValueAttrMat", "uint32_t1");
    DataArrayPath attrMat_int32_1("ReplaceValueTest", "ReplaceValueAttrMat", "int32_t1");
    DataArrayPath attrMat_uint64_1("ReplaceValueTest", "ReplaceValueAttrMat", "uint64_t1");
    DataArrayPath attrMat_int64_1("ReplaceValueTest", "ReplaceValueAttrMat", "int64_t1");
    DataArrayPath attrMat_float_1("ReplaceValueTest", "ReplaceValueAttrMat", "float1");
    DataArrayPath attrMat_double_1("ReplaceValueTest", "ReplaceValueAttrMat", "double1");
    DataArrayPath attrMat_bool_1("ReplaceValueTest", "ReplaceValueAttrMat", "bool");

    // Fail if an input array is not scalar
    SET_PROPERTIES_AND_CHECK_NE(filter, 10.0, 5.0, attrMat_uint8_3, -11002)

    // Fail if the replace value is out of range
    SET_PROPERTIES_AND_CHECK_NE(filter, 10.0, 256.0, attrMat_uint8_1, -100)
    SET_PROPERTIES_AND_CHECK_NE(filter, 10.0, 128.0, attrMat_int8_1, -100)
    SET_PROPERTIES_AND_CHECK_NE(filter, 10.0, 65536.0, attrMat_uint16_1, -100)
    SET_PROPERTIES_AND_CHECK_NE(filter, 10.0, 32768.0, attrMat_int16_1, -100)
    SET_PROPERTIES_AND_CHECK_NE(filter, 10.0, 4294967296.0, attrMat_uint32_1, -100)
    SET_PROPERTIES_AND_CHECK_NE(filter, 10.0, 2147483648.0, attrMat_int32_1, -100)
    SET_PROPERTIES_AND_CHECK_NE(filter, 10.0, 20000000000000000000.0, attrMat_uint64_1, -100)
    SET_PROPERTIES_AND_CHECK_NE(filter, 10.0, 10000000000000000000.0, attrMat_int64_1, -100)
    SET_PROPERTIES_AND_CHECK_NE(filter, 10.0, 3.41e38, attrMat_float_1, -101)
    // not checking double, because cannot make a value outside of the range

    // Succeed for all possible test combinations
    SET_PROPERTIES_AND_CHECK_EQ(filter, 10.0, 5.0, attrMat_uint8_1, dataArray, uint8_t)
    SET_PROPERTIES_AND_CHECK_EQ(filter, 10.0, 5.0, attrMat_int8_1, dataArray, int8_t)
    SET_PROPERTIES_AND_CHECK_EQ(filter, 10.0, 5.0, attrMat_uint16_1, dataArray, uint16_t)
    SET_PROPERTIES_AND_CHECK_EQ(filter, 10.0, 5.0, attrMat_int16_1, dataArray, int16_t)
    SET_PROPERTIES_AND_CHECK_EQ(filter, 10.0, 5.0, attrMat_uint32_1, dataArray, uint32_t)
    SET_PROPERTIES_AND_CHECK_EQ(filter, 10.0, 5.0, attrMat_int32_1, dataArray, int32_t)
    SET_PROPERTIES_AND_CHECK_EQ(filter, 10.0, 5.0, attrMat_uint64_1, dataArray, uint64_t)
    SET_PROPERTIES_AND_CHECK_EQ(filter, 10.0, 5.0, attrMat_int64_1, dataArray, int64_t)
    SET_PROPERTIES_AND_CHECK_EQ(filter, 10.0, 5.0, attrMat_float_1, dataArray, float)
    SET_PROPERTIES_AND_CHECK_EQ(filter, 10.0, 5.0, attrMat_double_1, dataArray, double)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    DataContainerArray::Pointer dca = initializeDataContainerArray();

    QString filtName = "ReplaceValueInArray";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr)

    AbstractFilter::Pointer replaceValueFilter = factory->create();
    DREAM3D_REQUIRE(replaceValueFilter.get() != nullptr)

    replaceValueFilter->setDataContainerArray(dca);

    validateReplaceValue(replaceValueFilter, dca);

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ReplaceValueTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());
    DREAM3D_REGISTER_TEST(RunTest())
  }

private:
  ReplaceValueTest(const ReplaceValueTest&); // Copy Constructor Not Implemented
  void operator=(const ReplaceValueTest&);   // Move assignment Not Implemented
};
