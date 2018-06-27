""" This is a test for DataContainer """

# These are the simpl_py python modules

import dream3d
import dream3d.dream3d_py
import dream3d.dream3d_py as d3d
import dream3d.dream3d_py.simpl_py as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd


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
  dc = simpl.DataContainer.New("Default Name")
  assert dc.Name == "Default Name"
  dc.Name = "ImageDataContainer"
  assert dc.Name == "ImageDataContainer"
  
  amType = simpl.AttributeMatrix.Type.Cell
  tupleDims = simpl.VectorSizeT([5,4,3])
  amName = "CellAttributeMatrix"
  am = sc.CreateAttributeMatrix(tupleDims, amName, amType)

  dc.addAttributeMatrix(am.Name, am)

  exists = dc.doesAttributeMatrixExist("CellAttributeMatrix")
  
  print("Exists: %s" % exists)




"""
Main entry point for python script
"""
if __name__ == "__main__":
  DataContainerTest()
  print("DataContainerTest Test Complete")
