set(SUBDIR_NAME REST)

set(REST_SOURCE_DIR "${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}")

# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(SIMPLib_${SUBDIR_NAME}_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLRequestMapper.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineListener.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLDirectoryListing.h

  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/V1RequestMapper.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/ExecutePipelineController.h      
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/NumFiltersController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/ListFilterParametersController.h 
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/PluginInfoController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/LoadedPluginsController.h        
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/PreflightPipelineController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/NamesOfFiltersController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/ApiNotFoundController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/SIMPLStaticFileController.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/SIMPLibVersionController.h
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
# QT5_WRAP_CPP( SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

set(SIMPLib_${SUBDIR_NAME}_HDRS

)


set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLRequestMapper.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineListener.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLDirectoryListing.cpp

  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/NumFiltersController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/V1RequestMapper.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/ExecutePipelineController.cpp      
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/NumFiltersController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/ListFilterParametersController.cpp 
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/PluginInfoController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/LoadedPluginsController.cpp        
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/PreflightPipelineController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/NamesOfFiltersController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/ApiNotFoundController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/SIMPLStaticFileController.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/V1Controllers/SIMPLibVersionController.cpp

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

#-------------------------------------------------------------------------------
# Add the unit testing sources
# -------------------------------------------------------------------- 
# If Testing is enabled, turn on the Unit Tests 
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()


