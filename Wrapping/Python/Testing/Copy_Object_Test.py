# Pipeline : 
# This pipeline is designed to test the Copy Object filter

import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd

def copy_object_test():
    # Create Data Container Array using simpl directly
    dca = simpl.DataContainerArray()

    # Create a Data Container using the pythonic version
    err = simplpy.create_data_container(dca, 'ImageDataContainer')
    assert err == 0

    # Create an Attribute Matrix
    amDims = [[4, 5, 6]]
    tableData = sc.CreateDynamicTableData(amDims)
    dap = simpl.DataArrayPath('ImageDataContainer', 'CellAttributeMatrix', '')
    err = simplpy.create_attribute_matrix(dca, dap, simpl.AttributeMatrix.Type.Cell, tableData)
    assert err == 0
    
    # Add a data array to the Attribute Matrix by using a Filter
    dap = simpl.DataArrayPath('ImageDataContainer', 'CellAttributeMatrix', 'Int32Data')
    inputRange = (0, 0)
    err = simplpy.create_data_array(dca,simpl.ScalarTypes.Int32, 1, dap, 0, '37', inputRange)
    assert err == 0

    # Test: Copy Object
    err = simplpy.copy_object(dca, sc.ObjectToCopy.AttributeArray, '', 
                              simpl.DataArrayPath('', '', ''),
                              simpl.DataArrayPath('ImageDataContainer', 'CellAttributeMatrix', 'Int32Data'),
                              'Int32Data_Copy')
    assert err == 0, f'CopyObject ErrorCondition: {err}'

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory()
                              + '/Data/Output/CoreFilterTests/CopyObjectTest.dream3d', dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    copy_object_test()
