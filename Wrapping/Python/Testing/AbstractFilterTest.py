import simpl

def BasicTest():
    filter = simpl.EmptyFilter()

    assert filter.GroupName == 'Core'

    assert filter.SubGroupName == 'Misc'

    assert filter.HumanLabel == 'Unknown Filter: '

    assert filter.FilterVersion != '0.0.0'

    assert filter.CompiledLibraryName == 'Core'

    filter.Cancel = False
    assert not filter.Cancel

    filter.Enabled = True
    assert filter.Enabled

    filter.setErrorCondition(-1000, 'Foo')
    assert filter.ErrorCode == -1000

if __name__ == '__main__':
    print('AbstractFilter Test Starting')
    BasicTest()
    print('AbstractFilter Test Complete')
