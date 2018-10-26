# Based on CreateVertexGeometry example
# Tests the Remove Flagged Vertices filter
# These are the simpl_py python modules

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import dream3dreviewpy


def remove_flagged_vertices_test():
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

    # Create Geometry
    err = sc.CreateGeometry(dca, 0, simpl.IGeometry.Type.Vertex, "DataContainer", False,
                            shared_vertex_list_array_path=simpl.DataArrayPath("DataContainer", "Bounds", "Vertices"),
                            vertex_attribute_matrix_name="VertexData")
    if err < 0:
        print("Create Geometry -  ErrorCondition: %d" % err)

    # Extract Attribute Arrays from Geometry
    empty_data_array_path = simpl.DataArrayPath("", "", "")
    vertex_list_path = simpl.DataArrayPath("DataContainer", "VertexData", "VertexCoordinates")
    err = simplpy.extract_attribute_arrays_from_geometry(dca, "DataContainer", vertex_list_path,
                                                         empty_data_array_path, empty_data_array_path,
                                                         empty_data_array_path, empty_data_array_path,
                                                         empty_data_array_path, empty_data_array_path,
                                                         empty_data_array_path, empty_data_array_path,
                                                         empty_data_array_path, empty_data_array_path,
                                                         empty_data_array_path)
    if err < 0:
        print("ExtractAttributeArraysFromGeometry ErrorCondition %d" % err)

    # Split Multicomponent Attribute Array
    err = simplpy.split_attribute_array(dca,
                                        simpl.DataArrayPath("DataContainer", "VertexData",
                                                            "VertexCoordinates"),
                                        "_Component_")
    if err < 0:
        print("SplitAttributeArray ErrorCondition %d" % err)

    # Threshold Objects
    err = sc.MultiThresholdObjects(dca, "Mask", [("DataContainer", "VertexData",
                                                  "VertexCoordinates_Component_0", ">", -0.5),
                                                 ("DataContainer", "VertexData",
                                                  "VertexCoordinates_Component_1", "<", 0.5)])
    if err < 0:
        print("MultiThresholdObjects ErrorCondition %d" % err)

    # Remove Flagged Vertices
    err = dream3dreviewpy.remove_flagged_vertices(dca, "DataContainer",
                                                  simpl.DataArrayPath("DataContainer", "VertexData",
                                                                      "Mask"),
                                                  "ReducedVertexDataContainer")
    if err < 0:
        print("RemoveFlaggedVertices ErrorCondition %d" % err)

    # Delete Data
    err = sc.RemoveArrays(dca, [["DataContainer", "", ""]])
    if err < 0:
        print("RemoveArrays ErrorCondition %d" % err)

    # Write to DREAM3D File
    err = simplpy.data_container_writer(dca, sd.GetBuildDirectory() +
                                        "/Debug/Data/Output/DREAM3DReview/" +
                                        "RemoveFlaggedVertices.dream3d", True, False)
    if err < 0:
        print("DataContainerWriter ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    remove_flagged_vertices_test()
