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



AddSIMPLUnitTest(TESTNAME AttributeArrayIteratorExample
                  SOURCES
                    ${SIMPLExperimental_SOURCE_DIR}/AttributeArrayIteratorExample/AttributeArrayIterators.cpp
                    ${SIMPLExperimental_SOURCE_DIR}/AttributeArrayIteratorExample/AttributeArray.hpp

                  FOLDER
                    "DREAM3D UnitTests"
                  LINK_LIBRARIES
                    Qt5::Core H5Support SIMPLib
                  INCLUDE_DIRS
                    ${SIMPLTools_BINARY_DIR}
                    ${SIMPLProj_SOURCE_DIR}/Source
                    ${SIMPLProj_BINARY_DIR}
)
