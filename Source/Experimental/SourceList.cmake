# --------------------------------------------------------------------------
# set project's source dir
set(SIMPLExperimental_SOURCE_DIR "${SIMPLProj_SOURCE_DIR}/Source/Experimental")


# --------------------------------------------------------------------------
macro(ConvertPathToHeaderCompatible INPUT)
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${INPUT} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${INPUT}  )
    endif()
endmacro()


set(SIMPLProj_SOURCE_DIR_NORM ${SIMPLProj_SOURCE_DIR})
ConvertPathToHeaderCompatible(${SIMPLProj_SOURCE_DIR_NORM})
set(SIMPLib_SOURCE_DIR_NORM ${SIMPLib_SOURCE_DIR})
ConvertPathToHeaderCompatible(${SIMPLib_SOURCE_DIR_NORM})
set(SIMPLib_PLUGIN_DIR_NORM "${SIMPLProj_SOURCE_DIR}/Source/Plugins")
ConvertPathToHeaderCompatible(${SIMPLib_PLUGIN_DIR_NORM})

get_filename_component(SIMPLProj_PARENT_DIR_NORM ${SIMPLProj_SOURCE_DIR} PATH)
ConvertPathToHeaderCompatible(${SIMPLProj_PARENT_DIR_NORM})


configure_file( ${SIMPLExperimental_SOURCE_DIR}/ToolConfiguration.h.in
                ${SIMPLTools_BINARY_DIR}/ToolConfiguration.h
)

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



AddSIMPLUnitTest(TESTNAME FilterParameterCallbackExample
                  SOURCES 
                    ${SIMPLExperimental_SOURCE_DIR}/FilterCallback/FilterParameterCallbackExample.cpp
                    ${SIMPLExperimental_SOURCE_DIR}/FilterCallback/FilterParameter.h
                    ${SIMPLExperimental_SOURCE_DIR}/FilterCallback/FilterParameter.cpp
                    ${SIMPLExperimental_SOURCE_DIR}/FilterCallback/FilterWidget.h
                    ${SIMPLExperimental_SOURCE_DIR}/FilterCallback/FilterWidget.cpp
                    ${SIMPLExperimental_SOURCE_DIR}/FilterCallback/IFilter.h
                    ${SIMPLExperimental_SOURCE_DIR}/FilterCallback/IFilter.cpp
                  FOLDER 
                    "DREAM3D UnitTests"
                  LINK_LIBRARIES 
                    Qt5::Core H5Support SIMPLib  
                  INCLUDE_DIRS
                    ${SIMPLTools_BINARY_DIR}
                    ${SIMPLProj_SOURCE_DIR}/Source
                    ${SIMPLProj_BINARY_DIR}
)

COMPILE_TOOL(
    TARGET FilterInfoUtility
    SOURCES ${SIMPLExperimental_SOURCE_DIR}/FilterInfoUtility.cpp
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${SIMPL_VER_MAJOR}
    VERSION_MINOR ${SIMPL_VER_MINOR}
    VERSION_PATCH ${SIMPL_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Tools
    INSTALL_DEST  "${install_dir}"
    LINK_LIBRARIES SIMPLib
)
