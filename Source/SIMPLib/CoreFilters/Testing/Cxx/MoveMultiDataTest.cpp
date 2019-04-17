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

#pragma once

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Filtering/QMetaObjectUtilities.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

#include "SIMPLib/Common/Constants.h"

class MoveMultiDataTest
{

public:
  MoveMultiDataTest() = default;
  ~MoveMultiDataTest() = default;
  MoveMultiDataTest(const MoveMultiDataTest&) = delete;            // Copy Constructor
  MoveMultiDataTest(MoveMultiDataTest&&) = delete;                 // Move Constructor
  MoveMultiDataTest& operator=(const MoveMultiDataTest&) = delete; // Copy Assignment
  MoveMultiDataTest& operator=(MoveMultiDataTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the MoveMultiDataTest Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The MoveMultiDataTest Requires the use of the " << m_FilterName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> std::shared_ptr<DataArray<T>> createDataArray(const QString& daName, const QVector<size_t>& tupleDims, const QVector<size_t>& cDims)
  {
    typename DataArray<T>::Pointer da = DataArray<T>::CreateArray(tupleDims, cDims, daName);
    for(size_t i = 0; i < da->getNumberOfTuples(); i++)
    {
      for(int j = 0; j < da->getNumberOfComponents(); j++)
      {
        da->setComponent(i, j, static_cast<T>(i + j));
      }
    }
    return da;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void checkDataArray(IDataArray::Pointer originalData, IDataArray::Pointer newData, bool isRenamed)
  {
    if(originalData == IDataArray::NullPointer() || newData == IDataArray::NullPointer())
    {
      bool bothNullPtr = (originalData == newData);
      DREAM3D_REQUIRE_EQUAL(bothNullPtr, true)
    }
    else
    {
      if(!isRenamed)
      {
        DREAM3D_REQUIRE_EQUAL(originalData->getName(), newData->getName())
      }

      QString oldType = originalData->getTypeAsString();
      QString newType = newData->getTypeAsString();
      DREAM3D_REQUIRE_EQUAL(newType, oldType)

      DREAM3D_REQUIRE_EQUAL(originalData->getNumberOfTuples(), newData->getNumberOfTuples())
      DREAM3D_REQUIRE_EQUAL(originalData->getNumberOfComponents(), newData->getNumberOfComponents())
      DREAM3D_REQUIRE_EQUAL(originalData->getSize(), newData->getSize())

      bool sameTupleDims = (originalData->getComponentDimensions() == newData->getComponentDimensions());
      DREAM3D_REQUIRE_EQUAL(sameTupleDims, true)

      typename DataArray<T>::Pointer daOriginal = std::dynamic_pointer_cast<DataArray<T>>(originalData);
      typename DataArray<T>::Pointer daNew = std::dynamic_pointer_cast<DataArray<T>>(newData);

      for(int i = 0; i < daOriginal->getSize(); i++)
      {
        T* valueOld = daOriginal->getPointer(i);
        T* valueNew = daNew->getPointer(i);
        DREAM3D_REQUIRE_EQUAL(valueNew[0], valueOld[0])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void checkAttributeMatrix(AttributeMatrix::Pointer oldAM, AttributeMatrix::Pointer newAM, bool isRenamed)
  {
    if(!isRenamed)
    {
      DREAM3D_REQUIRE_EQUAL(newAM->getName(), oldAM->getName())
    }

    bool sameTupleDims = (newAM->getTupleDimensions() == oldAM->getTupleDimensions());
    DREAM3D_REQUIRE_EQUAL(sameTupleDims, true)

    DREAM3D_REQUIRE_EQUAL(newAM->getNumberOfTuples(), oldAM->getNumberOfTuples())

    bool sameType = (newAM->getType() == oldAM->getType());
    DREAM3D_REQUIRE_EQUAL(sameType, true)

    QList<QString> oldArrayNames = oldAM->getAttributeArrayNames();
    QList<QString> newArrayNames = newAM->getAttributeArrayNames();

    bool sameArrayNames = (newArrayNames == oldArrayNames);
    DREAM3D_REQUIRE_EQUAL(sameArrayNames, true)

    for(int i = 0; i < newArrayNames.size(); i++)
    {
      IDataArray::Pointer oldArray = oldAM->getAttributeArray(oldArrayNames[i]);
      IDataArray::Pointer newArray = newAM->getAttributeArray(newArrayNames[i]);

      QString oldType = oldArray->getTypeAsString();
      QString newType = newArray->getTypeAsString();
      DREAM3D_REQUIRE_EQUAL(newType, oldType)

      if(newType == "int8_t")
      {
        checkDataArray<int8_t>(oldArray, newArray, false);
      }
      else if(newType == "uint8_t")
      {
        checkDataArray<uint8_t>(oldArray, newArray, false);
      }
      else if(newType == "int16_t")
      {
        checkDataArray<int16_t>(oldArray, newArray, false);
      }
      else if(newType == "uint16_t")
      {
        checkDataArray<uint16_t>(oldArray, newArray, false);
      }
      else if(newType == "int32_t")
      {
        checkDataArray<int32_t>(oldArray, newArray, false);
      }
      else if(newType == "uint32_t")
      {
        checkDataArray<uint32_t>(oldArray, newArray, false);
      }
      else if(newType == "int64_t")
      {
        checkDataArray<int64_t>(oldArray, newArray, false);
      }
      else if(newType == "uint64_t")
      {
        checkDataArray<uint64_t>(oldArray, newArray, false);
      }
      else if(newType == "float")
      {
        checkDataArray<float>(oldArray, newArray, false);
      }
      else if(newType == "double")
      {
        checkDataArray<double>(oldArray, newArray, false);
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCase(DataContainerArray::Pointer dca, QVector<DataArrayPath> sources, DataArrayPath dapDst, int whatToMove, int err)
  {

    QVector<AttributeMatrix::Pointer> amSourcesCopy;
    QVector<IDataArray::Pointer> daSourcesCopy;

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer moveMultiData = filterFactory->create();

    moveMultiData->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    var.setValue(whatToMove);
    bool propWasSet = moveMultiData->setProperty("WhatToMove", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    if(whatToMove == 0) // Move AttributeMatrix
    {
      var.setValue(dapDst.getDataContainerName());
      propWasSet = moveMultiData->setProperty("DataContainerDestination", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(sources);
      propWasSet = moveMultiData->setProperty("AttributeMatrixSources", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      for(int i = 0; i < sources.size(); i++)
      {
        amSourcesCopy.push_back(dca->getDataContainer(sources[i].getDataContainerName())->getAttributeMatrix(sources[i].getAttributeMatrixName())->deepCopy());
      }
    }
    else if(whatToMove == 1) // Move DataArray
    {
      var.setValue(dapDst);
      propWasSet = moveMultiData->setProperty("AttributeMatrixDestination", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(sources);
      propWasSet = moveMultiData->setProperty("DataArraySources", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      for(int i = 0; i < sources.size(); i++)
      {
        daSourcesCopy.push_back(
            dca->getDataContainer(sources[i].getDataContainerName())->getAttributeMatrix(sources[i].getAttributeMatrixName())->getAttributeArray(sources[i].getDataArrayName())->deepCopy());
      }
    }

    // Run Filter

    moveMultiData->execute();

    if(err >= 0)
    {
      DREAM3D_REQUIRED(moveMultiData->getErrorCondition(), >=, err)
    }
    else
    {
      DREAM3D_REQUIRED(moveMultiData->getErrorCondition(), ==, err)
    }

    // Check filter results if filter successfully executed

    if(err >= 0)
    {
      if(whatToMove == 0) // Move AttributeMatrix
      {
        for(int i = 0; i < sources.size(); i++)
        {
          DataContainer::Pointer dc = dca->getDataContainer(sources[i].getDataContainerName());
          DataContainer::Pointer dcDst = dca->getDataContainer(dapDst);

          QList<QString> srcMatrixNames = dc->getAttributeMatrixNames();
          QList<QString> dstMatrixNames = dcDst->getAttributeMatrixNames();

          // Check that it was successfully moved

          DREAM3D_REQUIRE_EQUAL(srcMatrixNames.contains(sources[i].getAttributeMatrixName()), false)
          DREAM3D_REQUIRE_EQUAL(dstMatrixNames.contains(sources[i].getAttributeMatrixName()), true)

          // Check that the data stayed the same after the move

          checkAttributeMatrix(amSourcesCopy[i], dcDst->getAttributeMatrix(sources[i].getAttributeMatrixName()), false);
        }
      }
      else if(whatToMove == 1) // Move DataArray
      {
        for(int i = 0; i < sources.size(); i++)
        {
          AttributeMatrix::Pointer amSrc = dca->getDataContainer(sources[i].getDataContainerName())->getAttributeMatrix(sources[i].getAttributeMatrixName());
          AttributeMatrix::Pointer amDst = dca->getDataContainer(dapDst.getDataContainerName())->getAttributeMatrix(dapDst.getAttributeMatrixName());

          QList<QString> srcDataArrayNames = amSrc->getAttributeArrayNames();
          QList<QString> dstDataArrayNames = amDst->getAttributeArrayNames();

          // Check that it was successfully moved

          DREAM3D_REQUIRE_EQUAL(srcDataArrayNames.contains(sources[i].getDataArrayName()), false)
          DREAM3D_REQUIRE_EQUAL(dstDataArrayNames.contains(sources[i].getDataArrayName()), true)

          // Check that the data stayed the same after the move

          IDataArray::Pointer daDst = amDst->getAttributeArray(sources[i].getDataArrayName());
          IDataArray::Pointer daSrcCopy = daSourcesCopy[i];

          QString oldType = daSrcCopy->getTypeAsString();
          QString newType = daDst->getTypeAsString();
          DREAM3D_REQUIRE_EQUAL(newType, oldType)

          if(newType == "int8_t")
          {
            checkDataArray<int8_t>(daSrcCopy, daDst, false);
          }
          else if(newType == "uint8_t")
          {
            checkDataArray<uint8_t>(daSrcCopy, daDst, false);
          }
          else if(newType == "int16_t")
          {
            checkDataArray<int16_t>(daSrcCopy, daDst, false);
          }
          else if(newType == "uint16_t")
          {
            checkDataArray<uint16_t>(daSrcCopy, daDst, false);
          }
          else if(newType == "int32_t")
          {
            checkDataArray<int32_t>(daSrcCopy, daDst, false);
          }
          else if(newType == "uint32_t")
          {
            checkDataArray<uint32_t>(daSrcCopy, daDst, false);
          }
          else if(newType == "int64_t")
          {
            checkDataArray<int64_t>(daSrcCopy, daDst, false);
          }
          else if(newType == "uint64_t")
          {
            checkDataArray<uint64_t>(daSrcCopy, daDst, false);
          }
          else if(newType == "float")
          {
            checkDataArray<float>(daSrcCopy, daDst, false);
          }
          else if(newType == "double")
          {
            checkDataArray<double>(daSrcCopy, daDst, false);
          }
        }
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestMoveMultiDataTest()
  {
    // Names and constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_DataContainerDstName("DataContainerDst");
    static const QString k_AttributeMatrixSrcName("AttributeMatrixSrc");
    static const QString k_AttributeMatrixDstName("AttributeMatrixDst");
    static const QString k_AttributeMatrixBadDstName("AttributeMatrixBadDst");
    static const QString k_uint8ArrayName("uint8Array");
    static const QString k_int8ArrayName("int8Array");
    static const QString k_uint16ArrayName("uint16Array");
    static const QString k_int16ArrayName("int16Array");
    static const QString k_uint32ArrayName("uint32Array");
    static const QString k_int32ArrayName("int32Array");
    static const QString k_uint64ArrayName("uint64Array");
    static const QString k_int64ArrayName("int64Array");
    static const QString k_floatArrayName("floatDataArray");
    static const QString k_doubleArrayName("doubleDataArray");

    QVector<size_t> tupleDims = {12};
    QVector<size_t> badTupleDims = {14};
    QVector<size_t> cDims = {3};

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    DataContainer::Pointer dcDst = DataContainer::New(k_DataContainerDstName);
    dca->addDataContainer(dcDst);

    // Create AttributeMatrix

    AttributeMatrix::Pointer amSrc = AttributeMatrix::New(tupleDims, k_AttributeMatrixSrcName, AttributeMatrix::Type::Generic);
    dc->addAttributeMatrix(k_AttributeMatrixSrcName, amSrc);

    AttributeMatrix::Pointer amDst = AttributeMatrix::New(tupleDims, k_AttributeMatrixDstName, AttributeMatrix::Type::Generic);
    dc->addAttributeMatrix(k_AttributeMatrixDstName, amDst);

    AttributeMatrix::Pointer amBadDst = AttributeMatrix::New(badTupleDims, k_AttributeMatrixBadDstName, AttributeMatrix::Type::Generic);
    dc->addAttributeMatrix(k_AttributeMatrixBadDstName, amBadDst);

    // Create DataArray

    IDataArray::Pointer daSrc = createDataArray<uint8_t>(k_uint8ArrayName, tupleDims, cDims);
    amSrc->addAttributeArray(k_uint8ArrayName, daSrc);

    daSrc = createDataArray<int8_t>(k_int8ArrayName, tupleDims, cDims);
    amSrc->addAttributeArray(k_int8ArrayName, daSrc);

    daSrc = createDataArray<uint16_t>(k_uint16ArrayName, tupleDims, cDims);
    amSrc->addAttributeArray(k_uint16ArrayName, daSrc);

    daSrc = createDataArray<int16_t>(k_int16ArrayName, tupleDims, cDims);
    amSrc->addAttributeArray(k_int16ArrayName, daSrc);

    daSrc = createDataArray<uint32_t>(k_uint32ArrayName, tupleDims, cDims);
    amSrc->addAttributeArray(k_uint32ArrayName, daSrc);

    daSrc = createDataArray<int32_t>(k_int32ArrayName, tupleDims, cDims);
    amDst->addAttributeArray(k_int32ArrayName, daSrc);

    daSrc = createDataArray<uint64_t>(k_uint64ArrayName, tupleDims, cDims);
    amDst->addAttributeArray(k_uint64ArrayName, daSrc);

    daSrc = createDataArray<int64_t>(k_int64ArrayName, tupleDims, cDims);
    amDst->addAttributeArray(k_int64ArrayName, daSrc);

    daSrc = createDataArray<float>(k_floatArrayName, tupleDims, cDims);
    amDst->addAttributeArray(k_floatArrayName, daSrc);

    daSrc = createDataArray<double>(k_doubleArrayName, tupleDims, cDims);
    amDst->addAttributeArray(k_doubleArrayName, daSrc);

    // Run test cases

    // Move AttributeMatrix

    DataContainerArray::Pointer dcaTest = dca->deepCopy();

    QVector<DataArrayPath> sources = {DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, ""), DataArrayPath(k_DataContainerName, k_AttributeMatrixDstName, "")};

    DataArrayPath dst(k_DataContainerDstName);

    TestCase(dcaTest, sources, dst, 0, 0);

    // Move DataArray

    // Move arrays of different types together

    dcaTest = dca->deepCopy();

    sources = {DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, k_uint8ArrayName), DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, k_int8ArrayName),
               DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, k_uint16ArrayName), DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, k_int16ArrayName),
               DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, k_uint32ArrayName)};

    dst = DataArrayPath(k_DataContainerName, k_AttributeMatrixDstName, "");

    TestCase(dcaTest, sources, dst, 1, 0);

    sources = { DataArrayPath(k_DataContainerName, k_AttributeMatrixDstName, k_int32ArrayName), DataArrayPath(k_DataContainerName, k_AttributeMatrixDstName, k_uint64ArrayName),
           DataArrayPath(k_DataContainerName, k_AttributeMatrixDstName, k_int64ArrayName), DataArrayPath(k_DataContainerName, k_AttributeMatrixDstName, k_floatArrayName),
           DataArrayPath(k_DataContainerName, k_AttributeMatrixDstName, k_doubleArrayName) };

    dst = DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, "");

    TestCase(dcaTest, sources, dst, 1, 0);

    // Yield error -11019 when source and destination AttributeMatrices number of tuples don't match

    dcaTest = dca->deepCopy();

    dst = DataArrayPath(k_DataContainerName, k_AttributeMatrixBadDstName, "");

    TestCase(dcaTest, sources, dst, 1, -11019);

    // Move arrays to different DataContainer

    dcaTest = dca->deepCopy();

    dcaTest->getDataContainer(k_DataContainerDstName)->addAttributeMatrix(k_AttributeMatrixDstName, dcaTest->getDataContainer(k_DataContainerName)->removeAttributeMatrix(k_AttributeMatrixDstName));

    sources = { DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, k_uint8ArrayName), DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, k_int8ArrayName),
           DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, k_uint16ArrayName), DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, k_int16ArrayName),
           DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, k_uint32ArrayName) };

    dst = DataArrayPath(k_DataContainerDstName, k_AttributeMatrixDstName, "");

    TestCase(dcaTest, sources, dst, 1, 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#### MoveMultiDataTest Starting ####" << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestMoveMultiDataTest())
  }

private:
  QString m_FilterName = QString("MoveMultiData");
};
