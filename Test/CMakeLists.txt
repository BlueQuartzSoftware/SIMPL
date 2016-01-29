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

AddSIMPLUnitTest(TESTNAME SIMPLUnitTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/SIMPLUnitTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

