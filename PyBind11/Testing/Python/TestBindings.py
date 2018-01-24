#!/usr/bin/env /Library/Frameworks/Python.framework/Versions/3.6/bin/python3



from SIMPLPy import *

filter = SIMPLPy.AbstractFilter.New()

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

pipeline = SIMPLPy.FilterPipeline.New()
pipeline.setName("New Pipeline")
print("Pipeline Name: %s " % pipeline.getName())

pipeline.pushBack(filter)
filterCount = pipeline.size()
print("Filter Count: %d" % filterCount)

err = pipeline.preflightPipeline()
print("Preflight ErrorCondition: %d" % err)

pipeline.execute()
err = pipeline.getErrorCondition()
print("Execute ErrorCondition: %d" % err)

pipeline.popFront()
filterCount = pipeline.size()
print("Filter Count: %d" % filterCount)

