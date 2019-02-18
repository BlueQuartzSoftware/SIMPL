# Based on CreateQuadGeometry example
# Tests the K Distance Graph filter

# These are the simpl_py python modules

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import dream3dreviewpy


def k_dist_graph_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Create the Data Container
    err = simplpy.create_data_container(dca, "DataContainer")
    if err < 0:
        print("DataContainer ErrorCondition: %d" % err)

    # Import ASCII Data - #1 - Vertex Coordinates
    import_file = sd.GetBuildDirectory() + "/Debug/Data/DREAM3DReview/VertexCoordinates.csv"
    wizard_data = {
        "inputFilePath": import_file,
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
    err = simplpy.read_ascii_data(dca, wizard_data)
    if err < 0:
        print("Import ASCII Data #1 -  ErrorCondition: %d" % err)

    # Import ASCII Data - #2 - Edge Connectivity
    import_file = sd.GetBuildDirectory() + "/Debug/Data/DREAM3DReview/QuadConnectivity.csv"
    wizard_data = {
        "inputFilePath": import_file,
        "beginIndex": 2,
        "numberOfLines": 122,
        "delimiters": [','],
        "consecutiveDelimiters": False,
        "automaticAM": True,
        "selectedPath": simpl.DataArrayPath("DataContainer", "QuadList", ""),
        "headers": ["V0", "V1", "V2", "V3"],
        "attrMatType": 3,
        "tupleDimensions": [121],
        "dataTypes": ["int64_t", "int64_t", "int64_t", "int64_t"]
    }
    err = simplpy.read_ascii_data(dca, wizard_data)
    if err < 0:
        print("Import ASCII Data #2 -  ErrorCondition: %d" % err)

    # Combine Attribute Arrays # 1:
    selected_data_array_paths = [simpl.DataArrayPath("DataContainer", "Bounds", "x"),
                                 simpl.DataArrayPath("DataContainer", "Bounds", "y"),
                                 simpl.DataArrayPath("DataContainer", "Bounds", "z")]
    err = simplpy.combine_attribute_arrays(dca, selected_data_array_paths, "Vertices", False)
    if err < 0:
        print("Combined Attribute Arrays #1 -  ErrorCondition: %d" % err)

    # Delete Data # 1
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy("DataContainer").flag = 0
    dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("Bounds").flag = 0
    dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("Bounds").getDataArrayProxy("x").flag = 2
    dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("Bounds").getDataArrayProxy("y").flag = 2
    dcap.getDataContainerProxy("DataContainer").getAttributeMatrixProxy("Bounds").getDataArrayProxy("z").flag = 2
    err = simplpy.remove_arrays(dca, dcap)
    if err < 0:
        print("Remove Arrays #1 -  ErrorCondition: %d" % err)

    # Combine Attribute Arrays #2:
    selected_data_array_paths = [simpl.DataArrayPath("DataContainer", "QuadList", "V0"),
                                 simpl.DataArrayPath("DataContainer", "QuadList", "V1"),
                                 simpl.DataArrayPath("DataContainer", "QuadList", "V2"),
                                 simpl.DataArrayPath("DataContainer", "QuadList", "V3")]
    err = simplpy.combine_attribute_arrays(dca, selected_data_array_paths, "Quads", False)
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
    err = simplpy.remove_arrays(dca, dcap)
    if err < 0:
        print("Remove Arrays #2 -  ErrorCondition: %d" % err)

    # Create Geometry
    err = sc.CreateGeometry(dca, 0, simpl.IGeometry.Type.Quad, "DataContainer", False,
                            shared_vertex_list_array_path=simpl.DataArrayPath("DataContainer", "Bounds", "Vertices"),
                            shared_quad_list_array_path=simpl.DataArrayPath("DataContainer", "QuadList", "Quads"),
                            vertex_attribute_matrix_name="VertexData",
                            face_attribute_matrix_name="FaceData")
    if err < 0:
        print("Create Geometry -  ErrorCondition: %d" % err)

    # K Distance Graph
    err = dream3dreviewpy.k_distance_graph(dca, simpl.DataArrayPath("DataContainer", "QuadList",
                                                                    "Quads"),
                                           False, simpl.DataArrayPath("", "", ""),
                                           simpl.DataArrayPath("DataContainer", "QuadList",
                                                               "KDistance"), 7, 3)
    if err < 0:
        print("KDistanceGraph ErrorCondition: %d" % err)

    # Write DREAM3D File
    err = simplpy.data_container_writer(dca, sd.GetBuildDirectory() +
                                        "/Debug/Data/Output/DREAM3DReview/" +
                                        "KDistanceGraph.dream3d", True, False)
    if err < 0:
        print("DataContainerWriter ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    k_dist_graph_test()
