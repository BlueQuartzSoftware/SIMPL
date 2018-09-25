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

#include "SIMPLib/Geometry/VertexGeom.h"

#include "SIMPLib/Common/Constants.h"

class CropVertexGeometryTest
{

public:
  CropVertexGeometryTest() = default;
  ~CropVertexGeometryTest() = default;
  CropVertexGeometryTest(const CropVertexGeometryTest&) = delete;            // Copy Constructor
  CropVertexGeometryTest(CropVertexGeometryTest&&) = delete;                 // Move Constructor
  CropVertexGeometryTest& operator=(const CropVertexGeometryTest&) = delete; // Copy Assignment
  CropVertexGeometryTest& operator=(CropVertexGeometryTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the CropVertexGeometryTest Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The CropVertexGeometryTest Requires the use of the " << m_FilterName.toStdString() << " filter which is found in Core Filters";
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

  void testCase(std::vector<std::vector<float>> vertices, std::vector<std::vector<float>> postCropVertices, float xMin, float yMin, float zMin, float xMax, float yMax, float zMax)
  {
    static const QString k_DataContainerName("DataContainer");
    static const QString k_CroppedDataContainerName("CroppedDataContainer");
    static const QString k_VertexAttributeMatrixName("VertexAttributeMatrix");
    static const QString k_AttributeMatrixName("AttributeMatrix");
    static const QString k_DataArray0Name("DataArray0");
    static const QString k_VertexCoordinatesDAName("VertexCoordinates");
    static const QString k_CroppedVertexCoordinatesDAName("CroppedVertexCoordinates");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create AttributeMatrix

    QVector<size_t> dims(1, vertices.size());

    AttributeMatrix::Pointer vertexAM = AttributeMatrix::New(dims, k_VertexAttributeMatrixName, AttributeMatrix::Type::Vertex);
    dc->addAttributeMatrix(k_VertexAttributeMatrixName, vertexAM);

    AttributeMatrix::Pointer nonVertexAM = AttributeMatrix::New(m_Dims4, k_AttributeMatrixName, AttributeMatrix::Type::Generic);
    dc->addAttributeMatrix(k_AttributeMatrixName, nonVertexAM);

    // Create DataArray used for geometry creation

    FloatArrayType::Pointer daVert = createDataArray<float>(k_VertexCoordinatesDAName, vertices, dims, m_Dims3);
    vertexAM->addAttributeArray(k_VertexCoordinatesDAName, daVert);

    // Manually create cropped DataArray for comparison

    QVector<size_t> croppedDims(1, postCropVertices.size());

    FloatArrayType::Pointer daCroppedVert = createDataArray<float>(k_CroppedVertexCoordinatesDAName, postCropVertices, croppedDims, m_Dims3);

    // Create extra DataArray to be copied

    FloatArrayType::Pointer da = DataArray<float>::CreateArray(m_Dims4, m_Dims3, k_DataArray0Name);
    for(int i = 0; i < da->getSize(); i++)
    {
      da->setValue(i, i + 0.5f);
    }
    nonVertexAM->addAttributeArray(k_DataArray0Name, da);

    // Create Geometry

    VertexGeom::Pointer geom = VertexGeom::CreateGeometry(daVert, SIMPL::Geometry::VertexGeometry);

    dc->setGeometry(geom);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer cropVertexGeometry = filterFactory->create();

    cropVertexGeometry->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    var.setValue(k_DataContainerName);
    bool propWasSet = cropVertexGeometry->setProperty("DataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(k_CroppedDataContainerName);
    propWasSet = cropVertexGeometry->setProperty("CroppedDataContainerName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(xMin);
    propWasSet = cropVertexGeometry->setProperty("XMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(yMin);
    propWasSet = cropVertexGeometry->setProperty("YMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(zMin);
    propWasSet = cropVertexGeometry->setProperty("ZMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(xMax);
    propWasSet = cropVertexGeometry->setProperty("XMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(yMax);
    propWasSet = cropVertexGeometry->setProperty("YMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(zMax);
    propWasSet = cropVertexGeometry->setProperty("ZMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run Filter

    cropVertexGeometry->execute();
    DREAM3D_REQUIRED(cropVertexGeometry->getErrorCondition(), >=, 0);

    // Check filter results

    DataContainer::Pointer croppedDC = dca->getDataContainer(DataArrayPath(k_CroppedDataContainerName));

    QList<QString> names = croppedDC->getAttributeMatrixNames();

    // Check VertexGeom and Vertex Matrix

    DREAM3D_REQUIRE_EQUAL(names.contains(k_VertexAttributeMatrixName), true)

    AttributeMatrix::Pointer croppedVertexAM = croppedDC->getAttributeMatrix(DataArrayPath(k_CroppedDataContainerName, k_VertexAttributeMatrixName, ""));

    DREAM3D_REQUIRE_EQUAL((daCroppedVert->getNumberOfTuples() == croppedVertexAM->getNumberOfTuples()), true)

    FloatArrayType::Pointer croppedVerticesFromAM = croppedVertexAM->getAttributeArrayAs<FloatArrayType>(k_VertexCoordinatesDAName);

    checkDataArray<float>(daCroppedVert, croppedVerticesFromAM);

    FloatArrayType::Pointer croppedVertices = croppedDC->getGeometryAs<VertexGeom>()->getVertices();

    checkDataArray<float>(daCroppedVert, croppedVertices);

    // Check Non-Vertex Matrix

    DREAM3D_REQUIRE_EQUAL(names.contains(k_AttributeMatrixName), true)

    AttributeMatrix::Pointer croppedNonVertexAM = croppedDC->getAttributeMatrix(DataArrayPath(k_CroppedDataContainerName, k_AttributeMatrixName, ""));

    DREAM3D_REQUIRE_EQUAL((da->getNumberOfTuples() == croppedNonVertexAM->getNumberOfTuples()), true)

    FloatArrayType::Pointer data = croppedNonVertexAM->getAttributeArrayAs<FloatArrayType>(k_DataArray0Name);

    checkDataArray<float>(da, data);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCropVertexGeometryTest()
  {
    //  0.0f <= x <=  3.0f
    //  0.0f <= y <= 10.0f
    //  0.0f <= z <= 10.0f

    std::vector<std::vector<float>> vertices = {{1.0, 1.0, 0.0}, {3.0, 1.0, 0.0}, {3.1, 1.0, -1.0}, {-1.0, 1.0, 2.0}};

    std::vector<std::vector<float>> croppedVertices = {{1.0, 1.0, 0.0}, {3.0, 1.0, 0.0}};

    testCase(vertices, croppedVertices, 0.0f, 0.0f, 0.0f, 3.0f, 10.0f, 10.0f);

    // -1.0f <= x <=  2.0f
    //  0.0f <= y <= 10.0f
    //  0.0f <= z <= 10.0f

    vertices = {{1.0, 1.0, 0.0}, {3.0, 1.0, 0.0}, {3.1, 1.0, -1.0}, {-1.0, 1.0, 2.0}};

    croppedVertices = {{1.0, 1.0, 0.0}, {-1.0, 1.0, 2.0}};

    testCase(vertices, croppedVertices, -1.0f, 0.0f, 0.0f, 2.0f, 10.0f, 10.0f);

    // -1.0f <= x <=  2.0f
    //  2.3f <= y <= 10.0f
    // -4.4f <= z <=  3.2f

    vertices = {{-0.5, 7.91, 1.15}, {0.0, 2.3, -5.5}, {0.3, 11.0, 3.2}, {1.0, 9.99, -4.399}, {0.0214f, 2.300001f, 3.19999f}};

    croppedVertices = {{-0.5, 7.91, 1.15}, {1.0, 9.99, -4.399}, {0.0214f, 2.300001f, 3.19999f}};

    testCase(vertices, croppedVertices, -1.0f, 2.3f, -4.4f, 2.0f, 10.0f, 3.2f);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#### CropVertexGeometryTest Starting ####" << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability())

    DREAM3D_REGISTER_TEST(TestCropVertexGeometryTest())
  }

private:
  QString m_FilterName = QString("CropVertexGeometry");
  QVector<size_t> m_Dims1 = {1};
  QVector<size_t> m_Dims2 = {2};
  QVector<size_t> m_Dims3 = {3};
  QVector<size_t> m_Dims4 = {4};
  QVector<size_t> m_Dims8 = {8};
};
