import time


# These are the simpl_py python modules

import dream3d
import dream3d.dream3d_py
import dream3d.dream3d_py as d3d
import dream3d.dream3d_py.simpl_py as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd

def Pipeline1():

  pipeline = simpl.FilterPipeline.New()
  pipeline.Name = ("Image Data Container Test Pipeline")

  createDataContainer = simpl.CreateDataContainer.New()
  createDataContainer.DataContainerName = "ImageDataContainer"
  pipeline.pushBack(createDataContainer)

  createImageGeom = simpl.CreateImageGeometry.New()
  createImageGeom.SelectedDataContainer = createDataContainer.DataContainerName
  createImageGeom.Dimensions = simpl.IntVec3(101, 101, 1)
  createImageGeom.Resolution = simpl.FloatVec3(1.0, 1.0, 1.0)
  createImageGeom.Origin = simpl.FloatVec3(0.0, 0.0, 0.0)
  pipeline.pushBack(createImageGeom)

  createAttributeMatrix = simpl.CreateAttributeMatrix.New()
  createAttributeMatrix.CreatedAttributeMatrix = simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", "")
  createAttributeMatrix.AttributeMatrixType = simpl.AttributeMatrix.Type.Cell
  # This next part of the code is really ugly and verbose and there is probably
  # a better way to do it, but this was all I could get working. Somebody please
  # change it.
  rHdrs = []
  rHdrs.append("Row[0]")
  cHdrs = []
  cHdrs.append("Col[6]")
  cHdrs.append("Col[6]")
  cHdrs.append("Col[6]")
  rowDim = simpl.VectorDouble([101,101,1])
  rDims = []
  rDims.append(rowDim)
  ddt = simpl.DynamicTableData.Create(rDims, rHdrs, cHdrs)
  createAttributeMatrix.TupleDimensions = ddt
  # END really ugly section
  pipeline.pushBack(createAttributeMatrix)

  createDataArray = simpl.CreateDataArray.New()
  createDataArray.ScalarType = simpl.ScalarTypes.UInt8
  createDataArray.NumberOfComponents = 1
  createDataArray.NewArray = simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", "ScalarValues")
  createDataArray.InitializationType = 0
  createDataArray.InitializationValue = "128"
  pipeline.pushBack(createDataArray)

  writer = simpl.DataContainerWriter.New()
  writer.OutputFile = (sd.GetTestTempDirectory() + "/PipelineTest.dream3d")
  print(" humanLabel: %s " % writer.HumanLabel)
  print(" groupName: %s " % writer.GroupName)
  print(" subGroupName: %s " % writer.SubGroupName)
  print(" filterVersion: %s " % writer.FilterVersion)
  print(" compiledLibraryName: %s " % writer.CompiledLibraryName)

  pipeline.pushBack(writer)
  print("%s" % sd.GetTestTempDirectory() + "/PipelineTest.dream3d")

  err = pipeline.preflightPipeline()
  print("Preflight ErrorCondition: %d" % err)
  dataContainer = pipeline.run()
  err = pipeline.ErrorCondition
  print("Execute ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
  print("=> PipelineTest Start")
  Pipeline1()
  print("=> PipelineTest Complete")
