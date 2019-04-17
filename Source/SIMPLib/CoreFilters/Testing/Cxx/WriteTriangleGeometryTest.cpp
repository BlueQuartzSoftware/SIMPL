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

#include "SIMPLib/Geometry/TriangleGeom.h"

#include "SIMPLib/Common/Constants.h"

class WriteTriangleGeometryTest
{

public:
  WriteTriangleGeometryTest() = default;
  ~WriteTriangleGeometryTest() = default;
  WriteTriangleGeometryTest(const WriteTriangleGeometryTest&) = delete;            // Copy Constructor
  WriteTriangleGeometryTest(WriteTriangleGeometryTest&&) = delete;                 // Move Constructor
  WriteTriangleGeometryTest& operator=(const WriteTriangleGeometryTest&) = delete; // Copy Assignment
  WriteTriangleGeometryTest& operator=(WriteTriangleGeometryTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::WriteTriangleGeometryTest::NodesFile);
    QFile::remove(UnitTest::WriteTriangleGeometryTest::TrianglesFile);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the WriteTriangleGeometryTest Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The WriteTriangleGeometryTest Requires the use of the " << m_FilterName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestWriteTriangleGeometryTest()
  {
    // Names and Constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_TriVertexListDAName("TriVertexList");
    static const QString k_TriListDAName("TriangleList");

    QVector<size_t> k_Dims3(1, 3);
    QVector<size_t> k_NumNodes(1, 99);
    QVector<size_t> k_NumTriangles(1, 33);

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addDataContainer(dc);

    // Create Triangle Data Arrays

    FloatArrayType::Pointer daTriVert = FloatArrayType::CreateArray(k_NumNodes, k_Dims3, k_TriVertexListDAName);
    for (size_t i = 0; i < daTriVert->getNumberOfTuples(); i++)
    {
      for (int j = 0; j < daTriVert->getNumberOfComponents(); j++)
      {
        float value(static_cast<float>(i + j));
        if (i % 2 != 0)
        {
          value *= -1.0f;
        }
        if (j > 0)
        {
          value += (j + 1) * 0.111f;
        }
        daTriVert->setComponent(i, j, value);
      }
    }

    Int64ArrayType::Pointer daTriList = Int64ArrayType::CreateArray(k_NumTriangles, k_Dims3, k_TriListDAName);
    for (size_t i = 0; i < daTriList->getNumberOfTuples(); i++)
    {
      for (int j = 0; j < daTriList->getNumberOfComponents(); j++)
      {
        daTriList->setComponent(i, j, i + j);
      }
    }

    TriangleGeom::Pointer triGeom = TriangleGeom::CreateGeometry(daTriList, daTriVert, SIMPL::Geometry::TriangleGeometry);

    dc->setGeometry(triGeom);

    // Create Filter

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer writeTriangleGeometry = filterFactory->create();

    writeTriangleGeometry->setDataContainerArray(dca);

    // Set up filter

    QVariant var;

    var.setValue(k_DataContainerName);
    bool propWasSet = writeTriangleGeometry->setProperty("DataContainerSelection", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(UnitTest::WriteTriangleGeometryTest::NodesFile);
    propWasSet = writeTriangleGeometry->setProperty("OutputNodesFile", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(UnitTest::WriteTriangleGeometryTest::TrianglesFile);
    propWasSet = writeTriangleGeometry->setProperty("OutputTrianglesFile", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Run filter

    writeTriangleGeometry->execute();
    DREAM3D_REQUIRED(writeTriangleGeometry->getErrorCondition(), >=, 0);

    // Test filter

    // Check nodes file

    QFile fileNodes(UnitTest::WriteTriangleGeometryTest::NodesFile);
    bool didOpen = fileNodes.open(QIODevice::ReadOnly | QIODevice::Text);
    DREAM3D_REQUIRE(didOpen == true)

    QTextStream inFileNodes(&fileNodes);

    QString line("");

    // Check header lines

    QVector<QString> headers = {"# All lines starting with '#' are comments", "# DREAM.3D Nodes file", "# DREAM.3D Version " + SIMPLib::Version::Complete(), "# Node Data is X Y Z space delimited."};

    for(int i = 0; i < headers.size(); i++)
    {
      line = inFileNodes.readLine();
      line = line.trimmed();
      DREAM3D_REQUIRE(line == headers[i])
    }

    // Check node count

    line = inFileNodes.readLine();
    QStringList list = line.split(':');
    DREAM3D_REQUIRE(list[0].trimmed() == "Node Count")

    qlonglong nodeCount = list[1].trimmed().toLongLong();
    DREAM3D_REQUIRE(nodeCount == triGeom->getNumberOfVertices())

    // Check vertices

    for(int64_t i = 0; i < triGeom->getNumberOfVertices(); i++)
    {
      line = inFileNodes.readLine();
      list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
      for(int j = 0; j < daTriVert->getNumberOfComponents(); j++)
      {
        DREAM3D_REQUIRE(list[j] == QString::number(daTriVert->getComponent(i, j), 'f', 5))
      }
    }

    fileNodes.close();

    // Check triangles file

    QFile fileTriangles(UnitTest::WriteTriangleGeometryTest::TrianglesFile);
    didOpen = fileTriangles.open(QIODevice::ReadOnly | QIODevice::Text);
    DREAM3D_REQUIRE(didOpen == true)

    QTextStream inFileTriangles(&fileTriangles);

    // Check header lines

    headers = {"# All lines starting with '#' are comments", "# DREAM.3D Triangle file", "# DREAM.3D Version " + SIMPLib::Version::Complete(), "# Each Triangle consists of 3 Node Ids.",
               "# NODE IDs START AT 0."};

    for(int i = 0; i < headers.size(); i++)
    {
      line = inFileTriangles.readLine();
      line = line.trimmed();
      DREAM3D_REQUIRE(line == headers[i])
    }

    // Check geometry type

    line = inFileTriangles.readLine();
    QString geomType = "Geometry Type: " + triGeom->getGeometryTypeAsString();
    DREAM3D_REQUIRE(line == geomType)

    // Check node count

    line = inFileTriangles.readLine();
    list = line.split(':');
    DREAM3D_REQUIRE(list[0].trimmed() == "Node Count")

    nodeCount = list[1].trimmed().toLongLong();
    DREAM3D_REQUIRE(nodeCount == triGeom->getNumberOfVertices())

    // Check max node ID

    line = inFileTriangles.readLine();
    list = line.split(':');
    DREAM3D_REQUIRE(list[0].trimmed() == "Max Node Id")

    qlonglong maxNodeID = list[1].trimmed().toLongLong();
    DREAM3D_REQUIRE(maxNodeID == (triGeom->getNumberOfVertices() - 1))

    // Check triangle count

    line = inFileTriangles.readLine();
    list = line.split(':');
    DREAM3D_REQUIRE(list[0].trimmed() == "Triangle Count")

    qlonglong triangleCount = list[1].trimmed().toLongLong();
    DREAM3D_REQUIRE(triangleCount == triGeom->getNumberOfTris())

    // Check triangles

    for(int64_t i = 0; i < triGeom->getNumberOfTris(); i++)
    {
      line = inFileTriangles.readLine();
      list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
      for(int j = 0; j < daTriList->getNumberOfComponents(); j++)
      {
        DREAM3D_REQUIRE(list[j].toLongLong() == daTriList->getComponent(i, j))
      }
    }

    fileTriangles.close();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#### WriteTriangleGeometryTest Starting ####" << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestWriteTriangleGeometryTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  QString m_FilterName = QString("WriteTriangleGeometry");
};
