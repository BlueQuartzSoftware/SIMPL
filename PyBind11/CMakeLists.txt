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

# --------------------------------------------------------------------------
# Add an executable that will generate our python bindings
add_executable(GeneratePythonBindings GeneratePythonBindings.cpp)
target_link_libraries(GeneratePythonBindings Qt5::Core)


# --------------------------------------------------------------------------
# Find the Pybind11 installation
set(PYBIND11_PYTHON_VERSION "3.6")
find_package(PyBind11)

# --------------------------------------------------------------------------
# Now compile the Python module
pybind11_add_module(SIMPLPy SIMPLPy.cxx)
set_target_properties(SIMPLPy PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(SIMPLPy PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  )
target_link_libraries(SIMPLPy LINK_PUBLIC SIMPLib LINK_PRIVATE pybind11::module)
target_compile_features(SIMPLPy PRIVATE cxx_local_type_template_args)
set_target_properties(SIMPLPy PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")
                                         
configure_file("${SIMPLPyBind11_SOURCE_DIR}/Testing/Python/TestBindings.py"
                "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/TestBindings.py" COPYONLY)
                                         
install (TARGETS SIMPLPy DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)

