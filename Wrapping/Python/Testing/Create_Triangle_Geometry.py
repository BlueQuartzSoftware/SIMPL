# Based on CreateTriangleGeometry example

import simpl
import simplpy as d3d
import simpl_helpers as sh
import simpl_test_dirs as sd

def CreateTriangleGeometryTest():
  # Create Data Container Array
  dca = simpl.DataContainerArray()

  # Create the Data Container
  err = d3d.create_data_container(dca, 'DataContainer')
  assert err == 0, f'DataContainer ErrorCondition: {err}'

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
  assert err == 0, f'Import ASCII Data #1 -  ErrorCondition: {err}'
  
  # Combine Attribute Arrays # 1:
  selectedDataArrayPaths = [simpl.DataArrayPath('DataContainer', 'Bounds', 'x'), 
  simpl.DataArrayPath('DataContainer', 'Bounds', 'y'), 
  simpl.DataArrayPath('DataContainer', 'Bounds', 'z')]
  err = d3d.combine_attribute_arrays(dca, selectedDataArrayPaths, 'Vertices', False)
  assert err == 0, f'Combined Attribute Arrays #1 -  ErrorCondition: {err}'

  # Delete Data # 1
  dcap = simpl.DataContainerArrayProxy()
  dcap.getDataContainerProxy('DataContainer').Flag = 0
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').Flag = 0
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').getDataArrayProxy('x').Flag = 2
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').getDataArrayProxy('y').Flag = 2
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').getDataArrayProxy('z').Flag = 2
  err = d3d.remove_arrays(dca, dcap)
  assert err == 0, f'Remove Arrays #1 -  ErrorCondition: {err}'

  # Import ASCII Data - #2 - Triangle Connectivity
  importFile = sd.GetBuildDirectory() + '/Data/SIMPL/TriangleConnectivity.csv'
  wizardData = {
    'inputFilePath': importFile,
    'beginIndex': 2,
    'numberOfLines': 243,
    'delimiters': [','],    
    'consecutiveDelimiters': False,
    'automaticAM': True,
    'selectedPath': simpl.DataArrayPath('DataContainer', 'Triangles', ''),
    'headers': ['V0', 'V1', 'V2'],
    'attrMatType': 3,
    'tupleDimensions': [242],
    'dataTypes': ['uint64_t', 'uint64_t','uint64_t']
  }  
  err = d3d.read_ascii_data(dca, wizardData)
  assert err == 0, f'Import ASCII Data #2 -  ErrorCondition: {err}'

  # Combine Attribute Arrays #2:
  selectedDataArrayPaths = [simpl.DataArrayPath('DataContainer', 'Triangles', 'V0'), 
  simpl.DataArrayPath('DataContainer', 'Triangles', 'V1'), simpl.DataArrayPath('DataContainer', 'Triangles', 'V2')]
  err = d3d.combine_attribute_arrays(dca, selectedDataArrayPaths, 'Triangles', False)
  assert err == 0, f'Combined Attribute Arrays #2 -  ErrorCondition: {err}'

  # Delete Data # 2
  dcap = simpl.DataContainerArrayProxy()
  dcap.getDataContainerProxy('DataContainer').Flag = 0
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').Flag = 0
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').getDataArrayProxy('V0').Flag = 2
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').getDataArrayProxy('V1').Flag = 2
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').getDataArrayProxy('V2').Flag = 2
  err = d3d.remove_arrays(dca, dcap)
  assert err == 0, f'Remove Arrays #2 -  ErrorCondition: {err}'

  # Create Geometry
  err = sh.CreateGeometry(dca, 0, simpl.IGeometry.Type.Triangle, 'DataContainer', False, 
  shared_vertex_list_array_path = simpl.DataArrayPath('DataContainer', 'Bounds', 'Vertices'),
  shared_tri_list_array_path = simpl.DataArrayPath('DataContainer', 'Triangles', 'Triangles'),
  vertex_attribute_matrix_name = 'VertexData',
  face_attribute_matrix_name = 'FaceData')
  assert err == 0, f'Create Geometry -  ErrorCondition: {err}'

  err = d3d.data_container_writer(dca, sd.GetTestTempDirectory() + '/CreateTriangleGeometry.dream3d', True, False)
  assert err == 0, f'DataContainerWriter ErrorCondition: {err}'

if __name__ == '__main__':
  CreateTriangleGeometryTest()
