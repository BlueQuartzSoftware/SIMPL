// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#include "SIMPLib/Filtering/QMetaObjectUtilities.h"

#include "SIMPLib/Testing/UnitTestSupport.hpp"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"

class ConvertDataContainerGeometryTest
{

  public:
    ConvertDataContainerGeometryTest() = default;
    ~ConvertDataContainerGeometryTest() = default;
    ConvertDataContainerGeometryTest(const ConvertDataContainerGeometryTest&) = delete;            // Copy Constructor
    ConvertDataContainerGeometryTest(ConvertDataContainerGeometryTest&&) = delete;                 // Move Constructor
    ConvertDataContainerGeometryTest& operator=(const ConvertDataContainerGeometryTest&) = delete; // Copy Assignment
    ConvertDataContainerGeometryTest& operator=(ConvertDataContainerGeometryTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
  #if REMOVE_TEST_FILES
    QFile::remove(UnitTest::ConvertDataContainerGeometryTest::TestFile1);
    QFile::remove(UnitTest::ConvertDataContainerGeometryTest::TestFile2);
  #endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the ConvertDataContainerGeometryTest Filter from the FilterManager
    QString filtName = "ConvertDataContainerGeometry";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if (nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The ConvertDataContainerGeometryTest Requires the use of the " << filtName.toStdString()
         << " filter which is found in the OrientationAnalysis Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestConvertDataContainerGeometryTest()
  {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   /* Please write ConvertDataContainerGeometryTest test code here.
    *
    * Your IO test files are:
    * UnitTest::ConvertDataContainerGeometryTest::TestFile1
    * UnitTest::ConvertDataContainerGeometryTest::TestFile2
    *
    * SIMPLib provides some macros that will throw exceptions when a test fails
    * and thus report that during testing. These macros are located in the
    * SIMPLib/Utilities/UnitTestSupport.hpp file. Some examples are:
    *
    * SIMPLib_REQUIRE_EQUAL(foo, 0)
    * This means that if the variable foo is NOT equal to Zero then test will fail
    * and the current test will exit immediately. If there are more tests registered
    * with the SIMPLib_REGISTER_TEST() macro, the next test will execute. There are
    * lots of examples in the SIMPLib/Test folder to look at.
    */
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    int foo = -1;
    DREAM3D_REQUIRE_EQUAL(foo, 0)

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST( TestFilterAvailability() );

    DREAM3D_REGISTER_TEST( TestConvertDataContainerGeometryTest() )

    DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  }

  private:


};

