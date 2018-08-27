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

#include <stdlib.h>

#include <iostream>
#include <string>

#include <QtCore/QDir>
#include <QtCore/QString>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/CoreFilters/WriteASCIIData.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/DataArrays/StructArray.hpp"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

/**
 * @brief The WriteASCIIDataTest class
 */
class WriteASCIIDataTest
{
public:
  const size_t k_ArraySize = 10;

  WriteASCIIDataTest() = default;

  virtual ~WriteASCIIDataTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES

#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RunTest()
  {
    QString outputDir = UnitTest::TestTempDir;

    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer dc = DataContainer::New("DataContainer");
    AttributeMatrix::Pointer am = AttributeMatrix::New(QVector<size_t>(1, k_ArraySize), "TestAttributeMatrix", AttributeMatrix::Type::Any);

    StringDataArray::Pointer strArray = StringDataArray::CreateArray(k_ArraySize, "ASCII_Data", true);
    strArray->setValue(0, QString("Foo"));
    strArray->setValue(1, QString("Bar"));
    strArray->setValue(2, QString("Baz"));
    strArray->setValue(3, QString("DREAM3D"));
    strArray->setValue(4, QString("Sample"));
    strArray->setValue(5, QString("Something Else"));
    strArray->setValue(6, QString("Developer"));
    strArray->setValue(7, QString("Long String value with some stuff"));
    strArray->setValue(8, QString("Weird words"));
    strArray->setValue(9, QString("Sierra"));

    am->addAttributeArray(strArray->getName(), strArray);
    dc->addAttributeMatrix(am->getName(), am);
    dca->addDataContainer(dc);

    QVector<DataArrayPath> paths = {DataArrayPath("DataContainer", "TestAttributeMatrix", "ASCII_Data")};
    WriteASCIIData::Pointer writer = WriteASCIIData::New();
    writer->setDataContainerArray(dca);
    writer->setSelectedDataArrayPaths(paths);
    writer->setOutputPath(outputDir);
    writer->setDelimiter(WriteASCIIData::DelimiterType::Comma);
    writer->setFileExtension("txt");
    writer->setMaxValPerLine(1);

    writer->preflight();
    int err = writer->getErrorCondition();
    DREAM3D_REQUIRE(err >= 0)

    writer->execute();
    err = writer->getErrorCondition();
    DREAM3D_REQUIRE(err >= 0)

    NeighborList<int32_t>::Pointer neighborList = NeighborList<int32_t>::CreateArray(k_ArraySize, "NeighborList", true);
    am->addAttributeArray(neighborList->getName(), neighborList);

    paths = {DataArrayPath("DataContainer", "TestAttributeMatrix", "NeighborList")};
    writer->setSelectedDataArrayPaths(paths);
    writer->preflight();
    err = writer->getErrorCondition();
    DREAM3D_REQUIRE(err < 0)

    writer->execute();
    err = writer->getErrorCondition();
    DREAM3D_REQUIRE(err < 0)

    StructArray<int32_t>::Pointer structArray = StructArray<int32_t>::CreateArray(k_ArraySize, "StructArray", true);
    am->addAttributeArray(structArray->getName(), structArray);
    paths = {DataArrayPath("DataContainer", "TestAttributeMatrix", "StructArray")};
    writer->setSelectedDataArrayPaths(paths);
    writer->preflight();
    err = writer->getErrorCondition();
    DREAM3D_REQUIRE(err < 0)

    writer->execute();
    err = writer->getErrorCondition();
    DREAM3D_REQUIRE(err < 0)

    StatsDataArray::Pointer statsDataArray = StatsDataArray::CreateArray(k_ArraySize, "StatsDataArray", true);
    am->addAttributeArray(statsDataArray->getName(), statsDataArray);
    paths = {DataArrayPath("DataContainer", "TestAttributeMatrix", "StatsDataArray")};
    writer->setSelectedDataArrayPaths(paths);
    writer->preflight();
    err = writer->getErrorCondition();
    DREAM3D_REQUIRE(err < 0)

    writer->execute();
    err = writer->getErrorCondition();
    DREAM3D_REQUIRE(err < 0)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### WriteASCIIDataTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

#if !REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif
    DREAM3D_REGISTER_TEST(RunTest())

#if REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif
  }

private:
  WriteASCIIDataTest(const WriteASCIIDataTest&); // Copy Constructor Not Implemented
  void operator=(const WriteASCIIDataTest&);     // Move assignment Not Implemented
};
