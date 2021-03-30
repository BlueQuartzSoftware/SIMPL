// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include <array>
#include <cstring>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/CoreFilters/GenerateVertexCoordinates.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class GenerateVertexCoordinatesTest
{

public:
  GenerateVertexCoordinatesTest() = default;
  ~GenerateVertexCoordinatesTest() = default;
  GenerateVertexCoordinatesTest(const GenerateVertexCoordinatesTest&) = delete;            // Copy Constructor
  GenerateVertexCoordinatesTest(GenerateVertexCoordinatesTest&&) = delete;                 // Move Constructor
  GenerateVertexCoordinatesTest& operator=(const GenerateVertexCoordinatesTest&) = delete; // Copy Assignment
  GenerateVertexCoordinatesTest& operator=(GenerateVertexCoordinatesTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES

#endif
  }

  const std::vector<size_t> k_Dims = {10ULL, 20ULL, 30ULL};

  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray()
  {
    // Create the DataContainer Array
    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create the DataContainer for the ImageGeometry
    DataContainer::Pointer imageGeomDC = DataContainer::New(k_ImageGeomDataContainerPath);
    dca->addOrReplaceDataContainer(imageGeomDC);

    // Create an ImageGeometry

    ImageGeom::Pointer imageGeom = ImageGeom::CreateGeometry("ImageGeometry");
    imageGeom->setDimensions(k_Dims.data());
    imageGeomDC->setGeometry(imageGeom);

    // Create the Cell AttributeMatrix
    AttributeMatrix::Pointer cellAttrMat = AttributeMatrix::New(k_Dims, k_CellAttrMatName, AttributeMatrix::Type::Cell);
    imageGeomDC->addOrReplaceAttributeMatrix(cellAttrMat);

    return dca;
  }

  // -----------------------------------------------------------------------------
  int RunTest()
  {
    int32_t err = 0;
    GenerateVertexCoordinates::Pointer filter = GenerateVertexCoordinates::New();

    DataContainerArray::Pointer dca = createDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->setSelectedDataContainerName({"", "", ""});
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -2004)

    DataContainer::Pointer dc = dca->getDataContainer(k_ImageGeomDataContainerPath);
    dc->setGeometry(IGeometry::NullPointer());
    filter->setSelectedDataContainerName(k_ImageGeomDataContainerPath);
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -2008)

    dca = createDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -80010)

    filter->setCoordinateArrayPath(k_CreatedCoordinatePath);
    filter->preflight();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0)

    // Reset the Data Container Array to run the filter
    dca = createDataContainerArray();
    filter->setDataContainerArray(dca);
    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0)

    FloatArrayType::Pointer coords =
        std::dynamic_pointer_cast<FloatArrayType>(dca->getDataContainer(k_ImageGeomDataContainerPath)->getAttributeMatrix(k_CellAttrMatName)->getAttributeArray(k_CreatedCoordinatePath));
    std::array<float, 3> origin = {0.0F, 0.0F, 0.0F};
    std::array<float, 3> spacing = {1.0F, 1.0F, 1.0F};
    for(size_t z = 0; z < k_Dims[2]; z++)
    {
      for(size_t y = 0; y < k_Dims[1]; y++)
      {
        for(size_t x = 0; x < k_Dims[0]; x++)
        {
          size_t idx = (z * k_Dims[1] * k_Dims[0]) + (y * k_Dims[0]) + x;
          float crds[3] = {
              x * spacing[0] + origin[0] + (0.5f * spacing[0]),
              y * spacing[1] + origin[1] + (0.5f * spacing[1]),
              z * spacing[2] + origin[2] + (0.5f * spacing[2]),
          };

          float* ptr = coords->getTuplePointer(idx);

          DREAM3D_REQUIRE_EQUAL(ptr[0], crds[0])
          DREAM3D_REQUIRE_EQUAL(ptr[1], crds[1])
          DREAM3D_REQUIRE_EQUAL(ptr[2], crds[2])
        }
      }
    }

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### GenerateVertexCoordinatesTest Starting ####" << std::endl;
    int32_t err; // needed inside the next macro.
    DREAM3D_REGISTER_TEST(RunTest())
  }

private:
  const DataArrayPath k_ImageGeomDataContainerPath = DataArrayPath("ImageGeomDataContainer", "", "");
  const QString k_CellAttrMatName = QString("CellData");
  const DataArrayPath k_CreatedCoordinatePath = DataArrayPath("ImageGeomDataContainer", "CellData", "Cell Centers");
};
