#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2015, BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

set(SIMPLTest_SOURCE_DIR ${SIMPLProj_SOURCE_DIR}/Test)
set(SIMPLTest_BINARY_DIR ${SIMPLProj_BINARY_DIR}/Test)


include_directories(${SIMPLTest_SOURCE_DIR})

include(${SIMPLProj_SOURCE_DIR}/Test/SIMPLTestFunctions.cmake)



set(TEST_TEMP_DIR ${SIMPLTest_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${SIMPLTest_SOURCE_DIR}/TestFileLocations.h.in
               ${SIMPLTest_BINARY_DIR}/SIMPLTestFileLocations.h @ONLY IMMEDIATE)

include_directories(${SIMPLTest_BINARY_DIR})


AddSIMPLUnitTest(TESTNAME ConditionalSetValueTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/ConditionalSetValueTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)
  
AddSIMPLUnitTest(TESTNAME CombineAttributeArraysTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/CombineAttributeArraysTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME CombineAttributeMatricesTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/CombineAttributeMatricesTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME DataContainerTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/DataContainerTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME ReplaceValueTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/ReplaceValueTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME StringDataArrayTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/StringDataArrayTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME StructArrayTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/StructArrayTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME FilterPipelineTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/FilterPipelineTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME DataArrayTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/DataArrayTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME ErrorMessageTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/ErrorMessageTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME DataContainerBundleTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/DataContainerBundleTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME FilterParametersRWTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/FilterParametersRWTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME CreateDataArrayTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/CreateDataArrayTest.cpp
   FOLDER "SIMPLibProj/Test"
 LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME RemoveComponentFromArrayTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/RemoveComponentFromArrayTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

 AddSIMPLUnitTest(TESTNAME QuaternionMathTest
   SOURCES ${SIMPLTest_SOURCE_DIR}/QuaternionMathTest.cpp
   FOLDER "SIMPLibProj/Test"
   LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

 AddSIMPLUnitTest(TESTNAME CreateFeatureArrayFromElementArrayTest
   SOURCES ${SIMPLTest_SOURCE_DIR}/CreateFeatureArrayFromElementArrayTest.cpp
   FOLDER "SIMPLibProj/Test"
   LINK_LIBRARIES Qt5::Core H5Support SIMPLib)


