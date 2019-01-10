
set(SIMPLTest_SOURCE_DIR ${SIMPLib_SOURCE_DIR}/Testing)
set(SIMPLTest_BINARY_DIR ${SIMPLib_BINARY_DIR}/Testing)

#------------------------------------------------------------------------------
# Include this file from the CMP Project
include(${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake)
include(${SIMPLProj_SOURCE_DIR}/Source/SIMPLib/SIMPLibMacros.cmake)

set(TEST_TEMP_DIR ${SIMPLTest_BINARY_DIR}/Temp)
set(TESTFILES_SRC_DIR ${SIMPLTest_SOURCE_DIR}/TestFiles)

# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


get_target_property(QtQMake_location Qt5::qmake LOCATION)
execute_process(COMMAND "${QtQMake_location}" -query QMAKE_VERSION OUTPUT_VARIABLE QMAKE_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)


configure_file(${SIMPLTest_SOURCE_DIR}/TestFileLocations.h.in
          ${SIMPLTest_BINARY_DIR}/SIMPLTestFileLocations.h @ONLY IMMEDIATE)

configure_file(${SIMPLProj_SOURCE_DIR}/Resources/UnitTestSupport.hpp
          ${SIMPLTest_BINARY_DIR}/UnitTestSupport.hpp COPYONLY IMMEDIATE)

# # QT5_WRAP_CPP( _moc_filter_source  ${SIMPLTest_SOURCE_DIR}/PipelinePauseTest.cpp)
# set_source_files_properties( ${_moc_filter_source} PROPERTIES GENERATED TRUE)
# set_source_files_properties( ${_moc_filter_source} PROPERTIES HEADER_FILE_ONLY TRUE)

STRING(REPLACE "|" ";" TestMainFunctors "${SIMPLib_UnitTest_MainFunctors}")
set(FilterTestIncludes  "${SIMPLib_UnitTest_Includes}")

set(QT_APPLICATION_CLASS_HEADER "#include <QtCore/QCoreApplication>")
set(QT_APPLICATION_CLASS "QCoreApplication")

configure_file(${SIMPLTest_SOURCE_DIR}/TestMain.cpp.in
          ${SIMPLTest_BINARY_DIR}/SIMPLUnitTest.cpp @ONLY)

# Set the source files properties on each source file.
foreach(f ${${SIMPL_UNIT_TEST}_TEST_SRCS})
  set_source_files_properties( ${f} PROPERTIES HEADER_FILE_ONLY TRUE)
endforeach()

set(LINKED_LIBRARIES
  Qt5::Core
  H5Support
  SIMPLib
  ${ITK_LIBRARIES}
)

if(SIMPL_Group_REST)
  set(LINKED_LIBRARIES ${LINKED_LIBRARIES} RESTClient)
endif()

AddSIMPLUnitTest(TESTNAME SIMPLUnitTest
  SOURCES ${SIMPLTest_BINARY_DIR}/SIMPLUnitTest.cpp ${${SIMPL_UNIT_TEST}_TEST_SRCS}
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES ${LINKED_LIBRARIES}
  INCLUDE_DIRS 
			${${PLUGIN_NAME}Test_SOURCE_DIR}
			${${PLUGIN_NAME}Test_BINARY_DIR}
  )

if(MSVC)
  set_source_files_properties( ${SIMPLTest_BINARY_DIR}/SIMPLUnitTest.cpp PROPERTIES COMPILE_FLAGS /bigobj)
endif()

#-------------------------------------------------------------------------------
#- This copies all the Test files into the Build directory
add_custom_target(SIMPLFileCopy ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${TESTFILES_SRC_DIR} ${TEST_TEMP_DIR}
            COMMENT "Copying ${PROJECT_NAME} Test Files to Build directory...")
set_target_properties(SIMPLFileCopy PROPERTIES FOLDER ZZ_COPY_FILES)
