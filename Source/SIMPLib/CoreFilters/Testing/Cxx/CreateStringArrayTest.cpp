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

class CreateStringArrayTest
{

public:
  CreateStringArrayTest() = default;
  ~CreateStringArrayTest() = default;
  CreateStringArrayTest(const CreateStringArrayTest&) = delete;            // Copy Constructor
  CreateStringArrayTest(CreateStringArrayTest&&) = delete;                 // Move Constructor
  CreateStringArrayTest& operator=(const CreateStringArrayTest&) = delete; // Copy Assignment
  CreateStringArrayTest& operator=(CreateStringArrayTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the CreateStringArrayTest Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The CreateStringArrayTest Requires the use of the " << m_FilterName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCreateStringArrayTest()
  {
    // Names and constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_AttributeMatrixName("AttributeMatrix");
    static const QString k_StringArrayName("StringArray");
    static const QString k_StringInitialValue("Test String");
    static const QString k_EmptyQString("");

    QVector<size_t> tupleDims(1, 5);

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrix

    AttributeMatrix::Pointer am = AttributeMatrix::New(tupleDims, k_AttributeMatrixName, AttributeMatrix::Type::Generic);
    dc->addAttributeMatrix(k_AttributeMatrixName, am);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer createStringArray = filterFactory->create();

    createStringArray->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    DataArrayPath dap(k_DataContainerName, k_AttributeMatrixName, k_StringArrayName);
    var.setValue(dap);
    bool propWasSet = createStringArray->setProperty("NewArray", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(k_StringInitialValue);
    propWasSet = createStringArray->setProperty("InitializationValue", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run Filter

    createStringArray->execute();
    DREAM3D_REQUIRED(createStringArray->getErrorCondition(), >=, 0);

    // Check Filter Results

    StringDataArray::Pointer stringArray = am->getAttributeArrayAs<StringDataArray>(k_StringArrayName);

    DREAM3D_REQUIRE_EQUAL(stringArray->getNumberOfTuples(), tupleDims[0])
    DREAM3D_REQUIRE_EQUAL(stringArray->getNumberOfComponents(), 1)

    for(size_t i = 0; i < stringArray->getNumberOfTuples(); i++)
    {
      DREAM3D_REQUIRE_EQUAL(stringArray->getValue(i), k_StringInitialValue)
    }

    // Test empty initialization value; should yield error -5759

    var.setValue(k_EmptyQString);
    propWasSet = createStringArray->setProperty("InitializationValue", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createStringArray->execute();
    DREAM3D_REQUIRED(createStringArray->getErrorCondition(), ==, -5759);

    // Test invalid array path; should yield error -8152

    dap = DataArrayPath(k_DataContainerName, k_EmptyQString, k_StringArrayName);
    var.setValue(dap);
    propWasSet = createStringArray->setProperty("NewArray", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(k_StringInitialValue);
    propWasSet = createStringArray->setProperty("InitializationValue", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createStringArray->execute();
    DREAM3D_REQUIRED(createStringArray->getErrorCondition(), ==, -8152);

    // Test negative number of components; should yield error -8150

    dap = DataArrayPath(k_DataContainerName, k_AttributeMatrixName, k_StringArrayName);
    var.setValue(dap);
    propWasSet = createStringArray->setProperty("NewArray", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(-1);
    propWasSet = createStringArray->setProperty("NumberOfComponents", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createStringArray->execute();
    DREAM3D_REQUIRED(createStringArray->getErrorCondition(), ==, -8150);

    // Test zero components; should yield error -8151

    var.setValue(0);
    propWasSet = createStringArray->setProperty("NumberOfComponents", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createStringArray->execute();
    DREAM3D_REQUIRED(createStringArray->getErrorCondition(), ==, -8151);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#### CreateStringArrayTest Starting ####" << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestCreateStringArrayTest())
  }

private:
  QString m_FilterName = QString("CreateStringArray");

};
