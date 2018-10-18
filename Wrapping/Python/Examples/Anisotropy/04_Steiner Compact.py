"""

Pipeline example based on 04_Steiner Compact in Anisotropy examples

"""

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy
from dream3d import anisotropypy
from dream3d import reconstructionpy
from dream3d import processingpy

# Numpy for some data management assistance
import numpy as np
import datetime as datetime


def adaptive_alignment():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read H5EBSD File
    err = orientationanalysispy.read_h5_ebsd(dca, "AlMgSc Data", "Phase Data", "EBSD SEM Scan Data",
                                            sd.GetBuildDirectory() + "/Debug/Data/AlMgSc.h5ebsd",
                                            0, 9, True, sc.AngleRepresentation.Radians,
                                            simpl.StringSet({"Fit", "Image Quality", "EulerAngles",
                                                             "SEM Signal", "Confidence Index", "Phases",
                                                             "X Position", "Y Position"}))
    if err < 0:
        print("ReadH5Ebsd ErrorCondition %d" % err)

    # MultiThreshold Objects
    err = sc.MultiThresholdObjects(dca, "Mask", [("AlMgSc Data", "EBSD SEM Scan Data", "Confidence Index", ">", 0.05),
                                                 ("AlMgSc Data", "EBSD SEM Scan Data", "Image Quality", ">", 800)])
    if err < 0:
        print("MultiThresholdObjects ErrorCondition: %d" % err)

    # Convert Orientation Representation
    err = orientationanalysispy.convert_orientations(dca, simpl.OrientationType.Euler, simpl.OrientationType.Quaternion,
                                                    simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "EulerAngles"),
                                                    "Quats")
    if err < 0:
        print("Convert Orientations ErrorCondition %d" % err)

    # Neighbor Orientation Comparison (Bad Data)
    err = orientationanalysispy.bad_data_neighbor_orientation_check(dca, 2, 4,
                                              simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Mask"),
                                              simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Phases"),
                                              simpl.DataArrayPath("AlMgSc Data", "Phase Data", "CrystalStructures"),
                                              simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Quats"))
    if err < 0:
        print("BadDataNeighborOrientationCheck ErrorCondition %d" % err)

    # Neighbor Orientation Correlation
    err = orientationanalysispy.neighbor_orientation_correlation(dca, 2, 0.05, 2,
                                           simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Confidence Index"),
                                           simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Phases"),
                                           simpl.DataArrayPath("AlMgSc Data", "Phase Data", "CrystalStructures"),
                                           simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Quats"))
    if err < 0:
        print("NeighborOrientationCorrelation ErrorCondition %d" % err)

    # EBSD Segment Features (Misorientation)
    err = reconstructionpy.ebsd_segment_features(dca, "CellFeatureData", 2,
                                                 True,
                                                 simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Mask"),
                                                 simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Phases"),
                                                 simpl.DataArrayPath("AlMgSc Data", "Phase Data", "CrystalStructures"),
                                                 simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Quats"),
                                                 "FeatureIds",
                                                 "Active")
    if err < 0:
        print("EbsdSegmentFeatures ErrorCondition %d" % err)

    # Erode Bad Data
    err = processingpy.erode_dilate_bad_data(dca, sc.BadDataOperation.Erode, 3, True, True, True,
                                           simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "FeatureIds"))
    if err < 0:
        print("ErodeDilateBadData ErrorCondition %d" % err)

    # Steiner Compact

    # The following 'enums' need binded
    # Section Plane options --> [0 : XY] , [1 : XZ], [2 : YZ]
    # Number of Sites options --> [0 : 8] , [1 : 12], [2 : 16], [3 : 24], [4 : 36]
    err = anisotropypy.steiner_compact(dca, True, sd.GetBuildDirectory() + "/Debug/SteinerCompactXY.vtk",
                          True, sd.GetBuildDirectory() + "/Debug/SteinerCompactXY.txt",
                          simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "FeatureIds"),
                          simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Phases"),
                          0, 1)
    if err < 0:
        print("SteinerCompact ErrorCondition %d" % err)

"""
Main entry point for python script
"""
if __name__ == "__main__":
    adaptive_alignment()


