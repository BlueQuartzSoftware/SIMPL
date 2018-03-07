#!/usr/bin/env /Library/Frameworks/Python.framework/Versions/3.6/bin/python3

import time

from simpl import *

def Test():

  amType = simpl.AttributeMatrix.Type.Cell
  print("amType: %s " % amType)

  v_size_t = simpl.VectorSizeT([4,3,2])

  attributeMatrix = simpl.AttributeMatrix.Create([100,100,100], "Python AM", simpl.AttributeMatrix.Type.Cell)
  err = 0

#-------------------------------------

#proxyRequirements = simpl.SIMPLH5DataReaderRequirements()
#simplH5Reader = simpl.SIMPLH5DataReader()
#simplH5Reader.openFile("/Users/Shared/DREAM3D_SDK/DREAM3D_Data/Data/CubicSingleEquiaxed.dream3d")
#dataContainerProxy = simplH5Reader.readDataContainerArrayStructure(proxyRequirements, err)
#simplH5Reader.closeFile()

def PipelineTest():
  inputPath = "/Users/mjackson/DREAM3D-Dev/DREAM3D-Build/Debug/Bin/Data/Output/SmallIN100.dream3d"
  reader = simpl.DataContainerReader.New()
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


  writer = simpl.DataContainerWriter.New()
  writer.OutputFile = ("/tmp/SmallIN100Mesh_GBCD.dream3d")
  print("humanLabel: %s " % writer.HumanLabel)
  print("groupName: %s " % writer.GroupName)
  print("subGroupName: %s " % writer.SubGroupName)
  print("filterVersion: %s " % writer.FilterVersion)
  print("compiledLibraryName: %s " % writer.CompiledLibraryName)


  pipeline = simpl.FilterPipeline.New()
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

  pipeline = simpl.FilterPipeline.New()
  print("Pipeline to Null")
  reader = simpl.AbstractFilter.New()
  writer = simpl.AbstractFilter.New()
  print("Filter to null")

  time.sleep(2)


"""
Main entry point for python script
"""
if __name__ == "__main__":
  PipelineTest()
  print("Test Complete")

