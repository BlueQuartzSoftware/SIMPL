
from SIMPLibPy import *


def AttributeMatrixTest() :
  # Create an AttributeMatrix
  amType = SIMPLibPy.AttributeMatrix.Type.Cell
  tupleDims = SIMPLibPy.VectorSizeT([5,4,3])
  am = SIMPLibPy.AttributeMatrix.Create(tupleDims, "CellAttributeMatrix", amType)
  amName = am.Name
  print("Name: %s" % amName)
  arrayExist = am.doesAttributeArrayExist("Test Data Array")
  print("arrayExist: %s" % arrayExist)


"""
Main entry point for python script
"""
if __name__ == "__main__":
  AttributeMatrixTest()
  print("Test Complete")
