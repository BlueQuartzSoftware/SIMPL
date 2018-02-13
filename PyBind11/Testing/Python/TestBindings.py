#!/usr/bin/env /Library/Frameworks/Python.framework/Versions/3.6/bin/python3

import time

from SIMPLibPy import *

filter = SIMPLibPy.AbstractFilter.New()

groupName = filter.getGroupName()
print("groupName: %s " % groupName)

subGroupName = filter.getSubGroupName()
print("subGroupName: %s " % subGroupName)

humanLabel = filter.getHumanLabel()
print("humanLabel: %s " % humanLabel)

filterVersion = filter.getFilterVersion()
print("filterVersion: %s " % filterVersion)

compiledLibraryName = filter.getCompiledLibraryName()
print("compiledLibraryName: %s " % compiledLibraryName)


filter.setCancel(False)
b = filter.getCancel()
print("Cancel: %s " % b)

filter.setEnabled(True)
b = filter.getEnabled()
print("Enabled: %s " % b)

filter.setErrorCondition(-1000)
e = filter.getErrorCondition()
print("ErrorCondition: %s " % e)


#-------------------------------------
err = 0
#proxyRequirements = SIMPLibPy.SIMPLH5DataReaderRequirements()
#simplH5Reader = SIMPLibPy.SIMPLH5DataReader()
#simplH5Reader.openFile("/Users/Shared/DREAM3D_SDK/DREAM3D_Data/Data/CubicSingleEquiaxed.dream3d")
#dataContainerProxy = simplH5Reader.readDataContainerArrayStructure(proxyRequirements, err)
#simplH5Reader.closeFile()


inputPath = "/Users/mjackson/DREAM3D-Dev/DREAM3D-Build/Debug/Bin/Data/Output/SmallIN100.dream3d"
reader = SIMPLibPy.DataContainerReader.New()
reader.setInputFile(inputPath)
reader.setOverwriteExistingDataContainers(True)
# This must be called to generate a default DataContainerArrayProxy object from the file structure
dataContainerProxy = reader.readDataContainerArrayStructure(inputPath)
# Now set that proxy into the filter.
reader.setInputFileDataContainerArrayProxy(dataContainerProxy)

print("humanLabel: %s " % reader.getHumanLabel())
print("groupName: %s " % reader.getGroupName())
print("subGroupName: %s " % reader.getSubGroupName())
print("filterVersion: %s " % reader.getFilterVersion())
print("compiledLibraryName: %s " % reader.getCompiledLibraryName())


writer = SIMPLibPy.DataContainerWriter.New()
writer.setOutputFile("/tmp/SmallIN100Mesh_GBCD.dream3d")
print("humanLabel: %s " % writer.getHumanLabel())
print("groupName: %s " % writer.getGroupName())
print("subGroupName: %s " % writer.getSubGroupName())
print("filterVersion: %s " % writer.getFilterVersion())
print("compiledLibraryName: %s " % writer.getCompiledLibraryName())



pipeline = SIMPLibPy.FilterPipeline.New()
pipeline.setName("New Pipeline")
print("Pipeline Name: %s " % pipeline.getName())

pipeline.pushBack(reader)
pipeline.pushBack(writer)
filterCount = pipeline.size()
print("Filter Count: %d" % filterCount)

err = pipeline.preflightPipeline()
print("Preflight ErrorCondition: %d" % err)

dataContainer = pipeline.run()
err = pipeline.getErrorCondition()
print("Execute ErrorCondition: %d" % err)

pipeline.popFront()
filterCount = pipeline.size()
print("Filter Count: %d" % filterCount)

pipeline = SIMPLibPy.FilterPipeline.New()
print("Pipeline to Null")
reader = SIMPLibPy.AbstractFilter.New();
writer = SIMPLibPy.AbstractFilter.New();
print("Filter to null")


time.sleep(2)
print("done")
