
from SIMPLibPy import *


def CreateDataContainerArray():
  """
  This method creates a DataContainerArray
  """
  dca = SIMPLibPy.DataContainerArray.New()
  return dca


def CreateDataContainer(name):
  """
  This method creates a DataContainer with a supplied name
  """
  dc = SIMPLibPy.DataContainer.New(name)
  return dc

def AttributeMatrixAccessTest():
  """
  Test the various methods that deal with AttributeMatrix
  and DataContainerArray
  """
  print("Starting AttributeMatrixAccessTest....")
  dca = CreateDataContainerArray()
  dc = CreateDataContainer("AM Test")
  dca.addDataContainer(dc)
  
  amType = SIMPLibPy.AttributeMatrix.Type.Cell
  tupleDims = SIMPLibPy.VectorSizeT([5,4,3])
  am = SIMPLibPy.AttributeMatrix.Create(tupleDims, "CellAttributeMatrix", amType)
  dc.addAttributeMatrix(am.Name, am)

  # See if we can get the AttributeMatrix based on a DataArrayPath object
  dap = SIMPLibPy.DataArrayPath("AM Test", "CellAttributeMatrix", "")
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
  dca = CreateDataContainerArray()
  dc0 = CreateDataContainer("dc0")
  dc1 = CreateDataContainer("dc1")

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


