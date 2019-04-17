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

class MoveDataTest
{

public:
  MoveDataTest() = default;
  ~MoveDataTest() = default;
  MoveDataTest(const MoveDataTest&) = delete;            // Copy Constructor
  MoveDataTest(MoveDataTest&&) = delete;                 // Move Constructor
  MoveDataTest& operator=(const MoveDataTest&) = delete; // Copy Assignment
  MoveDataTest& operator=(MoveDataTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the MoveDataTest Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The MoveDataTest Requires the use of the " << m_FilterName.toStdString() << " filter which is found in Core Filters";
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
  void TestCase(DataContainerArray::Pointer dca, DataArrayPath dapSrc, DataArrayPath dapDst, int whatToMove, int err)
  {
    AttributeMatrix::Pointer amSrcCopy = AttributeMatrix::NullPointer();
    IDataArray::Pointer daSrcCopy = IDataArray::NullPointer();

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer moveData = filterFactory->create();

    moveData->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    var.setValue(whatToMove);
    bool propWasSet = moveData->setProperty("WhatToMove", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    if(whatToMove == 0) // Move AttributeMatrix
    {
      var.setValue(dapDst.getDataContainerName());
      propWasSet = moveData->setProperty("DataContainerDestination", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(dapSrc);
      propWasSet = moveData->setProperty("AttributeMatrixSource", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      amSrcCopy = dca->getDataContainer(dapSrc.getDataContainerName())->getAttributeMatrix(dapSrc.getAttributeMatrixName())->deepCopy();
    }
    else if(whatToMove == 1) // Move DataArray
    {
      var.setValue(dapDst);
      propWasSet = moveData->setProperty("AttributeMatrixDestination", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(dapSrc);
      propWasSet = moveData->setProperty("DataArraySource", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      daSrcCopy = dca->getDataContainer(dapSrc.getDataContainerName())->getAttributeMatrix(dapSrc.getAttributeMatrixName())->getAttributeArray(dapSrc.getDataArrayName())->deepCopy();
    }

    // Run Filter

    moveData->execute();

    if(err >= 0)
    {
      DREAM3D_REQUIRED(moveData->getErrorCondition(), >=, err)
    }
    else
    {
      DREAM3D_REQUIRED(moveData->getErrorCondition(), ==, err)
    }

    // Check filter results if filter successfully executed

    if(err >= 0)
    {
      if(whatToMove == 0) // Move AttributeMatrix
      {
        DataContainer::Pointer dc = dca->getDataContainer(dapSrc.getDataContainerName());
        DataContainer::Pointer dcDst = dca->getDataContainer(dapDst);

        QList<QString> srcMatrixNames = dc->getAttributeMatrixNames();
        QList<QString> dstMatrixNames = dcDst->getAttributeMatrixNames();

        // Check that it was successfully moved

        DREAM3D_REQUIRE_EQUAL(srcMatrixNames.contains(dapSrc.getAttributeMatrixName()), false)
        DREAM3D_REQUIRE_EQUAL(dstMatrixNames.contains(dapSrc.getAttributeMatrixName()), true)

        // Check that the data stayed the same after the move

        checkAttributeMatrix(amSrcCopy, dcDst->getAttributeMatrix(dapSrc.getAttributeMatrixName()), false);
      }
      else if(whatToMove == 1) // Move DataArray
      {
        AttributeMatrix::Pointer amSrc = dca->getDataContainer(dapSrc.getDataContainerName())->getAttributeMatrix(dapSrc.getAttributeMatrixName());
        AttributeMatrix::Pointer amDst = dca->getDataContainer(dapDst.getDataContainerName())->getAttributeMatrix(dapDst.getAttributeMatrixName());

        QList<QString> srcDataArrayNames = amSrc->getAttributeArrayNames();
        QList<QString> dstDataArrayNames = amDst->getAttributeArrayNames();

        // Check that it was successfully moved

        DREAM3D_REQUIRE_EQUAL(srcDataArrayNames.contains(dapSrc.getDataArrayName()), false)
        DREAM3D_REQUIRE_EQUAL(dstDataArrayNames.contains(dapSrc.getDataArrayName()), true)

        // Check that the data stayed the same after the move

        IDataArray::Pointer daDst = amDst->getAttributeArray(dapSrc.getDataArrayName());

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

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestMoveDataTest()
  {
    // Names and constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_DataContainerDstName("DataContainerDst");
    static const QString k_AttributeMatrixSrcName("AttributeMatrixSrc");
    static const QString k_AttributeMatrixDstName("AttributeMatrixDst");
    static const QString k_AttributeMatrixBadDstName("AttributeMatrixBadDst");
    static const QString k_DataArraySrcName("DataArraySrc");

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

    IDataArray::Pointer daSrc = createDataArray<int8_t>(k_DataArraySrcName, tupleDims, cDims);
    amSrc->addAttributeArray(k_DataArraySrcName, daSrc);

    // Run test cases

    // Move AttributeMatrix

    DataContainerArray::Pointer dcaTest = dca->deepCopy();

    DataArrayPath src(k_DataContainerName, k_AttributeMatrixSrcName, "");
    DataArrayPath dst(k_DataContainerDstName);

    TestCase(dcaTest, src, dst, 0, 0);

    // Move DataArray

    // Move array to different DataContainer

    dcaTest = dca->deepCopy();

    dcaTest->getDataContainer(k_DataContainerDstName)->addAttributeMatrix(k_AttributeMatrixDstName, dcaTest->getDataContainer(k_DataContainerName)->removeAttributeMatrix(k_AttributeMatrixDstName));

    src = DataArrayPath(k_DataContainerName, k_AttributeMatrixSrcName, k_DataArraySrcName);
    dst = DataArrayPath(k_DataContainerDstName, k_AttributeMatrixDstName, "");

    TestCase(dcaTest, src, dst, 1, 0);

    // Yield error -11019 when source and destination AttributeMatrices number of tuples don't match

    dcaTest = dca->deepCopy();

    dst = DataArrayPath(k_DataContainerName, k_AttributeMatrixBadDstName, "");

    TestCase(dcaTest, src, dst, 1, -11019);

    // Move int8_t

    dcaTest = dca->deepCopy();

    dst = DataArrayPath(k_DataContainerName, k_AttributeMatrixDstName, "");

    TestCase(dcaTest, src, dst, 1, 0);

    // Move uint8_t

    dcaTest = dca->deepCopy();

    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->removeAttributeArray(k_DataArraySrcName);
    daSrc = createDataArray<uint8_t>(k_DataArraySrcName, tupleDims, cDims);
    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->addAttributeArray(k_DataArraySrcName, daSrc);

    TestCase(dcaTest, src, dst, 1, 0);

    // Move int16_t

    dcaTest = dca->deepCopy();

    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->removeAttributeArray(k_DataArraySrcName);
    daSrc = createDataArray<int16_t>(k_DataArraySrcName, tupleDims, cDims);
    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->addAttributeArray(k_DataArraySrcName, daSrc);

    TestCase(dcaTest, src, dst, 1, 0);

    // Move uint16_t

    dcaTest = dca->deepCopy();

    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->removeAttributeArray(k_DataArraySrcName);
    daSrc = createDataArray<uint16_t>(k_DataArraySrcName, tupleDims, cDims);
    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->addAttributeArray(k_DataArraySrcName, daSrc);

    TestCase(dcaTest, src, dst, 1, 0);

    // Move int32_t

    dcaTest = dca->deepCopy();

    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->removeAttributeArray(k_DataArraySrcName);
    daSrc = createDataArray<int32_t>(k_DataArraySrcName, tupleDims, cDims);
    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->addAttributeArray(k_DataArraySrcName, daSrc);

    TestCase(dcaTest, src, dst, 1, 0);

    // Move uint32_t

    dcaTest = dca->deepCopy();

    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->removeAttributeArray(k_DataArraySrcName);
    daSrc = createDataArray<uint32_t>(k_DataArraySrcName, tupleDims, cDims);
    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->addAttributeArray(k_DataArraySrcName, daSrc);

    TestCase(dcaTest, src, dst, 1, 0);

    // Move int64_t

    dcaTest = dca->deepCopy();

    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->removeAttributeArray(k_DataArraySrcName);
    daSrc = createDataArray<uint32_t>(k_DataArraySrcName, tupleDims, cDims);
    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->addAttributeArray(k_DataArraySrcName, daSrc);

    TestCase(dcaTest, src, dst, 1, 0);

    // Move uint64_t

    dcaTest = dca->deepCopy();

    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->removeAttributeArray(k_DataArraySrcName);
    daSrc = createDataArray<uint64_t>(k_DataArraySrcName, tupleDims, cDims);
    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->addAttributeArray(k_DataArraySrcName, daSrc);

    TestCase(dcaTest, src, dst, 1, 0);

    // Move float

    dcaTest = dca->deepCopy();

    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->removeAttributeArray(k_DataArraySrcName);
    daSrc = createDataArray<float>(k_DataArraySrcName, tupleDims, cDims);
    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->addAttributeArray(k_DataArraySrcName, daSrc);

    TestCase(dcaTest, src, dst, 1, 0);

    // Move double

    dcaTest = dca->deepCopy();

    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->removeAttributeArray(k_DataArraySrcName);
    daSrc = createDataArray<double>(k_DataArraySrcName, tupleDims, cDims);
    dcaTest->getDataContainer(k_DataContainerName)->getAttributeMatrix(k_AttributeMatrixSrcName)->addAttributeArray(k_DataArraySrcName, daSrc);

    TestCase(dcaTest, src, dst, 1, 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#### MoveDataTest Starting ####" << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestMoveDataTest())
  }

private:
  QString m_FilterName = QString("MoveData");
};
