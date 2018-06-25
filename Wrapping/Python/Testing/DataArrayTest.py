
import time

try:
    import numpy as np
except ImportError:
    raise RuntimeError("This module depends on the numpy module. Please make\
sure that it is installed properly.")

# These are the SIMPL python modules

import dream3d
import dream3d.dream3d_py
import dream3d.dream3d_py as d3d
import dream3d.dream3d_py.simpl_py as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd

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
    # We store each return numpy array in order to keep it in scope. If we let it
    # go out of scope then the SIMPL.DataArray will have garbage values since it will
    # be wrapping an invalid pointer.
    arrayList = []
    
    for index, item in enumerate(arrayTypes):
        # print("+++ Creating Array: %s" % item)
        z_flat, array = sc.CreateDataArray(arrayTypes[index].__name__, shape, cDims, item)
        cellAm.addAttributeArray(array.Name, array)
        arrayList.append(z_flat)
        # Now add an array that is purely allocated on the C++/SIMPL side of things.
        # print ("  Creating Int32Array locally to the Loop.... Int32 SIMPL %s" % array.Name)
        array = simpl.Int32ArrayType(shape[0]*shape[1]*shape[2], "Int32 SIMPL " + array.Name, True)
        # cellAm.addAttributeArray(array.Name, array)
        # for x in range(shape[0]*shape[1]*shape[2]):
        #     array.setValue(x, x)
        # This will force cleanup the array that just got created.
        array = simpl.Int32ArrayType.Cleanup()
        # print("--- Loop Complete for %s" % item)

    # Create a Geometry Object and store it in the DataContainer
    imageGeom = simpl.ImageGeom.CreateGeometry("ImageGeometry")
    imageGeom.setDimensions(shape[0], shape[1], shape[2])
    imageGeom.setResolution(1.0, 2.0, 3.0)
    imageGeom.setOrigin(5.5, -9.0, 0.0)
    dc.setGeometry(imageGeom)

    err = sc.WriteDREAM3DFile(sd.GetTestTempDirectory() + "/DataArrayTest.dream3d", dca, True)
    assert err == 0

"""
Main entry point for python script
"""
if __name__ == "__main__":
  DataArrayTest()
  print("[DataArrayTest] Complete")
