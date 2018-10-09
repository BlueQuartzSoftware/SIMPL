# Based on CreateQuadGeometry example
# 

# These are the simpl_py python modules
import dream3d
import dream3d.core as d3d
import dream3d.dream3d.simpl as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd

def CreateQuadGeometryTest():
  # Create Data Container Array
  dca = simpl.DataContainerArray.New()

  # Create the Data Container
  err = d3d.create_data_container(dca, "DataContainer")
  if err < 0:
    print("DataContainer ErrorCondition: %d" % err)

  # Import ASCII Data - #1 - Vertex Coordinates
  importFile = sd.GetBuildDirectory() + "/Debug/Data/DREAM3DReview/VertexCoordinates.csv"
  wizardData = {
    "inputFilePath": importFile,
    "beginIndex": 2,
    "numberOfLines": 145,
    "delimiters": [','],    
    "consecutiveDelimiters": False,
    "automaticAM": True,
    "selectedPath": simpl.DataArrayPath("DataContainer", "Bounds", ""),
    "headers": ["x", "y", "z"],
    "attrMatType": 3,
    "tupleDimensions": [144],
    "dataTypes": ["float", "float", "float"]
  }
  err = d3d.read_ascii_data(dca, wizardData)
  if err < 0:
    print("Import ASCII Data #1 -  ErrorCondition: %d" % err)
  

  # Import ASCII Data - #2 - Edge Connectivity
  importFile = sd.GetBuildDirectory() + "/Debug/Data/DREAM3DReview/QuadConnectivity.csv"
  wizardData = {
    "inputFilePath": importFile,
    "beginIndex": 2,
    "numberOfLines": 122,
    "delimiters": [','],    
    "consecutiveDelimiters": False,
    "automaticAM": True,
    "selectedPath": simpl.DataArrayPath("DataContainer", "QuadList", ""),
    "headers": ["V0", "V1", "V2", "V3"],
    "attrMatType": 3,
    "tupleDimensions": [121],
    "dataTypes": ["int64_t", "int64_t","int64_t", "int64_t"]
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
  dcap = simpl.DataContainerArrayProxy()
  dcap.getDataContainerProxy("DataContainer").flag = 0
  dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("Bounds").flag = 0
  dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("Bounds").getDataArrayProxy("x").flag = 2
  dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("Bounds").getDataArrayProxy("y").flag = 2
  dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("Bounds").getDataArrayProxy("z").flag = 2
  err = d3d.remove_arrays(dca, dcap)
  if err < 0:
    print("Remove Arrays #1 -  ErrorCondition: %d" % err)

  # Combine Attribute Arrays #2:
  selectedDataArrayPaths = [simpl.DataArrayPath("DataContainer", "QuadList", "V0"), 
  simpl.DataArrayPath("DataContainer", "QuadList", "V1"), simpl.DataArrayPath("DataContainer", "QuadList", "V2"), 
  simpl.DataArrayPath("DataContainer", "QuadList", "V3")]
  err = d3d.combine_attribute_arrays(dca, selectedDataArrayPaths, "Quads", False)
  if err < 0:
    print("Combined Attribute Arrays #2 -  ErrorCondition: %d" % err)

  # Delete Data # 2
  dcap = simpl.DataContainerArrayProxy()
  dcap.getDataContainerProxy("DataContainer").flag = 0
  dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("QuadList").flag = 0
  dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("QuadList").getDataArrayProxy("V0").flag = 2
  dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("QuadList").getDataArrayProxy("V1").flag = 2
  dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("QuadList").getDataArrayProxy("V2").flag = 2
  dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("QuadList").getDataArrayProxy("V3").flag = 2
  err = d3d.remove_arrays(dca, dcap)
  if err < 0:
    print("Remove Arrays #2 -  ErrorCondition: %d" % err)

  # Create Geometry
  err = sc.CreateGeometry(dca, 0, simpl.IGeometry.Type.Quad, "DataContainer", False, 
  shared_vertex_list_array_path = simpl.DataArrayPath("DataContainer", "Bounds", "Vertices"),
  shared_quad_list_array_path = simpl.DataArrayPath("DataContainer", "QuadList", "Quads"),
  vertex_attribute_matrix_name = "VertexData",
  face_attribute_matrix_name = "FaceData")
  if err < 0:
    print("Create Geometry -  ErrorCondition: %d" % err)

  err = d3d.data_container_writer(dca, sd.GetTestTempDirectory() + "/CreateQuadGeometry.dream3d", True, False)
  if err < 0:
    print("DataContainerWriter ErrorCondition: %d" % err)

"""
Main entry point for python script
"""
if __name__ == "__main__":
  CreateQuadGeometryTest()
