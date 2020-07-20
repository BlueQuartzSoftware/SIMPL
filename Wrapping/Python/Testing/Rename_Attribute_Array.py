# Pipeline : Rename Attribute Array Filter Test
# Based on (01) SmallIN100 Quick Mesh (from EBSD Surface Meshing)

import simpl
import simplpy
import simpl_helpers as sh
import simpl_test_dirs as sd

def rename_array_test():
    # Create Data Container Array
    dc_name = 'foo'
    am_name = 'bar'
    da_name = 'baz'

    dca = simpl.DataContainerArray()
    dc = simpl.DataContainer(dc_name)
    am = simpl.AttributeMatrix(simpl.VectorSizeT([10]), am_name, simpl.AttributeMatrix.Type.Generic)
    da = simpl.Int32Array(10, da_name, 0)

    am.addOrReplaceAttributeArray(da)
    dc.addOrReplaceAttributeMatrix(am)
    dca.addOrReplaceDataContainer(dc)

    # Rename Attribute Array
    err = simplpy.rename_attribute_array(dca, simpl.DataArrayPath(dc_name, am_name, da_name), 'qux')
    assert err == 0, f'RenameAttributeArray ErrorCondition {err}'

if __name__ == '__main__':
    rename_array_test()
