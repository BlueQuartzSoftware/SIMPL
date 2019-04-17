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

#include "SIMPLib/CoreFilters/SetOriginResolutionImageGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

namespace SetOriginResolutionImageGeometryTest
{
const FloatVec3_t ORIGIN = FloatVec3_t{1, 2, 3};
const FloatVec3_t RESOLUTION = FloatVec3_t{4, 5, 6};
}

class SetOriginResolutionImageGeomTest
{
public:
  SetOriginResolutionImageGeomTest() = default;
  virtual ~SetOriginResolutionImageGeomTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc1 = DataContainer::New("DataContainer1");
    dca->addDataContainer(dc1);

    DataContainer::Pointer dc2 = DataContainer::New("DataContainer2");
    dca->addDataContainer(dc2);

    DataContainer::Pointer dc3 = DataContainer::New("DataContainer3");
    dca->addDataContainer(dc3);

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
  SetOriginResolutionImageGeom::Pointer createFilter()
  {
    // Now instantiate the SetOriginResolutionImageGeom Filter from the FilterManager
    QString filtName = "SetOriginResolutionImageGeom";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The SetOriginResolutionImageGeomTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    SetOriginResolutionImageGeom::Pointer filter = std::dynamic_pointer_cast<SetOriginResolutionImageGeom>(filterFactory->create());
    DREAM3D_REQUIRE(nullptr != filter.get());

    return filter;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setGeometryTest(SetOriginResolutionImageGeom::Pointer filter)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(QString("DataContainer1"));
    filter->setProperty("DataContainerName", value);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setInvalidGeometry(SetOriginResolutionImageGeom::Pointer filter)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(QString("DataContainer2"));
    filter->setProperty("DataContainerName", value);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setNullGeometry(SetOriginResolutionImageGeom::Pointer filter)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(QString("DataContainer3"));
    filter->setProperty("DataContainerName", value);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void changeOrigin(SetOriginResolutionImageGeom::Pointer filter)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(true);
    filter->setProperty("ChangeOrigin", value);

    value.setValue(SetOriginResolutionImageGeometryTest::ORIGIN);
    filter->setProperty("Origin", value);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void changeResolution(SetOriginResolutionImageGeom::Pointer filter)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(true);
    filter->setProperty("ChangeResolution", value);

    value.setValue(SetOriginResolutionImageGeometryTest::RESOLUTION);
    filter->setProperty("Resolution", value);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestOrigin()
  {
    SetOriginResolutionImageGeom::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setGeometryTest(filter);
    changeOrigin(filter);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);

    DataContainer::Pointer dc = filter->getDataContainerArray()->getDataContainer("DataContainer1");
    ImageGeom::Pointer imgGeom = std::dynamic_pointer_cast<ImageGeom>(dc->getGeometry());

    DREAM3D_REQUIRE(imgGeom != nullptr);

    float origin[3];
    imgGeom->getOrigin(origin);
    DREAM3D_REQUIRE_EQUAL(origin[0], SetOriginResolutionImageGeometryTest::ORIGIN.x);
    DREAM3D_REQUIRE_EQUAL(origin[1], SetOriginResolutionImageGeometryTest::ORIGIN.y);
    DREAM3D_REQUIRE_EQUAL(origin[2], SetOriginResolutionImageGeometryTest::ORIGIN.z);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestResolution()
  {
    SetOriginResolutionImageGeom::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setGeometryTest(filter);
    changeResolution(filter);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);

    DataContainer::Pointer dc = filter->getDataContainerArray()->getDataContainer("DataContainer1");
    ImageGeom::Pointer imgGeom = std::dynamic_pointer_cast<ImageGeom>(dc->getGeometry());

    DREAM3D_REQUIRE(imgGeom != nullptr);

    float xRes = 0.0f;
    float yRes = 0.0f;
    float zRes = 0.0f;
    std::tie(xRes, yRes, zRes) = imgGeom->getResolution();

    DREAM3D_REQUIRE_EQUAL(xRes, SetOriginResolutionImageGeometryTest::RESOLUTION.x);
    DREAM3D_REQUIRE_EQUAL(yRes, SetOriginResolutionImageGeometryTest::RESOLUTION.y);
    DREAM3D_REQUIRE_EQUAL(zRes, SetOriginResolutionImageGeometryTest::RESOLUTION.z);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInvalidGeometry()
  {
    SetOriginResolutionImageGeom::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setInvalidGeometry(filter);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -384);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestNullGeometry()
  {
    SetOriginResolutionImageGeom::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setNullGeometry(filter);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -385);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### SetOriginResolutionImageGeomTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestOrigin());
    DREAM3D_REGISTER_TEST(TestResolution());

    DREAM3D_REGISTER_TEST(TestInvalidGeometry());
    DREAM3D_REGISTER_TEST(TestNullGeometry());
  }

private:
  SetOriginResolutionImageGeomTest(const SetOriginResolutionImageGeomTest&); // Copy Constructor Not Implemented
  void operator=(const SetOriginResolutionImageGeomTest&);                   // Move assignment Not Implemented
};
