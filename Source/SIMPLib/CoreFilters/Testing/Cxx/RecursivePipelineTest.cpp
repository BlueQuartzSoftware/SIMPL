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

#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QObject>

#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
#include "SIMPLib/CoreFilters/RemoveArrays.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

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
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::RecursivePipelineTest::OutputDREAM3DFile);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    FilterManager* fm = FilterManager::Instance();

    // Now instantiate the RemoveArrays Filter from the FilterManager
    QString filtName = "RemoveArrays";
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The RecursivePipelineTest requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    // Now instantiate the DataContainerReader Filter from the FilterManager
    filtName = "DataContainerReader";
    filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The RecursivePipelineTest requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    // Now instantiate the DataContainerWriter Filter from the FilterManager
    filtName = "DataContainerWriter";
    filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The RecursivePipelineTest requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  FilterPipeline::Pointer createPipeline()
  {
    FilterPipeline::Pointer pipeline = FilterPipeline::New();

    DataContainerReader::Pointer reader = DataContainerReader::New();
    reader->setInputFile(UnitTest::RecursivePipelineTest::InputDREAM3DFile);
    pipeline->pushBack(reader);

    DataContainerWriter::Pointer writer = DataContainerWriter::New();
    writer->setOutputFile(UnitTest::RecursivePipelineTest::OutputDREAM3DFile);
    pipeline->pushBack(writer);

    return pipeline;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RunTest()
  {
    FilterPipeline::Pointer pipeline = createPipeline();
    pipeline->preflightPipeline();
    DREAM3D_REQUIRE_EQUAL(pipeline->getErrorCondition(), 0);
    pipeline->execute();
    DREAM3D_REQUIRE_EQUAL(pipeline->getErrorCondition(), 0);

    H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();
    FilterPipeline::Pointer outputPipeline = reader->readPipelineFromFile(UnitTest::RecursivePipelineTest::OutputDREAM3DFile);
    outputPipeline->preflightPipeline();
    DREAM3D_REQUIRE_EQUAL(outputPipeline->getErrorCondition(), 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### RecursivePipelineTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(RunTest());
  }

private:
  RecursivePipelineTest(const RecursivePipelineTest&); // Copy Constructor Not Implemented
  void operator=(const RecursivePipelineTest&);        // Operator '=' Not Implemented
};
