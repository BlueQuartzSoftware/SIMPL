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

#include "SIMPLib/CoreFilters/RenameDataContainer.h"
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

class RenameDataContainerTest
{
public:
  RenameDataContainerTest() = default;
  virtual ~RenameDataContainerTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  AbstractFilter::Pointer createFilter()
  {
    // Now instantiate the RenameAttributeArray Filter from the FilterManager
    QString filtName = "RenameDataContainer";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The RenameDataContainerTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    AbstractFilter::Pointer filter = filterFactory->create();
    DREAM3D_REQUIRE(nullptr != filter.get());

    return filter;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestEmptyDataContainerArray()
  {
    AbstractFilter::Pointer filter = createFilter();
    DREAM3D_ASSERT(nullptr != filter.get());

    DataContainerArray::Pointer dca = DataContainerArray::New();

    filter->setDataContainerArray(dca);

    QVariant value;

    DataArrayPath dataContainerName("DataContainer");
    value.setValue(dataContainerName);
    filter->setProperty("SelectedDataContainerName", value);

    DataArrayPath newDataContainerName("DataContainer_");
    value.setValue(dataContainerName);
    filter->setProperty("NewDataContainerName", value);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -999);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestNullDataContainer()
  {
    AbstractFilter::Pointer filter = createFilter();
    DREAM3D_ASSERT(nullptr != filter.get());

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New("DataContainer");

    dca->addOrReplaceDataContainer(dc);

    filter->setDataContainerArray(dca);

    QVariant value;

    DataArrayPath dataContainerName;
    value.setValue(dataContainerName);
    filter->setProperty("SelectedDataContainerName", value);

    DataArrayPath newDataContainerName("DataContainer_");
    value.setValue(dataContainerName);
    filter->setProperty("NewDataContainerName", value);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -11001);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestValidDataContainer()
  {
    AbstractFilter::Pointer filter = createFilter();
    DREAM3D_ASSERT(nullptr != filter.get());

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New("DataContainer");

    dca->addOrReplaceDataContainer(dc);

    filter->setDataContainerArray(dca);

    QVariant value;

    DataArrayPath dataContainerName("DataContainer");
    value.setValue(dataContainerName);
    filter->setProperty("SelectedDataContainerName", value);

    DataArrayPath newDataContainerName("Container_2");
    value.setValue(newDataContainerName);
    filter->setProperty("NewDataContainerName", value);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), 0);

    DataArrayPath dap(dc->getName());

    DREAM3D_REQUIRE_EQUAL(dap, newDataContainerName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestSetEmptyName()
  {
    AbstractFilter::Pointer filter = createFilter();
    DREAM3D_ASSERT(nullptr != filter.get());

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New("DataContainer");

    dca->addOrReplaceDataContainer(dc);

    filter->setDataContainerArray(dca);

    QVariant value;

    DataArrayPath dataContainerName("DataContainer");
    value.setValue(dataContainerName);
    filter->setProperty("SelectedDataContainerName", value);

    DataArrayPath newDataContainerName;
    value.setValue(newDataContainerName);
    filter->setProperty("NewDataContainerName", value);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -11001);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestSetNullName()
  {
    AbstractFilter::Pointer filter = createFilter();
    DREAM3D_ASSERT(nullptr != filter.get());

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New("DataContainer");

    dca->addOrReplaceDataContainer(dc);

    filter->setDataContainerArray(dca);

    QVariant value;

    DataArrayPath dataContainerName("DataContainer");
    value.setValue(dataContainerName);
    filter->setProperty("SelectedDataContainerName", value);

    DataArrayPath newDataContainerName;
    value.setValue(newDataContainerName);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -11001);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### RenameDataContainerTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestEmptyDataContainerArray());
    DREAM3D_REGISTER_TEST(TestNullDataContainer());
    DREAM3D_REGISTER_TEST(TestValidDataContainer());
    DREAM3D_REGISTER_TEST(TestSetEmptyName());
    DREAM3D_REGISTER_TEST(TestSetNullName());
  }

private:
  RenameDataContainerTest(const RenameDataContainerTest&); // Copy Constructor Not Implemented
  void operator=(const RenameDataContainerTest&);          // Move assignment Not Implemented
};
