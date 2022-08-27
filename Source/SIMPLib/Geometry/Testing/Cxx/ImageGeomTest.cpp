
#include <cstdlib>

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
    SizeVec3Type dims(10, 20, 30);
    FloatVec3Type res = {0.4f, 2.3f, 5.0f};
    FloatVec3Type origin = {-1.0f, 6.0f, 10.0f};

    geom->setDimensions(dims);
    geom->setOrigin(origin);
    geom->setSpacing(res);

    float coords[3] = {2.5f, 9.23f, 12.78f};
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
    coords[0] = 2.9f;
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
    coords[0] = 2.5f;
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
  void TestCoordsToIndex()
  {
    ImageGeom::Pointer geom = ImageGeom::CreateGeometry("Test Geometry");
    SizeVec3Type dims(10, 20, 30);
    FloatVec3Type spacing = {0.5f, 0.5f, 0.5f};
    FloatVec3Type origin = {-10.0f, 5.0f, 2.0f};

    geom->setDimensions(dims);
    geom->setOrigin(origin);
    geom->setSpacing(spacing);

    {
      float coords[3] = {-9.9f, 5.25f, 2.15f};
      std::optional<size_t> result = geom->getIndex(coords[0], coords[1], coords[2]);
      DREAM3D_REQUIRE_EQUAL(result.has_value(), true)
      size_t index = result.value();
      DREAM3D_REQUIRE_EQUAL(index, 0)
    }

    {
      float coords[3] = {-9.26f, 5.25f, 2.1f};
      std::optional<size_t> result = geom->getIndex(coords[0], coords[1], coords[2]);
      DREAM3D_REQUIRE_EQUAL(result.has_value(), true)
      size_t index = result.value();
      DREAM3D_REQUIRE_EQUAL(index, 1)
    }

    {
      float coords[3] = {-9.8f, 5.8f, 2.05f};
      std::optional<size_t> result = geom->getIndex(coords[0], coords[1], coords[2]);
      DREAM3D_REQUIRE_EQUAL(result.has_value(), true)
      size_t index = result.value();
      DREAM3D_REQUIRE_EQUAL(index, 10)
    }

    {
      float coords[3] = {-6.55f, 5.6f, 2.45f};
      std::optional<size_t> result = geom->getIndex(coords[0], coords[1], coords[2]);
      DREAM3D_REQUIRE_EQUAL(result.has_value(), true)
      size_t index = result.value();
      DREAM3D_REQUIRE_EQUAL(index, 16)
    }

    {
      float coords[3] = {-6.95f, 5.9f, 2.55f};
      std::optional<size_t> result = geom->getIndex(coords[0], coords[1], coords[2]);
      DREAM3D_REQUIRE_EQUAL(result.has_value(), true)
      size_t index = result.value();
      DREAM3D_REQUIRE_EQUAL(index, 216)
    }

    {
      float coords[3] = {-10.0001f, 5.75f, 2.75f};
      std::optional<size_t> result = geom->getIndex(coords[0], coords[1], coords[2]);
      DREAM3D_REQUIRE_EQUAL(result.has_value(), false)
    }

    {
      float coords[3] = {-9.75f, 4.9999f, 2.75f};
      std::optional<size_t> result = geom->getIndex(coords[0], coords[1], coords[2]);
      DREAM3D_REQUIRE_EQUAL(result.has_value(), false)
    }

    {
      float coords[3] = {-9.75f, 5.75f, 1.9999f};
      std::optional<size_t> result = geom->getIndex(coords[0], coords[1], coords[2]);
      DREAM3D_REQUIRE_EQUAL(result.has_value(), false)
    }

    {
      float coords[3] = {-4.9f, 5.75f, 2.75f};
      std::optional<size_t> result = geom->getIndex(coords[0], coords[1], coords[2]);
      DREAM3D_REQUIRE_EQUAL(result.has_value(), false)
    }

    {
      float coords[3] = {-9.75f, 15.1f, 2.75f};
      std::optional<size_t> result = geom->getIndex(coords[0], coords[1], coords[2]);
      DREAM3D_REQUIRE_EQUAL(result.has_value(), false)
    }

    {
      float coords[3] = {-9.75f, 5.75f, 17.1f};
      std::optional<size_t> result = geom->getIndex(coords[0], coords[1], coords[2]);
      DREAM3D_REQUIRE_EQUAL(result.has_value(), false)
    }
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
    DREAM3D_REGISTER_TEST(TestCoordsToIndex());
    DREAM3D_REGISTER_TEST(RemoveTestFiles());
  }

private:
  ImageGeomTest(const ImageGeomTest&) = delete;  // Copy Constructor Not Implemented
  void operator=(const ImageGeomTest&) = delete; // Move assignment Not Implemented
};
