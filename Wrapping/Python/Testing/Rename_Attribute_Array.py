# Pipeline : Rename Attribute Array Filter Test
# Based on (01) SmallIN100 Quick Mesh (from EBSD Surface Meshing)

import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd

def rename_array_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/Statistics/SmallIN100_CrystalStats.dream3d')
    assert err == 0, f'DataContainerReader ErrorCondition {err}'

    # Rename Attribute Array
    err = simplpy.rename_attribute_array(dca,
                                         simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                             'IPFColor'),
                                         'InversePoleFigureColor')
    assert err == 0, f'RenameAttributeArray ErrorCondition {err}'

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/CoreFilterTests/' +
                              'RenameAttributeArray.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    rename_array_test()
