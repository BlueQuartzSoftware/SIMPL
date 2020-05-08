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

#include <QtCore/QDebug>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/CoreFilters/ConditionalSetValue.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

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

#define SET_PROPERTIES_AND_CHECK_NE(filter, replaceValue, selectedArray, conditionalArray, errVal)                                                                                                     \
  var.setValue(selectedArray);                                                                                                                                                                         \
  propWasSet = filter->setProperty("SelectedArrayPath", var);                                                                                                                                          \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property SelectedArrayPath";                                                                                                                                            \
  }                                                                                                                                                                                                    \
  var.setValue(conditionalArray);                                                                                                                                                                      \
  propWasSet = filter->setProperty("ConditionalArrayPath", var);                                                                                                                                       \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property ConditionalArrayPath";                                                                                                                                         \
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

#define SET_PROPERTIES_AND_CHECK_EQ(filter, replaceValue, selectedArray, conditionalArray, dataArray, condArray, type)                                                                                 \
  var.setValue(selectedArray);                                                                                                                                                                         \
  propWasSet = filter->setProperty("SelectedArrayPath", var);                                                                                                                                          \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property SelectedArrayPath";                                                                                                                                            \
  }                                                                                                                                                                                                    \
  var.setValue(conditionalArray);                                                                                                                                                                      \
  propWasSet = filter->setProperty("ConditionalArrayPath", var);                                                                                                                                       \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property ConditionalArrayPath";                                                                                                                                         \
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
  condArray = dc->getAttributeMatrix(conditionalArray.getAttributeMatrixName())->getAttributeArray(conditionalArray.getDataArrayName());                                                               \
  DREAM3D_REQUIRE_EQUAL(err, 0);                                                                                                                                                                       \
  validateReplacedValues<type>(dataArray, condArray);

class ConditionalSetValueTest
{
public:
  ConditionalSetValueTest() = default;
  virtual ~ConditionalSetValueTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FindDifferenceMapTest Filter from the FilterManager
    QString filtName = "ConditionalSetValue";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The ConditionalSetValueTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
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

    DataContainer::Pointer m = DataContainer::New("ConditionalSetValueTest");

    // Create Attribute Matrices with different tDims to test validation of tuple compatibility
    std::vector<size_t> tDims(1, 10);
    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, "ConditionalSetValueAttrMat", AttributeMatrix::Type::Cell);

    m->addOrReplaceAttributeMatrix(attrMat);

    dca->addOrReplaceDataContainer(m);

    std::vector<size_t> cDims(1, 3);
    int32_t initVal = 10;

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

    // this is the conditional array
    QString name = "ConditionalArray";
    BoolArrayType::Pointer condArrayPtr = BoolArrayType::CreateArray(tDims, cDims, name, true);
    attrMat->insertOrAssign(condArrayPtr);
    condArrayPtr->initializeWithValue(true);
    // Set some of the values to false int he conditional array
    bool* condArray = condArrayPtr->getPointer(0);
    for(size_t iter = 0; iter < tDims[0]; iter++)
    {
      if(iter % 2 == 0)
        condArray[iter] = false;
    }

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  void validateReplacedValues(IDataArray::Pointer iArray, IDataArray::Pointer cArray)
  {
    typename DataArray<T>::Pointer dataArrayPtr = std::dynamic_pointer_cast<DataArray<T>>(iArray);
    typename DataArray<bool>::Pointer condArrayPtr = std::dynamic_pointer_cast<DataArray<bool>>(cArray);
    T* dataArray = dataArrayPtr->getPointer(0);
    bool* condArray = condArrayPtr->getPointer(0);
    size_t numTuples = dataArrayPtr->getNumberOfTuples();

    for(size_t i = 0; i < numTuples; i++)
    {
      if(condArray[i] == true)
      {
        DREAM3D_REQUIRE_EQUAL(dataArray[i], 5.0);
      }
      else if(condArray[i] == false)
      {
        DREAM3D_REQUIRE_EQUAL(dataArray[i], 10.0);
      }
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
  void setPropertyAndCheckNE(ConditionalSetValue* filter, double replaceValue, const DataArrayPath& selectedArrayPath, const DataArrayPath& conditionalArrayPath, int32_t errCompare)
  {
    filter->setSelectedArrayPath(selectedArrayPath);

    filter->setConditionalArrayPath(conditionalArrayPath);

    filter->setReplaceValue(replaceValue);

    filter->execute();
    int32_t err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, errCompare);
  }

  // -----------------------------------------------------------------------------
  template <typename T>
  void setPropertyAndCheckEQ(ConditionalSetValue* filter, DataContainer::Pointer dc, double replaceValue, const DataArrayPath& selectedArrayPath, const DataArrayPath& conditionalArrayPath)
  {
    filter->setSelectedArrayPath(selectedArrayPath);

    filter->setConditionalArrayPath(conditionalArrayPath);

    filter->setReplaceValue(replaceValue);
    filter->execute();
    int32_t err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    IDataArray::Pointer dataArray = dc->getAttributeMatrix(selectedArrayPath.getAttributeMatrixName())->getAttributeArray(selectedArrayPath.getDataArrayName());
    IDataArray::Pointer condArray = dc->getAttributeMatrix(conditionalArrayPath.getAttributeMatrixName())->getAttributeArray(conditionalArrayPath.getDataArrayName());
    validateReplacedValues<T>(dataArray, condArray);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void validateReplaceValue(ConditionalSetValue* filter, DataContainerArray::Pointer dca)
  {
    DataContainer::Pointer dc = dca->getDataContainer("ConditionalSetValueTest");
    IDataArray::Pointer dataArray;
    IDataArray::Pointer condArray;

    DataArrayPath attrMat_uint8_3("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "uint8_t3");
    DataArrayPath attrMat_int8_3("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "int8_t3");
    DataArrayPath attrMat_uint16_3("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "uint16_t3");
    DataArrayPath attrMat_int16_3("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "int16_t3");
    DataArrayPath attrMat_uint32_3("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "uint32_t3");
    DataArrayPath attrMat_int32_3("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "int32_t3");
    DataArrayPath attrMat_uint64_3("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "uint64t3");
    DataArrayPath attrMat_int64_3("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "int64_t3");
    DataArrayPath attrMat_float_3("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "float3");
    DataArrayPath attrMat_double_3("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "double3");
    DataArrayPath attrMat_bool_3("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "bool3");

    DataArrayPath attrMat_uint8_1("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "uint8_t1");
    DataArrayPath attrMat_int8_1("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "int8_t1");
    DataArrayPath attrMat_uint16_1("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "uint16_t1");
    DataArrayPath attrMat_int16_1("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "int16_t1");
    DataArrayPath attrMat_uint32_1("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "uint32_t1");
    DataArrayPath attrMat_int32_1("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "int32_t1");
    DataArrayPath attrMat_uint64_1("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "uint64_t1");
    DataArrayPath attrMat_int64_1("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "int64_t1");
    DataArrayPath attrMat_float_1("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "float1");
    DataArrayPath attrMat_double_1("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "double1");
    DataArrayPath attrMat_bool_1("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "bool");

    DataArrayPath conditionalArray("ConditionalSetValueTest", "ConditionalSetValueAttrMat", "ConditionalArray");

    // Fail if the replace value is out of range
    setPropertyAndCheckNE(filter, 256.0, attrMat_uint8_1, conditionalArray, -100);
    setPropertyAndCheckNE(filter, 128.0, attrMat_int8_1, conditionalArray, -100);
    setPropertyAndCheckNE(filter, 65536.0, attrMat_uint16_1, conditionalArray, -100);
    setPropertyAndCheckNE(filter, 32768.0, attrMat_int16_1, conditionalArray, -100);
    setPropertyAndCheckNE(filter, 4294967296.0, attrMat_uint32_1, conditionalArray, -100);
    setPropertyAndCheckNE(filter, 2147483648.0, attrMat_int32_1, conditionalArray, -100);
    setPropertyAndCheckNE(filter, 20000000000000000000.0, attrMat_uint64_1, conditionalArray, -100);
    setPropertyAndCheckNE(filter, 10000000000000000000.0, attrMat_int64_1, conditionalArray, -100);
    setPropertyAndCheckNE(filter, 3.41e38, attrMat_float_1, conditionalArray, -101);
    // not checking double, because cannot make a value outside of the range

    // Succeed for all possible test combinations
    setPropertyAndCheckEQ<uint8_t>(filter, dc, 5.0, attrMat_uint8_1, conditionalArray);
    setPropertyAndCheckEQ<int8_t>(filter, dc, 5.0, attrMat_int8_1, conditionalArray);
    setPropertyAndCheckEQ<uint16_t>(filter, dc, 5.0, attrMat_uint16_1, conditionalArray);
    setPropertyAndCheckEQ<int16_t>(filter, dc, 5.0, attrMat_int16_1, conditionalArray);
    setPropertyAndCheckEQ<uint32_t>(filter, dc, 5.0, attrMat_uint32_1, conditionalArray);
    setPropertyAndCheckEQ<int32_t>(filter, dc, 5.0, attrMat_int32_1, conditionalArray);
    setPropertyAndCheckEQ<uint64_t>(filter, dc, 5.0, attrMat_uint64_1, conditionalArray);
    setPropertyAndCheckEQ<int64_t>(filter, dc, 5.0, attrMat_int64_1, conditionalArray);
    setPropertyAndCheckEQ<float>(filter, dc, 5.0, attrMat_float_1, conditionalArray);
    setPropertyAndCheckEQ<double>(filter, dc, 5.0, attrMat_double_1, conditionalArray);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int ReplaceValueTest()
  {
    DataContainerArray::Pointer dca = initializeDataContainerArray();

    ConditionalSetValue::Pointer conditionalSetValueFilter = ConditionalSetValue::New();
    DREAM3D_REQUIRE(conditionalSetValueFilter.get() != nullptr)

    conditionalSetValueFilter->setDataContainerArray(dca);

    validateReplaceValue(conditionalSetValueFilter.get(), dca);

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ConditionalSetValueTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());
    DREAM3D_REGISTER_TEST(ReplaceValueTest())
  }

private:
  ConditionalSetValueTest(const ConditionalSetValueTest&); // Copy Constructor Not Implemented
  void operator=(const ConditionalSetValueTest&);          // Move assignment Not Implemented
};
