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
#include "SIMPLib/CoreFilters/ScaleVolume.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

namespace ScaleVolumeTestConsts
{
const FloatVec3Type SCALE = FloatVec3Type{3, 3, 3};
}

class ScaleVolumeTest
{
public:
  ScaleVolumeTest() = default;
  virtual ~ScaleVolumeTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc1 = DataContainer::New("DataContainer1");
    dca->addOrReplaceDataContainer(dc1);

    DataContainer::Pointer dc2 = DataContainer::New("DataContainer2");
    dca->addOrReplaceDataContainer(dc2);

    DataContainer::Pointer dc3 = DataContainer::New("DataContainer3");
    dca->addOrReplaceDataContainer(dc3);

    DataContainer::Pointer dc4 = DataContainer::New("DataContainer4");
    dca->addOrReplaceDataContainer(dc4);

    ImageGeom::Pointer imgGeom = ImageGeom::New();
    imgGeom->setDimensions(2, 2, 2);
    dc1->setGeometry(imgGeom);

    TriangleGeom::Pointer triGeom = TriangleGeom::New();

    std::vector<size_t> dims;
    dims.push_back(3);
    DataArray<float>::Pointer verts = DataArray<float>::CreateArray(3, dims, "Vertices", true);
    verts->setComponent(0, 0, -1);
    verts->setComponent(0, 1, 0);
    verts->setComponent(0, 2, 0);

    verts->setComponent(1, 0, 0);
    verts->setComponent(1, 1, 0);
    verts->setComponent(1, 2, 1);

    verts->setComponent(2, 0, 0);
    verts->setComponent(2, 1, 1);
    verts->setComponent(2, 2, 0);

    std::vector<size_t> dims2;
    dims2.push_back(3);
    SharedTriList::Pointer tris = SharedTriList::CreateArray(1, dims2, "Triangles", true);

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
  ScaleVolume::Pointer createFilter()
  {
    // Now instantiate the ScaleVolume Filter from the FilterManager
    QString filtName = "ScaleVolume";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The ScaleVolumeTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    ScaleVolume::Pointer filter = std::dynamic_pointer_cast<ScaleVolume>(filterFactory->create());
    DREAM3D_REQUIRE(nullptr != filter.get());

    return filter;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setGeometryTest(ScaleVolume::Pointer filter, bool imageGeometry, bool surfaceGeometry)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(imageGeometry);
    bool propWasSet = filter->setProperty("ApplyToVoxelVolume", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(surfaceGeometry);
    propWasSet = filter->setProperty("ApplyToSurfaceMesh", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(ScaleVolumeTestConsts::SCALE);
    propWasSet = filter->setProperty("ScaleFactor", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(DataArrayPath("DataContainer1"));
    propWasSet = filter->setProperty("DataContainerName", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(DataArrayPath("DataContainer2"));
    propWasSet = filter->setProperty("SurfaceDataContainerName", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setIncorrectGeometry(ScaleVolume::Pointer filter, bool imageGeometry, bool surfaceGeometry)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(imageGeometry);
    bool propWasSet = filter->setProperty("ApplyToVoxelVolume", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(surfaceGeometry);
    propWasSet = filter->setProperty("ApplyToSurfaceMesh", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(ScaleVolumeTestConsts::SCALE);
    propWasSet = filter->setProperty("ScaleFactor", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(DataArrayPath("DataContainer2"));
    propWasSet = filter->setProperty("DataContainerName", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(DataArrayPath("DataContainer1"));
    propWasSet = filter->setProperty("SurfaceDataContainerName", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setNullGeometry(ScaleVolume::Pointer filter, bool imageGeometry, bool surfaceGeometry)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(imageGeometry);
    bool propWasSet = filter->setProperty("ApplyToVoxelVolume", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(surfaceGeometry);
    propWasSet = filter->setProperty("ApplyToSurfaceMesh", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(ScaleVolumeTestConsts::SCALE);
    propWasSet = filter->setProperty("ScaleFactor", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(DataArrayPath("DataContainer3"));
    propWasSet = filter->setProperty("DataContainerName", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(DataArrayPath("DataContainer4"));
    propWasSet = filter->setProperty("SurfaceDataContainerName", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestImageGeometry()
  {
    ScaleVolume::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setGeometryTest(filter, true, false);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), 0);

    DataContainer::Pointer dc = filter->getDataContainerArray()->getDataContainer("DataContainer1");
    ImageGeom::Pointer imgGeom = std::dynamic_pointer_cast<ImageGeom>(dc->getGeometry());

    DREAM3D_REQUIRE(imgGeom != nullptr);

    FloatVec3Type spacing = imgGeom->getSpacing();

    DREAM3D_REQUIRE_EQUAL(spacing[0], ScaleVolumeTestConsts::SCALE[0]);
    DREAM3D_REQUIRE_EQUAL(spacing[1], ScaleVolumeTestConsts::SCALE[1]);
    DREAM3D_REQUIRE_EQUAL(spacing[2], ScaleVolumeTestConsts::SCALE[2]);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestSurfaceGeometry()
  {
    ScaleVolume::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setGeometryTest(filter, false, true);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), 0);

    DataContainer::Pointer dc = filter->getDataContainerArray()->getDataContainer("DataContainer2");
    TriangleGeom::Pointer triGeom = std::dynamic_pointer_cast<TriangleGeom>(dc->getGeometry());

    DREAM3D_REQUIRE(triGeom != nullptr);

    DataArray<float>::Pointer verts = triGeom->getVertices();

    DREAM3D_REQUIRE_EQUAL(verts->getComponent(0, 0), -1);
    DREAM3D_REQUIRE_EQUAL(verts->getComponent(0, 1), 0);
    DREAM3D_REQUIRE_EQUAL(verts->getComponent(0, 2), 0);

    DREAM3D_REQUIRE_EQUAL(verts->getComponent(1, 0), 2);
    DREAM3D_REQUIRE_EQUAL(verts->getComponent(1, 1), 0);
    DREAM3D_REQUIRE_EQUAL(verts->getComponent(1, 2), 3);

    DREAM3D_REQUIRE_EQUAL(verts->getComponent(2, 0), 2);
    DREAM3D_REQUIRE_EQUAL(verts->getComponent(2, 1), 3);
    DREAM3D_REQUIRE_EQUAL(verts->getComponent(2, 2), 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInvalidImageGeom()
  {
    ScaleVolume::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setIncorrectGeometry(filter, true, false);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -384);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInvalidSurfaceGeom()
  {
    ScaleVolume::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setIncorrectGeometry(filter, false, true);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -384);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestNullImageGeom()
  {
    ScaleVolume::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setNullGeometry(filter, true, false);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -385);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestNullSurfaceGeom()
  {
    ScaleVolume::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    setNullGeometry(filter, false, true);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -385);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ScaleVolumeTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestImageGeometry());
    DREAM3D_REGISTER_TEST(TestSurfaceGeometry());

    DREAM3D_REGISTER_TEST(TestInvalidImageGeom());
    DREAM3D_REGISTER_TEST(TestInvalidSurfaceGeom());

    DREAM3D_REGISTER_TEST(TestNullImageGeom());
    DREAM3D_REGISTER_TEST(TestNullSurfaceGeom());
  }

private:
  ScaleVolumeTest(const ScaleVolumeTest&); // Copy Constructor Not Implemented
  void operator=(const ScaleVolumeTest&);  // Move assignment Not Implemented
};
