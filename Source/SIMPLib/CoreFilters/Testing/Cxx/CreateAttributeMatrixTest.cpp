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
#include <QtCore/QDir>

#include <assert.h>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/CoreFilters/CreateAttributeMatrix.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class CreateAttributeMatrixTest
{
public:
  CreateAttributeMatrixTest()
  {
  }
  virtual ~CreateAttributeMatrixTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New();
    dc->setName("DataContainer");
    dca->addDataContainer(dc);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  CreateAttributeMatrix::Pointer createFilter()
  {
    // Now instantiate the CreateAttributeMatrix Filter from the FilterManager
    QString filtName = "CreateAttributeMatrix";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The CreateAttributeMatrixTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    CreateAttributeMatrix::Pointer filter = std::dynamic_pointer_cast<CreateAttributeMatrix>(filterFactory->create());
    DREAM3D_REQUIRE(nullptr != filter.get());

    return filter;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  std::vector<std::vector<double>> createTupleDims(int num)
  {
    std::vector<double> tupleDims;

    for(int i = 0; i < num; i++)
    {
      tupleDims.push_back(i);
    }

    std::vector<std::vector<double>> tableDims;
    tableDims.push_back(tupleDims);

    return tableDims;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void setValues(CreateAttributeMatrix::Pointer filter, QString dataContainer, QString attributeMatrix, std::vector<std::vector<double>> tupleDims, AttributeMatrix::Type amType)
  {
    if(nullptr == filter.get())
    {
      return;
    }

    QVariant value;

    value.setValue(DataArrayPath(dataContainer, attributeMatrix, ""));
    filter->setProperty("CreatedAttributeMatrix", value);

    value.setValue(DynamicTableData(tupleDims));
    filter->setProperty("TupleDimensions", value);

    value.setValue(static_cast<int>(amType));
    filter->setProperty("AttributeMatrixType", value);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestMatrixTupleDims(CreateAttributeMatrix::Pointer filter, std::vector<std::vector<double>> tableDims, QString matrixName)
  {
    DataContainerArray::Pointer dca = filter->getDataContainerArray();
    DataContainer::Pointer dc = dca->getDataContainer("DataContainer");
    DREAM3D_REQUIRE(nullptr != dc.get())

    AttributeMatrix::Pointer am = dc->getAttributeMatrix(matrixName);
    DREAM3D_REQUIRE(nullptr != am.get())

    DREAM3D_REQUIRE_EQUAL(am->getTupleDimensions().size(), tableDims.at(0).size());

    for(int i = 0; i < am->getNumberOfTuples(); i++)
    {
      DREAM3D_REQUIRE_EQUAL(am->getTupleDimensions().at(i), tableDims.at(0).at(i))
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestFilter(std::vector<std::vector<double>> tableDims, AttributeMatrix::Type type)
  {
    CreateAttributeMatrix::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    QString amName = "AttributeMatrix";

    setValues(filter, "DataContainer", amName, tableDims, type);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0)
    TestMatrixTupleDims(filter, tableDims, amName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestTableDims(AttributeMatrix::Type type)
  {
    for(int i = 0; i < 3; i++)
    {
      TestFilter(createTupleDims(i), type);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestVertexMatrix()
  {
    TestTableDims(AttributeMatrix::Type::Vertex);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestEdgeMatrix()
  {
    TestTableDims(AttributeMatrix::Type::Edge);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestFaceMatrix()
  {
    TestTableDims(AttributeMatrix::Type::Face);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCellMatrix()
  {
    TestTableDims(AttributeMatrix::Type::Cell);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestVertexFeatureMatrix()
  {
    TestTableDims(AttributeMatrix::Type::VertexFeature);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestEdgeFeatureMatrix()
  {
    TestTableDims(AttributeMatrix::Type::EdgeFeature);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestFaceFeatureMatrix()
  {
    TestTableDims(AttributeMatrix::Type::FaceFeature);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCellFeatureMatrix()
  {
    TestTableDims(AttributeMatrix::Type::CellFeature);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestVertexEnsembleMatrix()
  {
    TestTableDims(AttributeMatrix::Type::VertexEnsemble);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestEdgeEnsembleMatrix()
  {
    TestTableDims(AttributeMatrix::Type::EdgeEnsemble);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestFaceEnsembleMatrix()
  {
    TestTableDims(AttributeMatrix::Type::FaceEnsemble);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCellEnsembleMatrix()
  {
    TestTableDims(AttributeMatrix::Type::CellEnsemble);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestMetaDataMatrix()
  {
    TestTableDims(AttributeMatrix::Type::MetaData);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestGenericMatrix()
  {
    TestTableDims(AttributeMatrix::Type::Generic);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestMissingDataContainer()
  {
    CreateAttributeMatrix::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    std::vector<std::vector<double>> tableDims = createTupleDims(3);

    setValues(filter, "DataContainer2", "AttributeMatrix", tableDims, AttributeMatrix::Type::Any);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -999);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestExistingAttributeMatrix()
  {
    CreateAttributeMatrix::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    std::vector<std::vector<double>> tableDims = createTupleDims(3);

    setValues(filter, "DataContainer", "AttributeMatrix", tableDims, AttributeMatrix::Type::Any);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);
    TestMatrixTupleDims(filter, tableDims, "AttributeMatrix");

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -10014);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestInvalidTableRows()
  {
    CreateAttributeMatrix::Pointer filter = createFilter();
    filter->setDataContainerArray(createDataContainerArray());

    std::vector<double> tupleDims;
    tupleDims.push_back(1);
    tupleDims.push_back(5);
    tupleDims.push_back(6);

    std::vector<std::vector<double>> tableDims;
    tableDims.push_back(tupleDims);
    tableDims.push_back(tupleDims);

    setValues(filter, "DataContainer", "AttributeMatrix", tableDims, AttributeMatrix::Type::Any);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -11000);

    tableDims.clear();
    tableDims.resize(0);

    setValues(filter, "DataContainer", "AttributeMatrix", tableDims, AttributeMatrix::Type::Any);
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -11000);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### CreateAttributeMatrixTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestVertexMatrix());
    DREAM3D_REGISTER_TEST(TestVertexFeatureMatrix());
    DREAM3D_REGISTER_TEST(TestVertexEnsembleMatrix());

    DREAM3D_REGISTER_TEST(TestEdgeMatrix());
    DREAM3D_REGISTER_TEST(TestEdgeFeatureMatrix());
    DREAM3D_REGISTER_TEST(TestEdgeEnsembleMatrix());

    DREAM3D_REGISTER_TEST(TestFaceMatrix());
    DREAM3D_REGISTER_TEST(TestFaceFeatureMatrix());
    DREAM3D_REGISTER_TEST(TestFaceEnsembleMatrix());

    DREAM3D_REGISTER_TEST(TestCellMatrix());
    DREAM3D_REGISTER_TEST(TestCellFeatureMatrix());
    DREAM3D_REGISTER_TEST(TestCellEnsembleMatrix());

    DREAM3D_REGISTER_TEST(TestMetaDataMatrix());
    DREAM3D_REGISTER_TEST(TestGenericMatrix());

    DREAM3D_REGISTER_TEST(TestMissingDataContainer());
    DREAM3D_REGISTER_TEST(TestExistingAttributeMatrix());
    DREAM3D_REGISTER_TEST(TestInvalidTableRows());
  }

private:
  CreateAttributeMatrixTest(const CreateAttributeMatrixTest&); // Copy Constructor Not Implemented
  void operator=(const CreateAttributeMatrixTest&);            // Operator '=' Not Implemented
};
