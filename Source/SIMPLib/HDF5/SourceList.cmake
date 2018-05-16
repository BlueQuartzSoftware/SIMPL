
set(SUBDIR_NAME HDF5)

set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_SOURCE_DIR}/HDF5/H5BoundaryStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5DataArrayReader.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5DataArrayWriter.hpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5Macros.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5MatrixStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrecipitateStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrimaryStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5StatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5TransformationStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/VTKH5Constants.h

)

set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/HDF5/H5BoundaryStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5DataArrayReader.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5MatrixStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrecipitateStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrimaryStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5StatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5TransformationStatsDataDelegate.cpp

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
