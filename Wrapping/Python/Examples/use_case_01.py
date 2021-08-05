import argparse

# Import SIMPL wrapping
import dream3d.simpl as simpl
import dream3d.simplpy as simplpy
import dream3d.simpl_helpers as simpl_helpers

# Import the OrientationAnalysis Filters
import dream3d.orientationanalysispy as orientationanalysispy

import numpy as np
import matplotlib.pyplot as plt
import math


def read_ang_data(file_path: str):
    # Create Data Container Array. Every Filter must be assigned a DataContainerArray. It is the root of the 
    # Data Structure heirarchy.
    dca = simpl.DataContainerArray()

    # Execute the ReadAngData() filter from DREAM.3D. Note that the filter is executed immediately when
    # the python code is executed.
    err = orientationanalysispy.read_ang_data(data_container_array=dca,
                                            data_container_name='EBSD Scan', 
                                            cell_ensemble_attribute_matrix_name='Phase Data', 
                                            cell_attribute_matrix_name='Scan Data',
                                            input_file=file_path)
    # Check for any errors that were encountered during the execution. Act on those errors if needed. The
    # default code will just assert and quit() the python interpretor is there was an error. This probably is
    # not very user friendly.
    assert err == 0, f'ReadAngData ErrorCondition: {err}'

    # Rotate Sample Reference Frame
    # Most is self explanatory except the rotation_representation_choice where 0=Axis-Angle and 1=Rotation Matrix
    err = simplpy.rotate_sample_ref_frame(data_container_array=dca, 
                                        cell_attribute_matrix_path=simpl.DataArrayPath('EBSD Scan', 'Scan Data', ''),
                                        rotation_axis=simpl.FloatVec3([0.0, 1.0, 0.0]), 
                                        rotation_angle=180.0, 
                                        slice_by_slice=False, 
                                        rotation_table=simpl_helpers.CreateDynamicTableData([[0.0 for x in range(3)] for y in range(3)]), 
                                        rotation_representation_choice=0)
    assert err == 0, f'RotateSampleRefFrame ErrorCondition: {err}'

    # Rotate Euler Reference Frame
    err = orientationanalysispy.rotate_euler_ref_frame(data_container_array=dca, 
                                            rotation_axis=simpl.FloatVec3([0.0, 0.0, 1.0]), 
                                            rotation_angle=90.0,
                                            cell_euler_angles_array_path=simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'EulerAngles'))
    assert err == 0, f'RotateEulerRefFrame ErrorCondition: {err}'

    # Write to DREAM3D file
    err = simpl_helpers.WriteDREAM3DFile('Small_IN100_Slice_1.dream3d', dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'
    
    # Retrieve the Confidence Index Data Array from the results using the [] notation
    dc = dca['EBSD Scan']

    # Gets all the names of the Attribute Matrixes using python list comprehension
    am_names = [am.Name for am in dc]
    
    # Grab a specific Attribute Matrix that holds the Scan Data, which is the data on each cell
    attr_mat = dc['Scan Data']

    # We need to get the dimenions of the scan from the ImageGeometry object that is stored in the Data Container
    # Note that the dimenions that come back from DREAM3D are in the order X, Y, Z which is OPPOSITE in how the data
    # is stored in DREAM.3D. The data is actually stored as Z, Y, X (Slowest to Fastest.). This will matter in a few
    # lines of code.
    image_geom = dc.Geometry
    scan_dims = image_geom.getDimensions()

    # Retrieve the Confidence Index Attribute Array which is a type of "DataArray" object
    ci = attr_mat['Confidence Index']

    # Convert the Data Array in-place to an numpy array and reshape to the proper dimensions
    # SIMPL DataArray objects just hold a flat array of Tuples. Tuples can have a dimensional aspect to them.
    np_ci = np.asarray(ci).reshape([scan_dims[1], scan_dims[0]])

    # Create a Plot that shows the Confidence Index and the histogram of the confidence Index
    fig, ax = plt.subplots(1, 2, figsize=(15,10))
    ax[0].set_title('Small IN100 Confidence Index')
    ax[0].set_xlabel('X')
    ax[0].set_ylabel('Y')
    ax[0].imshow(np_ci, cmap='inferno', vmin=0.0, vmax=1.0)
    ax[1].set_title('Confidence Index Histogram')
    ax[1].set_xlabel('Value')
    ax[1].set_ylabel('Occurrences')
    ax[1].hist(np_ci.ravel(), bins=100, range=(0, 1))

    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--input_file')
    args = parser.parse_args()

    read_ang_data(file_path=args.input_file)
    