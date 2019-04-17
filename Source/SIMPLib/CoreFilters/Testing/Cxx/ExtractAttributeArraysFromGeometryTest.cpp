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

#pragma once

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Filtering/QMetaObjectUtilities.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/HexahedralGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "SIMPLib/Common/Constants.h"

class ExtractAttributeArraysFromGeometryTest
{

public:
  ExtractAttributeArraysFromGeometryTest() = default;
  ~ExtractAttributeArraysFromGeometryTest() = default;
  ExtractAttributeArraysFromGeometryTest(const ExtractAttributeArraysFromGeometryTest&) = delete;            // Copy Constructor
  ExtractAttributeArraysFromGeometryTest(ExtractAttributeArraysFromGeometryTest&&) = delete;                 // Move Constructor
  ExtractAttributeArraysFromGeometryTest& operator=(const ExtractAttributeArraysFromGeometryTest&) = delete; // Copy Assignment
  ExtractAttributeArraysFromGeometryTest& operator=(ExtractAttributeArraysFromGeometryTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the ExtractAttributeArraysFromGeometryTest Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The ExtractAttributeArraysFromGeometryTest Requires the use of the " << m_FilterName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> std::shared_ptr<DataArray<T>> createDataArray(const QString& daName, const std::vector<std::vector<T>>& data, const QVector<size_t>& tupleDims, const QVector<size_t>& cDims)
  {
    typename DataArray<T>::Pointer da = DataArray<T>::CreateArray(tupleDims, cDims, daName);
    for(int i = 0; i < da->getNumberOfTuples(); i++)
    {
      da->setTuple(i, data[i]);
    }
    return da;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void checkDataArray(std::shared_ptr<DataArray<T>> originalData, std::shared_ptr<DataArray<T>> newData)
  {
    DREAM3D_REQUIRE_EQUAL(originalData->getSize(), newData->getSize())

    for(int i = 0; i < originalData->getSize(); i++)
    {
      T* valueOld = originalData->getPointer(i);
      T* valueNew = newData->getPointer(i);
      DREAM3D_REQUIRE_EQUAL(valueNew[0], valueOld[0])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void ExtractAttributeArraysFromGeometryRectGridTest()
  {
    static const QString k_DataContainerName("DataContainer");
    static const QString k_XBoundsDAName("XBounds");
    static const QString k_YBoundsDAName("YBounds");
    static const QString k_ZBoundsDAName("ZBounds");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrix

    AttributeMatrix::Pointer boundsAM = AttributeMatrix::New(m_Dims4, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
    dc->addAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName, boundsAM);

    // Create Geometry

    DataArray<float>::Pointer daXBounds = DataArray<float>::CreateArray(m_Dims4, m_Dims1, "X");
    for(size_t i = 0; i < daXBounds->getSize(); i++)
    {
      daXBounds->setValue(i, i);
    }

    DataArray<float>::Pointer daYBounds = DataArray<float>::CreateArray(m_Dims4, m_Dims1, "Y");
    for(size_t i = 0; i < daYBounds->getSize(); i++)
    {
      daYBounds->setValue(i, i);
    }

    DataArray<float>::Pointer daZBounds = DataArray<float>::CreateArray(m_Dims4, m_Dims1, "Z");
    for(size_t i = 0; i < daZBounds->getSize(); i++)
    {
      daZBounds->setValue(i, i);
    }

    RectGridGeom::Pointer geom = RectGridGeom::CreateGeometry(SIMPL::Geometry::RectGridGeometry);

    geom->setXBounds(daXBounds);
    geom->setYBounds(daYBounds);
    geom->setZBounds(daZBounds);

    dc->setGeometry(geom);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer extractAttributeArraysFromGeometry = filterFactory->create();

    extractAttributeArraysFromGeometry->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    bool propWasSet = extractAttributeArraysFromGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, k_XBoundsDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("XBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, k_YBoundsDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("YBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, k_ZBoundsDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("ZBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run Filter

    extractAttributeArraysFromGeometry->execute();
    DREAM3D_REQUIRED(extractAttributeArraysFromGeometry->getErrorCondition(), >=, 0);

    // Check Filter Results

    QList<QString> boundsNames = boundsAM->getAttributeArrayNames();
    DREAM3D_REQUIRE_EQUAL(boundsNames.contains(k_XBoundsDAName), true)
    DREAM3D_REQUIRE_EQUAL(boundsNames.contains(k_YBoundsDAName), true)
    DREAM3D_REQUIRE_EQUAL(boundsNames.contains(k_ZBoundsDAName), true)

    FloatArrayType::Pointer originalXBounds = geom->getXBounds();
    FloatArrayType::Pointer originalYBounds = geom->getXBounds();
    FloatArrayType::Pointer originalZBounds = geom->getXBounds();

    FloatArrayType::Pointer newXBounds = boundsAM->getAttributeArrayAs<FloatArrayType>(k_XBoundsDAName);
    FloatArrayType::Pointer newYBounds = boundsAM->getAttributeArrayAs<FloatArrayType>(k_YBoundsDAName);
    FloatArrayType::Pointer newZBounds = boundsAM->getAttributeArrayAs<FloatArrayType>(k_ZBoundsDAName);

    checkDataArray<float>(originalXBounds, newXBounds);
    checkDataArray<float>(originalYBounds, newYBounds);
    checkDataArray<float>(originalZBounds, newZBounds);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void ExtractAttributeArraysFromGeometryVertexTest()
  {
    static const QString k_DataContainerName("DataContainer");
    static const QString k_VerticesDAName("VertexCoordinates");
    static const QString k_ElementListDAName("Connectivity");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrix

    AttributeMatrix::Pointer vertexAM = AttributeMatrix::New(m_Dims2, SIMPL::Defaults::VertexAttributeMatrixName, AttributeMatrix::Type::Vertex);
    dc->addAttributeMatrix(SIMPL::Defaults::VertexAttributeMatrixName, vertexAM);

    // Create Geometry

    std::vector<std::vector<float>> vertices = {{1.0, 1.0, 0.0}, {3.0, 1.0, 0.0}};

    DataArray<float>::Pointer daVert = createDataArray<float>("Vertices", vertices, m_Dims2, m_Dims3);

    VertexGeom::Pointer geom = VertexGeom::CreateGeometry(daVert, SIMPL::Geometry::VertexGeometry);

    dc->setGeometry(geom);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer extractAttributeArraysFromGeometry = filterFactory->create();

    extractAttributeArraysFromGeometry->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    bool propWasSet = extractAttributeArraysFromGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, k_VerticesDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("SharedVertexListArrayPath0", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run Filter

    extractAttributeArraysFromGeometry->execute();
    DREAM3D_REQUIRED(extractAttributeArraysFromGeometry->getErrorCondition(), >=, 0);

    // Check Filter Results

    QList<QString> vertexNames = vertexAM->getAttributeArrayNames();
    DREAM3D_REQUIRE_EQUAL(vertexNames.contains(k_VerticesDAName), true)

    FloatArrayType::Pointer originalVertices = geom->getVertices();

    FloatArrayType::Pointer newVertices = vertexAM->getAttributeArrayAs<FloatArrayType>(k_VerticesDAName);

    checkDataArray<float>(originalVertices, newVertices);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void ExtractAttributeArraysFromGeometryEdgeTest()
  {
    static const QString k_DataContainerName("DataContainer");
    static const QString k_VerticesDAName("VertexCoordinates");
    static const QString k_ElementListDAName("Connectivity");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrix

    AttributeMatrix::Pointer vertexAM = AttributeMatrix::New(m_Dims2, SIMPL::Defaults::VertexAttributeMatrixName, AttributeMatrix::Type::Vertex);
    dc->addAttributeMatrix(SIMPL::Defaults::VertexAttributeMatrixName, vertexAM);

    AttributeMatrix::Pointer elementAM = AttributeMatrix::New(m_Dims1, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Edge);
    dc->addAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName, elementAM);

    // Create Geometry

    std::vector<std::vector<float>> vertices = {{1.0, 1.0, 0.0}, {3.0, 1.0, 0.0}};

    std::vector<std::vector<int64_t>> elements = {{0, 1}};

    DataArray<float>::Pointer daVert = createDataArray<float>("Vertices", vertices, m_Dims2, m_Dims3);
    DataArray<int64_t>::Pointer daList = createDataArray<int64_t>("List", elements, m_Dims1, m_Dims2);

    EdgeGeom::Pointer geom = EdgeGeom::CreateGeometry(daList, daVert, SIMPL::Geometry::EdgeGeometry);

    dc->setGeometry(geom);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer extractAttributeArraysFromGeometry = filterFactory->create();

    extractAttributeArraysFromGeometry->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    bool propWasSet = extractAttributeArraysFromGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, k_VerticesDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("SharedVertexListArrayPath1", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, k_ElementListDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("SharedEdgeListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run Filter

    extractAttributeArraysFromGeometry->execute();
    DREAM3D_REQUIRED(extractAttributeArraysFromGeometry->getErrorCondition(), >=, 0);

    // Check Filter Results

    QList<QString> vertexNames = vertexAM->getAttributeArrayNames();
    QList<QString> elementNames = elementAM->getAttributeArrayNames();
    DREAM3D_REQUIRE_EQUAL(vertexNames.contains(k_VerticesDAName), true)
    DREAM3D_REQUIRE_EQUAL(elementNames.contains(k_ElementListDAName), true)

    FloatArrayType::Pointer originalVertices = geom->getVertices();
    Int64ArrayType::Pointer originalConnectivity = geom->getEdges();

    FloatArrayType::Pointer newVertices = vertexAM->getAttributeArrayAs<FloatArrayType>(k_VerticesDAName);
    Int64ArrayType::Pointer newConnectivity = elementAM->getAttributeArrayAs<Int64ArrayType>(k_ElementListDAName);

    checkDataArray<float>(originalVertices, newVertices);
    checkDataArray<int64_t>(originalConnectivity, newConnectivity);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void ExtractAttributeArraysFromGeometryTriangleTest()
  {
    static const QString k_DataContainerName("DataContainer");
    static const QString k_VerticesDAName("VertexCoordinates");
    static const QString k_ElementListDAName("Connectivity");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrix

    AttributeMatrix::Pointer vertexAM = AttributeMatrix::New(m_Dims3, SIMPL::Defaults::VertexAttributeMatrixName, AttributeMatrix::Type::Vertex);
    dc->addAttributeMatrix(SIMPL::Defaults::VertexAttributeMatrixName, vertexAM);

    AttributeMatrix::Pointer elementAM = AttributeMatrix::New(m_Dims1, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Face);
    dc->addAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName, elementAM);

    // Create Geometry

    std::vector<std::vector<float>> vertices = {{1.0, 1.0, 0.0}, {3.0, 1.0, 0.0}, {2.0, 3.0, 0.0}};

    std::vector<std::vector<int64_t>> elements = {{0, 1, 2}};

    DataArray<float>::Pointer daVert = createDataArray<float>("Vertices", vertices, m_Dims3, m_Dims3);
    DataArray<int64_t>::Pointer daList = createDataArray<int64_t>("List", elements, m_Dims1, m_Dims3);

    TriangleGeom::Pointer geom = TriangleGeom::CreateGeometry(daList, daVert, SIMPL::Geometry::TriangleGeometry);

    dc->setGeometry(geom);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer extractAttributeArraysFromGeometry = filterFactory->create();

    extractAttributeArraysFromGeometry->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    bool propWasSet = extractAttributeArraysFromGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, k_VerticesDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("SharedVertexListArrayPath2", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, k_ElementListDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("SharedTriListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run Filter

    extractAttributeArraysFromGeometry->execute();
    DREAM3D_REQUIRED(extractAttributeArraysFromGeometry->getErrorCondition(), >=, 0);

    // Check Filter Results

    QList<QString> vertexNames = vertexAM->getAttributeArrayNames();
    QList<QString> elementNames = elementAM->getAttributeArrayNames();
    DREAM3D_REQUIRE_EQUAL(vertexNames.contains(k_VerticesDAName), true)
    DREAM3D_REQUIRE_EQUAL(elementNames.contains(k_ElementListDAName), true)

    FloatArrayType::Pointer originalVertices = geom->getVertices();
    Int64ArrayType::Pointer originalConnectivity = geom->getTriangles();

    FloatArrayType::Pointer newVertices = vertexAM->getAttributeArrayAs<FloatArrayType>(k_VerticesDAName);
    Int64ArrayType::Pointer newConnectivity = elementAM->getAttributeArrayAs<Int64ArrayType>(k_ElementListDAName);

    checkDataArray<float>(originalVertices, newVertices);
    checkDataArray<int64_t>(originalConnectivity, newConnectivity);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void ExtractAttributeArraysFromGeometryQuadTest()
  {
    static const QString k_DataContainerName("DataContainer");
    static const QString k_VerticesDAName("VertexCoordinates");
    static const QString k_ElementListDAName("Connectivity");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrix

    AttributeMatrix::Pointer vertexAM = AttributeMatrix::New(m_Dims4, SIMPL::Defaults::VertexAttributeMatrixName, AttributeMatrix::Type::Vertex);
    dc->addAttributeMatrix(SIMPL::Defaults::VertexAttributeMatrixName, vertexAM);

    AttributeMatrix::Pointer elementAM = AttributeMatrix::New(m_Dims1, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Face);
    dc->addAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName, elementAM);

    // Create Geometry

    std::vector<std::vector<float>> vertices = {{1.0, 1.0, 0.0}, {3.0, 1.0, 0.0}, {2.0, 3.0, 0.0}, {2.0, 2.0, 0.0}};

    std::vector<std::vector<int64_t>> elements = {{0, 1, 2, 3}};

    DataArray<float>::Pointer daVert = createDataArray<float>("Vertices", vertices, m_Dims4, m_Dims3);
    DataArray<int64_t>::Pointer daList = createDataArray<int64_t>("List", elements, m_Dims1, m_Dims4);

    QuadGeom::Pointer geom = QuadGeom::CreateGeometry(daList, daVert, SIMPL::Geometry::QuadGeometry);

    dc->setGeometry(geom);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer extractAttributeArraysFromGeometry = filterFactory->create();

    extractAttributeArraysFromGeometry->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    bool propWasSet = extractAttributeArraysFromGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, k_VerticesDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("SharedVertexListArrayPath3", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, k_ElementListDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("SharedQuadListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run Filter

    extractAttributeArraysFromGeometry->execute();
    DREAM3D_REQUIRED(extractAttributeArraysFromGeometry->getErrorCondition(), >=, 0);

    // Check Filter Results

    QList<QString> vertexNames = vertexAM->getAttributeArrayNames();
    QList<QString> elementNames = elementAM->getAttributeArrayNames();
    DREAM3D_REQUIRE_EQUAL(vertexNames.contains(k_VerticesDAName), true)
    DREAM3D_REQUIRE_EQUAL(elementNames.contains(k_ElementListDAName), true)

    FloatArrayType::Pointer originalVertices = geom->getVertices();
    Int64ArrayType::Pointer originalConnectivity = geom->getQuads();

    FloatArrayType::Pointer newVertices = vertexAM->getAttributeArrayAs<FloatArrayType>(k_VerticesDAName);
    Int64ArrayType::Pointer newConnectivity = elementAM->getAttributeArrayAs<Int64ArrayType>(k_ElementListDAName);

    checkDataArray<float>(originalVertices, newVertices);
    checkDataArray<int64_t>(originalConnectivity, newConnectivity);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void ExtractAttributeArraysFromGeometryTetrahedralTest()
  {
    static const QString k_DataContainerName("DataContainer");
    static const QString k_VerticesDAName("VertexCoordinates");
    static const QString k_ElementListDAName("Connectivity");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrix

    AttributeMatrix::Pointer vertexAM = AttributeMatrix::New(m_Dims4, SIMPL::Defaults::VertexAttributeMatrixName, AttributeMatrix::Type::Vertex);
    dc->addAttributeMatrix(SIMPL::Defaults::VertexAttributeMatrixName, vertexAM);

    AttributeMatrix::Pointer elementAM = AttributeMatrix::New(m_Dims1, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
    dc->addAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName, elementAM);

    // Create Geometry

    std::vector<std::vector<float>> vertices = {{1.0f, 1.0f, 1.55f}, {3.0f, 1.0f, 1.55f}, {2.0f, 3.0f, 1.55f}, {2.0f, 2.0f, 3.55f}};

    std::vector<std::vector<int64_t>> elements = {{0, 1, 2, 3}};

    DataArray<float>::Pointer daVert = createDataArray<float>("Vertices", vertices, m_Dims4, m_Dims3);
    DataArray<int64_t>::Pointer daList = createDataArray<int64_t>("List", elements, m_Dims1, m_Dims4);

    TetrahedralGeom::Pointer geom = TetrahedralGeom::CreateGeometry(daList, daVert, SIMPL::Geometry::TetrahedralGeometry);

    dc->setGeometry(geom);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer extractAttributeArraysFromGeometry = filterFactory->create();

    extractAttributeArraysFromGeometry->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    bool propWasSet = extractAttributeArraysFromGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, k_VerticesDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("SharedVertexListArrayPath4", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, k_ElementListDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("SharedTetListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run Filter

    extractAttributeArraysFromGeometry->execute();
    DREAM3D_REQUIRED(extractAttributeArraysFromGeometry->getErrorCondition(), >=, 0);

    // Check Filter Results

    QList<QString> vertexNames = vertexAM->getAttributeArrayNames();
    QList<QString> elementNames = elementAM->getAttributeArrayNames();
    DREAM3D_REQUIRE_EQUAL(vertexNames.contains(k_VerticesDAName), true)
    DREAM3D_REQUIRE_EQUAL(elementNames.contains(k_ElementListDAName), true)

    FloatArrayType::Pointer originalVertices = geom->getVertices();
    Int64ArrayType::Pointer originalConnectivity = geom->getTetrahedra();

    FloatArrayType::Pointer newVertices = vertexAM->getAttributeArrayAs<FloatArrayType>(k_VerticesDAName);
    Int64ArrayType::Pointer newConnectivity = elementAM->getAttributeArrayAs<Int64ArrayType>(k_ElementListDAName);

    checkDataArray<float>(originalVertices, newVertices);
    checkDataArray<int64_t>(originalConnectivity, newConnectivity);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void ExtractAttributeArraysFromGeometryHexahedralTest()
  {
    static const QString k_DataContainerName("DataContainer");
    static const QString k_VerticesDAName("VertexCoordinates");
    static const QString k_ElementListDAName("Connectivity");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrix

    AttributeMatrix::Pointer vertexAM = AttributeMatrix::New(m_Dims8, SIMPL::Defaults::VertexAttributeMatrixName, AttributeMatrix::Type::Vertex);
    dc->addAttributeMatrix(SIMPL::Defaults::VertexAttributeMatrixName, vertexAM);

    AttributeMatrix::Pointer elementAM = AttributeMatrix::New(m_Dims1, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
    dc->addAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName, elementAM);

    // Create Geometry

    std::vector<std::vector<float>> vertices = {{1.0f, 1.0f, 1.55f}, {3.0f, 1.0f, 1.55f}, {2.0f, 3.0f, 1.55f}, {2.0f, 2.0f, 3.55f}, {2.5f, 1.0f, 1.55f}, {4.3f, 1.0f, 1.55f}, {5.1f, 3.0f, 1.55f}, {7.63f, 2.0f, 3.55f}};

    std::vector<std::vector<int64_t>> elements = {{0, 1, 2, 3, 4, 5, 6, 7}};

    DataArray<float>::Pointer daVert = createDataArray<float>("Vertices", vertices, m_Dims8, m_Dims3);
    DataArray<int64_t>::Pointer daList = createDataArray<int64_t>("List", elements, m_Dims1, m_Dims8);

    HexahedralGeom::Pointer geom = HexahedralGeom::CreateGeometry(daList, daVert, SIMPL::Geometry::HexahedralGeometry);

    dc->setGeometry(geom);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer extractAttributeArraysFromGeometry = filterFactory->create();

    extractAttributeArraysFromGeometry->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    bool propWasSet = extractAttributeArraysFromGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, k_VerticesDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("SharedVertexListArrayPath5", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, k_ElementListDAName);
    var.setValue(dap);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("SharedHexListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run Filter

    extractAttributeArraysFromGeometry->execute();
    DREAM3D_REQUIRED(extractAttributeArraysFromGeometry->getErrorCondition(), >=, 0);

    // Check Filter Results

    QList<QString> vertexNames = vertexAM->getAttributeArrayNames();
    QList<QString> elementNames = elementAM->getAttributeArrayNames();
    DREAM3D_REQUIRE_EQUAL(vertexNames.contains(k_VerticesDAName), true)
    DREAM3D_REQUIRE_EQUAL(elementNames.contains(k_ElementListDAName), true)

    FloatArrayType::Pointer originalVertices = geom->getVertices();
    Int64ArrayType::Pointer originalConnectivity = geom->getHexahedra();

    FloatArrayType::Pointer newVertices = vertexAM->getAttributeArrayAs<FloatArrayType>(k_VerticesDAName);
    Int64ArrayType::Pointer newConnectivity = elementAM->getAttributeArrayAs<Int64ArrayType>(k_ElementListDAName);

    checkDataArray<float>(originalVertices, newVertices);
    checkDataArray<int64_t>(originalConnectivity, newConnectivity);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#### ExtractAttributeArraysFromGeometryTest Starting ####" << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(ExtractAttributeArraysFromGeometryRectGridTest())
    DREAM3D_REGISTER_TEST(ExtractAttributeArraysFromGeometryVertexTest())
    DREAM3D_REGISTER_TEST(ExtractAttributeArraysFromGeometryEdgeTest())
    DREAM3D_REGISTER_TEST(ExtractAttributeArraysFromGeometryTriangleTest())
    DREAM3D_REGISTER_TEST(ExtractAttributeArraysFromGeometryQuadTest())
    DREAM3D_REGISTER_TEST(ExtractAttributeArraysFromGeometryTetrahedralTest())
    DREAM3D_REGISTER_TEST(ExtractAttributeArraysFromGeometryHexahedralTest())
  }

private:
  QString m_FilterName = QString("ExtractAttributeArraysFromGeometry");
  QVector<size_t> m_Dims1 = {1};
  QVector<size_t> m_Dims2 = {2};
  QVector<size_t> m_Dims3 = {3};
  QVector<size_t> m_Dims4 = {4};
  QVector<size_t> m_Dims8 = {8};
};
