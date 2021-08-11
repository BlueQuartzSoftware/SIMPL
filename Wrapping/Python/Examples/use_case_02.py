import argparse
import csv
import math

import numpy as np
from numpy.lib import recfunctions as rfn

# Import SIMPL wrapping
import dream3d.simpl as simpl
import dream3d.simplpy as simplpy
import dream3d.simpl_helpers as simpl_helpers


#Generates a csv file with floats 0-37988 and ints 1-37989
def make_random_data():
    with open('new_data.csv', 'w') as f:
        writer = csv.writer(f, lineterminator='\n',)
        writer.writerow(["Float", "Int"])
        i = 0
        while i < 37989:
            row = [i, i+1]
            writer.writerow(row)
            i = i+1

def import_csv_xyz_vertices_2(file_path: str):
    """
    This version of the sample code uses slightly different CSV python APIs the difference being the use of
    the numpy.loadtxt() function.

    """
    verts_np = np.loadtxt(file_path, dtype={'names': ('x', 'y', 'z'), 'formats': ('f4', 'f4', 'f4')}, usecols=(0, 1, 2), delimiter=',', skiprows=1)
    verts_flat = rfn.structured_to_unstructured(verts_np, dtype=np.float32, copy=False, casting='safe')
    
    # Create a simpl.DataArray object to hold the vertices by reference
    verts = simpl.FloatArray(verts_flat, "Vertices", False)
    # Create a VertexGeometry and set the vertices into the geometry
    vertGeom = simpl.VertexGeom()
    vertGeom.setVertices(verts)
    # Create the DataContainerArray
    dca = simpl.DataContainerArray()
    # Create the DataContainer with the Geometry attached
    dc = simpl.DataContainer('Vertex Geometry')
    dc.setGeometry(vertGeom)
    # Add the data container to the DataContainerArray
    dca.addOrReplaceDataContainer(dc)
    # Write the Vertex Geometry out to a .dream3d file.
    err = simpl_helpers.WriteDREAM3DFile('vert_geom_2.dream3d', dca, True)
    
    # Create Cell AttributeMatrix
    tupleDims = simpl.VectorSizeT([1, 37989])
    am = simpl_helpers.CreateAttributeMatrix(tupleDims, 'CellAttributeMatrix', simpl.AttributeMatrix.Type.Cell)

    # Create Data Arrays
    #Loads in csv file and gets data for each type
    uints = np.genfromtxt('new_data.csv', dtype=np.uint32, skip_header=1, usecols=1, delimiter=',')
    f32s = np.genfromtxt('new_data.csv', dtype=np.float32, usecols=0, skip_header=1, delimiter=',')

    #Creates arrays from data with shape of (# of vertices x 1)
    uintarr = np.ndarray(shape=(37989, 1), dtype=np.uint32, buffer=uints)
    floatarr = np.ndarray(shape=(37989, 1), dtype=np.float32, buffer=f32s)

    #     Number of tuples must match the number of vertices
    #     Create 2 Data Arrays: UInt32 and Float32
    #     Populate data ararys with some random data
    #       or
    #     for bonus points read the data from another text file
    #Makes simpl arrays using numpy arrays
    uint32arr = simpl.UInt32Array(uintarr, 'UInt32 Arr', True)
    f32arr = simpl.FloatArray(floatarr, 'Float32 Arr', True)
    
    # Add Data Arrays to AttributeMatrix
    am.addOrReplaceAttributeArray(uint32arr)
    am.addOrReplaceAttributeArray(f32arr)

    # Add AttributeMatirx to DataContainer
    dc.addOrReplaceAttributeMatrix(am)


def import_csv_xyz_vertices_1(file_path: str):
    """
    This version of the sample code uses slightly different CSV python APIs the difference being the use of
    the numpy.genfromtxt() function.

    The downside of this approach is that the code will read the entire CSV file into memory into a string variable
    first before passing that string variable to the numpy.genfromtxt() function.

    This function may have issues if there are more columns in the input file.
    """
    # Read the entire file into a string variable and then have numpy parse the columns out into independent arrays

    with open(file_path, 'r') as file:
        data = file.read()
    x,y,z = np.genfromtxt(data.splitlines(), dtype="f4,f4,f4", delimiter=',', encoding=None, skip_header=1, unpack=True) 
    
    # Combine the arrays into a single array where each Tuple of the array is XYZ,XYZ,XYZ
    np_verts = np.stack((x,y,z), axis=-1)
    shape = np_verts.shape

    # Create a simpl.DataArray object to hold the vertices by reference
    verts = simpl.FloatArray(np_verts, "Vertices", False)
    # Create a VertexGeometry and set the vertices into the geometry
    vertGeom = simpl.VertexGeom()
    vertGeom.setVertices(verts)
    # Create the DataContainerArray
    dca = simpl.DataContainerArray()
    # Create the DataContainer with the Geometry attached
    dc = simpl.DataContainer('Vertex Geometry')
    dc.setGeometry(vertGeom)
    # Add the data container to the DataContainerArray
    dca.addOrReplaceDataContainer(dc)
    # Write the Vertex Geometry out to a .dream3d file.
    err = simpl_helpers.WriteDREAM3DFile('vert_geom_1.dream3d', dca, True)



if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--input_file')
    args = parser.parse_args()

    import_csv_xyz_vertices_1(file_path=args.input_file)
    import_csv_xyz_vertices_2(file_path=args.input_file)
