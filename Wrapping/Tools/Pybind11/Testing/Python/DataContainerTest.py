
from SIMPLibPy import *


def CreateAttributeMatrix():
  """
  Creates a Cell AttributeMatrix and returns it.
  """
  amType = SIMPLibPy.AttributeMatrix.Type.Cell
  tupleDims = SIMPLibPy.VectorSizeT([5,4,3])
  am = SIMPLibPy.AttributeMatrix.Create(tupleDims, "CellAttributeMatrix", amType)
  return am

def DataContainerTest() :
  """
  Tests various methods of the DataContainer Class
  """
  # Create a DataContainer
  dc = SIMPLibPy.DataContainer.New("Default Name")
  dc.Name = "ImageDataContainer"
  
  am = CreateAttributeMatrix()
  dc.addAttributeMatrix(am.Name, am)
  exists = dc.doesAttributeMatrixExist("CellAttributeMatrix")
  print("Exists: %s" % exists)




"""
Main entry point for python script
"""
if __name__ == "__main__":
  DataContainerTest()
  print("DataContainerTest Test Complete")
