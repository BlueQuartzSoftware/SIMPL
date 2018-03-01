#!/usr/bin/env /Library/Frameworks/Python.framework/Versions/3.6/bin/python3

import time

from SIMPLibPy import *

def Test():

  amType = SIMPLibPy.AttributeMatrix.Type.Cell
  print("amType: %s " % amType)

  v_size_t = SIMPLibPy.VectorSizeT([4,3,2])

  attributeMatrix = SIMPLibPy.AttributeMatrix.Create([100,100,100], "Python AM", SIMPLibPy.AttributeMatrix.Type.Cell)
  err = 0

#-------------------------------------

#proxyRequirements = SIMPLibPy.SIMPLH5DataReaderRequirements()
#simplH5Reader = SIMPLibPy.SIMPLH5DataReader()
#simplH5Reader.openFile("/Users/Shared/DREAM3D_SDK/DREAM3D_Data/Data/CubicSingleEquiaxed.dream3d")
#dataContainerProxy = simplH5Reader.readDataContainerArrayStructure(proxyRequirements, err)
#simplH5Reader.closeFile()

def PipelineTest():
  inputPath = "/Users/mjackson/DREAM3D-Dev/DREAM3D-Build/Debug/Bin/Data/Output/SmallIN100.dream3d"
  reader = SIMPLibPy.DataContainerReader.New()
  reader.InputFile = (inputPath)
  reader.OverwriteExistingDataContainers = (True)
  # This must be called to generate a default DataContainerArrayProxy object from the file structure
  dataContainerProxy = reader.readDataContainerArrayStructure(inputPath)
  # Now set that proxy into the filter.
  reader.InputFileDataContainerArrayProxy = (dataContainerProxy)

  print("humanLabel: %s " % reader.HumanLabel)
  print("groupName: %s " % reader.GroupName)
  print("subGroupName: %s " % reader.SubGroupName)
  print("filterVersion: %s " % reader.FilterVersion)
  print("compiledLibraryName: %s " % reader.CompiledLibraryName)


  writer = SIMPLibPy.DataContainerWriter.New()
  writer.OutputFile = ("/tmp/SmallIN100Mesh_GBCD.dream3d")
  print("humanLabel: %s " % writer.HumanLabel)
  print("groupName: %s " % writer.GroupName)
  print("subGroupName: %s " % writer.SubGroupName)
  print("filterVersion: %s " % writer.FilterVersion)
  print("compiledLibraryName: %s " % writer.CompiledLibraryName)


  pipeline = SIMPLibPy.FilterPipeline.New()
  pipeline.Name = ("New Pipeline")
  print("Pipeline Name: %s " % pipeline.Name)

  pipeline.pushBack(reader)
  pipeline.pushBack(writer)
  filterCount = pipeline.size()
  print("Filter Count: %d" % filterCount)

  err = pipeline.preflightPipeline()
  print("Preflight ErrorCondition: %d" % err)

  dataContainer = pipeline.run()
  err = pipeline.ErrorCondition
  print("Execute ErrorCondition: %d" % err)

  pipeline.popFront()
  filterCount = pipeline.size()
  print("Filter Count: %d" % filterCount)

  pipeline = SIMPLibPy.FilterPipeline.New()
  print("Pipeline to Null")
  reader = SIMPLibPy.AbstractFilter.New()
  writer = SIMPLibPy.AbstractFilter.New()
  print("Filter to null")

  time.sleep(2)


"""
Main entry point for python script
"""
if __name__ == "__main__":
  PipelineTest()
  print("Test Complete")

