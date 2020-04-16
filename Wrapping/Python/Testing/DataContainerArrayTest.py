import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd

def AttributeMatrixAccessTest():
    '''
    Test the various methods that deal with AttributeMatrix
    and DataContainerArray
    '''
    print('Starting AttributeMatrixAccessTest....')
    dca = sc.CreateDataContainerArray()
    dc = sc.CreateDataContainer('AM Test')
    dca.addOrReplaceDataContainer(dc)
    
    amType = simpl.AttributeMatrix.Type.Cell
    tupleDims = simpl.VectorSizeT([5, 4, 3])
    am = simpl.AttributeMatrix.New(tupleDims, 'CellAttributeMatrix', amType)
    dc.addOrReplaceAttributeMatrix(am)

    # See if we can get the AttributeMatrix based on a DataArrayPath object
    dap = simpl.DataArrayPath('AM Test', 'CellAttributeMatrix', '')
    am0 = dca.getAttributeMatrix(dap)
    assert am0 == am
    assert True == dca.doesAttributeMatrixExist(dap)

    # Change the data array path to something that does not exist.
    dap.update('AM Test', 'Foo', '')
    am0 = dca.getAttributeMatrix(dap)
    assert am0 != am
    assert False == dca.doesAttributeMatrixExist(dap)

def DataContainerTest():
    '''
    This is the main unit test
    This needs to be updated as more methods are exposed to Python
    '''
    print('Starting DataContainerTest....')
    dca = sc.CreateDataContainerArray()
    dc0 = sc.CreateDataContainer('dc0')
    dc1 = sc.CreateDataContainer('dc1')

    dca.addOrReplaceDataContainer(dc0)
    dca.addOrReplaceDataContainer(dc1)

    assert dca.doesDataContainerExist('dc0')
    assert dca.doesDataContainerExist('dc1')

    dc0 = dca.removeDataContainer('dc0')
    assert not dca.doesDataContainerExist('dc0')

    dca.renameDataContainer('dc1', 'dc0')
    assert not dca.doesDataContainerExist('dc1')
    assert dca.doesDataContainerExist('dc0')

    numDC = dca.getNumDataContainers()
    assert numDC == 1

    dca.duplicateDataContainer('dc0', 'dc1')
    numDC = dca.getNumDataContainers()
    assert numDC == 2

    dca.clearDataContainers()
    numDC = dca.getNumDataContainers()
    assert numDC == 0

def MiscTest():
    print('Starting MiscTest')
    dca = sc.CreateDataContainerArray()
    dc = sc.CreateDataContainer('My New DataContainer')
    dca.addOrReplaceDataContainer(dc)

if __name__ == '__main__':
    MiscTest()
    DataContainerTest()
    AttributeMatrixAccessTest()
    print('DataContainerArrayTest Test Complete')
