set(SUBDIR_NAME REST)

configure_file(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/SIMPLRestServer.ini ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SIMPLRestServer.ini )

# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(SIMPLib_${SUBDIR_NAME}_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/config.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/mrestrequest.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/mrestrequestmanager.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/mrestrequestptr.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/mrestrequestqueue.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/SIMPLClientRequest.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/SIMPLRestClient.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/userptr.h

  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/SIMPLRequestMapper.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/PipelineListener.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/SIMPLDirectoryListing.h

  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/V1RequestMapper.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/ExecutePipelineController.h      
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/NumFiltersController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/ListFilterParametersController.h 
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/PluginInfoController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/LoadedPluginsController.h        
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/PreflightPipelineController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/NamesOfFiltersController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/ApiNotFoundController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/SIMPLStaticFileController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/SIMPLibVersionController.h
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
# QT5_WRAP_CPP( SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

set(SIMPLib_${SUBDIR_NAME}_HDRS

)


set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/config.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/mrestrequest.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/mrestrequestmanager.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/mrestrequestqueue.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/SIMPLClientRequest.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTClient/SIMPLRestClient.cpp

  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/SIMPLRequestMapper.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/PipelineListener.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/SIMPLDirectoryListing.cpp

  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/NumFiltersController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/V1RequestMapper.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/ExecutePipelineController.cpp      
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/NumFiltersController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/ListFilterParametersController.cpp 
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/PluginInfoController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/LoadedPluginsController.cpp        
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/PreflightPipelineController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/NamesOfFiltersController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/ApiNotFoundController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/SIMPLStaticFileController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RESTServer/RESTServer/V1Controllers/SIMPLibVersionController.cpp

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

#-------------------------------------------------------------------------------
# Add the unit testing sources
# -------------------------------------------------------------------- 
# If Testing is enabled, turn on the Unit Tests 
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()


