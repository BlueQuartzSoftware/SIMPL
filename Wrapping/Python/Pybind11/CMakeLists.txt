#------------------------------------------------------------------------------
# Create the Top level Project
#
project(SIMPLPyBind11 VERSION 0.0.1.0)
#message(STATUS "pybind11_INCLUDE_DIRS: ${pybind11_INCLUDE_DIRS}")

find_package(PythonInterp REQUIRED)


option(SIMPL_PYTHON_BUILD_CONDA_PACKAGE "Build a Conda package" OFF)
if(DEFINED DREAM3DProj_BINARY_DIR)
  set(SIMPL_CONDA_OUTPUT_DIR "${DREAM3DProj_BINARY_DIR}/Anaconda")
elseif(TARGET SIMPLView)
  set(SIMPL_CONDA_OUTPUT_DIR "${SIMPLView_BINARY_DIR}/Anaconda")
else()
  set(SIMPL_CONDA_OUTPUT_DIR "${SIMPLProj_BINARY_DIR}/Anaconda")
endif()
set_property(GLOBAL PROPERTY SIMPL_CONDA_OUTPUT_DIR "${SIMPL_CONDA_OUTPUT_DIR}")


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
# Copy the recipe and egg info for the building Conda package to the 
# output directory
if (SIMPL_PYTHON_BUILD_CONDA_PACKAGE)
  if("${PYTHON_CONDA_DIRECTORY}" STREQUAL "")
    message(FATAL_ERROR "Please set the PYTHON_CONDA_DIRECTORY variable either through the CMake-GUI or through the -DPYTHON_CONDA_DIRECTORY=/path/to/Anaconda3")
  endif()
  if("${PYTHON_CONDA_ENVIRONMENT_NAME}" STREQUAL "")
    message(FATAL_ERROR "Please set the PYTHON_CONDA_ENVIRONMENT_NAME variable either through the CMake-GUI or through the -DPYTHON_CONDA_ENVIRONMENT_NAME=ANACONDA_ENV_NAME")
  endif()   
  
  option(SIMPL_PYTHON_INSTALL_CONDA_PACKAGE_LOCALLY "Install the dream3d Conda package locally" OFF)

    
  set(CONDA_SCRIPTS_DIR_NAME "bin")
  if(WIN32)
    set(CONDA_SCRIPTS_DIR_NAME "Scripts")
  endif()

  file(MAKE_DIRECTORY ${SIMPL_CONDA_OUTPUT_DIR}/UnitTest)
  file(MAKE_DIRECTORY ${SIMPL_CONDA_OUTPUT_DIR}/${PYTHON_SITE_PACKAGES_NAME}/${SIMPL_PYTHON_MODULE_NAME})
  
  file(TO_NATIVE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} CROD_NATIVE)
  file(TO_NATIVE_PATH ${SIMPL_CONDA_OUTPUT_DIR} SCOD_NATIVE)

  if(WIN32)
    configure_file(${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Conda/package.in.bat
                ${SIMPL_CONDA_OUTPUT_DIR}/package.bat @ONLY)
    add_custom_target(CreateAnacondaPackage 
                "${SIMPL_CONDA_OUTPUT_DIR}/package.bat" 
                DEPENDS ${SIMPL_PYTHON_MODULE_NAME}
                WORKING_DIRECTORY ${SIMPL_CONDA_OUTPUT_DIR}
                COMMENT "Building an Anaconda package for ${SIMPL_PYTHON_MODULE_NAME}"
    )
  else()
    configure_file(${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Conda/package.in.sh
                ${SIMPL_CONDA_OUTPUT_DIR}/package.sh @ONLY)
    add_custom_target(CreateAnacondaPackage 
                "${SIMPL_CONDA_OUTPUT_DIR}/package.sh" 
                DEPENDS ${SIMPL_PYTHON_MODULE_NAME}
                WORKING_DIRECTORY ${SIMPL_CONDA_OUTPUT_DIR}
                COMMENT "Building an Anaconda package for ${SIMPL_PYTHON_MODULE_NAME}"
                )
  endif()

  #--- Copy the Anaconda Recipe files
  set(conda_recipe_files bld.bat build.sh LICENSE meta.yaml setup.py)
  foreach(f ${conda_recipe_files})
    configure_file("${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Conda/conda-recipe/${f}"
                    "${SIMPL_CONDA_OUTPUT_DIR}/conda-recipe/${f}" @ONLY)
  endforeach(f ${conda_recipe_files})
  
  #--- Copy the Anaconda Egg Files
  set(conda_egg_files MANIFEST.in PKG-INFO ReadMe.md setup.cfg setup.py)
  foreach(f ${conda_egg_files})
    configure_file("${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Conda/egg-info/${f}"
                    "${SIMPL_CONDA_OUTPUT_DIR}/${PYTHON_SITE_PACKAGES_NAME}/${f}" @ONLY)
  endforeach(f ${conda_recipe_files})    


  option(SIMPL_PYTHON_INSTALL_CONDA_PACKAGE_LOCALLY "Install the dream3d Conda site-package locally" OFF)
  
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
