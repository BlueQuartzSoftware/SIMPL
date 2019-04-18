"""

Pipeline example based on 03_Adaptive Alignment - Mutual Information - SEM Images in Anisotropy examples

"""

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy
from dream3d import anisotropypy
from dream3d import itkimageprocessing
from dream3d import itkimageprocessingpy

# Numpy for some data management assistance
import numpy as np
import datetime as datetime


def adaptive_alignment():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Register the ImageIO Factories
    imageWriter = itkimageprocessing.ITKImageWriter.New()    
    imageWriter.registerImageIOFactories()

    # Read H5EBSD File
    err = orientationanalysispy.read_h5_ebsd(dca, "AlMgSc Data", "Phase Data", "EBSD SEM Scan Data",
                                            sd.GetBuildDirectory() + "/Debug/Data/AlMgSc.h5ebsd",
                                            0, 9, True, sc.AngleRepresentation.Radians,
                                            simpl.StringSet({"Fit", "Image Quality", "EulerAngles",
                                                             "SEM Signal", "Confidence Index", "Phases",
                                                             "X Position", "Y Position"}))
    if err < 0:
        print("ReadH5Ebsd ErrorCondition %d" % err)

    # Import Image Stack [ITK]
    fileListInfo = simpl.FileListInfo(3, 0, 0, 9, 0, sd.GetBuildDirectory() + "/Debug/Data/Anisotropy/tif",
                                      "AlMgSc-TD_", "", "tif")
    err = itkimageprocessingpy.itk_import_image_stack(dca, "SEMAlMgSc Data", "EBSD SEM Scan Data",
                                                      simpl.FloatVec3Type(0, 0, 0), simpl.FloatVec3Type(1, 1, 1),
                                                      "", fileListInfo, 10, "ImageData")
    if err < 0:
        print("ITK Import Image Stack ErrorCondition %d" % err)

    # Convert Orientation Representation
    err = orientationanalysispy.convert_orientations(dca, simpl.OrientationType.Euler, simpl.OrientationType.Quaternion,
                                                    simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "EulerAngles"),
                                                    "Quats")
    if err < 0:
        print("Convert Orientations ErrorCondition %d" % err)

    # Adaptive Alignment Mutual Information
    err = anisotropypy.adaptive_alignment_mutual_information(dca, 5, False,
                                                             simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Quats"),
                                                             simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Phases"),
                                                             simpl.DataArrayPath("", "", ""),
                                                             simpl.DataArrayPath("AlMgSc Data", "Phase Data", "CrystalStructures"))
    if err < 0:
        print("AdaptiveAlignment Mutual Information %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    adaptive_alignment()


