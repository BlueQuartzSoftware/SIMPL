
import time

from SIMPLibPy import *
try:
    import numpy as np
except ImportError:
    raise RuntimeError("This module depends on the numpy module. Please make\
sure that it is installed properly.")

def CreateDataContainerArray():
    """
    Creates a Top level Data Container Array object. Everything will get packed in this
    """
    dca = SIMPLibPy.DataContainerArray.New()
    return dca

def CreateDataContainer(name):
    """
    Creates a DataContainer object
    """
    dc = SIMPLibPy.DataContainer.New(name)
    return dc


def CreateAttributeMatrix(name, type, dims):
    """
    Creates an AttriuteMatrix of the given type
    """
    am = SIMPLibPy.AttributeMatrix.Create(dims, name, type)
    return am


def WriteDREAM3DFile(path, dca):
    # Lets write out the whole Heirarchy to a .dream3d file
    print("Creating DREAM3D Writer filter....")
    writer = SIMPLibPy.DataContainerWriter.New()
    writer.OutputFile = (path)
    writer.setDataContainerArray(dca)
    writer.execute()
    if(writer.ErrorCondition == 0):
        print("DREAM.3D File written to %s" % path)
    return writer.ErrorCondition


def CreateDataArray(name, shape, cDims, type):
    """
    Creates a data array with the attributes listed
    """
    # Create a numpy array of ones to hold our data
    num_array = np.ndarray(shape, dtype=type, order="C")

    z = np.asarray(num_array)
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
    # Declare the number of components for the array
    if type == np.int8:
        array = SIMPLibPy.Int8ArrayType(z_flat, cDims, name, False)
    elif type == np.uint8:
        array = SIMPLibPy.UInt8ArrayType(z_flat, cDims, name, False)
    elif type == np.int16:
        array = SIMPLibPy.Int16ArrayType(z_flat, cDims, name, False)
    elif type == np.uint16:
        array = SIMPLibPy.UInt16ArrayType(z_flat, cDims, name, False)
    elif type == np.int32:
        array = SIMPLibPy.Int32ArrayType(z_flat, cDims, name, False)
    elif type == np.uint32:
        array = SIMPLibPy.UInt32ArrayType(z_flat, cDims, name, False)
    elif type == np.int64:
        array = SIMPLibPy.Int64ArrayType(z_flat, cDims, name, False)
    elif type == np.uint64:
        array = SIMPLibPy.UInt64ArrayType(z_flat, cDims, name, False)
    elif type == np.float32:
        array = SIMPLibPy.FloatArrayType(z_flat, cDims, name, False)
    elif type == np.double:
        array = SIMPLibPy.DoubleArrayType(z_flat, cDims, name, False)        
    return array

def DataArrayTest():
    """
    This is the Top level to test the creation of Data Arrays from numpy and placing those
    arrays into a DREAM3D DataContainer heirarchy.
    """
    dca = CreateDataContainerArray()

    dc = CreateDataContainer("ImageDataContainer")
    dca.addDataContainer(dc)

    shape = SIMPLibPy.VectorSizeT([4,5,2])
    cellAm = CreateAttributeMatrix("CellAttributeMatrix", SIMPLibPy.AttributeMatrix.Type.Cell, shape)
    dc.addAttributeMatrix(cellAm.Name, cellAm)

    # Create the Component Dimensions for the Array, 1 Component in this case
    cDims = SIMPLibPy.VectorSizeT([1])

    arrayTypes = [np.int8, np.uint8, np.int16, np.uint16, np.int32, np.uint32, np.int64, np.uint64, np.float32, np.double]
    for index, item in enumerate(arrayTypes):
        print("Creating Array: %s" % item)
        array = CreateDataArray(arrayTypes[index].__name__, shape, cDims, item)
        cellAm.addAttributeArray(array.Name, array)

    err = WriteDREAM3DFile("/tmp/DataArrayTest.dream3d", dca)
    assert err == 0



def Misc():
    """

    """



"""
Main entry point for python script
"""
if __name__ == "__main__":
  DataArrayTest()
  print("DataArrayTest Complete")
