

""" module to get build specific directories  """

def GetBuildDirectory():
  return "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"

def GetTestDirectory():
  return "${SIMPLProj_BINARY_DIR}/Testing"

def GetTestTempDirectory():
  return "${SIMPLProj_BINARY_DIR}/Testing/Temporary"

def GetDataDirectory():
  return "${DREAM3D_DATA_DIR}"
