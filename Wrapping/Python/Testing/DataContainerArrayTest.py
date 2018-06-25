
""" This is a test for DataContainerArray """

import dream3d
import dream3d.dream3d_py
import dream3d.dream3d_py as d3d
import dream3d.dream3d_py.simpl_py as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd


def AttributeMatrixAccessTest():
  """
  Test the various methods that deal with AttributeMatrix
  and DataContainerArray
  """
  print("Starting AttributeMatrixAccessTest....")
  dca = sc.CreateDataContainerArray()
  dc = sc.CreateDataContainer("AM Test")
  dca.addDataContainer(dc)
  
  amType = simpl.AttributeMatrix.Type.Cell
  tupleDims = simpl.VectorSizeT([5,4,3])
  am = simpl.AttributeMatrix.Create(tupleDims, "CellAttributeMatrix", amType)
  dc.addAttributeMatrix(am.Name, am)

  # See if we can get the AttributeMatrix based on a DataArrayPath object
  dap = simpl.DataArrayPath("AM Test", "CellAttributeMatrix", "")
  am0 = dca.getAttributeMatrix(dap)
  assert am0 == am
  assert True == dca.doesAttributeMatrixExist(dap)

  # Change the data array path to something that does not exist.
  dap.update("AM Test", "Foo", "")
  am0 = dca.getAttributeMatrix(dap)
  assert am0 != am
  assert False == dca.doesAttributeMatrixExist(dap)


def DataContainerTest():
  """
  This is the main unit test
  This needs to be updated as more methods are exposed to Python
  """
  print("Starting DataContainerTest....")
  dca = sc.CreateDataContainerArray()
  dc0 = sc.CreateDataContainer("dc0")
  dc1 = sc.CreateDataContainer("dc1")

  dca.addDataContainer(dc0)
  dca.addDataContainer(dc1)

  exist = dca.doesDataContainerExist("dc0")
  assert exist == True
  exist = dca.doesDataContainerExist("dc1")
  assert exist == True

  dc0 = dca.removeDataContainer("dc0")
  exist = dca.doesDataContainerExist("dc0")
  assert exist == False

  dca.renameDataContainer("dc1", "dc0")
  exist = dca.doesDataContainerExist("dc1")
  assert exist == False
  exist = dca.doesDataContainerExist("dc0")
  assert exist == True

  numDC = dca.getNumDataContainers()
  assert numDC == 1

  dca.duplicateDataContainer("dc0", "dc1")
  numDC = dca.getNumDataContainers()
  assert numDC == 2

  dca.clearDataContainers()
  numDC = dca.getNumDataContainers()
  assert numDC == 0  


"""
Main entry point for python script
"""
if __name__ == "__main__":
  DataContainerTest()
  AttributeMatrixAccessTest()
  print("DataContainerArrayTest Test Complete")


