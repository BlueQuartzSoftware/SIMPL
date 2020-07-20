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

  # Read vertices
  err = d3d.create_attribute_matrix(dca, simpl.DataArrayPath('DataContainer', 'Bounds', ''), simpl.AttributeMatrix.Type.Cell, sh.CreateDynamicTableData([[144]]))
  assert err == 0, f'CreateAttributeMatrix - Error: {err}'

  err = d3d.import_asci_data_array(dca, simpl.DataArrayPath('DataContainer', 'Bounds', 'Vertices'), simpl.NumericTypes.Float, 3, 1, sd.GetBuildDirectory() + '/Data/SIMPL/VertexCoordinates.csv', 0)
  assert err == 0, f'ImportAsciDataArray - Error {err}'

  # Read edges
  err = d3d.create_attribute_matrix(dca, simpl.DataArrayPath('DataContainer', 'EdgeList', ''), simpl.AttributeMatrix.Type.Cell, sh.CreateDynamicTableData([[264]]))
  assert err == 0, f'CreateAttributeMatrix - Error: {err}'

  err = d3d.import_asci_data_array(dca, simpl.DataArrayPath('DataContainer', 'EdgeList', 'Edges'), simpl.NumericTypes.SizeT, 2, 1, sd.GetBuildDirectory() + '/Data/SIMPL/EdgeConnectivity.csv', 0)
  assert err == 0, f'ImportAsciDataArray - Error {err}'

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
