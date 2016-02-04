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

#------------------------------------------------------------------------------
# List all the source files here. They will NOT be compiled but instead
# be directly included in the main test source file. We list them here so that
# they will show up in IDEs
set(TEST_NAMES
  ConditionalSetValueTest
  CombineAttributeArraysTest
  CombineAttributeMatricesTest
  DataContainerTest
  ReplaceValueTest
  StringDataArrayTest
  StructArrayTest
  FilterPipelineTest
  DataArrayTest
  ErrorMessageTest
  DataContainerBundleTest
  FilterParametersRWTest
  CreateDataArrayTest
  RemoveComponentFromArrayTest
  QuaternionMathTest
  CreateFeatureArrayFromElementArrayTest

  )

set( ${PLUGIN_NAME}_TEST_SRCS )
set(FilterTestIncludes "")
set(TestMainFunctors "")

foreach(name  ${TEST_NAMES})
  set( ${PLUGIN_NAME}_TEST_SRCS
    ${${PLUGIN_NAME}_TEST_SRCS}
    "${SIMPLTest_SOURCE_DIR}/${name}.cpp"
    )
  string(CONCAT
    FilterTestIncludes
    ${FilterTestIncludes}
    "#include \"${name}.cpp\"\n"
    )

    string(CONCAT
      TestMainFunctors
     ${TestMainFunctors}
     "  ${name}()()|\n")
endforeach()

STRING(REPLACE "|" ";" TestMainFunctors ${TestMainFunctors}   )

configure_file(${SIMPLTest_SOURCE_DIR}/TestMain.cpp.in
               ${SIMPLTest_BINARY_DIR}/SIMPLUnitTest.cpp @ONLY)


# Set the source files properties on each source file.
foreach(f ${${PLUGIN_NAME}_TEST_SRCS})
  set_source_files_properties( ${f} PROPERTIES HEADER_FILE_ONLY TRUE)
endforeach()

AddSIMPLUnitTest(TESTNAME SIMPLUnitTest
  SOURCES ${SIMPLTest_BINARY_DIR}/SIMPLUnitTest.cpp ${${PLUGIN_NAME}_TEST_SRCS}
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

if(MSVC)
  set_source_files_properties( ${SIMPLTest_BINARY_DIR}/SIMPLUnitTest.cpp PROPERTIES COMPILE_FLAGS /bigobj)
endif()
