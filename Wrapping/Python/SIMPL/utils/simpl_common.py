
import dream3d
import dream3d.dream3d_py
import dream3d.dream3d_py as d3d
import dream3d.dream3d_py.simpl_py as simpl

try:
    import numpy as np
except ImportError:
    raise RuntimeError("This module depends on the numpy module. Please make\
sure that it is installed properly.")


def CreateDataContainerArray():
    """
    Creates a Top level Data Container Array object. Everything will get packed in this
    """
    dca = simpl.DataContainerArray.New()
    return dca

def CreateDataContainer(name):
    """
    Creates a DataContainer object

    Keyword arguments:
    name -- The name of the DataContainer    
    """
    dc = simpl.DataContainer.New(name)
    return dc


def CreateAttributeMatrix(dims, name, type):
    """
    Creates an AttriuteMatrix of the given type

    Keyword arguments:
    dims -- the dimensions of the AttributeMatrix
    name -- The name of the AttributeMatrix
    type -- the type of the AttributeMatrix
    """
    am = simpl.AttributeMatrix.Create(dims, name, type)
    return am


def WriteDREAM3DFile(path, dca, verbose=False):
    """
    Writes a dream3d file and returns the error code

    Keyword arguments:
    path -- The output path of the file.
    dca -- The DataContainerArray to write
    """
    # Lets write out the whole Heirarchy to a .dream3d file
    if verbose:
        print("Creating DREAM3D Writer filter....")
    writer = simpl.DataContainerWriter.New()
    writer.OutputFile = (path)
    if verbose:
        print("Writing to file:", path)
    writer.setDataContainerArray(dca)
    writer.execute()
    if(writer.ErrorCondition != 0):
        print("Error %d writing DREAM.3D File to path %s" % writer.ErrorCondition, path)
    return writer.ErrorCondition


def CreateDataArray(name, shape, cDims, type):
    """
    Creates a data array with the attributes listed. The array will be a numpy array in "C" ordering.

    Keyword arguments:
    name -- The name of the DataArray
    shape -- The dimensions of the DataArray. Note that the array will be flattened into a contiguous array
    cDims -- The component dimensions, i.e., the number of components at each tuple.
    type -- The numpy type of array to create: 8,16,32,64 signed/unsiged and 32/64 floats are supported
    """
    # Create a numpy array of ones to hold our data
    num_array = np.ndarray(shape, dtype=type, order="C")

    z = np.asarray(num_array)
    if not z.flags.contiguous:
        z = np.ascontiguousarray(z)
    z.fill(0)

    shape = z.shape
    assert z.flags.contiguous, 'Only contiguous arrays are supported.'
    assert not np.issubdtype(z.dtype, np.complex128), \
            "Complex numpy arrays cannot be converted to vtk arrays."\
            "Use real() or imag() to get a component of the array before"\
            " passing it to vtk."

    # Get the Pointer to the numpy array
    z_flat = np.ravel(z)
    
    #np.info(z)
    
    # Declare the number of components for the array
    if type == np.int8:
        array = simpl.Int8ArrayType(z_flat, cDims, name, False)
    elif type == np.uint8:
        array = simpl.UInt8ArrayType(z_flat, cDims, name, False)
    elif type == np.int16:
        array = simpl.Int16ArrayType(z_flat, cDims, name, False)
    elif type == np.uint16:
        array = simpl.UInt16ArrayType(z_flat, cDims, name, False)
    elif type == np.int32:
        array = simpl.Int32ArrayType(z_flat, cDims, name, False)
    elif type == np.uint32:
        array = simpl.UInt32ArrayType(z_flat, cDims, name, False)
    elif type == np.int64:
        array = simpl.Int64ArrayType(z_flat, cDims, name, False)
    elif type == np.uint64:
        array = simpl.UInt64ArrayType(z_flat, cDims, name, False)
    elif type == np.float32:
        array = simpl.FloatArrayType(z_flat, cDims, name, False)
    elif type == np.double:
        array = simpl.DoubleArrayType(z_flat, cDims, name, False)     
    
    # we need to return the 'z' numpy array so it does not go out of scope.
    return (z, array)


def ConvertToDataArray(name, array):
    """
    Converts a numpy array into a Data Array for DREAM3D.
    \nKeyword arguments:
    \nname:  The name of the DataArray
    \narray: The numpy array to be converted
    """
    # Make sure it is a numpy array
    array = np.array(array)
    # Get shape
    shape = array.shape
    # Determine cDims
    cDims = simpl.VectorSizeT([1])  # Default
    dimensions = []
    arraySize = np.size(array)
    if arraySize > 0 and np.size(array[0]) > 1:
        for i in range(0, arraySize):
            dimensions.append(np.size(array[i]))
        cDims = simpl.VectorSizeT(dimensions)

    # Determine type
    type = array.dtype
    # Make sure it is contiguous
    z = np.asarray(array)
    if not z.flags.contiguous:
        z = np.ascontiguousarray(z)

    shape = z.shape
    assert z.flags.contiguous, 'Only contiguous arrays are supported.'
    assert not np.issubdtype(z.dtype, np.complex128), \
        'Complex numpy arrays cannot be converted to vtk arrays.' \
        'Use real() or imag() to get a component of the array before' \
        ' passing it to vtk.'

    # Flatten
    z_flat = np.ravel(z)

    # Declare the number of components for the array
    if type == np.int8:
        da = simpl.Int8ArrayType(z_flat, cDims, name, False)
    elif type == np.uint8:
        da = simpl.UInt8ArrayType(z_flat, cDims, name, False)
    elif type == np.int16:
        da = simpl.Int16ArrayType(z_flat, cDims, name, False)
    elif type == np.uint16:
        da = simpl.UInt16ArrayType(z_flat, cDims, name, False)
    elif type == np.int32:
        da = simpl.Int32ArrayType(z_flat, cDims, name, False)
    elif type == np.uint32:
        da = simpl.UInt32ArrayType(z_flat, cDims, name, False)
    elif type == np.int64:
        da = simpl.Int64ArrayType(z_flat, cDims, name, False)
    elif type == np.uint64:
        da = simpl.UInt64ArrayType(z_flat, cDims, name, False)
    elif type == np.float32:
        da = simpl.FloatArrayType(z_flat, cDims, name, False)
    elif type == np.double:
        da = simpl.DoubleArrayType(z_flat, cDims, name, False)

    # we need to return the 'z' numpy array so it does not go out of scope.
    return (z, da)

def CreateGeometry(data_container_array, array_handling, geometry_type, data_container_name, treat_warnings_as_errors, dimensions = simpl.IntVec3(0,0,0),
origin = simpl.FloatVec3(0,0,0), resolution = simpl.FloatVec3(0,0,0), cell_attribute_matrix_name="", x_bounds_array_path = simpl.DataArrayPath("", "", ""),
y_bounds_array_path = simpl.DataArrayPath("", "", ""), z_bounds_array_path = simpl.DataArrayPath("", "", ""), 
shared_vertex_list_array_path = simpl.DataArrayPath("", "", ""), vertex_attribute_matrix_name = "", shared_edge_list_array_path = simpl.DataArrayPath("", "", ""),
edge_attribute_matrix_name = "", shared_tri_list_array_path = simpl.DataArrayPath("", "", ""), face_attribute_matrix_name="",
shared_quad_list_array_path = simpl.DataArrayPath("", "", ""), shared_tet_list_array_path = simpl.DataArrayPath("", "", "")):
    """
    Create Geometry

    Required inputs: 
    * Data Container Array
    * Array Handling
    * Geometry Type [simpl.IGeometry.Type.*  (where * is Image, RectGrid, Vertex, Edge, Triangle, Quad, Tetrahedral)]
    * Data Container Name
    * Treat Warnings As Errors

    Options by Geometry Type
    * Image
        * Dimensions
        * Origin
        * Resolution
        * Cell Attribute Matrix Name
    * Rectilinear grid
        * X Bounds
        * Y Bounds
        * Z Bounds
        * Cell Attribute Matrix Name
    * Vertex
        * Vertex List
        * Vertex Attribute Matrix Name
    * Edge
        * Shared Vertex List
        * Edge List
        * Vertex Attribute Matrix Name
        * Edge Attribute Matrix Name
    * Triangle
        * Shared Vertex List
        * Triangle List
        * Vertex Attribute Matrix Name
        * Face Attribute Matrix Name
    * Quadrilateral
        * Shared Vertex List
        * Quadrilateral List
        * Vertex Attribute Matrix Name
        * Face Attribute Matrix Name
    * Tetrahedral
        * Shared Vertex List
        * Tetrahedral List
        * Vertex Attribute Matrix Name
        * Cell Attribute Matrix Name

    """

    # For all geometry types
    create_geometry = simpl.CreateGeometry.New()
    create_geometry.setDataContainerArray(data_container_array)
    create_geometry.DataContainerName = data_container_name
    create_geometry.GeometryType = geometry_type
    create_geometry.TreatWarningsAsErrors = treat_warnings_as_errors
    create_geometry.ArrayHandling = array_handling

	# Set required parameters based on Geometry Type
    if geometry_type == simpl.IGeometry.Type.Image:     
        create_geometry.Dimensions = dimensions
        create_geometry.Origin = origin
        create_geometry.Resolution = resolution
        create_geometry.ImageCellAttributeMatrixName = cell_attribute_matrix_name
    elif geometry_type == simpl.IGeometry.Type.RectGrid:
        create_geometry.XBoundsArrayPath = x_bounds_array_path
        create_geometry.YBoundsArrayPath = y_bounds_array_path
        create_geometry.ZBoundsArrayPath = z_bounds_array_path
        create_geometry.RectGridCellAttributeMatrixName = cell_attribute_matrix_name
    elif geometry_type == simpl.IGeometry.Type.Vertex:
        create_geometry.SharedVertexListArrayPath0 = shared_vertex_list_array_path
        create_geometry.VertexAttributeMatrixName0 = vertex_attribute_matrix_name
    elif geometry_type == simpl.IGeometry.Type.Edge:
        create_geometry.SharedVertexListArrayPath1 = shared_vertex_list_array_path
        create_geometry.VertexAttributeMatrixName1 = vertex_attribute_matrix_name
        create_geometry.SharedEdgeListArrayPath = shared_edge_list_array_path
        create_geometry.EdgeAttributeMatrixName = edge_attribute_matrix_name
    elif geometry_type == simpl.IGeometry.Type.Triangle:
        create_geometry.SharedVertexListArrayPath2 = shared_vertex_list_array_path
        create_geometry.VertexAttributeMatrixName2 = vertex_attribute_matrix_name
        create_geometry.SharedTriListArrayPath = shared_tri_list_array_path
        create_geometry.FaceAttributeMatrixName0 = face_attribute_matrix_name
    elif geometry_type == simpl.IGeometry.Type.Quad:
        create_geometry.SharedVertexListArrayPath3 = shared_vertex_list_array_path
        create_geometry.VertexAttributeMatrixName3 = vertex_attribute_matrix_name
        create_geometry.SharedQuadListArrayPath = shared_quad_list_array_path
        create_geometry.FaceAttributeMatrixName1 = face_attribute_matrix_name
    elif geometry_type == simpl.IGeometry.Type.Tetrahedral:
        create_geometry.SharedVertexListArrayPath4 = shared_vertex_list_array_path
        create_geometry.VertexAttributeMatrixName4 = vertex_attribute_matrix_name
        create_geometry.SharedTetListArrayPath = shared_tet_list_array_path
        create_geometry.TetCellAttributeMatrixName = cell_attribute_matrix_name

    # Execute the filter and return the error condition
    create_geometry.execute()
    executeError = create_geometry.ErrorCondition
    return executeError

def CreateAsciiWizardData(input_file_path, begin_index, number_of_lines, delimiters, consecutive_delimiters,
automatic_AM, selected_path, headers, attribute_matrix_type, tuple_dimensions, data_types):
    """
    Creates an ASCII Wizard Data object for use in the ReadASCIIData filter
    \ninput_file_path: a string containing the path to the input file
    \nbegin_index: an integer for the line number to start reading in the file
    \nnumber_of_lines: an integer of the number of lines to read
    \ndelimiters: a list of characters to parse file with (ie. [',', ':'])
    \nconsecutive_delimiters: a boolean for whether to treat consecutive delimiters as one
    \nautomatic_AM: a boolean for whether to create a new attribute matrix
    \nselected_path: a DataArrayPath object for the input data
    \nheaders: a list of strings containing header names (ie. ["x", "y", "z"])
    \nattribute_matrix_type: either an AttributeMatrix.Type object or the integer equivalent
    \ntuple_dimensions: a list of integers for the dimensions of each tuple
    \ndata_types: a list of strings for the data types (ie. ["float", "float", "float"])
    """
    wizardData = {
    "inputFilePath": input_file_path,
    "beginIndex": begin_index,
    "numberOfLines": number_of_lines,
    "delimiters": delimiters,    
    "consecutiveDelimiters": consecutive_delimiters,
    "automaticAM": automatic_AM,
    "selectedPath": selected_path,
    "headers": headers,
    "attrMatType": attribute_matrix_type,
    "tupleDimensions": tuple_dimensions,
    "dataTypes": data_types
    }
    return wizardData

def CreateDynamicTableData(data):
    """
    Creates a DynamicTableData object for use in creation of attribute matrices
    \ndata: a list of lists of integer values for the tuple dimensions of the attribute matrix
    """
    cols = len(data[0])
    rows = len(data)
    columnheaders = []
    rowheaders = []
    for i in range(cols):
        columnheaders.append(str(i))
    for j in range(rows):
        rowheaders.append(str(j))
    
    # Convert list of lists into list of VectorDoubles
    tabledata = list()
    for row in data:
        newRow = simpl.VectorDouble()
        for col in row:
            newRow.append(col)
        tabledata.append(newRow)
    dtd = simpl.DynamicTableData(tabledata, columnheaders, rowheaders)
    return dtd

    