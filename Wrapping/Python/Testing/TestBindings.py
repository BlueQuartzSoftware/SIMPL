
import time


# These are the simpl_py python modules

import dream3d
import dream3d.dream3d_py
import dream3d.dream3d_py as d3d
import dream3d.dream3d_py.simpl_py as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd

def PipelineTest():
  inputPath = sd.GetTestTempDirectory() + "/SmallIN100.dream3d"
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
  writer.OutputFile = (sd.GetTestTempDirectory() + "/TestBindings.dream3d")
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

