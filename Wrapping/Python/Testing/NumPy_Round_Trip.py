
import time

try:
    import numpy as np
except ImportError:
    raise RuntimeError("This module depends on the numpy module. Please make\
sure that it is installed properly.")

# These are the SIMPL python modules
from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orient

def Test1():
  """
  This will test manually creating an numpy array, wrapping the numpy array with
  a simpl.DataArray, running a filter to adjust those values, and then showing 
  that the original numpy array has in fact been changed.
  """
  print("===================== Test 1 =====================")
  shape = [4, 3, 2]
  cDims = [3]
  # Create a numpy array of ones to hold our data
  ashape = np.append([np.prod(shape)], cDims)
  # Create a numpy array to hold our data
  num_array = np.ndarray(ashape, dtype=np.float32, order="C")
  # Get the numpy array as contiguous
  z = np.asarray(num_array)
  if not z.flags.contiguous:
      z = np.ascontiguousarray(z)
  z.fill(3.1415927)
  assert z.flags.contiguous, 'Only contiguous arrays are supported.'
  print(z[0])

  # Create the DataContainerArray
  dca = sc.CreateDataContainerArray()
  # Create and add a DataContainer to the DataContainerArray
  dc = sc.CreateDataContainer("DC")
  dca.addOrReplaceDataContainer(dc)
  # Create and add an AttributeMatrix to the DataContainerArray
  shape = simpl.VectorSizeT([4, 3, 2])
  cellAm = sc.CreateAttributeMatrix(shape, "CAM", simpl.AttributeMatrix.Type.Cell)
  dc.addOrReplaceAttributeMatrix(cellAm)

  # Create the wrapped DataArray<float> array
  eulers = simpl.FloatArrayType(z, "Eulers", False)
  # Add the DataArray to the AttributeMatrix
  cellAm.addOrReplaceAttributeArray(eulers)

  # Create a DataArrayPath
  dap = simpl.DataArrayPath("DC", "CAM", "Eulers")
  # Run the filter ChangeAngleRepresentation since this filter acts on data "in place"
  err = orient.change_angle_representation(dca, 1, dap)
  if err < 0:
    print("change_angle_representation ErrorCondition: %d" % err)

  roundtrip = np.asarray(eulers)

  # print("--------------- roundtrip ---------------")
  print(roundtrip.shape)
  print(roundtrip[0])



def Test2():
  """
  This is the Top level to test the creation of Data Arrays from numpy and placing those
  arrays into a DREAM3D DataContainer heirarchy. The numpy array is created on the 
  python side of things.
  """
  print("===================== Test 2 =====================")
  dca = sc.CreateDataContainerArray()

  dc = sc.CreateDataContainer("DC")
  dca.addOrReplaceDataContainer(dc)

  shape = simpl.VectorSizeT([4, 3, 2])
  cellAm = sc.CreateAttributeMatrix(shape, "CAM", simpl.AttributeMatrix.Type.Cell)
  dc.addOrReplaceAttributeMatrix(cellAm)
  # Create the Component Dimensions for the Array, 3 Component in this case
  cDims = simpl.VectorSizeT([3])
  # Create the array which wraps a Numpy contiguous array 
  npyArray, eulers = sc.CreateDataArray("Eulers", shape, cDims, np.float32)
  npyArray.fill(3.1415927)
  # Add the DataArray to the AttributeMatrix
  cellAm.addOrReplaceAttributeArray(eulers)

  # Create a DataArrayPath
  dap = simpl.DataArrayPath("DC", "CAM", "Eulers")
  # Run the filter ChangeAngleRepresentation since this filter acts on data "in place"
  err = orient.change_angle_representation(dca, 1, dap)
  if err < 0:
    print("change_angle_representation ErrorCondition: %d" % err)


def Test3():
  """
  Create a simpl DataArray<T>, then convert that to a numpy array in place
  and do numpy operations to create a new array, then wrote both arrays
  to a .dream3d file
  """
  print("===================== Test 3 =====================")

  dca = sc.CreateDataContainerArray()

  dc = sc.CreateDataContainer("DC")
  dca.addOrReplaceDataContainer(dc)
  # The shape is in XYZ order
  shape = simpl.VectorSizeT([4, 3, 2])
  cellAm = sc.CreateAttributeMatrix(shape, "CAM", simpl.AttributeMatrix.Type.Cell)
  dc.addOrReplaceAttributeMatrix(cellAm)

  # Create the Component Dimensions for the Array, 3 Component in this case
  cDims = simpl.VectorSizeT([3])
  eulers = simpl.FloatArrayType(24, cDims, "Eulers", 3.1415927)
  cellAm.addOrReplaceAttributeArray(eulers)

  # now build up a pipeline to act on the Data Structure that we just created
  dap = simpl.DataArrayPath("DC", "CAM", "Eulers")
  err = orient.change_angle_representation(dca, 1, dap)
  if err < 0:
    print("change_angle_representation ErrorCondition: %d" % err)

  # Convert in-place to an numpy array
  np_eulers = np.asarray(eulers)
  print(np_eulers.shape)

  # Create new numpy array with the result of the calculation
  np_rads = np_eulers * 3.1415927 / 180.0

  if not np_rads.flags.contiguous:
    np_rads = np.ascontiguousarray(np_rads)
  assert np_rads.flags.contiguous, 'Only contiguous arrays are supported.'
  
  # Create the wrapped DataArray<float> object
  simpl_rads = simpl.FloatArrayType(np_rads, "Eulers as Rads", False)
  # Add the DataArray to the AttributeMatrix
  cellAm.addOrReplaceAttributeArray(simpl_rads)

  # Write to DREAM3D file
  err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/Python_RoundTrip/RoundTripTest.dream3d", dca)
  if err < 0:
      print("WriteDREAM3DFile ErrorCondition: %d" % err)
 


"""
Main entry point for python script
"""
if __name__ == "__main__":
  Test1()
  Test2()
  Test3()
  print("[NumPy_Round_Trip] Complete")
