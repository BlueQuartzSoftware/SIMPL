#pragma once
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
 * SERVICES; LOSS OF USE, Data, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
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

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <QtCore/QDebug>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/CoreFilters/ConvertColorToGrayScale.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class ConvertColorToGrayScaleTest
{
  const FloatVec3Type m_defaultWeights{0.2125f, 0.7154f, 0.0721f};
  const bool m_createNewAM = false;
  const QString m_filtName = "ConvertColorToGrayScale";
  const QString m_outputArrayPrefix = "grayTestImage";
  const QString m_outputAMName = "grayTestAM";
  //  ConvertColorToGrayScale::Pointer m_colorToGrayscaleFilter;
  //  DataContainerArray::Pointer m_dca;

  enum Algorithms
  {
    LUMINOSITY,
    AVERAGE,
    LIGHTNESS,
    SINGLE_CHANNEL
  };

  const std::vector<std::vector<uint8_t>> testColors{
      {0, 0, 0},       // black
      {0, 0, 128},     // navy
      {0, 0, 255},     // blue
      {128, 0, 0},     // maroon
      {128, 0, 128},   // purple
      {255, 0, 0},     // red
      {255, 0, 255},   // fuchsia
      {0, 128, 0},     // green
      {0, 128, 128},   // teal
      {128, 128, 0},   // olive
      {128, 128, 128}, // gray
      {0, 255, 0},     // lime
      {192, 192, 192}, // silver
      {0, 255, 255},   // aqua
      {255, 255, 0},   // yellow
      {255, 255, 255}, // white
  };
  const std::vector<uint8_t> checkDefaultLuminosityColors{
      0,   // black
      9,   // navy
      18,  // blue
      27,  // maroon
      36,  // purple
      54,  // red
      73,  // fuchsia
      92,  // green
      101, // teal
      119, // olive
      128, // gray
      182, // lime
      192, // silver
      201, // aqua
      237, // yellow
      255, // white
  };
  const std::vector<uint8_t> checkCustomLuminosityColors{
      0,   // black
      96,  // navy
      191, // blue
      96,  // maroon
      192, // purple
      191, // red
      127, // fuchsia
      96,  // green
      192, // teal
      192, // olive
      32,  // gray
      191, // lime
      176, // silver
      127, // aqua
      127, // yellow
      62,  // white
  };
  const std::vector<uint8_t> checkLessZeroLuminosityColors{
      0,   // black
      160, // navy
      65,  // blue
      160, // maroon
      64,  // purple
      65,  // red
      129, // fuchsia
      160, // green
      64,  // teal
      64,  // olive
      224, // gray
      65,  // lime
      80,  // silver
      129, // aqua
      129, // yellow
      194, // white
  };
  const std::vector<uint8_t> checkGreaterOneLuminosityColors{
      0,   // black
      224, // navy
      190, // blue
      224, // maroon
      192, // purple
      190, // red
      125, // fuchsia
      224, // green
      192, // teal
      192, // olive
      160, // gray
      190, // lime
      240, // silver
      125, // aqua
      125, // yellow
      59,  // white
  };
  const std::vector<uint8_t> checkLessNegativeOneLuminosityColors{
      0,   // black
      32,  // navy
      66,  // blue
      32,  // maroon
      64,  // purple
      66,  // red
      131, // fuchsia
      32,  // green
      64,  // teal
      64,  // olive
      96,  // gray
      66,  // lime
      16,  // silver
      131, // aqua
      131, // yellow
      197, // white
  };
  const std::vector<uint8_t> checkAverageColors{
      0,   // black
      43,  // navy
      85,  // blue
      43,  // maroon
      85,  // purple
      85,  // red
      170, // fuchsia
      43,  // green
      85,  // teal
      85,  // olive
      128, // gray
      85,  // lime
      192, // silver
      170, // aqua
      170, // yellow
      255, // white
  };
  const std::vector<uint8_t> checkLightnessColors{
      0,   // black
      64,  // navy
      128, // blue
      64,  // maroon
      64,  // purple
      128, // red
      128, // fuchsia
      64,  // green
      64,  // teal
      64,  // olive
      128, // gray
      128, // lime
      192, // silver
      128, // aqua
      128, // yellow
      255, // white
  };
  const std::vector<uint8_t> checkRChannelColors{
      0,   // black
      0,   // navy
      0,   // blue
      128, // maroon
      128, // purple
      255, // red
      255, // fuchsia
      0,   // green
      0,   // teal
      128, // olive
      128, // gray
      0,   // lime
      192, // silver
      0,   // aqua
      255, // yellow
      255, // white
  };
  const std::vector<uint8_t> checkGChannelColors{
      0,   // black
      0,   // navy
      0,   // blue
      0,   // maroon
      0,   // purple
      0,   // red
      0,   // fuchsia
      128, // green
      128, // teal
      128, // olive
      128, // gray
      255, // lime
      192, // silver
      255, // aqua
      255, // yellow
      255, // white
  };
  const std::vector<uint8_t> checkBChannelColors{
      0,   // black
      128, // navy
      255, // blue
      0,   // maroon
      128, // purple
      0,   // red
      255, // fuchsia
      0,   // green
      128, // teal
      0,   // olive
      128, // gray
      0,   // lime
      192, // silver
      255, // aqua
      0,   // yellow
      255, // white
  };

  const std::vector<std::vector<uint8_t>> algorithmMap{checkDefaultLuminosityColors,
                                                       checkCustomLuminosityColors,
                                                       checkLessZeroLuminosityColors,
                                                       checkGreaterOneLuminosityColors,
                                                       checkLessNegativeOneLuminosityColors,
                                                       checkAverageColors,
                                                       checkLightnessColors,
                                                       checkRChannelColors,
                                                       checkGChannelColors,
                                                       checkBChannelColors};

  //------------------------------------------------------------------------------
  void SetDataArrayTestValues(DataArray<uint8_t>::Pointer& aa)
  {
    aa->initializeWithValue(0);
    size_t index = 0;
    for(const auto& eachColor : testColors)
    {
      aa->setTuple(index++, eachColor);
    }
  }

  //------------------------------------------------------------------------------
  static DataContainer::Pointer createVertexGeometryDataContainer(const DataArray<uint8_t>::Pointer& aa, const std::vector<size_t>& tDims)
  {
    AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, SIMPL::Defaults::VertexAttributeMatrixName, AttributeMatrix::Type::Vertex);
    am->insertOrAssign(aa);

    DataContainer::Pointer dc = DataContainer::New(SIMPL::Defaults::VertexDataContainerName);
    dc->insertOrAssign(am);
    dc->setGeometry(VertexGeom::CreateGeometry(static_cast<int64_t>(aa->getNumberOfTuples()), SIMPL::Geometry::VertexGeometry));
    return dc;
  }

  //------------------------------------------------------------------------------
  int CheckFilterParameters(ConvertColorToGrayScale::Pointer filter, const QVariant& algorithm, const FloatVec3Type& cws, const uint8_t& cc)
  {
    QVariant conversionAlgorithm{filter->property("ConversionAlgorithm")};
    FloatVec3Type colorWeights = (filter->property("ColorWeights").value<FloatVec3Type>());
    QVariant outputArrayPrefix{filter->property("OutputArrayPrefix")};
    QVariant colorChannel{filter->property("ColorChannel")};
    QVariant createNewAM{filter->property("CreateNewAttributeMatrix")};
    QVariant outputAMName{filter->property("OutputAttributeMatrixName")};

    int wrongParameters = 0;
    wrongParameters += (conversionAlgorithm == algorithm) ? 0 : 1;
    float epsilon = 0.000000001f;
    wrongParameters += (abs(colorWeights.getX() - cws.getX()) < epsilon && abs(colorWeights.getY() - cws.getY()) < epsilon && abs(colorWeights.getZ() - cws.getZ()) < epsilon) ? 0 : 1;
    wrongParameters += (outputArrayPrefix == m_outputArrayPrefix) ? 0 : 1;
    wrongParameters += (colorChannel == cc) ? 0 : 1;
    wrongParameters += (createNewAM == m_createNewAM) ? 0 : 1;
    wrongParameters += (outputAMName == m_outputAMName) ? 0 : 1;

    return wrongParameters;
  }

  //------------------------------------------------------------------------------
  int CompareResults(const uint8_t& algoMapIndex, DataContainer::Pointer dc) const
  {
    QString amName = m_outputAMName;
    if(!m_createNewAM)
    {
      amName = dc->getAttributeMatrices().front()->getName();
    }
    AttributeMatrix::Pointer am = dc->getAttributeMatrix(amName);

    UCharArrayType::Pointer testArray{};
    for(const auto& eachAAName : am->getAttributeArrayNames())
    {
      if(eachAAName.contains(m_outputArrayPrefix))
      {
        testArray = am->getAttributeArrayAs<UCharArrayType>(eachAAName);
      }
    }

    std::vector<uint8_t> colorArray{algorithmMap[algoMapIndex]};

    int wrongValues = 0;
    for(size_t index = 0; index < colorArray.size(); ++index)
    {
      wrongValues += (testArray->getValue(index) == colorArray[index]) ? 0 : 1;
    }

    return wrongValues;
  }

  //------------------------------------------------------------------------------
  int RunTest(const uint8_t& algoMapIndex, const Algorithms& algorithm, const FloatVec3Type& colorWeights, const uint8_t& colorChannel)
  {
    const QString aaName = SIMPL::VertexData::SurfaceMeshNodes;
    const std::vector<size_t> tDims{16};
    const std::vector<size_t> cDims{3, 1, 1};

    UInt8ArrayType::Pointer testAA = UInt8ArrayType::CreateArray(tDims, cDims, aaName, true);
    SetDataArrayTestValues(testAA);

    // NOTE: This filter has no geometry requirements
    DataContainer::Pointer dc = createVertexGeometryDataContainer(testAA, tDims);

    // Setup
    DataContainerArray::Pointer m_dca = DataContainerArray::New();
    m_dca->setName("TEST DATA CONTAINER ARRAY");
    m_dca->insertOrAssign(dc);
    QVector<DataArrayPath> daps;
    for(const AttributeMatrix::Pointer& eachAM : dc->getAttributeMatrices())
    {
      QString amName = eachAM->getName();
      for(const QString& eachAAName : eachAM->getAttributeArrayNames())
      {
        // Make sure not to add an output AA
        if(!eachAAName.contains(m_outputArrayPrefix))
        {
          daps.append(DataArrayPath(dc->getName(), amName, eachAAName));
        }
      }
    }

    ConvertColorToGrayScale::Pointer m_colorToGrayscaleFilter = ConvertColorToGrayScale::New();
    DREAM3D_REQUIRE(m_colorToGrayscaleFilter.get() != nullptr);

    m_colorToGrayscaleFilter->setConversionAlgorithm(algorithm);
    m_colorToGrayscaleFilter->setOutputArrayPrefix(m_outputArrayPrefix);
    m_colorToGrayscaleFilter->setColorWeights(colorWeights);
    m_colorToGrayscaleFilter->setColorChannel(colorChannel);
    m_colorToGrayscaleFilter->setCreateNewAttributeMatrix(m_createNewAM);
    m_colorToGrayscaleFilter->setOutputAttributeMatrixName(m_outputAMName);
    m_colorToGrayscaleFilter->setInputDataArrayVector(daps);
    m_colorToGrayscaleFilter->setDataContainerArray(m_dca);

    int wrongParameters = CheckFilterParameters(m_colorToGrayscaleFilter, algorithm, colorWeights, colorChannel);
    DREAM3D_REQUIRE_EQUAL(0, wrongParameters)

    // RunTest

    Observer obs;
    m_colorToGrayscaleFilter->connect(m_colorToGrayscaleFilter.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
    m_colorToGrayscaleFilter->execute();
    int erred = m_colorToGrayscaleFilter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(erred, 0);
    int wrongValues = CompareResults(algoMapIndex, dc);
    DREAM3D_REQUIRE_EQUAL(wrongValues, 0)
    return erred + wrongValues;
  }

public:
  ConvertColorToGrayScaleTest(const ConvertColorToGrayScaleTest&) = delete;            // Copy Constructor
  ConvertColorToGrayScaleTest(ConvertColorToGrayScaleTest&&) = delete;                 // Move Constructor
  ConvertColorToGrayScaleTest& operator=(const ConvertColorToGrayScaleTest&) = delete; // Copy Assignment
  ConvertColorToGrayScaleTest& operator=(ConvertColorToGrayScaleTest&&) = delete;      // Move Assignment

  ConvertColorToGrayScaleTest() = default;
  ~ConvertColorToGrayScaleTest() = default;

  //------------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ConvertColorToGrayScaleTest Starting ####" << std::endl;

    int err = 0;

    // Luminosity Algorithm testing
    // Test defaults
    qDebug() << "Testing luminosity algorithm...";
    qDebug() << "Default weights (0.2125, 0.7154, 0.0721)...";
    DREAM3D_REGISTER_TEST(RunTest(0, Algorithms::LUMINOSITY, {0.2125f, 0.7154f, 0.0721f}, 0))

    // Test custom
    FloatVec3Type colorWeights{0.75, 0.75, 0.75};
    qDebug() << "Custom weights (0.75, 0.75, 0.75)...";
    DREAM3D_REGISTER_TEST(RunTest(1, Algorithms::LUMINOSITY, colorWeights, 0))

    // Test <0
    colorWeights = {-0.75, -0.75, -0.75};
    qDebug() << "Testing weights < 0 (-0.75, -0.75, -0.75)...";
    DREAM3D_REGISTER_TEST(RunTest(2, Algorithms::LUMINOSITY, colorWeights, 0))

    // Test >1
    colorWeights = {1.75, 1.75, 1.75};
    qDebug() << "Testing weights > 1 (1.75, 1.75, 1.75)...";
    DREAM3D_REGISTER_TEST(RunTest(3, Algorithms::LUMINOSITY, colorWeights, 0))

    // Test <-1
    colorWeights = {-1.75, -1.75, -1.75};
    qDebug() << "Testing weights < -1 (-1.75, -1.75, -1.75)...";
    DREAM3D_REGISTER_TEST(RunTest(4, Algorithms::LUMINOSITY, colorWeights, 0))

    // Average Algorithm testing
    qDebug() << "Testing average algorithm...";
    DREAM3D_REGISTER_TEST(RunTest(5, Algorithms::AVERAGE, {0.2125f, 0.7154f, 0.0721f}, 0))

    // Lightness Algorithm testing
    qDebug() << "Testing lightness algorithm...";
    DREAM3D_REGISTER_TEST(RunTest(6, Algorithms::LIGHTNESS, {0.2125f, 0.7154f, 0.0721f}, 0))

    // Single Channel Algorithm testing
    // Red channel
    qDebug() << "Testing red channel algorithm...";
    DREAM3D_REGISTER_TEST(RunTest(7, Algorithms::SINGLE_CHANNEL, {0.2125f, 0.7154f, 0.0721f}, 0))

    // Green channel
    qDebug() << "Testing green channel algorithm...";
    DREAM3D_REGISTER_TEST(RunTest(8, Algorithms::SINGLE_CHANNEL, {0.2125f, 0.7154f, 0.0721f}, 1))

    // Blue channel
    qDebug() << "Testing blue channel algorithm...";
    DREAM3D_REGISTER_TEST(RunTest(9, Algorithms::SINGLE_CHANNEL, {0.2125f, 0.7154f, 0.0721f}, 2))
  }
};
