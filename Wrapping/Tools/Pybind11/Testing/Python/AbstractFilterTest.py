import time

from SIMPLibPy import *


def BasicTest () :
  """
  This will test the basic AbstractFilter methods
  """
  filter = SIMPLibPy.AbstractFilter.New()

  groupName = filter.GroupName
  print("groupName: %s " % groupName)

  subGroupName = filter.SubGroupName
  print("subGroupName: %s " % subGroupName)

  humanLabel = filter.HumanLabel
  print("humanLabel: %s " % humanLabel)

  filterVersion = filter.FilterVersion
  print("filterVersion: %s " % filterVersion)

  compiledLibraryName = filter.CompiledLibraryName
  print("compiledLibraryName: %s " % compiledLibraryName)


  filter.Cancel = False
  b = filter.Cancel
  print("Cancel: %s " % b)

  filter.Enabled = True
  b = filter.Enabled
  print("Enabled: %s " % b)

  filter.ErrorCondition = -1000
  e = filter.ErrorCondition
  print("ErrorCondition: %s " % e)

"""
Main entry point for python script
"""
if __name__ == "__main__":
  BasicTest()
  print("Test Complete")
