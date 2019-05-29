#------------------------------------------------------------------------------
# Create the Top level Project
#
project(SIMPLPyBind11 VERSION 0.0.1.0)
#message(STATUS "pybind11_INCLUDE_DIRS: ${pybind11_INCLUDE_DIRS}")

find_package(PythonInterp REQUIRED)


option(SIMPL_ENABLE_ANACONDA_PACKAGE "Build a Conda package" OFF)
if(DEFINED DREAM3DProj_BINARY_DIR)
  set(SIMPL_ANACONDA_OUTPUT_DIR "${DREAM3DProj_BINARY_DIR}/Anaconda")
elseif(TARGET SIMPLView)
  set(SIMPL_ANACONDA_OUTPUT_DIR "${SIMPLView_BINARY_DIR}/Anaconda")
else()
  set(SIMPL_ANACONDA_OUTPUT_DIR "${SIMPLProj_BINARY_DIR}/Anaconda")
endif()

# --------------------------------------------------------------------------
# macro to convert a file system path to compatible C++ strings, i.e., on
# Windows, convert the backslash to forward slash.
macro(ConvertPathToHeaderCompatible INPUT)
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${INPUT} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${INPUT}  )
    endif()
endmacro()

# --------------------------------------------------------------------------
# Convert any needed paths before we configure our header file
set(SIMPLProj_SOURCE_DIR_NORM ${SIMPLProj_SOURCE_DIR})
ConvertPathToHeaderCompatible(${SIMPLProj_SOURCE_DIR_NORM})
set(SIMPLPyBind11_SOURCE_DIR_NORM ${SIMPLPyBind11_SOURCE_DIR})
ConvertPathToHeaderCompatible(${SIMPLPyBind11_SOURCE_DIR_NORM})
set(SIMPLPyBind11_BINARY_DIR_NORM ${SIMPLPyBind11_BINARY_DIR})
ConvertPathToHeaderCompatible(${SIMPLPyBind11_BINARY_DIR_NORM})

set(SIMPLPyBind11_LIBRARY_OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
ConvertPathToHeaderCompatible(${SIMPLPyBind11_LIBRARY_OUTPUT_DIR})

# --------------------------------------------------------------------------
# Configure the header file with build specific paths
set(SIMPL_PYTHON_MODULE_SUFFIX "")
set_property(GLOBAL PROPERTY SIMPL_PYTHON_MODULE_SUFFIX ${SIMPL_PYTHON_MODULE_SUFFIX})
set(SIMPL_PYTHON_MODULE_NAME "dream3d")
set(PYTHON_SITE_PACKAGES_NAME "site-packages")
set_property(GLOBAL PROPERTY PYTHON_SITE_PACKAGES_NAME ${PYTHON_SITE_PACKAGES_NAME})
set(TEST_SCRIPT_FILE_EXT "sh")
if(WIN32)
  set(TEST_SCRIPT_FILE_EXT "bat")
endif()
set_property(GLOBAL PROPERTY SIMPL_PYTHON_TEST_SCRIPT "${SIMPLPyBind11_SOURCE_DIR}/python_test_driver.${TEST_SCRIPT_FILE_EXT}")
# --------------------------------------------------------------------------
# Add an executable that will generate our python bindings
add_executable(GeneratePythonBindings)

get_target_property(GeneratePythonBindings_RUNTIME_OUTPUT_DIR GeneratePythonBindings RUNTIME_OUTPUT_DIRECTORY)
get_filename_component(SIMPL_PYB11_PARENT_DIR  ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
get_filename_component(SIMPL_PYB11_PARENT_DIR  ${SIMPL_PYB11_PARENT_DIR} DIRECTORY)

ConvertPathToHeaderCompatible(${SIMPL_ANACONDA_OUTPUT_DIR})
configure_file( ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/SIMPLPyBind11Config.h.in
                ${SIMPLPyBind11_BINARY_DIR}/CodeScraper/SIMPLPyBind11Config.h
                )
file(TO_NATIVE_PATH "${SIMPL_ANACONDA_OUTPUT_DIR}" SIMPL_ANACONDA_OUTPUT_DIR)
set_property(GLOBAL PROPERTY SIMPL_ANACONDA_OUTPUT_DIR "${SIMPL_ANACONDA_OUTPUT_DIR}")
                


target_sources(GeneratePythonBindings PRIVATE ${SIMPLPyBind11_BINARY_DIR}/CodeScraper/SIMPLPyBind11Config.h)

target_sources(GeneratePythonBindings PRIVATE
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/CodeScraper.cpp
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/CodeScraperConstants.h
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PythonBindingClass.h
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PythonBindingClass.cpp
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PythonBindingsModule.h
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PythonBindingsModule.cpp
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PyBind11Generator.h
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PyBind11Generator.cpp
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PythonUtils.h
)
target_include_directories(GeneratePythonBindings 
                          PRIVATE
                            ${SIMPLProj_SOURCE_DIR}/Source
                            ${SIMPLPyBind11_BINARY_DIR}
                            )

target_link_libraries(GeneratePythonBindings Qt5::Core)

# --------------------------------------------------------------------------
# This section starts the main pybind11 code
include("${SIMPLPyBind11_SOURCE_DIR}/cmake/WrappingFunctions.cmake")


CreatePybind11Module(MODULE_NAME ${SIMPL_PYTHON_MODULE_NAME}
                    SOURCE_DIR "${SIMPLProj_SOURCE_DIR_NORM}/Source/SIMPLib"
                    PATH_TO_STRIP "SIMPL/Source/"
                    OUTPUT_PATH "${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore"
                    BINARY_DIR "${SIMPLProj_BINARY_DIR}"
                    MODULE_TEMPLATE_FILE "${SIMPLPyBind11_SOURCE_DIR}/Templates/SIMPLModuleCodeTemplate.in.cpp"
                    MODULE_LINK_LIBRARIES SIMPLib
                    SIMPLIB
)

# --------------------------------------------------------------------------
# Add each of the python scripts as a ctest unit test that should get run
# during the normal CTest execution
# --------------------------------------------------------------------------
if(SIMPL_BUILD_TESTING)

  if("${ANACONDA_DIR}" STREQUAL "")
    set(ANACONDA_DIR "" CACHE STRING "")
    message(FATAL_ERROR "Please set the ANACONDA_DIR variable either through the CMake-GUI or through the -DANACONDA_DIR=/path/to/Anaconda3")
  endif()
  if(NOT SIMPL_ENABLE_ANACONDA_PACKAGE)
    message(FATAL_ERROR "Please SIMPL_ENABLE_ANACONDA_PACKAGE=ON in order for the Python unit tests to execute correctly.")
  endif()

  #------------------------------
  # Initialize the PYTHON_TEST_INPUT_DIR variable to point to the "Testing Directory"
  file(TO_NATIVE_PATH "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Testing" PYTHON_TEST_INPUT_DIR)

  set(SIMPL_PYTHON_TESTS
    AbstractFilterTest
    AttributeMatrixTest
    DataArrayTest
    DataContainerArrayTest
    DataContainerTest
    GeometryTest
    ImageReadTest
    TestBindings

    Create_Edge_Geometry
    Create_Quad_Geometry
    Create_RectilinearGrid
    Create_Triangle_Geometry
    Create_Vertex_Geometry
    
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
  )

  foreach(test ${SIMPL_PYTHON_TESTS})
    set(SIMPL_PYTHON_TEST_DRIVER "${SIMPL_ANACONDA_OUTPUT_DIR}/UnitTest/SIMPL/${test}.${TEST_SCRIPT_FILE_EXT}")
    configure_file(${SIMPLPyBind11_SOURCE_DIR}/python_test_driver.${TEST_SCRIPT_FILE_EXT}
                  "${SIMPL_PYTHON_TEST_DRIVER}" @ONLY)
    add_test(NAME PY_SIMPL_${test} COMMAND "${SIMPL_PYTHON_TEST_DRIVER}" )
  endforeach(test ${SIMPL_PYTHON_TESTS})
  
endif()

# --------------------------------------------------------------------------
# Copy the recipe and egg info for the building Conda package to the 
# output directory
if (SIMPL_ENABLE_ANACONDA_PACKAGE)
  if("${ANACONDA_DIR}" STREQUAL "" OR "${ANACONDA_ENVIRONMENT_NAME}" STREQUAL "")
    message(FATAL_ERROR "You have enabled Anaconda3 style packaging by setting SIMPL_ENABLE_ANACONDA_PACKAGE=ON, but some required variables are empty or OFF.\
    The following CMake variables need to be set either through the CMake-Gui, ccmake or the command line:\n\
    [VAR NAME] = [CURRENT VALUE]\n\
    ANACONDA_DIR = ${ANACONDA_DIR}\n\
    ANACONDA_ENVIRONMENT_NAME = ${ANACONDA_ENVIRONMENT_NAME}\n\
    SIMPL_ENABLE_ANACONDA_PACKAGE = ${SIMPL_ENABLE_ANACONDA_PACKAGE}\n\
    SIMPL_ENABLE_ANACONDA_LOCAL_INSTALL = ${SIMPL_ENABLE_ANACONDA_LOCAL_INSTALL}\n\
    **********************************************************\n\
    ANACONDA_DIR is the top level directory to Anaconda3 such as /usr/local/anaconda3 or C:/Anaconda3\n\
    ANACONDA_ENVIRONMENT_NAME is the virtual environment name. Leaving this blank might work....\n\
    SIMPL_ENABLE_ANACONDA_PACKAGE This will enable the creation of a script that will create an Anaconda3 package for DREAM3D/SIMPL\n\
    SIMPL_ENABLE_ANACONDA_LOCAL_INSTALL Will install the python package to the ANACONDA_ENVIRONMENT_NAME name on your local computer. Use this with caution.\n\
    ")
  endif()
  #if()
  #  set(ANACONDA_ENVIRONMENT_NAME "" CACHE STRING "")
  #  message(FATAL_ERROR "Please set the ANACONDA_ENVIRONMENT_NAME variable either through the CMake-GUI or through the -DANACONDA_ENVIRONMENT_NAME=ANACONDA_ENV_NAME")
  #endif()   
  
    
  set(ANACONDA_SCRIPTS_DIR_NAME "bin")
  if(WIN32)
    set(ANACONDA_SCRIPTS_DIR_NAME "Scripts")
  endif()

  file(MAKE_DIRECTORY ${SIMPL_ANACONDA_OUTPUT_DIR}/UnitTest)
  file(MAKE_DIRECTORY ${SIMPL_ANACONDA_OUTPUT_DIR}/${PYTHON_SITE_PACKAGES_NAME}/${SIMPL_PYTHON_MODULE_NAME})
  
  file(TO_NATIVE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} CROD_NATIVE)
  file(TO_NATIVE_PATH ${SIMPL_ANACONDA_OUTPUT_DIR} SCOD_NATIVE)
  file(TO_NATIVE_PATH ${ANACONDA_DIR} PCD_NATIVE)

  if(WIN32)
    configure_file(${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Conda/package.in.bat
                ${SIMPL_ANACONDA_OUTPUT_DIR}/package.bat @ONLY)
    add_custom_target(CreateAnacondaPackage 
                "${SIMPL_ANACONDA_OUTPUT_DIR}/package.bat" 
                DEPENDS ${SIMPL_PYTHON_MODULE_NAME}
                WORKING_DIRECTORY ${SIMPL_ANACONDA_OUTPUT_DIR}
                COMMENT "Building an Anaconda package for ${SIMPL_PYTHON_MODULE_NAME}"
    )
  else()
    configure_file(${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Conda/macOS_package.in.sh
                ${SIMPL_ANACONDA_OUTPUT_DIR}/package.sh @ONLY)
    add_custom_target(CreateAnacondaPackage 
                "${SIMPL_ANACONDA_OUTPUT_DIR}/package.sh" 
                DEPENDS ${SIMPL_PYTHON_MODULE_NAME}
                WORKING_DIRECTORY ${SIMPL_ANACONDA_OUTPUT_DIR}
                COMMENT "Building an Anaconda package for ${SIMPL_PYTHON_MODULE_NAME}"
                )
  endif()

  #--- Copy the Anaconda Recipe files
  set(conda_recipe_files bld.bat build.sh LICENSE meta.yaml setup.py)
  foreach(f ${conda_recipe_files})
    configure_file("${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Conda/conda-recipe/${f}"
                    "${SIMPL_ANACONDA_OUTPUT_DIR}/conda-recipe/${f}" @ONLY)
  endforeach(f ${conda_recipe_files})
  
  #--- Copy the Anaconda Egg Files
  set(conda_egg_files MANIFEST.in PKG-INFO ReadMe.md setup.cfg setup.py)
  foreach(f ${conda_egg_files})
    configure_file("${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Conda/egg-info/${f}"
                    "${SIMPL_ANACONDA_OUTPUT_DIR}/${PYTHON_SITE_PACKAGES_NAME}/${f}" @ONLY)
  endforeach(f ${conda_recipe_files})    


  # option(SIMPL_ENABLE_ANACONDA_LOCAL_INSTALL "Install the dream3d Conda site-package locally" OFF)
  
  # if(SIMPL_ENABLE_ANACONDA_LOCAL_INSTALL)
  #   # Build a new copy of the DREAM3D Conda package in the environments conda-bld folder
  #   add_custom_command(TARGET dream3d POST_BUILD 
  #       COMMAND ${ANACONDA_DIR}/envs/${ANACONDA_ENVIRONMENT_NAME}/${ANACONDA_SCRIPTS_DIR_NAME}/conda build --quiet ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/conda-recipe 
  #       COMMAND ${ANACONDA_DIR}/envs/${ANACONDA_ENVIRONMENT_NAME}/${ANACONDA_SCRIPTS_DIR_NAME}/conda build purge)
  #   add_custom_command(TARGET dream3d POST_BUILD 
  #     COMMAND ${ANACONDA_DIR}/envs/${ANACONDA_ENVIRONMENT_NAME}/${ANACONDA_SCRIPTS_DIR_NAME}/conda remove -y dream3d 
  #     COMMENT "Removing any existing dream3d Conda package")
  #   add_custom_command(TARGET dream3d POST_BUILD COMMAND ${ANACONDA_DIR}/envs/${ANACONDA_ENVIRONMENT_NAME}/${ANACONDA_SCRIPTS_DIR_NAME}/conda install -y --use-local dream3d 
  #     COMMENT "Installing the dream3d Conda package")
  # endif()



endif()


