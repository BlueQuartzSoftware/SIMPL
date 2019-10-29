

function(CreatePybind11Module)
  set(options VERBOSE SIMPLIB)
  set(oneValueArgs MODULE_NAME SOURCE_DIR PATH_TO_STRIP OUTPUT_PATH BINARY_DIR MODULE_TEMPLATE_FILE MODULE_LINK_LIBRARIES)
  set(multiValueArgs )
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(ARGS_VERBOSE)
    message(STATUS "[${ARGS_MODULE_NAME}] ## Begin CreatePybind11Module ")
  endif()

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


  # --------------------------------------------------------------------------
  # Find the Pybind11 installation
  find_package(pybind11 CONFIG REQUIRED)

  if(NOT pybind11_FOUND)
    message(FATAL_ERROR "[${ARGS_MODULE_NAME}] pybind11 is REQUIRED to build the SIMPL Python bindings")
  endif()
  
  get_property(SIMPL_PYTHON_MODULE_SUFFIX GLOBAL PROPERTY SIMPL_PYTHON_MODULE_SUFFIX)

  # Give our module a name. Python standards dictate ALL lowercase for the module name
  set(pybind_module_name "${ARGS_MODULE_NAME}${SIMPL_PYTHON_MODULE_SUFFIX}")
  set(pybind_module_name_CamelCase "${ARGS_MODULE_NAME}")
  string(TOLOWER ${pybind_module_name} pybind_module_name)
  set(pybind_plugin_list_file "${SIMPLProj_BINARY_DIR}/Pybind11Plugins.txt")

  set(SIMPL_PY_MODULE_NAME "dream3d")

  # Now create a custom task to scrape through our code and generate all the binding sources
  add_custom_target(${pybind_module_name}CreatePythonBindings ALL
      COMMAND GeneratePythonBindings "${ARGS_SOURCE_DIR}"
      "${ARGS_PATH_TO_STRIP}"
      "${pybind_module_name_CamelCase}"
      "${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore"
      "${ARGS_MODULE_TEMPLATE_FILE}"
      "${ARGS_SIMPLIB}"
      "${CMAKE_CFG_INTDIR}"
      DEPENDS GeneratePythonBindings ${COPY_LIBRARY_TARGETS}
      COMMENT " ${ARGS_MODULE_NAME}: Creating Python Bindings using pybind11"
    )

  # --------------------------------------------------------------------------
  # Now compile the Python module using pybind11 cmake functions
  # Note the name of source file is also explicitly used in the CodeScraper program. If you change
  # the name below you need to update the CodeScraper program
  set(file_ext "h")
  if(${ARGS_SIMPLIB})
    set(file_ext "cxx")
  endif()
  set(pybind_module_file_name "${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore/${pybind_module_name}_pybind11_module.${file_ext}")
  set(submodules_headers_file_name "${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore/DREAM3D_SubmoduleHeaders.h")
  set(submodule_init_cxx_file_name "${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore/DREAM3D_SubmoduleInit.hpp")
  if(NOT EXISTS "${pybind_module_file_name}")
    FILE(WRITE "${pybind_module_file_name}" "/* Pybind11 Module code */\n") 
  endif()
  if(NOT TARGET ${SIMPL_PY_MODULE_NAME})
    file(WRITE ${submodules_headers_file_name} "/* This file includes all the submodule CXX files */\n")
    file(WRITE ${submodule_init_cxx_file_name} "/* This file includes the submodule init code. */\n")
    file(WRITE ${pybind_plugin_list_file} "SIMPL\n")

    pybind11_add_module(${SIMPL_PY_MODULE_NAME}
      ${submodules_headers_file_name}
      ${submodule_init_cxx_file_name}
      ${pybind_module_file_name}
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/DerivedSharedPointerClassInit.in.h"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/PluginModuleCodeTemplate.in.cpp"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/Pybind11CustomTypeCasts.in.h"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/Pybind11TopMatter.in.h"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/SIMPLModuleCodeTemplate.in.cpp"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/SharedPointerClassInit.in.h"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/SimpleClassInit.in.h"
      )
    set_source_files_properties("${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/DerivedSharedPointerClassInit.in.h"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/PluginModuleCodeTemplate.in.cpp"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/Pybind11CustomTypeCasts.in.h"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/Pybind11TopMatter.in.h"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/SIMPLModuleCodeTemplate.in.cpp"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/SharedPointerClassInit.in.h"
      "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Pybind11/Templates/SimpleClassInit.in.h"
      PROPERTIES  
        HEADER_FILE_ONLY ON
        SKIP_AUTOMOC ON
    )
    
    set_source_files_properties("${submodules_headers_file_name}"
        PROPERTIES  
            GENERATED TRUE
            HEADER_FILE_ONLY ON
            SKIP_AUTOMOC ON
    )
    
    target_link_libraries(${SIMPL_PY_MODULE_NAME} 
                            LINK_PRIVATE pybind11::module SIMPLib
    )

    set (SIMPL_PY_MODULE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR})
    if (SIMPL_ENABLE_ANACONDA_PACKAGE)
      #message(STATUS "${SIMPL_ANACONDA_OUTPUT_DIR}/${PYTHON_SITE_PACKAGES_NAME}/${SIMPL_PYTHON_MODULE_NAME}")
      #message(STATUS "SIMPL_ENABLE_ANACONDA_PACKAGE: ON <================>")
      set (SIMPL_PY_MODULE_LIBRARY_OUTPUT_DIRECTORY ${SIMPL_ANACONDA_OUTPUT_DIR}/${PYTHON_SITE_PACKAGES_NAME}/${SIMPL_PYTHON_MODULE_NAME})
    endif()

  
    set_target_properties(${SIMPL_PY_MODULE_NAME} PROPERTIES 
      PREFIX "${PYTHON_MODULE_PREFIX}"
      SUFFIX "${PYTHON_MODULE_EXTENSION}"
      OUTPUT_NAME ${SIMPL_PY_MODULE_NAME}
      LIBRARY_OUTPUT_DIRECTORY ${SIMPL_PY_MODULE_LIBRARY_OUTPUT_DIRECTORY}
      LIBRARY_OUTPUT_DIRECTORY_DEBUG ${SIMPL_PY_MODULE_LIBRARY_OUTPUT_DIRECTORY}
      LIBRARY_OUTPUT_DIRECTORY_RELEASE ${SIMPL_PY_MODULE_LIBRARY_OUTPUT_DIRECTORY}
      LIBRARY_OUTPUT_DIRECTORY_RELWDEBUG ${SIMPL_PY_MODULE_LIBRARY_OUTPUT_DIRECTORY}
      LIBRARY_OUTPUT_DIRECTORY_RELMINSIZE ${SIMPL_PY_MODULE_LIBRARY_OUTPUT_DIRECTORY}
      ARCHIVE_OUTPUT_NAME ${PYTHON_MODULE_PREFIX}${SIMPL_PY_MODULE_NAME}${PYTHON_MODULE_EXTENSION}
      PDB_NAME ${PYTHON_MODULE_PREFIX}${SIMPL_PY_MODULE_NAME}${PYTHON_MODULE_EXTENSION}
    )
    set_target_properties(${SIMPL_PY_MODULE_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    target_compile_features(${SIMPL_PY_MODULE_NAME} PRIVATE cxx_local_type_template_args)

    if(WIN32)
      # create a list of files to copy
      set (PLUGIN_DEBUG_EXTENSION $<$<CONFIG:Debug>:"_debug">)
      set (QT_DEBUG_EXTENSION $<$<CONFIG:Debug>:"d">)
      set (OTHER_DEBUG_EXTENSION $<$<CONFIG:Debug>:"_D">)
    endif()

    # Copy all necessary content from the CMake output directory to the "egg" directory
    # if (SIMPL_ENABLE_ANACONDA_PACKAGE)
    #   get_property(SIMPL_ANACONDA_OUTPUT_DIR GLOBAL PROPERTY SIMPL_ANACONDA_OUTPUT_DIR)
    #   add_custom_command(TARGET ${SIMPL_PY_MODULE_NAME} 
    #         POST_BUILD 
    #         COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR} ${SIMPL_ANACONDA_OUTPUT_DIR}/package/dream3d)
    # endif()
    
  else()

    file(APPEND ${submodules_headers_file_name} "\n /* Pybind11 Init Code for ${ARGS_MODULE_NAME} */\n")
    file(APPEND ${submodules_headers_file_name} "#include \"${pybind_module_file_name}\"\n")
    file(APPEND ${submodule_init_cxx_file_name} "  Init_${pybind_module_name}(m);\n")
    target_sources(${SIMPL_PY_MODULE_NAME} PRIVATE ${pybind_module_file_name})
    set_source_files_properties("${pybind_module_file_name}"
                                PROPERTIES  
                                  GENERATED TRUE
                                  HEADER_FILE_ONLY TRUE
                                  SKIP_AUTOMOC TRUE
    )
    file(APPEND ${pybind_plugin_list_file} "${ARGS_MODULE_NAME}\n")

  endif() 
  #---------------------------------------------------------------------
  
  target_include_directories(${SIMPL_PY_MODULE_NAME} PUBLIC
    #$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SIMPLib_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SIMPLProj_BINARY_DIR}/Wrapping/PythonCore>
    $<BUILD_INTERFACE:${${pybind_module_name}_BINARY_DIR}>
    )
  target_link_libraries(${SIMPL_PY_MODULE_NAME} 
                          LINK_PUBLIC ${ARGS_MODULE_LINK_LIBRARIES}
  )

  if(SIMPL_USE_ITK)
    ## YOU MUST INCLUDE THIS FILE TO USE ITK !!!
    ## Then do not forget to add "${ITK_LIBRARIES}" to the target_link_libraries below
    include( ${CMP_SOURCE_DIR}/ITKSupport/IncludeITK.cmake)
    target_include_directories(${SIMPL_PY_MODULE_NAME} PUBLIC $<BUILD_INTERFACE:${ITK_INCLUDE_DIRS}>)
    target_link_libraries(${SIMPL_PY_MODULE_NAME} LINK_PRIVATE ${ITK_LIBRARIES})
  endif()

  if(TARGET ${SIMPL_PY_MODULE_NAME} AND TARGET ${pybind_module_name}CreatePythonBindings)
    add_dependencies(${SIMPL_PY_MODULE_NAME} ${pybind_module_name}CreatePythonBindings)
  endif()

  
  if(0)
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


    INSTALL(TARGETS ${SIMPL_PY_MODULE_NAME}
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
        LIBRARY DESTINATION ${lib_install_dir}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
        ARCHIVE DESTINATION lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
        BUNDLE DESTINATION "."
    )
  endif()

  if(ARGS_VERBOSE)
    message(STATUS "[${ARGS_MODULE_NAME}] ## End CreatePybind11Module ")
  endif()

endfunction()


