project(SVWidgetsLib VERSION 2.0)


set(SVWidgetsLib_BUILT_AS_DYNAMIC_LIB 0)
if(BUILD_SHARED_LIBS)
  set(SVWidgetsLib_BUILT_AS_DYNAMIC_LIB 1)
endif(BUILD_SHARED_LIBS)

set(GENERATE_GIT_VERSION 1)

if(NOT PROJECT_RESOURCES_DIR)
  set(PROJECT_RESOURCES_DIR ${SVWidgetsLib_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
  set(CMP_SOURCE_DIR ${SVWidgetsLib_SOURCE_DIR}/Support/cmp)
endif()

set(PROJECT_PREFIX "SVWidgetsLib" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(SVWidgetsLib_INSTALL_FILES 0)
# -----------------------------------------------------------------------
# The project generally does NOT want to install headers. See if another
# project has over ridden that property
get_property(CMP_PROJECT_INSTALL_HEADERS GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS)
if("${CMP_PROJECT_INSTALL_HEADERS}" STREQUAL "")
  set_property(GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS "0")
else()
  set(SVWidgetsLib_INSTALL_FILES "${CMP_PROJECT_INSTALL_HEADERS}")
endif()
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
set(CMP_HEADER_DIR ${SVWidgetsLib_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "SVWidgetsLibConfiguration.h")
set(CMP_TYPES_FILE_NAME "SVWidgetsLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "SVWidgetsLibVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "SVWidgetsLibVersion.cpp")
set(CMP_TOP_HEADER_FILE "SVWidgetsLib.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_GENERATE_VERSION_STRING ${GENERATE_GIT_VERSION})

# Extra Variables that need to be set before all the configured files are generated.
set(SVWidgetsLib_HDF5_SUPPORT 1)
option(SVWidgetsLib_ENABLE_HDF5 "Enable the use of HDF5 functionality" ON)
mark_as_advanced(SVWidgetsLib_ENABLE_HDF5)

include(${CMP_SOURCE_DIR}/cmpProject.cmake)

option(SIMPL_USE_QtWebEngine "Use QWebEngine to display Help. OFF=Use default web browser" ON)
set_property(GLOBAL PROPERTY SIMPL_USE_QtWebEngine "${SIMPL_USE_QtWebEngine}")

# --------------------------------------------------------------------
# Find and Use the Qt5 Libraries
include(${CMP_SOURCE_DIR}/ExtLib/Qt5Support.cmake)
set(SVWidgetsLib_Qt5_Components Core Widgets Network Gui Concurrent Svg Xml OpenGL PrintSupport )
CMP_AddQt5Support( "${SVWidgetsLib_Qt5_Components}"
                    "${SIMPL_USE_QtWebEngine}"
                    "${SIMPLProj_BINARY_DIR}"
                    "SVWidgetsLib")

option(SIMPL_USE_MKDOCS "Use mkdocs (python) to generate the Documentation" ON)
set_property(GLOBAL PROPERTY SIMPL_USE_MKDOCS "${SIMPL_USE_MKDOCS}")

option(SIMPL_USE_DISCOUNT "Use Discount to generate the Documentation" OFF)
set_property(GLOBAL PROPERTY SIMPL_USE_DISCOUNT "${SIMPL_USE_DISCOUNT}")


configure_file(${SVWidgetsLib_SOURCE_DIR}/Core/SVWidgetsLibConfiguration.h.in
               ${SVWidgetsLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE})

set(Project_SRCS "")

set(${PROJECT_NAME}_Generated_HDRS
        ${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE}
)

set(${PROJECT_NAME}_Generated_SRCS
        ${CMP_HEADER_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
)

cmp_IDE_GENERATED_PROPERTIES("Generated" "${${PROJECT_NAME}_Generated_HDRS}" "${${PROJECT_NAME}_Generated_SRCS}")


# -- Add the resources into the resource system
QT5_ADD_RESOURCES( SVWidgetsLib_Generated_RC_SRCS "${SIMPLProj_SOURCE_DIR}/Resources/SIMPL.qrc")

foreach(h ${SVWidgetsLib_Generated_RC_SRCS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

include(${SVWidgetsLib_SOURCE_DIR}/Animations/SourceList.cmake)
include(${SVWidgetsLib_SOURCE_DIR}/Core/SourceList.cmake)
include(${SVWidgetsLib_SOURCE_DIR}/Dialogs/SourceList.cmake)
include(${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/SourceList.cmake)
include(${SVWidgetsLib_SOURCE_DIR}/QtSupport/SourceList.cmake)
include(${SVWidgetsLib_SOURCE_DIR}/Widgets/SourceList.cmake)
include(${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/SourceList.cmake)

#------------------------------------------------------------------------------
# Clear these variables and look for any extra Widget Directories that the
# developer would like to add
set(SVWidgetsLib_Extra_Sources)
set(SVWidgetsLib_Extra_Headers)
set(SVWidgetsLib_Extra_Link_Libraries)

foreach(SVWidgetsExtraPath ${SVWidgetsExtraPaths})
  include( ${SVWidgetsExtraPath}/SourceList.cmake )
endforeach()

set(Project_SRCS

  ${${PROJECT_NAME}_Generated_HDRS}
  ${${PROJECT_NAME}_Generated_SRCS}
  ${SVWidgetsLib_Generated_RC_SRCS}

  #---------------
  ${SVWidgets_Animations_Files}
  ${SVWidgets_Core_Files}
  ${SVWidgets_Dialogs_Files}
  ${SVWidgetsLib_ParameterFilterWidgets_Files}
  ${SVWidgets_QtSupport_Files}
  ${SVWidgets_Widgets_Files}
  #---------------
  ${SVWidgetsLib_Extra_Sources}
  ${SVWidgetsLib_Extra_Headers}

  ${SVWidgetsLib_Widgets_HDRS}
  ${SVWidgetsLib_Widgets_HDRS_MOC}
  ${SVWidgetsLib_Widgets_SRCS}
  ${SVWidgetsLib_Widgets_UIS}
  ${SVWidgetsLib_Widgets_Generated_MOC_SRCS}
  ${SVWidgetsLib_Widgets_Generated_UI_HDRS}
)

if(UNIX AND NOT APPLE)
    set_source_files_properties(${Project_SRCS} PROPERTIES COMPILE_FLAGS -fPIC)
endif()

# Add in all the dependent link libraries
set(SVWidgetsLib_LINK_LIBRARIES
  SIMPLib
  ${SVWidgetsLib_Extra_Link_Libraries}
)

foreach(qt5module ${SVWidgetsLib_Qt5_Components})
  set(SVWidgetsLib_LINK_LIBRARIES ${SVWidgetsLib_LINK_LIBRARIES} Qt5::${qt5module})
endforeach()

add_library(SVWidgetsLib ${LIB_TYPE} ${Project_SRCS})
CMP_AddDefinitions(TARGET SVWidgetsLib)
target_include_directories(SVWidgetsLib
                            PUBLIC
                              ${SIMPLProj_SOURCE_DIR}/Source
                              ${SIMPLProj_BINARY_DIR}
                              ${SVWidgetsLib_SOURCE_DIR}/Animations
                              ${SVWidgetsLib_SOURCE_DIR}/Core
                              ${SVWidgetsLib_SOURCE_DIR}/Dialogs
                              ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets
                              ${SVWidgetsLib_SOURCE_DIR}/QtSupport
                              ${SVWidgetsLib_SOURCE_DIR}/Widgets
                              ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard
                              ${SVWidgetsLib_BINARY_DIR}/Animations
                              ${SVWidgetsLib_BINARY_DIR}/Core
                              ${SVWidgetsLib_BINARY_DIR}/Dialogs
                              ${SVWidgetsLib_BINARY_DIR}/FilterParameterWidgets
                              ${SVWidgetsLib_BINARY_DIR}/QtSupport
                              ${SVWidgetsLib_BINARY_DIR}/Widgets
                              ${SVWidgetsLib_BINARY_DIR}/Widgets/ImportASCIIDataWizard
                              ${SIMPLProj_SOURCE_DIR}/ThirdParty
)
if(WIN32 AND BUILD_SHARED_LIBS)
		target_compile_definitions(SVWidgetsLib PUBLIC "-DSVWidgetsLib_BUILT_AS_DYNAMIC_LIB")
endif()
target_compile_definitions(SVWidgetsLib PRIVATE -DQT_CORE_LIB)

LibraryProperties( SVWidgetsLib ${EXE_DEBUG_EXTENSION})
target_link_libraries(SVWidgetsLib ${SVWidgetsLib_LINK_LIBRARIES} QtWebAppLib)

set(install_dir "tools")
set(lib_install_dir "lib")
if(WIN32)
    set(install_dir ".")
    set(lib_install_dir "")
endif()


if(BUILD_SHARED_LIBS AND NOT APPLE)
    INSTALL(TARGETS ${PROJECT_NAME}
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}
        BUNDLE DESTINATION "."
    )
endif()

