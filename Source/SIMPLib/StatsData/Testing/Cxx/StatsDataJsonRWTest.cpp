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
*    United States Air Force Prime Contract FA8650-10-D-5210
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <cmath>

#include <iostream>

#include <QtCore/QDir>
#include <QtCore/QJsonDocument>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/StatsData/BoundaryStatsData.h"
#include "SIMPLib/StatsData/MatrixStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class StatsDataJsonRWTest
{
public:
  StatsDataJsonRWTest()
  : m_BoundaryStatsFileName("BoundaryStats.json")
  , m_MatrixStatsFileName("MatrixStats.json")
  , m_PrecipitateStatsFileName("PrecipitateStats.json")
  , m_PrimaryStatsFileName("PrimaryStats.json")
  , m_TransformationStatsFileName("Transformation.json")
  {
  }

  virtual ~StatsDataJsonRWTest() = default;

  SIMPL_INSTANCE_STRING_PROPERTY(BoundaryStatsFileName)
  SIMPL_INSTANCE_STRING_PROPERTY(MatrixStatsFileName)
  SIMPL_INSTANCE_STRING_PROPERTY(PrecipitateStatsFileName)
  SIMPL_INSTANCE_STRING_PROPERTY(PrimaryStatsFileName)
  SIMPL_INSTANCE_STRING_PROPERTY(TransformationStatsFileName)

  /**
   * @brief Removes any test files that were created during this test
   */
  void RemoveTestFiles()
  {
    QString jsonFilePath = UnitTest::TestTempDir + QDir::separator() + m_BoundaryStatsFileName;
    QFile::remove(jsonFilePath);

    jsonFilePath = UnitTest::TestTempDir + QDir::separator() + m_MatrixStatsFileName;
    QFile::remove(jsonFilePath);

    jsonFilePath = UnitTest::TestTempDir + QDir::separator() + m_PrecipitateStatsFileName;
    QFile::remove(jsonFilePath);

    jsonFilePath = UnitTest::TestTempDir + QDir::separator() + m_PrimaryStatsFileName;
    QFile::remove(jsonFilePath);

    jsonFilePath = UnitTest::TestTempDir + QDir::separator() + m_TransformationStatsFileName;
    QFile::remove(jsonFilePath);
  }

  /**
   * @brief writeJsonFile
   * @param jsonRoot
   * @param fileName
   */
  void writeJsonFile(QJsonObject& jsonRoot, const QString& fileName) const
  {
    QString jsonFilePath = UnitTest::TestTempDir + QDir::separator() + fileName;
    QFile jsonFile(jsonFilePath);
    DREAM3D_REQUIRE_EQUAL(jsonFile.open(QIODevice::WriteOnly | QIODevice::Text), true);
    // qDebug() << "Output Path:" << jsonFilePath;

    QJsonDocument doc(jsonRoot);
    QByteArray binary = doc.toJson();
    qint64 bytesWritten = jsonFile.write(binary.data(), binary.length());
    DREAM3D_REQUIRE_EQUAL(bytesWritten, binary.length());
  }

  /**
   * @brief readJsonFile
   * @param fileName
   * @return
   */
  QJsonDocument readJsonFile(const QString& fileName) const
  {
    QString jsonFilePath = UnitTest::TestTempDir + QDir::separator() + fileName;
    QFile jsonFile(jsonFilePath);
    DREAM3D_REQUIRE_EQUAL(jsonFile.open(QIODevice::ReadOnly | QIODevice::Text), true);
    // qDebug() << "JSON Path:" << jsonFilePath;

    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll(), &jsonParseError);
    DREAM3D_REQUIRED(jsonParseError.error, ==, QJsonParseError::NoError)
    DREAM3D_REQUIRE_EQUAL(doc.isNull(), false);
    return doc;
  }

  /**
   * @brief generateDistributionValues
   * @param arrays
   * @param binCount
   */
  void generateDistributionValues(VectorOfFloatArray& arrays, int binCount)
  {
    for(size_t a = 0; a < arrays.size(); a++)
    {
      for(int i = 0; i < binCount; i++)
      {
        arrays[a]->setValue(i, i * 2.0f);
      }
    }
  }

  /**
   * @brief validateDistributionValues
   * @param arrays
   * @param binCount
   */
  void validateDistributionValues(VectorOfFloatArray& arrays, int binCount)
  {
    float epsilon = 0.00001f;
    for(size_t a = 0; a < arrays.size(); a++)
    {
      for(int i = 0; i < binCount; i++)
      {
        float truth = static_cast<float>(i * 2.0f);
        float value = arrays[a]->getValue(i);
        DREAM3D_REQUIRED(std::abs(truth - value), <, epsilon);
      }
    }
  }

  template <typename T> void ConfigureStatsData(typename T::Pointer statsData)
  {

    statsData->setPhaseFraction(0.1f);
    statsData->setBoundaryArea(0.5f);

    // Write the Bin Numbers
    statsData->setBinStepSize(0.2f);
    statsData->setMaxFeatureDiameter(1.2f);
    statsData->setMinFeatureDiameter(.2f);
    FloatArrayType::Pointer bins = statsData->generateBinNumbers();
    int numTuples = bins->getNumberOfTuples();

    // Feature Size Distribution
    {
      VectorOfFloatArray data;
      FloatArrayType::Pointer d1 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Average);
      FloatArrayType::Pointer d2 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::StandardDeviation);
      data.push_back(d1);
      data.push_back(d2);
      d1->setValue(0, 1.1f);
      d2->setValue(0, 0.1f);
      statsData->setFeatureSizeDistribution(data);
      statsData->setFeatureSize_DistType(SIMPL::DistributionType::LogNormal);
    }

    // Write the B Over A
    VectorOfFloatArray arrays = StatsData::CreateCorrelatedDistributionArrays(statsData->getBOverA_DistType(), numTuples);
    generateDistributionValues(arrays, numTuples);
    statsData->setFeatureSize_BOverA(arrays);

    // Write the C Over A
    arrays = StatsData::CreateCorrelatedDistributionArrays(statsData->getCOverA_DistType(), numTuples);
    generateDistributionValues(arrays, numTuples);
    statsData->setFeatureSize_COverA(arrays);

    // Write the Omegas
    arrays = StatsData::CreateCorrelatedDistributionArrays(statsData->getOmegas_DistType(), numTuples);
    generateDistributionValues(arrays, numTuples);
    statsData->setFeatureSize_Omegas(arrays);

    // Write the Misorientation Weights
    {
      FloatArrayType::Pointer angles = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Angle);
      angles->initializeWithValue(45.0f, 0);
      std::vector<size_t> vec;
      vec.push_back(3);
      FloatArrayType::Pointer axis = FloatArrayType::CreateArray(1, vec, SIMPL::StringConstants::Axis);
      axis->initializeWithValue(1.0, 0);
      FloatArrayType::Pointer weight = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Weight);
      weight->initializeWithValue(1000.0f, 0);
      arrays.clear();
      arrays.push_back(angles);
      arrays.push_back(axis);
      arrays.push_back(weight);
      statsData->setMDF_Weights(arrays);
    }

    // Write the ODF
    {
      FloatArrayType::Pointer e1 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Euler1);
      e1->initializeWithValue(45.0f, 0);
      FloatArrayType::Pointer e2 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Euler2);
      e2->initializeWithValue(30.0f, 0);
      FloatArrayType::Pointer e3 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Euler3);
      e3->initializeWithValue(60.0f, 0);
      FloatArrayType::Pointer sigma = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Sigma);
      sigma->initializeWithValue(2.0, 0);
      FloatArrayType::Pointer weight = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Weight);
      weight->initializeWithValue(100000.0f, 0);
      arrays.clear();
      arrays.push_back(e1);
      arrays.push_back(e2);
      arrays.push_back(e3);
      arrays.push_back(sigma);
      arrays.push_back(weight);
      statsData->setODF_Weights(arrays);

      // Write the Axis ODF
      statsData->setAxisODF_Weights(arrays);
    }
  }

  /**
   * @brief ValidateStatsData
   */
  template <typename T> void ValidateStatsData(typename T::Pointer statsData)
  {
    float epsilon = 0.00001f;

    DREAM3D_REQUIRED(std::abs(statsData->getPhaseFraction() - 0.10), <, epsilon);
    DREAM3D_REQUIRED(std::abs(statsData->getBoundaryArea() - 0.50), <, epsilon);
    float diamInfo[3] = {0.0f, 0.0f, 0.0f};
    statsData->getFeatureDiameterInfo(diamInfo);
    DREAM3D_REQUIRED(std::abs(diamInfo[0] - 0.2), <, epsilon);
    DREAM3D_REQUIRED(std::abs(diamInfo[1] - 1.2), <, epsilon);
    DREAM3D_REQUIRED(std::abs(diamInfo[2] - 0.2), <, epsilon);

    // Test Feature Size Distribution
    VectorOfFloatArray arrays = statsData->getFeatureSizeDistribution();
    DREAM3D_REQUIRED(std::abs(arrays[0]->getValue(0) - 1.1), <, epsilon);
    DREAM3D_REQUIRED(std::abs(arrays[1]->getValue(0) - 0.1), <, epsilon);

    // Test B Over A
    arrays = statsData->getFeatureSize_BOverA();
    int binCount = arrays[0]->getNumberOfTuples();
    validateDistributionValues(arrays, binCount);

    // Test C Over A
    arrays = statsData->getFeatureSize_COverA();
    binCount = arrays[0]->getNumberOfTuples();
    validateDistributionValues(arrays, binCount);

    // Test Omegas
    arrays = statsData->getFeatureSize_Omegas();
    binCount = arrays[0]->getNumberOfTuples();
    validateDistributionValues(arrays, binCount);

    // Test Misorientation Weights
    arrays = statsData->getMDF_Weights();
    FloatArrayType::Pointer array = arrays[0]; // Angle _should_ be first based on reading the arrays in alphabetical order
    float value = array->getValue(0);
    DREAM3D_REQUIRED(std::abs(value - 45.0f), <, epsilon);
    array = arrays[1]; // Axis _should_ be next
    for(int i = 0; i < 3; i++)
    {
      value = array->getValue(i);
      DREAM3D_REQUIRED(std::abs(value - 1.0f), <, epsilon);
    }
    array = arrays[2]; // Weight should be last
    value = array->getValue(0);
    DREAM3D_REQUIRED(std::abs(value - 1000.0f), <, epsilon);

    // Test ODF
    arrays = statsData->getODF_Weights();
    std::vector<float> testValues = {45.0f, 30.0f, 60.f, 2.0f, 100000.0f};
    for(int i = 0; i < testValues.size(); i++)
    {
      array = arrays[i]; // Euler1
      value = array->getValue(0);
      DREAM3D_REQUIRED(std::abs(value - testValues[i]), <, epsilon);
    }

    // Test Axis ODF
    arrays = statsData->getAxisODF_Weights();
    for(int i = 0; i < testValues.size(); i++)
    {
      array = arrays[i]; // Euler1
      value = array->getValue(0);
      DREAM3D_REQUIRED(std::abs(value - testValues[i]), <, epsilon);
    }
  }

  /**
   * @brief BoundaryWriteTest
   */
  void BoundaryWriteTest()
  {
    BoundaryStatsData::Pointer statsData = BoundaryStatsData::New();
    statsData->setBoundaryArea(0.5f);
    statsData->setParentPhase(0.25f);
    statsData->setPhaseFraction(0.65f);

    QJsonObject jsonRoot;
    statsData->writeJson(jsonRoot);
    jsonRoot.insert(SIMPL::EnsembleData::CrystalStructures, 1);

    writeJsonFile(jsonRoot, getBoundaryStatsFileName());
  }

  /**
   * @brief BoundaryReadTest
   */
  void BoundaryReadTest()
  {
    float epsilon = 0.00001f;
    QJsonDocument doc = readJsonFile(getBoundaryStatsFileName());
    QJsonObject jsonRoot = doc.object();

    BoundaryStatsData::Pointer boundary = BoundaryStatsData::New();
    boundary->readJson(jsonRoot);
    DREAM3D_REQUIRED(std::abs(boundary->getBoundaryArea() - 0.5), <, epsilon);
    DREAM3D_REQUIRED(std::abs(boundary->getParentPhase() - 0.25), <, epsilon);
    DREAM3D_REQUIRED(std::abs(boundary->getPhaseFraction() - 0.65), <, epsilon);
  }

  /**
   * @brief MatrixWriteTest
   */
  void MatrixWriteTest()
  {
    MatrixStatsData::Pointer statsData = MatrixStatsData::New();
    statsData->setPhaseFraction(0.65f);

    QJsonObject jsonRoot;
    statsData->writeJson(jsonRoot);
    jsonRoot.insert(SIMPL::EnsembleData::CrystalStructures, 1);

    writeJsonFile(jsonRoot, getMatrixStatsFileName());
  }

  /**
   * @brief MatrixReadTest
   */
  void MatrixReadTest()
  {
    float epsilon = 0.00001f;
    QJsonDocument doc = readJsonFile(getMatrixStatsFileName());
    QJsonObject jsonRoot = doc.object();

    MatrixStatsData::Pointer statsData = MatrixStatsData::New();
    statsData->readJson(jsonRoot);

    DREAM3D_REQUIRED(std::abs(statsData->getPhaseFraction() - 0.65), <, epsilon);
  }

  /**
   * @brief PrecipitateWriteTest
   */
  void PrecipitateWriteTest()
  {
    PrecipitateStatsData::Pointer statsData = PrecipitateStatsData::New();

    // Precipitate Specific Values
    statsData->setPrecipBoundaryFraction(0.01f);

    ConfigureStatsData<PrecipitateStatsData>(statsData);

    // Write RDF Distribution Data
    std::vector<float> boxDims(3, 98.0f);
    std::vector<float> boxRes(3, 0.1f);
    std::vector<float> frequencies = RadialDistributionFunction::GenerateRandomDistribution(8, 93, 55, boxDims, boxRes);
    RdfData::Pointer rdf = RdfData::New();
    rdf->setFrequencies(frequencies);
    rdf->setBoxResolution(boxRes.data());
    rdf->setBoxSize(boxDims.data());
    rdf->setMinDistance(8.0f);
    rdf->setMaxDistance(98);
    statsData->setRadialDistFunction(rdf);

    QJsonObject jsonRoot;
    statsData->writeJson(jsonRoot);
    jsonRoot.insert(SIMPL::EnsembleData::CrystalStructures, 1);
    writeJsonFile(jsonRoot, getPrecipitateStatsFileName());
  }

  /**
   * @brief PrecipitateReadTest
   */
  void PrecipitateReadTest()
  {
    float epsilon = 0.00001f;
    QJsonDocument doc = readJsonFile(getPrecipitateStatsFileName());
    QJsonObject jsonRoot = doc.object();

    PrecipitateStatsData::Pointer statsData = PrecipitateStatsData::New();
    statsData->readJson(jsonRoot);

    ValidateStatsData<PrecipitateStatsData>(statsData);

    DREAM3D_REQUIRED(std::abs(statsData->getPrecipBoundaryFraction() - 0.01), <, epsilon);

    RdfData::Pointer rdf = statsData->getRadialDistFunction();
    float boxRes[3] = {0.0f, 0.0f, 0.0f};
    rdf->getBoxResolution(boxRes);
    float boxSize[3] = {0, 0, 0};
    rdf->getBoxSize(boxSize);
    DREAM3D_REQUIRED(std::abs(rdf->getMinDistance() - 8.0f), <, epsilon);
    DREAM3D_REQUIRED(std::abs(rdf->getMaxDistance() - 98.0f), <, epsilon);
  }

  /**
   * @brief PrimaryWriteTest
   */
  void PrimaryWriteTest()
  {
    PrimaryStatsData::Pointer statsData = PrimaryStatsData::New();

    // Primary Specific Values
    ConfigureStatsData<PrimaryStatsData>(statsData);

    FloatArrayType::Pointer bins = statsData->generateBinNumbers();
    int numTuples = bins->getNumberOfTuples();

    // Write the Neighbors Distribution
    VectorOfFloatArray arrays = StatsData::CreateCorrelatedDistributionArrays(statsData->getNeighbors_DistType(), numTuples);
    generateDistributionValues(arrays, numTuples);
    statsData->setFeatureSize_Neighbors(arrays);

    QJsonObject jsonRoot;
    statsData->writeJson(jsonRoot);
    jsonRoot.insert(SIMPL::EnsembleData::CrystalStructures, 1);

    writeJsonFile(jsonRoot, getPrimaryStatsFileName());
  }

  /**
   * @brief PrimaryReadTest
   */
  void PrimaryReadTest()
  {
    QJsonDocument doc = readJsonFile(getPrimaryStatsFileName());
    QJsonObject jsonRoot = doc.object();

    PrimaryStatsData::Pointer statsData = PrimaryStatsData::New();
    statsData->readJson(jsonRoot);

    ValidateStatsData<PrimaryStatsData>(statsData);

    // Test Neighbors Distribution
    VectorOfFloatArray arrays = statsData->getFeatureSize_Neighbors();
    size_t binCount = arrays[0]->getNumberOfTuples();
    validateDistributionValues(arrays, binCount);
  }

  /**
   * @brief TransformationWriteTest
   */
  void TransformationWriteTest()
  {
    TransformationStatsData::Pointer statsData = TransformationStatsData::New();

    // Transformation Phase Specific values
    statsData->setParentPhase(0.25);

    ConfigureStatsData<TransformationStatsData>(statsData);

    FloatArrayType::Pointer bins = statsData->generateBinNumbers();
    int numTuples = bins->getNumberOfTuples();

    // Write the Neighbors Distribution
    VectorOfFloatArray arrays = StatsData::CreateCorrelatedDistributionArrays(statsData->getNeighbors_DistType(), numTuples);
    generateDistributionValues(arrays, numTuples);
    statsData->setFeatureSize_Neighbors(arrays);

    QJsonObject jsonRoot;
    statsData->writeJson(jsonRoot);
    jsonRoot.insert(SIMPL::EnsembleData::CrystalStructures, 1);

    writeJsonFile(jsonRoot, getTransformationStatsFileName());
  }

  /**
   * @brief TransformationReadTest
   */
  void TransformationReadTest()
  {
    float epsilon = 0.00001f;
    QJsonDocument doc = readJsonFile(getTransformationStatsFileName());
    QJsonObject jsonRoot = doc.object();

    TransformationStatsData::Pointer statsData = TransformationStatsData::New();
    statsData->readJson(jsonRoot);

    ValidateStatsData<TransformationStatsData>(statsData);

    DREAM3D_REQUIRED(std::abs(statsData->getParentPhase() - 0.25), <, epsilon);

    // Test Neighbors Distribution
    VectorOfFloatArray arrays = statsData->getFeatureSize_Neighbors();
    size_t binCount = arrays[0]->getNumberOfTuples();
    validateDistributionValues(arrays, binCount);
  }

  /**
   * @brief operator () The main entry point to this test suite
   */
  void operator()()
  {
    std::cout << "#### StatsDataJsonRWTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

#if !REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif

    DREAM3D_REGISTER_TEST(BoundaryWriteTest())
    DREAM3D_REGISTER_TEST(BoundaryReadTest())

    DREAM3D_REGISTER_TEST(MatrixWriteTest())
    DREAM3D_REGISTER_TEST(MatrixReadTest())

    DREAM3D_REGISTER_TEST(PrecipitateWriteTest())
    DREAM3D_REGISTER_TEST(PrecipitateReadTest())

    DREAM3D_REGISTER_TEST(PrimaryWriteTest())
    DREAM3D_REGISTER_TEST(PrimaryReadTest())

    DREAM3D_REGISTER_TEST(TransformationWriteTest())
    DREAM3D_REGISTER_TEST(TransformationReadTest())

#if REMOVE_TEST_FILES
//   DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif
  }

private:
  StatsDataJsonRWTest(const StatsDataJsonRWTest&); // Copy Constructor Not Implemented
  void operator=(const StatsDataJsonRWTest&);      // Move assignment Not Implemented
};
