# Pipeline : (01) SmallIN100 Morphological Statistics (from EBSD Statistics)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import genericpy as generic
from dream3d import statisticspy as statistics

import datetime as datetime


def small_in100_morph_stats():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Confidence Index").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "EulerAngles").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Fit").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "IPFColor").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Image Quality").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Mask").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Phases").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Quats").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "SEM Signal").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "FeatureIds").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "ParentIds").flag = 2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "CrystalStructures").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "LatticeConstants").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "MaterialName").flag = 2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Active").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AvgEuler").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AvgQuats").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "NumNeighbors2").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "ParentIds").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Phases").flag = 2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("NewGrain Data").getDataArrayProxy(
        "Active").flag = 2

    now_time = datetime.datetime.now()
    now_time_seconds = now_time.hour * 3600 + now_time.minute * 60 + now_time.second
    dt = simpl.DateTime(now_time.year, now_time.month, now_time.day, now_time_seconds)
    err = d3d.data_container_reader(dca,
                                    sd.GetBuildDirectory() +
                                    "/Debug/Data/Output/Reconstruction/SmallIN100_Final.dream3d",
                                    False, "", dt, dcap)
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Find Feature Centroids
    err = generic.find_feature_centroids(dca, simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                         simpl.DataArrayPath("Small IN100", "Grain Data", "Centroids"))
    if err < 0:
        print("FindFeatureCentroids ErrorCondition %d" % err)

    # Find Feature Sizes
    err = statistics.find_sizes(dca, simpl.DataArrayPath("Small IN100", "Grain Data", ""),
                                simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                "Size Volumes", "EquivalentDiameters", "NumElements", False)
    if err < 0:
        print("FindSizes ErrorCondition %d" % err)

    # Find Feature Shapes
    err = statistics.find_shapes(dca, simpl.DataArrayPath("Small IN100", "Grain Data", ""),
                                 simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                 simpl.DataArrayPath("Small IN100", "Grain Data", "Centroids"),
                                 "Omega3s", "Shape Volumes", "AxisLengths", "AxisEulerAngles", "AspectRatios")
    if err < 0:
        print("FindSizes ErrorCondition %d" % err)

    # Find Feature Neighbors
    err = statistics.find_neighbors(dca, simpl.DataArrayPath("Small IN100", "Grain Data", ""),
                                    "SharedSurfaceAreaList", "NeighborList",
                                    simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                    "", "NumNeighbors", "", False, False)
    if err < 0:
        print("FindNeighbors ErrorCondition %d" % err)

    # Find Feature Neighborhoods
    err = statistics.find_neighborhoods(dca, "NeighborhoodList", 1,
                                        simpl.DataArrayPath("Small IN100", "Grain Data", "EquivalentDiameters"),
                                        simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                                        simpl.DataArrayPath("Small IN100", "Grain Data", "Centroids"),
                                        "Neighborhoods")
    if err < 0:
        print("FindNeighborhoods ErrorCondition %d" % err)

    # Find Euclidean Distance Map
    err = statistics.find_euclidean_dist_map(dca,
                                             simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                             "GBManhattanDistances", "TJManhattanDistances", "QPManhattanDistances",
                                             "NearestNeighbors", True, True, True, False, True)
    if err < 0:
        print("FindEuclideanDistanceMap ErrorCondition %d" % err)

    # Find Surface Area to Volume and Sphericity
    err = statistics.find_surface_area_to_volume(dca,
                                                 simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                                 simpl.DataArrayPath("Small IN100", "Grain Data", "NumElements"),
                                                 "SurfaceAreaVolumeRatio", "Sphericity", True)
    if err < 0:
        print("FindSurfaceAreaToVolume ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Debug/Data/Output/Statistics/SmallIN100_Morph.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_morph_stats()
