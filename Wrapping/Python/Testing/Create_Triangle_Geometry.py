# Based on CreateTriangleGeometry example

import simpl
import simplpy as d3d
import simpl_helpers as sc
import simpl_test_dirs as sd

def CreateTriangleGeometryTest():
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
    'dataTypes': ['int64_t', 'int64_t','int64_t']
  }  
  err = d3d.read_ascii_data(dca, wizardData)
  if err < 0:
    print('Import ASCII Data #2 -  ErrorCondition: %d' % err)

  # Combine Attribute Arrays #2:
  selectedDataArrayPaths = [simpl.DataArrayPath('DataContainer', 'Triangles', 'V0'), 
  simpl.DataArrayPath('DataContainer', 'Triangles', 'V1'), simpl.DataArrayPath('DataContainer', 'Triangles', 'V2')]
  err = d3d.combine_attribute_arrays(dca, selectedDataArrayPaths, 'Triangles', False)
  if err < 0:
    print('Combined Attribute Arrays #2 -  ErrorCondition: %d' % err)

  # Delete Data # 2
  dcap = simpl.DataContainerArrayProxy()
  dcap.getDataContainerProxy('DataContainer').Flag = 0
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').Flag = 0
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').getDataArrayProxy('V0').Flag = 2
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').getDataArrayProxy('V1').Flag = 2
  dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').getDataArrayProxy('V2').Flag = 2
  err = d3d.remove_arrays(dca, dcap)
  if err < 0:
    print('Remove Arrays #2 -  ErrorCondition: %d' % err)

  # Create Geometry
  err = sc.CreateGeometry(dca, 0, simpl.IGeometry.Type.Triangle, 'DataContainer', False, 
  shared_vertex_list_array_path = simpl.DataArrayPath('DataContainer', 'Bounds', 'Vertices'),
  shared_tri_list_array_path = simpl.DataArrayPath('DataContainer', 'Triangles', 'Triangles'),
  vertex_attribute_matrix_name = 'VertexData',
  face_attribute_matrix_name = 'FaceData')
  if err < 0:
    print('Create Geometry -  ErrorCondition: %d' % err)

  err = d3d.data_container_writer(dca, sd.GetTestTempDirectory() + '/CreateTriangleGeometry.dream3d', True, False)
  if err < 0:
    print('DataContainerWriter ErrorCondition: %d' % err)

if __name__ == '__main__':
  CreateTriangleGeometryTest()
