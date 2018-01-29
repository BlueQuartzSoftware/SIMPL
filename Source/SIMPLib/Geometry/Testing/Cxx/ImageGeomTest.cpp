
#include <stdlib.h>

#include <iostream>

#include <QtCore/QFile>

#include "SIMPLib/Geometry/ImageGeom.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class ImageGeomTest
{
public:
  ImageGeomTest() = default;

  virtual ~ImageGeomTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
//      QFile::remove(UnitTest::FindDerivativesFilterTest::TestFile1);
//      QFile::remove(UnitTest::FindDerivativesFilterTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestIndexCalculation()
  {
    ImageGeom::Pointer geom = ImageGeom::CreateGeometry("Test Geometry");
    size_t dims[3] = {10, 20, 30};
    float res[3] = {0.4f, 2.3f, 5.0f};
    float origin[3] = {-1.0f, 6.0f, 10.0f};
    size_t totalCells = dims[0] * dims[1] * dims[2];

    geom->setDimensions(dims);
    geom->setOrigin(origin);
    geom->getResolution(res);

    float coords[3] = {3.5f, 9.23f, 12.78f};
    size_t indices[3] = {0, 0, 0};
    size_t index = 0;
    ImageGeom::ErrorType err = geom->computeCellIndex(coords, indices);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::NoError)
    err = geom->computeCellIndex(coords, index);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::NoError)

    // X Coord out of bounds
    coords[0] = -5.0f;
    err = geom->computeCellIndex(coords, indices);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::XOutOfBoundsLow)
    err = geom->computeCellIndex(coords, index);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::XOutOfBoundsLow)

    coords[0] = 200.0f;
    err = geom->computeCellIndex(coords, indices);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::XOutOfBoundsHigh)
    err = geom->computeCellIndex(coords, index);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::XOutOfBoundsHigh)

    // Y Coord out of bounds
    coords[0] = 3.5f;
    coords[1] = 4.0f;
    err = geom->computeCellIndex(coords, indices);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::YOutOfBoundsLow)
    err = geom->computeCellIndex(coords, index);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::YOutOfBoundsLow)
    coords[1] = 200.0f;
    err = geom->computeCellIndex(coords, indices);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::YOutOfBoundsHigh)
    err = geom->computeCellIndex(coords, index);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::YOutOfBoundsHigh)

    // Z Coord out of bounds
    coords[0] = 3.5f;
    coords[1] = 9.23f;
    coords[2] = 5.0f;
    err = geom->computeCellIndex(coords, indices);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::ZOutOfBoundsLow)
    err = geom->computeCellIndex(coords, index);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::ZOutOfBoundsLow)
    coords[2] = 2000.0f;
    err = geom->computeCellIndex(coords, indices);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::ZOutOfBoundsHigh)
    err = geom->computeCellIndex(coords, index);
    DREAM3D_REQUIRE(err == ImageGeom::ErrorType::ZOutOfBoundsHigh)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ImageGeomTest Starting ####" << std::endl;
    int err = EXIT_SUCCESS;

    // Use this to register a specific function that will run a test
    DREAM3D_REGISTER_TEST(TestIndexCalculation());
    DREAM3D_REGISTER_TEST(RemoveTestFiles());
  }

private:
  ImageGeomTest(const ImageGeomTest&) = delete;  // Copy Constructor Not Implemented
  void operator=(const ImageGeomTest&) = delete; // Operator '=' Not Implemented
};
