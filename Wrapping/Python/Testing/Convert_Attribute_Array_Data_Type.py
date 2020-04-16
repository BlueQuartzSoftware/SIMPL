''' 
Creates a Int32 array and then converts it to UInt8
'''

import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd

import datetime

def convert_data_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Create a Data Container
    dc = sc.CreateDataContainer('ImageDataContainer')
    dca.addOrReplaceDataContainer(dc)
 
    shape = simpl.VectorSizeT([4, 5, 2])
    cellAm = sc.CreateAttributeMatrix(shape, 'CellAttributeMatrix', simpl.AttributeMatrix.Type.Cell)
    dc.addOrReplaceAttributeMatrix(cellAm)   
    
    dap = simpl.DataArrayPath('ImageDataContainer', 'CellAttributeMatrix', 'Int32Data')
    inputRange = (0,0)
    err = simplpy.create_data_array(dca,simpl.ScalarTypes.Int32, 1, dap, 0, '37', inputRange)
    assert err == 0

    # Test: Convert AttributeArray Data Type
    err = simplpy.convert_data(dca, simpl.NumericTypes.UInt8, 'uint8',
                               simpl.DataArrayPath('ImageDataContainer', 'CellAttributeMatrix', 'Int32Data'))
    assert err == 0

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory()
                              + '/Data/Output/CoreFilterTests/ConvertAttributeArrayDataType.dream3d', dca)
    assert err == 0

if __name__ == '__main__':
    convert_data_test()
