# Based on CreateRectilinearGrid example

import simpl
import simplpy as d3d
import simpl_helpers as sh
import simpl_test_dirs as sd

def CreateRectilinearGridTest():
  # Create Data Container Array
  dca = simpl.DataContainerArray()

  # Create the Data Container
  err = d3d.create_data_container(dca, 'DataContainer')
  assert err == 0, f'DataContainer ErrorCondition: {err}'

  err = d3d.create_attribute_matrix(dca, simpl.DataArrayPath('DataContainer', 'Bounds', ''), simpl.AttributeMatrix.Type.Cell, sh.CreateDynamicTableData([[14]]))
  assert err == 0, f'CreateAttributeMatrix ErrorCondition: {err}'

  err = d3d.create_data_array(dca, simpl.ScalarTypes.Float, 1, simpl.DataArrayPath('DataContainer', 'Bounds', 'x'), 1, '0', (-50.0, 50.0))
  assert err == 0, f'CreateDataArray ErrorCondition: {err}'

  err = d3d.create_data_array(dca, simpl.ScalarTypes.Float, 1, simpl.DataArrayPath('DataContainer', 'Bounds', 'y'), 1, '0', (-50.0, 50.0))
  assert err == 0, f'CreateDataArray ErrorCondition: {err}'

  err = d3d.create_data_array(dca, simpl.ScalarTypes.Float, 1, simpl.DataArrayPath('DataContainer', 'Bounds', 'z'), 1, '0', (-50.0, 50.0))
  assert err == 0, f'CreateDataArray ErrorCondition: {err}'
  
  # Create Geometry
  err = sh.CreateGeometry(dca, 0, simpl.IGeometry.Type.RectGrid, 'DataContainer', False, 
  x_bounds_array_path = simpl.DataArrayPath('DataContainer', 'Bounds', 'x'),
  y_bounds_array_path = simpl.DataArrayPath('DataContainer', 'Bounds', 'y'),
  z_bounds_array_path = simpl.DataArrayPath('DataContainer', 'Bounds', 'z'),
  cell_attribute_matrix_name = 'CellData')
  assert err == 0, f'Create Geometry -  ErrorCondition: {err}'

  err = d3d.data_container_writer(dca, sd.GetTestTempDirectory() + '/CreateRectilinearGrid.dream3d', True, False)
  assert err == 0, f'DataContainerWriter ErrorCondition: {err}'

if __name__ == '__main__':
  CreateRectilinearGridTest()
