#------------------------------------------------------------------------------
# Create the Top level Project
#
project(SIMPLPyBind11 VERSION 0.0.1.0)
#message(STATUS "pybind11_INCLUDE_DIRS: ${pybind11_INCLUDE_DIRS}")

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

set(SIMPLPyBind11_LIBRARY_OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python/site-packages")
ConvertPathToHeaderCompatible(${SIMPLPyBind11_LIBRARY_OUTPUT_DIR})

# --------------------------------------------------------------------------
# Configure the header file with build specific paths
set(SIMPL_PYTHON_MODULE_SUFFIX "_py")
set_property(GLOBAL PROPERTY SIMPL_PYTHON_MODULE_SUFFIX ${SIMPL_PYTHON_MODULE_SUFFIX})
set(SIMPL_PYTHON_MODULE_NAME "simpl")

configure_file( ${SIMPLPyBind11_SOURCE_DIR}/CodeScraper/SIMPLPyBind11Config.h.in
                ${SIMPLPyBind11_BINARY_DIR}/CodeScraper/SIMPLPyBind11Config.h
                )
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


# --------------------------------------------------------------------------
# Configure a python __init__.py file for simpl_dirs                
set(SIMPLib_PyInit_File "${SIMPLPyBind11_LIBRARY_OUTPUT_DIR}/simpl_dirs/__init__.py")
configure_file( ${SIMPLPyBind11_SOURCE_DIR}/Testing/Python/simpl_dirs.in.py
                "${SIMPLib_PyInit_File}"
)

# --------------------------------------------------------------------------
# Configure a python __init__.py file for simpl_common                
set(SIMPLib_PyInit_File "${SIMPLPyBind11_LIBRARY_OUTPUT_DIR}/simpl_common/__init__.py")
configure_file( ${SIMPLPyBind11_SOURCE_DIR}/Testing/Python/simpl_common.in.py
                "${SIMPLib_PyInit_File}"
)

# --------------------------------------------------------------------------
# Configure a python setup.py file  
set(SIMPLib_PySetup_File "${SIMPLPyBind11_LIBRARY_OUTPUT_DIR}/setup.py")
configure_file( ${SIMPLPyBind11_SOURCE_DIR}/Testing/Python/setup.py
                "${SIMPLib_PySetup_File}"
              )

# --------------------------------------------------------------------------
# This section starts the main pybind11 code
include("${SIMPLProj_SOURCE_DIR}/Wrapping/Tools/Pybind11/cmake/WrappingFunctions.cmake")


CreatePybind11Module(MODULE_NAME ${SIMPL_PYTHON_MODULE_NAME}
                    SOURCE_DIR "${SIMPLProj_SOURCE_DIR_NORM}/Source/SIMPLib"
                    PATH_TO_STRIP "SIMPL/Source/"
                    OUTPUT_PATH "${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore"
                    BINARY_DIR "${SIMPLProj_BINARY_DIR}"
                    MODULE_TEMPLATE_FILE "${SIMPLPyBind11_SOURCE_DIR}/Templates/SIMPLModuleCodeTemplate.cpp"
                    MODULE_LINK_LIBRARIES SIMPLib
                    SIMPLIB
)



