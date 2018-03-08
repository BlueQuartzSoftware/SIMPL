#------------------------------------------------------------------------------
# Create the Top level Project
#
project(SIMPLPyBind11 VERSION 0.0.1.0)


# --------------------------------------------------------------------------
# Find the Pybind11 installation
set(PYBIND11_PYTHON_VERSION "3.6")
find_package(pybind11)
if(NOT pybind11_FOUND)
  message(FATAL_ERROR "pybind11 is REQUIRED to build the SIMPL Python bindings")
endif()
message(STATUS "pybind11_VERSION: ${pybind11_VERSION}")
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

# --------------------------------------------------------------------------
# Configure the header file with build specific paths
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
set(SIMPLib_PyInit_File "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python/site-packages/simpl_dirs/__init__.py")
configure_file( ${SIMPLPyBind11_SOURCE_DIR}/Testing/Python/simpl_dirs.in.py
                "${SIMPLib_PyInit_File}"
              )
# --------------------------------------------------------------------------
# Configure a python setup.py file for simpl_dirs  
set(SIMPLib_PySetup_File "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python/site-packages/setup.py")
configure_file( ${SIMPLPyBind11_SOURCE_DIR}/Testing/Python/setup.py
                "${SIMPLib_PySetup_File}"
              )

# --------------------------------------------------------------------------
# This section starts the main pybind11 code

# Give our module a name. Python standards dictate ALL lowercase for the module name
set(pybind_module_name "simpl")

if(1)
# Now create a custom task to scrape through our code and generate all the binding sources
add_custom_target(SIMPLCreatePythonBindings ALL
    COMMAND GeneratePythonBindings "${SIMPLProj_SOURCE_DIR_NORM}/Source/SIMPLib" "SIMPL/Source/" "${pybind_module_name}" "${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore"
    DEPENDS GeneratePythonBindings ${COPY_LIBRARY_TARGETS}
    COMMENT "GeneratePythonBindings: Creating Python Bindings for SIMPL using pybind11"
  )
endif()

# --------------------------------------------------------------------------
# Now compile the Python module using pybind11 cmake functions
# Note the name of source file is also explicitly used in the CodeScraper program. If you change
# the name below you need to update the CodeScraper program
pybind11_add_module(${pybind_module_name}
    "${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.cxx"
    )
if(NOT EXISTS ${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.cxx)
  FILE(WRITE "${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.cxx" "/* SOMETHING */\n")
endif()

set_source_files_properties(${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.cxx
                              PROPERTIES  
                                  #GENERATED TRUE
                                  SKIP_AUTOMOC ON)
set_target_properties(${pybind_module_name} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${pybind_module_name} PUBLIC
  #$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${SIMPLib_SOURCE_DIR}
  $<BUILD_INTERFACE:${${pybind_module_name}_BINARY_DIR}
  )
target_link_libraries(${pybind_module_name} 
                        LINK_PUBLIC SIMPLib 
                        LINK_PRIVATE pybind11::module)
target_compile_features(${pybind_module_name} PRIVATE cxx_local_type_template_args)
set_target_properties(${pybind_module_name} PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                          SUFFIX "${PYTHON_MODULE_EXTENSION}"
                                          LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python/site-packages
                                          )
add_dependencies(${pybind_module_name} SIMPLCreatePythonBindings)                                         

                                          
install (TARGETS ${pybind_module_name} 
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)


