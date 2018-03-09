
set(SUBDIR_NAME Math)

set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ArrayHelpers.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GeometryMath.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/MatrixMath.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/QuaternionMath.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RadialDistributionFunction.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibMath.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibRandom.h
)
set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GeometryMath.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/MatrixMath.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RadialDistributionFunction.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibMath.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibRandom.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "${SUBDIR_NAME}" "${SIMPLib_${SUBDIR_NAME}_HDRS};${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}" "${SIMPLib_${SUBDIR_NAME}_SRCS}" "${PROJECT_INSTALL_HEADERS}")
cmp_IDE_SOURCE_PROPERTIES( "Generated/${SUBDIR_NAME}" "" "${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS}" "0")

if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_Math_HDRS}
            DESTINATION include/SIMPLib/Math
            COMPONENT Headers   )
endif()

#-------------------------------------------------------------------------------
# Add the unit testing sources
# -------------------------------------------------------------------- 
# If Testing is enabled, turn on the Unit Tests 
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()
