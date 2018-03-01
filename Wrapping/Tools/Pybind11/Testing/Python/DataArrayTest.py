#!/usr/bin/env /Library/Frameworks/Python.framework/Versions/3.6/bin/python3

import time

from SIMPLibPy import *
try:
    import numpy
except ImportError:
    raise RuntimeError("This module depends on the numpy module. Please make\
sure that it is installed properly.")


num_array = numpy.zeros((100,100, 100), dtype=int)

z = numpy.asarray(num_array)
if not z.flags.contiguous:
    z = numpy.ascontiguousarray(z)

shape = z.shape
assert z.flags.contiguous, 'Only contiguous arrays are supported.'
# assert len(shape) < 3, \
#         "Only arrays of dimensionality 2 or lower are allowed!"
assert not numpy.issubdtype(z.dtype, numpy.complex128), \
        "Complex numpy arrays cannot be converted to vtk arrays."\
        "Use real() or imag() to get a component of the array before"\
        " passing it to vtk."



print("%s" % z)

dataContainer = SIMPLibPy.DataContainer.New("DataContainer")

amType = SIMPLibPy.AttributeMatrix.Type.Cell
print("amType: %s " % amType)

v_size_t = SIMPLibPy.VectorSizeT([4,3,2])



