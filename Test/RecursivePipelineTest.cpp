///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <QtCore/QObject>

#include "SIMPLib/CoreFilters/RemoveArrays.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "SIMPLTestFileLocations.h"

class RecursivePipelineTest
{
  public:
    RecursivePipelineTest()
    {
    }
    virtual ~RecursivePipelineTest()
    {
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    int TestFilterAvailability()
    {
      // Now instantiate the FindDifferenceMapTest Filter from the FilterManager
      QString filtName = "CombineAttributeMatrices";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
      if(nullptr == filterFactory.get())
      {
        std::stringstream ss;
        ss << "The CombineAttribtueMatricesTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
        DREAM3D_TEST_THROW_EXCEPTION(ss.str())
      }
      return 0;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    DataContainerArray::Pointer createDataContainerArray()
    {
      DataContainerArray::Pointer dca = DataContainerArray::New();

      //  DataContainer::Pointer dc1 = DataContainer::New("DataContainer1");
      //  DataContainer::Pointer dc2 = DataContainer::New("DataContainer2");
      //  DataContainer::Pointer dc3 = DataContainer::New("DataContainer3");
      //  AttributeMatrix::Pointer am1 = AttributeMatrix::New(QVector<size_t>(3, 2), "AttributeMatrix1", 0);
      //  AttributeMatrix::Pointer am2 = AttributeMatrix::New(QVector<size_t>(7, 2), "AttributeMatrix2", 0);
      //  AttributeMatrix::Pointer am3 = AttributeMatrix::New(QVector<size_t>(4, 3), "AttributeMatrix3", 0);
      //  AttributeMatrix::Pointer am4 = AttributeMatrix::New(QVector<size_t>(7, 2), "AttributeMatrix4", 0);
      //  AttributeMatrix::Pointer am5 = AttributeMatrix::New(QVector<size_t>(7, 2), "AttributeMatrix5", 0);
      //  IDataArray::Pointer da1 = DataArray<size_t>::CreateArray(8, "DataArray1");
      //  IDataArray::Pointer da2 = DataArray<size_t>::CreateArray(128, "DataArray2");
      //  IDataArray::Pointer da3 = DataArray<size_t>::CreateArray(128, "DataArray3");
      //  IDataArray::Pointer da4 = DataArray<size_t>::CreateArray(81, "DataArray4");
      //  IDataArray::Pointer da5 = DataArray<size_t>::CreateArray(81, "DataArray5");

      //  am1->addAttributeArray("DataArray1", da1);
      //  am2->addAttributeArray("DataArray2", da2);
      //  am2->addAttributeArray("DataArray3", da3);
      //  am3->addAttributeArray("DataArray4", da4);
      //  am3->addAttributeArray("DataArray5", da5);

      //  dc1->addAttributeMatrix("AttributeMatrix1", am1);
      //  dc1->addAttributeMatrix("AttributeMatrix2", am2);
      //  dc2->addAttributeMatrix("AttributeMatrix3", am3);
      //  dc2->addAttributeMatrix("AttributeMatrix4", am4);
      //  dc3->addAttributeMatrix("AttributeMatrix5", am5);

      //  dca->addDataContainer(dc1);
      //  dca->addDataContainer(dc2);
      //  dca->addDataContainer(dc3);

      return dca;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void RunTest()
    {
      DataContainerArray::Pointer dca = createDataContainerArray();

      //  MoveData::Pointer moveDataPtr = MoveData::New();
      //  moveDataPtr->setDataContainerArray(dca);
      //  moveDataPtr->setWhatToMove(k_MoveAttributeMatrix);

      //  // "Destination Data Container Does Not Exist" Test
      //  moveDataPtr->setDataContainerDestination("ThisDataContainerShouldNotExist");
      //  DataArrayPath amSource("DataContainer1", "AttributeMatrix1", "");
      //  moveDataPtr->setAttributeMatrixSource(amSource);
      //  moveDataPtr->execute();
      //  DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), DC_DEST_NOT_FOUND)
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void operator()()
    {
      std::cout << "#### RecursivePipelineTest Starting ####" << std::endl;

      int err = EXIT_SUCCESS;

      DREAM3D_REGISTER_TEST(RunTest())
    }

  private:
    RecursivePipelineTest(const RecursivePipelineTest&); // Copy Constructor Not Implemented
    void operator=(const RecursivePipelineTest&);               // Operator '=' Not Implemented
};
