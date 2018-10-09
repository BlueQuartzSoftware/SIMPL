
import time

# Based on CreateEdgeGeometry example
# 

# These are the simpl_py python modules

import dream3d
import dream3d.core as d3d
import dream3d.dream3d.simpl as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd

def CreateEdgeGeometryTest():
  # Create Data Container Array
  dca = simpl.DataContainerArray.New()

  # Create the Data Container
  err = d3d.create_data_container(dca, "DataContainer")
  if err < 0:
    print("DataContainer ErrorCondition: %d" % err)

  # Import ASCII Data - #1 - Vertex Coordinates (Using helper function)
  importFile = sd.GetBuildDirectory() + "/Debug/Data/DREAM3DReview/VertexCoordinates.csv"

  err = d3d.read_ascii_data(dca, sc.CreateAsciiWizardData(importFile, 2, 145, [','], False, True, 
  simpl.DataArrayPath("DataContainer", "Bounds", ""), ["x", "y", "z"], 3,
  [144], ["float", "float", "float"]))
  if err < 0:
    print("Import ASCII Data #1 -  ErrorCondition: %d" % err)
  

  # Import ASCII Data - #2 - Edge Connectivity (Without using helper function)
  importFile = sd.GetBuildDirectory() + "/Debug/Data/DREAM3DReview/EdgeConnectivity.csv"
  wizardData = {
    "inputFilePath": importFile,
    "beginIndex": 2,
    "numberOfLines": 265,
    "delimiters": [','],    
    "consecutiveDelimiters": False,
    "automaticAM": True,
    "selectedPath": simpl.DataArrayPath("DataContainer", "EdgeList", ""),
    "headers": ["V1", "V0"],
    "attrMatType": 3,
    "tupleDimensions": [264],
    "dataTypes": ["int64_t", "int64_t"]
  }  
  err = d3d.read_ascii_data(dca, wizardData)
  if err < 0:
    print("Import ASCII Data #2 -  ErrorCondition: %d" % err)

  # Combine Attribute Arrays # 1:
  selectedDataArrayPaths = [simpl.DataArrayPath("DataContainer", "Bounds", "x"), 
  simpl.DataArrayPath("DataContainer", "Bounds", "y"), 
  simpl.DataArrayPath("DataContainer", "Bounds", "z")]
  err = d3d.combine_attribute_arrays(dca, selectedDataArrayPaths, "Vertices", False)
  if err < 0:
    print("Combined Attribute Arrays #1 -  ErrorCondition: %d" % err)

  # Delete Data # 1
  # Remove array helper function:
  err = sc.RemoveArrays(dca, [("DataContainer", "Bounds", "x"), ("DataContainer", "Bounds", "y"), ("DataContainer", "Bounds", "z")])
  if err < 0:
    print("Remove Arrays #1 -  ErrorCondition: %d" % err)

  # Combine Attribute Arrays #2:
  selectedDataArrayPaths = [simpl.DataArrayPath("DataContainer", "EdgeList", "V0"), 
  simpl.DataArrayPath("DataContainer", "EdgeList", "V1")]
  err = d3d.combine_attribute_arrays(dca, selectedDataArrayPaths, "Edges", False)
  if err < 0:
    print("Combined Attribute Arrays #2 -  ErrorCondition: %d" % err)

  # Delete Data # 2
  # Remove array helper function:
  err = sc.RemoveArrays(dca, [("DataContainer", "EdgeList", "V0"), ("DataContainer", "EdgeList", "V1")])
  if err < 0:
    print("Remove Arrays #2 -  ErrorCondition: %d" % err)

  # Create Geometry
  err = sc.CreateGeometry(dca, sc.ArrayHandling.CopyArrays, simpl.IGeometry.Type.Edge, "DataContainer", False, 
  shared_vertex_list_array_path = simpl.DataArrayPath("DataContainer", "Bounds", "Vertices"),
  shared_edge_list_array_path = simpl.DataArrayPath("DataContainer", "EdgeList", "Edges"),
  vertex_attribute_matrix_name = "VertexData",
  edge_attribute_matrix_name = "EdgeData")
  if err < 0:
    print("Create Geometry -  ErrorCondition: %d" % err)

  err = d3d.data_container_writer(dca, sd.GetTestTempDirectory() + "/CreateEdgeGeometry.dream3d", True, False)
  if err < 0:
    print("DataContainerWriter ErrorCondition: %d" % err)

"""
Main entry point for python script
"""
if __name__ == "__main__":
  #help(simpl.ImageGeom)
  CreateEdgeGeometryTest()
