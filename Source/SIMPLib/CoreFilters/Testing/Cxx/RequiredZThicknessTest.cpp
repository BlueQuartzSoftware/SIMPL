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

#include "SIMPLib/CoreFilters/RequiredZThickness.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

class RequiredZThicknessTest
{
public:
  RequiredZThicknessTest()
  {
  }
  virtual ~RequiredZThicknessTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc1 = DataContainer::New();
    dc1->setName("DataContainer1");
    dca->addDataContainer(dc1);

    DataContainer::Pointer dc2 = DataContainer::New();
    dc2->setName("DataContainer2");
    dca->addDataContainer(dc2);

    ImageGeom::Pointer imgGeom = ImageGeom::New();
    imgGeom->setDimensions(2, 2, 2);
    dc1->setGeometry(imgGeom);

    // incorrect geometry
    TriangleGeom::Pointer triGeom = TriangleGeom::New();

    QVector<size_t> dims;
    dims.push_back(3);
    DataArray<float>::Pointer verts = DataArray<float>::CreateArray(3, dims, "Vertices");
    verts->setComponent(0, 0, -1);
    verts->setComponent(0, 1, 0);
    verts->setComponent(0, 2, 0);

    verts->setComponent(1, 0, 0);
    verts->setComponent(1, 1, 0);
    verts->setComponent(1, 2, 1);

    verts->setComponent(2, 0, 0);
    verts->setComponent(2, 1, 1);
    verts->setComponent(2, 2, 0);

    QVector<size_t> dims2;
    dims2.push_back(3);
    DataArray<int64_t>::Pointer tris = DataArray<int64_t>::CreateArray(1, dims2, "Triangles");

    tris->setComponent(0, 0, 0);
    tris->setComponent(0, 1, 1);
    tris->setComponent(0, 2, 2);

    triGeom->setVertices(verts);
    triGeom->setTriangles(tris);
    dc2->setGeometry(triGeom);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  RequiredZThickness::Pointer createFilter()
  {
    // Now instantiate the RequiredZThickness Filter from the FilterManager
    QString filtName = "RequiredZThickness";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The RequiredZThicknessTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    RequiredZThickness::Pointer filter = std::dynamic_pointer_cast<RequiredZThickness>(filterFactory->create());
    DREAM3D_REQUIRE(nullptr != filter.get());

    return filter;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setGeometryTest(RequiredZThickness::Pointer filter, int numZVoxels)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(QString("DataContainer1"));
    filter->setProperty("DataContainerSelection", value);

    value.setValue(numZVoxels);
    filter->setProperty("NumZVoxels", value);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setInvalidGeometry(RequiredZThickness::Pointer filter, int numZVoxels)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(QString("DataContainer2"));
    filter->setProperty("DataContainerSelection", value);

    value.setValue(numZVoxels);
    filter->setProperty("NumZVoxels", value);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setNullDataContainer(RequiredZThickness::Pointer filter)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(QString("DataContainer3"));
    filter->setProperty("DataContainerSelection", value);

    value.setValue(0);
    filter->setProperty("NumZVoxels", value);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestValidZ()
  {
    RequiredZThickness::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setGeometryTest(filter, 1);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);

    // test Z equal to minimum Z
    setGeometryTest(filter, 2);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInvalidZ()
  {
    RequiredZThickness::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setGeometryTest(filter, 3);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -7788);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInvalidGeometry()
  {
    RequiredZThickness::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setInvalidGeometry(filter, 3);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -7789);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestNullDataContainer()
  {
    RequiredZThickness::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setNullDataContainer(filter);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -999);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### RequiredZThicknessTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestValidZ());
    DREAM3D_REGISTER_TEST(TestInvalidZ());

    DREAM3D_REGISTER_TEST(TestInvalidGeometry());
    DREAM3D_REGISTER_TEST(TestNullDataContainer());
  }

private:
  RequiredZThicknessTest(const RequiredZThicknessTest&); // Copy Constructor Not Implemented
  void operator=(const RequiredZThicknessTest&);         // Operator '=' Not Implemented
};
