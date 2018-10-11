# Pipeline : (04) SmallIN100 Presegmentation Processing (from EBSD Reconstruction)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientation_analysis

import datetime as datetime


def small_in100_presegmentation_processing():
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

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "CrystalStructures").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "LatticeConstants").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "MaterialName").flag = 2

    now_time = datetime.datetime.now()
    now_time_seconds = now_time.hour * 3600 + now_time.minute * 60 + now_time.second
    dt = simpl.DateTime(now_time.year, now_time.month, now_time.day, now_time_seconds)
    err = d3d.data_container_reader(dca,
                                    sd.GetBuildDirectory() +
                                    "/Debug/Data/Output/Reconstruction/03_SmallIN100_Aligned.dream3d",
                                    False, "", dt, dcap)
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Neighbor Orientation Comparison (Bad Data)
    err = orientation_analysis.bad_data_neighbor_orientation_check(dca, 5, 4,
                                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                       "Mask"),
                                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                       "Phases"),
                                                                   simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                                       "CrystalStructures"),
                                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                       "Quats"))
    if err < 0:
        print("NeighborOrientationComparison ErrorCondition %d" % err)

    # Neighbor Orientation Correlation
    err = orientation_analysis.neighbor_orientation_correlation(dca, 5, 0.2, 2,
                                                                simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                    "Confidence Index"),
                                                                simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                    "Phases"),
                                                                simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                                    "CrystalStructures"),
                                                                simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                    "Quats"))
    if err < 0:
        print("NeighborOrientationCorrelation ErrorCondition %d" % err)

    # Delete Data (Remove array helper function)
    sc.RemoveArrays(dca, [("Small IN100", "EBSD Scan Data", "IPFColor")])

    # Generate IPF Colors
    err = orientation_analysis.generate_ipf_colors(dca, simpl.FloatVec3(0, 0, 1),
                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Phases"),
                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "EulerAngles"),
                                                   simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                       "CrystalStructures"),
                                                   True,
                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Mask"),
                                                   "IPFColor")
    if err < 0:
        print("GenerateIPFColors ErrorCondition: %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(
        sd.GetBuildDirectory() + "/Debug/Data/Output/Reconstruction/04_SmallIN100_Presegmentation.dream3d",
        dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_presegmentation_processing()
