set(SUBDIR_NAME ITK)

set(SIMPLib_${SUBDIR_NAME}_HDRS
)

set(SIMPLib_${SUBDIR_NAME}_SRCS
)

cmp_IDE_SOURCE_PROPERTIES( "${SUBDIR_NAME}" "${SIMPLib_${SUBDIR_NAME}_HDRS};${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}" "${SIMPLib_${SUBDIR_NAME}_SRCS}" "${PROJECT_INSTALL_HEADERS}")
cmp_IDE_SOURCE_PROPERTIES( "Generated/${SUBDIR_NAME}" "" "${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS}" "0")


#-------------------------------------------------------------------------------
# Add the unit testing sources
# --------------------------------------------------------------------
# If Testing is enabled, turn on the Unit Tests
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()
