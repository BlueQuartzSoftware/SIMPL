# set project's name
PROJECT( RESTClient VERSION ${SIMPLProj_VERSION_MAJOR}.${SIMPLProj_VERSION_MINOR})

set(PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Source)
if(NOT PROJECT_RESOURCES_DIR)
  set(PROJECT_RESOURCES_DIR ${RESTClient_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
  set(CMP_SOURCE_DIR ${RESTClient_SOURCE_DIR}/Support/cmp)
endif()

set(PROJECT_PREFIX "RESTClient" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(RESTClient_INSTALL_FILES 0)
# -----------------------------------------------------------------------
# The project generally does NOT want to install headers. See if another
# project has over ridden that property
get_property(CMP_PROJECT_INSTALL_HEADERS GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS)
if("${CMP_PROJECT_INSTALL_HEADERS}" STREQUAL "")
  set_property(GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS "0")
  get_property(CMP_PROJECT_INSTALL_HEADERS GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS)
endif()
set(RESTClient_INSTALL_FILES "${CMP_PROJECT_INSTALL_HEADERS}")

set(RESTClient_BUILT_AS_DYNAMIC_LIB)
if(BUILD_SHARED_LIBS)
    set(RESTClient_BUILT_AS_DYNAMIC_LIB 1)
endif(BUILD_SHARED_LIBS)

#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
set(CMP_HEADER_DIR ${${PROJECT_NAME}_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "${PROJECT_NAME}Configuration.h")
set(CMP_EXPORT_FILE_NAME "${PROJECT_NAME}DLLExport.h")
set(CMP_TYPES_FILE_NAME "${PROJECT_NAME}Types.h")
set(CMP_VERSION_HEADER_FILE_NAME "${PROJECT_NAME}Version.h")
set(CMP_VERSION_SOURCE_FILE_NAME "${PROJECT_NAME}Version.cpp")
set(CMP_TOP_HEADER_FILE "${PROJECT_NAME}.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_GENERATE_VERSION_STRING 1)
set(PROJECT_INSTALL_HEADERS "${CMP_PROJECT_INSTALL_HEADERS}")

INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

set(RESTClient_HDRS
  ${RESTClient_SOURCE_DIR}/config.h
  ${RESTClient_SOURCE_DIR}/mrestrequest.h
  ${RESTClient_SOURCE_DIR}/mrestrequestmanager.h
  ${RESTClient_SOURCE_DIR}/mrestrequestptr.h
  ${RESTClient_SOURCE_DIR}/mrestrequestqueue.h
  ${RESTClient_SOURCE_DIR}/RESTClientDLLExport.h
  ${RESTClient_SOURCE_DIR}/SIMPLClientRequest.h
  ${RESTClient_SOURCE_DIR}/SIMPLRestClient.h
  ${RESTClient_SOURCE_DIR}/userptr.h
)

set(RESTClient_SRCS
  ${RESTClient_SOURCE_DIR}/config.cpp
  ${RESTClient_SOURCE_DIR}/mrestrequest.cpp
  ${RESTClient_SOURCE_DIR}/mrestrequestmanager.cpp
  ${RESTClient_SOURCE_DIR}/mrestrequestqueue.cpp
  ${RESTClient_SOURCE_DIR}/SIMPLClientRequest.cpp
  ${RESTClient_SOURCE_DIR}/SIMPLRestClient.cpp
)

#---------------------------------
# Now configure the main header file.
configure_file(${RESTClient_SOURCE_DIR}/RESTClientConfiguration.h.in
               ${RESTClient_BINARY_DIR}/${CMP_TOP_HEADER_FILE})

cmp_IDE_SOURCE_PROPERTIES( "" "${RESTClient_HDRS}" "${RESTClient_SRCS}" ${PROJECT_INSTALL_HEADERS})

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
# set(VERSION_GEN_NAME "RESTClient")
# set(VERSION_GEN_NAMESPACE "RESTClient")
# set(PROJECT_PREFIX "RESTClient")
# set(VERSION_GEN_COMPLETE  "1.0.0")
# set(VERSION_GEN_VER_MAJOR "1")
# set(VERSION_GEN_VER_MINOR "0")
# set(VERSION_GEN_VER_PATCH "0")
# set(VERSION_GEN_HEADER_FILE_NAME "RESTClientVersion.h")

# configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in
#                ${RESTClient_BINARY_DIR}/${PROJECT_NAME}/RESTClientVersion.h  )
# configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.cpp.in
#                ${RESTClient_BINARY_DIR}/${PROJECT_NAME}/RESTClientVersion.cpp  )


set(PROJECT_SRCS
    ${RESTClient_SRCS}
    ${RESTClient_HDRS}
    ${RESTClient_BINARY_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
)

add_library(${PROJECT_NAME} ${LIB_TYPE} ${PROJECT_SRCS})
CMP_AddDefinitions(TARGET ${PROJECT_NAME})
LibraryProperties( ${PROJECT_NAME}  ${EXE_DEBUG_EXTENSION} )
CMP_MODULE_INCLUDE_DIRS (TARGET ${PROJECT_NAME} LIBVARS)

get_filename_component(TARGET_BINARY_DIR_PARENT ${RESTClient_BINARY_DIR} PATH)
get_filename_component(TARGET_SOURCE_DIR_PARENT ${RESTClient_SOURCE_DIR} PATH)


TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} PUBLIC
                              $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
                              $<BUILD_INTERFACE:${TARGET_SOURCE_DIR_PARENT}>
                              $<BUILD_INTERFACE:${TARGET_BINARY_DIR_PARENT}>
                              )

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network)

if(BUILD_SHARED_LIBS)
  if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC "-DRESTClient_BUILT_AS_DYNAMIC_LIB")
  endif(WIN32)
endif(BUILD_SHARED_LIBS)

set(install_dir "bin")
set(lib_install_dir "lib")

if(APPLE AND NOT DREAM3D_ANACONDA)
  get_property(DREAM3D_PACKAGE_DEST_PREFIX GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX)
  set(install_dir "${DREAM3D_PACKAGE_DEST_PREFIX}bin")
  set(lib_install_dir "${DREAM3D_PACKAGE_DEST_PREFIX}lib")
elseif(WIN32)
  set(install_dir ".")
  set(lib_install_dir ".")
endif()


INSTALL(TARGETS ${PROJECT_NAME}
    EXPORT RESTClientTargets
    COMPONENT Applications
    RUNTIME DESTINATION ${install_dir}
    LIBRARY DESTINATION ${lib_install_dir}
    ARCHIVE DESTINATION lib
    BUNDLE DESTINATION "."
)


# --------------------------------------------------------------------
# Allow the generation and installation of a CMake configuration file
# which makes using SIMPL from another project easier.
# --------------------------------------------------------------------
export(EXPORT RESTClientTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake"
  NAMESPACE RESTClient::
)

if (RESTClient_INSTALL_FILES)
  install(EXPORT RESTClientTargets
    FILE
      ${PROJECT_NAME}Targets.cmake
    NAMESPACE
      ${PROJECT_NAME}::
    DESTINATION
      ${ConfigPackageLocation}
  )

  install(FILES 
      ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE}
      ${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}
      ${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}
      ${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
    DESTINATION
      include/${PROJECT_NAME}
  )

endif()


