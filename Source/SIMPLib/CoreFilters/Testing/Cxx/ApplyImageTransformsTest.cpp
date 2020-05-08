// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include "SIMPLib/CoreFilters/ApplyImageTransforms.h"
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

class ApplyImageTransformsTest
{

public:
  ApplyImageTransformsTest() = default;
  ~ApplyImageTransformsTest() = default;
  ApplyImageTransformsTest(const ApplyImageTransformsTest&) = delete;            // Copy Constructor
  ApplyImageTransformsTest(ApplyImageTransformsTest&&) = delete;                 // Move Constructor
  ApplyImageTransformsTest& operator=(const ApplyImageTransformsTest&) = delete; // Copy Assignment
  ApplyImageTransformsTest& operator=(ApplyImageTransformsTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestFilterAvailability()
  {
    ApplyImageTransforms::Pointer filter = ApplyImageTransforms::New();
    filter->preflight();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestApplyImageTransformsTest()
  {
    int foo = 0;
    DREAM3D_REQUIRE_EQUAL(foo, 0)

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestApplyImageTransformsTest())
  }

private:
};
