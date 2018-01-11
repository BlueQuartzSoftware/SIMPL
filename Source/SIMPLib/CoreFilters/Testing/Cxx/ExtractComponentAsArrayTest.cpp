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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

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

#include "SIMPLib/CoreFilters/ExtractComponentAsArray.h"

class ExtractComponentAsArrayTest
{
public:
  ExtractComponentAsArrayTest()
  {
  }
  virtual ~ExtractComponentAsArrayTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New();
    dc->setName("DataContainer");
    dca->addDataContainer(dc);

    QVector<size_t> dims;
    dims.push_back(2);
    AttributeMatrix::Pointer am = AttributeMatrix::New(dims, "AttributeMatrix", AttributeMatrix::Type::Any);
    dc->addAttributeMatrix("AttributeMatrix", am);

    QVector<size_t> cdims;
    cdims.push_back(2);
    DataArray<int>::Pointer da = DataArray<int>::CreateArray(2, cdims, "DataArray");
    da->setComponent(0, 0, 1);
    da->setComponent(0, 1, 2);
    da->setComponent(1, 0, 3);
    da->setComponent(1, 1, 4);
    am->addAttributeArray("DataArray", da);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  ExtractComponentAsArray::Pointer createFilter()
  {
    // Now instantiate the ExtractComponentAsArray Filter from the FilterManager
    QString filtName = "ExtractComponentAsArray";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The ExtractComponentAsArrayTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    ExtractComponentAsArray::Pointer filter = std::dynamic_pointer_cast<ExtractComponentAsArray>(filterFactory->create());
    DREAM3D_REQUIRE(nullptr != filter.get());

    return filter;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setValues(ExtractComponentAsArray::Pointer filter, QString dataArray, int component, QString newArrayName)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(DataArrayPath("DataContainer", "AttributeMatrix", dataArray));
    filter->setProperty("SelectedArrayPath", value);

    value.setValue(component);
    filter->setProperty("CompNumber", value);

    value.setValue(newArrayName);
    filter->setProperty("NewArrayArrayName", value);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void checkCopiedComponent(AttributeMatrix::Pointer am, int component, QString newArrayName)
  {
    DataArray<int>::Pointer da = std::dynamic_pointer_cast<DataArray<int>>(am->getAttributeArray("DataArray"));
    DREAM3D_REQUIRE(nullptr != da.get());

    DataArray<int>::Pointer copiedArray = std::dynamic_pointer_cast<DataArray<int>>(am->getAttributeArray(newArrayName));
    DREAM3D_REQUIRE(nullptr != copiedArray.get());

    for(size_t i = 0; i < da->getNumberOfTuples(); i++)
    {
      DREAM3D_REQUIRE_EQUAL(da->getComponent(i, component), copiedArray->getComponent(i, 0));
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestValidInput()
  {
    ExtractComponentAsArray::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    QString newArrayName = "NewArray";

    setValues(filter, "DataArray", 1, newArrayName);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);

    DataContainerArray::Pointer dca = filter->getDataContainerArray();
    DataContainer::Pointer dc = dca->getDataContainer("DataContainer");
    AttributeMatrix::Pointer am = dc->getAttributeMatrix("AttributeMatrix");

    checkCopiedComponent(am, 1, newArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInvalidComponent()
  {
    ExtractComponentAsArray::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    QString newArrayName = "NewArray";

    setValues(filter, "DataArray", 2, newArrayName);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -11004);

    setValues(filter, "DataArray", -1, newArrayName);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -11005);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInvalidDataArray()
  {
    ExtractComponentAsArray::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    QString newArrayName = "NewArray";

    setValues(filter, "DataArray1", 1, newArrayName);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -90002);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInvalidCreatedArrayName()
  {
    ExtractComponentAsArray::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    QString newArrayName = "DataArray";

    setValues(filter, "DataArray", 1, newArrayName);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -10002);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ExtractComponentAsArrayTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestValidInput());

    DREAM3D_REGISTER_TEST(TestInvalidComponent());
    DREAM3D_REGISTER_TEST(TestInvalidDataArray());
    DREAM3D_REGISTER_TEST(TestInvalidCreatedArrayName());
  }

private:
  ExtractComponentAsArrayTest(const ExtractComponentAsArrayTest&); // Copy Constructor Not Implemented
  void operator=(const ExtractComponentAsArrayTest&);              // Operator '=' Not Implemented
};
