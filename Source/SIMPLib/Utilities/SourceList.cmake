
set(SUBDIR_NAME Utilities)

# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the hierarchy needs to have its header listed here
set(SIMPLib_${SUBDIR_NAME}_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLH5DataReader.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/TestObserver.h
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
# QT5_WRAP_CPP( SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)


set(SIMPLib_Utilities_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ColorTable.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ColorUtilities.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilePathGenerator.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FloatSummation.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLDataPathValidator.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLH5DataReaderRequirements.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibEndian.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/StringOperations.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/TimeUtilities.h
)

set(SIMPLib_Utilities_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ColorTable.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ColorUtilities.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilePathGenerator.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FloatSummation.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLDataPathValidator.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLH5DataReader.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLH5DataReaderRequirements.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/StringOperations.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/TestObserver.cpp
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

if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_${SUBDIR_NAME}_HDRS}
                   ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}
            DESTINATION include/SIMPLib/${SUBDIR_NAME}
            COMPONENT Headers   )
endif()


#-------------------------------------------------------------------------------
# Add the unit testing sources
# -------------------------------------------------------------------- 
# If Testing is enabled, turn on the Unit Tests 
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()
