
set(SUBDIR_NAME DataArrays)


set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataArray.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IDataArray.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IDataArrayFilter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/NeighborList.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/StatsDataArray.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/StringDataArray.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/StructArray.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicListArray.hpp
)

set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IDataArray.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IDataArrayFilter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/StatsDataArray.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/StringDataArray.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "${SUBDIR_NAME}" "${SIMPLib_${SUBDIR_NAME}_HDRS};${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}" "${SIMPLib_${SUBDIR_NAME}_SRCS}" "${PROJECT_INSTALL_HEADERS}")
cmp_IDE_SOURCE_PROPERTIES( "Generated/${SUBDIR_NAME}" "" "${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS}" "0")

message(STATUS "SIMPL_BUILD_TESTING: ${SIMPL_BUILD_TESTING}")
#-------------------------------------------------------------------------------
# Add the unit testing sources
# -------------------------------------------------------------------- 
# If Testing is enabled, turn on the Unit Tests 
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()
