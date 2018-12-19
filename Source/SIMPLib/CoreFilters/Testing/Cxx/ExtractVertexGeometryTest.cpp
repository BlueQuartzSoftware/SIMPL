// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include <array>
#include <numeric>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

#include "SIMPLib/CoreFilters/ExtractVertexGeometry.h"

// This header is auto-generated during cmake time
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"

class ExtractVertexGeometryTest
{

public:
  ExtractVertexGeometryTest() = default;
  ~ExtractVertexGeometryTest() = default;
  ExtractVertexGeometryTest(const ExtractVertexGeometryTest&) = delete;            // Copy Constructor
  ExtractVertexGeometryTest(ExtractVertexGeometryTest&&) = delete;                 // Move Constructor
  ExtractVertexGeometryTest& operator=(const ExtractVertexGeometryTest&) = delete; // Copy Assignment
  ExtractVertexGeometryTest& operator=(ExtractVertexGeometryTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::ExtractVertexGeometryTest::TestFile);
    QFile::remove(UnitTest::ExtractVertexGeometryTest::TestFileXdmf);
#endif
  }

  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the ExtractVertexGeometryTest Filter from the FilterManager
    QString filtName = "ExtractVertexGeometry";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The ExtractVertexGeometryTest Requires the use of the " << filtName.toStdString() << " filter which is found in the OrientationAnalysis Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray()
  {
    // Create the DataContainer Array
    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create the DataContainer for the ImageGeometry
    DataContainer::Pointer imageGeomDC = DataContainer::New(k_ImageGeomDataContainerName);
    dca->addDataContainer(imageGeomDC);

    DataContainer::Pointer badDataContainer = DataContainer::New(k_BadDataContainerName);
    dca->addDataContainer(badDataContainer);

    DataContainer::Pointer wrongGeomDataContainer = DataContainer::New(k_WrongGeomDataContainerName);
    dca->addDataContainer(wrongGeomDataContainer);
    TriangleGeom::Pointer triangleGeom = TriangleGeom::New();
    wrongGeomDataContainer->setGeometry(triangleGeom);

    // Create an ImageGeometry
    std::vector<size_t> dims = {10ULL, 20ULL, 30ULL};
    size_t cellCount = std::accumulate(dims.begin(), dims.end(), static_cast<size_t>(1), std::multiplies<size_t>());
    ImageGeom::Pointer imageGeom = ImageGeom::CreateGeometry("ImageGeometry");
    imageGeom->setDimensions(dims.data());
    imageGeomDC->setGeometry(imageGeom);

    // Create the Cell AttributeMatrix
    AttributeMatrix::Pointer cellAttrMat = AttributeMatrix::Create(dims, k_CellAttrMatName, AttributeMatrix::Type::Cell);
    imageGeomDC->addAttributeMatrix(cellAttrMat->getName(), cellAttrMat);

    // Create a cell attribute array
    FloatArrayType::Pointer f32Data = FloatArrayType::CreateArray(cellCount, k_FloatArrayName, true);
    f32Data->initializeWithValue(45.243f);
    cellAttrMat->addAttributeArray(f32Data->getName(), f32Data);

    // Create a Feature AttributeMatrix with different dimensions
    dims[0] = 1;
    dims[1] = 2;
    dims[2] = 3;
    AttributeMatrix::Pointer featureAttrMat = AttributeMatrix::Create(dims, k_FeatureAttrMatName, AttributeMatrix::Type::CellFeature);
    imageGeomDC->addAttributeMatrix(featureAttrMat->getName(), featureAttrMat);

    // Create a feature attribute array
    cellCount = std::accumulate(dims.begin(), dims.end(), static_cast<size_t>(1), std::multiplies<size_t>());
    Int32ArrayType::Pointer i32Data = Int32ArrayType::CreateArray(cellCount, k_FeatureArrayName, true);
    i32Data->initializeWithValue(1);
    featureAttrMat->addAttributeArray(i32Data->getName(), i32Data);
    return dca;
  }

  // -----------------------------------------------------------------------------
  int TestExtractVertexGeometryTest()
  {
    static const int32_t k_MoveArrays = 0;
    static const int32_t k_CopyArrays = 1;

    FilterPipeline::Pointer pipeline = FilterPipeline::New();

    ExtractVertexGeometry::Pointer extVertGeomFilter = ExtractVertexGeometry::New();
    DataContainerArray::Pointer dca = createDataContainerArray();
    extVertGeomFilter->setDataContainerArray(dca);

    //
    extVertGeomFilter->setArrayHandling(-10);
    extVertGeomFilter->preflight();
    int32_t err = extVertGeomFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, -2002)

    dca = createDataContainerArray();
    extVertGeomFilter->setDataContainerArray(dca);
    extVertGeomFilter->setArrayHandling(10);
    extVertGeomFilter->preflight();
    err = extVertGeomFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, -2003)

    // Empty SelectedDataContainerName
    dca = createDataContainerArray();
    extVertGeomFilter->setDataContainerArray(dca);
    extVertGeomFilter->setArrayHandling(k_MoveArrays);
    extVertGeomFilter->preflight();
    err = extVertGeomFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, -2004)

    // Empty VertexDataContainerName
    dca = createDataContainerArray();
    extVertGeomFilter->setDataContainerArray(dca);
    extVertGeomFilter->setSelectedDataContainerName(k_ImageGeomDataContainerName);
    extVertGeomFilter->setVertexDataContainerName(QString(""));
    extVertGeomFilter->preflight();
    err = extVertGeomFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, -2006)

    // Set the Vertex Data Container to an existing DataContainer
    dca = createDataContainerArray();
    extVertGeomFilter->setDataContainerArray(dca);
    extVertGeomFilter->setVertexDataContainerName(k_ImageGeomDataContainerName);
    extVertGeomFilter->preflight();
    err = extVertGeomFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, -2007)

    // Set the Vertex Data Container to an existing DataContainer
    // Set a DataContainer that does not have a Geometry
    dca = createDataContainerArray();
    extVertGeomFilter->setDataContainerArray(dca);
    extVertGeomFilter->setVertexDataContainerName(k_VertexDataContainerName);
    extVertGeomFilter->setSelectedDataContainerName(k_BadDataContainerName);
    extVertGeomFilter->preflight();
    err = extVertGeomFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, -2008)

    // Set the Vertex Data Container to an existing DataContainer
    // Set a DataContainer that does not have the proper geometry
    dca = createDataContainerArray();
    extVertGeomFilter->setDataContainerArray(dca);
    extVertGeomFilter->setVertexDataContainerName(k_VertexDataContainerName);
    extVertGeomFilter->setSelectedDataContainerName(k_WrongGeomDataContainerName);
    extVertGeomFilter->preflight();
    err = extVertGeomFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, -2010)

    // Set the Vertex Data Container to an existing DataContainer
    dca = createDataContainerArray();
    extVertGeomFilter->setDataContainerArray(dca);
    extVertGeomFilter->setVertexDataContainerName(k_VertexDataContainerName);
    extVertGeomFilter->setSelectedDataContainerName(k_ImageGeomDataContainerName);
    // Set an incorrect AttributeMatrix/DataArray Path
    QVector<DataArrayPath> includePaths;
    DataArrayPath dap(k_ImageGeomDataContainerName, k_FeatureAttrMatName, k_FeatureArrayName);
    includePaths.push_back(dap);
    extVertGeomFilter->setIncludedDataArrayPaths(includePaths);
    extVertGeomFilter->preflight();
    err = extVertGeomFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, -2009)

    // Set the correct paths.
    dca = createDataContainerArray();
    extVertGeomFilter->setDataContainerArray(dca);
    dap = DataArrayPath(k_ImageGeomDataContainerName, k_CellAttrMatName, k_FloatArrayName);
    includePaths[0] = dap;
    extVertGeomFilter->setIncludedDataArrayPaths(includePaths);
    extVertGeomFilter->preflight();
    err = extVertGeomFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0)

    // The preflight should be passing at this point so execute the filter
    dca = createDataContainerArray();
    extVertGeomFilter->setDataContainerArray(dca);
    extVertGeomFilter->execute();
    err = extVertGeomFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0)

    dap.update(k_ImageGeomDataContainerName, k_CellAttrMatName, k_FloatArrayName);
    bool exists = dca->doesAttributeArrayExist(dap);
    DREAM3D_REQUIRE_EQUAL(exists, false)

    dap.update(k_VertexDataContainerName, k_CellAttrMatName, k_FloatArrayName);
    exists = dca->doesAttributeArrayExist(dap);
    DREAM3D_REQUIRE_EQUAL(exists, true)

    // Now make a copy of the data arrays
    dca = createDataContainerArray();
    extVertGeomFilter->setDataContainerArray(dca);
    extVertGeomFilter->setArrayHandling(k_CopyArrays);
    extVertGeomFilter->execute();
    err = extVertGeomFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0)

    dap.update(k_ImageGeomDataContainerName, k_CellAttrMatName, k_FloatArrayName);
    exists = dca->doesAttributeArrayExist(dap);
    DREAM3D_REQUIRE_EQUAL(exists, true)

    dap.update(k_VertexDataContainerName, k_CellAttrMatName, k_FloatArrayName);
    exists = dca->doesAttributeArrayExist(dap);
    DREAM3D_REQUIRE_EQUAL(exists, true)
    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestExtractVertexGeometryTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  const QString k_ImageGeomDataContainerName = QString("ImageGeomDataContainer");
  const QString k_CellAttrMatName = QString("CellData");
  const QString k_FloatArrayName = QString("FloatArray");
  const QString k_VertexDataContainerName = QString("VertexDataContainer");
  const QString k_BadDataContainerName = QString("BadDataContainer");
  const QString k_WrongGeomDataContainerName = QString("WrongGeomDataContainer");
  const QString k_FeatureAttrMatName = QString("Feature Attr Mat");
  const QString k_FeatureArrayName = QString("featureData");
};
