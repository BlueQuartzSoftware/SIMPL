# Pipeline : (06) SmallIN100 Postsegmentation Processing (from EBSD Reconstruction)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientation_analysis
from dream3d import reconstructionpy as reconstruction
from dream3d import processingpy as processing
from dream3d import genericpy as generic
from dream3d import statisticspy as statistics

import datetime as datetime


def small_in100_postseg_processing():
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

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "CrystalStructures").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "LatticeConstants").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "MaterialName").flag = 2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Active").flag = 2

    now_time = datetime.datetime.now()
    now_time_seconds = now_time.hour * 3600 + now_time.minute * 60 + now_time.second
    dt = simpl.DateTime(now_time.year, now_time.month, now_time.day, now_time_seconds)
    err = d3d.data_container_reader(dca,
                                    sd.GetBuildDirectory() +
                                    "/Debug/Data/Output/Reconstruction/05_SmallIN100_Segmentation.dream3d",
                                    False, "", dt, dcap)
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Find Feature Phases
    err = generic.find_feature_phases(dca, simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                      simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Phases"),
                                      simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"))
    if err < 0:
        print("FindFeaturePhases ErrorCondition %d" % err)

    # Find Feature Average Orientations
    err = orientation_analysis.find_avg_orientations(dca,
                                                     simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                                     simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Phases"),
                                                     simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Quats"),
                                                     simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                         "CrystalStructures"),
                                                     simpl.DataArrayPath("Small IN100", "Grain Data", "AvgQuats"),
                                                     simpl.DataArrayPath("Small IN100", "Grain Data", "AvgEuler"))
    if err < 0:
        print("FindAvgOrientations ErrorCondition %d" % err)

    # Find Feature Neighbors #1
    err = statistics.find_neighbors(dca, simpl.DataArrayPath("Small IN100", "Grain Data", ""),
                                    "SharedSurfaceAreaList2", "NeighborList2",
                                    simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                    "", "NumNeighbors2", "", False, False)
    if err < 0:
        print("FindNeighbors #1 ErrorCondition %d" % err)

    # Merge Twins
    err = reconstruction.merge_twins(dca, "NewGrain Data", 3, 2,
                                     simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                     simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                                     simpl.DataArrayPath("Small IN100", "Grain Data", "AvgQuats"),
                                     simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures"),
                                     "ParentIds", "ParentIds", "Active",
                                     simpl.DataArrayPath("Small IN100", "Grain Data", "NeighborList2"),
                                     simpl.DataArrayPath("", "", ""),
                                     False)
    if err < 0:
        print("MergeTwins ErrorCondition %d" % err)

    # Find Feature Sizes
    err = statistics.find_sizes(dca, simpl.DataArrayPath("Small IN100", "Grain Data", ""),
                                simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                "Volumes", "EquivalentDiameters", "NumElements", False)
    if err < 0:
        print("FindSizes ErrorCondition %d" % err)

    # Minimum Size
    err = processing.min_size(dca, 16, False, 0,
                              simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                              simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                              simpl.DataArrayPath("Small IN100", "Grain Data", "NumElements"))
    if err < 0:
        print("MinSize ErrorCondition %d" % err)

    # Find Feature Neighbors #12
    err = statistics.find_neighbors(dca, simpl.DataArrayPath("Small IN100", "Grain Data", ""),
                                    "SharedSurfaceAreaList", "NeighborList",
                                    simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                    "", "NumNeighbors", "", False, False)
    if err < 0:
        print("FindNeighbors #2 ErrorCondition %d" % err)

    # Minimum Number of Neighbors
    err = processing.min_neighbors(dca, 2, False, 0, simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                   simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                                   simpl.DataArrayPath("Small IN100", "Grain Data", "NumNeighbors"))
    if err < 0:
        print("MinNeighbors ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(
        sd.GetBuildDirectory() + "/Debug/Data/Output/Reconstruction/06_SmallIN100_Postsegmentation.dream3d",
        dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_postseg_processing()
