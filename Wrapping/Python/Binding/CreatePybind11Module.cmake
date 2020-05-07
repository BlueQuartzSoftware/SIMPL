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

  set(CREATE_PYTHON_BINDINGS_TARGET ${ARGS_MODULE_NAME}CreatePythonBindings)
  set(PYTHON_MODULE_SOURCE_FILE ${ARGS_OUTPUT_DIR}/py_${ARGS_MODULE_NAME}.cpp)
  set(PYTHON_UNIT_TEST_FILE ${ARGS_PYTHON_OUTPUT_DIR}/${ARGS_MODULE_NAME}_UnitTest.py)

  file(MAKE_DIRECTORY ${ARGS_OUTPUT_DIR})
  file(MAKE_DIRECTORY ${ARGS_PYTHON_OUTPUT_DIR})

  set(PY_GENERATOR ${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Binding/generate_python_bindings.py)

  add_custom_target(${CREATE_PYTHON_BINDINGS_TARGET} ALL
      COMMAND ${PYTHON_EXECUTABLE} ${PY_GENERATOR}
      "${ARGS_OUTPUT_DIR}"
      "${ARGS_FILE_LIST_PATH}"
      "${ARGS_SOURCE_DIR}"
      "${PLUGIN_ARG}"
      "${ARGS_PYTHON_OUTPUT_DIR}"
      "--include_dir=${ARGS_INCLUDE_DIR}"
      "--module_name=${ARGS_MODULE_NAME}"
      "--header_path=${ARGS_HEADER_PATH}"
      "--body_path=${ARGS_BODY_PATH}"
      COMMENT "${ARGS_MODULE_NAME}: Generating Python bindings"
      BYPRODUCTS ${PYTHON_MODULE_SOURCE_FILE}
  )

  set_property(TARGET ${CREATE_PYTHON_BINDINGS_TARGET} PROPERTY FOLDER "Python/Generator")

  pybind11_add_module(${ARGS_MODULE_NAME}
    ${PYTHON_MODULE_SOURCE_FILE}
  )

  set_property(TARGET ${ARGS_MODULE_NAME} PROPERTY FOLDER "Python/Bindings")

  set_source_files_properties(${PYTHON_MODULE_SOURCE_FILE}
    PROPERTIES  
      SKIP_AUTOMOC ON
  )

  add_dependencies(${ARGS_MODULE_NAME} ${CREATE_PYTHON_BINDINGS_TARGET})

  target_link_libraries(${ARGS_MODULE_NAME}
    PUBLIC
      ${ARGS_LINK_LIBRARIES}
  )

  target_include_directories(${ARGS_MODULE_NAME}
    PUBLIC
      ${ARGS_INCLUDE_DIRS}
  )

  set(TESTS_PYTHONPATH
    "$<TARGET_FILE_DIR:simpl>"
  )

  AddPythonTest(NAME PY_${ARGS_MODULE_NAME}_UnitTest
    FILE ${PYTHON_UNIT_TEST_FILE}
    PYTHONPATH ${TESTS_PYTHONPATH}
  )
endfunction()

function(CreatePybind11Plugin)
  set(options)
  set(oneValueArgs PLUGIN_NAME PLUGIN_TARGET HEADER_PATH BODY_PATH)
  set(multiValueArgs)
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  string(TOLOWER ${ARGS_PLUGIN_NAME} PLUGIN_NAME_lower)

  CreatePybind11Module(MODULE_NAME ${PLUGIN_NAME_lower}
    OUTPUT_DIR "${${ARGS_PLUGIN_NAME}_BINARY_DIR}/Wrapping/PythonCore"
    FILE_LIST_PATH "${SIMPLProj_BINARY_DIR}/${ARGS_PLUGIN_NAME}PublicFilters.txt"
    SOURCE_DIR "${${ARGS_PLUGIN_NAME}_SOURCE_DIR}/${ARGS_PLUGIN_NAME}Filters"
    INCLUDE_DIR "${${ARGS_PLUGIN_NAME}_SOURCE_DIR}"
    PYTHON_OUTPUT_DIR "${SIMPLProj_BINARY_DIR}/Wrapping/dream3d"
    HEADER_PATH ${ARGS_HEADER_PATH}
    BODY_PATH ${ARGS_BODY_PATH}
    LINK_LIBRARIES ${ARGS_PLUGIN_TARGET}
    INCLUDE_DIRS ${SIMPLProj_SOURCE_DIR}/Wrapping/Python
    PLUGIN
  )
endfunction()

function(CreatePythonTests)
  set(options)
  set(oneValueArgs PREFIX INPUT_DIR)
  set(multiValueArgs TEST_NAMES)
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(TESTS_PYTHONPATH
    "$<TARGET_FILE_DIR:simpl>"
    "${SIMPLProj_SOURCE_DIR}/Wrapping/Python/SIMPL"
    "${SIMPLProj_BINARY_DIR}/Wrapping/dream3d"
    "${SIMPLProj_BINARY_DIR}/Wrapping/dream3d/$<CONFIG>"
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
