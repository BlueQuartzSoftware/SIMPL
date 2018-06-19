
from simpl_py import *
import simpl.simpl_common as sc
import simpl.simpl_test_dirs as sd

def AttributeMatrixTest() :
  # Create an AttributeMatrix
  amType = simpl_py.AttributeMatrix.Type.Cell
  tupleDims = simpl_py.VectorSizeT([5,4,3])
  amName = "CellAttributeMatrix"
  am = sc.CreateAttributeMatrix(tupleDims, amName, amType)

  print("Name: %s" % am.Name)

  arrayExist = am.doesAttributeArrayExist("Test Data Array")
  assert arrayExist == False
  print("arrayExist: %s" % arrayExist)


"""
Main entry point for python script
"""
if __name__ == "__main__":
  AttributeMatrixTest()
  print("AttributeMatrixTest Test Complete")
