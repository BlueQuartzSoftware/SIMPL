# ============================================================================
# Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

if(NOT "${DREAM3D_SDK}" STREQUAL "")
  include("${DREAM3D_SDK}/DREAM3D_SDK.cmake")
  get_property(DREAM3D_SDK_STATUS_PRINTED GLOBAL PROPERTY DREAM3D_SDK_STATUS_PRINTED)
  if(NOT DREAM3D_SDK_STATUS_PRINTED)
    message(STATUS "DREAM3D_SDK Location: ${DREAM3D_SDK}")
    set_property(GLOBAL PROPERTY DREAM3D_SDK_STATUS_PRINTED TRUE)
  endif()
endif()

# Request C++11 standard, using new CMake variables.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

if(${CMAKE_MAJOR_VERSION} STREQUAL "3")
  # CMake 3.0 introduces VERSION variables for each Project.
  cmake_policy(SET CMP0048 NEW)
  # CMake 3.0 introduces some OS X RPath policies
  cmake_policy(SET CMP0042 OLD)
  # CMake 3.1 introduces if() policies on dereferencing variables in quotes
  cmake_policy(SET CMP0054 NEW)
endif()

cmake_minimum_required(VERSION 3.6.0)

#------------------------------------------------------------------------------
# Create the Top level Project
#
project(SIMPLProj VERSION 1.0.0.0)


# ---------- Setup output Directories -------------------------
if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${SIMPLProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Libraries"
    )
endif()

# --------- Setup the Executable output Directory -------------
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${SIMPLProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Executables."
    )
endif()

# --------- Setup the Executable output Directory -------------
if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${SIMPLProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all static libraries."
    )
endif()


#------------------------------------------------------------------------------
# Find the Git Package for Versioning. It should be ok if Git is NOT found
Find_package(Git)

get_filename_component(SIMPL_PARENT ${SIMPLProj_SOURCE_DIR} DIRECTORY CACHE)

#------------------------------------------------------------------------------
# If the developer has set another CMP directory then use that, otherwise look
# for the CMP directory at the same level as the SIMPL directory
if("${CMP_SOURCE_DIR}" STREQUAL "")
  set(CMP_SOURCE_DIR ${SIMPL_PARENT}/CMP)
  message(STATUS "CMP_SOURCE_DIR: ${CMP_SOURCE_DIR}")
endif()

if(NOT EXISTS "${CMP_SOURCE_DIR}")
  if(Git_FOUND)
    message(STATUS "CMP_SOURCE_DIR does not exist at ${CMP_SOURCE_DIR}. Attempting to clone it from Github....")
    execute_process(COMMAND ${GIT_EXECUTABLE} clone -b develop git://github.com/bluequartzsoftware/CMP.git ${SIMPL_PARENT}/CMP
                    OUTPUT_VARIABLE CLONE_OUTPUT
                    RESULT_VARIABLE did_run
                    ERROR_VARIABLE git_error
                    WORKING_DIRECTORY ${SIMPL_PARENT} )
  endif()
  # One last check. If it does not exist at this point then we simply Abort CMake
  if(NOT EXISTS "${CMP_SOURCE_DIR}")
    message(FATAL_ERROR "CMP_SOURCE_DIR does not exist. We attempted to clone it from the GitHub repo but that also seemed\
      to fail. SIMPLView can not proceed without the CMP directory. Please ask a SIMPL developer for more\
      information about this error. Can you access http://github.com/bluequartzsoftware/CMP.git? is\
      a firewall blocking the 'git' protocol? ")
  endif()
endif()


#------------------------------------------------------------------------------
# There are several different Release Types that we are going to use.
# Official: This is an official release from BlueQuartz Software
# Beta: This is a Beta release that has is getting ready to be officially released.
# Development: This is currently in development and probably has bugs.
get_property(SIMPLProj_RELEASE_TYPE GLOBAL PROPERTY PACKAGE_RELEASE_TYPE)
if("${SIMPLProj_RELEASE_TYPE}" STREQUAL "")
  set(SIMPLProj_RELEASE_TYPE "Development")
  set_property(GLOBAL PROPERTY PACKAGE_RELEASE_TYPE "${SIMPLProj_RELEASE_TYPE}")
endif()

include(CMakeParseArguments)


#Enable use of Solution Folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Source)
set(PROJECT_RESOURCES_DIR ${SIMPLProj_SOURCE_DIR}/Resources)
set(EXE_DEBUG_EXTENSION "_debug")
set(PROJECT_INSTALL_EXECUTABLES "1")
# -----------------------------------------------------------------------
# The project generally does NOT want to install headers. See if another
# project has over ridden that property
get_property(CMP_PROJECT_INSTALL_HEADERS GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS)
if("${CMP_PROJECT_INSTALL_HEADERS}" STREQUAL "")
  set_property(GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS "1")
  set(PROJECT_INSTALL_HEADERS "1")
else()
  set(PROJECT_INSTALL_HEADERS "${CMP_PROJECT_INSTALL_HEADERS}")
endif()

# -----------------------------------------------------------------------
# Setup a Global property that is used to gather Documentation Information
# into a single known location
get_property(DREAM3DDocRoot GLOBAL PROPERTY DREAM3DDocRoot)
if("${DREAM3DDocRoot}" STREQUAL "")
  set_property(GLOBAL PROPERTY DREAM3DDocRoot "${SIMPLProj_BINARY_DIR}")
  get_property(DREAM3DDocRoot GLOBAL PROPERTY DREAM3DDocRoot)
  file(WRITE "${SIMPLProj_BINARY_DIR}/DREAM3DDocGroupList" "")
endif()


get_property(SIMPLPluginList GLOBAL PROPERTY SIMPLPluginList)
if("${SIMPLPluginList}" STREQUAL "")
  set_property(GLOBAL PROPERTY SIMPLPluginList "${SIMPLProj_BINARY_DIR}/SIMPLPluginList.txt")
  get_property(SIMPLPluginList GLOBAL PROPERTY SIMPLPluginList)
  file(WRITE "${SIMPLProj_BINARY_DIR}/SIMPLPluginList.txt" "")
endif()


get_property(SIMPLibSearchDirs GLOBAL PROPERTY SIMPLibSearchDirs)
if("${SIMPLibSearchDirs}" STREQUAL "")
  set_property(GLOBAL PROPERTY SIMPLibSearchDirs "${SIMPLProj_BINARY_DIR}/LibSearchDirs.txt")
  get_property(SIMPLibSearchDirs GLOBAL PROPERTY SIMPLibSearchDirs)
  file(WRITE "${SIMPLProj_BINARY_DIR}/LibSearchDirs.txt" "")
endif()

# -----------------------------------------------------------------------
# The project generally wants to install headers. See if another
# project has over ridden that property
get_property(CMP_PROJECT_INSTALL_HEADERS GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS)
if("${CMP_PROJECT_INSTALL_HEADERS}" STREQUAL "")
  set_property(GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS "1")
endif()


set(PROJECT_INSTALL_HEADERS "${CMP_PROJECT_INSTALL_HEADERS}")
set(CMP_CONFIGURED_FILES_SOURCE_DIR ${CMP_SOURCE_DIR}/ConfiguredFiles)
INCLUDE (${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake )
set(CMP_ENABLE_PLUGINS "1")
set(CMP_PLUGIN_LIST_FILE "${SIMPLPluginList}")
set(CMP_PLUGIN_SEARCHDIR_FILE "${SIMPLibSearchDirs}")
file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin/plugins;")
file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin;")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_MODULES_SOURCE_DIR ${CMP_SOURCE_DIR}/Modules CACHE INTERNAL "")

if(UNIX AND NOT APPLE)
    set(DREAM3D_LINUX_SYSTEM ${CMAKE_SYSTEM_NAME} CACHE STRING "The Name of the linux distrubution for packaging")
endif()



#------------------------------------------------------------------------------
# Find the Git Package for Versioning. It should be ok if Git is NOT found
Find_package(Git)

cmpRevisionString( GENERATED_HEADER_FILE_PATH "SIMPLFileNotUsed.h"
                        GENERATED_SOURCE_FILE_PATH "SIMPLFileNotUsed.cpp"
                        NAMESPACE "SIMPLProj"
                        PROJECT_NAME "${PROJECT_NAME}"
                        EXPORT_MACRO "SIMPLProj_EXPORT")

# --------------------------------------------------------------------
# TODO: Do we need a SIMPL_DATA_DIR?

# --------------------------------------------------------------------
# Override CMake's built in module directory by prepending cmp's module
# directory first
set(CMAKE_MODULE_PATH ${CMP_MODULES_SOURCE_DIR} ${CMAKE_MODULE_PATH})

# Build shared libraries
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" ON)
set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
  #message(FATAL_ERROR "Due to issues exporting symbols with MSVC DREAM3D does NOT currently support intermediate shared libraries. Please turn OFF BUILD_SHARED_LIBS")
    set(LIB_TYPE SHARED)
    list(APPEND CMP_LIB_SEARCH_DIRS  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    if(MSVC)
        list(APPEND CMP_LIB_SEARCH_DIRS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
                                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release )
    endif()

else()
  message(FATAL_ERROR "BUILD_SHARED_LIBS Must be set to ON in order to build properly.")
endif(BUILD_SHARED_LIBS)

option(SIMPL_BUILD_WIDGETS "Build the QtWidgets that are needed for GUI Applications" OFF)

# --------------------------------------------------------------------
# Find and Use the Qt5 Libraries
include(${CMP_SOURCE_DIR}/ExtLib/Qt5Support.cmake)
set(SIMPL_Qt5_COMPONENTS Core Network)
if( SIMPL_BUILD_WIDGETS)
  set(SIMPL_Qt5_COMPONENTS Core Widgets Network Gui Concurrent Svg Xml OpenGL PrintSupport)
endif()
set(USE_QtWebEngine FALSE)
CMP_AddQt5Support( "${SIMPL_Qt5_COMPONENTS}" "${USE_QtWebEngine}" "${SIMPLProj_BINARY_DIR}" "SIMPL")


# --------------------------------------------------------------------
# Find HDF5 Headers/Libraries
# HDF5 now comes with everything that is needed for CMake to load
# up the targets (Exported) that it needs. We just need to find where
# HDF5 is installed.
include(${CMP_SOURCE_DIR}/ExtLib/HDF5Support.cmake)

# --------------------------------------------------------------------
# Should we use Intel Threading Building Blocks
# --------------------------------------------------------------------
set(SIMPLib_USE_PARALLEL_ALGORITHMS "")
option(SIMPL_USE_MULTITHREADED_ALGOS "Use MultiThreaded Algorithms" ON)
if(SIMPL_USE_MULTITHREADED_ALGOS)
    find_package(TBB)
    if(TBB_FOUND)
        include_directories(${TBB_INCLUDE_DIRS} )
        CMP_COPY_DEPENDENT_LIBRARIES("tbb;tbb_malloc")
        # Append the locations of the TBB libraries to our Library Search Paths
        list(APPEND CMP_LIB_SEARCH_DIRS ${TBB_BINARY_DIR} ${TBB_LIBRARY_DIR} )
        # Create an install rule to copy the dependent shared library over to the installed location
        CMP_LIBRARIES_INSTALL_RULES("tbb;tbb_malloc" ".")
    else()
        message(FATAL_ERROR "The Intel Threading Building Blocks library is needed to enable the multithreaded algorithms. Please make sure it is installed properly")
    endif()
    set(SIMPLib_USE_PARALLEL_ALGORITHMS "1")
endif()

# --------------------------------------------------------------------
# SIMPL needs the Eigen library for Least Squares fit and Eigen
# value/vector calculations.
set(SIMPLib_USE_EIGEN "")
Find_Package(Eigen REQUIRED)
if(EIGEN_FOUND)
  message(STATUS "Found Eigen Library")
  include_directories( ${EIGEN_INCLUDE_DIRS} )
  set(SIMPLib_USE_EIGEN "1")
else()
  message(WARNING "The Eigen Library is required for some algorithms to execute. These algorithms will be disabled.")
endif()


# --------------------------------------------------------------------
# Find the Doxygen package
FIND_PACKAGE(Doxygen)
if(DOXYGEN_FOUND)
  get_property(Doxygen_STATUS_PRINTED GLOBAL PROPERTY Doxygen_STATUS_PRINTED)
  if(NOT Doxygen_STATUS_PRINTED)
    message(STATUS "Doxygen Found at ${DOXYGEN_EXECUTABLE}")
    set_property(GLOBAL PROPERTY Doxygen_STATUS_PRINTED TRUE)
  endif()
else()
  message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
endif()

# --------------------------------------------------------------------
# Should we use enable NTFS file checking on Windows
# --------------------------------------------------------------------
if(WIN32)
  set(SIMPLib_NTFS_FILE_CHECK "")
  option(SIMPL_ENABLE_NTFS_FILE_CHECKING "Enable NTFS File Permissions Checking" ON)
  if(SIMPL_ENABLE_NTFS_FILE_CHECKING)
      set(SIMPLib_NTFS_FILE_CHECK "1")
  endif()
endif()

# -----------------------------------------------------------------------
# Set a Global Prefix where everthing is going to get intalled. For OS X we are
# going to try to install everything into the .app package
# -----------------------------------------------------------------------
set_property(GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX ".")
if(APPLE)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX "${SIMPLView_APPLICATION_NAME}${EXE_DEBUG_EXTENSION}.app/Contents/")
  else()
    set_property(GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX "${SIMPLView_APPLICATION_NAME}.app/Contents/")
  endif()
endif()
get_property(DREAM3D_PACKAGE_DEST_PREFIX GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX)

# --------------------------------------------------------------------
# Now that we have found all of our required libraries and packages we can start the all
# the tests

# We do NOT want to install any of the headers
set(SIMPLProj_INSTALL_FILES "${PROJECT_INSTALL_HEADERS}")

# --------------------------------------------------------------------
# Generate our ReadMe and License Files
configure_file(${PROJECT_RESOURCES_DIR}/SIMPL/SIMPLLicense.txt
                ${PROJECT_BINARY_DIR}/SIMPL/SIMPLLicense.txt )

# --------------------------------------------------------------------
# Generate install rules for the text files
install(FILES ${PROJECT_BINARY_DIR}/SIMPL/SIMPLLicense.txt
        DESTINATION "${DREAM3D_PACKAGE_DEST_PREFIX}"
        COMPONENT Applications)

# Include some directories for the compiler
include_directories(${PROJECT_BINARY_DIR})

# --------------------------------------------------------------------
# Add in some compiler definitions
# --------------------------------------------------------------------
if( CMAKE_BUILD_TYPE MATCHES Debug )
  ADD_DEFINITIONS(-DDEBUG)
ENDif( CMAKE_BUILD_TYPE MATCHES Debug )

if(NOT MSVC)
 # ADD_DEFINITIONS(-Wall)
endif()

# On linux we need to set this because some of the libraries are Static
# and some are shared.
if( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND NOT MSVC )
  ADD_DEFINITIONS(-fPIC)
ENDif()

# --------------------------------------------------------------------
# If was are using GCC, make the compiler messages on a single line
if(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDif(CMAKE_COMPILER_IS_GNUCC)
if(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDif(CMAKE_COMPILER_IS_GNUCXX)

if(MSVC)
 add_definitions(-D_CRT_SECURE_NO_WARNINGS)
 add_definitions(-D_SCL_SECURE_NO_WARNINGS)
 set(SIMPLib_DISABLE_MSVC_WARNINGS "")
 option(SIMPL_DISABLE_MSVC_WARNINGS "Disable some MSVC Compiler warnings" OFF)
 if(SIMPL_DISABLE_MSVC_WARNINGS)
    set(SIMPLib_DISABLE_MSVC_WARNINGS "1")
 endif()
endif()



include_directories(${SIMPLProj_SOURCE_DIR}/Source)
include_directories(${PROJECT_BINARY_DIR})

# -----------------------------------------------------------------------
# Setup a Global property that is used to gather Documentation Information
# into a single known location
get_property(DREAM3DDocRoot GLOBAL PROPERTY DREAM3DDocRoot)
if("${DREAM3DDocRoot}" STREQUAL "")
  set_property(GLOBAL PROPERTY DREAM3DDocRoot "${SIMPLProj_BINARY_DIR}")
endif()
file(WRITE ${SIMPLProj_BINARY_DIR}/DREAM3DDocGroupList "")

# ------- Enable the CTest testing. Use make test to run all tests ---------------
OPTION(SIMPL_BUILD_TESTING "Compile the test programs" ON)
if(SIMPL_BUILD_TESTING)
    #ENABLE_TESTING()
    include( CTest )
endif()


# --------------------------------------------------------------------
#
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/SIMPL/SIMPLTargetsConfigVersion.cmake"
  VERSION ${SIMPL_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_file(${SIMPLProj_SOURCE_DIR}/SIMPLConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/SIMPL/SIMPLConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/SIMPL)
if(APPLE)
  set(ConfigPackageLocation ${DREAM3D_PACKAGE_DEST_PREFIX}${ConfigPackageLocation})
endif()

install(
  FILES
    "${SIMPLProj_SOURCE_DIR}/SIMPLConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/SIMPL/SIMPLTargetsConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)


# --------------------------------------------------------------------
# Build the SimpleITKExplicit Library.
option(SIMPL_USE_ITK "Use ITK For Image Processing" ON)
if(SIMPL_USE_ITK)
  add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/SimpleITKExplicit ${PROJECT_BINARY_DIR}/SimpleITKExplicit)
  get_property(SimpleITKExplicit_LIB_NAME TARGET SimpleITKExplicit PROPERTY OUTPUT_NAME)
endif()


# -----------------------------------------------------------------------
# Add in the Necessary HDF5 Support Sources into a library
# -----------------------------------------------------------------------
add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/H5Support ${PROJECT_BINARY_DIR}/H5Support)

# -----------------------------------------------------------------------
# This needs to be set here as we are going to look for files in this directory
# -----------------------------------------------------------------------
if("${FilterWidgetsLib_BINARY_DIR}" STREQUAL "")
  set(FilterWidgetsLib_BINARY_DIR ${SIMPLProj_BINARY_DIR}/FilterWidgetsLib)
endif()

# -----------------------------------------------------------------------
# -- Add in the DREAM3D Sources into a library
add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/SIMPLib/ ${PROJECT_BINARY_DIR}/SIMPLib)


# add the Command line utilities
# Figure out here if we are going to build the command line tools
add_subdirectory(${SIMPLProj_SOURCE_DIR}/Tools ${PROJECT_BINARY_DIR}/Tools)

# --------------------------------------------------------------------
# Build the SVWidgetsLib Library which holds all the GUI Specific Widgets such
# as the Filter Parameter Widgets and various building block widgets for the GUI applications.
# The plugins are also going to depend on these being built
if(SIMPL_BUILD_WIDGETS)
  add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/SVWidgetsLib ${PROJECT_BINARY_DIR}/SVWidgetsLib)
endif()


if(0)
#-------------------------------------------------------------------------------
# Should we build the documentation using DOxygen
option(SIMPLView_BUILD_DOCUMENTATION "Build the DREAM3D documentation" ON)
if(SIMPLView_BUILD_DOCUMENTATION)
  set(DOCUMENTATION_RESOURCES_DIR ${PROJECT_RESOURCES_DIR}/DocConfigFiles)

  set(DOX_GENERATE_RTF "NO")
  option(SIMPLView_GENERATE_RTF "Generate RTF Documentation" OFF)
  if(SIMPLView_GENERATE_RTF STREQUAL "ON")
    set(DOX_GENERATE_RTF "YES")
  endif()

  # By default we generate the Documentation in HTML format
  set(DOX_GENERATE_HTML "NO")
  option(SIMPLView_GENERATE_HTML "Generate HTML Documentation" ON)
  if(SIMPLView_GENERATE_HTML STREQUAL "ON")
    set(DOX_GENERATE_HTML "YES")
  endif()

  #most windows machines do NOT have LATEX built in so default this to OFF
  set(DOX_GENERATE_LATEX "NO")
  option(SIMPLView_GENERATE_LATEX "Generate PDF Documentation" OFF)
  if(SIMPLView_GENERATE_LATEX STREQUAL "ON")
    set(DOX_GENERATE_LATEX "YES")
    # Check for existence of LaTex executable files
    INCLUDE( ${DOCUMENTATION_RESOURCES_DIR}/CMake/UseLATEX.cmake)
    if(APPLE)
      option(AUTO_OPEN_PDF "Automatically open generated PDF files with default PDF reading application" ON)
    endif()
  endif()


  # Now add each subdirectory that holds our documentation files
  add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation/ReferenceManual ${PROJECT_BINARY_DIR}/Documentation/ReferenceManual)
  add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation/APIDocs ${PROJECT_BINARY_DIR}/Documentation/APIDocs)
endif()

endif()


# This should be the last line in this file:
include(${PROJECT_RESOURCES_DIR}/CPack/PackageProject.cmake)

# ------- Enable the CTest testing. Use make test to run all tests ---------------
if(SIMPL_BUILD_TESTING)
    include(${SIMPLProj_SOURCE_DIR}/Test/CMakeLists.txt)
endif()


