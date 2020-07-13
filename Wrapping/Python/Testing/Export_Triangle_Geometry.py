import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd

def export_triangle_geometry():
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

    # Import ASCII Data - #2 - Triangle Connectivity
    import_file = sd.GetBuildDirectory() + '/Data/SIMPL/TriangleConnectivity.csv'
    wizard_data = {
        'inputFilePath': import_file,
        'beginIndex': 2,
        'numberOfLines': 243,
        'delimiters': [','],
        'consecutiveDelimiters': False,
        'automaticAM': True,
        'selectedPath': simpl.DataArrayPath('DataContainer', 'Triangles', ''),
        'headers': ['V0', 'V1', 'V2'],
        'attrMatType': 3,
        'tupleDimensions': [242],
        'dataTypes': ['uint64_t', 'uint64_t', 'uint64_t']
    }
    err = simplpy.read_ascii_data(dca, wizard_data)
    assert err == 0, f'Import ASCII Data #2 -  ErrorCondition: {err}'

    # Combine Attribute Arrays #2:
    selected_data_array_paths = [simpl.DataArrayPath('DataContainer', 'Triangles', 'V0'),
                                 simpl.DataArrayPath('DataContainer', 'Triangles', 'V1'),
                                 simpl.DataArrayPath('DataContainer', 'Triangles', 'V2')]
    err = simplpy.combine_attribute_arrays(dca, selected_data_array_paths, 'Triangles', False)
    assert err == 0, f'Combined Attribute Arrays #2 -  ErrorCondition: {err}'

    # Delete Data # 2
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy('DataContainer').Flag = 0
    dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').Flag = 0
    dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').getDataArrayProxy('V0').Flag = 2
    dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').getDataArrayProxy('V1').Flag = 2
    dcap.getDataContainerProxy('DataContainer').getAttributeMatrixProxy('Triangles').getDataArrayProxy('V2').Flag = 2
    err = simplpy.remove_arrays(dca, dcap)
    assert err == 0, f'Remove Arrays #2 -  ErrorCondition: {err}'

    # Create Geometry
    err = sc.CreateGeometry(dca, 0, simpl.IGeometry.Type.Triangle, 'DataContainer', False,
                            shared_vertex_list_array_path=simpl.DataArrayPath('DataContainer', 'Bounds', 'Vertices'),
                            shared_tri_list_array_path=simpl.DataArrayPath('DataContainer', 'Triangles', 'Triangles'),
                            vertex_attribute_matrix_name='VertexData',
                            face_attribute_matrix_name='FaceData')
    assert err == 0, f'Create Geometry -  ErrorCondition: {err}'

    # Write to DREAM3D File
    err = simplpy.data_container_writer(dca, sd.GetTestTempDirectory() + '/Data/Output/CoreFilters/ExportTriangleGeometry.dream3d', True, False)
    assert err == 0, f'DataContainerWriter ErrorCondition: {err}'

    # Export Triangle Geometry
    err = simplpy.write_triangle_geometry(dca, 'DataContainer', sd.GetBuildDirectory() +
                                          '/Data/Output/CoreFilters/output_nodes.txt',
                                          sd.GetBuildDirectory() +
                                          '/Data/Output/CoreFilters/output_triangles.txt')
    assert err == 0, f'WriteTriangleGeometry ErrorCondition: {err}'

if __name__ == '__main__':
    export_triangle_geometry()
