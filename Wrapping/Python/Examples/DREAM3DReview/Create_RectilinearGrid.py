
# Based on CreateRectilinearGrid example
# 

# As of 9-13-18, this example will not work as the input file is not available

# These are the simpl_py python modules

import dream3d
import dream3d.core as d3d
import dream3d.dream3d.simpl as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd

def CreateRectilinearGridTest():
  # Create Data Container Array
  dca = simpl.DataContainerArray.New()

  # Create the Data Container
  err = d3d.create_data_container(dca, "DataContainer")
  if err < 0:
    print("DataContainer ErrorCondition: %d" % err)

  # Import ASCII Data - Rectilinear Grid
  importFile = sd.GetBuildDirectory() + "/Debug/Data/DREAM3DReview/RectilinearGrid.csv"
  wizardData = {
    "inputFilePath": importFile,
    "beginIndex": 2,
    "numberOfLines": 15,
    "delimiters": [','],    
    "consecutiveDelimiters": False,
    "automaticAM": True,
    "selectedPath": simpl.DataArrayPath("DataContainer", "Bounds", ""),
    "headers": ["x", "y", "z"],
    "attrMatType": 3,
    "tupleDimensions": [14],
    "dataTypes": ["float", "float", "float"]
  }
  err = d3d.read_ascii_data(dca, wizardData)
  if err < 0:
    print("Import ASCII Data #1 -  ErrorCondition: %d" % err)
  
  # Create Geometry
  err = sc.CreateGeometry(dca, 0, simpl.IGeometry.Type.Triangle, "DataContainer", False, 
  x_bounds_array_path = simpl.DataArrayPath("DataContainer", "Bounds", "x"),
  y_bounds_array_path = simpl.DataArrayPath("DataContainer", "Bounds", "y"),
  z_bounds_array_path = simpl.DataArrayPath("DataContainer", "Bounds", "z"),
  face_attribute_matrix_name = "CellData")
  if err < 0:
    print("Create Geometry -  ErrorCondition: %d" % err)

  err = d3d.data_container_writer(dca, sd.GetTestTempDirectory() + "/CreateRectilinearGrid.dream3d", True, False)
  if err < 0:
    print("DataContainerWriter ErrorCondition: %d" % err)

"""
Main entry point for python script
"""
if __name__ == "__main__":
  CreateRectilinearGridTest()
