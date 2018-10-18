"""

Pipeline example based on 02_Adaptive Alignment - Misorientation - Zero Shifts in Anisotropy examples

"""

from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy
from dream3d import anisotropypy


def import_data():
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

    # Convert Orientation Representation
    err = orientationanalysispy.convert_orientations(dca, simpl.OrientationType.Euler, simpl.OrientationType.Quaternion,
                                                     simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data",
                                                                         "EulerAngles"),
                                                     "Quats")
    if err < 0:
        print("Convert Orientations ErrorCondition %d" % err)

    # Adaptive Alignment Misorientation
    err = anisotropypy.adaptive_alignment_misorientation(dca, 5, False,
                                                         simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data",
                                                                             "Quats"),
                                                         simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data",
                                                                             "Phases"),
                                                         simpl.DataArrayPath("", "", ""),
                                                         simpl.DataArrayPath("AlMgSc Data", "Phase Data",
                                                                             "CrystalStructures"))
    if err < 0:
        print("AdaptiveAlignment Misorientation %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    import_data()


