
set(TEST_${SUBDIR_NAME}_NAMES
  DataArrayTest
  StringDataArrayTest
  StructArrayTest
)

SIMPL_ADD_UNIT_TEST("${TEST_${SUBDIR_NAME}_NAMES}" "${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx")
