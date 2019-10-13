// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/CreateDataArray.h"
#include "SIMPLib/CoreFilters/CreateDataContainer.h"
#include "SIMPLib/CoreFilters/CreateGeometry.h"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
#include "SIMPLib/CoreFilters/GenerateTiltSeries.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

// These Headers are generated into the build directory
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

// -----------------------------------------------------------------------------
QTextStream& operator<<(QTextStream& out, const FilterPipeline::ExecutionResult& result)
{
  out << static_cast<unsigned int>(result);
  return out;
}

// -----------------------------------------------------------------------------
class GenerateTiltSeriesTest
{
  const DataArrayPath k_DCName = DataArrayPath("Data Container", "", "");
  const DataArrayPath k_CellDataArray = DataArrayPath("Data Container", "Cell Data", "FeatureIds");

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
    CreateDataContainer::Pointer createDataContainer = CreateDataContainer::New();
    createDataContainer->setDataContainerName(k_DCName);

    CreateGeometry::Pointer createGeometry = CreateGeometry::New();
    IntVec3Type dims(500, 500, 500);
    createGeometry->setDimensions(dims);
    FloatVec3Type spacing(0.25f, 0.5f, 1.2f);
    createGeometry->setSpacing(spacing);
    FloatVec3Type origin(-100.34f, 234.947f, 1823.3f);
    createGeometry->setOrigin(origin);
    createGeometry->setDataContainerName(k_DCName);
    createGeometry->setImageCellAttributeMatrixName("Cell Data");

    CreateDataArray::Pointer createDataArray = CreateDataArray::New();
    createDataArray->setScalarType(SIMPL::ScalarTypes::Type::Int32);
    createDataArray->setNumberOfComponents(1);
    createDataArray->setNewArray(k_CellDataArray);

    GenerateTiltSeries::Pointer generateTiltSeries = GenerateTiltSeries::New();
    generateTiltSeries->setRotationAxis(0);
    generateTiltSeries->setIncrement(1.0f);
    FloatVec3Type sampleSpacing(0.1f, 0.4f, 1.5f);
    generateTiltSeries->setSpacing(sampleSpacing);
    generateTiltSeries->setInputDataArrayPath(k_CellDataArray);

    DataContainerWriter::Pointer writer = DataContainerWriter::New();
    writer->setOutputFile(UnitTest::GenerateTiltSeriesTest::TestFile1);

    FilterPipeline::Pointer pipeline = FilterPipeline::New();
    pipeline->pushBack(createDataContainer);
    pipeline->pushBack(createGeometry);
    pipeline->pushBack(createDataArray);
    pipeline->pushBack(generateTiltSeries);
    pipeline->pushBack(writer);

    int err = pipeline->preflightPipeline();
    DREAM3D_REQUIRED(err, >=, 0)

    pipeline->execute();
    FilterPipeline::ExecutionResult result = pipeline->getExecutionResult();
    DREAM3D_REQUIRE_EQUAL(result, FilterPipeline::ExecutionResult::Completed)

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
