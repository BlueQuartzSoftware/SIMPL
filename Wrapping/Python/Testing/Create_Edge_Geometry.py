# Based on CreateEdgeGeometry example

import simpl
import simplpy as d3d
import simpl_helpers as sh
import simpl_test_dirs as sd

def CreateEdgeGeometryTest():
  # Create Data Container Array
  dca = simpl.DataContainerArray()

  # Create the Data Container
  err = d3d.create_data_container(dca, 'DataContainer')
  assert err == 0, f'DataContainer ErrorCondition: {err}'

  # Import ASCII Data - #1 - Vertex Coordinates (Using helper function)
  importFile = sd.GetBuildDirectory() + '/Data/SIMPL/VertexCoordinates.csv'

  err = d3d.read_ascii_data(dca, sh.CreateAsciiWizardData(importFile, 2, 145, [','], False, True, 
  simpl.DataArrayPath('DataContainer', 'Bounds', ''), ['x', 'y', 'z'], 3,
  [144], ['float', 'float', 'float']))
  assert err == 0, f'Import ASCII Data #1 -  ErrorCondition: {err}'
  
  # Import ASCII Data - #2 - Edge Connectivity (Without using helper function)
  importFile = sd.GetBuildDirectory() + '/Data/SIMPL/EdgeConnectivity.csv'
  wizardData = {
    'inputFilePath': importFile,
    'beginIndex': 2,
    'numberOfLines': 265,
    'delimiters': [','],    
    'consecutiveDelimiters': False,
    'automaticAM': True,
    'selectedPath': simpl.DataArrayPath('DataContainer', 'EdgeList', ''),
    'headers': ['V1', 'V0'],
    'attrMatType': 3,
    'tupleDimensions': [264],
    'dataTypes': ['uint64_t', 'uint64_t']
  }  
  err = d3d.read_ascii_data(dca, wizardData)
  assert err == 0, f'Import ASCII Data #2 -  ErrorCondition: {err}'

  # Combine Attribute Arrays # 1:
  selectedDataArrayPaths = [simpl.DataArrayPath('DataContainer', 'Bounds', 'x'), 
  simpl.DataArrayPath('DataContainer', 'Bounds', 'y'), 
  simpl.DataArrayPath('DataContainer', 'Bounds', 'z')]
  err = d3d.combine_attribute_arrays(dca, selectedDataArrayPaths, 'Vertices', False)
  assert err == 0, f'Combined Attribute Arrays #1 -  ErrorCondition: {err}'

  # Delete Data # 1
  # Remove array helper function:
  err = sh.RemoveArrays(dca, [('DataContainer', 'Bounds', 'x'), ('DataContainer', 'Bounds', 'y'), ('DataContainer', 'Bounds', 'z')])
  assert err, f'Remove Arrays #1 -  ErrorCondition: {err}'

  # Combine Attribute Arrays #2:
  selectedDataArrayPaths = [simpl.DataArrayPath('DataContainer', 'EdgeList', 'V0'), 
  simpl.DataArrayPath('DataContainer', 'EdgeList', 'V1')]
  err = d3d.combine_attribute_arrays(dca, selectedDataArrayPaths, 'Edges', False)
  assert err == 0, f'Combined Attribute Arrays #2 -  ErrorCondition: {err}'

  # Delete Data # 2
  # Remove array helper function:
  err = sh.RemoveArrays(dca, [('DataContainer', 'EdgeList', 'V0'), ('DataContainer', 'EdgeList', 'V1')])
  assert err, f'Remove Arrays #2 -  ErrorCondition: {err}'

  # Create Geometry
  err = sh.CreateGeometry(dca, sh.ArrayHandling.CopyArrays, simpl.IGeometry.Type.Edge, 'DataContainer', False, 
  shared_vertex_list_array_path = simpl.DataArrayPath('DataContainer', 'Bounds', 'Vertices'),
  shared_edge_list_array_path = simpl.DataArrayPath('DataContainer', 'EdgeList', 'Edges'),
  vertex_attribute_matrix_name = 'VertexData',
  edge_attribute_matrix_name = 'EdgeData')
  assert err == 0, f'Create Geometry -  ErrorCondition: {err}'

  err = d3d.data_container_writer(dca, sd.GetTestTempDirectory() + '/CreateEdgeGeometry.dream3d', True, False)
  assert err == 0, f'DataContainerWriter ErrorCondition: {err}'

if __name__ == '__main__':
  CreateEdgeGeometryTest()
