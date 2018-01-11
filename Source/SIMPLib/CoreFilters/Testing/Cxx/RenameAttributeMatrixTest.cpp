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

#include "SIMPLib/CoreFilters/RenameAttributeMatrix.h"

class RenameAttributeMatrixTest
{
public:
  RenameAttributeMatrixTest()
  {
  }
  virtual ~RenameAttributeMatrixTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  AbstractFilter::Pointer createFilter()
  {
    // Now instantiate the RenameAttributeArray Filter from the FilterManager
    QString filtName = "RenameAttributeMatrix";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The RenameAttributeMatrixTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    AbstractFilter::Pointer filter = filterFactory->create();
    DREAM3D_REQUIRE(nullptr != filter.get());

    return filter;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestEmptyDataContainer()
  {
    AbstractFilter::Pointer filter = createFilter();
    DREAM3D_ASSERT(nullptr != filter);

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New();
    dc->setName("DataContainer");
    dca->addDataContainer(dc);

    filter->setDataContainerArray(dca);

    QVariant value;

    DataArrayPath dataArrayPath = DataArrayPath("DataContainer", "AttributeMatrix", "");
    value.setValue(dataArrayPath);
    filter->setProperty("SelectedAttributeMatrixPath", value);

    QString newAttributeMatrixName = "DataContainer_";
    value.setValue(newAttributeMatrixName);
    filter->setProperty("NewAttributeMatrix", value);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -307020);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestNullAttributeMatrix()
  {
    AbstractFilter::Pointer filter = createFilter();
    DREAM3D_ASSERT(nullptr != filter.get());

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New();
    dc->setName("DataContainer");
    dca->addDataContainer(dc);

    QVector<size_t> dims = QVector<size_t>();
    dims.push_back(1);
    AttributeMatrix::Pointer am = AttributeMatrix::New(dims, "AttributeMatrix", AttributeMatrix::Type::Any);
    dc->addAttributeMatrix("AttributeMatrix", am);

    filter->setDataContainerArray(dca);

    QVariant value;

    DataArrayPath dataArrayPath = DataArrayPath("DataContainer", "", "");
    value.setValue(dataArrayPath);
    filter->setProperty("SelectedAttributeMatrixPath", value);

    QString newAttributeMatrixName = "Matrix_2";
    value.setValue(newAttributeMatrixName);
    filter->setProperty("NewAttributeMatrix", value);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -301000);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestValidAttributeMatrix()
  {
    AbstractFilter::Pointer filter = createFilter();
    DREAM3D_ASSERT(nullptr != filter.get());

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New();
    dc->setName("DataContainer");
    dca->addDataContainer(dc);

    QVector<size_t> dims = QVector<size_t>();
    dims.push_back(1);
    AttributeMatrix::Pointer am = AttributeMatrix::New(dims, "AttributeMatrix", AttributeMatrix::Type::Any);
    dc->addAttributeMatrix("AttributeMatrix", am);

    filter->setDataContainerArray(dca);

    QVariant value;

    DataArrayPath dataArrayPath = DataArrayPath("DataContainer", "AttributeMatrix", "");
    value.setValue(dataArrayPath);
    filter->setProperty("SelectedAttributeMatrixPath", value);

    QString newAttributeMatrixName = "Matrix_2";
    value.setValue(newAttributeMatrixName);
    filter->setProperty("NewAttributeMatrix", value);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);

    DREAM3D_REQUIRE_EQUAL(am->getName(), newAttributeMatrixName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestSetEmptyName()
  {
    AbstractFilter::Pointer filter = createFilter();
    DREAM3D_ASSERT(nullptr != filter.get());

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New();
    dc->setName("DataContainer");
    dca->addDataContainer(dc);

    QVector<size_t> dims = QVector<size_t>();
    dims.push_back(1);
    AttributeMatrix::Pointer am = AttributeMatrix::New(dims, "AttributeMatrix", AttributeMatrix::Type::Any);
    dc->addAttributeMatrix("AttributeMatrix", am);

    filter->setDataContainerArray(dca);

    QVariant value;

    DataArrayPath dataArrayPath = DataArrayPath("DataContainer", "AttributeMatrix", "");
    value.setValue(dataArrayPath);
    filter->setProperty("SelectedAttributeMatrixPath", value);

    QString newAttributeMatrixName = "";
    value.setValue(newAttributeMatrixName);
    filter->setProperty("NewAttributeMatrix", value);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -11004);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestSetNullName()
  {
    AbstractFilter::Pointer filter = createFilter();
    DREAM3D_ASSERT(nullptr != filter.get());

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New();
    dc->setName("DataContainer");
    dca->addDataContainer(dc);

    QVector<size_t> dims = QVector<size_t>();
    dims.push_back(1);
    AttributeMatrix::Pointer am = AttributeMatrix::New(dims, "AttributeMatrix", AttributeMatrix::Type::Any);
    dc->addAttributeMatrix("AttributeMatrix", am);

    filter->setDataContainerArray(dca);

    QVariant value;

    DataArrayPath dataArrayPath = DataArrayPath("DataContainer", "AttributeMatrix", "");
    value.setValue(dataArrayPath);
    filter->setProperty("SelectedAttributeMatrixPath", value);

    QString newAttributeMatrixName = "";
    value.setValue(newAttributeMatrixName);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -11004);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### RenameAttributeMatrixTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestEmptyDataContainer());
    DREAM3D_REGISTER_TEST(TestNullAttributeMatrix());
    DREAM3D_REGISTER_TEST(TestValidAttributeMatrix());
    DREAM3D_REGISTER_TEST(TestSetEmptyName());
    DREAM3D_REGISTER_TEST(TestSetNullName());
  }

private:
  RenameAttributeMatrixTest(const RenameAttributeMatrixTest&); // Copy Constructor Not Implemented
  void operator=(const RenameAttributeMatrixTest&);            // Operator '=' Not Implemented
};