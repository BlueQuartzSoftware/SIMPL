# ============================================================================
# Copyright (c) 2009-2016 BlueQuartz Software, LLC
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
# contributors may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The code contained herein was partially funded by the followig contracts:
#    United States Air Force Prime Contract FA8650-07-D-5800
#    United States Air Force Prime Contract FA8650-10-D-5210
#    United States Prime Contract Navy N00173-07-C-2068
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

set(SIMPLTest_SOURCE_DIR ${SIMPLProj_SOURCE_DIR}/Test)
set(SIMPLTest_BINARY_DIR ${SIMPLProj_BINARY_DIR}/Test)

include_directories(${SIMPLTest_SOURCE_DIR})

#------------------------------------------------------------------------------
# Include this file from the SIMPL Project
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
  ArrayCalculatorTest
  CombineAttributeArraysTest
  CombineAttributeMatricesTest
  ConditionalSetValueTest
  CreateDataArrayTest
  CreateFeatureArrayFromElementArrayTest
  DataArrayTest
  DataContainerBundleTest
  DataContainerTest
  ErrorMessageTest
  ExecuteProcessTest
  FilterParametersRWTest
  FilterPipelineTest
  QuaternionMathTest
  RemoveComponentFromArrayTest
  ReplaceValueTest
  StatsDataJsonRWTest
  StringDataArrayTest
  StructArrayTest
)


QT5_WRAP_CPP( _moc_filter_source  ${SIMPLTest_SOURCE_DIR}/PipelinePauseTest.cpp)
set_source_files_properties( ${_moc_filter_source} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${_moc_filter_source} PROPERTIES HEADER_FILE_ONLY TRUE)

QT5_WRAP_CPP( execute_process_moc  ${SIMPLTest_SOURCE_DIR}/ExecuteProcessTest.cpp)
set_source_files_properties( ${execute_process_moc} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${execute_process_moc} PROPERTIES HEADER_FILE_ONLY TRUE)

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
  SOURCES ${SIMPLTest_BINARY_DIR}/SIMPLUnitTest.cpp ${${PLUGIN_NAME}_TEST_SRCS} ${execute_process_moc}
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddSIMPLUnitTest(TESTNAME PipelinePauseTest
  SOURCES ${SIMPLTest_SOURCE_DIR}/PipelinePauseTest.cpp ${_moc_filter_source}
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

if(MSVC)
  set_source_files_properties( ${SIMPLTest_BINARY_DIR}/SIMPLUnitTest.cpp PROPERTIES COMPILE_FLAGS /bigobj)
endif()
