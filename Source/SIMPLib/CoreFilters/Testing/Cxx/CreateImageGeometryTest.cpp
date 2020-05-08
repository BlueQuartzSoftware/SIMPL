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
#include "SIMPLib/CoreFilters/CreateImageGeometry.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include <QVector>

class CreateImageGeometryTest
{
public:
  CreateImageGeometryTest() = default;

  ~CreateImageGeometryTest() = default;

  CreateImageGeometry::Pointer initializeData()
  {
    CreateImageGeometry::Pointer imageGeometry = CreateImageGeometry::New();

    std::vector<size_t> attributeMatrixSize = std::vector<size_t>();
    attributeMatrixSize.push_back(3);
    attributeMatrixSize.push_back(3);
    attributeMatrixSize.push_back(3);
    AttributeMatrix::Pointer am = AttributeMatrix::New(attributeMatrixSize, "Attribute Matrix", AttributeMatrix::Type::Any);

    DataContainer::Pointer dc = DataContainer::New("Data Container");
    dc->addOrReplaceAttributeMatrix(am);

    DataContainerArray::Pointer dca = DataContainerArray::New();
    dca->addOrReplaceDataContainer(dc);

    imageGeometry->setDataContainerArray(dca);
    imageGeometry->setSelectedDataContainer(DataArrayPath("Data Container", "", ""));
    return imageGeometry;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testDimensions()
  {
    CreateImageGeometry::Pointer imageGeometry = initializeData();

    IntVec3Type numDimensions;
    numDimensions[0] = 5;
    numDimensions[1] = 15;
    numDimensions[2] = 25;

    imageGeometry->setDimensions(numDimensions);

    IntVec3Type dimensions = imageGeometry->getDimensions();

    DREAM3D_REQUIRE_EQUAL(numDimensions[0], dimensions[0]);
    DREAM3D_REQUIRE_EQUAL(numDimensions[1], dimensions[1]);
    DREAM3D_REQUIRE_EQUAL(numDimensions[2], dimensions[2]);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testOrigin()
  {
    CreateImageGeometry::Pointer imageGeometry = initializeData();

    FloatVec3Type originPos;
    originPos[0] = 5;
    originPos[1] = 15;
    originPos[2] = 25;

    imageGeometry->setOrigin(originPos);

    FloatVec3Type origin = imageGeometry->getOrigin();

    DREAM3D_REQUIRE_EQUAL(originPos[0], origin[0]);
    DREAM3D_REQUIRE_EQUAL(originPos[1], origin[1]);
    DREAM3D_REQUIRE_EQUAL(originPos[2], origin[2]);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testResolution()
  {
    CreateImageGeometry::Pointer imageGeometry = initializeData();

    FloatVec3Type imgResolution;
    imgResolution[0] = 5;
    imgResolution[1] = 15;
    imgResolution[2] = 25;

    imageGeometry->setSpacing(imgResolution);

    FloatVec3Type resolution = imageGeometry->getSpacing();

    DREAM3D_REQUIRE_EQUAL(imgResolution[0], resolution[0]);
    DREAM3D_REQUIRE_EQUAL(imgResolution[1], resolution[1]);
    DREAM3D_REQUIRE_EQUAL(imgResolution[2], resolution[2]);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testExecution()
  {
    CreateImageGeometry::Pointer imageGeometry = initializeData();

    IntVec3Type numDimensions;
    numDimensions[0] = 5;
    numDimensions[1] = 15;
    numDimensions[2] = 25;
    imageGeometry->setDimensions(numDimensions);

    FloatVec3Type originPos;
    originPos[0] = 5;
    originPos[1] = 15;
    originPos[2] = 25;
    imageGeometry->setOrigin(originPos);

    FloatVec3Type imgResolution;
    imgResolution[0] = 5;
    imgResolution[1] = 15;
    imgResolution[2] = 25;
    imageGeometry->setSpacing(imgResolution);

    imageGeometry->execute();

    DataContainerArray::Pointer dca = imageGeometry->getDataContainerArray();
    DataContainer::Pointer dc = dca->getDataContainer("Data Container");
    ImageGeom::Pointer geometry = std::dynamic_pointer_cast<ImageGeom>(dc->getGeometry());

    DREAM3D_REQUIRE(nullptr != geometry.get());

    {
      FloatVec3Type res = geometry->getSpacing();

      DREAM3D_REQUIRE_EQUAL(res[0], imgResolution[0]);
      DREAM3D_REQUIRE_EQUAL(res[1], imgResolution[1]);
      DREAM3D_REQUIRE_EQUAL(res[2], imgResolution[2]);
    }

    {
      FloatVec3Type origin = geometry->getOrigin();

      DREAM3D_REQUIRE_EQUAL(origin[0], originPos[0]);
      DREAM3D_REQUIRE_EQUAL(origin[1], originPos[1]);
      DREAM3D_REQUIRE_EQUAL(origin[2], originPos[2]);
    }

    {
      SizeVec3Type dims = geometry->getDimensions();

      DREAM3D_REQUIRE_EQUAL(dims[0], numDimensions[0]);
      DREAM3D_REQUIRE_EQUAL(dims[1], numDimensions[1]);
      DREAM3D_REQUIRE_EQUAL(dims[2], numDimensions[2]);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    testDimensions();
    testOrigin();
    testResolution();

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### CreateImageGeometryTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(RunTest())
    DREAM3D_REGISTER_TEST(testDimensions())
    DREAM3D_REGISTER_TEST(testOrigin())
    DREAM3D_REGISTER_TEST(testResolution())

    DREAM3D_REGISTER_TEST(testExecution())
  }

public:
  CreateImageGeometryTest(const CreateImageGeometryTest&) = delete;            // Copy Constructor Not Implemented
  CreateImageGeometryTest(CreateImageGeometryTest&&) = delete;                 // Move Constructor Not Implemented
  CreateImageGeometryTest& operator=(const CreateImageGeometryTest&) = delete; // Copy Assignment Not Implemented
  CreateImageGeometryTest& operator=(CreateImageGeometryTest&&) = delete;      // Move Assignment Not Implemented
};
