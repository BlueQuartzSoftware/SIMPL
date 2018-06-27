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

import dream3d
import dream3d.dream3d_py
import dream3d.dream3d_py as d3d
import dream3d.dream3d_py.simpl_py as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd


def ImageReadTest():
    """

    """
    # Read the Image first so we can get the dimensions
    tif = imageio.imread(sd.GetDataDirectory() + "/Data/Image/slice_11.tif")
    z = np.asarray(tif)
    if not z.flags.contiguous:
        z = np.ascontiguousarray(z)

    shape = z.shape
    assert z.flags.contiguous, 'Only contiguous arrays are supported.'
    assert not np.issubdtype(z.dtype, np.complex128), \
        "Complex numpy arrays cannot be converted to vtk arrays."\
        "Use real() or imag() to get a component of the array before"\
        " passing it to vtk."
    # Get the Pointer to the numpy array
    z_flat = np.ravel(z)

    # Create the DataContainerArray
    dca = sc.CreateDataContainerArray()

    # Create the DataContainer
    dc = sc.CreateDataContainer("ImageDataContainer")
    dca.addDataContainer(dc)

    # Create an AttributeMatrix and add it to the DataContainer
    amShape = simpl.VectorSizeT([shape[1], shape[0], 1])
    cellAm = sc.CreateAttributeMatrix(amShape, "CellAttributeMatrix", simpl.AttributeMatrix.Type.Cell)
    dc.addAttributeMatrix(cellAm.Name, cellAm)

    # Create an AttributeArray from the image data and add it to the AttributeMatrix
    cDims = simpl.VectorSizeT([1])
    name = "slice_11.tiff"
    array = simpl.UInt8ArrayType(z_flat, cDims, name, False)
    cellAm.addAttributeArray(array.Name, array)

    # Create a Geometry Object and store it in the DataContainer
    imageGeom = simpl.ImageGeom.CreateGeometry("ImageGeometry")
    imageGeom.setDimensions(shape[1], shape[0], 1)
    imageGeom.setResolution(1.0, 1.0, 1.0)
    imageGeom.setOrigin(20, 45, 70)
    dc.setGeometry(imageGeom)

    # Write the file out
    err = sc.WriteDREAM3DFile( sd.GetTestTempDirectory() + "/ImageReadTest.dream3d", dca, True)
    assert err == 0


"""
Main entry point for python script
"""
if __name__ == "__main__":
  ImageReadTest()
  print("ImageReadTest Complete")

