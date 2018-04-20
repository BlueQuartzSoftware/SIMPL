

function(CreatePybind11Module)
  set(options VERBOSE)
  set(oneValueArgs MODULE_NAME SOURCE_DIR PATH_TO_STRIP OUTPUT_PATH BINARY_DIR MODULE_TEMPLATE_FILE MODULE_LINK_LIBRARIES)
  set(multiValueArgs )
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(ARGS_VERBOSE)
    message(STATUS "## START CreatePybind11Module")
  endif()
  # --------------------------------------------------------------------------
  # Find the Pybind11 installation
  set(PYBIND11_PYTHON_VERSION "3")
  find_package(pybind11 CONFIG REQUIRED)
  if(ARGS_VERBOSE)
    message(STATUS "PYTHON_EXECUTABLE: ${PYTHON_EXECUTABLE}")
    message(STATUS "Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIRS}")
    message(STATUS "PYTHONLIBS_FOUND: ${PYTHONLIBS_FOUND}")
    message(STATUS "PYTHON_PREFIX: ${PYTHON_PREFIX}")
    message(STATUS "PYTHON_LIBRARIES: ${PYTHON_LIBRARIES}")
    message(STATUS "PYTHON_INCLUDE_DIRS: ${PYTHON_INCLUDE_DIRS}")
    message(STATUS "PYTHON_MODULE_EXTENSION: ${PYTHON_MODULE_EXTENSION}")
    message(STATUS "PYTHON_MODULE_PREFIX: ${PYTHON_MODULE_PREFIX}")
    message(STATUS "PYTHON_SITE_PACKAGES: ${PYTHON_SITE_PACKAGES}")
    message(STATUS "PYTHON_IS_DEBUG: ${PYTHON_IS_DEBUG}")
  endif()
  if(NOT pybind11_FOUND)
    message(FATAL_ERROR "pybind11 is REQUIRED to build the SIMPL Python bindings")
  endif()

  # Give our module a name. Python standards dictate ALL lowercase for the module name
  set(pybind_module_name "${ARGS_MODULE_NAME}py11")
  string(TOLOWER ${pybind_module_name} pybind_module_name)

  set(pybind_module_output_name "${ARGS_MODULE_NAME}")
  string(TOLOWER ${pybind_module_output_name} pybind_module_output_name)


  # Now create a custom task to scrape through our code and generate all the binding sources
  add_custom_target(${pybind_module_name}CreatePythonBindings ALL
      COMMAND GeneratePythonBindings "${ARGS_SOURCE_DIR}"
      "${ARGS_PATH_TO_STRIP}"
      "${pybind_module_name}"
      "${ARGS_OUTPUT_PATH}" "${ARGS_MODULE_TEMPLATE_FILE}"
      DEPENDS GeneratePythonBindings ${COPY_LIBRARY_TARGETS}
      COMMENT "GeneratePythonBindings: Creating Python Bindings for ${ARGS_MODULE_NAME} using pybind11"
    )

  # --------------------------------------------------------------------------
  # Now compile the Python module using pybind11 cmake functions
  # Note the name of source file is also explicitly used in the CodeScraper program. If you change
  # the name below you need to update the CodeScraper program
  pybind11_add_module(${pybind_module_name}
      "${ARGS_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.cxx"
      )

  if(NOT EXISTS "${ARGS_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.cxx")
    FILE(WRITE "${ARGS_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.cxx" "/* Pybind11 Module code */\n") 
  endif()

  set_source_files_properties("${ARGS_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.cxx"
                                PROPERTIES  
                                    #GENERATED TRUE
                                    SKIP_AUTOMOC ON)
  set_target_properties(${pybind_module_name} PROPERTIES LINKER_LANGUAGE CXX)
  target_include_directories(${pybind_module_name} PUBLIC
    #$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SIMPLib_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore>
    $<BUILD_INTERFACE:${${pybind_module_name}_BINARY_DIR}>
    )
  target_link_libraries(${pybind_module_name} 
                          LINK_PUBLIC ${ARGS_MODULE_LINK_LIBRARIES}
                          LINK_PRIVATE pybind11::module
                          )
  target_compile_features(${pybind_module_name} PRIVATE cxx_local_type_template_args)
  set_target_properties(${pybind_module_name} PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                            SUFFIX "${PYTHON_MODULE_EXTENSION}"
                                            OUTPUT_NAME ${pybind_module_output_name}
                                            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python/site-packages
                                            )
  if(TARGET ${pybind_module_name}CreatePythonBindings)                                            
    add_dependencies(${pybind_module_name} ${pybind_module_name}CreatePythonBindings)
  endif()

                                            
  install (TARGETS ${pybind_module_name} 
            DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
          )
  
  if(ARGS_VERBOSE)
    message(STATUS "## END CreatePybind11Module")
  endif()

endfunction()


