#------------------------------------------------------------------------------
# Create the Top level Project
#
project(SIMPLPyBind11 VERSION 0.0.1.0)

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

# --------------------------------------------------------------------------
# Configure the header file with system specific paths
configure_file( ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/SIMPLPyBind11Config.h.in
                ${SIMPLPyBind11_BINARY_DIR}/CodeScraper/SIMPLPyBind11Config.h)

# --------------------------------------------------------------------------
# Configure a python file that has the location of the build directory
# configure_file( ${SIMPLPyBind11_SOURCE_DIR}/Testing/Python/SIMPL_Build_Location.py.in
#                 ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python/site-packages/simplutil/SIMPL_Build_Location.py)
set(SIMPLib_PyInit_File "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python/site-packages/simplutil/__init__.py")
file(WRITE "${SIMPLib_PyInit_File}" "\"\"\" Utility module for the Python tests  \"\"\"\n\n")
#file(APPEND "${SIMPLib_PyInit_File}" "__all__ = ['SIMPL_Build_Location']\n\n")
file(APPEND "${SIMPLib_PyInit_File}" "def GetBuildDirectory():\n")
file(APPEND "${SIMPLib_PyInit_File}" "  return \"${SIMPLProj_BINARY_DIR}\"\n")
file(APPEND "${SIMPLib_PyInit_File}" "\n")
file(APPEND "${SIMPLib_PyInit_File}" "def GetTestDirectory():\n")
file(APPEND "${SIMPLib_PyInit_File}" "  return \"${SIMPLProj_BINARY_DIR}/Testing\"\n")
file(APPEND "${SIMPLib_PyInit_File}" "\n")
file(APPEND "${SIMPLib_PyInit_File}" "def GetTestTempDirectory():\n")
file(APPEND "${SIMPLib_PyInit_File}" "  return \"${SIMPLProj_BINARY_DIR}/Testing/Temporary\"\n")
file(APPEND "${SIMPLib_PyInit_File}" "\n")
file(APPEND "${SIMPLib_PyInit_File}" "def GetDataDirectory():\n")
file(APPEND "${SIMPLib_PyInit_File}" "  return \"${DREAM3D_DATA_DIR}\"\n")
file(APPEND "${SIMPLib_PyInit_File}" "\n")

if(0)
  set(SIMPLib_PySetup_File "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python/site-packages/simplutil/setup.py")
  file(WRITE  "${SIMPLib_PySetup_File}" "\n")
  file(APPEND "${SIMPLib_PySetup_File}" "from setuptools import setup\n")
  file(APPEND "${SIMPLib_PySetup_File}" "\n")
  file(APPEND "${SIMPLib_PySetup_File}" "setup(name='simplutil',\n")
  file(APPEND "${SIMPLib_PySetup_File}" "      version='0.1',\n")
  file(APPEND "${SIMPLib_PySetup_File}" "      description='Python Utilities to find various directories in the build',\n")
  file(APPEND "${SIMPLib_PySetup_File}" "      url='http://github.com/bluequartzsoftware/SIMPL',\n")
  file(APPEND "${SIMPLib_PySetup_File}" "      author='BlueQuartz Software',\n")
  file(APPEND "${SIMPLib_PySetup_File}" "      author_email='info@bluequartz.net',\n")
  file(APPEND "${SIMPLib_PySetup_File}" "      license='BSD',\n")
  file(APPEND "${SIMPLib_PySetup_File}" "      packages=['simplutil'],\n")
  file(APPEND "${SIMPLib_PySetup_File}" "      zip_safe=False)\n")
  file(APPEND "${SIMPLib_PySetup_File}" "\n")
endif()

# --------------------------------------------------------------------------
# Add an executable that will generate our python bindings
add_executable(GeneratePythonBindings ${SIMPLPyBind11_BINARY_DIR}/CodeScraper/SIMPLPyBind11Config.h)

target_sources(GeneratePythonBindings PRIVATE
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/CodeScraper.cpp
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/CodeScraperConstants.h
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PythonBindingClass.h
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PythonBindingClass.cpp
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PythonBindingsModule.h
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PythonBindingsModule.cpp
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PyBind11Generator.h
  ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/PyBind11Generator.cpp
)
target_include_directories(GeneratePythonBindings 
                          PRIVATE
                            ${SIMPLProj_SOURCE_DIR}/Source
                            ${SIMPLPyBind11_BINARY_DIR}
                            )

target_link_libraries(GeneratePythonBindings Qt5::Core)

set(pybind_module_name "simpl")

add_custom_target(SIMPLCreatePythonBindings ALL
    COMMAND GeneratePythonBindings "${SIMPLProj_SOURCE_DIR_NORM}/Source/SIMPLib" "SIMPL/Source/" "${pybind_module_name}" "${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore"
    DEPENDS ${COPY_LIBRARY_TARGETS} GeneratePythonBindings
    COMMENT "Pybind11: Creating Python Bindings for SIMPL"
  )


# --------------------------------------------------------------------------
# Find the Pybind11 installation
set(PYBIND11_PYTHON_VERSION "3.6")
find_package(PyBind11)


# --------------------------------------------------------------------------
# Now compile the Python module
pybind11_add_module(${pybind_module_name} ${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.cxx)
set_source_files_properties(${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.cxx
                              PROPERTIES  GENERATED TRUE
                                          SKIP_AUTOMOC ON)
set_target_properties(${pybind_module_name} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${pybind_module_name} PUBLIC
  #$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${SIMPLib_SOURCE_DIR}
  $<BUILD_INTERFACE:${${pybind_module_name}_BINARY_DIR}
  )
target_link_libraries(${pybind_module_name} LINK_PUBLIC SIMPLib 
                                LINK_PRIVATE pybind11::module)
target_compile_features(${pybind_module_name} PRIVATE cxx_local_type_template_args)
set_target_properties(${pybind_module_name} PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                          SUFFIX "${PYTHON_MODULE_EXTENSION}"
                                          LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python/site-packages
                                          )
add_dependencies(${pybind_module_name} SIMPLCreatePythonBindings)                                         

                                          
install (TARGETS ${pybind_module_name} 
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)


