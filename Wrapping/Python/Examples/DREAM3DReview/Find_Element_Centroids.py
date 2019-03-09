# Based on CreateLambertSphereSurface pipeline example
# Tests the Find Element Centroids filter
# These are the simpl_py python modules

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy
from dream3d import dream3dreviewpy


def find_element_centroids():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Create Data Container
    err = simplpy.create_data_container(dca, "ImageDataContainer")
    if err < 0:
        print("DataContainer ErrorCondition: %d" % err)

    # Create Image Geometry
    err = simplpy.create_image_geometry(dca, "ImageDataContainer", simpl.IntVec3(101, 101, 1), simpl.FloatVec3(0, 0, 0),
                                        simpl.FloatVec3(1, 1, 1))
    if err < 0:
        print("ImageGeometry ErrorCondition: %d" % err)

    # Create Attribute Matrix
    new_row = simpl.VectorDouble()
    new_row.append(101)
    new_row.append(101)
    new_row.append(1)
    table_data = list()
    table_data.append(new_row)
    err = simplpy.create_attribute_matrix(dca, simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", ""), 3,
                                          simpl.DynamicTableData(table_data, ["0", "1", "2"], ["0"]))
    if err < 0:
        print("AttributeMatrix ErrorCondition: %d" % err)

    # Create Data Array
    err = simplpy.create_data_array(dca, simpl.ScalarTypes.UInt8, 1,
                                    simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", "ScalarValues"),
                                    simpl.InitializationType.Manual, "128", (0.0, 151.1))
    if err < 0:
        print("DataArray ErrorCondition: %d" % err)

    # Create Lambert Sphere
    err = orientationanalysispy.create_lambert_sphere(dca, sc.Hemisphere.Northern,
                                                      simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix",
                                                                          "ScalarValues"),
                                                      "QuadDataContainer", "TriangleDataContainer", "EdgeDataContainer",
                                                      "VertexDataContainer", "VertexAttributeMatrix",
                                                      "EdgeAttributeMatrix", "FaceAttributeMatrix",
                                                      True, True, True, True)
    if err < 0:
        print("LambertSphere ErrorCondition: %d" % err)

    # Find Element Centroids
    err = dream3dreviewpy.find_element_centroids(dca,
                                                 simpl.DataArrayPath("EdgeDataContainer",
                                                                     "EdgeAttributeMatrix", "Centroids"),
                                                 False, "", "")
    if err < 0:
        print("FindElementCentroids ErrorCondition: %d" % err)

    # Write DREAM3D File
    err = simplpy.data_container_writer(dca, sd.GetBuildDirectory() +
                                        "/Debug/Data/Output/DREAM3DReview/" +
                                        "FindElementCentroid.dream3d",
                                        True, False)
    if err < 0:
        print("DataContainerWriter ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    find_element_centroids()
