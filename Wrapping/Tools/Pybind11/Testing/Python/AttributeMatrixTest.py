
from simpl import *
import simpl_dirs as sd
import simpl_common as sc

def AttributeMatrixTest() :
  # Create an AttributeMatrix
  amType = simpl.AttributeMatrix.Type.Cell
  tupleDims = simpl.VectorSizeT([5,4,3])
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
