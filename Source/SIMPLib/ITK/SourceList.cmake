set(SUBDIR_NAME ITK)

#-------------------------------------------------------------------------------
# Add the unit testing sources
# --------------------------------------------------------------------
# If Testing is enabled, turn on the Unit Tests
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()
