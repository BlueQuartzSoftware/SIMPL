# Based on CreateVertexGeometry example

import simpl
import simplpy as d3d
import simpl_helpers as sc
import simpl_test_dirs as sd

def CreateVertexGeometryTest():
  # Create Data Container Array
  dca = simpl.DataContainerArray()

  # Create the Data Container
  err = d3d.create_data_container(dca, 'DataContainer')
  if err < 0:
    print('DataContainer ErrorCondition: %d' % err)

  # Import ASCII Data - #1 - Vertex Coordinates
  importFile = sd.GetBuildDirectory() + '/Data/SIMPL/VertexCoordinates.csv'
  wizardData = {
    'inputFilePath': importFile,
    'beginIndex': 2,
    'numberOfLines': 145,
    'delimiters': [','],    
    'consecutiveDelimiters': False,
    'automaticAM': True,
    'selectedPath': simpl.DataArrayPath('DataContainer', 'Bounds', ''),
    'headers': ['x', 'y', 'z'],
    'attrMatType': 3,
    'tupleDimensions': [144],
    'dataTypes': ['float', 'float', 'float']
  }
  err = d3d.read_ascii_data(dca, wizardData)
  if err < 0:
    print('Import ASCII Data #1 -  ErrorCondition: %d' % err)
  
  # Combine Attribute Arrays # 1:
  selectedDataArrayPaths = [simpl.DataArrayPath('DataContainer', 'Bounds', 'x'), 
  simpl.DataArrayPath('DataContainer', 'Bounds', 'y'), 
  simpl.DataArrayPath('DataContainer', 'Bounds', 'z')]
  err = d3d.combine_attribute_arrays(dca, selectedDataArrayPaths, 'Vertices', False)
  if err < 0:
    print('Combined Attribute Arrays #1 -  ErrorCondition: %d' % err)

  # Delete Data # 1
  dcap = simpl.DataContainerArrayProxy()
  dcap.getDataContainerProxy('DataContainer').Flag = 0
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').Flag = 0
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').getDataArrayProxy('x').Flag = 2
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').getDataArrayProxy('y').Flag = 2
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').getDataArrayProxy('z').Flag = 2
  err = d3d.remove_arrays(dca, dcap)
  if err < 0:
    print('Remove Arrays #1 -  ErrorCondition: %d' % err)

  # Create Geometry
  data_container_name = 'DataContainer'
  shared_vertex_list_array_path = simpl.DataArrayPath('DataContainer', 'Bounds', 'Vertices')
  vertex_attribute_matrix_name = 'VertexData'
  err = sc.CreateGeometry(dca, 0, simpl.IGeometry.Type.Vertex, 'DataContainer', False, 
  shared_vertex_list_array_path = simpl.DataArrayPath('DataContainer', 'Bounds', 'Vertices'),
  vertex_attribute_matrix_name = 'VertexData')
  if err < 0:
    print('Create Geometry -  ErrorCondition: %d' % err)

  err = d3d.data_container_writer(dca, sd.GetTestTempDirectory() + '/CreateVertexGeometry.dream3d', True, False)
  if err < 0:
    print('DataContainerWriter ErrorCondition: %d' % err)

if __name__ == '__main__':
  CreateVertexGeometryTest()
