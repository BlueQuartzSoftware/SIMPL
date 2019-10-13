// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/GenerateTiltSeries.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "UnitTestSupport.hpp"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"

class GenerateTiltSeriesTest
{

  public:
    GenerateTiltSeriesTest() = default;
    ~GenerateTiltSeriesTest() = default;
    GenerateTiltSeriesTest(const GenerateTiltSeriesTest&) = delete;            // Copy Constructor
    GenerateTiltSeriesTest(GenerateTiltSeriesTest&&) = delete;                 // Move Constructor
    GenerateTiltSeriesTest& operator=(const GenerateTiltSeriesTest&) = delete; // Copy Assignment
    GenerateTiltSeriesTest& operator=(GenerateTiltSeriesTest&&) = delete;      // Move Assignment

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void RemoveTestFiles()
    {
#if REMOVE_TEST_FILES
      QFile::remove(UnitTest::GenerateTiltSeriesTest::TestFile1);
      QFile::remove(UnitTest::GenerateTiltSeriesTest::TestFile2);
#endif
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    int TestGenerateTiltSeriesTest()
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

      DREAM3D_REGISTER_TEST(TestGenerateTiltSeriesTest())

      DREAM3D_REGISTER_TEST(RemoveTestFiles())
    }

  private:


};

