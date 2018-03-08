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

# These are the SIMPL python modules
from simpl import *
import simpl_dirs as sd
import simpl_common as sc


def GeometryTest():
  imageGeom = simpl.ImageGeom.CreateGeometry("ImageGeometry")
  assert imageGeom.Name == "ImageGeometry"
  imageGeom.Name = "Foo"
  imageRes = imageGeom.getRes()
  print("ImageRes: ", imageRes)
  imageGeom.getDimensions(xDim, yDim, zDim)
  print("ImageGeom: ", imageGeom)
  print("Dims: " ,xDim, yDim, zDim, sep=" ")
  infoString = imageGeom.getInfoString(simpl.HtmlFormat)
  print(infoString)

"""
Main entry point for python script
"""
if __name__ == "__main__":
  GeometryTest()
  print("GeometryTest Complete")




