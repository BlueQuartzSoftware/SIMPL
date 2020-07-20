# Pipeline: Crop Vertex Geometry Test
# Based on CreateVertexGeometry example

import simpl
import simplpy
import simpl_helpers as sh
import simpl_test_dirs as sd

def crop_vertex_geometry_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Create the Data Container
    err = simplpy.create_data_container(dca, 'DataContainer')
    assert err == 0, f'DataContainer ErrorCondition: {err}'

    # Import ASCII Data - #1 - Vertex Coordinates
    import_file = sd.GetBuildDirectory() + '/Data/SIMPL/VertexCoordinates.csv'
    wizard_data = {
        'inputFilePath': import_file,
        'beginIndex': 2,
        'numberOfLines': 145,
        'delimiters': [','],
        'consecutiveDelimiters': False,
        'automaticAM': True,
        'selectedPath': simpl.DataArrayPath('DataContainer', 'Bounds', ''),
        'headers': ['x', 'y', 'z'],
        'attrMatType': 3,
        'tupleDimensions': [144],
        'dataTypes': ['float', 'float', 'float']
    }
    err = simplpy.read_ascii_data(dca, wizard_data)
    assert err == 0, f'Import ASCII Data #1 -  ErrorCondition: {err}'

    # Combine Attribute Arrays # 1:
    selected_data_array_paths = [simpl.DataArrayPath('DataContainer', 'Bounds', 'x'),
                                 simpl.DataArrayPath('DataContainer', 'Bounds', 'y'),
                                 simpl.DataArrayPath('DataContainer', 'Bounds', 'z')]
    err = simplpy.combine_attribute_arrays(dca, selected_data_array_paths, 'Vertices', False)
    assert err == 0, f'Combined Attribute Arrays #1 -  ErrorCondition: {err}'

    # Delete Data # 1
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy('DataContainer').Flag = 0
    dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').Flag = 0
    dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').getDataArrayProxy('x').Flag = 2
    dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').getDataArrayProxy('y').Flag = 2
    dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').getDataArrayProxy('z').Flag = 2
    err = simplpy.remove_arrays(dca, dcap)
    assert err == 0, f'Remove Arrays #1 -  ErrorCondition: {err}'

    # Create Geometry
    err = sh.CreateGeometry(dca, 0, simpl.IGeometry.Type.Vertex, 'DataContainer', False,
                            shared_vertex_list_array_path=simpl.DataArrayPath('DataContainer', 'Bounds', 'Vertices'),
                            vertex_attribute_matrix_name='VertexData')
    assert err == 0, f'Create Geometry -  ErrorCondition: {err}'

    # Crop Geometry (Vertex)
    err = simplpy.crop_vertex_geometry(dca, 'DataContainer', 'CroppedDataContainer',
                                       -0.5, -0.5, 0.0, 0.5, 0.5, 1)
    assert err == 0, f'CropVertexGeometry -  ErrorCondition: {err}'

    # Delete Data #2
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy('DataContainer').Flag = 2
    dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').Flag = 2
    dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('VertexData').Flag = 2
    dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Bounds').getDataArrayProxy('Vertices').Flag = 2
    err = simplpy.remove_arrays(dca, dcap)
    assert err == 0, f'Remove Arrays #2 -  ErrorCondition: {err}'

    # Write to DREAM.3D File
    err = simplpy.data_container_writer(dca, sd.GetBuildDirectory() +
                                        '/Data/Output/CoreFilters/' +
                                        'CropVertexGeometry.dream3d', True, False)
    assert err == 0, f'DataContainerWriter ErrorCondition: {err}'

if __name__ == '__main__':
    crop_vertex_geometry_test()
