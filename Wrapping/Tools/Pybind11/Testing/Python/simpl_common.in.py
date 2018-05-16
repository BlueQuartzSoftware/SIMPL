
#import _simpl as simpl_py
from @SIMPL_PYTHON_MODULE_NAME@@SIMPL_PYTHON_MODULE_SUFFIX@ import *

try:
    import numpy as np
except ImportError:
    raise RuntimeError("This module depends on the numpy module. Please make\
sure that it is installed properly.")


def CreateDataContainerArray():
    """
    Creates a Top level Data Container Array object. Everything will get packed in this
    """
    dca = simpl_py.DataContainerArray.New()
    return dca

def CreateDataContainer(name):
    """
    Creates a DataContainer object

    Keyword arguments:
    name -- The name of the DataContainer    
    """
    dc = simpl_py.DataContainer.New(name)
    return dc


def CreateAttributeMatrix(dims, name, type):
    """
    Creates an AttriuteMatrix of the given type

    Keyword arguments:
    dims -- the dimensions of the AttributeMatrix
    name -- The name of the AttributeMatrix
    type -- the type of the AttributeMatrix
    """
    am = simpl_py.AttributeMatrix.Create(dims, name, type)
    return am


def WriteDREAM3DFile(path, dca, verbose=False):
    """
    Writes a dream3d file and returns the error code

    Keyword arguments:
    path -- The output path of the file.
    dca -- The DataContainerArray to write
    """
    # Lets write out the whole Heirarchy to a .dream3d file
    if verbose:
        print("Creating DREAM3D Writer filter....")
    writer = simpl_py.DataContainerWriter.New()
    writer.OutputFile = (path)
    if verbose:
        print("Writing to file:", path)
    writer.setDataContainerArray(dca)
    writer.execute()
    if(writer.ErrorCondition != 0):
        print("Error %d writing DREAM.3D File to path %s" % writer.ErrorCondition, path)
    return writer.ErrorCondition


def CreateDataArray(name, shape, cDims, type):
    """
    Creates a data array with the attributes listed. The array will be a numpy array in "C" ordering.

    Keyword arguments:
    name -- The name of the DataArray
    shape -- The dimensions of the DataArray. Note that the array will be flattened into a contiguous array
    cDims -- The component dimensions, i.e., the number of components at each tuple.
    type -- The numpy type of array to create: 8,16,32,64 signed/unsiged and 32/64 floats are supported
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
        array = simpl_py.Int8ArrayType(z_flat, cDims, name, False)
    elif type == np.uint8:
        array = simpl_py.UInt8ArrayType(z_flat, cDims, name, False)
    elif type == np.int16:
        array = simpl_py.Int16ArrayType(z_flat, cDims, name, False)
    elif type == np.uint16:
        array = simpl_py.UInt16ArrayType(z_flat, cDims, name, False)
    elif type == np.int32:
        array = simpl_py.Int32ArrayType(z_flat, cDims, name, False)
    elif type == np.uint32:
        array = simpl_py.UInt32ArrayType(z_flat, cDims, name, False)
    elif type == np.int64:
        array = simpl_py.Int64ArrayType(z_flat, cDims, name, False)
    elif type == np.uint64:
        array = simpl_py.UInt64ArrayType(z_flat, cDims, name, False)
    elif type == np.float32:
        array = simpl_py.FloatArrayType(z_flat, cDims, name, False)
    elif type == np.double:
        array = simpl_py.DoubleArrayType(z_flat, cDims, name, False)        
    return array
