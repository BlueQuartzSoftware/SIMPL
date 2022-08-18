
set(TEST_TESTFILES_DIR ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/TestFiles)

set(TEST_${SUBDIR_NAME}_NAMES
  WriteASCIIDataTest
  ApplyImageTransformsTest
  ArrayCalculatorTest
  CombineAttributeArraysTest
  CombineAttributeMatricesTest
  ConditionalSetValueTest
  ConvertDataTest
  ConvertColorToGrayScaleTest
  CopyFeatureArrayToElementArrayTest
  CopyObjectTest
  CreateAttributeMatrixTest
  CreateDataArrayTest
  CreateDataContainerTest
  CreateFeatureArrayFromElementArrayTest
  CreateGeometryTest
  CreateImageGeometryTest
  CreateStringArrayTest
  CropVertexGeometryTest
  DataContainerTest
  ErrorMessageTest
  ExecuteProcessTest
  ExportDataTest
  ExtractAttributeArraysFromGeometryTest
  ExtractComponentAsArrayTest
  ExtractVertexGeometryTest
  FindDerivativesFilterTest
  FeatureDataCSVWriterTest
  GenerateColorTableTest
  GenerateTiltSeriesTest
  ImportAsciDataArrayTest
  ImportHDF5DatasetTest
  MoveDataTest
  MoveMultiDataTest
  MultiThresholdObjectsTest
  MultiThresholdObjects2Test
  PadImageGeometryTest
  RawBinaryReaderTest
  ReadASCIIDataTest
  RemoveArraysTest
  RemoveComponentFromArrayTest
  RenameAttributeArrayTest
  RenameAttributeMatrixTest
  RenameDataContainerTest
  # RenameTimingTest
  ReplaceValueTest
  RequiredZThicknessTest
  RotateSampleRefFrameTest
  ScaleVolumeTest
  SetOriginResolutionImageGeomTest
  SplitAttributeArrayTest
  WriteTriangleGeometryTest
  GenerateVertexCoordinatesTest
)


if( SIMPL_USE_ITK )
  set(TEST_${SUBDIR_NAME}_NAMES ${TEST_${SUBDIR_NAME}_NAMES}
  )
endif()



SIMPL_ADD_UNIT_TEST("${TEST_${SUBDIR_NAME}_NAMES}" "${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx")
SIMPL_ADD_UNIT_TEST_MOC_FILE(ExecuteProcessTest "${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx")

set(SIMPLTest_SOURCE_DIR ${SIMPLib_SOURCE_DIR}/Testing)
set(SIMPLTest_BINARY_DIR ${SIMPLib_BINARY_DIR}/Testing)

# # QT5_WRAP_CPP( execute_process_moc  ${SIMPLTest_SOURCE_DIR}/ExecuteProcessTest.cpp)
# set_source_files_properties( ${execute_process_moc} PROPERTIES GENERATED TRUE)
# set_source_files_properties( ${execute_process_moc} PROPERTIES HEADER_FILE_ONLY TRUE)
