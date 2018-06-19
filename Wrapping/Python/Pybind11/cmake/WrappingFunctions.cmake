

function(CreatePybind11Module)
  set(options VERBOSE SIMPLIB)
  set(oneValueArgs MODULE_NAME SOURCE_DIR PATH_TO_STRIP OUTPUT_PATH BINARY_DIR MODULE_TEMPLATE_FILE MODULE_LINK_LIBRARIES)
  set(multiValueArgs )
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(ARGS_VERBOSE)
    message(STATUS "[${ARGS_MODULE_NAME}] ## Begin CreatePybind11Module ")
  endif()
  # --------------------------------------------------------------------------
  # Find the Pybind11 installation
  set(PYBIND11_PYTHON_VERSION "3")
  find_package(pybind11 CONFIG REQUIRED)
  if(ARGS_VERBOSE)
    message(STATUS "[${ARGS_MODULE_NAME}] PYTHON_EXECUTABLE: ${PYTHON_EXECUTABLE}")
    message(STATUS "[${ARGS_MODULE_NAME}] Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIRS}")
    message(STATUS "[${ARGS_MODULE_NAME}] PYTHONLIBS_FOUND: ${PYTHONLIBS_FOUND}")
    message(STATUS "[${ARGS_MODULE_NAME}] PYTHON_PREFIX: ${PYTHON_PREFIX}")
    message(STATUS "[${ARGS_MODULE_NAME}] PYTHON_LIBRARIES: ${PYTHON_LIBRARIES}")
    message(STATUS "[${ARGS_MODULE_NAME}] PYTHON_INCLUDE_DIRS: ${PYTHON_INCLUDE_DIRS}")
    message(STATUS "[${ARGS_MODULE_NAME}] PYTHON_MODULE_EXTENSION: ${PYTHON_MODULE_EXTENSION}")
    message(STATUS "[${ARGS_MODULE_NAME}] PYTHON_MODULE_PREFIX: ${PYTHON_MODULE_PREFIX}")
    message(STATUS "[${ARGS_MODULE_NAME}] PYTHON_SITE_PACKAGES: ${PYTHON_SITE_PACKAGES}")
    message(STATUS "[${ARGS_MODULE_NAME}] PYTHON_IS_DEBUG: ${PYTHON_IS_DEBUG}")
  endif()
  if(NOT pybind11_FOUND)
    message(FATAL_ERROR "[${ARGS_MODULE_NAME}] pybind11 is REQUIRED to build the SIMPL Python bindings")
  endif()
  
  get_property(SIMPL_PYTHON_MODULE_SUFFIX GLOBAL PROPERTY SIMPL_PYTHON_MODULE_SUFFIX)

  # Give our module a name. Python standards dictate ALL lowercase for the module name
  set(pybind_module_name "${ARGS_MODULE_NAME}${SIMPL_PYTHON_MODULE_SUFFIX}")
  string(TOLOWER ${pybind_module_name} pybind_module_name)

  set(pybind_module_output_name "${ARGS_MODULE_NAME}${SIMPL_PYTHON_MODULE_SUFFIX}")
  string(TOLOWER ${pybind_module_output_name} pybind_module_output_name)


  # Now create a custom task to scrape through our code and generate all the binding sources
  add_custom_target(${pybind_module_name}CreatePythonBindings ALL
      COMMAND GeneratePythonBindings "${ARGS_SOURCE_DIR}"
      "${ARGS_PATH_TO_STRIP}"
      "${pybind_module_name}"
      "${ARGS_OUTPUT_PATH}" 
      "${ARGS_MODULE_TEMPLATE_FILE}"
      "${ARGS_SIMPLIB}"
      DEPENDS GeneratePythonBindings ${COPY_LIBRARY_TARGETS}
      COMMENT "GeneratePythonBindings: Creating Python Bindings for ${ARGS_MODULE_NAME} using pybind11"
    )

  # --------------------------------------------------------------------------
  # Now compile the Python module using pybind11 cmake functions
  # Note the name of source file is also explicitly used in the CodeScraper program. If you change
  # the name below you need to update the CodeScraper program

  set(pybind_module_file_name "${ARGS_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.cxx")
  pybind11_add_module(${pybind_module_name}
      ${pybind_module_file_name} 
      ${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/SIMPLModuleCodeTemplate.cpp
      )

  if(NOT EXISTS "${pybind_module_file_name}")
    FILE(WRITE "${pybind_module_file_name}" "/* Pybind11 Module code */\n") 
  endif()

  set_source_files_properties("${pybind_module_file_name}"
                              PROPERTIES  
                                  GENERATED TRUE
                                  SKIP_AUTOMOC ON
  )
  set_source_files_properties("${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/SIMPLModuleCodeTemplate.cpp"
                              PROPERTIES  
                                HEADER_FILE_ONLY ON
                                SKIP_AUTOMOC ON
  )
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
                                            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python/site-packages/dream3d
                                            )
  if(TARGET ${pybind_module_name} AND TARGET ${pybind_module_name}CreatePythonBindings)                                            
    add_dependencies(${pybind_module_name} ${pybind_module_name}CreatePythonBindings)
  endif()

                                            
  # install (TARGETS ${pybind_module_name} 
  #           DESTINATION ./lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
  # )
  
  set(install_dir "bin")
  set(lib_install_dir "lib")

  if(APPLE)
    get_property(DREAM3D_PACKAGE_DEST_PREFIX GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX)
    set(install_dir "${DREAM3D_PACKAGE_DEST_PREFIX}bin")
    set(lib_install_dir "${DREAM3D_PACKAGE_DEST_PREFIX}lib")
  elseif(WIN32)
    set(install_dir ".")
    set(lib_install_dir ".")
  endif()


  INSTALL(TARGETS ${pybind_module_name}
      COMPONENT Applications
      RUNTIME DESTINATION ${install_dir}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
      LIBRARY DESTINATION ${lib_install_dir}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
      ARCHIVE DESTINATION lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
      BUNDLE DESTINATION "."
  )

  if(ARGS_VERBOSE)
    message(STATUS "[${ARGS_MODULE_NAME}] ## End CreatePybind11Module ")
  endif()

endfunction()


