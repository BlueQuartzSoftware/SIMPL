""" This is a basic test for AbstractFilter """

import time

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd


def BasicTest():
    """
    This will test the basic AbstractFilter methods
    """
    filter = simpl.AbstractFilter.New()

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
    assert b is False

    filter.Enabled = True
    b = filter.Enabled
    assert b is True

    filter.setErrorCondition(-1000, "Foo")
    e = filter.ErrorCode
    assert e == -1000

"""
Main entry point for python script
"""
if __name__ == "__main__":
    print("AbstractFilter Test Starting")
    BasicTest()
    print("AbstractFilter Test Complete")
