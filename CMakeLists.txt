
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

cmake_minimum_required(VERSION 3.8.0)

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
  message(FATAL_ERROR "CMP_SOURCE_DIR does not exist at '${CMP_SOURCE_DIR}'")
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
  get_property(CMP_PROJECT_INSTALL_HEADERS GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS)
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

# ------------------
# Generate a Revision String
cmpRevisionString( GENERATED_HEADER_FILE_PATH "SIMPLFileNotUsed.h"
                        GENERATED_SOURCE_FILE_PATH "SIMPLFileNotUsed.cpp"
                        NAMESPACE "SIMPLProj"
                        PROJECT_NAME "${PROJECT_NAME}"
                        EXPORT_MACRO "SIMPLProj_EXPORT")

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

if(MSVC)
  option(SIMPL_DISABLE_MSVC_WARNINGS "Disable some MSVC Compiler warnings" ON)
  if(SIMPL_DISABLE_MSVC_WARNINGS)
    set(SIMPLib_DISABLE_MSVC_WARNINGS "1")
  endif()
endif()

#-------------------------------------------------------------------------------
# SIMPL_ENABLE_GROUP
# 
function(SIMPL_ENABLE_GROUP)
  set(options)
  set(oneValueArgs GROUP_NAME DESC ENABLE)
  set(multiValueArgs DEPENDENCIES)

  cmake_parse_arguments(P "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  option(SIMPL_Group_${P_GROUP_NAME} "${P_DESC}" ${P_ENABLE})

  if(SIMPL_Group_${P_GROUP_NAME})
    foreach(group ${P_DEPENDENCIES})
      set(SIMPL_Group_${group} ON CACHE BOOL "" FORCE)
    endforeach()
  endif()
endfunction()

# --------------------------------------------------------------------
# Turn on SIMPLib Groups to compile

SIMPL_ENABLE_GROUP(GROUP_NAME BASE
            ENABLE ON
            DESC "Compile SIMPL Base functionality")
SIMPL_ENABLE_GROUP(GROUP_NAME FILTERS
            ENABLE ON
            DESC "Compile SIMPL Filtering functionality"
            DEPENDENCIES BASE)
SIMPL_ENABLE_GROUP(GROUP_NAME PLUGIN
            ENABLE ON
            DESC "Compile SIMPL Plugin functionality"
            DEPENDENCIES BASE FILTERS)
SIMPL_ENABLE_GROUP(GROUP_NAME WIDGETS
            ENABLE ON
            DESC "Compile SIMPL Qt Widgets functionality"
            DEPENDENCIES BASE FILTERS PLUGIN)

OPTION(SIMPL_BUILD_TESTING "Compile the test programs" ON)

# --------------------------------------------------------------------
# Find HDF5 Headers/Libraries
# HDF5 now comes with everything that is needed for CMake to load
# up the targets (Exported) that it needs. We just need to find where
# HDF5 is installed.
include(${CMP_SOURCE_DIR}/ExtLib/HDF5Support.cmake)

# --------------------------------------------------------------------
# Should we use Intel Threading Building Blocks
# --------------------------------------------------------------------
set(SIMPL_USE_PARALLEL_ALGORITHMS "")
option(SIMPL_USE_MULTITHREADED_ALGOS "Use MultiThreaded Algorithms" OFF)
if(SIMPL_USE_MULTITHREADED_ALGOS)
  find_package(TBB)
  if(TBB_FOUND)
      CMP_COPY_DEPENDENT_LIBRARIES("tbb;tbb_malloc")
      # Append the locations of the TBB libraries to our Library Search Paths
      # TBB_LIB_SEARCH_PATH Comes from the FindTBB.cmake file and should work across all platforms
      file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${TBB_LIB_SEARCH_PATH};")

      # Create an install rule to copy the dependent shared library over to the installed location
      CMP_LIBRARIES_INSTALL_RULES("tbb;tbb_malloc" ".")
  else()
      message(FATAL_ERROR "The Intel Threading Building Blocks library is needed to enable the multithreaded algorithms. Please make sure it is installed properly")
  endif()
  set(SIMPL_USE_PARALLEL_ALGORITHMS "1")
endif()

# --------------------------------------------------------------------
# SIMPL needs the Eigen library for Least Squares fit and Eigen value/vector calculations.
set(SIMPL_USE_EIGEN "")
Find_Package(Eigen REQUIRED)
if(EIGEN_FOUND)
  message(STATUS "Found Eigen Library")
  set(SIMPL_USE_EIGEN "1")
else()
  message(WARNING "The Eigen Library is required for some algorithms to execute. These algorithms will be disabled.")
endif()

# --------------------------------------------------------------------
# Find and Use the Qt5 Libraries
include(${CMP_SOURCE_DIR}/ExtLib/Qt5Support.cmake)
set(SIMPL_Qt5_COMPONENTS Core Network)
if( SIMPL_Group_WIDGETS)
  set(SIMPL_Qt5_COMPONENTS Core Widgets Network Gui Concurrent Svg Xml OpenGL PrintSupport)
endif()
set(USE_QtWebEngine FALSE)
CMP_AddQt5Support( "${SIMPL_Qt5_COMPONENTS}" "${USE_QtWebEngine}" "${SIMPLProj_BINARY_DIR}" "SIMPL")

# --------------------------------------------------------------------
# Should we use enable NTFS file checking on Windows
# --------------------------------------------------------------------
if(WIN32)
  set(SIMPL_NTFS_FILE_CHECK "")
  option(SIMPL_ENABLE_NTFS_FILE_CHECKING "Enable NTFS File Permissions Checking" ON)
  if(SIMPL_ENABLE_NTFS_FILE_CHECKING)
      set(SIMPL_NTFS_FILE_CHECK "1")
  endif()
endif()

# -----------------------------------------------------------------------
# Set a Global Prefix where everthing is going to get intalled. For OS X we are
# going to try to install everything into the .app package
# -----------------------------------------------------------------------
# set_property(GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX ".")
get_property(DREAM3D_PACKAGE_DEST_PREFIX GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX)
if("${DREAM3D_PACKAGE_DEST_PREFIX}" STREQUAL "")
  set_property(GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX ".")
  get_property(DREAM3D_PACKAGE_DEST_PREFIX GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX)
  set(APPLE_RESOURCES_DIR "share/SIMPL")
else ()
    set(APPLE_RESOURCES_DIR "${DREAM3D_PACKAGE_DEST_PREFIX}/Resources")

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
if(APPLE)
  install(FILES ${PROJECT_BINARY_DIR}/SIMPL/SIMPLLicense.txt
          DESTINATION "${APPLE_RESOURCES_DIR}"
          COMPONENT Applications)
else()
  install(FILES ${PROJECT_BINARY_DIR}/SIMPL/SIMPLLicense.txt
          DESTINATION "${DREAM3D_PACKAGE_DEST_PREFIX}"
          COMPONENT Applications)
endif()

# -----------------------------------------------------------------------
# Setup a Global property that is used to gather Documentation Information
# into a single known location
get_property(DREAM3DDocRoot GLOBAL PROPERTY DREAM3DDocRoot)
if("${DREAM3DDocRoot}" STREQUAL "")
  set_property(GLOBAL PROPERTY DREAM3DDocRoot "${SIMPLProj_BINARY_DIR}")
endif()
file(WRITE ${SIMPLProj_BINARY_DIR}/DREAM3DDocGroupList "")

# ------- Enable the CTest testing. Use make test to run all tests ---------------
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
# Use ITK
option(SIMPL_USE_ITK "Use ITK For Image Processing" ON)

# -----------------------------------------------------------------------
# Add in the Necessary HDF5 Support Sources into a library
# -----------------------------------------------------------------------
add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/H5Support ${PROJECT_BINARY_DIR}/H5Support)

# -----------------------------------------------------------------------
# Add in the support to host the documentation over a thin-http server
# -----------------------------------------------------------------------
add_subdirectory(${SIMPLProj_SOURCE_DIR}/3rdParty/QtWebApp )


# -----------------------------------------------------------------------
# This needs to be set here as we are going to look for files in this directory
# -----------------------------------------------------------------------
if("${FilterWidgetsLib_BINARY_DIR}" STREQUAL "")
  set(FilterWidgetsLib_BINARY_DIR ${SIMPLProj_BINARY_DIR}/FilterWidgetsLib)
endif()

# -----------------------------------------------------------------------
# -- Add in the SIMPL Sources into a library
add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/SIMPLib ${PROJECT_BINARY_DIR}/SIMPLib)

# add the Command line utilities
# Figure out here if we are going to build the command line tools
add_subdirectory(${SIMPLProj_SOURCE_DIR}/Tools ${PROJECT_BINARY_DIR}/Tools)

# --------------------------------------------------------------------
# Build the SVWidgetsLib Library which holds all the GUI Specific Widgets such
# as the Filter Parameter Widgets and various building block widgets for the GUI applications.
# The plugins are also going to depend on these being built
if(SIMPL_Group_WIDGETS)
  add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/SVWidgetsLib ${PROJECT_BINARY_DIR}/SVWidgetsLib)
endif()

# This should be the last line in this file:
include(${PROJECT_RESOURCES_DIR}/CPack/PackageProject.cmake)



