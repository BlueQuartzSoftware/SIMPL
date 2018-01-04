
set(SUBDIR_NAME Filtering)

# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(SIMPLib_${SUBDIR_NAME}_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractDecisionFilter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonInputs.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonInputsAdvanced.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterPipeline.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/CorePlugin.h
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
# QT5_WRAP_CPP( SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractComparison.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonSet.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonValue.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/CoreConstants.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterFactory.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterManager.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IFilterFactory.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/QMetaObjectUtilities.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ThresholdFilterHelper.h
)


set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractComparison.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractDecisionFilter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonInputs.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonInputsAdvanced.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonSet.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonValue.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/CorePlugin.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterManager.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterPipeline.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/QMetaObjectUtilities.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ThresholdFilterHelper.cpp
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

if(MSVC)
  set_source_files_properties(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterManager.cpp PROPERTIES COMPILE_FLAGS /bigobj)
  set_source_files_properties(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterManager.h PROPERTIES COMPILE_FLAGS /bigobj)
endif()


#-------------------------------------------------------------------------------
# Add the unit testing sources
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()

