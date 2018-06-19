"""
For the standard Python 3 installation from python.org I had to pip3 install scipy imageio
in order to use the below modules.
"""
import time
import imageio

try:
    import numpy as np
except ImportError:
    raise RuntimeError("This module depends on the numpy module. Please make\
sure that it is installed properly.")

# These are the simpl_py python modules
from simpl_py import *
import simpl.simpl_common as sc
import simpl.simpl_test_dirs as sd


def GeometryTest():
  imageGeom = simpl_py.ImageGeom.CreateGeometry("ImageGeometry")
  assert imageGeom.Name == "ImageGeometry"

  imageGeom.setDimensions(100, 1001, 200)
  dims = imageGeom.getDimensions()
  assert dims[0] == 100
  assert dims[1] == 1001
  assert dims[2] == 200

  imageGeom.setResolution(1.0, 2.0, 3.0)
  imageRes = imageGeom.getResolution()
  assert imageRes[0] == 1.0
  assert imageRes[1] == 2.0
  assert imageRes[2] == 3.0

  imageGeom.setOrigin(20, 45, 70)
  origin = imageGeom.getOrigin()
  assert origin[0] == 20
  assert origin[1] == 45
  assert origin[2] == 70
  
  infoString = imageGeom.getInfoString(simpl_py.HtmlFormat)

  boundingBox = imageGeom.getBoundingBox()
  assert boundingBox[0] == 20
  assert boundingBox[1] == 120
  assert boundingBox[2] == 45
  assert boundingBox[3] == 2047
  assert boundingBox[4] == 70
  assert boundingBox[5] == 670

"""
Main entry point for python script
"""
if __name__ == "__main__":
  GeometryTest()
  print("GeometryTest Complete")




