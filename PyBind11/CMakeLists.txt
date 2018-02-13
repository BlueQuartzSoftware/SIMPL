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
configure_file( ${SIMPLPyBind11_SOURCE_DIR}/SIMPLPyBind11Config.h.in
                ${SIMPLPyBind11_BINARY_DIR}/SIMPLPyBind11Config.h)

find_package(Boost)
# --------------------------------------------------------------------------
# Add an executable that will generate our python bindings
add_executable(GeneratePythonBindings GeneratePythonBindings.cpp)
target_include_directories(GeneratePythonBindings 
                          PRIVATE
                            ${SIMPLProj_SOURCE_DIR}/Source
                            ${Boost_INCLUDE_DIRS})

target_link_libraries(GeneratePythonBindings Qt5::Core)


add_custom_target(SIMPLCreatePythonBindings ALL
      COMMAND GeneratePythonBindings "${SIMPLProj_SOURCE_DIR_NORM}/Source/SIMPLib" "SIMPL/Source/" "SIMPLib"
      DEPENDS ${COPY_LIBRARY_TARGETS} GeneratePythonBindings
      COMMENT "Creating Python Bindings for SIMPL"
    )


# --------------------------------------------------------------------------
# Find the Pybind11 installation
set(PYBIND11_PYTHON_VERSION "3.6")
find_package(PyBind11)

if(1)
  # --------------------------------------------------------------------------
  # Now compile the Python module
  pybind11_add_module(SIMPLibPy ${SIMPLProj_SOURCE_DIR}/Source/SIMPLib/pybind11/SIMPLib_Pybindings.cxx)
  set_source_files_properties(${SIMPLProj_SOURCE_DIR}/Source/SIMPLib/pybind11/SIMPLib_Pybindings.cxx
                              PROPERTIES GENERATED TRUE
                                         SKIP_AUTOMOC ON)
  set_target_properties(SIMPLibPy PROPERTIES LINKER_LANGUAGE CXX)
  target_include_directories(SIMPLibPy PUBLIC
    #$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SIMPLib_SOURCE_DIR}
    )
  target_link_libraries(SIMPLibPy LINK_PUBLIC SIMPLib LINK_PRIVATE pybind11::module)
  target_compile_features(SIMPLibPy PRIVATE cxx_local_type_template_args)
  set_target_properties(SIMPLibPy PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                           SUFFIX "${PYTHON_MODULE_EXTENSION}"
                                           LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python
                                           )
  add_dependencies(SIMPLibPy SIMPLCreatePythonBindings)                                         
  
                                           
  install (TARGETS SIMPLibPy 
            DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)

endif()
