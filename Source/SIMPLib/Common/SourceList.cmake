
set(SUBDIR_NAME Common)

# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(SIMPLib_${SUBDIR_NAME}_Moc_HDRS

  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DocRequestManager.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/EnsembleInfo.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Observable.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Observer.h
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
# QT5_WRAP_CPP( SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AppVersion.h

  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Constants.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/CreatedArrayHelpIndexEntry.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IObserver.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PhaseType.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibDLLExport.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibSetGetMacros.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ScopedFileMonitor.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ShapeType.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/TemplateHelpers.hpp
)


set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AppVersion.cpp

  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/CreatedArrayHelpIndexEntry.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DocRequestManager.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/EnsembleInfo.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IObserver.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Observable.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Observer.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PhaseType.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineMessage.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ShapeType.cpp

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


