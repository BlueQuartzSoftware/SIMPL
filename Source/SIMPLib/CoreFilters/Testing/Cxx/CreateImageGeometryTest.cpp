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

#include <QVector>

class CreateImageGeometryTest
{
public:
  CreateImageGeometryTest() = default;

  ~CreateImageGeometryTest() = default;

  CreateImageGeometry::Pointer initializeData()
  {
    CreateImageGeometry::Pointer imageGeometry = CreateImageGeometry::New();

    QVector<size_t> attributeMatrixSize = QVector<size_t>();
    attributeMatrixSize.push_back(3);
    attributeMatrixSize.push_back(3);
    attributeMatrixSize.push_back(3);
    AttributeMatrix::Pointer am = AttributeMatrix::New(attributeMatrixSize, "Attribute Matrix", AttributeMatrix::Type::Any);

    DataContainer::Pointer dc = DataContainer::New("Data Container");
    dc->addAttributeMatrix("Attribute Matrix", am);

    DataContainerArray::Pointer dca = DataContainerArray::New();
    dca->addDataContainer(dc);

    imageGeometry->setDataContainerArray(dca);
    imageGeometry->setSelectedDataContainer("Data Container");
    return imageGeometry;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testDimensions()
  {
    CreateImageGeometry::Pointer imageGeometry = initializeData();

    IntVec3_t numDimensions;
    numDimensions.x = 5;
    numDimensions.y = 15;
    numDimensions.z = 25;

    imageGeometry->setDimensions(numDimensions);

    IntVec3_t dimensions = imageGeometry->getDimensions();

    DREAM3D_REQUIRE_EQUAL(numDimensions.x, dimensions.x);
    DREAM3D_REQUIRE_EQUAL(numDimensions.y, dimensions.y);
    DREAM3D_REQUIRE_EQUAL(numDimensions.z, dimensions.z);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testOrigin()
  {
    CreateImageGeometry::Pointer imageGeometry = initializeData();

    FloatVec3_t originPos;
    originPos.x = 5;
    originPos.y = 15;
    originPos.z = 25;

    imageGeometry->setOrigin(originPos);

    FloatVec3_t origin = imageGeometry->getOrigin();

    DREAM3D_REQUIRE_EQUAL(originPos.x, origin.x);
    DREAM3D_REQUIRE_EQUAL(originPos.y, origin.y);
    DREAM3D_REQUIRE_EQUAL(originPos.z, origin.z);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testResolution()
  {
    CreateImageGeometry::Pointer imageGeometry = initializeData();

    FloatVec3_t imgResolution;
    imgResolution.x = 5;
    imgResolution.y = 15;
    imgResolution.z = 25;

    imageGeometry->setResolution(imgResolution);

    FloatVec3_t resolution = imageGeometry->getResolution();

    DREAM3D_REQUIRE_EQUAL(imgResolution.x, resolution.x);
    DREAM3D_REQUIRE_EQUAL(imgResolution.y, resolution.y);
    DREAM3D_REQUIRE_EQUAL(imgResolution.z, resolution.z);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void testExecution()
  {
    CreateImageGeometry::Pointer imageGeometry = initializeData();

    IntVec3_t numDimensions;
    numDimensions.x = 5;
    numDimensions.y = 15;
    numDimensions.z = 25;
    imageGeometry->setDimensions(numDimensions);

    FloatVec3_t originPos;
    originPos.x = 5;
    originPos.y = 15;
    originPos.z = 25;
    imageGeometry->setOrigin(originPos);

    FloatVec3_t imgResolution;
    imgResolution.x = 5;
    imgResolution.y = 15;
    imgResolution.z = 25;
    imageGeometry->setResolution(imgResolution);

    imageGeometry->execute();

    DataContainerArray::Pointer dca = imageGeometry->getDataContainerArray();
    DataContainer::Pointer dc = dca->getDataContainer("Data Container");
    ImageGeom::Pointer geometry = std::dynamic_pointer_cast<ImageGeom>(dc->getGeometry());

    DREAM3D_REQUIRE(nullptr != geometry.get());

    {
      float* res = new float[3];
      geometry->getResolution(res);

      DREAM3D_REQUIRE_EQUAL(res[0], imgResolution.x);
      DREAM3D_REQUIRE_EQUAL(res[1], imgResolution.y);
      DREAM3D_REQUIRE_EQUAL(res[2], imgResolution.z);

      delete[] res;
    }

    {
      float* origin = new float[3];
      geometry->getOrigin(origin);

      DREAM3D_REQUIRE_EQUAL(origin[0], originPos.x);
      DREAM3D_REQUIRE_EQUAL(origin[1], originPos.y);
      DREAM3D_REQUIRE_EQUAL(origin[2], originPos.z);

      delete[] origin;
    }

    {
      size_t* dims = new size_t[3];
      std::tie(dims[0], dims[1], dims[2]) = geometry->getDimensions();

      DREAM3D_REQUIRE_EQUAL(dims[0], numDimensions.x);
      DREAM3D_REQUIRE_EQUAL(dims[1], numDimensions.y);
      DREAM3D_REQUIRE_EQUAL(dims[2], numDimensions.z);

      delete[] dims;
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

private:
  CreateImageGeometryTest(const CreateImageGeometryTest&); // Copy Constructor Not Implemented
  void operator=(const CreateImageGeometryTest&);          // Move assignment Not Implemented
};
