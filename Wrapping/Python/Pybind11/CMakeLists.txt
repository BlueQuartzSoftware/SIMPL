#------------------------------------------------------------------------------
# Create the Top level Project
#
project(SIMPLPyBind11 VERSION 0.0.1.0)
#message(STATUS "pybind11_INCLUDE_DIRS: ${pybind11_INCLUDE_DIRS}")

find_package(PythonInterp REQUIRED)

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


# --------------------------------------------------------------------------
# Add an executable that will generate our python bindings
add_executable(GeneratePythonBindings)

get_target_property(GeneratePythonBindings_RUNTIME_OUTPUT_DIR GeneratePythonBindings RUNTIME_OUTPUT_DIRECTORY)
get_filename_component(SIMPL_PYB11_PARENT_DIR  ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
get_filename_component(SIMPL_PYB11_PARENT_DIR  ${SIMPL_PYB11_PARENT_DIR} DIRECTORY)

configure_file( ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/SIMPLPyBind11Config.h.in
                ${SIMPLPyBind11_BINARY_DIR}/CodeScraper/SIMPLPyBind11Config.h
                )

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
# This section sets up the Conda package building to run after the Python wrapping
if (SIMPL_PYTHON_BUILD_CONDA_PACKAGE)    
  set(CONDA_SCRIPTS_DIR_NAME "bin")
  if(WIN32)
    set(CONDA_SCRIPTS_DIR_NAME "Scripts")
  endif()
  set(PYTHON_CONDA_DIRECTORY "" CACHE PATH "The location of the Conda installation Scripts/bin directory")
  set(PYTHON_CONDA_ENVIRONMENT_NAME "" CACHE STRING "The name of the environment for building the Conda package")
  option(SIMPL_PYTHON_INSTALL_CONDA_PACKAGE_LOCALLY "Install the dream3d Conda package locally" OFF)
  add_custom_command(TARGET dream3d POST_BUILD COMMAND ${PYTHON_CONDA_DIRECTORY}/${CONDA_SCRIPTS_DIR_NAME}/activate ${PYTHON_CONDA_ENVIRONMENT_NAME} COMMENT "Preparing to build the Conda package of DREAM3D using Conda environment: ${PYTHON_CONDA_ENVIRONMENT_NAME} and Conda installation: ${PYTHON_CONDA_DIRECTORY}. Activating the environment...")
  
  #add_custom_command(TARGET dream3d POST_BUILD COMMAND ${PYTHON_CONDA_DIRECTORY}/envs/py365/${CONDA_SCRIPTS_DIR_NAME}/conda install conda-build COMMENT "Ensuring conda-build is installed...")
  add_custom_command(TARGET dream3d POST_BUILD 
      COMMAND ${PYTHON_CONDA_DIRECTORY}/${CONDA_SCRIPTS_DIR_NAME}/conda install conda-build 
      COMMENT "Ensuring conda-build is installed...")
  
  add_custom_target(MakeCondaPackageDirectory ALL 
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/conda-package)  
  
  add_custom_command(TARGET dream3d POST_BUILD 
    COMMAND ${PYTHON_CONDA_DIRECTORY}/envs/${PYTHON_CONDA_ENVIRONMENT_NAME}/${CONDA_SCRIPTS_DIR_NAME}/conda build --quiet --output-folder ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/conda-package ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/conda-recipe 
    COMMAND ${PYTHON_CONDA_DIRECTORY}/envs/${PYTHON_CONDA_ENVIRONMENT_NAME}/${CONDA_SCRIPTS_DIR_NAME}/conda build purge 
    COMMENT "Building the Conda package...")

  if(SIMPL_PYTHON_INSTALL_CONDA_PACKAGE_LOCALLY)
    # Build a new copy of the DREAM3D Conda package in the environments conda-bld folder
    add_custom_command(TARGET dream3d POST_BUILD 
        COMMAND ${PYTHON_CONDA_DIRECTORY}/envs/${PYTHON_CONDA_ENVIRONMENT_NAME}/${CONDA_SCRIPTS_DIR_NAME}/conda build --quiet ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/conda-recipe 
        COMMAND ${PYTHON_CONDA_DIRECTORY}/envs/${PYTHON_CONDA_ENVIRONMENT_NAME}/${CONDA_SCRIPTS_DIR_NAME}/conda build purge)
    add_custom_command(TARGET dream3d POST_BUILD 
      COMMAND ${PYTHON_CONDA_DIRECTORY}/envs/${PYTHON_CONDA_ENVIRONMENT_NAME}/${CONDA_SCRIPTS_DIR_NAME}/conda remove -y dream3d 
      COMMENT "Removing any existing dream3d Conda package")
    add_custom_command(TARGET dream3d POST_BUILD COMMAND ${PYTHON_CONDA_DIRECTORY}/envs/${PYTHON_CONDA_ENVIRONMENT_NAME}/${CONDA_SCRIPTS_DIR_NAME}/conda install -y --use-local dream3d 
      COMMENT "Installing the dream3d Conda package")
  endif()
endif()
