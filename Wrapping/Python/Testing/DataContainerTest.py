""" This is a test for DataContainer """

# These are the simpl_py python modules
from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd


def CreateAttributeMatrix():
  """
  Creates a Cell AttributeMatrix and returns it.
  """
  
  am = simpl.AttributeMatrix.Create(tupleDims, "CellAttributeMatrix", amType)
  return am

def DataContainerTest() :
  """
  Tests various methods of the DataContainer Class
  """

  dca = sc.CreateDataContainerArray()

  # Create a DataContainer
  dc = simpl.DataContainer("Default Name")
  assert dc.Name == "Default Name"
  dc.Name = "ImageDataContainer"
  assert dc.Name == "ImageDataContainer"
  
  amType = simpl.AttributeMatrix.Type.Cell
  tupleDims = simpl.VectorSizeT([5,4,3])
  amName = "CellAttributeMatrix"
  am = sc.CreateAttributeMatrix(tupleDims, amName, amType)

  dc.addOrReplaceAttributeMatrix(am)

  exists = dc.doesAttributeMatrixExist("CellAttributeMatrix")
  
  print("Exists: %s" % exists)




"""
Main entry point for python script
"""
if __name__ == "__main__":
  DataContainerTest()
  print("DataContainerTest Test Complete")
