
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.1.0")
cmake_policy(SET CMP0002 NEW)
cmake_policy(SET CMP0054 NEW)
endif()

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.12.0")
cmake_policy(SET CMP0074 NEW)
endif()

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13.0")
cmake_policy(SET CMP0079 NEW)
cmake_policy(SET CMP0077 NEW)
endif()

cmake_minimum_required(VERSION 3.10.0)

if(NOT "${DREAM3D_SDK}" STREQUAL "")
  include("${DREAM3D_SDK}/DREAM3D_SDK.cmake")
  get_property(DREAM3D_SDK_STATUS_PRINTED GLOBAL PROPERTY DREAM3D_SDK_STATUS_PRINTED)
  if(NOT DREAM3D_SDK_STATUS_PRINTED)
    message(STATUS "DREAM3D_SDK Location: ${DREAM3D_SDK}")
    set_property(GLOBAL PROPERTY DREAM3D_SDK_STATUS_PRINTED TRUE)
  endif()
endif()

# Request C++17 standard, using new CMake variables.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

cmake_minimum_required(VERSION 3.14.0)

#------------------------------------------------------------------------------
# Create the Top level Project
#
project(SIMPLProj VERSION 2.0.0.0)

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
# If the developer has set another H5Support directory then use that, otherwise look
# for the H5Support directory at the same level as the SIMPL directory
if("${H5Support_SOURCE_DIR}" STREQUAL "")
  set(H5Support_SOURCE_DIR ${SIMPL_PARENT}/H5Support)
  message(STATUS "H5Support_SOURCE_DIR: ${H5Support_SOURCE_DIR}")
endif()

if(NOT EXISTS "${H5Support_SOURCE_DIR}")
  message(FATAL_ERROR "H5Support_SOURCE_DIR does not exist at '${H5Support_SOURCE_DIR}'")
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

# Check relative paths
OPTION (SIMPL_RELATIVE_PATH_CHECK "Check relative paths" OFF)

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
SIMPL_ENABLE_GROUP(GROUP_NAME REST
            ENABLE ON
            DESC "Compile SIMPL REST functionality"
            DEPENDENCIES BASE)
SIMPL_ENABLE_GROUP(GROUP_NAME WIDGETS
            ENABLE ON
            DESC "Compile SIMPL Qt Widgets functionality"
            DEPENDENCIES BASE FILTERS PLUGIN)

OPTION(SIMPL_BUILD_TESTING "Compile the test programs" ON)


# --------------------------------------------------------------------
# Should we use Intel Threading Building Blocks
# --------------------------------------------------------------------
set(SIMPL_USE_PARALLEL_ALGORITHMS "")
option(SIMPL_USE_MULTITHREADED_ALGOS "Use MultiThreaded Algorithms" OFF)
if(SIMPL_USE_MULTITHREADED_ALGOS)
  include(${CMP_SOURCE_DIR}/ExtLib/TBBSupport.cmake)
  set(SIMPL_USE_PARALLEL_ALGORITHMS "1")
endif()

# --------------------------------------------------------------------
# SIMPL needs the Eigen library for Least Squares fit and Eigen value/vector calculations.
set(SIMPL_USE_EIGEN "")
Find_Package(Eigen3 REQUIRED)
if(EIGEN3_FOUND)
  message(STATUS "Eigen3 Location: ${EIGEN3_ROOT_DIR}")
  message(STATUS "Eigen3 Version: ${EIGEN3_VERSION_STRING}")
  set(SIMPL_USE_EIGEN "1" CACHE INTERNAL "")
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
CMP_AddQt5Support( "${SIMPL_Qt5_COMPONENTS}" "${SIMPLProj_BINARY_DIR}" "SIMPL")

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
set(license_install_dir "${DREAM3D_PACKAGE_DEST_PREFIX}")

if(APPLE)
  set(license_install_dir "${APPLE_RESOURCES_DIR}")
endif()

if(DREAM3D_ANACONDA)
  set(license_install_dir "share/DREAM3D")
endif()

install(FILES ${PROJECT_BINARY_DIR}/SIMPL/SIMPLLicense.txt
  DESTINATION ${license_install_dir}
  COMPONENT Applications
)

# -----------------------------------------------------------------------
# Setup a Global property that is used to gather Documentation Information
# into a single known location
get_property(DREAM3DDocRoot GLOBAL PROPERTY DREAM3DDocRoot)
if("${DREAM3DDocRoot}" STREQUAL "")
  set_property(GLOBAL PROPERTY DREAM3DDocRoot "${SIMPLProj_BINARY_DIR}")
endif()
file(WRITE ${SIMPLProj_BINARY_DIR}/DREAM3DDocGroupList "")

# ------- Enable the CTest testing. Use make test to run all tests ---------------
# -------------------------------------------------------------------- 
# If Testing is enabled, turn on the Unit Tests 
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

if(NOT DREAM3D_ANACONDA)
  install(
    FILES
      "${SIMPLProj_SOURCE_DIR}/SIMPLConfig.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/SIMPL/SIMPLTargetsConfigVersion.cmake"
    DESTINATION
      ${ConfigPackageLocation}
    COMPONENT
      Devel
  )
endif()

# -----------------------------------------------------------------------
# Add in the support to host the documentation over a thin-http server
# -----------------------------------------------------------------------
add_subdirectory(${SIMPLProj_SOURCE_DIR}/ThirdParty/QtWebApp )

# -----------------------------------------------------------------------
# This needs to be set here as we are going to look for files in this directory
# -----------------------------------------------------------------------
if("${FilterWidgetsLib_BINARY_DIR}" STREQUAL "")
  set(FilterWidgetsLib_BINARY_DIR ${SIMPLProj_BINARY_DIR}/FilterWidgetsLib)
endif()


# --------------------------------------------------------------------
# Use ITK
option(SIMPL_USE_ITK "Use ITK For Image Processing" ON)
if(SIMPL_USE_ITK)
  # --------------------------------------------------------------------
  # find ITK so that we can figure out the version of ITK the user has selected
  # Does not register IO factories. It will be done by each plugin that needs it.
  set(ITK_NO_IO_FACTORY_REGISTER_MANAGER TRUE)
  find_package(ITK REQUIRED)
  include(${CMP_SOURCE_DIR}/ITKSupport/IncludeITK.cmake)

  # --- ONLY Mac OS using ITK 5 needs SimpleITKExplicit
  get_property(ITK_VERSION_MAJOR GLOBAL PROPERTY ITK_VERSION_MAJOR)
  if(APPLE AND "${ITK_VERSION_MAJOR}" STREQUAL "5")
    add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/SimpleITKExplicit ${PROJECT_BINARY_DIR}/SimpleITKExplicit)
  endif()
endif()

# -----------------------------------------------------------------------
# Determine if we need the ghcFilesystem library
#-------------------------------------------------------------------------------
set(SIMPL_USE_GHC_FILESYSTEM OFF)

if(APPLE)
  exec_program(uname ARGS -v  OUTPUT_VARIABLE DARWIN_VERSION)
  string(REGEX MATCH "[0-9]+" DARWIN_VERSION ${DARWIN_VERSION})
  if(DARWIN_VERSION VERSION_LESS 19)
    message(STATUS "The current macOS System is too old to compile and will fail. Please set the SIMPL_USE_GHC_FILESYSTEM=ON variable to allow Ebsdlib to compile")
  endif()
  message(STATUS "SIMPL: DARWIN_VERSION ${DARWIN_VERSION}")
  message(STATUS "SIMPL: CMAKE_OSX_DEPLOYMENT_TARGET: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
  if(NOT "${CMAKE_OSX_DEPLOYMENT_TARGET}" STREQUAL "" )
    if(CMAKE_OSX_DEPLOYMENT_TARGET VERSION_LESS 10.15)
      message(STATUS "SIMPL: CMAKE_OSX_DEPLOYMENT_TARGET less than 10.15. Enabling EbsdLib_USE_GHC_FILESYSTEM")
      set(SIMPL_USE_GHC_FILESYSTEM ON)
    endif()
  endif()
endif()
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
  set(SIMPL_USE_GHC_FILESYSTEM ON)
endif()


message(STATUS "SIMPL: SIMPL_USE_GHC_FILESYSTEM: ${SIMPL_USE_GHC_FILESYSTEM}")

if(SIMPL_USE_GHC_FILESYSTEM)
  find_package(ghcFilesystem REQUIRED NAMES ghc_filesystem ghcFilesystem)
endif()



# -----------------------------------------------------------------------
# -- Add in the SIMPL Sources into a library
add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/SIMPLib ${PROJECT_BINARY_DIR}/SIMPLib)

# --------------------------------------------------------------------
# add the Command line PipelineRunner
# Figure out here if we are going to build the command line tools
add_subdirectory(${SIMPLProj_SOURCE_DIR}/Source/PipelineRunner ${PROJECT_BINARY_DIR}/PipelineRunner)

option(SIMPL_BUILD_CLI_TOOLS "Build the command line versions of the applications" ON)
if( SIMPL_BUILD_CLI_TOOLS )
  # --------------------------------------------------------------------
  # add the Command line MakeFilterUuid
  # Figure out here if we are going to build the command line tools
  add_subdirectory(${SIMPLProj_SOURCE_DIR}/Source/MakeFilterUuid ${PROJECT_BINARY_DIR}/MakeFilterUuid)
endif()

# --------------------------------------------------------------------
# add the Command line PipelineRunner
option(SIMPL_BUILD_EXPERIMENTAL "Build experimental codes." OFF)
if(SIMPL_BUILD_EXPERIMENTAL)
  include(${SIMPLProj_SOURCE_DIR}/Source/Experimental/SourceList.cmake)
endif()

# --------------------------------------------------------------------
# Build the SVWidgetsLib Library which holds all the GUI Specific Widgets such
# as the Filter Parameter Widgets and various building block widgets for the GUI applications.
# The plugins are also going to depend on these being built
if(SIMPL_Group_WIDGETS)
  add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/SVWidgetsLib ${PROJECT_BINARY_DIR}/SVWidgetsLib)
endif()

# --------------------------------------------------------------------
# Include any REST related CMake codes
if(SIMPL_Group_REST)
  add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/RESTClient ${PROJECT_BINARY_DIR}/RESTClient)
  add_subdirectory( ${SIMPLProj_SOURCE_DIR}/Source/RESTServer ${PROJECT_BINARY_DIR}/RESTServer)
endif()

# --------------------------------------------------------------------
# This should be the last line in this file:
include(${PROJECT_RESOURCES_DIR}/CPack/PackageProject.cmake)

# --------------------------------------------------------------------
# Include any Support related CMake codes
include(${SIMPLProj_SOURCE_DIR}/Support/Support.cmake)

# --------------------------------------------------------------------------
# Find the Pybind11 installation
option(SIMPL_WRAP_PYTHON "Generate and Compile Python wrapping codes" OFF)
include(CMakeDependentOption)
cmake_dependent_option(SIMPL_EMBED_PYTHON "Embed the Python interpreter in SIMPL and allow for loading filters from Python" OFF SIMPL_WRAP_PYTHON OFF)
if(SIMPL_EMBED_PYTHON)
  if(APPLE)
    message(WARNING "Embedding Python is experimental on macOS")
  endif()
endif()
if(SIMPL_WRAP_PYTHON)
  set(SIMPL_PYTHON_TEST_ENV ${SIMPL_PYTHON_TEST_ENV} CACHE STRING "A semicolon delimited list of environment variables used for Python tests")

  option(SIMPL_USE_ANACONDA_PYTHON "Whether the selected Python executable is part of an Anaconda environment" OFF)

  mark_as_advanced(CLEAR PYTHON_EXECUTABLE)

  if(SIMPL_USE_ANACONDA_PYTHON)
    if(NOT ANACONDA_DIR OR NOT ANACONDA_ENVIRONMENT_NAME)
      set(ANACONDA_DIR "ANACONDA_DIR-NOTFOUND" CACHE PATH "Path to Anaconda directory")
      set(ANACONDA_ENVIRONMENT_NAME "ANACONDA_ENVIRONMENT_NAME-NOTFOUND" CACHE STRING "Anaconda virtual environment to use")
      set(ANACONDA_ERROR_MESSAGE "ANACONDA_DIR = ${ANACONDA_DIR}\nANACONDA_ENVIRONMENT_NAME = ${ANACONDA_ENVIRONMENT_NAME}\n")
      if(NOT ANACONDA_DIR)
        string(APPEND ANACONDA_ERROR_MESSAGE "ANACONDA_DIR is not set.\n")
      endif()
      if(NOT ANACONDA_ENVIRONMENT_NAME)
        string(APPEND ANACONDA_ERROR_MESSAGE "ANACONDA_ENVIRONMENT_NAME is not set.\n")
      endif()
      message(FATAL_ERROR ${ANACONDA_ERROR_MESSAGE})
    endif()

    if(WIN32)
      set(SIMPL_CONDA_EXECUTABLE ${ANACONDA_DIR}/condabin/conda.bat CACHE FILEPATH "Path to conda executable")
    else()
      set(SIMPL_CONDA_EXECUTABLE ${ANACONDA_DIR}/condabin/conda CACHE FILEPATH "Path to conda executable")
    endif()

    mark_as_advanced(SIMPL_CONDA_EXECUTABLE)
  endif()

  option(SIMPL_GENERATE_PYI "Generate .pyi files for python bindings" OFF)

  if(SIMPL_GENERATE_PYI)
    find_program(MYPY_STUBGEN_EXE stubgen REQUIRED)
  endif()

  include(${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Binding/CreatePybind11Module.cmake)

  get_target_property(SIMPL_PY_SOURCES SIMPLib SOURCES)

  set(SIMPL_PY_SOURCES_FILE ${SIMPLProj_BINARY_DIR}/SIMPL_py_sources.txt)

  file(WRITE ${SIMPL_PY_SOURCES_FILE} "${SIMPL_PY_SOURCES}")

  set(PY_ALL_MODULES_GENERATOR ${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Binding/generate_all_modules_file.py)
  set(PY_ALL_MODULES_FILE ${SIMPLProj_BINARY_DIR}/Wrapping/all_modules.txt)
  set_property(GLOBAL PROPERTY PY_ALL_MODULES_FILE ${PY_ALL_MODULES_FILE})

  file(WRITE ${PY_ALL_MODULES_FILE} "")

  get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

  if(NOT DREAM3D_ANACONDA)
    add_custom_target(CreateDream3dModuleFile ALL
      COMMAND ${PYTHON_EXECUTABLE} ${PY_ALL_MODULES_GENERATOR}
      "${PY_ALL_MODULES_FILE}"
      "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}$<${_isMultiConfig}:/$<CONFIG>>"
      COMMENT "Creating dream3d.py module file"
    )

    set_property(TARGET CreateDream3dModuleFile PROPERTY FOLDER "Python")
  endif()

  CreatePybind11Module(MODULE_NAME "simpl"
    OUTPUT_DIR "${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore"
    FILE_LIST_PATH ${SIMPL_PY_SOURCES_FILE}
    SOURCE_DIR "${SIMPLProj_SOURCE_DIR}/Source"
    HEADER_PATH "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Binding/pySIMPLHeader.cpp"
    BODY_PATH "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Binding/pySIMPLBody.cpp"
    BODY_TOP_PATH "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Binding/pySIMPLBodyTop.cpp"
    POST_TYPES_PATH "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Binding/pySIMPLPostTypes.cpp"
    PYTHON_OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}$<${_isMultiConfig}:/$<CONFIG>>"
    LINK_LIBRARIES SIMPLib
    INCLUDE_DIRS ${SIMPLProj_SOURCE_DIR}/Wrapping/Python
  )

  set(SIMPL_HELPERS_PY_FILE ${SIMPLProj_SOURCE_DIR}/Wrapping/Python/SIMPL/simpl_helpers.py)

  if(DREAM3D_ANACONDA)
    configure_file(${SIMPL_HELPERS_PY_FILE} ${DREAM3D_PYTHON_PACKAGE_DIR}/simpl_helpers.py COPYONLY)
    file(APPEND ${DREAM3D_INIT_PY_FILE} "from . import simpl_helpers\n")
  else()
    add_custom_target(CopyPythonSupport ALL
      COMMAND ${CMAKE_COMMAND} -E copy
        ${SIMPL_HELPERS_PY_FILE}
        "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}$<${_isMultiConfig}:/$<CONFIG>>"
      COMMENT "Copying simpl_helpers.py to binary directory"
    )

    add_dependencies(simpl CopyPythonSupport)

    set_property(TARGET CopyPythonSupport PROPERTY FOLDER "ZZ_COPY_FILES")
  endif()

  if(SIMPL_BUILD_TESTING)
    file(READ ${SIMPLProj_SOURCE_DIR}/Wrapping/Python/SIMPL/simpl_test_dirs.in.py SIMPL_TEST_DIRS_FILE)

    string(REPLACE "\${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" $<TARGET_FILE_DIR:simpl> SIMPL_TEST_DIRS_FILE ${SIMPL_TEST_DIRS_FILE})
    string(REPLACE "\${SIMPLProj_BINARY_DIR}" ${SIMPLProj_BINARY_DIR} SIMPL_TEST_DIRS_FILE ${SIMPL_TEST_DIRS_FILE})
    string(REPLACE "\${DREAM3D_DATA_DIR}" ${DREAM3D_DATA_DIR} SIMPL_TEST_DIRS_FILE ${SIMPL_TEST_DIRS_FILE})

    file(GENERATE
      OUTPUT "$<TARGET_FILE_DIR:simpl>/simpl_test_dirs.py"
      CONTENT ${SIMPL_TEST_DIRS_FILE}
    )

    set(PYTHON_TEST_INPUT_DIR "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Testing")

    set(SIMPL_PYTHON_TESTS
      "AbstractFilterTest"
      "AttributeMatrixTest"
      "DataArrayTest"
      "DataContainerArrayTest"
      "DataContainerTest"
      "GeometryTest"
      "ImageReadTest"
      "TestBindings"

      "Create_Edge_Geometry"
      "Create_Quad_Geometry"
      "Create_RectilinearGrid"
      "Create_Triangle_Geometry"
      "Create_Vertex_Geometry"
      
      "Array_Calculator_Test"
      "Convert_Attribute_Array_Data_Type"
      "Copy_Object_Test"
      "Crop_Vertex_Geometry"
      "Export_Triangle_Geometry"
      "Import_Ascii_Data_Array"
      "Import_Hdf5_Dataset"
      "Remove_Component_From_Array"
      "Rename_Attribute_Array"
      "Split_Attribute_Array"
      "NumPy_Round_Trip"
    )

    CreatePythonTests(PREFIX "PY_SIMPL"
      INPUT_DIR ${PYTHON_TEST_INPUT_DIR}
      TEST_NAMES ${SIMPL_PYTHON_TESTS}
    )
  endif()
endif()
