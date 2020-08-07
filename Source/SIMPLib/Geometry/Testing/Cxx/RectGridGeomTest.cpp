
#include <cstdlib>

#include <iostream>

#include <QtCore/QFile>

#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class RectGridGeomTest
{
public:
  RectGridGeomTest() = default;

  virtual ~RectGridGeomTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestGetIndex()
  {
    RectGridGeom::Pointer geom = RectGridGeom::CreateGeometry("Test Geometry");
    SizeVec3Type dims(5, 5, 5);
    FloatArrayType::Pointer xBnds = FloatArrayType::CreateArray(6, QString("xBnds"), true);
    FloatArrayType::Pointer yBnds = FloatArrayType::CreateArray(6, QString("yBnds"), true);
    FloatArrayType::Pointer zBnds = FloatArrayType::CreateArray(6, QString("zBnds"), true);

    // Fill all three arrays with 0, 1, 2, 3, 4, 5
    std::generate(xBnds->begin(), xBnds->end(), [n = -1]() mutable { return n += 1; });
    std::generate(yBnds->begin(), yBnds->end(), [n = -1]() mutable { return n += 1; });
    std::generate(zBnds->begin(), zBnds->end(), [n = -1]() mutable { return n += 1; });

    geom->setDimensions(dims);
    geom->setXBounds(xBnds);
    geom->setYBounds(yBnds);
    geom->setZBounds(zBnds);

    // All three coordinates inside voxel (0, 0, 0)
    auto idxOpt = geom->getIndex(0.4, 0.4, 0.4);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), true)
    DREAM3D_REQUIRE_EQUAL(*idxOpt, 0)

    // All three coordinates inside voxel (4, 4, 4)
    idxOpt = geom->getIndex(4.99, 4.99, 4.99);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), true)
    DREAM3D_REQUIRE_EQUAL(*idxOpt, 124)

    // X Coordinate outside range (min)
    idxOpt = geom->getIndex(-0.01, 4.99, 4.99);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), false)

    // X Coordinate outside range (max)
    idxOpt = geom->getIndex(5.0, 4.99, 4.99);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), false)

    // Y Coordinate outside range (min)
    idxOpt = geom->getIndex(4.99, -0.01, 4.99);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), false)

    // Y Coordinate outside range (max)
    idxOpt = geom->getIndex(4.99, 5.0, 4.99);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), false)

    // Z Coordinate outside range (min)
    idxOpt = geom->getIndex(4.99, 4.99, -0.01);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), false)

    // Z Coordinate outside range (max)
    idxOpt = geom->getIndex(4.99, 4.99, 5.0);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), false)

    // More tests
    idxOpt = geom->getIndex(0.0, 3.2, 4.1);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), true)
    DREAM3D_REQUIRE_EQUAL(*idxOpt, 115)

    idxOpt = geom->getIndex(1.2, 1.9, 0.9);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), true)
    DREAM3D_REQUIRE_EQUAL(*idxOpt, 6)

    idxOpt = geom->getIndex(-56.0, -12.1, -2.3);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), false)

    idxOpt = geom->getIndex(43.2, 51.9, 82.1);
    DREAM3D_REQUIRE_EQUAL(idxOpt.has_value(), false)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### RectGridGeomTest Starting ####" << std::endl;
    int err = EXIT_SUCCESS;

    // Use this to register a specific function that will run a test
    DREAM3D_REGISTER_TEST(TestGetIndex());
  }

private:
  RectGridGeomTest(const RectGridGeomTest&) = delete; // Copy Constructor Not Implemented
  void operator=(const RectGridGeomTest&) = delete;   // Move assignment Not Implemented
};
