
#include <stdlib.h>

#include <fstream>
#include <iostream>

#include "SIMPLib/Common/Observer.h"

#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class ImportAsciDataArrayTest
{

public:
  ImportAsciDataArrayTest() = default;

  virtual ~ImportAsciDataArrayTest() = default;

  std::string getOutputFile()
  {
    return m_FilePath;
  }

  void writeFile(char delimiter)
  {
    std::ofstream outfile;

    outfile.open(getOutputFile().c_str(), std::ios_base::binary);
    int32_t index = 0;
    for(size_t y = 0; y < m_YDim; y++)
    {
      for(size_t x = 0; x < m_XDim; x++)
      {
        outfile << index++;
        // if(x < m_XDim - 1)
        {
          outfile << delimiter;
        }
      }
      outfile << std::endl;
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> int RunTest(char sep, int delimiter, int scalarType)
  {
    writeFile(sep);

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New("ImageDataContainer");

    ImageGeom::Pointer imageGeom = ImageGeom::New();
    imageGeom->setDimensions(m_XDim, m_YDim, m_ZDim);

    dc->setGeometry(imageGeom);
    dca->addDataContainer(dc);

    QVector<size_t> tDims = {m_XDim, m_YDim, m_ZDim};
    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, "AttributeMatrix", AttributeMatrix::Type::Generic);

    dc->addAttributeMatrix(attrMat->getName(), attrMat);

    FilterManager* fm = FilterManager::Instance();

    IFilterFactory::Pointer ff = fm->getFactoryFromClassName(QString("ImportAsciDataArray"));

    AbstractFilter::Pointer filter = ff->create();

    filter->setDataContainerArray(dca);
    QVariant value;

    DataArrayPath createdAttributeArrayPath("ImageDataContainer", "AttributeMatrix", "ImportedData");
    // int scalarType = 0; // int8
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

    value.setValue(delimiter);
    propSet = filter->setProperty("Delimiter", value);
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

    typename DataArray<T>::Pointer dataPtr = attrMat->getAttributeArrayAs<DataArray<T>>("ImportedData");
    T* ptr = dataPtr->getPointer(0);
    size_t index = 0;
    for(size_t y = 0; y < m_YDim; y++)
    {
      for(size_t x = 0; x < m_XDim; x++)
      {
        if(scalarType != 10) // Numeric Types
        {
          T value = static_cast<T>(index);
          DREAM3D_REQUIRE_EQUAL(value, ptr[index])
          index++;
        }
        else // Bool type
        {
          bool value = (index != 0);
		  //DREAM3D_REQUIRE_EQUAL(value, (ptr[index]))
          DREAM3D_REQUIRE_EQUAL(value, static_cast<bool>(ptr[index]))
          index++;
        }
      }
    }

    return EXIT_SUCCESS;
  }

  void Comma()
  {
    RunTest<int8_t>(',', 0, 0);
    RunTest<uint8_t>(',', 0, 1);
    RunTest<int16_t>(',', 0, 2);
    RunTest<uint16_t>(',', 0, 3);
    RunTest<int32_t>(',', 0, 4);
    RunTest<uint32_t>(',', 0, 5);
    RunTest<int64_t>(',', 0, 6);
    RunTest<uint64_t>(',', 0, 7);
    RunTest<float>(',', 0, 8);
    RunTest<double>(',', 0, 9);
    RunTest<bool>(',', 0, 10);
  }
  void Semicolon()
  {
    RunTest<int8_t>(';', 1, 0);
    RunTest<uint8_t>(';', 1, 1);
    RunTest<int16_t>(';', 1, 2);
    RunTest<uint16_t>(';', 1, 3);
    RunTest<int32_t>(';', 1, 4);
    RunTest<uint32_t>(';', 1, 5);
    RunTest<int64_t>(';', 1, 6);
    RunTest<uint64_t>(';', 1, 7);
    RunTest<float>(';', 1, 8);
    RunTest<double>(';', 1, 9);
    RunTest<bool>(';', 1, 10);
  }
  void Space()
  {
    RunTest<int8_t>(' ', 2, 0);
    RunTest<uint8_t>(' ', 2, 1);
    RunTest<int16_t>(' ', 2, 2);
    RunTest<uint16_t>(' ', 2, 3);
    RunTest<int32_t>(' ', 2, 4);
    RunTest<uint32_t>(' ', 2, 5);
    RunTest<int64_t>(' ', 2, 6);
    RunTest<uint64_t>(' ', 2, 7);
    RunTest<float>(' ', 2, 8);
    RunTest<double>(' ', 2, 9);
    RunTest<bool>(' ', 2, 10);
  }
  void Colon()
  {
    RunTest<int8_t>(':', 3, 0);
    RunTest<uint8_t>(':', 3, 1);
    RunTest<int16_t>(':', 3, 2);
    RunTest<uint16_t>(':', 3, 3);
    RunTest<int32_t>(':', 3, 4);
    RunTest<uint32_t>(':', 3, 5);
    RunTest<int64_t>(':', 3, 6);
    RunTest<uint64_t>(':', 3, 7);
    RunTest<float>(':', 3, 8);
    RunTest<double>(':', 3, 9);
    RunTest<bool>(':', 3, 10);
  }
  void Tab()
  {
    RunTest<int8_t>('\t', 4, 0);
    RunTest<uint8_t>('\t', 4, 1);
    RunTest<int16_t>('\t', 4, 2);
    RunTest<uint16_t>('\t', 4, 3);
    RunTest<int32_t>('\t', 4, 4);
    RunTest<uint32_t>('\t', 4, 5);
    RunTest<int64_t>('\t', 4, 6);
    RunTest<uint64_t>('\t', 4, 7);
    RunTest<float>('\t', 4, 8);
    RunTest<double>('\t', 4, 9);
    RunTest<bool>('\t', 4, 10);
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
  void operator=(const ImportAsciDataArrayTest&);          // Move assignment Not Implemented
};
