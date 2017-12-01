
set(SUBDIR_NAME FilterParameters)

set(SIMPLib_${SUBDIR_NAME}_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicTableData.h
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
# QT5_WRAP_CPP( SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)


set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilterParametersReader.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilterParametersWriter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AttributeMatrixCreationFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AttributeMatrixSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AxisAngleFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AxisAngleInput.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/BooleanFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/CalculatorFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ChoiceFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonSelectionAdvancedFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ConstrainedDoubleFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ConstrainedIntFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataArrayCreationFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataArraySelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerArrayProxyFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerCreationFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerReaderFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DoubleFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicChoiceFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicTableFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FileListInfoFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FloatFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FloatVec2FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FloatVec3FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FourthOrderPolynomialFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GenerateColorTableFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/H5FilterParametersConstants.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/H5FilterParametersReader.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/H5FilterParametersWriter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/InputFileFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/InputPathFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IntFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IntVec3FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/JsonFilterParametersReader.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/JsonFilterParametersWriter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/LinkedBooleanFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/LinkedChoicesFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/LinkedDataContainerSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/MultiAttributeMatrixSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/MultiDataArraySelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/NumericTypeFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/OutputFileFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/OutputPathFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ParagraphFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PhaseTypeSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PreflightUpdatedValueFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RangeFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ImportHDF5DatasetFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ReadASCIIDataFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ScalarTypeFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SecondOrderPolynomialFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SeparatorFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ShapeTypeSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/StringFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ThirdOrderPolynomialFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/UnknownFilterParameter.h
)

set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilterParametersReader.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilterParametersWriter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AttributeMatrixCreationFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AttributeMatrixSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AxisAngleFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/BooleanFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/CalculatorFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ChoiceFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonSelectionAdvancedFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ConstrainedDoubleFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ConstrainedIntFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataArrayCreationFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataArraySelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerArrayProxyFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerCreationFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerReaderFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DoubleFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicChoiceFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicTableData.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicTableFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FileListInfoFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FloatFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FloatVec2FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FloatVec3FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FourthOrderPolynomialFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/GenerateColorTableFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/H5FilterParametersReader.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/H5FilterParametersWriter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/InputFileFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/InputPathFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IntFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IntVec3FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/JsonFilterParametersReader.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/JsonFilterParametersWriter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/LinkedBooleanFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/LinkedChoicesFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/LinkedDataContainerSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/MultiAttributeMatrixSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/MultiDataArraySelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/NumericTypeFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/OutputFileFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/OutputPathFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ParagraphFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PhaseTypeSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PreflightUpdatedValueFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/RangeFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ReadASCIIDataFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ImportHDF5DatasetFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ScalarTypeFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SecondOrderPolynomialFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SeparatorFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ShapeTypeSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/StringFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ThirdOrderPolynomialFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/UnknownFilterParameter.cpp
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
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()

