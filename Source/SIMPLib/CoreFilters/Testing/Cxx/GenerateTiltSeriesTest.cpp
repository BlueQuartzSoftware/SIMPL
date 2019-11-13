/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *    United States Air Force Prime Contract FA8650-10-D-5210
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"
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

template <class T>
bool dataArrayEqual(const T& a, const T& b)
{
  std::vector<size_t> aDims = a.getComponentDimensions();
  size_t aSize = a.getNumberOfTuples();

  std::vector<size_t> bDims = b.getComponentDimensions();
  size_t bSize = b.getNumberOfTuples();

  if(aDims != bDims)
  {
    return false;
  }

  if(aSize != bSize)
  {
    return false;
  }

  return std::equal(a.begin(), a.end(), b.begin(), b.end());
}

// -----------------------------------------------------------------------------
class GenerateTiltSeriesTest
{
  const DataArrayPath k_CellDataArray = DataArrayPath("SyntheticVolumeDataContainer", "CellData", "FeatureIds");
  const QString k_RotationBaseString = QString("Rotation_%1_%2");
  const QString k_FeatureIdsName = QString("FeatureIds");
  const QString k_SliceDataName = QString("Slice Data");
  const int k_NumRotations = 12;
  const float k_Increment = 15.0f;

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
    // QFile::remove(UnitTest::GenerateTiltSeriesTest::TestFile1);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestGenerateTiltSeriesTest()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainerReader::Pointer dataContainerReader = DataContainerReader::New();
    dataContainerReader->setDataContainerArray(dca);
    dataContainerReader->setInputFile(UnitTest::GenerateTiltSeriesTest::TestFile1);
    DataContainerArrayProxy proxy = dataContainerReader->readDataContainerArrayStructure(UnitTest::GenerateTiltSeriesTest::TestFile1);
    dataContainerReader->setInputFileDataContainerArrayProxy(proxy);

    dataContainerReader->execute();
    int err = dataContainerReader->getErrorCode();
    DREAM3D_REQUIRED(err, >=, 0)

    DataArrayPath featureIdsPath("", k_SliceDataName, k_FeatureIdsName);

    const std::map<QString, int> rotations = {{"X", 0}, {"Y", 1}, {"Z", 2}};

    GenerateTiltSeries::Pointer generateTiltSeries = GenerateTiltSeries::New();
    generateTiltSeries->setDataContainerArray(dca);
    FloatVec3Type rotationLimits = {0.0f, 180.0f, k_Increment};
    generateTiltSeries->setRotationLimits(rotationLimits);
    generateTiltSeries->setInputDataArrayPath(k_CellDataArray);

    for(const auto& rotation : rotations)
    {
      QString rotationString = rotation.first;
      int rotationValue = rotation.second;
      QString outputDc = QString("Rotation_Test_%1_").arg(rotationString);

      generateTiltSeries->setOutputPrefix(outputDc);
      generateTiltSeries->setRotationAxis(rotationValue);

      for(int i = 0; i < k_NumRotations; i++)
      {
        DataArrayPath outputFeatureIdsPath(outputDc + QString::number(i), k_SliceDataName, k_FeatureIdsName);

        QString rotationDc = QString(k_RotationBaseString).arg(rotationString).arg(i);
        featureIdsPath.setDataContainerName(rotationDc);
        Int8ArrayType::Pointer dataArray = dca->getPrereqIDataArrayFromPath<Int8ArrayType, AbstractFilter>(nullptr, featureIdsPath);
        DREAM3D_REQUIRE_VALID_POINTER(dataArray)

        ImageGeom::Pointer imageGeom = dca->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(nullptr, featureIdsPath);
        DREAM3D_REQUIRE_VALID_POINTER(imageGeom)

        FloatVec3Type spacing = imageGeom->getSpacing();
        generateTiltSeries->setSpacing(spacing);

        generateTiltSeries->preflight();
        err = generateTiltSeries->getErrorCode();
        DREAM3D_REQUIRED(err, >=, 0)

        generateTiltSeries->execute();
        err = generateTiltSeries->getErrorCode();
        DREAM3D_REQUIRED(err, >=, 0)

        Int8ArrayType::Pointer testArray = dca->getPrereqIDataArrayFromPath<Int8ArrayType, AbstractFilter>(nullptr, outputFeatureIdsPath);
        DREAM3D_REQUIRE_VALID_POINTER(testArray)

        DREAM3D_REQUIRE(dataArrayEqual(*testArray, *dataArray))
      }
    }

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
};
