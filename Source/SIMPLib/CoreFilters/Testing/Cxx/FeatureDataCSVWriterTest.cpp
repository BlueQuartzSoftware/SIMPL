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

#include "SIMPLib/Common/Constants.h"

class FeatureDataCSVWriterTest
{

public:
  FeatureDataCSVWriterTest() = default;
  ~FeatureDataCSVWriterTest() = default;
  FeatureDataCSVWriterTest(const FeatureDataCSVWriterTest&) = delete;            // Copy Constructor
  FeatureDataCSVWriterTest(FeatureDataCSVWriterTest&&) = delete;                 // Move Constructor
  FeatureDataCSVWriterTest& operator=(const FeatureDataCSVWriterTest&) = delete; // Copy Assignment
  FeatureDataCSVWriterTest& operator=(FeatureDataCSVWriterTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::FeatureDataCSVWriterTest::OutputFile);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FeatureDataCSVWriterTest Filter from the FilterManager
    QString filtName = "FeatureDataCSVWriter";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FeatureDataCSVWriterTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> T convertQByteArray(const QByteArray& value)
  {
    if(typeid(T) == typeid(float))
    {
      return value.toFloat();
    }
    if(typeid(T) == typeid(double))
    {
      return value.toDouble();
    }
    if(typeid(T) == typeid(short))
    {
      return value.toShort();
    }
    if(typeid(T) == typeid(int))
    {
      return value.toInt();
    }
    if(typeid(T) == typeid(long long))
    {
      return value.toLongLong();
    }
    if(typeid(T) == typeid(unsigned long long))
    {
      return value.toULongLong();
    }
    if(typeid(T) == typeid(long int))
    {
      return value.toLong();
    }
    return static_cast<T>(0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void checkDataList(AttributeMatrix::Pointer am, const QString& daName, const QList<QByteArray>& tokens, int featureID, int column)
  {
    typename DataArray<T>::Pointer dataPtr = am->getAttributeArrayAs<DataArray<T>>(daName);

    T* row = dataPtr->getTuplePointer(featureID);

    for(int j = 0; j < dataPtr->getNumberOfComponents(); j++)
    {
      QByteArray value = tokens[j + column];
      T x = convertQByteArray<T>(value);

      DREAM3D_REQUIRE(x == row[j])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void checkFilter(AttributeMatrix::Pointer am, SIMPL::DelimiterTypes::Type delimiterChoice, bool writeNumFeaturesLine, bool writeNeighborListData)
  {
    char delimiter;
    switch(delimiterChoice)
    {
    case SIMPL::DelimiterTypes::Type::Comma:
      delimiter = ',';
      break;
    case SIMPL::DelimiterTypes::Type::Semicolon:
      delimiter = ';';
      break;
    case SIMPL::DelimiterTypes::Type::Colon:
      delimiter = ':';
      break;
    case SIMPL::DelimiterTypes::Type::Tab:
      delimiter = '\t';
      break;
    case SIMPL::DelimiterTypes::Type::Space:
      delimiter = ' ';
      break;
    default:
      delimiter = ',';
      break;
    }

    QList<QString> daNames = am->getAttributeArrayNames();

    std::vector<IDataArray::Pointer> daList;
    std::vector<IDataArray::Pointer> nlList;
    int originalNumColumns = 0;

    NeighborList<int32_t>::Pointer neighborlistPtr = NeighborList<int32_t>::CreateArray(0, "_INTERNAL_USE_ONLY_JunkNeighborList", false);

    for(int i = 0; i < daNames.size(); i++)
    {
      IDataArray::Pointer dataPtr = am->getAttributeArray(daNames[i]);
      if(dataPtr->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
      {
        daList.push_back(dataPtr);
        originalNumColumns += dataPtr->getNumberOfComponents();
      }
      else
      {
        nlList.push_back(dataPtr);
      }
    }

    // Open output file

    QFile file(UnitTest::FeatureDataCSVWriterTest::OutputFile);
    bool didOpen = file.open(QIODevice::ReadOnly | QIODevice::Text);
    DREAM3D_REQUIRE(didOpen == true)

    // Parse file and check against original data

    QByteArray buf;

    // Compare number of elements

    if(writeNumFeaturesLine)
    {
      buf = file.readLine();
      buf = buf.trimmed();

      size_t numFeatures = buf.toInt();
      DREAM3D_REQUIRE(numFeatures == (am->getNumberOfTuples() - 1))
    }

    // Compare column labels

    buf = file.readLine();
    buf = buf.trimmed();

    QString labels = QString::fromStdString(buf.toStdString());
    QString originalLabels(SIMPL::FeatureData::FeatureID + delimiter);

    for(int i = 0; i < daList.size(); i++)
    {
      if(daList[i]->getNumberOfComponents() == 1)
      {
        originalLabels.append(daList[i]->getName());
      }
      else
      {
        for(int j = 0; j < daList[i]->getNumberOfComponents(); j++)
        {
          originalLabels.append(daList[i]->getName() + '_' + QString::number(j));
          if(j != (daList[i]->getNumberOfComponents() - 1))
          {
            originalLabels.append(delimiter);
          }
        }
      }
      if(i != (daList.size() - 1))
      {
        originalLabels.append(delimiter);
      }
    }

    DREAM3D_REQUIRE(labels == originalLabels)

    // Compare data by row

    // i starts at 1 since FeatureDataCSVWriter skips feature 0 in the data array

    for(size_t i = 1; i <= (am->getNumberOfTuples() - 1); i++)
    {
      buf = file.readLine();
      buf = buf.trimmed();
      QList<QByteArray> tokens = buf.split(delimiter);

      // Check the Feature_ID column

      DREAM3D_REQUIRE(tokens[0].toInt() == i)

      // Check number of columns

      int numColumns = tokens.size() - 1;

      DREAM3D_REQUIRE(numColumns == originalNumColumns)

      // Check the rest of the columns

      // The data array added last in code will be the first printed and first in daList

      checkDataList<float>(am, daList[0]->getName(), tokens, i, 1);
      checkDataList<int32_t>(am, daList[1]->getName(), tokens, i, 1 + daList[0]->getNumberOfComponents());
    }

    if(writeNeighborListData)
    {
      for(int i = 0; i < nlList.size(); i++)
      {
        // Compare column labels

        buf = file.readLine();
        buf = buf.trimmed();

        QString labels = QString::fromStdString(buf.toStdString());
        QString originalLabels(SIMPL::FeatureData::FeatureID + delimiter + SIMPL::FeatureData::NumNeighbors + delimiter + nlList[i]->getName());

        DREAM3D_REQUIRE(labels == originalLabels)

        // Compare data by row

        // i starts at 1 since FeatureDataCSVWriter skips feature 0 in the data array

        for(size_t j = 1; j <= (am->getNumberOfTuples() - 1); j++)
        {
          buf = file.readLine();
          buf = buf.trimmed();
          QList<QByteArray> tokens = buf.split(delimiter);

          // Check the Feature_ID column

          DREAM3D_REQUIRE(tokens[0].toInt() == j)

          // Check the NumNeighbors column

          Int32NeighborListType::Pointer neighborPtr = am->getAttributeArrayAs<Int32NeighborListType>(nlList[i]->getName());

          Int32NeighborListType::SharedVectorType row = neighborPtr->getList(j);

          DREAM3D_REQUIRE(tokens[1].toInt(0) == row->size())

          // Check the NeighborList

          DREAM3D_REQUIRE((tokens.size() - 2) == row->size())

          for(int k = 0; k < row->size(); k++)
          {
            DREAM3D_REQUIRE(tokens[k + 2].toInt() == row->at(k));
          }
        }
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testCase(AbstractFilter::Pointer featureDataCSVWriter, AttributeMatrix::Pointer am, SIMPL::DelimiterTypes::Type delimiterChoice, bool writeNumFeaturesLine, bool writeNeighborListData)
  {
    // Set up filter parameters

    QVariant var;

    var.setValue(writeNeighborListData);
    bool propWasSet = featureDataCSVWriter->setProperty("WriteNeighborListData", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(writeNumFeaturesLine);
    propWasSet = featureDataCSVWriter->setProperty("WriteNumFeaturesLine", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(delimiterChoice);
    propWasSet = featureDataCSVWriter->setProperty("DelimiterChoice", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run filter

    featureDataCSVWriter->preflight();
    DREAM3D_REQUIRED(featureDataCSVWriter->getErrorCondition(), >=, 0);

    featureDataCSVWriter->execute();
    DREAM3D_REQUIRED(featureDataCSVWriter->getErrorCondition(), >=, 0);

    checkFilter(am, delimiterChoice, writeNumFeaturesLine, writeNeighborListData);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestFeatureDataCSVWriterTest()
  {
    // Names and Constants

    static const QString k_FilterName("FeatureDataCSVWriter");
    static const QString k_DataContainerName("DataContainer");
    static const QString k_AttributeMatrixName("AttributeMatrix");
    static const QString k_DataArrayIntName("DataArrayInt");
    static const QString k_DataArrayFloatName("DataArrayFloat");
    static const QString k_NeighborListName("NeighborList");
    static const QString k_NeighborListName2("NeighborList2");

    static const size_t xDim(2);
    static const size_t yDim(2);
    static const size_t zDim(3);

    QVector<size_t> tupleDims = {xDim, yDim, zDim};
    QVector<size_t> cDims = {2};
    QVector<size_t> cDimsFloat = {3};
    QVector<size_t> cDimsNeighbor = {1};

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create Image Geometry

    ImageGeom::Pointer ig = ImageGeom::New();
    ig->setDimensions(xDim, yDim, zDim);
    dc->setGeometry(ig);

    // Create AttributeMatrix

    AttributeMatrix::Pointer am = AttributeMatrix::New(tupleDims, k_AttributeMatrixName, AttributeMatrix::Type::CellFeature);
    dc->addAttributeMatrix(k_AttributeMatrixName, am);

    // Create DataArrayInt (add in under checkFilter)

    DataArray<int32_t>::Pointer da = DataArray<int32_t>::CreateArray(tupleDims, cDims, k_DataArrayIntName);
    for(size_t i = 0; i < da->getSize(); i++)
    {
      da->setValue(i, i);
    }
    am->addAttributeArray(k_DataArrayIntName, da);

    // Create DataArrayFloat

    DataArray<float>::Pointer daFloat = DataArray<float>::CreateArray(tupleDims, cDimsFloat, k_DataArrayFloatName);
    for(size_t i = 0; i < daFloat->getSize(); i++)
    {
      if(i % 2 == 0)
      {
        daFloat->setValue(i, static_cast<float>(i) * 0.1);
      }
      else
      {
        daFloat->setValue(i, static_cast<float>(i) * -0.1);
      }
    }
    am->addAttributeArray(k_DataArrayFloatName, daFloat);

    // Create NeighborLists (must be int32_t)

    NeighborList<int32_t>::Pointer nl = NeighborList<int32_t>::CreateArray(tupleDims, cDimsNeighbor, k_NeighborListName);
    for(size_t i = 0; i < nl->getNumberOfTuples(); i++)
    {
      nl->addEntry(i, i);
      nl->addEntry(i, i * 2);
    }
    am->addAttributeArray(k_NeighborListName, nl);

    NeighborList<int32_t>::Pointer nl2 = NeighborList<int32_t>::CreateArray(tupleDims, cDimsNeighbor, k_NeighborListName2);
    for(size_t i = 0; i < nl2->getNumberOfTuples(); i++)
    {
      nl2->addEntry(i, i * 3);
      nl2->addEntry(i, i);
      nl2->addEntry(i, i + 1);
    }
    am->addAttributeArray(k_NeighborListName2, nl2);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(k_FilterName);
    AbstractFilter::Pointer featureDataCSVWriter = filterFactory->create();

    // Set up filter

    QVariant var;

    var.setValue(UnitTest::FeatureDataCSVWriterTest::OutputFile);
    bool propWasSet = featureDataCSVWriter->setProperty("FeatureDataFile", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(true);
    propWasSet = featureDataCSVWriter->setProperty("WriteNeighborListData", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(true);
    propWasSet = featureDataCSVWriter->setProperty("WriteNumFeaturesLine", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(SIMPL::DelimiterTypes::Type::Comma);
    propWasSet = featureDataCSVWriter->setProperty("DelimiterChoice", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    DataArrayPath dap(k_DataContainerName, k_AttributeMatrixName, k_DataArrayIntName);
    var.setValue(dap);
    propWasSet = featureDataCSVWriter->setProperty("CellFeatureAttributeMatrixPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    featureDataCSVWriter->setDataContainerArray(dca);

    // Run filter

    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Comma, true, true);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Comma, true, false);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Comma, false, true);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Comma, false, false);

    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Semicolon, true, true);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Semicolon, true, false);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Semicolon, false, true);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Semicolon, false, false);

    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Colon, true, true);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Colon, true, false);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Colon, false, true);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Colon, false, false);

    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Tab, true, true);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Tab, true, false);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Tab, false, true);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Tab, false, false);

    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Space, true, true);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Space, true, false);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Space, false, true);
    testCase(featureDataCSVWriter, am, SIMPL::DelimiterTypes::Type::Space, false, false);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#### FeatureDataCSVWriterTest Starting ####" << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestFeatureDataCSVWriterTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
};
