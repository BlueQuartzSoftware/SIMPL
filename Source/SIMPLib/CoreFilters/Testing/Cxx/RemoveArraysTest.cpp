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

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/CoreFilters/RemoveArrays.h"
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

class RemoveArraysTest
{

public:
  RemoveArraysTest() = default;
  ~RemoveArraysTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  AbstractFilter::Pointer createFilter()
  {
    std::vector<size_t> vec;
    vec.push_back(1);

    const QString arrayName = "DataArray";
    DataArray<uint>::Pointer da = DataArray<uint>::CreateArray(1, vec, arrayName, true);

    std::vector<size_t> sizeList;
    sizeList.push_back(1);
    sizeList.push_back(1);
    sizeList.push_back(1);

    AttributeMatrix::Pointer am = AttributeMatrix::New(sizeList, "AttributeMatrix", AttributeMatrix::Type::Any);
    am->insertOrAssign(da);

    DataContainer::Pointer dc = DataContainer::New("DataContainer");

    dc->addOrReplaceAttributeMatrix(am);

    DataContainerArray::Pointer dca = DataContainerArray::New();
    dca->addOrReplaceDataContainer(dc);

    FilterManager* fm = FilterManager::Instance();

    IFilterFactory::Pointer ff = fm->getFactoryFromClassName(QString("RemoveArrays"));

    AbstractFilter::Pointer filter = ff->create();

    filter->setDataContainerArray(dca);

    return filter;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setFlags(DataContainerArrayProxy& proxy, const QString& dataContainerKey, const QString& attributeMatrixKey, const QString& dataArrayKey)
  {
    QMap<QString, DataContainerProxy>::iterator dcIterator = proxy.getDataContainers().begin();
    while(dcIterator != proxy.getDataContainers().end())
    {
      if(dcIterator.key() == dataContainerKey)
      {
        dcIterator.value().setFlag(Qt::Checked);
      }
      else
      {
        dcIterator.value().setFlag(Qt::Unchecked);

        QMap<QString, AttributeMatrixProxy>::iterator amIterator = dcIterator.value().getAttributeMatricies().begin();
        while(amIterator != dcIterator.value().getAttributeMatricies().end())
        {
          if(amIterator.key() == attributeMatrixKey)
          {
            amIterator.value().setFlag(Qt::Checked);
          }
          else
          {
            amIterator.value().setFlag(Qt::Unchecked);

            QMap<QString, DataArrayProxy>::iterator daIterator = amIterator.value().getDataArrays().begin();
            while(daIterator != amIterator.value().getDataArrays().end())
            {
              if(daIterator.key() == dataArrayKey)
              {
                daIterator.value().setFlag(Qt::Checked);
              }
              else
              {
                daIterator.value().setFlag(Qt::Unchecked);
              }

              daIterator++;
            } // end daIterator
          }

          amIterator++;
        } // end amIterator
      }

      dcIterator++;
    } // end dcIterator
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestDeleteDataArray()
  {
    RemoveArrays::Pointer filter = std::dynamic_pointer_cast<RemoveArrays>(createFilter());
    DREAM3D_ASSERT(nullptr != filter);

    QVariant value;

    DataContainerArrayProxy dataContainerArrayProxy = DataContainerArrayProxy(filter->getDataContainerArray().get());
    setFlags(dataContainerArrayProxy, "", "", "DataArray");

    value.setValue(dataContainerArrayProxy);
    bool propSet = filter->setProperty("DataArraysToRemove", value);
    DREAM3D_REQUIRE_EQUAL(propSet, true);

    filter->execute();

    DataContainerArray::Pointer dca = filter->getDataContainerArray();
    DREAM3D_ASSERT(nullptr != dca);

    DataContainer::Pointer dc = dca->getDataContainer("DataContainer");
    DREAM3D_ASSERT(nullptr != dc);

    AttributeMatrix::Pointer am = dca->getAttributeMatrix(DataArrayPath("DataContainer", "AttributeMatrix", ""));
    DREAM3D_ASSERT(nullptr != am);

    IDataArray::Pointer da = am->getAttributeArray("DataArray");
    DREAM3D_ASSERT(nullptr == da);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestDeleteAttributeMatrix()
  {
    RemoveArrays::Pointer filter = std::dynamic_pointer_cast<RemoveArrays>(createFilter());
    DREAM3D_ASSERT(nullptr != filter);

    QVariant value;

    DataContainerArrayProxy dataContainerArrayProxy = DataContainerArrayProxy(filter->getDataContainerArray().get());
    setFlags(dataContainerArrayProxy, "", "AttributeMatrix", "");

    value.setValue(dataContainerArrayProxy);
    bool propSet = filter->setProperty("DataArraysToRemove", value);
    DREAM3D_REQUIRE_EQUAL(propSet, true);

    filter->execute();

    DataContainerArray::Pointer dca = filter->getDataContainerArray();
    DREAM3D_ASSERT(nullptr != dca);

    DataContainer::Pointer dc = dca->getDataContainer("DataContainer");
    DREAM3D_ASSERT(nullptr != dc);

    AttributeMatrix::Pointer am = dca->getAttributeMatrix(DataArrayPath("DataContainer", "AttributeMatrix", ""));
    DREAM3D_ASSERT(nullptr == am);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestDeleteDataContainer()
  {
    RemoveArrays::Pointer filter = std::dynamic_pointer_cast<RemoveArrays>(createFilter());
    DREAM3D_ASSERT(nullptr != filter);

    QVariant value;

    DataContainerArrayProxy dataContainerArrayProxy = DataContainerArrayProxy(filter->getDataContainerArray().get());
    setFlags(dataContainerArrayProxy, "DataContainer", "", "");

    value.setValue(dataContainerArrayProxy);
    bool propSet = filter->setProperty("DataArraysToRemove", value);
    DREAM3D_REQUIRE_EQUAL(propSet, true);

    filter->execute();

    DataContainerArray::Pointer dca = filter->getDataContainerArray();
    DREAM3D_ASSERT(nullptr != dca);

    DataContainer::Pointer dc = dca->getDataContainer("DataContainer");
    DREAM3D_ASSERT(nullptr == dc);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### RemoveArraysTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestDeleteDataArray());
    DREAM3D_REGISTER_TEST(TestDeleteAttributeMatrix());
    DREAM3D_REGISTER_TEST(TestDeleteDataContainer());
  }

public:
  RemoveArraysTest(const RemoveArraysTest&) = delete;            // Copy Constructor Not Implemented
  RemoveArraysTest(RemoveArraysTest&&) = delete;                 // Move Constructor Not Implemented
  RemoveArraysTest& operator=(const RemoveArraysTest&) = delete; // Copy Assignment Not Implemented
  RemoveArraysTest& operator=(RemoveArraysTest&&) = delete;      // Move Assignment Not Implemented
};
