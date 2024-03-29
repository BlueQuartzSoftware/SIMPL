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
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
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

#define CREATE_DATA_ARRAY(type, attrMat, tDims, cDimsVec, cDimsScalar, vecName, scalarName, err)                                                                                                       \
  DataArray<type>::Pointer _##type##VectorArray = DataArray<type>::CreateArray(tDims, cDimsVec, #vecName, true);                                                                                       \
  err = attrMat->insertOrAssign(_##type##VectorArray);                                                                                                                                                 \
  _##type##VectorArray->initializeWithZeros();                                                                                                                                                         \
  DREAM3D_REQUIRE(err >= 0);                                                                                                                                                                           \
  DataArray<type>::Pointer _##type##ScalarArray = DataArray<type>::CreateArray(tDims, cDimsScalar, #scalarName, true);                                                                                 \
  err = attrMat->insertOrAssign(_##type##ScalarArray);                                                                                                                                                 \
  _##type##ScalarArray->initializeWithZeros();                                                                                                                                                         \
  DREAM3D_REQUIRE(err >= 0);

class CombineAttributeArraysTest
{
public:
  CombineAttributeArraysTest() = default;
  virtual ~CombineAttributeArraysTest() = default;
  /**
   * @brief Returns the name of the class for CombineAttributeArraysTest
   */
  /**
   * @brief Returns the name of the class for CombineAttributeArraysTest
   */
  QString getNameOfClass() const
  {
    return QString("CombineAttributeArraysTest");
  }

  /**
   * @brief Returns the name of the class for CombineAttributeArraysTest
   */
  QString ClassName()
  {
    return QString("CombineAttributeArraysTest");
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  void initializeData(IDataArray::Pointer inputPtr)
  {
    auto inputDataPtr = std::dynamic_pointer_cast<DataArray<T>>(inputPtr);
    T* inputData = static_cast<T*>(inputDataPtr->getPointer(0));

    size_t numTuples = inputPtr->getNumberOfTuples();
    int32_t numComps = inputPtr->getNumberOfComponents();
    QString type = inputDataPtr->getTypeAsString();

    for(size_t i = 0; i < numTuples; i++)
    {
      if(!type.compare(SIMPL::TypeNames::Bool))
      {
        if(numComps == 1)
        {
          inputData[i] = true;
        }
        else
        {
          for(int32_t j = 0; j < numComps; j++)
          {
            inputData[numComps * i + j] = false;
          }
        }
      }
      else
      {
        for(int32_t j = 0; j < numComps; j++)
        {
          inputData[numComps * i + j] = j;
        }
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer initializeDataContainerArray()
  {
    int32_t err = 0;

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer m = DataContainer::New("CombineAttributeArraysTest");

    std::vector<size_t> tDims(1, 100);
    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, "CombineAttributeArraysTest", AttributeMatrix::Type::Cell);
    m->addOrReplaceAttributeMatrix(attrMat);
    dca->addOrReplaceDataContainer(m);

    std::vector<size_t> cDimsVec(1, 3);
    std::vector<size_t> cDimsScalar(1, 1);

    CREATE_DATA_ARRAY(uint8_t, attrMat, tDims, cDimsVec, cDimsScalar, uint8_tVectorArray, uint8_tScalarArray, err);
    CREATE_DATA_ARRAY(int8_t, attrMat, tDims, cDimsVec, cDimsScalar, int8_tVectorArray, int8_tScalarArray, err);
    CREATE_DATA_ARRAY(uint16_t, attrMat, tDims, cDimsVec, cDimsScalar, uint16_tVectorArray, uint16_tScalarArray, err);
    CREATE_DATA_ARRAY(int16_t, attrMat, tDims, cDimsVec, cDimsScalar, int16_tVectorArray, int16_tScalarArray, err);
    CREATE_DATA_ARRAY(uint32_t, attrMat, tDims, cDimsVec, cDimsScalar, uint32_tVectorArray, uint32_tScalarArray, err);
    CREATE_DATA_ARRAY(int32_t, attrMat, tDims, cDimsVec, cDimsScalar, int32_tVectorArray, int32_tScalarArray, err);
    CREATE_DATA_ARRAY(uint64_t, attrMat, tDims, cDimsVec, cDimsScalar, uint64_tVectorArray, uint64_tScalarArray, err);
    CREATE_DATA_ARRAY(int64_t, attrMat, tDims, cDimsVec, cDimsScalar, int64_tVectorArray, int64_tScalarArray, err);
    CREATE_DATA_ARRAY(double, attrMat, tDims, cDimsVec, cDimsScalar, doubleVectorArray, doubleScalarArray, err);
    CREATE_DATA_ARRAY(float, attrMat, tDims, cDimsVec, cDimsScalar, floatVectorArray, floatScalarArray, err);
    CREATE_DATA_ARRAY(bool, attrMat, tDims, cDimsVec, cDimsScalar, boolVectorArray, boolScalarArray, err);

    QStringList attrArrayNames = attrMat->getAttributeArrayNames();

    for(auto iter = attrArrayNames.begin(); iter != attrArrayNames.end(); ++iter)
    {
      IDataArray::Pointer tmpPtr = attrMat->getAttributeArray(*iter);
      DREAM3D_REQUIRE(tmpPtr.get() != nullptr);
      EXECUTE_FUNCTION_TEMPLATE(AbstractFilter::NullPointer(), initializeData, tmpPtr, tmpPtr)
    }

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  void checkCombineAttributeArraysFilter(AbstractFilter::Pointer filter, DataArrayPath vectorArrayPath, DataArrayPath scalarArrayPath)
  {
    int32_t err = 0;

    std::vector<DataArrayPath> dataArrayPathsVecFirst = {vectorArrayPath, scalarArrayPath};
    std::vector<DataArrayPath> dataArrayPathsScalarFirst = {scalarArrayPath, vectorArrayPath};

    bool isBool = false;

    T checkVecFirst[4];
    T checkScalarFirst[4];

    T value = static_cast<T>(0);
    if(typeid(value) == typeid(bool))
    {
      isBool = true;
    }

    if(isBool)
    {
      checkVecFirst[0] = false;
      checkVecFirst[1] = false;
      checkVecFirst[2] = false;
      checkVecFirst[3] = true;
      checkScalarFirst[0] = true;
      checkScalarFirst[1] = false;
      checkScalarFirst[2] = false;
      checkScalarFirst[3] = false;
    }
    else
    {
      checkVecFirst[0] = static_cast<T>(0);
      checkVecFirst[1] = static_cast<T>(1);
      checkVecFirst[2] = static_cast<T>(2);
      checkVecFirst[3] = static_cast<T>(0);
      checkScalarFirst[0] = static_cast<T>(0);
      checkScalarFirst[1] = static_cast<T>(0);
      checkScalarFirst[2] = static_cast<T>(1);
      checkScalarFirst[3] = static_cast<T>(2);
    }

    QVariant var;
    bool propWasSet = false;

    bool standardize = false;

    var.setValue(standardize);
    propWasSet = filter->setProperty("NormalizeData", var);
    if(false == propWasSet)
    {
      qDebug() << "Unable to set property NormalizeData";
    }

    var.setValue(dataArrayPathsVecFirst);
    propWasSet = filter->setProperty("SelectedDataArrayPaths", var);
    if(false == propWasSet)
    {
      qDebug() << "Unable to set property SelectedDataArrayPaths";
    }

    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    auto outputArrayPtr = filter->getDataContainerArray()->getDataContainer("CombineAttributeArraysTest")->getAttributeMatrix("CombineAttributeArraysTest")->getAttributeArray("CombinedData");
    auto outputArray = std::dynamic_pointer_cast<DataArray<T>>(outputArrayPtr);
    T* outputData = static_cast<T*>(outputArray->getPointer(0));

    size_t numTuples = outputArrayPtr->getNumberOfTuples();
    int32_t numComps = outputArrayPtr->getNumberOfComponents();

    for(size_t i = 0; i < numTuples; i++)
    {
      for(int32_t j = 0; j < numComps; j++)
      {
        DREAM3D_REQUIRE_EQUAL(outputData[numComps * i + j], checkVecFirst[j]);
      }
    }

    filter->getDataContainerArray()->getDataContainer("CombineAttributeArraysTest")->getAttributeMatrix("CombineAttributeArraysTest")->removeAttributeArray("CombinedData");

    var.setValue(dataArrayPathsScalarFirst);
    propWasSet = filter->setProperty("SelectedDataArrayPaths", var);
    if(false == propWasSet)
    {
      qDebug() << "Unable to set property SelectedDataArrayPaths";
    }

    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    outputArrayPtr = filter->getDataContainerArray()->getDataContainer("CombineAttributeArraysTest")->getAttributeMatrix("CombineAttributeArraysTest")->getAttributeArray("CombinedData");
    outputArray = std::dynamic_pointer_cast<DataArray<T>>(outputArrayPtr);
    outputData = static_cast<T*>(outputArray->getPointer(0));

    numTuples = outputArrayPtr->getNumberOfTuples();
    numComps = outputArrayPtr->getNumberOfComponents();

    for(size_t i = 0; i < numTuples; i++)
    {
      for(int32_t j = 0; j < numComps; j++)
      {
        DREAM3D_REQUIRE_EQUAL(outputData[numComps * i + j], checkScalarFirst[j]);
      }
    }

    filter->getDataContainerArray()->getDataContainer("CombineAttributeArraysTest")->getAttributeMatrix("CombineAttributeArraysTest")->removeAttributeArray("CombinedData");

    standardize = true;

    var.setValue(standardize);
    propWasSet = filter->setProperty("NormalizeData", var);
    if(false == propWasSet)
    {
      qDebug() << "Unable to set property NormalizeData";
    }

    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    outputArrayPtr = filter->getDataContainerArray()->getDataContainer("CombineAttributeArraysTest")->getAttributeMatrix("CombineAttributeArraysTest")->getAttributeArray("CombinedData");
    outputArray = std::dynamic_pointer_cast<DataArray<T>>(outputArrayPtr);
    outputData = static_cast<T*>(outputArray->getPointer(0));

    numTuples = outputArrayPtr->getNumberOfTuples();
    numComps = outputArrayPtr->getNumberOfComponents();

    for(size_t i = 0; i < numTuples; i++)
    {
      for(int32_t j = 0; j < numComps; j++)
      {
        if(outputData[numComps * i + j] < static_cast<T>(0) || outputData[numComps * i + j] > static_cast<T>(1))
        {
          err = -1;
        }
        DREAM3D_REQUIRE_EQUAL(err, 0);
      }
    }

    filter->getDataContainerArray()->getDataContainer("CombineAttributeArraysTest")->getAttributeMatrix("CombineAttributeArraysTest")->removeAttributeArray("CombinedData");
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCombineAttributeArraysFilter()
  {
    DataContainerArray::Pointer dca = initializeDataContainerArray();

    QString filtName = "CombineAttributeArrays";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr);

    AbstractFilter::Pointer filter = factory->create();
    DREAM3D_REQUIRE(filter.get() != nullptr);

    filter->setDataContainerArray(dca);

    QVariant var;
    bool propWasSet = false;

    QString outputArrayName = "CombinedData";
    var.setValue(outputArrayName);
    propWasSet = filter->setProperty("StackedDataArrayName", var);
    if(false == propWasSet)
    {
      qDebug() << "Unable to set property StackedDataArrayName";
      DREAM3D_REQUIRE_EQUAL(0, 1);
    }

    DataArrayPath path1("CombineAttributeArraysTest", "CombineAttributeArraysTest", "uint8_tVectorArray");
    DataArrayPath path2("CombineAttributeArraysTest", "CombineAttributeArraysTest", "uint8_tScalarArray");
    checkCombineAttributeArraysFilter<uint8_t>(filter, path1, path2);

    path1.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "int8_tVectorArray");
    path2.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "int8_tScalarArray");
    checkCombineAttributeArraysFilter<int8_t>(filter, path1, path2);

    path1.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "uint16_tVectorArray");
    path2.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "uint16_tScalarArray");
    checkCombineAttributeArraysFilter<uint16_t>(filter, path1, path2);

    path1.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "int16_tVectorArray");
    path2.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "int16_tScalarArray");
    checkCombineAttributeArraysFilter<int16_t>(filter, path1, path2);

    path1.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "uint32_tVectorArray");
    path2.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "uint32_tScalarArray");
    checkCombineAttributeArraysFilter<uint32_t>(filter, path1, path2);

    path1.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "int32_tVectorArray");
    path2.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "int32_tScalarArray");
    checkCombineAttributeArraysFilter<int32_t>(filter, path1, path2);

    path1.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "uint64_tVectorArray");
    path2.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "uint64_tScalarArray");
    checkCombineAttributeArraysFilter<uint64_t>(filter, path1, path2);

    path1.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "int64_tVectorArray");
    path2.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "int64_tScalarArray");
    checkCombineAttributeArraysFilter<int64_t>(filter, path1, path2);

    path1.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "floatVectorArray");
    path2.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "floatScalarArray");
    checkCombineAttributeArraysFilter<float>(filter, path1, path2);

    path1.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "doubleVectorArray");
    path2.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "doubleScalarArray");
    checkCombineAttributeArraysFilter<double>(filter, path1, path2);

    path1.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "boolVectorArray");
    path2.update("CombineAttributeArraysTest", "CombineAttributeArraysTest", "boolScalarArray");
    checkCombineAttributeArraysFilter<bool>(filter, path1, path2);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### CombineAttributeArraysTest Starting ####" << std::endl;
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestCombineAttributeArraysFilter())
  }

private:
  CombineAttributeArraysTest(const CombineAttributeArraysTest&); // Copy Constructor Not Implemented
  void operator=(const CombineAttributeArraysTest&);             // Move assignment Not Implemented
};
