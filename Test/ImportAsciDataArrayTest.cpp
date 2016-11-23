
#include <stdlib.h>

#include <iostream>
#include <fstream>

#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "SIMPLTestFileLocations.h"

class ImportAsciDataArrayTest
{

public:
  ImportAsciDataArrayTest()
  {
  }

  virtual ~ImportAsciDataArrayTest()
  {
  }

  std::string getOutputFile() {
    return m_FilePath;
  }

  void writeFile(char delimeter)
  {
    std::ofstream outfile;

    outfile.open(getOutputFile().c_str(), std::ios_base::binary);
    int32_t index = 0;
    for(size_t y = 0; y < m_YDim; y++)
    {
      for (size_t x = 0;x < m_XDim;x++)
      {
        outfile << index++;
        //if(x < m_XDim - 1)
        { outfile << delimeter; }
      }
      outfile << std::endl;
    }

  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest(char sep, int delimeter)
  {
    writeFile(sep);

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New();
    dc->setName("ImageDataContainer");

    ImageGeom::Pointer imageGeom = ImageGeom::New();
    imageGeom->setDimensions(m_XDim, m_YDim, m_ZDim);

    dc->setGeometry(imageGeom);
    dca->addDataContainer(dc);

    QVector<size_t> tDims = { m_XDim, m_YDim, m_ZDim };
    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, "AttributeMatrix", SIMPL::AttributeMatrixType::Generic);

    dc->addAttributeMatrix(attrMat->getName(), attrMat);

    FilterManager* fm = FilterManager::Instance();

    IFilterFactory::Pointer ff = fm->getFactoryForFilter(QString("ImportAsciDataArray"));

    AbstractFilter::Pointer filter = ff->create();

    filter->setDataContainerArray(dca);
    QVariant value;

    DataArrayPath createdAttributeArrayPath("ImageDataContainer", "AttributeMatrix", "ImportedData");
    int scalarType = 0; // int8
    int numberOfComponents = 1;
    int skipHeaderLines = 0;
    QString inputFile = QString::fromStdString(getOutputFile());

    value.setValue(createdAttributeArrayPath);
    bool propSet = filter->setProperty("CreatedAttributeArrayPath", value);
    DREAM3D_REQUIRE_EQUAL(propSet, true);

    value.setValue(scalarType);
    propSet = filter->setProperty("ScalarType", value);
    DREAM3D_REQUIRE_EQUAL(propSet, true);

    value.setValue(numberOfComponents);
    propSet = filter->setProperty("NumberOfComponents", value);
    DREAM3D_REQUIRE_EQUAL(propSet, true);

    value.setValue(skipHeaderLines);
    propSet = filter->setProperty("SkipHeaderLines", value);
    DREAM3D_REQUIRE_EQUAL(propSet, true);

    value.setValue(inputFile);
    propSet = filter->setProperty("InputFile", value);
    DREAM3D_REQUIRE_EQUAL(propSet, true);

    value.setValue(delimeter);
    propSet = filter->setProperty("Delimeter", value);
    DREAM3D_REQUIRE_EQUAL(propSet, true);

#if 0
    Observer obs;
    filter->connect(filter.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
            &obs, SLOT(processPipelineMessage(const PipelineMessage&)));
#endif
    filter->preflight();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >=, 0);

    attrMat->removeAttributeArray("ImportedData");

    filter->execute();

    DREAM3D_REQUIRED(filter->getErrorCondition(), >=, 0);

    Int8ArrayType::Pointer dataPtr = attrMat->getAttributeArrayAs<Int8ArrayType>("ImportedData");
    int8_t* ptr = dataPtr->getPointer(0);
    size_t index = 0;
    for(size_t y = 0; y < m_YDim; y++)
    {
      for (size_t x = 0;x < m_XDim;x++)
      {
        int8_t value = index;
        DREAM3D_REQUIRED(value, ==, ptr[index])
        index++;
      }
    }



    return EXIT_SUCCESS;
  }


  int Comma()
  {
      return RunTest(',', 0);
  }
  int Semicolon()
  {
      return RunTest(';', 1);
  }
  int Space()
  {
      return RunTest(' ', 2);
  }
  int Colon()
  {
      return RunTest(':', 3);
  }
  int Tab()
  {
      return RunTest('\t', 4);
  }


  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ImportAsciDataArrayTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(Comma())
    DREAM3D_REGISTER_TEST(Semicolon())
    DREAM3D_REGISTER_TEST(Space())
    DREAM3D_REGISTER_TEST(Colon())
    DREAM3D_REGISTER_TEST(Tab())
  }

private:
  size_t m_XDim = 10;
  size_t m_YDim = 5;
  size_t m_ZDim = 1;
  std::string m_FilePath = UnitTest::TestTempDir.toStdString() + "/ImportAsciDataArrayTest.txt";


  ImportAsciDataArrayTest(const ImportAsciDataArrayTest&); // Copy Constructor Not Implemented
  void operator=(const ImportAsciDataArrayTest&); // Operator '=' Not Implemented
};
