#!/usr/bin/env /Library/Frameworks/Python.framework/Versions/3.6/bin/python3

import time

from SIMPLibPy import *
try:
    import numpy as np
except ImportError:
    raise RuntimeError("This module depends on the numpy module. Please make\
sure that it is installed properly.")

def CreateDataContainerArray(name)
    dca = SIMPLibPy.DataContainerArray.New()
    dca.Name = name
    return dca

def CreateDataContainer():





# Create a DataContainerArray
dca = SIMPLibPy.DataContainerArray.New()


# Create a Data Container
dc = SIMPLibPy.DataContainer.New("DataContainer")
print("DataContainer Created: %s" % dc.Name)
# Add the DataContainer to the DataContainerArray
dca.addDataContainer(dc)

# Create a Cell AtttributeMatrix to hold the AttributeArrays
shape = SIMPLibPy.VectorSizeT([4,5,2])
amType = SIMPLibPy.AttributeMatrix.Type.Cell
am = SIMPLibPy.AttributeMatrix.Create(shape, "CellAttributeMatrix", amType)
print("Attribute Matrix Created: %s" % am.Name)
print("  Dimensions: %s" % shape)
# Add the AttributeMatrix to the DataContainer
dc.addAttributeMatrix(am.Name, am)


# Create an array that holds the number of voxels in each of the orthoganal axis of the cartesian coordinate system

# charArray = SIMPLibPy.Int8ArrayType(100, "Test", 1)
# charArray.setValue(0, 10)
# val = charArray.getValue(0)
# print("val: %d" % val)

# Create a numpy array of ones to hold our data
num_array = np.ones(shape, dtype=np.int32)

z = np.asarray(num_array)
if not z.flags.contiguous:
    z = np.ascontiguousarray(z)

shape = z.shape
assert z.flags.contiguous, 'Only contiguous arrays are supported.'
# assert len(shape) < 3, \
#         "Only arrays of dimensionality 2 or lower are allowed!"
assert not np.issubdtype(z.dtype, np.complex128), \
        "Complex numpy arrays cannot be converted to vtk arrays."\
        "Use real() or imag() to get a component of the array before"\
        " passing it to vtk."

# Get the Pointer to the numpy array
z_flat = np.ravel(z) 
nTuples = np.prod(shape)
print("nTuples: %d " % nTuples)
# Declare the number of components for the array
cDims = SIMPLibPy.VectorSizeT([1])
# Create the SIMPL.DataArray<int32_t> from the pointer. This should _wrap_ the array and NOT copy any data
intArray = SIMPLibPy.Int32ArrayType(z_flat, cDims, "Int32Array_1Comp", False)
#int8Array = SIMPLibPy.Int8ArrayType(z_flat, nTuples, cDims, "Int8Array_1Comp", False)


# Add the DataArray to the AttributeMatrix
am.addAttributeArray(intArray.Name, intArray)

if am.doesAttributeArrayExist("Int32Array_1Comp"):
    print("'Int32Array_1Comp' does exist")
else:
    print("'Int32Array_1Comp' does NOT exist in the Attribute Matrix")


# Lets write out the whole Heirarchy to a .dream3d file
print("Creating DREAM3D Writer filter....")
writer = SIMPLibPy.DataContainerWriter.New()
writer.OutputFile = ("/tmp/DataArrayTest.dream3d")
writer.setDataContainerArray(dca)
writer.execute()



#intArray.setValue(0, 24)
print("num_array: %s" % num_array)
writer.OutputFile = ("/tmp/DataArrayTest_alt.dream3d")
writer.execute()

