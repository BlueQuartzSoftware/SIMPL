import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd

def DataContainerTest() :
  '''
  Tests various methods of the DataContainer Class
  '''

  dca = sc.CreateDataContainerArray()

  # Create a DataContainer
  dc = simpl.DataContainer('Default Name')
  assert dc.Name == 'Default Name'
  dc.Name = 'ImageDataContainer'
  assert dc.Name == 'ImageDataContainer'
  
  amType = simpl.AttributeMatrix.Type.Cell
  tupleDims = simpl.VectorSizeT([5, 4, 3])
  amName = 'CellAttributeMatrix'
  am = sc.CreateAttributeMatrix(tupleDims, amName, amType)

  dc.addOrReplaceAttributeMatrix(am)

  assert dc.doesAttributeMatrixExist(amName)

if __name__ == '__main__':
  DataContainerTest()
  print('DataContainerTest Test Complete')
