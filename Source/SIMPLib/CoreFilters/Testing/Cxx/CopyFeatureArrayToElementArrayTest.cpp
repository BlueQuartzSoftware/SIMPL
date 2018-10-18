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

class CopyFeatureArrayToElementArrayTest
{

public:
  CopyFeatureArrayToElementArrayTest() = default;
  ~CopyFeatureArrayToElementArrayTest() = default;
  CopyFeatureArrayToElementArrayTest(const CopyFeatureArrayToElementArrayTest&) = delete;            // Copy Constructor
  CopyFeatureArrayToElementArrayTest(CopyFeatureArrayToElementArrayTest&&) = delete;                 // Move Constructor
  CopyFeatureArrayToElementArrayTest& operator=(const CopyFeatureArrayToElementArrayTest&) = delete; // Copy Assignment
  CopyFeatureArrayToElementArrayTest& operator=(CopyFeatureArrayToElementArrayTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the CopyFeatureArrayToElementArrayTest Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The CopyFeatureArrayToElementArrayTest Requires the use of the " << m_FilterName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> std::shared_ptr<DataArray<T>> createDataArray(const QString& daName, const std::vector<std::vector<T>>& data, const QVector<size_t>& tupleDims, const QVector<size_t>& cDims)
  {
    typename DataArray<T>::Pointer da = DataArray<T>::CreateArray(tupleDims, cDims, daName);
    for(int i = 0; i < da->getNumberOfTuples(); i++)
    {
      da->setTuple(i, data[i]);
    }
    return da;
  }

  template <typename T> void TestCopyFeatureArrayToElementArrayForType()
  {
    // Names and Constants

    static const QString k_DataContainerName("DataContainer");

    static const QString k_FeatureDataMatrixName("FeatureDataMatrix");
    static const QString k_FeatureIDMatrixName("FeatureIDMatrix");

    static const QString k_FeatureDataArrayName("FeatureData");
    static const QString k_FeatureIDArrayName("FeatureIDArray");
    static const QString k_ElementArrayName("ElementArray");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrices

    AttributeMatrix::Pointer dataAM = AttributeMatrix::New(m_Dims10, k_FeatureDataMatrixName, AttributeMatrix::Type::VertexFeature);
    dc->addAttributeMatrix(k_FeatureDataMatrixName, dataAM);

    AttributeMatrix::Pointer idsAM = AttributeMatrix::New(m_Dims10, k_FeatureIDMatrixName, AttributeMatrix::Type::Vertex);
    dc->addAttributeMatrix(k_FeatureIDMatrixName, idsAM);

    // Create feature data and feature IDs

    typename DataArray<T>::Pointer daFeatures = DataArray<T>::CreateArray(m_Dims10, m_Dims3, k_FeatureDataArrayName);
    for(int i = 0; i < daFeatures->getSize(); i++)
    {
      daFeatures->setValue(i, static_cast<T>(i + 2.13));
    }
    dataAM->addAttributeArray(k_FeatureDataArrayName, daFeatures);

    // Use reverse ordering on the ID list to test that the filter moves data to the appropriate location

    DataArray<int32_t>::Pointer daIDList = DataArray<int32_t>::CreateArray(m_Dims10, m_Dims1, k_FeatureIDArrayName);
    for(int i = 0; i < daIDList->getNumberOfTuples(); i++)
    {
      daIDList->setValue(daIDList->getNumberOfTuples() - i - 1, i);
    }
    idsAM->addAttributeArray(k_FeatureIDArrayName, daIDList);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer copyFeatureArrayToElementArray = filterFactory->create();

    copyFeatureArrayToElementArray->setDataContainerArray(dca);

    // Set up filter

    QVariant var;

    DataArrayPath dap(k_DataContainerName, k_FeatureDataMatrixName, k_FeatureDataArrayName);
    var.setValue(dap);
    bool propWasSet = copyFeatureArrayToElementArray->setProperty("SelectedFeatureArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_FeatureIDMatrixName, k_FeatureIDArrayName);
    var.setValue(dap);
    propWasSet = copyFeatureArrayToElementArray->setProperty("FeatureIdsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(k_ElementArrayName);
    propWasSet = copyFeatureArrayToElementArray->setProperty("CreatedArrayName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run filter

    copyFeatureArrayToElementArray->execute();
    DREAM3D_REQUIRED(copyFeatureArrayToElementArray->getErrorCondition(), >=, 0);

    // Check filter results

    typename DataArray<T>::Pointer createdElementArray = idsAM->getAttributeArrayAs<DataArray<T>>(k_ElementArrayName);

    for(size_t i = 0; i < daFeatures->getNumberOfTuples(); i++)
    {
      for(size_t j = 0; j < daFeatures->getNumberOfComponents(); j++)
      {
        T oldValue = daFeatures->getComponent(i, j);
        T newValue = createdElementArray->getComponent(daIDList->getValue(i), j);
        DREAM3D_REQUIRE_EQUAL(newValue, oldValue)
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCopyFeatureArrayToElementArrayTest()
  {
    TestCopyFeatureArrayToElementArrayForType<uint8_t>();
    TestCopyFeatureArrayToElementArrayForType<int8_t>();
    TestCopyFeatureArrayToElementArrayForType<uint16_t>();
    TestCopyFeatureArrayToElementArrayForType<int16_t>();
    TestCopyFeatureArrayToElementArrayForType<uint32_t>();
    TestCopyFeatureArrayToElementArrayForType<int32_t>();
    TestCopyFeatureArrayToElementArrayForType<uint64_t>();
    TestCopyFeatureArrayToElementArrayForType<int64_t>();
    TestCopyFeatureArrayToElementArrayForType<float>();
    TestCopyFeatureArrayToElementArrayForType<double>();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#### CopyFeatureArrayToElementArrayTest Starting ####" << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestCopyFeatureArrayToElementArrayTest())
  }

private:
  QString m_FilterName = QString("CopyFeatureArrayToElementArray");
  QVector<size_t> m_Dims1 = {1};
  QVector<size_t> m_Dims3 = {3};
  QVector<size_t> m_Dims10 = {10};
};
