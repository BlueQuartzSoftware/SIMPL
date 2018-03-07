
from simpl import *


def AttributeMatrixTest() :
  # Create an AttributeMatrix
  amType = simpl.AttributeMatrix.Type.Cell
  tupleDims = simpl.VectorSizeT([5,4,3])
  am = simpl.AttributeMatrix.Create(tupleDims, "CellAttributeMatrix", amType)
  amName = am.Name
  print("Name: %s" % amName)
  arrayExist = am.doesAttributeArrayExist("Test Data Array")
  assert arrayExist == False
  print("arrayExist: %s" % arrayExist)


"""
Main entry point for python script
"""
if __name__ == "__main__":
  AttributeMatrixTest()
  print("AttributeMatrixTest Test Complete")
