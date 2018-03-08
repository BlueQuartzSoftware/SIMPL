
import time

try:
    import numpy as np
except ImportError:
    raise RuntimeError("This module depends on the numpy module. Please make\
sure that it is installed properly.")

# These are the SIMPL python modules
from simpl import *
import simpl_dirs as sd
import simpl_common as sc


def DataArrayTest():
    """
    This is the Top level to test the creation of Data Arrays from numpy and placing those
    arrays into a DREAM3D DataContainer heirarchy.
    """
    dca = sc.CreateDataContainerArray()

    dc = sc.CreateDataContainer("ImageDataContainer")
    dca.addDataContainer(dc)

    shape = simpl.VectorSizeT([4,5,2])
    cellAm = sc.CreateAttributeMatrix(shape, "CellAttributeMatrix", simpl.AttributeMatrix.Type.Cell )
    dc.addAttributeMatrix(cellAm.Name, cellAm)

    # Create the Component Dimensions for the Array, 1 Component in this case
    cDims = simpl.VectorSizeT([1])

    arrayTypes = [np.int8, np.uint8, np.int16, np.uint16, np.int32, np.uint32, np.int64, np.uint64, np.float32, np.double]
    for index, item in enumerate(arrayTypes):
        print("Creating Array: %s" % item)
        array = sc.CreateDataArray(arrayTypes[index].__name__, shape, cDims, item)
        cellAm.addAttributeArray(array.Name, array)

    err = sc.WriteDREAM3DFile(sd.GetTestTempDirectory() + "/DataArrayTest.dream3d", dca)
    assert err == 0



"""
Main entry point for python script
"""
if __name__ == "__main__":
  DataArrayTest()
  print("DataArrayTest Complete")
