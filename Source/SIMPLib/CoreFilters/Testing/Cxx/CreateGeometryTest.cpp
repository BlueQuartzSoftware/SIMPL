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

#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"

#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/HexahedralGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "SIMPLib/Common/Constants.h"

class CreateGeometryTest
{

public:
  CreateGeometryTest() = default;
  ~CreateGeometryTest() = default;
  CreateGeometryTest(const CreateGeometryTest&) = delete;            // Copy Constructor
  CreateGeometryTest(CreateGeometryTest&&) = delete;                 // Move Constructor
  CreateGeometryTest& operator=(const CreateGeometryTest&) = delete; // Copy Assignment
  CreateGeometryTest& operator=(CreateGeometryTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the CreateGeometryTest Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The CreateGeometryTest Requires the use of the " << m_FilterName.toStdString() << " filter which is found in Core Filters";
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
  void removeGeometry(DataContainer::Pointer dc)
  {
    dc->setGeometry(IGeometry::NullPointer());

    QList<QString> attributeMatrices = dc->getAttributeMatrixNames();

    if(attributeMatrices.contains(SIMPL::Defaults::CellAttributeMatrixName))
    {
      dc->removeAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName);
    }
    if(attributeMatrices.contains(SIMPL::Defaults::EdgeAttributeMatrixName))
    {
      dc->removeAttributeMatrix(SIMPL::Defaults::EdgeAttributeMatrixName);
    }
    if(attributeMatrices.contains(SIMPL::Defaults::FaceAttributeMatrixName))
    {
      dc->removeAttributeMatrix(SIMPL::Defaults::FaceAttributeMatrixName);
    }
    if(attributeMatrices.contains(SIMPL::Defaults::VertexAttributeMatrixName))
    {
      dc->removeAttributeMatrix(SIMPL::Defaults::VertexAttributeMatrixName);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testCaseImage(AbstractFilter::Pointer createGeometry, DataContainer::Pointer dc, IGeometry::Type geomType, IntVec3_t numDimensions, FloatVec3_t originPos, FloatVec3_t imgResolution)
  {
    if(geomType != IGeometry::Type::Image)
    {
      return;
    }

    QVariant var;

    var.setValue(0);
    bool propWasSet = createGeometry->setProperty("GeometryType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createGeometry->execute();
    DREAM3D_REQUIRED(createGeometry->getErrorCondition(), >=, 0);

    bool correctGeom = (geomType == dc->getGeometry()->getGeometryType());
    DREAM3D_REQUIRE_EQUAL(correctGeom, true)

    ImageGeom::Pointer imageGeom = dc->getGeometryAs<ImageGeom>();

    SIMPL::Tuple3SVec dim = imageGeom->getDimensions();
    SIMPL::Tuple3FVec origin = imageGeom->getOrigin();
    SIMPL::Tuple3FVec res = imageGeom->getResolution();

    DREAM3D_REQUIRE_EQUAL(std::get<0>(dim), numDimensions.x)
    DREAM3D_REQUIRE_EQUAL(std::get<1>(dim), numDimensions.y)
    DREAM3D_REQUIRE_EQUAL(std::get<2>(dim), numDimensions.z)

    DREAM3D_REQUIRE_EQUAL(std::get<0>(origin), originPos.x)
    DREAM3D_REQUIRE_EQUAL(std::get<1>(origin), originPos.y)
    DREAM3D_REQUIRE_EQUAL(std::get<2>(origin), originPos.z)

    DREAM3D_REQUIRE_EQUAL(std::get<0>(res), imgResolution.x)
    DREAM3D_REQUIRE_EQUAL(std::get<1>(res), imgResolution.y)
    DREAM3D_REQUIRE_EQUAL(std::get<2>(res), imgResolution.z)

    removeGeometry(dc);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testCaseRectGrid(AbstractFilter::Pointer createGeometry, DataContainer::Pointer dc, AttributeMatrix::Pointer am, IGeometry::Type geomType, FloatArrayType::Pointer xBounds,
                        FloatArrayType::Pointer yBounds, FloatArrayType::Pointer zBounds, bool treatWarningsAsErrors, bool arrayHandling)
  {
    if(geomType != IGeometry::Type::RectGrid)
    {
      return;
    }

    QVariant var;

    var.setValue(1);
    bool propWasSet = createGeometry->setProperty("GeometryType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(treatWarningsAsErrors);
    propWasSet = createGeometry->setProperty("TreatWarningsAsErrors", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(arrayHandling);
    propWasSet = createGeometry->setProperty("ArrayHandling", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createGeometry->execute();

    if(treatWarningsAsErrors)
    {
      DREAM3D_REQUIRED(createGeometry->getErrorCondition(), ==, -1)
    }
    else
    {
      DREAM3D_REQUIRED(createGeometry->getErrorCondition(), >=, 0);

      bool correctGeom = (geomType == dc->getGeometry()->getGeometryType());
      DREAM3D_REQUIRE_EQUAL(correctGeom, true)

      RectGridGeom::Pointer rectGridGeom = dc->getGeometryAs<RectGridGeom>();
      FloatArrayType::Pointer x = rectGridGeom->getXBounds();
      FloatArrayType::Pointer y = rectGridGeom->getYBounds();
      FloatArrayType::Pointer z = rectGridGeom->getZBounds();

      checkDataArray<float>(x, xBounds);
      checkDataArray<float>(y, yBounds);
      checkDataArray<float>(z, zBounds);

      if(arrayHandling)
      {
        QList<QString> names = am->getAttributeArrayNames();

        DREAM3D_REQUIRE_EQUAL(names.contains(xBounds->getName()), false)
        DREAM3D_REQUIRE_EQUAL(names.contains(yBounds->getName()), false)
        DREAM3D_REQUIRE_EQUAL(names.contains(zBounds->getName()), false)
      }
    }
    removeGeometry(dc);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testCase(AbstractFilter::Pointer createGeometry, DataContainer::Pointer dc, AttributeMatrix::Pointer am, IGeometry::Type geomType, FloatArrayType::Pointer daVertices,
                Int64ArrayType::Pointer daElements, bool treatWarningsAsErrors, bool arrayHandling)
  {
    int geomChoice = 2;

    if(geomType == IGeometry::Type::Image)
    {
      return;
    }
    else if(geomType == IGeometry::Type::RectGrid)
    {
      return;
    }
    else if(geomType == IGeometry::Type::Vertex)
    {
      geomChoice = 2;
    }
    else if(geomType == IGeometry::Type::Edge)
    {
      geomChoice = 3;
    }
    else if(geomType == IGeometry::Type::Triangle)
    {
      geomChoice = 4;
    }
    else if(geomType == IGeometry::Type::Quad)
    {
      geomChoice = 5;
    }
    else if(geomType == IGeometry::Type::Tetrahedral)
    {
      geomChoice = 6;
    }
    else if(geomType == IGeometry::Type::Hexahedral)
    {
      geomChoice = 7;
    }

    QVariant var;

    var.setValue(geomChoice);
    bool propWasSet = createGeometry->setProperty("GeometryType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(treatWarningsAsErrors);
    propWasSet = createGeometry->setProperty("TreatWarningsAsErrors", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(arrayHandling);
    propWasSet = createGeometry->setProperty("ArrayHandling", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createGeometry->execute();

    // Vertex Geometry is not affected by the treatWarningsAsErrors option

    if(treatWarningsAsErrors && geomType != IGeometry::Type::Vertex)
    {
      DREAM3D_REQUIRED(createGeometry->getErrorCondition(), ==, -1)
    }
    else
    {
      DREAM3D_REQUIRED(createGeometry->getErrorCondition(), >=, 0);

      bool correctGeom = (geomType == dc->getGeometry()->getGeometryType());

      DREAM3D_REQUIRE_EQUAL(correctGeom, true)

      FloatArrayType::Pointer vert = FloatArrayType::NullPointer();
      Int64ArrayType::Pointer elements = Int64ArrayType::NullPointer();

      if(geomType == IGeometry::Type::Vertex)
      {
        VertexGeom::Pointer vertexGeom = dc->getGeometryAs<VertexGeom>();
        FloatArrayType::Pointer vert = vertexGeom->getVertices();

        checkDataArray<float>(vert, daVertices);
      }
      else if(geomType == IGeometry::Type::Edge)
      {
        EdgeGeom::Pointer edgeGeom = dc->getGeometryAs<EdgeGeom>();
        FloatArrayType::Pointer vert = edgeGeom->getVertices();
        Int64ArrayType::Pointer edges = edgeGeom->getEdges();

        checkDataArray<float>(vert, daVertices);
        checkDataArray<int64_t>(edges, daElements);
      }
      else if(geomType == IGeometry::Type::Triangle)
      {
        TriangleGeom::Pointer triGeom = dc->getGeometryAs<TriangleGeom>();
        FloatArrayType::Pointer vert = triGeom->getVertices();
        Int64ArrayType::Pointer triangles = triGeom->getTriangles();

        checkDataArray<float>(vert, daVertices);
        checkDataArray<int64_t>(triangles, daElements);
      }
      else if(geomType == IGeometry::Type::Quad)
      {
        QuadGeom::Pointer quadGeom = dc->getGeometryAs<QuadGeom>();
        FloatArrayType::Pointer vert = quadGeom->getVertices();
        Int64ArrayType::Pointer quads = quadGeom->getQuads();

        checkDataArray<float>(vert, daVertices);
        checkDataArray<int64_t>(quads, daElements);
      }
      else if(geomType == IGeometry::Type::Tetrahedral)
      {
        TetrahedralGeom::Pointer tetraGeom = dc->getGeometryAs<TetrahedralGeom>();
        FloatArrayType::Pointer vert = tetraGeom->getVertices();
        Int64ArrayType::Pointer tetrahedra = tetraGeom->getTetrahedra();

        checkDataArray<float>(vert, daVertices);
        checkDataArray<int64_t>(tetrahedra, daElements);
      }
      else if(geomType == IGeometry::Type::Hexahedral)
      {
        HexahedralGeom::Pointer hexaGeom = dc->getGeometryAs<HexahedralGeom>();
        FloatArrayType::Pointer vert = hexaGeom->getVertices();
        Int64ArrayType::Pointer hexahedra = hexaGeom->getHexahedra();

        checkDataArray<float>(vert, daVertices);
        checkDataArray<int64_t>(hexahedra, daElements);
      }

      if(arrayHandling)
      {
        QList<QString> names = am->getAttributeArrayNames();

        DREAM3D_REQUIRE_EQUAL(names.contains(daVertices->getName()), false)
        if(geomType != IGeometry::Type::Vertex)
        {
          DREAM3D_REQUIRE_EQUAL(names.contains(daElements->getName()), false)
        }
      }
    }

    removeGeometry(dc);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  AbstractFilter::Pointer createFilter()
  {
    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    return filterFactory->create();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CreateGeometryImageTest()
  {
    // Names and Constants

    static const QString k_DataContainerName("DataContainer");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // ImageGeom Parameters

    IntVec3_t numDimensions;
    numDimensions.x = 5;
    numDimensions.y = 15;
    numDimensions.z = 25;

    FloatVec3_t originPos;
    originPos.x = 5;
    originPos.y = 15;
    originPos.z = 25;

    FloatVec3_t imgResolution;
    imgResolution.x = 5;
    imgResolution.y = 15;
    imgResolution.z = 25;

    // Create Filter

    AbstractFilter::Pointer createGeometry = createFilter();

    // Set up filter

    QVariant var;

    var.setValue(0);
    bool propWasSet = createGeometry->setProperty("GeometryType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("TreatWarningsAsErrors", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("ArrayHandling", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    propWasSet = createGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::CellAttributeMatrixName);
    propWasSet = createGeometry->setProperty("ImageCellAttributeMatrixName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::VertexAttributeMatrixName);

    var.setValue(numDimensions);
    propWasSet = createGeometry->setProperty("Dimensions", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(originPos);
    propWasSet = createGeometry->setProperty("Origin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(imgResolution);
    propWasSet = createGeometry->setProperty("Resolution", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createGeometry->setDataContainerArray(dca);

    // Image test cases

    testCaseImage(createGeometry, dc, IGeometry::Type::Image, numDimensions, originPos, imgResolution);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CreateGeometryRectGridTest()
  {
    // Names and Constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_BoundsMatrixName("BoundsMatrix");
    static const QString k_XBoundsDAName("XBounds");
    static const QString k_YBoundsDAName("YBounds");
    static const QString k_ZBoundsDAName("ZBounds");
    static const QString k_BadXBoundsDAName("BadXBounds");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrices

    AttributeMatrix::Pointer boundsAM = AttributeMatrix::New(m_Dims4, k_BoundsMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_BoundsMatrixName, boundsAM);

    // Create Bounds Arrays

    DataArray<float>::Pointer daXBounds = DataArray<float>::CreateArray(m_Dims4, m_Dims1, k_XBoundsDAName);
    for(size_t i = 0; i < daXBounds->getSize(); i++)
    {
      daXBounds->setValue(i, i);
    }
    boundsAM->addAttributeArray(k_XBoundsDAName, daXBounds);

    DataArray<float>::Pointer daYBounds = DataArray<float>::CreateArray(m_Dims4, m_Dims1, k_YBoundsDAName);
    for(size_t i = 0; i < daYBounds->getSize(); i++)
    {
      daYBounds->setValue(i, i);
    }
    boundsAM->addAttributeArray(k_YBoundsDAName, daYBounds);

    DataArray<float>::Pointer daZBounds = DataArray<float>::CreateArray(m_Dims4, m_Dims1, k_ZBoundsDAName);
    for(size_t i = 0; i < daZBounds->getSize(); i++)
    {
      daZBounds->setValue(i, i);
    }
    boundsAM->addAttributeArray(k_ZBoundsDAName, daZBounds);

    // Create bad xBounds arrays

    DataArray<float>::Pointer daBadXBounds = DataArray<float>::CreateArray(m_Dims4, m_Dims1, k_BadXBoundsDAName);
    for(size_t i = 0; i < daBadXBounds->getSize(); i++)
    {
      daBadXBounds->setValue(i, i);
    }
    daBadXBounds->setValue(0, 100);
    boundsAM->addAttributeArray(k_BadXBoundsDAName, daBadXBounds);

    // Create Filter

    AbstractFilter::Pointer createGeometry = createFilter();

    // Set up filter

    QVariant var;

    var.setValue(1);
    bool propWasSet = createGeometry->setProperty("GeometryType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("TreatWarningsAsErrors", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("ArrayHandling", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    propWasSet = createGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::CellAttributeMatrixName);
    propWasSet = createGeometry->setProperty("RectGridCellAttributeMatrixName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_BoundsMatrixName, k_XBoundsDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("XBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_BoundsMatrixName, k_YBoundsDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("YBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_BoundsMatrixName, k_ZBoundsDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("ZBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createGeometry->setDataContainerArray(dca);

    // RectGrid test cases

    testCaseRectGrid(createGeometry, dc, boundsAM, IGeometry::Type::RectGrid, daXBounds, daYBounds, daZBounds, false, false);

    // Give bad input x bounds array and check that it gives the appropriate error code when treatWarningsAsErrors == true

    dap = DataArrayPath(k_DataContainerName, k_BoundsMatrixName, k_BadXBoundsDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("XBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCaseRectGrid(createGeometry, dc, boundsAM, IGeometry::Type::RectGrid, daBadXBounds, daYBounds, daZBounds, true, false);

    // Give bad input y bounds array and check that it gives the appropriate error code when treatWarningsAsErrors == true

    dap = DataArrayPath(k_DataContainerName, k_BoundsMatrixName, k_XBoundsDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("XBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_BoundsMatrixName, k_BadXBoundsDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("YBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCaseRectGrid(createGeometry, dc, boundsAM, IGeometry::Type::RectGrid, daXBounds, daBadXBounds, daZBounds, true, false);

    // Give bad input z bounds array and check that it gives the appropriate error code when treatWarningsAsErrors == true

    dap = DataArrayPath(k_DataContainerName, k_BoundsMatrixName, k_YBoundsDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("YBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_BoundsMatrixName, k_BadXBoundsDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("ZBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCaseRectGrid(createGeometry, dc, boundsAM, IGeometry::Type::RectGrid, daXBounds, daYBounds, daBadXBounds, true, false);

    // Test Rectgrid with moving arrays from attribute matrix

    dap = DataArrayPath(k_DataContainerName, k_BoundsMatrixName, k_ZBoundsDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("ZBoundsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCaseRectGrid(createGeometry, dc, boundsAM, IGeometry::Type::RectGrid, daXBounds, daYBounds, daZBounds, false, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CreateGeometryVertexTest()
  {
    // Names and Constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_VertexMatrixName("VertexMatrix");
    static const QString k_VertexListDAName("VertexList");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrices

    AttributeMatrix::Pointer vertexAM = AttributeMatrix::New(m_Dims2, k_VertexMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_VertexMatrixName, vertexAM);

    // Create Vertex Data Arrays

    std::vector<std::vector<float>> vertices = {{1.0, 1.0, 0.0}, {3.0, 1.0, 0.0}};

    DataArray<float>::Pointer daVert = createDataArray<float>(k_VertexListDAName, vertices, m_Dims2, m_Dims3);
    vertexAM->addAttributeArray(k_VertexListDAName, daVert);

    // Create Filter

    AbstractFilter::Pointer createGeometry = createFilter();

    // Set up filter

    QVariant var;

    var.setValue(2);
    bool propWasSet = createGeometry->setProperty("GeometryType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("TreatWarningsAsErrors", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("ArrayHandling", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    propWasSet = createGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_VertexMatrixName, k_VertexListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedVertexListArrayPath0", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::VertexAttributeMatrixName);
    propWasSet = createGeometry->setProperty("VertexAttributeMatrixName0", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createGeometry->setDataContainerArray(dca);

    // Vertex test cases

    testCase(createGeometry, dc, vertexAM, IGeometry::Type::Vertex, daVert, Int64ArrayType::NullPointer(), false, false);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CreateGeometryEdgeTest()
  {
    // Names and Constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_EdgeVertexMatrixName("EdgeVertexMatrix");
    static const QString k_EdgeElementAttributeMatrixName("EdgeElementMatrix");
    static const QString k_EdgeVertexListDAName("EdgeVertexList");
    static const QString k_EdgeListDAName("EdgeList");
    static const QString k_BadEdgeListDAName("BadEdgeList");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrices

    AttributeMatrix::Pointer edgeVertexAM = AttributeMatrix::New(m_Dims2, k_EdgeVertexMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_EdgeVertexMatrixName, edgeVertexAM);

    AttributeMatrix::Pointer edgeElementAM = AttributeMatrix::New(m_Dims1, k_EdgeElementAttributeMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_EdgeElementAttributeMatrixName, edgeElementAM);

    // Create Edge Data Arrays

    std::vector<std::vector<float>> vertices = {{1.0, 1.0, 0.0}, {3.0, 1.0, 0.0}};
    std::vector<std::vector<int64_t>> elements = {{0, 1}};

    DataArray<float>::Pointer daEdgeVert = createDataArray<float>(k_EdgeVertexListDAName, vertices, m_Dims2, m_Dims3);
    DataArray<int64_t>::Pointer daEdgeList = createDataArray<int64_t>(k_EdgeListDAName, elements, m_Dims1, m_Dims2);
    edgeVertexAM->addAttributeArray(k_EdgeVertexListDAName, daEdgeVert);
    edgeElementAM->addAttributeArray(k_EdgeListDAName, daEdgeList);

    // Create Bad EdgeList

    elements = {{0, 2}};

    DataArray<int64_t>::Pointer daBadEdgeList = createDataArray<int64_t>(k_BadEdgeListDAName, elements, m_Dims1, m_Dims2);
    edgeElementAM->addAttributeArray(k_BadEdgeListDAName, daBadEdgeList);

    // Create Filter

    AbstractFilter::Pointer createGeometry = createFilter();

    // Set up filter

    QVariant var;

    var.setValue(3);
    bool propWasSet = createGeometry->setProperty("GeometryType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("TreatWarningsAsErrors", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("ArrayHandling", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    propWasSet = createGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_EdgeVertexMatrixName, k_EdgeVertexListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedVertexListArrayPath1", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_EdgeElementAttributeMatrixName, k_EdgeListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedEdgeListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::EdgeAttributeMatrixName);
    propWasSet = createGeometry->setProperty("EdgeAttributeMatrixName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::VertexAttributeMatrixName);
    propWasSet = createGeometry->setProperty("VertexAttributeMatrixName1", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createGeometry->setDataContainerArray(dca);

    // Edge test cases

    testCase(createGeometry, dc, edgeElementAM, IGeometry::Type::Edge, daEdgeVert, daEdgeList, false, false);

    // Bad EdgeList test with treatErrorsAsWarnings == true

    dap = DataArrayPath(k_DataContainerName, k_EdgeElementAttributeMatrixName, k_BadEdgeListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedEdgeListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCase(createGeometry, dc, edgeElementAM, IGeometry::Type::Edge, daEdgeVert, daBadEdgeList, true, false);

    // Test Edge with moving arrays from attribute matrix

    dap = DataArrayPath(k_DataContainerName, k_EdgeElementAttributeMatrixName, k_EdgeListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedEdgeListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCase(createGeometry, dc, edgeElementAM, IGeometry::Type::Edge, daEdgeVert, daEdgeList, false, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CreateGeometryTriangleTest()
  {
    // Names and Constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_TriVertexMatrixName("TriVertexMatrix");
    static const QString k_TriElementAttributeMatrixName("TriElementMatrix");
    static const QString k_TriVertexListDAName("TriVertexList");
    static const QString k_TriListDAName("TriangleList");
    static const QString k_BadTriListDAName("BadTriangleList");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrices

    AttributeMatrix::Pointer triVertexAM = AttributeMatrix::New(m_Dims3, k_TriVertexMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_TriVertexMatrixName, triVertexAM);

    AttributeMatrix::Pointer triElementAM = AttributeMatrix::New(m_Dims1, k_TriElementAttributeMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_TriElementAttributeMatrixName, triElementAM);

    // Create Triangle Data Arrays

    std::vector<std::vector<float>> vertices = {{1.0, 1.0, 0.0}, {3.0, 1.0, 0.0}, {2.0, 3.0, 0.0}};
    std::vector<std::vector<int64_t>> elements = {{0, 1, 2}};

    DataArray<float>::Pointer daTriVert = createDataArray<float>(k_TriVertexListDAName, vertices, m_Dims3, m_Dims3);
    DataArray<int64_t>::Pointer daTriList = createDataArray<int64_t>(k_TriListDAName, elements, m_Dims1, m_Dims3);
    triVertexAM->addAttributeArray(k_TriVertexListDAName, daTriVert);
    triElementAM->addAttributeArray(k_TriListDAName, daTriList);

    // Create Bad TriangleList

    elements = {{0, 3, 2}};

    DataArray<int64_t>::Pointer daBadTriList = createDataArray<int64_t>(k_BadTriListDAName, elements, m_Dims1, m_Dims3);
    triElementAM->addAttributeArray(k_BadTriListDAName, daBadTriList);

    // Create Filter

    AbstractFilter::Pointer createGeometry = createFilter();

    // Set up filter

    QVariant var;

    var.setValue(4);
    bool propWasSet = createGeometry->setProperty("GeometryType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("TreatWarningsAsErrors", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("ArrayHandling", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    propWasSet = createGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_TriVertexMatrixName, k_TriVertexListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedVertexListArrayPath2", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_TriElementAttributeMatrixName, k_TriListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedTriListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::FaceAttributeMatrixName);
    propWasSet = createGeometry->setProperty("FaceAttributeMatrixName0", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::VertexAttributeMatrixName);
    propWasSet = createGeometry->setProperty("VertexAttributeMatrixName2", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createGeometry->setDataContainerArray(dca);

    // Triangle test cases

    testCase(createGeometry, dc, triElementAM, IGeometry::Type::Triangle, daTriVert, daTriList, false, false);

    // Bad TriangleList test with treatErrorsAsWarnings == true

    dap = DataArrayPath(k_DataContainerName, k_TriElementAttributeMatrixName, k_BadTriListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedTriListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCase(createGeometry, dc, triElementAM, IGeometry::Type::Triangle, daTriVert, daBadTriList, true, false);

    // Test Triangle with moving arrays from attribute matrix

    dap = DataArrayPath(k_DataContainerName, k_TriElementAttributeMatrixName, k_TriListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedTriListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCase(createGeometry, dc, triElementAM, IGeometry::Type::Triangle, daTriVert, daTriList, false, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CreateGeometryQuadTest()
  {
    // Names and Constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_QuadVertexMatrixName("QuadVertexMatrix");
    static const QString k_QuadElementAttributeMatrixName("QuadElementMatrix");
    static const QString k_QuadVertexListDAName("QuadVertexList");
    static const QString k_QuadListDAName("QuadrilateralList");
    static const QString k_BadQuadListDAName("BadQuadrilateralList");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrices

    AttributeMatrix::Pointer quadVertexAM = AttributeMatrix::New(m_Dims4, k_QuadVertexMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_QuadVertexMatrixName, quadVertexAM);

    AttributeMatrix::Pointer quadElementAM = AttributeMatrix::New(m_Dims1, k_QuadElementAttributeMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_QuadElementAttributeMatrixName, quadElementAM);

    // Create Quadrilateral Data Arrays

    std::vector<std::vector<float>> vertices = {{1.0, 1.0, 0.0}, {3.0, 1.0, 0.0}, {2.0, 3.0, 0.0}, {2.0, 2.0, 0.0}};
    std::vector<std::vector<int64_t>> elements = {{0, 1, 2, 3}};

    DataArray<float>::Pointer daQuadVert = createDataArray<float>(k_QuadVertexListDAName, vertices, m_Dims4, m_Dims3);
    DataArray<int64_t>::Pointer daQuadList = createDataArray<int64_t>(k_QuadListDAName, elements, m_Dims1, m_Dims4);
    quadVertexAM->addAttributeArray(k_QuadVertexListDAName, daQuadVert);
    quadElementAM->addAttributeArray(k_QuadListDAName, daQuadList);

    // Create Bad QuadrilateralList

    elements = {{0, 1, 7, 3}};

    DataArray<int64_t>::Pointer daBadQuadList = createDataArray<int64_t>(k_BadQuadListDAName, elements, m_Dims1, m_Dims4);
    quadElementAM->addAttributeArray(k_BadQuadListDAName, daBadQuadList);

    // Create Filter

    AbstractFilter::Pointer createGeometry = createFilter();

    // Set up filter

    QVariant var;

    var.setValue(5);
    bool propWasSet = createGeometry->setProperty("GeometryType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("TreatWarningsAsErrors", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("ArrayHandling", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    propWasSet = createGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_QuadVertexMatrixName, k_QuadVertexListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedVertexListArrayPath3", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_QuadElementAttributeMatrixName, k_QuadListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedQuadListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::FaceAttributeMatrixName);
    propWasSet = createGeometry->setProperty("FaceAttributeMatrixName1", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::VertexAttributeMatrixName);
    propWasSet = createGeometry->setProperty("VertexAttributeMatrixName3", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createGeometry->setDataContainerArray(dca);

    // Quad test cases

    testCase(createGeometry, dc, quadElementAM, IGeometry::Type::Quad, daQuadVert, daQuadList, false, false);

    // Bad QuadList test with treatErrorsAsWarnings == true

    dap = DataArrayPath(k_DataContainerName, k_QuadElementAttributeMatrixName, k_BadQuadListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedQuadListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCase(createGeometry, dc, quadElementAM, IGeometry::Type::Quad, daQuadVert, daBadQuadList, true, false);

    // Test Quad with moving arrays from attribute matrix

    dap = DataArrayPath(k_DataContainerName, k_QuadElementAttributeMatrixName, k_QuadListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedQuadListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCase(createGeometry, dc, quadElementAM, IGeometry::Type::Quad, daQuadVert, daQuadList, false, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CreateGeometryTetrahedralTest()
  {
    // Names and Constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_TetVertexMatrixName("TetVertexMatrix");
    static const QString k_TetElementAttributeMatrixName("TetElementMatrix");
    static const QString k_TetVertexListDAName("TetVertexList");
    static const QString k_TetListDAName("TetrahedraList");
    static const QString k_BadTetListDAName("BadTetrahedraList");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrices

    AttributeMatrix::Pointer tetVertexAM = AttributeMatrix::New(m_Dims4, k_TetVertexMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_TetVertexMatrixName, tetVertexAM);

    AttributeMatrix::Pointer tetElementAM = AttributeMatrix::New(m_Dims1, k_TetElementAttributeMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_TetElementAttributeMatrixName, tetElementAM);

    // Create Tetrahedron Data Arrays

    std::vector<std::vector<float>> vertices = {{1.0, 1.0, 1.55}, {3.0, 1.0, 1.55}, {2.0, 3.0, 1.55}, {2.0, 2.0, 3.55}};

    std::vector<std::vector<int64_t>> elements = {{0, 1, 2, 3}};

    DataArray<float>::Pointer daTetVert = createDataArray<float>(k_TetVertexListDAName, vertices, m_Dims4, m_Dims3);
    DataArray<int64_t>::Pointer daTetList = createDataArray<int64_t>(k_TetListDAName, elements, m_Dims1, m_Dims4);
    tetVertexAM->addAttributeArray(k_TetVertexListDAName, daTetVert);
    tetElementAM->addAttributeArray(k_TetListDAName, daTetList);

    // Create Bad TetrahedronList

    elements = {{0, 10, 2, 3}};

    DataArray<int64_t>::Pointer daBadTetList = createDataArray<int64_t>(k_BadTetListDAName, elements, m_Dims1, m_Dims4);
    tetElementAM->addAttributeArray(k_BadTetListDAName, daBadTetList);

    // Create Filter

    AbstractFilter::Pointer createGeometry = createFilter();

    // Set up filter

    QVariant var;

    var.setValue(6);
    bool propWasSet = createGeometry->setProperty("GeometryType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("TreatWarningsAsErrors", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("ArrayHandling", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    propWasSet = createGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_TetVertexMatrixName, k_TetVertexListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedVertexListArrayPath4", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_TetElementAttributeMatrixName, k_TetListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedTetListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::CellAttributeMatrixName);
    propWasSet = createGeometry->setProperty("TetCellAttributeMatrixName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::VertexAttributeMatrixName);
    propWasSet = createGeometry->setProperty("VertexAttributeMatrixName4", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createGeometry->setDataContainerArray(dca);

    // Tetrahedral test cases

    testCase(createGeometry, dc, tetElementAM, IGeometry::Type::Tetrahedral, daTetVert, daTetList, false, false);

    // Bad TetList test with treatErrorsAsWarnings == true

    dap = DataArrayPath(k_DataContainerName, k_TetElementAttributeMatrixName, k_BadTetListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedTetListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCase(createGeometry, dc, tetElementAM, IGeometry::Type::Tetrahedral, daTetVert, daBadTetList, true, false);

    // Test Tetrahedral with moving arrays from attribute matrix

    dap = DataArrayPath(k_DataContainerName, k_TetElementAttributeMatrixName, k_TetListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedTetListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCase(createGeometry, dc, tetElementAM, IGeometry::Type::Tetrahedral, daTetVert, daTetList, false, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CreateGeometryHexahedralTest()
  {
    // Names and Constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_HexVertexMatrixName("HexVertexMatrix");
    static const QString k_HexElementAttributeMatrixName("HexElementMatrix");
    static const QString k_HexVertexListDAName("HexVertexList");
    static const QString k_HexListDAName("HexahedraList");
    static const QString k_BadHexListDAName("BadHexahedraList");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrices

    AttributeMatrix::Pointer hexVertexAM = AttributeMatrix::New(m_Dims8, k_HexVertexMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_HexVertexMatrixName, hexVertexAM);

    AttributeMatrix::Pointer hexElementAM = AttributeMatrix::New(m_Dims1, k_HexElementAttributeMatrixName, AttributeMatrix::Type::Any);
    dc->addAttributeMatrix(k_HexElementAttributeMatrixName, hexElementAM);

    // Create Hexahedron Data Arrays

    std::vector<std::vector<float>> vertices = {{1.0, 1.0, 1.55}, {3.0, 1.0, 1.55}, {2.0, 3.0, 1.55}, {2.0, 2.0, 3.55}, {2.5, 1.0, 1.55}, {4.3, 1.0, 1.55}, {5.1, 3.0, 1.55}, {7.63, 2.0, 3.55}};

    std::vector<std::vector<int64_t>> elements = {{0, 1, 2, 3, 4, 5, 6, 7}};

    DataArray<float>::Pointer daHexVert = createDataArray<float>(k_HexVertexListDAName, vertices, m_Dims8, m_Dims3);
    DataArray<int64_t>::Pointer daHexList = createDataArray<int64_t>(k_HexListDAName, elements, m_Dims1, m_Dims8);
    hexVertexAM->addAttributeArray(k_HexVertexListDAName, daHexVert);
    hexElementAM->addAttributeArray(k_HexListDAName, daHexList);

    // Create Bad HexahedronList

    elements = {{0, 1, 2, 32, 4, 5, 6, 7}};

    DataArray<int64_t>::Pointer daBadHexList = createDataArray<int64_t>(k_BadHexListDAName, elements, m_Dims1, m_Dims8);
    hexElementAM->addAttributeArray(k_BadHexListDAName, daBadHexList);

    // Create Filter

    AbstractFilter::Pointer createGeometry = createFilter();

    // Set up filter

    QVariant var;

    var.setValue(7);
    bool propWasSet = createGeometry->setProperty("GeometryType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("TreatWarningsAsErrors", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = createGeometry->setProperty("ArrayHandling", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    DataArrayPath dap(k_DataContainerName);
    var.setValue(k_DataContainerName);
    propWasSet = createGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_HexVertexMatrixName, k_HexVertexListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedVertexListArrayPath5", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_HexElementAttributeMatrixName, k_HexListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedHexListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::CellAttributeMatrixName);
    propWasSet = createGeometry->setProperty("HexCellAttributeMatrixName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::Defaults::VertexAttributeMatrixName);
    propWasSet = createGeometry->setProperty("VertexAttributeMatrixName5", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    createGeometry->setDataContainerArray(dca);

    // Hexahedral test cases

    testCase(createGeometry, dc, hexElementAM, IGeometry::Type::Hexahedral, daHexVert, daHexList, false, false);

    // Bad HexList test with treatErrorsAsWarnings == true

    dap = DataArrayPath(k_DataContainerName, k_HexElementAttributeMatrixName, k_BadHexListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedHexListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCase(createGeometry, dc, hexElementAM, IGeometry::Type::Hexahedral, daHexVert, daBadHexList, true, false);

    // Test Hexahedral with moving arrays from attribute matrix

    dap = DataArrayPath(k_DataContainerName, k_HexElementAttributeMatrixName, k_HexListDAName);
    var.setValue(dap);
    propWasSet = createGeometry->setProperty("SharedHexListArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    testCase(createGeometry, dc, hexElementAM, IGeometry::Type::Hexahedral, daHexVert, daHexList, false, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#### CreateGeometryTest Starting ####" << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(CreateGeometryImageTest())
    DREAM3D_REGISTER_TEST(CreateGeometryRectGridTest())
    DREAM3D_REGISTER_TEST(CreateGeometryVertexTest())
    DREAM3D_REGISTER_TEST(CreateGeometryEdgeTest())
    DREAM3D_REGISTER_TEST(CreateGeometryTriangleTest())
    DREAM3D_REGISTER_TEST(CreateGeometryQuadTest())
    DREAM3D_REGISTER_TEST(CreateGeometryTetrahedralTest())
    DREAM3D_REGISTER_TEST(CreateGeometryHexahedralTest())
  }

private:
  QString m_FilterName = QString("CreateGeometry");
  QVector<size_t> m_Dims1 = {1};
  QVector<size_t> m_Dims2 = {2};
  QVector<size_t> m_Dims3 = {3};
  QVector<size_t> m_Dims4 = {4};
  QVector<size_t> m_Dims8 = {8};
};
