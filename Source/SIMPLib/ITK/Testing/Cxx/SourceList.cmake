set(TEST_${SUBDIR_NAME}_NAMES
  itkDream3DITransformContainerToTransformTest
  itkDream3DTransformContainerToTransformTest
  itkTransformToDream3DTransformContainerTest
  itkTransformToDream3DITransformContainerTest
)

include( ${CMP_SOURCE_DIR}/ITKSupport/IncludeITK.cmake)
SIMPL_ADD_UNIT_TEST("${TEST_${SUBDIR_NAME}_NAMES}" "${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx")
