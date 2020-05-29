
#-------------------------------------------------------------------------------
# @Brief function AddPythonTest
# @ NAME
# @ FILE
# @ PYTHONPATH
#-------------------------------------------------------------------------------
function(AddPythonTest)
  set(options )
  set(oneValueArgs NAME FILE)
  set(multiValueArgs PYTHONPATH)
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(SIMPL_USE_ANACONDA_PYTHON)
    if(WIN32)
      add_test(NAME ${ARGS_NAME}
        COMMAND ${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Binding/anaconda_test.bat
      )

      set_property(TEST ${ARGS_NAME}
        PROPERTY
          ENVIRONMENT
            "SIMPL_CONDA_EXECUTABLE=${SIMPL_CONDA_EXECUTABLE}"
            "SIMPL_CONDA_ENV=${ANACONDA_ENVIRONMENT_NAME}"
            "PYTHON_TEST_FILE=${ARGS_FILE}"
      )
    else()
      add_test(NAME ${ARGS_NAME}
        COMMAND ${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Binding/anaconda_test.sh
      )

      set_property(TEST ${ARGS_NAME}
        PROPERTY
          ENVIRONMENT
            "SIMPL_ANACONDA_DIR=${ANACONDA_DIR}"
            "SIMPL_CONDA_ENV=${ANACONDA_ENVIRONMENT_NAME}"
            "PYTHON_TEST_FILE=${ARGS_FILE}"
      )
    endif()
  else()
    add_test(NAME ${ARGS_NAME}
      COMMAND ${PYTHON_EXECUTABLE} ${ARGS_FILE}
    )
  endif()

  if(WIN32)
    string(REPLACE ";" "\\;" ARGS_PYTHONPATH "${ARGS_PYTHONPATH}")
  else()
    string(REPLACE ";" ":" ARGS_PYTHONPATH "${ARGS_PYTHONPATH}")
  endif()

  set_property(TEST ${ARGS_NAME}
    APPEND
    PROPERTY
      ENVIRONMENT
      "PYTHONPATH=${ARGS_PYTHONPATH}"
  )
endfunction()

#-------------------------------------------------------------------------------
# @Brief function CreatePybind11Module
# @ MODULE_NAME
# @ OUTPUT_DIR
# @ FILE_LIST_PATH
# @ SOURCE_DIR
# @ HEADER_PATH
# @ BODY_PATH
# @ INCLUDE_DIR
# @ PYTHON_OUTPUT_DIR
#-------------------------------------------------------------------------------
function(CreatePybind11Module)
  set(options PLUGIN)
  set(oneValueArgs MODULE_NAME OUTPUT_DIR FILE_LIST_PATH SOURCE_DIR HEADER_PATH BODY_PATH INCLUDE_DIR PYTHON_OUTPUT_DIR)
  set(multiValueArgs LINK_LIBRARIES INCLUDE_DIRS)
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  find_package(pybind11 REQUIRED)

  set(PLUGIN_ARG "")
  if(${ARGS_PLUGIN})
    set(PLUGIN_ARG "--plugin")
  endif()

  string(TOLOWER ${ARGS_MODULE_NAME} MODULE_NAME_lower)

set(d3dAllModules "${SIMPLProj_BINARY_DIR}/CreateDream3dAll.py")
if("${ARGS_MODULE_NAME}" STREQUAL "simpl")
  file(WRITE "${d3dAllModules}" "import argparse\n")
  file(APPEND "${d3dAllModules}" "import os\n")
  file(APPEND "${d3dAllModules}" "import re\n")
  file(APPEND "${d3dAllModules}" "from pathlib import Path\n")

  file(APPEND "${d3dAllModules}" "if __name__ == '__main__':\n")
  file(APPEND "${d3dAllModules}" "  parser = argparse.ArgumentParser()\n")
  file(APPEND "${d3dAllModules}" "  parser.add_argument('python_output_dir')\n")
  file(APPEND "${d3dAllModules}" "  args = parser.parse_args()\n")
  file(APPEND "${d3dAllModules}" "  with open(f'{args.python_output_dir}/dream3d.py', 'w') as d3d_file:\n")
  file(APPEND "${d3dAllModules}" "    d3d_file.write(f'import ${MODULE_NAME_lower}py\\n')\n")

  add_custom_target(CreateDream3dModuleFile ALL
      COMMAND ${PYTHON_EXECUTABLE} ${d3dAllModules} "${ARGS_PYTHON_OUTPUT_DIR}"
      COMMENT "Creating dream3d.py module file")

else()
  file(APPEND "${d3dAllModules}" "    d3d_file.write(f'import ${MODULE_NAME_lower}py\\n')\n")
endif()

  set(CREATE_PYTHON_BINDINGS_TARGET ${MODULE_NAME_lower}CreatePythonBindings)
  set(PYTHON_MODULE_SOURCE_FILE ${ARGS_OUTPUT_DIR}/py_${MODULE_NAME_lower}.cpp)
  set(PYTHON_UNIT_TEST_FILE ${ARGS_PYTHON_OUTPUT_DIR}/${MODULE_NAME_lower}_UnitTest.py)

  file(MAKE_DIRECTORY ${ARGS_OUTPUT_DIR})

  set(PY_GENERATOR ${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Binding/generate_python_bindings.py)

  add_custom_target(${CREATE_PYTHON_BINDINGS_TARGET} ALL
      COMMAND ${PYTHON_EXECUTABLE} ${PY_GENERATOR}
      "${ARGS_OUTPUT_DIR}"
      "${ARGS_FILE_LIST_PATH}"
      "${ARGS_SOURCE_DIR}"
      "${PLUGIN_ARG}"
      "${ARGS_PYTHON_OUTPUT_DIR}"
      "--include_dir=${ARGS_INCLUDE_DIR}"
      "--module_name=${MODULE_NAME_lower}"
      "--header_path=${ARGS_HEADER_PATH}"
      "--body_path=${ARGS_BODY_PATH}"
      "--plugin_name=${ARGS_MODULE_NAME}"
      COMMENT "${ARGS_MODULE_NAME}: Generating Python bindings"
      BYPRODUCTS ${PYTHON_MODULE_SOURCE_FILE}
  )
  set_property(TARGET ${CREATE_PYTHON_BINDINGS_TARGET} PROPERTY FOLDER "Python/Generator")

  add_custom_command(TARGET ${CREATE_PYTHON_BINDINGS_TARGET}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ARGS_PYTHON_OUTPUT_DIR}
  )

  pybind11_add_module(${MODULE_NAME_lower}
    ${PYTHON_MODULE_SOURCE_FILE}
  )

  set_property(TARGET ${MODULE_NAME_lower} PROPERTY FOLDER "Python/Bindings")

  set_source_files_properties(${PYTHON_MODULE_SOURCE_FILE}
    PROPERTIES  
      SKIP_AUTOMOC ON
  )

  add_dependencies(${MODULE_NAME_lower} ${CREATE_PYTHON_BINDINGS_TARGET})

  target_link_libraries(${MODULE_NAME_lower}
    PUBLIC
      ${ARGS_LINK_LIBRARIES}
  )

  target_include_directories(${MODULE_NAME_lower}
    PUBLIC
      ${ARGS_INCLUDE_DIRS}
  )

  set(TESTS_PYTHONPATH
    "$<TARGET_FILE_DIR:simpl>"
  )

  AddPythonTest(NAME PY_${MODULE_NAME_lower}_UnitTest
    FILE ${PYTHON_UNIT_TEST_FILE}
    PYTHONPATH ${TESTS_PYTHONPATH}
  )
endfunction()

#-------------------------------------------------------------------------------
# @Brief function CreatePybind11Plugin
# @ PLUGIN_NAME
# @ PLUGIN_TARGET
# @ HEADER_PATH
# @ BODY_PATH
#-------------------------------------------------------------------------------
function(CreatePybind11Plugin)
  set(options)
  set(oneValueArgs PLUGIN_NAME PLUGIN_TARGET HEADER_PATH BODY_PATH)
  set(multiValueArgs)
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

  CreatePybind11Module(MODULE_NAME ${ARGS_PLUGIN_NAME}
    OUTPUT_DIR "${${ARGS_PLUGIN_NAME}_BINARY_DIR}/Wrapping/PythonCore"
    FILE_LIST_PATH "${SIMPLProj_BINARY_DIR}/Wrapping/${ARGS_PLUGIN_NAME}Filters.txt"
    SOURCE_DIR "${${ARGS_PLUGIN_NAME}_SOURCE_DIR}/${ARGS_PLUGIN_NAME}Filters"
    INCLUDE_DIR "${${ARGS_PLUGIN_NAME}_SOURCE_DIR}"
    PYTHON_OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$<${_isMultiConfig}:$<CONFIG>>"
    HEADER_PATH ${ARGS_HEADER_PATH}
    BODY_PATH ${ARGS_BODY_PATH}
    LINK_LIBRARIES ${ARGS_PLUGIN_TARGET}
    INCLUDE_DIRS ${SIMPLProj_SOURCE_DIR}/Wrapping/Python
    PLUGIN
  )
endfunction()


#-------------------------------------------------------------------------------
# @Brief function CreatePythonTests
# @ PREFIX
# @ INPUT_DIR
# @ TEST_NAMES
#-------------------------------------------------------------------------------
function(CreatePythonTests)
  set(options)
  set(oneValueArgs PREFIX INPUT_DIR)
  set(multiValueArgs TEST_NAMES)
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(TESTS_PYTHONPATH
    "$<TARGET_FILE_DIR:simpl>"
  )

  foreach(test ${ARGS_TEST_NAMES})
    string(REPLACE "/" "_" test_name ${test})
    set(PY_TEST_NAME ${ARGS_PREFIX}_${test_name})

    AddPythonTest(NAME ${PY_TEST_NAME}
      FILE ${ARGS_INPUT_DIR}/${test}.py
      PYTHONPATH ${TESTS_PYTHONPATH}
    )
  endforeach()
endfunction()
