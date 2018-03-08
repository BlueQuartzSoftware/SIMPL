

""" module to get build specific directories  """

def GetBuildDirectory():
  return "${SIMPLProj_BINARY_DIR}"

def GetTestDirectory():
  return "${SIMPLProj_BINARY_DIR}/Testing"

def GetTestTempDirectory():
  return "${SIMPLProj_BINARY_DIR}/Testing/Temporary"

def GetDataDirectory():
  return "${DREAM3D_DATA_DIR}"
