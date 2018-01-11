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

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QObject>
#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

#include "SIMPLib/CoreFilters/GenerateColorTable.h"

class GenerateColorTableTest
{

public:
  GenerateColorTableTest()
  {
  }
  virtual ~GenerateColorTableTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the ImportAsciDataArray Filter from the FilterManager
    {
      QString filtName = "ImportAsciDataArray";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
      if(nullptr == filterFactory.get())
      {
        std::stringstream ss;
        ss << "The GenerateColorTableTest Requires the use of the " << filtName.toStdString() << " filter which is found in CoreFilters.";
        DREAM3D_TEST_THROW_EXCEPTION(ss.str())
      }
    }

    // Now instantiate the GenerateColorTable Filter from the FilterManager
    {
      QString filtName = "GenerateColorTable";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
      if(nullptr == filterFactory.get())
      {
        std::stringstream ss;
        ss << "The GenerateColorTableTest Requires the use of the " << filtName.toStdString() << " filter which is found in CoreFilters";
        DREAM3D_TEST_THROW_EXCEPTION(ss.str())
      }
    }

    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void ReadPresets()
  {
    QFile inputFile(":/CoreResources/ColorTablePresets/ColorTablePresets.json");
    if(inputFile.open(QIODevice::ReadOnly))
    {
      QJsonParseError parseError;
      QByteArray byteArray = inputFile.readAll();
      QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
      if(parseError.error != QJsonParseError::NoError)
      {
        // "Failed to parse presets file" error
        DREAM3D_REQUIRE_EQUAL(0, -1);
      }
      QJsonArray presetArray = doc.array();

      for(int i = 0; i < presetArray.size(); i++)
      {
        QJsonObject preset = presetArray[i].toObject();
        if(preset["ColorSpace"].toString() != "RGB")
        {
          presetArray.removeAt(i);
          i--;
        }
      }

      for(int i = 0; i < presetArray.size(); i++)
      {
        QJsonObject preset = presetArray[i].toObject();
        if(preset.contains("Name") && preset.contains("RGBPoints") && preset["Name"].isString() && preset["RGBPoints"].isArray())
        {
          m_PresetMap.insert(preset["Name"].toString(), preset["RGBPoints"].toArray());
        }
      }
    }
    else
    {
      // "Failed to open presets file" error
      DREAM3D_REQUIRE_EQUAL(0, -1);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CheckPreset(QString presetName, QString presetFilePath, DataContainerArray::Pointer dca)
  {
    // Apply Preset
    {
      GenerateColorTable::Pointer filter = GenerateColorTable::New();
      filter->setRGB_ArrayName("CI_RGB");
      filter->setSelectedDataArrayPath(DataArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::ConfidenceIndex));
      filter->setSelectedPresetName(presetName);

      QJsonArray presetPoints = m_PresetMap.value(filter->getSelectedPresetName());
      DREAM3D_REQUIRE(presetPoints.size() > 0)

      filter->setSelectedPresetControlPoints(presetPoints);

      filter->setDataContainerArray(dca);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0)
    }

    // Validate Results
    {
      DataArrayPath daPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "CI_RGB");
      UInt8ArrayType::Pointer da = dca->getPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(nullptr, daPath, QVector<size_t>(1, 3));
      if(da.get() != nullptr)
      {
        QFile file(presetFilePath);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
          QTextStream in(&file);
          int currentLine = 0;
          while(!in.atEnd())
          {
            QString line = in.readLine();
            QStringList list = line.split(',');
            for(int i = 0; i < list.size(); i++)
            {
              bool ok = false;
              int exemplar = list[i].toInt(&ok);
              int generated = da->getComponent(currentLine, i);
              DREAM3D_REQUIRE_EQUAL(ok, true)
              DREAM3D_REQUIRE_EQUAL(exemplar, generated)
            }
            currentLine++;
          }
        }
      }
    }

    DataArrayPath amPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "");
    AttributeMatrix::Pointer am = dca->getPrereqAttributeMatrixFromPath<AbstractFilter>(nullptr, amPath, -300);
    am->removeAttributeArray("CI_RGB");
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestGenerateColorTable()
  {
    ReadPresets();

    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer dc = DataContainer::New(SIMPL::Defaults::ImageDataContainerName);
    AttributeMatrix::Pointer am = AttributeMatrix::New(QVector<size_t>(1, 37989), SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Generic);
    dc->addAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName, am);
    dca->addDataContainer(dc);

    // Read Image File
    {
      QString filtName = "ImportAsciDataArray";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filtName);
      DREAM3D_REQUIRE(factory.get() != nullptr);

      AbstractFilter::Pointer filter = factory->create();
      DREAM3D_REQUIRE(filter.get() != nullptr);

      QVariant var;
      bool propWasSet = false;

      var.setValue(UnitTest::GenerateColorTableTest::ImageFilePath);
      propWasSet = filter->setProperty("InputFile", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(SIMPL::NumericTypes::Type::Float);
      propWasSet = filter->setProperty("ScalarType", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(1);
      propWasSet = filter->setProperty("NumberOfComponents", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(DataArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::ConfidenceIndex));
      propWasSet = filter->setProperty("CreatedAttributeArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      filter->setDataContainerArray(dca);
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0)
    }

    CheckPreset("Black, Blue and White", UnitTest::GenerateColorTableTest::BlackBlueWhiteFile, dca);

    CheckPreset("Black, Orange and White", UnitTest::GenerateColorTableTest::BlackOrangeWhiteFile, dca);

    CheckPreset("Black-Body Radiation", UnitTest::GenerateColorTableTest::BlackBodyRadiationFile, dca);

    CheckPreset("Blue to Yellow", UnitTest::GenerateColorTableTest::BlueToYellowFile, dca);

    CheckPreset("Cold and Hot", UnitTest::GenerateColorTableTest::ColdAndHotFile, dca);

    CheckPreset("Grayscale", UnitTest::GenerateColorTableTest::GrayscaleFile, dca);

    CheckPreset("Haze", UnitTest::GenerateColorTableTest::HazeFile, dca);

    CheckPreset("hsv", UnitTest::GenerateColorTableTest::HsvFile, dca);

    CheckPreset("jet", UnitTest::GenerateColorTableTest::JetFile, dca);

    CheckPreset("Rainbow Blended Black", UnitTest::GenerateColorTableTest::RainbowBlendedBlackFile, dca);

    CheckPreset("Rainbow Blended Grey", UnitTest::GenerateColorTableTest::RainbowBlendedGreyFile, dca);

    CheckPreset("Rainbow Blended White", UnitTest::GenerateColorTableTest::RainbowBlendedWhiteFile, dca);

    CheckPreset("Rainbow Desaturated", UnitTest::GenerateColorTableTest::RainbowDesaturatedFile, dca);

    CheckPreset("rainbow", UnitTest::GenerateColorTableTest::RainbowFile, dca);

    CheckPreset("X Ray", UnitTest::GenerateColorTableTest::XRayFile, dca);

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### GenerateColorTableTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestGenerateColorTable())
  }

private:
  QMap<QString, QJsonArray> m_PresetMap;

  GenerateColorTableTest(const GenerateColorTableTest&); // Copy Constructor Not Implemented
  void operator=(const GenerateColorTableTest&);         // Operator '=' Not Implemented
};
