/* ============================================================================
 * Copyright (c) 2022-2022 BlueQuartz Software, LLC
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/CoreFilters/PadImageGeometry.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#include "SIMPLib/Testing/UnitTestSupport.hpp"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"

class PadImageGeometryTest
{

public:
  PadImageGeometryTest() = default;
  ~PadImageGeometryTest() = default;
  PadImageGeometryTest(const PadImageGeometryTest&) = delete;            // Copy Constructor
  PadImageGeometryTest(PadImageGeometryTest&&) = delete;                 // Move Constructor
  PadImageGeometryTest& operator=(const PadImageGeometryTest&) = delete; // Copy Assignment
  PadImageGeometryTest& operator=(PadImageGeometryTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the PadImageGeometryTest Filter from the FilterManager
    QString filtName = "PadImageGeometry";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The PadImageGeometryTest Requires the use of the " << filtName.toStdString() << " filter which is found in the Generic Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  template <typename T, size_t N>
  DataContainerArray::Pointer createDataStructure(T arrayInitValue, const FloatVec3Type& origin)
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New(k_DataArrayPath.getDataContainerName());
    ImageGeom::Pointer imageGeom = ImageGeom::New();
    imageGeom->setName(k_ImageGeomName);
    imageGeom->setDimensions(k_TupleDims);
    imageGeom->setOrigin(origin);
    imageGeom->setSpacing({1, 1, 1});
    dc->setGeometry(imageGeom);
    dca->addOrReplaceDataContainer(dc);

    AttributeMatrix::Pointer am = AttributeMatrix::New(k_TupleDims, k_DataArrayPath.getAttributeMatrixName(), AttributeMatrix::Type::Any);
    dc->addOrReplaceAttributeMatrix(am);

    std::vector<size_t> cDims = {N};
    typename DataArray<T>::Pointer da = DataArray<T>::CreateArray(k_TupleDims, cDims, k_DataArrayPath.getDataArrayName(), true);
    da->initializeWithValue(arrayInitValue);
    am->addOrReplaceAttributeArray(da);

    return dca;
  }

  // -----------------------------------------------------------------------------
  template <typename T, size_t N>
  DataContainerArray::Pointer createDataStructure_NoGeometry(T arrayInitValue)
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New(k_DataArrayPath.getDataContainerName());
    dca->addOrReplaceDataContainer(dc);

    AttributeMatrix::Pointer am = AttributeMatrix::New(k_TupleDims, k_DataArrayPath.getAttributeMatrixName(), AttributeMatrix::Type::Any);
    dc->addOrReplaceAttributeMatrix(am);

    std::vector<size_t> cDims = {N};
    typename DataArray<T>::Pointer da = DataArray<T>::CreateArray(k_TupleDims, cDims, k_DataArrayPath.getDataArrayName(), true);
    da->initializeWithValue(arrayInitValue);
    am->addOrReplaceAttributeArray(da);

    return dca;
  }

  // -----------------------------------------------------------------------------
  template <typename T, size_t N>
  DataContainerArray::Pointer createDataStructure_NoImageGeometry(T arrayInitValue)
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New(k_DataArrayPath.getDataContainerName());
    VertexGeom::Pointer vertexGeom = VertexGeom::New();
    dc->setGeometry(vertexGeom);
    dca->addOrReplaceDataContainer(dc);

    AttributeMatrix::Pointer am = AttributeMatrix::New(k_TupleDims, k_DataArrayPath.getAttributeMatrixName(), AttributeMatrix::Type::Any);
    dc->addOrReplaceAttributeMatrix(am);

    std::vector<size_t> cDims = {N};
    typename DataArray<T>::Pointer da = DataArray<T>::CreateArray(k_TupleDims, cDims, k_DataArrayPath.getDataArrayName(), true);
    da->initializeWithValue(arrayInitValue);
    am->addOrReplaceAttributeArray(da);

    return dca;
  }

  // -----------------------------------------------------------------------------
  bool isPointInsideBox(int x, int y, int z, const IntVec2Type& xRange, const IntVec2Type& yRange, const IntVec2Type& zRange)
  {
    return (x >= xRange[0] && x < xRange[1] && y >= yRange[0] && y < yRange[1] && z >= zRange[0] && z < zRange[1]);
  }

  // -----------------------------------------------------------------------------
  // execute_valid: This tests a valid run of the PadImageGeometry filter
  template <typename T, size_t N>
  void execute_valid(const IntVec2Type& xMinMax, const IntVec2Type& yMinMax, const IntVec2Type& zMinMax, T arrayInitValue, int arrayPaddingValue, const FloatVec3Type& origin, bool updateOrigin)
  {
    DataContainerArray::Pointer dca = createDataStructure<T, N>(arrayInitValue, origin);

    PadImageGeometry::Pointer filter = PadImageGeometry::New();
    filter->setDataContainerArray(dca);
    filter->setAttributeMatrixPath(k_DataArrayPath);
    filter->setXMinMax(xMinMax);
    filter->setYMinMax(yMinMax);
    filter->setZMinMax(zMinMax);
    filter->setDefaultFillValue(arrayPaddingValue);
    filter->setUpdateOrigin(updateOrigin);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), 0)

    SizeVec3Type newTupleDims = {k_TupleDims[0] + xMinMax[0] + xMinMax[1], k_TupleDims[1] + yMinMax[0] + yMinMax[1], k_TupleDims[2] + zMinMax[0] + zMinMax[1]};

    DataContainer::Pointer dc = dca->getDataContainer(k_DataArrayPath.getDataContainerName());
    ImageGeom::Pointer geom = dc->getGeometryAs<ImageGeom>();
    AttributeMatrix::Pointer am = dc->getAttributeMatrix(k_DataArrayPath.getAttributeMatrixName());
    typename DataArray<T>::Pointer da = am->getAttributeArrayAs<DataArray<T>>(k_DataArrayPath.getDataArrayName());
    T* daPtr = da->getPointer(0);
    size_t numOfComps = da->getNumberOfComponents();

    int xMin = xMinMax[0];
    int yMin = yMinMax[0];
    int zMin = zMinMax[0];
    int xMax = static_cast<int>(newTupleDims[0]) - xMinMax[1];
    int yMax = static_cast<int>(newTupleDims[1]) - yMinMax[1];
    int zMax = static_cast<int>(newTupleDims[2]) - zMinMax[1];
    for(size_t z = 0; z < newTupleDims[2]; z++)
    {
      for(size_t y = 0; y < newTupleDims[1]; y++)
      {
        for(size_t x = 0; x < newTupleDims[0]; x++)
        {
          size_t index = (z * newTupleDims[0] * newTupleDims[1]) + (y * newTupleDims[0]) + x;
          bool insideBox = isPointInsideBox(x, y, z, {xMin, xMax}, {yMin, yMax}, {zMin, zMax});
          for(size_t c = 0; c < numOfComps; c++)
          {
            if(insideBox)
            {
              DREAM3D_REQUIRE_EQUAL(daPtr[index * numOfComps + c], arrayInitValue)
            }
            else if constexpr(std::is_same<T, bool>::value)
            {
              DREAM3D_REQUIRE_EQUAL(daPtr[index * numOfComps + c], (arrayPaddingValue != 0))
            }
            else
            {
              DREAM3D_REQUIRE_EQUAL(daPtr[index * numOfComps + c], static_cast<T>(arrayPaddingValue))
            }
          }
        }
      }
    }

    if(updateOrigin)
    {
      FloatVec3Type newOrigin = geom->getOrigin();
      DREAM3D_REQUIRE_EQUAL(newOrigin[0], origin[0] - xMin)
      DREAM3D_REQUIRE_EQUAL(newOrigin[1], origin[1] - yMin)
      DREAM3D_REQUIRE_EQUAL(newOrigin[2], origin[2] - zMin)
    }
  }

  // -----------------------------------------------------------------------------
  // execute_invalid: This tests an invalid run of the PadImageGeometry filter
  template <typename T, size_t N>
  void execute_invalid(const IntVec2Type& xMinMax, const IntVec2Type& yMinMax, const IntVec2Type& zMinMax, T arrayInitValue, int arrayPaddingValue, bool updateOrigin, DataContainerArray::Pointer dca,
                       int expectedErrCode)
  {
    PadImageGeometry::Pointer filter = PadImageGeometry::New();
    filter->setDataContainerArray(dca);
    filter->setAttributeMatrixPath(k_DataArrayPath);
    filter->setXMinMax(xMinMax);
    filter->setYMinMax(yMinMax);
    filter->setZMinMax(zMinMax);
    filter->setDefaultFillValue(arrayPaddingValue);
    filter->setUpdateOrigin(updateOrigin);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), expectedErrCode)
  }

  // -----------------------------------------------------------------------------
  // executeValid_TestPrimitives: This tests a valid run of the PadImageGeometry filter
  template <typename T>
  void executeValid_TestPrimitives(const IntVec2Type& xMinMax, const IntVec2Type& yMinMax, const IntVec2Type& zMinMax, T arrayInitValue, int arrayPaddingValue, const FloatVec3Type& origin,
                                   bool updateOrigin)
  {
    execute_valid<T, 1>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, origin, updateOrigin);
    execute_valid<T, 2>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, origin, updateOrigin);
    execute_valid<T, 3>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, origin, updateOrigin);
  }

  // -----------------------------------------------------------------------------
  // executeInvalid_TestPrimitives: This tests an invalid run of the PadImageGeometry filter
  template <typename T>
  void executeInvalid_TestPrimitives(const IntVec2Type& xMinMax, const IntVec2Type& yMinMax, const IntVec2Type& zMinMax, T arrayInitValue, int arrayPaddingValue, const FloatVec3Type& origin,
                                     bool updateOrigin, int expectedErrCode)
  {
    DataContainerArray::Pointer dca = createDataStructure<T, 1>(arrayInitValue, origin);
    execute_invalid<T, 1>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, updateOrigin, dca, expectedErrCode);
    dca = createDataStructure<T, 2>(arrayInitValue, origin);
    execute_invalid<T, 2>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, updateOrigin, dca, expectedErrCode);
    dca = createDataStructure<T, 3>(arrayInitValue, origin);
    execute_invalid<T, 3>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, updateOrigin, dca, expectedErrCode);
  }

  // -----------------------------------------------------------------------------
  // executeInvalid_TestPrimitives: This tests an invalid run of the PadImageGeometry filter
  template <typename T>
  void executeInvalid_WrongGeometry_TestPrimitives(const IntVec2Type& xMinMax, const IntVec2Type& yMinMax, const IntVec2Type& zMinMax, T arrayInitValue, int arrayPaddingValue, bool updateOrigin)
  {
    DataContainerArray::Pointer dca = createDataStructure_NoGeometry<T, 1>(arrayInitValue);
    execute_invalid<T, 1>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, updateOrigin, dca, PadImageGeometry::ErrorCodes::MISSING_IMG_GEOMETRY);
    dca = createDataStructure_NoGeometry<T, 2>(arrayInitValue);
    execute_invalid<T, 2>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, updateOrigin, dca, PadImageGeometry::ErrorCodes::MISSING_IMG_GEOMETRY);
    dca = createDataStructure_NoGeometry<T, 3>(arrayInitValue);
    execute_invalid<T, 3>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, updateOrigin, dca, PadImageGeometry::ErrorCodes::MISSING_IMG_GEOMETRY);

    dca = createDataStructure_NoImageGeometry<T, 1>(arrayInitValue);
    execute_invalid<T, 1>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, updateOrigin, dca, PadImageGeometry::ErrorCodes::MISSING_IMG_GEOMETRY);
    dca = createDataStructure_NoImageGeometry<T, 2>(arrayInitValue);
    execute_invalid<T, 2>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, updateOrigin, dca, PadImageGeometry::ErrorCodes::MISSING_IMG_GEOMETRY);
    dca = createDataStructure_NoImageGeometry<T, 3>(arrayInitValue);
    execute_invalid<T, 3>(xMinMax, yMinMax, zMinMax, arrayInitValue, arrayPaddingValue, updateOrigin, dca, PadImageGeometry::ErrorCodes::MISSING_IMG_GEOMETRY);
  }

  // -----------------------------------------------------------------------------
  // testCase1: This tests padding a geometry by 2 voxels on all sides
  void testCase1()
  {
    executeValid_TestPrimitives<int8_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint8_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int16_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint16_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int32_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint32_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int64_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint64_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<float>({2, 2}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<double>({2, 2}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<bool>({2, 2}, {2, 2}, {2, 2}, true, 0, {0, 0, 0}, false);
  }

  // -----------------------------------------------------------------------------
  // testCase2: This tests padding a geometry by 2 voxels on just the right side of each dimension
  void testCase2()
  {
    executeValid_TestPrimitives<int8_t>({0, 2}, {0, 2}, {0, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint8_t>({0, 2}, {0, 2}, {0, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int16_t>({0, 2}, {0, 2}, {0, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint16_t>({0, 2}, {0, 2}, {0, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int32_t>({0, 2}, {0, 2}, {0, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint32_t>({0, 2}, {0, 2}, {0, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int64_t>({0, 2}, {0, 2}, {0, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint64_t>({0, 2}, {0, 2}, {0, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<float>({0, 2}, {0, 2}, {0, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<double>({0, 2}, {0, 2}, {0, 2}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<bool>({0, 2}, {0, 2}, {0, 2}, true, 0, {0, 0, 0}, false);
  }

  // -----------------------------------------------------------------------------
  // testCase3: This tests padding a geometry by 3 voxels on just the left side of each dimension
  void testCase3()
  {
    executeValid_TestPrimitives<int8_t>({3, 0}, {3, 0}, {3, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint8_t>({3, 0}, {3, 0}, {3, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int16_t>({3, 0}, {3, 0}, {3, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint16_t>({3, 0}, {3, 0}, {3, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int32_t>({3, 0}, {3, 0}, {3, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint32_t>({3, 0}, {3, 0}, {3, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int64_t>({3, 0}, {3, 0}, {3, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint64_t>({3, 0}, {3, 0}, {3, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<float>({3, 0}, {3, 0}, {3, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<double>({3, 0}, {3, 0}, {3, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<bool>({3, 0}, {3, 0}, {3, 0}, true, 0, {0, 0, 0}, false);
  }

  // -----------------------------------------------------------------------------
  // testCase4: This tests padding a geometry by no voxels at all
  void testCase4()
  {
    executeValid_TestPrimitives<int8_t>({0, 0}, {0, 0}, {0, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint8_t>({0, 0}, {0, 0}, {0, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int16_t>({0, 0}, {0, 0}, {0, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint16_t>({0, 0}, {0, 0}, {0, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int32_t>({0, 0}, {0, 0}, {0, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint32_t>({0, 0}, {0, 0}, {0, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<int64_t>({0, 0}, {0, 0}, {0, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<uint64_t>({0, 0}, {0, 0}, {0, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<float>({0, 0}, {0, 0}, {0, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<double>({0, 0}, {0, 0}, {0, 0}, 1, 0, {0, 0, 0}, false);
    executeValid_TestPrimitives<bool>({0, 0}, {0, 0}, {0, 0}, true, 0, {0, 0, 0}, false);
  }

  // -----------------------------------------------------------------------------
  // testCase5: This tests padding a geometry and updating the origin
  void testCase5()
  {
    executeValid_TestPrimitives<int8_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {10, 10, 10}, true);
    executeValid_TestPrimitives<uint8_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {10, 10, 10}, true);
    executeValid_TestPrimitives<int16_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {10, 10, 10}, true);
    executeValid_TestPrimitives<uint16_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {10, 10, 10}, true);
    executeValid_TestPrimitives<int32_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {10, 10, 10}, true);
    executeValid_TestPrimitives<uint32_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {10, 10, 10}, true);
    executeValid_TestPrimitives<int64_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {10, 10, 10}, true);
    executeValid_TestPrimitives<uint64_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, {10, 10, 10}, true);
    executeValid_TestPrimitives<float>({2, 2}, {2, 2}, {2, 2}, 1, 0, {10, 10, 10}, true);
    executeValid_TestPrimitives<double>({2, 2}, {2, 2}, {2, 2}, 1, 0, {10, 10, 10}, true);
    executeValid_TestPrimitives<bool>({2, 2}, {2, 2}, {2, 2}, true, 0, {10, 10, 10}, true);
  }

  // -----------------------------------------------------------------------------
  // testCase6: This tests padding a geometry with negative numbers in the X dimension (should fail)
  void testCase6()
  {
    executeInvalid_TestPrimitives<int8_t>({-5, 5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint8_t>({-5, 5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int16_t>({-5, 5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint16_t>({-5, 5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int32_t>({-5, 5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint32_t>({-5, 5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int64_t>({-5, 5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint64_t>({-5, 5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<float>({-5, 5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<double>({-5, 5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<bool>({-5, 5}, {2, 2}, {2, 2}, true, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);

    executeInvalid_TestPrimitives<int8_t>({5, -5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint8_t>({5, -5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int16_t>({5, -5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint16_t>({5, -5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int32_t>({5, -5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint32_t>({5, -5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int64_t>({5, -5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint64_t>({5, -5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<float>({5, -5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<double>({5, -5}, {2, 2}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<bool>({5, -5}, {2, 2}, {2, 2}, true, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::X_VALUES_NEGATIVE);
  }

  // -----------------------------------------------------------------------------
  // testCase7: This tests padding a geometry with negative numbers in the Y dimension (should fail)
  void testCase7()
  {
    executeInvalid_TestPrimitives<int8_t>({2, 2}, {-5, 5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint8_t>({2, 2}, {-5, 5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int16_t>({2, 2}, {-5, 5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint16_t>({2, 2}, {-5, 5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int32_t>({2, 2}, {-5, 5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint32_t>({2, 2}, {-5, 5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int64_t>({2, 2}, {-5, 5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint64_t>({2, 2}, {-5, 5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<float>({2, 2}, {-5, 5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<double>({2, 2}, {-5, 5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<bool>({2, 2}, {-5, 5}, {2, 2}, true, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);

    executeInvalid_TestPrimitives<int8_t>({2, 2}, {5, -5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint8_t>({2, 2}, {5, -5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int16_t>({2, 2}, {5, -5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint16_t>({2, 2}, {5, -5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int32_t>({2, 2}, {5, -5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint32_t>({2, 2}, {5, -5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int64_t>({2, 2}, {5, -5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint64_t>({2, 2}, {5, -5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<float>({2, 2}, {5, -5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<double>({2, 2}, {5, -5}, {2, 2}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<bool>({2, 2}, {5, -5}, {2, 2}, true, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Y_VALUES_NEGATIVE);
  }

  // -----------------------------------------------------------------------------
  // testCase8: This tests padding a geometry with negative numbers in the Z dimension (should fail)
  void testCase8()
  {
    executeInvalid_TestPrimitives<int8_t>({2, 2}, {2, 2}, {-5, 5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint8_t>({2, 2}, {2, 2}, {-5, 5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int16_t>({2, 2}, {2, 2}, {-5, 5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint16_t>({2, 2}, {2, 2}, {-5, 5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int32_t>({2, 2}, {2, 2}, {-5, 5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint32_t>({2, 2}, {2, 2}, {-5, 5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int64_t>({2, 2}, {2, 2}, {-5, 5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint64_t>({2, 2}, {2, 2}, {-5, 5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<float>({2, 2}, {2, 2}, {-5, 5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<double>({2, 2}, {2, 2}, {-5, 5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<bool>({2, 2}, {2, 2}, {-5, 5}, true, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);

    executeInvalid_TestPrimitives<int8_t>({2, 2}, {2, 2}, {5, -5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint8_t>({2, 2}, {2, 2}, {5, -5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int16_t>({2, 2}, {2, 2}, {5, -5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint16_t>({2, 2}, {2, 2}, {5, -5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int32_t>({2, 2}, {2, 2}, {5, -5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint32_t>({2, 2}, {2, 2}, {5, -5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<int64_t>({2, 2}, {2, 2}, {5, -5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<uint64_t>({2, 2}, {2, 2}, {5, -5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<float>({2, 2}, {2, 2}, {5, -5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<double>({2, 2}, {2, 2}, {5, -5}, 1, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
    executeInvalid_TestPrimitives<bool>({2, 2}, {2, 2}, {5, -5}, true, 0, {0, 0, 0}, false, PadImageGeometry::ErrorCodes::Z_VALUES_NEGATIVE);
  }

  // -----------------------------------------------------------------------------
  // testCase9: This tests padding a geometry with the wrong geometry or a missing geometry (should fail)
  void testCase9()
  {
    executeInvalid_WrongGeometry_TestPrimitives<int8_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, false);
    executeInvalid_WrongGeometry_TestPrimitives<uint8_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, false);
    executeInvalid_WrongGeometry_TestPrimitives<int16_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, false);
    executeInvalid_WrongGeometry_TestPrimitives<uint16_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, false);
    executeInvalid_WrongGeometry_TestPrimitives<int32_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, false);
    executeInvalid_WrongGeometry_TestPrimitives<uint32_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, false);
    executeInvalid_WrongGeometry_TestPrimitives<int64_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, false);
    executeInvalid_WrongGeometry_TestPrimitives<uint64_t>({2, 2}, {2, 2}, {2, 2}, 1, 0, false);
    executeInvalid_WrongGeometry_TestPrimitives<float>({2, 2}, {2, 2}, {2, 2}, 1, 0, false);
    executeInvalid_WrongGeometry_TestPrimitives<double>({2, 2}, {2, 2}, {2, 2}, 1, 0, false);
    executeInvalid_WrongGeometry_TestPrimitives<bool>({2, 2}, {2, 2}, {2, 2}, true, 0, false);
  }

  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;
    Q_UNUSED(err)

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(testCase1())
    DREAM3D_REGISTER_TEST(testCase2())
    DREAM3D_REGISTER_TEST(testCase3())
    DREAM3D_REGISTER_TEST(testCase4())
    DREAM3D_REGISTER_TEST(testCase5())
    DREAM3D_REGISTER_TEST(testCase6())
    DREAM3D_REGISTER_TEST(testCase7())
    DREAM3D_REGISTER_TEST(testCase8())
    DREAM3D_REGISTER_TEST(testCase9())
  }

private:
  const DataArrayPath k_DataArrayPath = DataArrayPath("DataContainer", "AttributeMatrix", "DataArray");
  const QString k_ImageGeomName = "Image Geometry";
  const std::vector<size_t> k_TupleDims = {10, 10, 10};
};
