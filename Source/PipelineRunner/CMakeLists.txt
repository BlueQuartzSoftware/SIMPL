
# set project's name
PROJECT( SIMPLTools )
cmake_minimum_required(VERSION 3.8.0)

# --------------------------------------------------------------------
# Setup the install rules for the various platforms
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


#-------------------------------------------------------------------------------
# Function COMPILE_TOOL to help alleviate lots of extra code below for adding
# simple command line tools that just need one or two source files
#
function(COMPILE_TOOL)
    set(options)
    set(oneValueArgs TARGET DEBUG_EXTENSION BINARY_DIR COMPONENT INSTALL_DEST DEFINITION)
    set(multiValueArgs SOURCES LINK_LIBRARIES)
    cmake_parse_arguments(D3DTOOL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    BuildToolBundle(
        TARGET ${D3DTOOL_TARGET}
        SOURCES ${D3DTOOL_SOURCES}
        DEBUG_EXTENSION ${D3DTOOL_DEBUG_EXTENSION}
        VERSION_MAJOR ${D3DTOOL_SIMPL_VER_MAJOR}
        VERSION_MINOR ${D3DTOOL_SIMPL_VER_MINOR}
        VERSION_PATCH ${D3DTOOL_SIMPL_VER_PATCH}
        BINARY_DIR    ${D3DTOOL_BINARY_DIR}
        LINK_LIBRARIES Qt5::Core "${D3DTOOL_LINK_LIBRARIES}"
        LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMPONENT     ${D3DTOOL_COMPONENT}
        INSTALL_DEST  "${D3DTOOL_INSTALL_DEST}"
        SOLUTION_FOLDER "Applications"
    )

    if( ${D3DTOOL_DEFINITION} )
        target_compile_definitions(${D3DTOOL_TARGET} PRIVATE -D${D3DTOOL_DEFINITION})
    endif()
endfunction()


# Create a Command line tool for the PipelineRunner
if(SIMPL_Group_PLUGIN AND SIMPL_Group_BASE AND SIMPL_Group_FILTERS)
  COMPILE_TOOL(
      TARGET PipelineRunner
      SOURCES ${SIMPLTools_SOURCE_DIR}/PipelineRunner.cpp
      DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
      VERSION_MAJOR ${SIMPL_VER_MAJOR}
      VERSION_MINOR ${SIMPL_VER_MINOR}
      VERSION_PATCH ${SIMPL_VER_PATCH}
      BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
      COMPONENT     Tools
      INSTALL_DEST  "${install_dir}"
      LINK_LIBRARIES SIMPLib Qt5::Core
  )
endif()

