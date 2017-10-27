
set(TEST_${SUBDIR_NAME}_NAMES
  ArrayCalculatorTest
  CombineAttributeArraysTest
  CombineAttributeMatricesTest
  ConditionalSetValueTest
  ConvertDataTest
  CreateAttributeMatrixTest
  CreateDataArrayTest
  CreateDataContainerTest
  CreateFeatureArrayFromElementArrayTest
  CreateImageGeometryTest
  DataContainerTest
  ErrorMessageTest
  ExecuteProcessTest
  ExtractComponentAsArrayTest
  FindDerivativesFilterTest
  GenerateColorTableTest
  ImportAsciDataArrayTest
  ImportHDF5DatasetTest
  RawBinaryReaderTest
  ReadASCIIDataTest
  RecursivePipelineTest
  RemoveArraysTest
  RemoveComponentFromArrayTest
  RenameAttributeArrayTest
  RenameAttributeMatrixTest
  RenameDataContainerTest
  ReplaceValueTest
  RequiredZThicknessTest
  ScaleVolumeTest
  SetOriginResolutionImageGeomTest
  WriteASCIIDataTest
)

SIMPL_ADD_UNIT_TEST("${TEST_${SUBDIR_NAME}_NAMES}" "${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx")
SIMPL_ADD_UNIT_TEST_MOC_FILE(ExecuteProcessTest "${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx")

set(SIMPLTest_SOURCE_DIR ${SIMPLib_SOURCE_DIR}/Testing)
set(SIMPLTest_BINARY_DIR ${SIMPLib_BINARY_DIR}/Testing)
#-------------------------------------------------------------------------------
#- This copies all the Prebuilt Pipeline files into the Build directory so the help
#- works from the Build Tree
add_custom_target(ImageASCIICopy ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${SIMPLTest_SOURCE_DIR}/ImageASCII
            ${SIMPLTest_BINARY_DIR}/ImageASCII/
            COMMENT "Copying Test Images into Binary Directory")
set_target_properties(ImageASCIICopy PROPERTIES FOLDER ZZ_COPY_FILES)



# QT5_WRAP_CPP( execute_process_moc  ${SIMPLTest_SOURCE_DIR}/ExecuteProcessTest.cpp)
# set_source_files_properties( ${execute_process_moc} PROPERTIES GENERATED TRUE)
# set_source_files_properties( ${execute_process_moc} PROPERTIES HEADER_FILE_ONLY TRUE)
