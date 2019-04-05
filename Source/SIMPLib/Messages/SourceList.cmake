
set(SUBDIR_NAME Messages)

# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(SIMPLib_${SUBDIR_NAME}_Moc_HDRS
  # ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DocRequestManager.h
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
# QT5_WRAP_CPP( SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractMessageHandler.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractErrorMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractProgressMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractStatusMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractWarningMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterErrorMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterProgressMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterStatusMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterWarningMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GenericErrorMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GenericProgressMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GenericStatusMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GenericWarningMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineErrorMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineProgressMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineStatusMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineWarningMessage.h
)


set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractMessageHandler.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterErrorMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterProgressMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterStatusMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterWarningMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GenericErrorMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GenericProgressMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GenericStatusMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GenericWarningMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineErrorMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineProgressMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineStatusMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineWarningMessage.cpp
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


