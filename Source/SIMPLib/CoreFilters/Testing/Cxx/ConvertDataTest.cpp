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

#include <assert.h>
#include <stdint.h>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/ConvertData.h"
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

class ConvertDataTest
{
public:
  ConvertDataTest() = default;
  virtual ~ConvertDataTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray(SIMPL::NumericTypes::Type dataType)
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New("DataContainer");
    dca->addDataContainer(dc);

    QVector<size_t> dims;
    dims.push_back(2);
    AttributeMatrix::Pointer am = AttributeMatrix::New(dims, "AttributeMatrix", AttributeMatrix::Type::Any);
    dc->addAttributeMatrix("AttributeMatrix", am);

    QVector<size_t> cdims;
    cdims.push_back(2);
    IDataArray::Pointer da;

    switch(dataType)
    {
    case SIMPL::NumericTypes::Type::Int8:
      da = DataArray<int8_t>::CreateArray(2, cdims, "DataArray");
      break;
    case SIMPL::NumericTypes::Type::UInt8:
      da = DataArray<uint8_t>::CreateArray(2, cdims, "DataArray");
      break;
    case SIMPL::NumericTypes::Type::Int16:
      da = DataArray<int16_t>::CreateArray(2, cdims, "DataArray");
      break;
    case SIMPL::NumericTypes::Type::UInt16:
      da = DataArray<uint16_t>::CreateArray(2, cdims, "DataArray");
      break;
    case SIMPL::NumericTypes::Type::Int32:
      da = DataArray<int32_t>::CreateArray(2, cdims, "DataArray");
      break;
    case SIMPL::NumericTypes::Type::UInt32:
      da = DataArray<uint32_t>::CreateArray(2, cdims, "DataArray");
      break;
    case SIMPL::NumericTypes::Type::Int64:
      da = DataArray<int64_t>::CreateArray(2, cdims, "DataArray");
      break;
    case SIMPL::NumericTypes::Type::UInt64:
      da = DataArray<uint64_t>::CreateArray(2, cdims, "DataArray");
      break;
    case SIMPL::NumericTypes::Type::Float:
      da = DataArray<float>::CreateArray(2, cdims, "DataArray");
      break;
    case SIMPL::NumericTypes::Type::Double:
      da = DataArray<double>::CreateArray(2, cdims, "DataArray");
      break;
    case SIMPL::NumericTypes::Type::Bool:
      da = DataArray<bool>::CreateArray(2, cdims, "DataArray");
      break;
    case SIMPL::NumericTypes::Type::UnknownNumType:
      break;
    }
    da->initializeWithZeros();
    am->addAttributeArray("DataArray", da);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  ConvertData::Pointer createFilter()
  {
    // Now instantiate the ConvertData Filter from the FilterManager
    QString filtName = "ConvertData";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The ConvertDataTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    ConvertData::Pointer filter = std::dynamic_pointer_cast<ConvertData>(filterFactory->create());
    DREAM3D_REQUIRE(nullptr != filter.get());

    return filter;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setValues(ConvertData::Pointer filter, QString dataArray, SIMPL::NumericTypes::Type type, QString outputArrayName)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    int typei = static_cast<int>(type);
    value.setValue(typei);
    filter->setProperty("ScalarType", value);

    value.setValue(outputArrayName);
    filter->setProperty("OutputArrayName", value);

    value.setValue(DataArrayPath("DataContainer", "AttributeMatrix", dataArray));
    filter->setProperty("SelectedCellArrayPath", value);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> std::shared_ptr<DataArray<T>> getDataArray(AttributeMatrix::Pointer am, QString dataArray)
  {
    typename DataArray<T>::Pointer da = std::dynamic_pointer_cast<DataArray<T>>(am->getAttributeArray(dataArray));

    return da;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T, typename U> void checkConvertedArray(std::shared_ptr<DataArray<T>> originalDataArray, std::shared_ptr<DataArray<U>> convertedDataArray)
  {
    DREAM3D_ASSERT(nullptr != originalDataArray.get());
    DREAM3D_ASSERT(nullptr != convertedDataArray.get());

    DREAM3D_REQUIRE_EQUAL(originalDataArray->getNumberOfComponents(), convertedDataArray->getNumberOfComponents());

    int componentDims = static_cast<int>(originalDataArray->getNumberOfTuples()) / originalDataArray->getNumberOfComponents();

    for(size_t i = 0; i < originalDataArray->getNumberOfTuples(); i++)
    {
      for(int j = 0; j < componentDims; j++)
      {
        T value1 = originalDataArray->getComponent(i, j);
        U value2 = convertedDataArray->getComponent(i, j);
        if(static_cast<U>(value1) != value2)
        {
          std::cout << "orig: " << value1 << "  converted: " << value2 << std::endl;
        }
        DREAM3D_REQUIRE_EQUAL(static_cast<U>(value1), value2);
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T, typename U>
  void TestConversion(ConvertData::Pointer filter, QString arrayName, SIMPL::NumericTypes::Type newType, QString newArrayName, int errorCode = 0, bool checkArray = true)
  {
    setValues(filter, arrayName, newType, newArrayName);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);

    if(checkArray)
    {
      DataContainerArray::Pointer dca = filter->getDataContainerArray();
      DataContainer::Pointer dc = dca->getDataContainer("DataContainer");
      AttributeMatrix::Pointer am = dc->getAttributeMatrix("AttributeMatrix");

      typename DataArray<T>::Pointer dataArray = getDataArray<T>(am, arrayName);
      typename DataArray<U>::Pointer convertedDataArray = getDataArray<U>(am, newArrayName);

      DREAM3D_REQUIRE(nullptr != dataArray.get());
      DREAM3D_REQUIRE(nullptr != convertedDataArray.get());

      checkConvertedArray<T, U>(dataArray, convertedDataArray);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInt8Signed()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Int8;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<int8_t, int8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int8, "NewArrayChar", 0);
    TestConversion<int8_t, int16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int16, "NewArrayShort", 0);
    TestConversion<int8_t, int32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int32, "NewArrayInt", 0);
    TestConversion<int8_t, int64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int64, "NewArrayLong", 0);

    TestConversion<int8_t, float>(filter, "DataArray", SIMPL::NumericTypes::Type::Float, "NewArrayFloat", 0);
    TestConversion<int8_t, double>(filter, "DataArray", SIMPL::NumericTypes::Type::Double, "NewArrayDouble", 0);
    TestConversion<int8_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInt8Unsigned()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Int8;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<int8_t, uint8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt8, "NewArrayUChar", 0);
    TestConversion<int8_t, uint16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt16, "NewArrayUShort", 0);
    TestConversion<int8_t, uint32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt32, "NewArrayUInt", 0);
    TestConversion<int8_t, uint64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt64, "NewArrayULong", 0);
    TestConversion<int8_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestUInt8Signed()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::UInt8;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<uint8_t, int8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int8, "NewArrayChar", 0);
    TestConversion<uint8_t, int16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int16, "NewArrayShort", 0);
    TestConversion<uint8_t, int32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int32, "NewArrayInt", 0);
    TestConversion<uint8_t, int64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int64, "NewArrayLong", 0);

    TestConversion<uint8_t, float>(filter, "DataArray", SIMPL::NumericTypes::Type::Float, "NewArrayFloat", 0);
    TestConversion<uint8_t, double>(filter, "DataArray", SIMPL::NumericTypes::Type::Double, "NewArrayDouble", 0);
    TestConversion<uint8_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestUInt8Unsigned()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::UInt8;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<uint8_t, uint8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt8, "NewArrayUChar", 0);
    TestConversion<uint8_t, uint16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt16, "NewArrayUShort", 0);
    TestConversion<uint8_t, uint32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt32, "NewArrayUInt", 0);
    TestConversion<uint8_t, uint64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt64, "NewArrayULong", 0);
    TestConversion<uint8_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInt16Signed()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Int16;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<int16_t, int8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int8, "NewArrayChar", 0);
    TestConversion<int16_t, int16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int16, "NewArrayShort", 0);
    TestConversion<int16_t, int32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int32, "NewArrayInt", 0);
    TestConversion<int16_t, int64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int64, "NewArrayLong", 0);

    TestConversion<int16_t, float>(filter, "DataArray", SIMPL::NumericTypes::Type::Float, "NewArrayFloat", 0);
    TestConversion<int16_t, double>(filter, "DataArray", SIMPL::NumericTypes::Type::Double, "NewArrayDouble", 0);
    TestConversion<int16_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInt16Unsigned()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Int16;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<int16_t, uint8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt8, "NewArrayUChar", 0);
    TestConversion<int16_t, uint16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt16, "NewArrayUShort", 0);
    TestConversion<int16_t, uint32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt32, "NewArrayUInt", 0);
    TestConversion<int16_t, uint64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt64, "NewArrayULong", 0);
    TestConversion<int16_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestUInt16Signed()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::UInt16;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<uint16_t, int8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int8, "NewArrayChar", 0);
    TestConversion<uint16_t, int16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int16, "NewArrayShort", 0);
    TestConversion<uint16_t, int32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int32, "NewArrayInt", 0);
    TestConversion<uint16_t, int64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int64, "NewArrayLong", 0);

    TestConversion<uint16_t, float>(filter, "DataArray", SIMPL::NumericTypes::Type::Float, "NewArrayFloat", 0);
    TestConversion<uint16_t, double>(filter, "DataArray", SIMPL::NumericTypes::Type::Double, "NewArrayDouble", 0);
    TestConversion<uint16_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestUInt16Unsigned()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::UInt16;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<uint16_t, uint8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt8, "NewArrayUChar", 0);
    TestConversion<uint16_t, uint16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt16, "NewArrayUShort", 0);
    TestConversion<uint16_t, uint32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt32, "NewArrayUInt", 0);
    TestConversion<uint16_t, uint64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt64, "NewArrayULong", 0);
    TestConversion<uint16_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInt32Signed()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Int32;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<int32_t, int8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int8, "NewArrayChar", 0);
    TestConversion<int32_t, int16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int16, "NewArrayShort", 0);
    TestConversion<int32_t, int32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int32, "NewArrayInt", 0);
    TestConversion<int32_t, int64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int64, "NewArrayLong", 0);

    TestConversion<int32_t, float>(filter, "DataArray", SIMPL::NumericTypes::Type::Float, "NewArrayFloat", 0);
    TestConversion<int32_t, double>(filter, "DataArray", SIMPL::NumericTypes::Type::Double, "NewArrayDouble", 0);
    TestConversion<int32_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInt32Unsigned()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Int32;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<int32_t, uint8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt8, "NewArrayUChar", 0);
    TestConversion<int32_t, uint16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt16, "NewArrayUShort", 0);
    TestConversion<int32_t, uint32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt32, "NewArrayUInt", 0);
    TestConversion<int32_t, uint64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt64, "NewArrayULong", 0);
    TestConversion<int32_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestUInt32Signed()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::UInt32;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<uint32_t, int8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int8, "NewArrayChar", 0);
    TestConversion<uint32_t, int16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int16, "NewArrayShort", 0);
    TestConversion<uint32_t, int32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int32, "NewArrayInt", 0);
    TestConversion<uint32_t, int64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int64, "NewArrayLong", 0);

    TestConversion<uint32_t, float>(filter, "DataArray", SIMPL::NumericTypes::Type::Float, "NewArrayFloat", 0);
    TestConversion<uint32_t, double>(filter, "DataArray", SIMPL::NumericTypes::Type::Double, "NewArrayDouble", 0);
    TestConversion<uint32_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestUInt32Unsigned()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::UInt32;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<uint32_t, uint8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt8, "NewArrayUChar", 0);
    TestConversion<uint32_t, uint16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt16, "NewArrayUShort", 0);
    TestConversion<uint32_t, uint32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt32, "NewArrayUInt", 0);
    TestConversion<uint32_t, uint64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt64, "NewArrayULong", 0);
    TestConversion<uint32_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInt64Signed()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Int64;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<int64_t, int8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int8, "NewArrayChar", 0);
    TestConversion<int64_t, int16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int16, "NewArrayShort", 0);
    TestConversion<int64_t, int32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int32, "NewArrayInt", 0);
    TestConversion<int64_t, int64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int64, "NewArrayLong", 0);

    TestConversion<int64_t, float>(filter, "DataArray", SIMPL::NumericTypes::Type::Float, "NewArrayFloat", 0);
    TestConversion<int64_t, double>(filter, "DataArray", SIMPL::NumericTypes::Type::Double, "NewArrayDouble", 0);
    TestConversion<int64_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInt64Unsigned()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Int64;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<int64_t, uint8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt8, "NewArrayUChar", 0);
    TestConversion<int64_t, uint16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt16, "NewArrayUShort", 0);
    TestConversion<int64_t, uint32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt32, "NewArrayUInt", 0);
    TestConversion<int64_t, uint64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt64, "NewArrayULong", 0);
    TestConversion<int64_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestUInt64Signed()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::UInt64;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<uint64_t, int8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int8, "NewArrayChar", 0);
    TestConversion<uint64_t, int16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int16, "NewArrayShort", 0);
    TestConversion<uint64_t, int32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int32, "NewArrayInt", 0);
    TestConversion<uint64_t, int64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int64, "NewArrayLong", 0);

    TestConversion<uint64_t, float>(filter, "DataArray", SIMPL::NumericTypes::Type::Float, "NewArrayFloat", 0);
    TestConversion<uint64_t, double>(filter, "DataArray", SIMPL::NumericTypes::Type::Double, "NewArrayDouble", 0);
    TestConversion<uint64_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestUInt64Unsigned()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::UInt64;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<uint64_t, uint8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt8, "NewArrayUChar", 0);
    TestConversion<uint64_t, uint16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt16, "NewArrayUShort", 0);
    TestConversion<uint64_t, uint32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt32, "NewArrayUInt", 0);
    TestConversion<uint64_t, uint64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::UInt64, "NewArrayULong", 0);
    TestConversion<uint64_t, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestFloat()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Float;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<float, int8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int8, "NewArrayChar", 0);
    TestConversion<float, int16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int16, "NewArrayShort", 0);
    TestConversion<float, int32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int32, "NewArrayInt", 0);
    TestConversion<float, int64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int64, "NewArrayLong", 0);

    TestConversion<float, float>(filter, "DataArray", SIMPL::NumericTypes::Type::Float, "NewArrayFloat", 0);
    TestConversion<float, double>(filter, "DataArray", SIMPL::NumericTypes::Type::Double, "NewArrayDouble", 0);
    TestConversion<float, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestDouble()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Double;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    TestConversion<double, int8_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int8, "NewArrayChar", 0);
    TestConversion<double, int16_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int16, "NewArrayShort", 0);
    TestConversion<double, int32_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int32, "NewArrayInt", 0);
    TestConversion<double, int64_t>(filter, "DataArray", SIMPL::NumericTypes::Type::Int64, "NewArrayLong", 0);

    TestConversion<double, float>(filter, "DataArray", SIMPL::NumericTypes::Type::Float, "NewArrayFloat", 0);
    TestConversion<double, double>(filter, "DataArray", SIMPL::NumericTypes::Type::Double, "NewArrayDouble", 0);
    TestConversion<double, bool>(filter, "DataArray", SIMPL::NumericTypes::Type::Bool, "NewArrayBool", 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInvalidDataArray()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Int8;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    QString newArrayName = "NewArray";

    setValues(filter, "Array1", SIMPL::NumericTypes::Type::Int8, newArrayName);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -90002);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestOverwriteArray()
  {
    SIMPL::NumericTypes::Type baseType = SIMPL::NumericTypes::Type::Int8;

    ConvertData::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray(baseType));

    QString newArrayName = "DataArray";

    setValues(filter, "DataArray", SIMPL::NumericTypes::Type::Int8, newArrayName);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ConvertDataTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestInt8Signed());
    DREAM3D_REGISTER_TEST(TestInt8Unsigned());

    DREAM3D_REGISTER_TEST(TestInt16Signed());
    DREAM3D_REGISTER_TEST(TestInt16Unsigned());

    DREAM3D_REGISTER_TEST(TestInt32Signed());
    DREAM3D_REGISTER_TEST(TestInt32Unsigned());

    DREAM3D_REGISTER_TEST(TestInt64Signed());
    DREAM3D_REGISTER_TEST(TestInt64Unsigned());

    DREAM3D_REGISTER_TEST(TestUInt8Signed());
    DREAM3D_REGISTER_TEST(TestUInt8Unsigned());

    DREAM3D_REGISTER_TEST(TestUInt16Signed());
    DREAM3D_REGISTER_TEST(TestUInt16Unsigned());

    DREAM3D_REGISTER_TEST(TestUInt32Signed());
    DREAM3D_REGISTER_TEST(TestUInt32Unsigned());

    DREAM3D_REGISTER_TEST(TestUInt64Signed());
    DREAM3D_REGISTER_TEST(TestUInt64Unsigned());

    DREAM3D_REGISTER_TEST(TestFloat());
    DREAM3D_REGISTER_TEST(TestDouble());

    DREAM3D_REGISTER_TEST(TestInvalidDataArray());
    DREAM3D_REGISTER_TEST(TestOverwriteArray());
  }

private:
  ConvertDataTest(const ConvertDataTest&); // Copy Constructor Not Implemented
  void operator=(const ConvertDataTest&);  // Move assignment Not Implemented
};
