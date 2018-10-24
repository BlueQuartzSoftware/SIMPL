set(SUBDIR_NAME REST)

set(REST_SOURCE_DIR "${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}")

configure_file(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLRestServer.ini ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RESTServer.ini )

# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(SIMPLib_${SUBDIR_NAME}_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/HttpResponseParser.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/SIMPLRequestMapper.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/PipelineListener.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/SIMPLDirectoryListing.h

  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/V1RequestMapper.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/ExecutePipelineController.h      
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/NumFiltersController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/ListFilterParametersController.h 
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/PluginInfoController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/LoadedPluginsController.h        
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/PreflightPipelineController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/NamesOfFiltersController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/ApiNotFoundController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/SIMPLStaticFileController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/SIMPLibVersionController.h
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
# QT5_WRAP_CPP( SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

set(SIMPLib_${SUBDIR_NAME}_HDRS

)


set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/HttpResponseParser.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/SIMPLRequestMapper.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/PipelineListener.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/SIMPLDirectoryListing.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/main.cpp

  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/NumFiltersController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/V1RequestMapper.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/ExecutePipelineController.cpp      
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/NumFiltersController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/ListFilterParametersController.cpp 
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/PluginInfoController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/LoadedPluginsController.cpp        
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/PreflightPipelineController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/NamesOfFiltersController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/ApiNotFoundController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/SIMPLStaticFileController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/V1Controllers/SIMPLibVersionController.cpp

)

cmp_IDE_SOURCE_PROPERTIES( "${SUBDIR_NAME}" "${SIMPLib_${SUBDIR_NAME}_HDRS};${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}" "${SIMPLib_${SUBDIR_NAME}_SRCS}" "${PROJECT_INSTALL_HEADERS}")
cmp_IDE_SOURCE_PROPERTIES( "Generated/${SUBDIR_NAME}" "" "${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS}" "0")

set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_${SUBDIR_NAME}_HDRS}
  ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}  # Add the headers that get Moc'ed here so they show up in solutions/IDEs/Project files
)

set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_${SUBDIR_NAME}_SRCS}
  ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS}
)

include_directories(${SIMPLProj_SOURCE_DIR}/ThirdParty)

link_libraries(QtWebAppLib)

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

BuildToolBundle(
    TARGET RESTServer
    SOURCES ${SIMPLib_${SUBDIR_NAME}_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${SIMPL_VER_MAJOR}
    VERSION_MINOR ${SIMPL_VER_MINOR}
    VERSION_PATCH ${SIMPL_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    LINK_LIBRARIES Qt5::Core Qt5::Network SIMPLib QtWebAppLib
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMPONENT     Tools
    INSTALL_DEST  "${install_dir}"
    SOLUTION_FOLDER "Applications"
)


#-------------------------------------------------------------------------------
# Add the unit testing sources
# -------------------------------------------------------------------- 
# If Testing is enabled, turn on the Unit Tests 
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()


