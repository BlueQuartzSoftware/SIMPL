
# Based on CreateRectilinearGrid example

# As of 9-13-18, this example will not work as the input file is not available

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

  # Import ASCII Data - Rectilinear Grid
  importFile = sd.GetBuildDirectory() + '/Data/SIMPL/RectilinearGrid.csv'
  wizardData = {
    'inputFilePath': importFile,
    'beginIndex': 2,
    'numberOfLines': 15,
    'delimiters': [','],    
    'consecutiveDelimiters': False,
    'automaticAM': True,
    'selectedPath': simpl.DataArrayPath('DataContainer', 'Bounds', ''),
    'headers': ['x', 'y', 'z'],
    'attrMatType': 3,
    'tupleDimensions': [14],
    'dataTypes': ['float', 'float', 'float']
  }
  err = d3d.read_ascii_data(dca, wizardData)
  assert err == 0, f'Import ASCII Data #1 -  ErrorCondition: {err}'
  
  # Create Geometry
  err = sh.CreateGeometry(dca, 0, simpl.IGeometry.Type.Triangle, 'DataContainer', False, 
  x_bounds_array_path = simpl.DataArrayPath('DataContainer', 'Bounds', 'x'),
  y_bounds_array_path = simpl.DataArrayPath('DataContainer', 'Bounds', 'y'),
  z_bounds_array_path = simpl.DataArrayPath('DataContainer', 'Bounds', 'z'),
  face_attribute_matrix_name = 'CellData')
  assert err == 0, f'Create Geometry -  ErrorCondition: {err}'

  err = d3d.data_container_writer(dca, sd.GetTestTempDirectory() + '/CreateRectilinearGrid.dream3d', True, False)
  assert err == 0, f'DataContainerWriter ErrorCondition: {err}'

if __name__ == '__main__':
  CreateRectilinearGridTest()
