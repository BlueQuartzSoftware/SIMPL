

#include <stdlib.h>

#include <fstream>
#include <iostream>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/CoreFilters/ImportHDF5Dataset.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

#define COMPDIMPROD 72
#define TUPLEDIMPROD 40

class ImportHDF5DatasetTest
{

public:
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  ImportHDF5DatasetTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  virtual ~ImportHDF5DatasetTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  QString getOutputFile()
  {
    return m_FilePath;
  }

  // -----------------------------------------------------------------------------
  //  Uses Raw Pointers to save data to the data file
  // -----------------------------------------------------------------------------
  template <typename T> herr_t writePointer1DArrayDataset(hid_t loc_id)
  {

    T value = 0x0;
    herr_t err = 1;
    int32_t rank = 1;
    // Create the Dimensions
    hsize_t dims[1];
    dims[0] = COMPDIMPROD * TUPLEDIMPROD;

    /* Make dataset char */
    int32_t tSize = dims[0];
    // T data[dimx*dimy];
    QVector<T> data(tSize);
    for(int32_t i = 0; i < tSize; ++i)
    {
      data[i] = static_cast<T>(i * 5);
    }

    QString dsetName = QH5Lite::HDFTypeForPrimitiveAsStr(value);
    dsetName = "Pointer1DArrayDataset<" + dsetName + ">";
    err = QH5Lite::writePointerDataset(loc_id, dsetName, rank, dims, &(data.front()));
    DREAM3D_REQUIRE(err >= 0);

    return err;
  }

  // -----------------------------------------------------------------------------
  //  Uses Raw Pointers to save data to the data file
  // -----------------------------------------------------------------------------
  template <typename T> herr_t writePointer2DArrayDataset(hid_t loc_id)
  {

    T value = 0x0;
    herr_t err = 1;
    int32_t rank = 2;
    // Create the Dimensions
    hsize_t dims[2];
    dims[0] = 10;
    dims[1] = (COMPDIMPROD * TUPLEDIMPROD) / 10;

    /* Make dataset char */
    int32_t tSize = dims[0] * dims[1];
    // T data[dimx*dimy];
    QVector<T> data(tSize);
    for(int32_t i = 0; i < tSize; ++i)
    {
      data[i] = static_cast<T>(i * 5);
    }

    QString dsetName = QH5Lite::HDFTypeForPrimitiveAsStr(value);
    dsetName = "Pointer2DArrayDataset<" + dsetName + ">";
    err = QH5Lite::writePointerDataset(loc_id, dsetName, rank, dims, &(data.front()));
    DREAM3D_REQUIRE(err >= 0);

    return err;
  }

  // -----------------------------------------------------------------------------
  //  Uses Raw Pointers to save data to the data file
  // -----------------------------------------------------------------------------
  template <typename T> herr_t writePointer3DArrayDataset(hid_t loc_id)
  {

    T value = 0x0;
    herr_t err = 1;
    int32_t rank = 3;
    // Create the Dimensions
    hsize_t dims[3];
    dims[0] = 10;
    dims[1] = 8;
    dims[2] = (COMPDIMPROD * TUPLEDIMPROD) / 10 / 8;

    /* Make dataset char */
    int32_t tSize = dims[0] * dims[1] * dims[2];
    // T data[dimx*dimy];
    QVector<T> data(tSize);
    for(int32_t i = 0; i < tSize; ++i)
    {
      data[i] = static_cast<T>(i * 5);
    }

    QString dsetName = QH5Lite::HDFTypeForPrimitiveAsStr(value);
    dsetName = "Pointer3DArrayDataset<" + dsetName + ">";
    err = QH5Lite::writePointerDataset(loc_id, dsetName, rank, dims, &(data.front()));
    DREAM3D_REQUIRE(err >= 0);

    return err;
  }

  // -----------------------------------------------------------------------------
  //  Uses Raw Pointers to save data to the data file
  // -----------------------------------------------------------------------------
  template <typename T> herr_t writePointer4DArrayDataset(hid_t loc_id)
  {

    T value = 0x0;
    herr_t err = 1;
    int32_t rank = 4;
    // Create the Dimensions
    hsize_t dims[4];
    dims[0] = 10;
    dims[1] = 8;
    dims[2] = 6;
    dims[3] = (COMPDIMPROD * TUPLEDIMPROD) / 10 / 8 / 6;

    /* Make dataset char */
    int32_t tSize = dims[0] * dims[1] * dims[2] * dims[3];
    // T data[dimx*dimy];
    QVector<T> data(tSize);
    for(int32_t i = 0; i < tSize; ++i)
    {
      data[i] = static_cast<T>(i * 5);
    }

    QString dsetName = QH5Lite::HDFTypeForPrimitiveAsStr(value);
    dsetName = "Pointer4DArrayDataset<" + dsetName + ">";
    err = QH5Lite::writePointerDataset(loc_id, dsetName, rank, dims, &(data.front()));
    DREAM3D_REQUIRE(err >= 0);

    return err;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void writeHDF5File()
  {
    QFileInfo fi(m_FilePath);
    if(fi.exists())
    {
      if(QFile::remove(m_FilePath) == false)
      {
        DREAM3D_REQUIRE_EQUAL(0, 1);
      }
    }

    hid_t file_id = QH5Utilities::createFile(m_FilePath);
    DREAM3D_REQUIRE(file_id > 0);
    H5ScopedFileSentinel sentinel(&file_id, false);

    // Create the Pointer group
    hid_t ptrId = QH5Utilities::createGroup(file_id, "Pointer");
    sentinel.addGroupId(&ptrId);
    DREAM3D_REQUIRED(ptrId, >, 0);

    DREAM3D_REQUIRE(writePointer1DArrayDataset<int8_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer1DArrayDataset<uint8_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer1DArrayDataset<int16_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer1DArrayDataset<uint16_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer1DArrayDataset<int32_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer1DArrayDataset<uint32_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer1DArrayDataset<int64_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer1DArrayDataset<uint64_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer1DArrayDataset<float32>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer1DArrayDataset<float64>(ptrId) >= 0);

    DREAM3D_REQUIRE(writePointer2DArrayDataset<int8_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer2DArrayDataset<uint8_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer2DArrayDataset<int16_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer2DArrayDataset<uint16_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer2DArrayDataset<int32_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer2DArrayDataset<uint32_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer2DArrayDataset<int64_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer2DArrayDataset<uint64_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer2DArrayDataset<float32>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer2DArrayDataset<float64>(ptrId) >= 0);

    DREAM3D_REQUIRE(writePointer3DArrayDataset<int8_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer3DArrayDataset<uint8_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer3DArrayDataset<int16_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer3DArrayDataset<uint16_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer3DArrayDataset<int32_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer3DArrayDataset<uint32_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer3DArrayDataset<int64_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer3DArrayDataset<uint64_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer3DArrayDataset<float32>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer3DArrayDataset<float64>(ptrId) >= 0);

    DREAM3D_REQUIRE(writePointer4DArrayDataset<int8_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer4DArrayDataset<uint8_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer4DArrayDataset<int16_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer4DArrayDataset<uint16_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer4DArrayDataset<int32_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer4DArrayDataset<uint32_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer4DArrayDataset<int64_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer4DArrayDataset<uint64_t>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer4DArrayDataset<float32>(ptrId) >= 0);
    DREAM3D_REQUIRE(writePointer4DArrayDataset<float64>(ptrId) >= 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  ImportHDF5Dataset::Pointer createFilter()
  {
    ImportHDF5Dataset::Pointer filter = ImportHDF5Dataset::New();

    // Set a dummy data container array for error testing purposes
    DataContainerArray::Pointer dca = createDataContainerArray(QVector<size_t>(1, 1));
    filter->setDataContainerArray(dca);
    filter->setSelectedAttributeMatrix(DataArrayPath("DataContainer", "AttributeMatrix", ""));

    // Check empty file path error
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -20001);
    filter->setErrorCondition(0);

    // Check incorrect extension error
    filter->setHDF5FilePath("foo.txt");

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -20002);
    filter->setErrorCondition(0);

    // Check non-existent file error
    filter->setHDF5FilePath("foo.h5");

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -20003);
    filter->setErrorCondition(0);

    // Put in the correct file path
    filter->setHDF5FilePath(m_FilePath);

    // Check empty dataset path error
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -20004);
    filter->setErrorCondition(0);

    // Check incorrect dataset path error
    QList<ImportHDF5Dataset::DatasetImportInfo> importInfoList;
    ImportHDF5Dataset::DatasetImportInfo importInfo;
    importInfo.dataSetPath = "/Foo";
    importInfoList.push_back(importInfo);
    filter->setDatasetImportInfoList(importInfoList);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -20005);
    filter->setErrorCondition(0);

    // Fill in Dataset Path with a valid path so that we can continue our error checks
    importInfoList.clear();
    int8_t dummyVal = 0x0;
    QString typeStr = QH5Lite::HDFTypeForPrimitiveAsStr(dummyVal);
    importInfo.dataSetPath = "Pointer/Pointer1DArrayDataset<" + typeStr + ">";
    importInfoList.push_back(importInfo);
    filter->setDatasetImportInfoList(importInfoList);

    // Check empty component dimensions
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -20006);
    filter->setErrorCondition(0);

    // Check incorrect component dimensions
    importInfoList.clear();
    importInfo.componentDimensions = "(abcdg 635w";
    importInfoList.push_back(importInfo);
    filter->setDatasetImportInfoList(importInfoList);

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -20007);
    filter->setErrorCondition(0);

    return filter;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataContainerArray(QVector<size_t> tDims)
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New("DataContainer");

    dca->addDataContainer(dc);

    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, "AttributeMatrix", AttributeMatrix::Type::Generic);
    dc->addAttributeMatrix(attrMat->getName(), attrMat);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  QString createVectorString(QVector<size_t> vec)
  {
    QString str = "(";
    for(int i = 0; i < vec.size(); i++)
    {
      str.append(QString::number(vec[i]));
      if(i < vec.size() - 1)
      {
        str.append(",");
      }
    }
    str.append(")");

    return str;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void DatasetTest(ImportHDF5Dataset::Pointer filter, QList<ImportHDF5Dataset::DatasetImportInfo> importInfoList, QVector<size_t> amDims, int errCode)
  {
    if(importInfoList.empty())
    {
      return;
    }

    T value = 0x0;
    QString typeStr = QH5Lite::HDFTypeForPrimitiveAsStr(value);

    DataContainerArray::Pointer dca = createDataContainerArray(amDims);
    filter->setDataContainerArray(dca);

    QList<ImportHDF5Dataset::DatasetImportInfo> dsetInfoList = importInfoList;
    for(ImportHDF5Dataset::DatasetImportInfo& info : dsetInfoList)
    {
      info.dataSetPath = info.dataSetPath.replace("@TYPE_STRING@", typeStr);
    }

    filter->setDatasetImportInfoList(dsetInfoList);

    // Execute Dataset Test
    QString tDimsStr = createVectorString(amDims);

    if(dsetInfoList.size() > 1)
    {
      QString statusMessage = "Starting %1 Multiple Dataset Test: ";
      QString dsetPathsStr = "";
      QString cDimsVectorStr = "";
      for(int i = 0; i < dsetInfoList.size(); i++)
      {
        ImportHDF5Dataset::DatasetImportInfo info = dsetInfoList[i];
        dsetPathsStr.append(info.dataSetPath + "\n");
        QString cDimsStr = info.componentDimensions;
        cDimsVectorStr.append(cDimsStr + "\n");
      }

      statusMessage.append("Dataset Paths = \n" + dsetPathsStr);
      statusMessage.append("tDims = " + tDimsStr + "\n");
      statusMessage.append("cDims = \n" + cDimsVectorStr);

      //std::cout << statusMessage.toStdString() << std::endl;
    }
    else
    {
      ImportHDF5Dataset::DatasetImportInfo info = dsetInfoList[0];
//      std::cout << QObject::tr("Starting %1 Dataset Test: Dataset Path = %2, tDims = %3, cDims = %4").arg(typeStr).arg(info.dataSetPath).arg(tDimsStr).arg(info.componentDimensions).toStdString()
//                << std::endl;
    }

    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), errCode);

    // If we got through without errors, validate the results
    if(errCode == 0)
    {
      for(int i = 0; i < dsetInfoList.size(); i++)
      {
        // Calculate the total number of tuples
        size_t tDimsProduct = 1;
        for(int i = 0; i < amDims.size(); i++)
        {
          tDimsProduct = tDimsProduct * amDims[i];
        }

        QString cDimsStr = dsetInfoList[i].componentDimensions;
        QStringList tokens = cDimsStr.split(", ");
        QVector<size_t> cDims;
        for(int i = 0; i < tokens.size(); i++)
        {
          cDims.push_back(tokens[i].toInt());
        }

        // Calculate the total number of components
        size_t cDimsProduct = 1;
        for(int i = 0; i < cDims.size(); i++)
        {
          cDimsProduct = cDimsProduct * cDims[i];
        }

        QString dsetPath = dsetInfoList[i].dataSetPath;
        QString dsetName = dsetPath.remove("/Pointer/");
        typename DataArray<T>::Pointer da = dca->getPrereqIDataArrayFromPath<DataArray<T>, AbstractFilter>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", dsetName));
        size_t totalArrayValues = da->getNumberOfTuples() * da->getNumberOfComponents();
        DREAM3D_REQUIRE_EQUAL(totalArrayValues, tDimsProduct * cDimsProduct);

        T* daPtr = da->getPointer(0);
        for(size_t i = 0; i < tDimsProduct * cDimsProduct; ++i)
        {
          T value = daPtr[i];
          DREAM3D_REQUIRE_EQUAL(value, static_cast<T>(i * 5));
        }
      }
    }

    // std::cout << "Test Complete!" << std::endl << std::endl;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    bool success = QFile::remove(m_FilePath);
    DREAM3D_REQUIRE_EQUAL(success, true);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> QString joinVector(QVector<T> vector, const QString& separator)
  {
    QString cDimsStr = "";
    for(int i = 0; i < vector.size(); i++)
    {
      cDimsStr.append(QString::number(vector[i]));
      if(i < vector.size() - 1)
      {
        cDimsStr.append(QObject::tr(" %1 ").arg(separator));
      }
    }

    return cDimsStr;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RunImportHDF5DatasetTest()
  {
    writeHDF5File();

    //  // ******************* Test Reading Data *************************************

    // Create tuple and component dimensions for all tests
    QVector<QVector<size_t>> tDimsVector;
    QVector<QVector<size_t>> cDimsVector;

    // Add 1D, 2D, 3D, and 4D tuple and component dimensions that test all 4 possibilities:
    // 1. Tuple dimensions and component dimensions are both valid
    // 2. Tuple dimensions are valid, but component dimensions are invalid
    // 3. Tuple dimensions are invalid, but component dimensions are valid
    // 4. Neither tuple dimensions or component dimensions are valid

    tDimsVector.push_back(QVector<size_t>(1) = {TUPLEDIMPROD});
    cDimsVector.push_back(QVector<size_t>(1) = {COMPDIMPROD});

    tDimsVector.push_back(QVector<size_t>(2) = {10, 4});
    cDimsVector.push_back(QVector<size_t>(2) = {12, 6});

    tDimsVector.push_back(QVector<size_t>(3) = {2, 2, 10});
    cDimsVector.push_back(QVector<size_t>(3) = {4, 3, 6});

    tDimsVector.push_back(QVector<size_t>(4) = {2, 2, 5, 2});
    cDimsVector.push_back(QVector<size_t>(4) = {4, 3, 3, 2});

    tDimsVector.push_back(QVector<size_t>(1) = {TUPLEDIMPROD - 1});
    cDimsVector.push_back(QVector<size_t>(1) = {COMPDIMPROD - 1});

    tDimsVector.push_back(QVector<size_t>(2) = {TUPLEDIMPROD - 1, 34});
    cDimsVector.push_back(QVector<size_t>(2) = {COMPDIMPROD - 1, 56});

    tDimsVector.push_back(QVector<size_t>(3) = {TUPLEDIMPROD - 1, 23, 654});
    cDimsVector.push_back(QVector<size_t>(3) = {COMPDIMPROD - 1, 56, 12});

    tDimsVector.push_back(QVector<size_t>(4) = {TUPLEDIMPROD - 1, 98, 12, 45});
    cDimsVector.push_back(QVector<size_t>(4) = {COMPDIMPROD - 1, 43, 12, 53});

    // Execute all combinations of tests
    for(int i = 0; i < tDimsVector.size(); i++)
    {
      for(int j = 0; j < cDimsVector.size(); j++)
      {
        QVector<size_t> tDims = tDimsVector[i];
        QVector<size_t> cDims = cDimsVector[j];

        size_t amTupleCount = 1;
        for(int t = 0; t < tDims.size(); t++)
        {
          amTupleCount *= tDims[t];
        }

        size_t cDimsProd = 1;
        for(int c = 0; c < cDims.size(); c++)
        {
          cDimsProd *= cDims[c];
        }

        // Figure out our error code based on the dimensions coming in
        int errCode = 0;
        if(TUPLEDIMPROD * COMPDIMPROD != amTupleCount * cDimsProd)
        {
          errCode = -20008;
        }

        ImportHDF5Dataset::Pointer filter = createFilter();

        QList<ImportHDF5Dataset::DatasetImportInfo> importInfoList;
        ImportHDF5Dataset::DatasetImportInfo info;
        info.componentDimensions = joinVector(cDims, ", ");

        QStringList dsetPaths;
        dsetPaths.push_back("/Pointer/Pointer1DArrayDataset<@TYPE_STRING@>");
        dsetPaths.push_back("/Pointer/Pointer2DArrayDataset<@TYPE_STRING@>");
        dsetPaths.push_back("/Pointer/Pointer3DArrayDataset<@TYPE_STRING@>");
        dsetPaths.push_back("/Pointer/Pointer4DArrayDataset<@TYPE_STRING@>");

        // Run 1D Array Tests
        info.dataSetPath = dsetPaths[0];
        importInfoList.push_back(info);
        DatasetTest<int8_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint8_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int16_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint16_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int32_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint32_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int64_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint64_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<float32>(filter, importInfoList, tDims, errCode);
        DatasetTest<float64>(filter, importInfoList, tDims, errCode);

        importInfoList.clear();

        // Run 2D Array Tests
        info.dataSetPath = dsetPaths[1];
        importInfoList.push_back(info);
        DatasetTest<int8_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint8_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int16_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint16_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int32_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint32_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int64_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint64_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<float32>(filter, importInfoList, tDims, errCode);
        DatasetTest<float64>(filter, importInfoList, tDims, errCode);

        importInfoList.clear();

        // Run 3D Array Tests
        info.dataSetPath = dsetPaths[2];
        importInfoList.push_back(info);
        DatasetTest<int8_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint8_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int16_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint16_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int32_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint32_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int64_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint64_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<float32>(filter, importInfoList, tDims, errCode);
        DatasetTest<float64>(filter, importInfoList, tDims, errCode);

        importInfoList.clear();

        // Run 4D Array Tests
        info.dataSetPath = dsetPaths[3];
        importInfoList.push_back(info);
        DatasetTest<int8_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint8_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int16_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint16_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int32_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint32_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int64_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint64_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<float32>(filter, importInfoList, tDims, errCode);
        DatasetTest<float64>(filter, importInfoList, tDims, errCode);

        importInfoList.clear();

        // Test every possible set of 2 datasets
        for(int a = 0; a < dsetPaths.size(); a++)
        {
          for(int b = a + 1; b < dsetPaths.size(); b++)
          {
            info.dataSetPath = dsetPaths[a];
            importInfoList.push_back(info);
            info.dataSetPath = dsetPaths[b];
            importInfoList.push_back(info);

            DatasetTest<int8_t>(filter, importInfoList, tDims, errCode);
            DatasetTest<uint8_t>(filter, importInfoList, tDims, errCode);
            DatasetTest<int16_t>(filter, importInfoList, tDims, errCode);
            DatasetTest<uint16_t>(filter, importInfoList, tDims, errCode);
            DatasetTest<int32_t>(filter, importInfoList, tDims, errCode);
            DatasetTest<uint32_t>(filter, importInfoList, tDims, errCode);
            DatasetTest<int64_t>(filter, importInfoList, tDims, errCode);
            DatasetTest<uint64_t>(filter, importInfoList, tDims, errCode);
            DatasetTest<float32>(filter, importInfoList, tDims, errCode);
            DatasetTest<float64>(filter, importInfoList, tDims, errCode);

            importInfoList.clear();
          }
        }

        // Test every possible set of 3 datasets
        for(int a = 0; a < dsetPaths.size(); a++)
        {
          for(int b = a + 1; b < dsetPaths.size(); b++)
          {
            for(int c = b + 1; c < dsetPaths.size(); c++)
            {
              info.dataSetPath = dsetPaths[a];
              importInfoList.push_back(info);
              info.dataSetPath = dsetPaths[b];
              importInfoList.push_back(info);
              info.dataSetPath = dsetPaths[c];
              importInfoList.push_back(info);

              DatasetTest<int8_t>(filter, importInfoList, tDims, errCode);
              DatasetTest<uint8_t>(filter, importInfoList, tDims, errCode);
              DatasetTest<int16_t>(filter, importInfoList, tDims, errCode);
              DatasetTest<uint16_t>(filter, importInfoList, tDims, errCode);
              DatasetTest<int32_t>(filter, importInfoList, tDims, errCode);
              DatasetTest<uint32_t>(filter, importInfoList, tDims, errCode);
              DatasetTest<int64_t>(filter, importInfoList, tDims, errCode);
              DatasetTest<uint64_t>(filter, importInfoList, tDims, errCode);
              DatasetTest<float32>(filter, importInfoList, tDims, errCode);
              DatasetTest<float64>(filter, importInfoList, tDims, errCode);

              importInfoList.clear();
            }
          }
        }

        importInfoList.clear();

        // Test the set of 4 datasets
        info.dataSetPath = dsetPaths[0];
        importInfoList.push_back(info);
        info.dataSetPath = dsetPaths[1];
        importInfoList.push_back(info);
        info.dataSetPath = dsetPaths[2];
        importInfoList.push_back(info);
        info.dataSetPath = dsetPaths[3];
        importInfoList.push_back(info);

        DatasetTest<int8_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint8_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int16_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint16_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int32_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint32_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<int64_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<uint64_t>(filter, importInfoList, tDims, errCode);
        DatasetTest<float32>(filter, importInfoList, tDims, errCode);
        DatasetTest<float64>(filter, importInfoList, tDims, errCode);
      }
    }

    QFileInfo fi(m_FilePath);
    if(fi.exists())
    {
      if(QFile::remove(m_FilePath) == false)
      {
        DREAM3D_REQUIRE_EQUAL(0, 1);
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ImportHDF5DatasetTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    //#if !REMOVE_TEST_FILES
    //    DREAM3D_REGISTER_TEST(RemoveTestFiles())
    //#endif

    DREAM3D_REGISTER_TEST(RunImportHDF5DatasetTest())

    //#if REMOVE_TEST_FILES
    //    DREAM3D_REGISTER_TEST(RemoveTestFiles())
    //#endif
  }

private:
  QString m_FilePath = UnitTest::TestTempDir + "/ImportHDF5DatasetTest.h5";

  ImportHDF5DatasetTest(const ImportHDF5DatasetTest&); // Copy Constructor Not Implemented
  void operator=(const ImportHDF5DatasetTest&);        // Move assignment Not Implemented
};
