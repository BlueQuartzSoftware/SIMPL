import imageio
import numpy as np

import simpl
import simplpy
import simpl_helpers as sh
import simpl_test_dirs as sd

def ImageReadTest():
    # Read the Image first so we can get the dimensions
    tif = imageio.imread(sd.GetDataDirectory() + '/Data/Image/slice_11.tif')
    z = np.asarray(tif)
    if not z.flags.contiguous:
        z = np.ascontiguousarray(z)

    shape = z.shape
    assert z.flags.contiguous, 'Only contiguous arrays are supported.'
    assert not np.issubdtype(z.dtype, np.complex128), \
        'Complex numpy arrays cannot be converted to vtk arrays.'\
        'Use real() or imag() to get a component of the array before'\
        ' passing it to vtk.'
    # Get the Pointer to the numpy array
    z_flat = np.ravel(z)

    # Create the DataContainerArray
    dca = sh.CreateDataContainerArray()

    # Create the DataContainer
    dc = sh.CreateDataContainer('ImageDataContainer')
    dca.addOrReplaceDataContainer(dc)

    # Create an AttributeMatrix and add it to the DataContainer
    amShape = simpl.VectorSizeT([shape[1], shape[0], 1])
    cellAm = sh.CreateAttributeMatrix(amShape, 'CellAttributeMatrix', simpl.AttributeMatrix.Type.Cell)
    dc.addOrReplaceAttributeMatrix(cellAm)

    # Create an AttributeArray from the image data and add it to the AttributeMatrix
    name = 'slice_11.tiff'
    array = simpl.UInt8Array(z_flat, name)
    cellAm.addOrReplaceAttributeArray(array)

    # Create a Geometry Object and store it in the DataContainer
    imageGeom = simpl.ImageGeom.CreateGeometry('ImageGeometry')
    imageGeom.setDimensions(shape[1], shape[0], 1)
    imageGeom.setSpacing(1.0, 1.0, 1.0)
    imageGeom.setOrigin(20, 45, 70)
    dc.setGeometry(imageGeom)

    # Write the file out
    err = sh.WriteDREAM3DFile( sd.GetBuildDirectory() + '/Data/Output/PythonTest/ImageReadTest.dream3d', dca, True)
    assert err == 0

if __name__ == '__main__':
  ImageReadTest()
  print('ImageReadTest Complete')
