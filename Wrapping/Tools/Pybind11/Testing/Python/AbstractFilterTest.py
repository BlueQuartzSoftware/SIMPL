import time

from simpl_py import *

# This is a test

def BasicTest () :
  """
  This will test the basic AbstractFilter methods
  """
  filter = simpl_py.AbstractFilter.New()

  groupName = filter.GroupName
  assert groupName == "YOUR CLASS SHOULD IMPLEMENT THIS"

  subGroupName = filter.SubGroupName
  assert subGroupName == "YOUR CLASS SHOULD IMPLEMENT THIS"


  humanLabel = filter.HumanLabel
  assert humanLabel == "YOUR CLASS SHOULD IMPLEMENT THIS"

  filterVersion = filter.FilterVersion
  assert filterVersion == "0.0.0"

  compiledLibraryName = filter.CompiledLibraryName
  assert compiledLibraryName == ""


  filter.Cancel = False
  b = filter.Cancel
  assert b == False

  filter.Enabled = True
  b = filter.Enabled
  assert b == True

  filter.ErrorCondition = -1000
  e = filter.ErrorCondition
  assert e == -1000

"""
Main entry point for python script
"""
if __name__ == "__main__":
  BasicTest()
  print("AbstractFilter Test Complete")
