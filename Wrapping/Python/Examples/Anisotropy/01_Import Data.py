"""

Pipeline example based on 01_Import Data in Anisotropy examples

"""

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy

# Numpy for some data management assistance
import numpy as np
import datetime as datetime


def import_data():

    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    err = orientationanalysispy.ebsd_to_h5_ebsd(dca, sd.GetBuildDirectory() + "\Debug\Data\\ang",
                          sd.GetBuildDirectory() + "\Debug\Data\AlMgSc.h5ebsd",
                          "AlMgSc-TD_", "", "ang", 3,
                          0, 9, simpl.AxisAngleInput(0, 0, 0, 0), simpl.AxisAngleInput(0, 0, 0, 0))
    if err < 0:
        print("EBSD to H5EBSD ErrorCondition %d" % err)

"""
Main entry point for python script
"""
if __name__ == "__main__":
    import_data()


