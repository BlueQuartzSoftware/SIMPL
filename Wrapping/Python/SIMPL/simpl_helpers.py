
from dream3d import simpl
from dream3d import simplpy as d3d
#from dream3d import syntheticbuildingpy as synthetic_building
from enum import IntEnum

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
    dc = simpl.DataContainer(name)
    return dc


def CreateAttributeMatrix(dims, name, type):
    """
    Creates an AttriuteMatrix of the given type

    Keyword arguments:
    dims -- the dimensions of the AttributeMatrix
    name -- The name of the AttributeMatrix
    type -- the type of the AttributeMatrix
    """
    am = simpl.AttributeMatrix.New(dims, name, type)
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
    if(writer.ErrorCode != 0):
        print("Error %d writing DREAM.3D File to path %s" % writer.ErrorCode, path)
    return writer.ErrorCode


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


def ConvertToDataArray(name, array, componentDimensions = 1):
    """
    Converts a numpy array into a Data Array for DREAM3D.
    \nKeyword arguments:
    \nname:  The name of the DataArray
    \narray: The numpy array to be converted
    """
    # Make sure it is a numpy array
    # array = np.asanyarray(array)
    # Get shape
    shape = array.shape
    # Determine cDims
    cDims = simpl.VectorSizeT([componentDimensions])  # Default is 1

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
    return (z_flat, da)


def CreateDataContainerProxy(dca, data_array_paths):
    """
    Create a DataContainerProxy
    \ndca: Data Container Array
    \ndata_array_paths: list of DataArrayPaths (or list/tuple equivalent) to be "checked"
    """
    dcap = simpl.DataContainerArrayProxy()
    checked = 2 # The flag for "checking" an item
    for data_array_path in data_array_paths:
        data_container_name = ""
        attr_matrix_name = ""
        data_array_name = ""
        if isinstance(data_array_path, (list, tuple)):
            data_container_name = data_array_path[0]
            attr_matrix_name = data_array_path[1]
            data_array_name = data_array_path[2]
        if isinstance(data_array_path, simpl.DataArrayPath):
            data_container_name = data_array_path.DataContainerName
            attr_matrix_name = data_array_path.AttributeMatrixName
            data_array_name = data_array_path.DataArrayName
        if data_container_name == "":
            continue
        elif attr_matrix_name == "":
            dcap.getDataContainerProxy(data_container_name).Flag =checked
        elif data_array_name == "":
            dcap.getDataContainerProxy(data_container_name).getAttributeMatrixProxy(attr_matrix_name).Flag =checked
        else:
            dcap.getDataContainerProxy(data_container_name).getAttributeMatrixProxy(attr_matrix_name) \
            .getDataArrayProxy(data_array_name).Flag =checked
    return dcap


def CreateGeometry(data_container_array, array_handling, geometry_type, data_container_name, treat_warnings_as_errors, dimensions = simpl.IntVec3Type([0,0,0]),
origin = simpl.FloatVec3Type([0,0,0]), spacing = simpl.FloatVec3Type([0,0,0]), cell_attribute_matrix_name="", x_bounds_array_path = simpl.DataArrayPath("", "", ""),
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
    create_geometry.DataContainerName = simpl.DataArrayPath(data_container_name, "", "")
    create_geometry.GeometryType = geometry_type
    create_geometry.TreatWarningsAsErrors = treat_warnings_as_errors

    # Array Handling
    if not type(array_handling) == ArrayHandling and is_number(array_handling):
        create_geometry.ArrayHandling = array_handling
    elif array_handling == ArrayHandling.CopyArrays or "copy" in array_handling.toLowerCase():
        create_geometry.ArrayHandling = 0
    elif  array_handling == ArrayHandling.MoveArrays or "move" in array_handling.toLowerCase():
        create_geometry.ArrayHandling = 1        

	# Set required parameters based on Geometry Type
    if geometry_type == simpl.IGeometry.Type.Image:     
        create_geometry.Dimensions = dimensions
        create_geometry.Origin = origin
        create_geometry.Spacing= spacing
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
    executeError = create_geometry.ErrorCode
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


def CreateDynamicTableData(data, columnheaders = [], rowheaders = []):
    """
    Creates a DynamicTableData object for use in creation of attribute matrices
    \ndata: a list of lists of integer values for the tuple dimensions of the attribute matrix
    \ncolumnheaders: a list of the column headers for the data (optional)
    \nrowheaders: a list of the row headers for the data (optional)
    """
    if not columnheaders:
        cols = len(data[0])
        for i in range(cols):
            columnheaders.append(str(i))
    if not rowheaders:
        rows = len(data)
        for j in range(rows):
            rowheaders.append(str(j))
    
    tabledata = list()
    if isinstance(data, simpl.VectorDouble):
        dtd = simpl.DynamicTableData(data, columnheaders, rowheaders)
        return dtd
    # Convert list of lists into list of VectorDoubles
    else:
        for row in data:
            newRow = simpl.VectorDouble()
            for col in row:
                newRow.append(col)
            tabledata.append(newRow)
        dtd = simpl.DynamicTableData(tabledata, columnheaders, rowheaders)        
        return dtd

    
def RemoveArray(dca, path):
    """
    Removes the array given by 'path'
    \ndca: Data Container Array containing the array to be removed
    \npath: Either a DataArrayPath object or a list of 3 strings (data container, attribute matrix name, array name)
    \nReturns a boolean variable of True if successful, False otherwise
    """
    # Validate path object and get names of path elements
    datacontainername = ""
    attrmatrixname = ""
    dataarrayname = ""
    if isinstance(path, simpl.DataArrayPath):
        datacontainername = path.DataContainerName
        attrmatrixname = path.AttributeMatrixName
        dataarrayname = path.DataArrayName
    elif isinstance(path, tuple) or isinstance(path, list):
        if len(path) != 3:
            return False
        datacontainername = path[0]
        attrmatrixname = path[1]
        dataarrayname = path[2]
    else:
        print("Invalid path object")
        return False
    
    # Construct Data Container Array Proxy
    dcap = simpl.DataContainerArrayProxy()
    if datacontainername == "":
        return False
    elif attrmatrixname == "" and dataarrayname == "":
        dcap.getDataContainerProxy(datacontainername).Flag =2
    elif dataarrayname == "":
        dcap.getDataContainerProxy(datacontainername).getAttributeMatrixProxy(attrmatrixname).Flag =2
    else:
        dcap.getDataContainerProxy(datacontainername).Flag =0
        dcap.getDataContainerProxy(datacontainername).getAttributeMatrixProxy(attrmatrixname).Flag =0
        dcap.getDataContainerProxy(datacontainername).getAttributeMatrixProxy(attrmatrixname).getDataArrayProxy(dataarrayname).Flag =2

    err = d3d.remove_arrays(dca, dcap)
    if err < 0:
        print("Error condition for Remove Arrays: %d" % err)
        return False
    else:      
        return True


def RemoveArrays(dca, paths):
    """
    Remove the list of data array paths
    \ndca: Data Container Array containing the arrays to be removed
    \npaths: List of either DataArrayPath objects or of lists of 3 strings (data container, attribute matrix name, array name)
    \nReturns a boolean variable of True if successful, False otherwise
    """
    success = True

    for path in paths:
        if not RemoveArray(dca, path):
            success = False
    
    return success

def MultiThresholdObjects(dca, destination_array_name, selected_thresholds):
    """
    Create and run a Multi Threshold Objects filter
    \ndca: Data Container Array containing the arrays
    \ndestination_array_name: The name of the destination array for storing the results
    \nselected_thresholds: A list of tuples containing the Data Container Name, Attribute Matrix Name, Data Array Name,  a Comparison Operator, and a Comparison Value
    \n The comparison operator choices are ["<", ">", "=", "!="] (or their index in this list)
    \n The comparison value is an double floating point number to compare the values with. 
    """
    comparison_operators = ["<", ">", "=", "!="]
    if len(selected_thresholds) == 0:
        print("Insufficient data passed")
        return
    thresholds = simpl.ComparisonInputs()
    for selected_threshold in selected_thresholds:
        if len(selected_threshold) < 5:
            print("Selected threshold has insufficient data")
            continue
        datacontainername = selected_threshold[0]
        attrmatrixname = selected_threshold[1]
        dataarrayname = selected_threshold[2]
        comparison_operator = selected_threshold[3]
        if not is_number(comparison_operator):
            comparison_operator = comparison_operators.index(selected_threshold[3])
            if comparison_operator < 0:
                print("Invalid comparison operator passed in selected threshold")
                continue
        elif comparison_operator < 0 or comparison_operator > (len(comparison_operators) - 1):
            print("Invalid comparison operator passed in selected threshold")
            continue
        comparison_value = selected_threshold[4]
        if not is_number(comparison_value):
            print("Non-numerical value passed for comparison value")
        thresholds.addInput(datacontainername, attrmatrixname, dataarrayname, comparison_operator, comparison_value)
    
    err = d3d.multi_threshold_objects(dca, destination_array_name, thresholds)
    return err


def MultiThresholdObjects2(dca, source_path, destination_array_name, selected_thresholds):
    """
    Create and run a Multi Threshold Objects filter
    \ndca: Data Container Array containing the arrays
    \nsource_path: Data Array Path or tuple/list of 3 strings representing a Data Array Path
    \ndestination_array_name: The name of the destination array for storing the results
    \nselected_thresholds: A list of Comparison Sets that has a DataContainerName, AttributeMatrixName, and a Union Operator (AND, OR)
    \nA list of tuples containing the AttributeArrayName, a CompOperator, and a CompValue
    \nThe comparison operator choices are ["<", ">", "=", "!="] (or their index in this list)
    \nThe comparison value is an double floating point number to compare the values with. 
    \nNOTE: Comparison Sets can contain Comparison Sets or Values but Comparison Values cannot contain either
    \n Place the UnionOperator in front of the Comparison Values in a a Comparison Set
    \n Comparison Sets should be lists in [] form and Comparison Values should be tuples in () form
    \nExample:
    \n["AND", (CV1), (CV2), ["OR", (CV3, (CV4)]] ==> CV1 && CV2 && (CV3 OR CV4)
    """
    thresholds = simpl.ComparisonInputsAdvanced()
    topLevelComparisonSet = simpl.ComparisonSet()
    for selected_threshold in selected_thresholds:
        if selected_threshold == "AND":
            topLevelComparisonSet.UnionOperator = 0
        elif selected_threshold == "OR":
            topLevelComparisonSet.UnionOperator = 1
        if isinstance(selected_threshold, tuple):
            comparisonValue = simpl.ComparisonValue()
            comparisonValue.AttributeArrayName = selected_threshold[0]
            comparisonValue.CompOperator = selected_threshold[1]
            comparisonValue.CompValue = selected_threshold[2]
            topLevelComparisonSet.addComparison(comparisonValue)
        if isinstance(selected_threshold, list):
            comparisonSet = BuildSelectedThresholds(selected_threshold)
            topLevelComparisonSet.addComparison(comparisonSet)
    thresholds.addInput(topLevelComparisonSet)
    if isinstance(source_path, (tuple, list)):
        thresholds.DataContainerName = source_path[0]
        thresholds.AttributeMatrixName = source_path[1]
    elif isinstance(source_path, simpl.DataArrayPath):
        thresholds.DataContainerName = source_path.DataContainerName
        thresholds.AttributeMatrixName = source_path.AttributeMatrixName
    else:
        print("Invalid source path")
        return -1        
    
    err = d3d.multi_threshold_objects2(dca, destination_array_name, thresholds)
    if err < 0:
        print("MultiThresholdObjects ErrorCode: %d" % err)
    return err


def BuildSelectedThresholds(selected_threshold):
    """
    Recursive function to build a Comparison Set
    \nselected_threshold: A nested Comparison Set to be built
    """
    # AND = 0 / OR = 1
    newComparisonSet = simpl.ComparisonSet()
    if selected_threshold[0] == "AND":
        newComparisonSet.UnionOperator = 0
    elif selected_threshold[0] == "OR":
        newComparisonSet.UnionOperator = 1
    for element in selected_threshold:
        if isinstance(element, tuple):
            comparisonValue = simpl.ComparisonValue()
            comparisonValue.AttributeArrayName = element[0]
            comparisonValue.CompOperator = element[1]
            comparisonValue.CompValue = element[2]
            newComparisonSet.addComparison(comparisonValue)
        if isinstance(element, list):
            comparisonSet = BuildSelectedThresholds(element)
            newComparisonSet.addComparison(comparisonSet) 
    return newComparisonSet


def MoveData(dca, what_to_move, source_path, destination_path):
    """
    Move the data from source to destination
    \ndca: Data Container Array containing project data
    \nwhat_to_move: Either "Attribute Matrix" or "Data Array" (or the actual type such as simpl.AttributeMatrix or simpl.IDataArray)
    \nsource_path: The full Data Array Path to the object being moved (or a list/tuple of strings)
    \ndestination: The full Data Array Path where the object is moved to (or a list/tuple of strings)
    """
    what_to_move_number = 0
    if not isinstance(what_to_move, (simpl.AttributeMatrix, simpl.IDataArray)):
        if what_to_move == "Data Array" or what_to_move == "Attribute Array" or what_to_move == WhatToMove.AttributeArray:
            what_to_move_number = 1
        elif what_to_move != "Attribute Matrix" and what_to_move != WhatToMove.AttributeMatrix:
            print("Invalid type for 'what to move'")
            return -1
    if not isinstance(source_path, (simpl.DataArrayPath, list, tuple)):
        print("Invalid source path")
        return -1
    if not isinstance(destination_path, (simpl.DataArrayPath, list, tuple)):
        print("Invalid destination path")
        return -1
    
    destination_datacontainername = ""
    if isinstance(destination_path, simpl.DataArrayPath):
        destination_datacontainername = destination_path.DataContainerName
    else:
        destination_datacontainername = destination_path[0]
    destination_attrmatrixname = ""
    if isinstance(destination_path, simpl.DataArrayPath):
        destination_attrmatrixname = destination_path.AttributeMatrixName
    else:
        destination_attrmatrixname = destination_path[1]
    destination_dataarrayname = ""
    if isinstance(destination_path, simpl.DataArrayPath):
        destination_dataarrayname = destination_path.DataArrayName
    else:
        destination_dataarrayname = destination_path[2]
    
    source_datacontainername = ""
    if isinstance(source_path, simpl.DataArrayPath):
        source_datacontainername = source_path.DataContainerName
    else:
        source_datacontainername = source_path[0]
    source_attrmatrixname = ""
    if isinstance(source_path, simpl.DataArrayPath):
        source_attrmatrixname = source_path.AttributeMatrixName
    else:
        source_attrmatrixname = source_path[1]
    source_dataarrayname = ""
    if isinstance(source_path, simpl.DataArrayPath):
        source_dataarrayname = source_path.DataArrayName
    else:
        source_dataarrayname = source_path[2]

    err = d3d.move_data(dca, what_to_move_number, destination_datacontainername, simpl.DataArrayPath(source_datacontainername, source_attrmatrixname, ""),
    simpl.DataArrayPath(destination_datacontainername, destination_attrmatrixname, ""), simpl.DataArrayPath(source_datacontainername, source_attrmatrixname, source_dataarrayname))
    if err < 0:
        print("MoveData ErrorCode %d: " % err)
    return err
    

def MoveMultiData(dca, what_to_move, source_paths, destination_path):
    """
    Moves multiple data arrays or attribute matrices to the destination path
    \ndca: Data Container Array with the data in it
    \nwhat_to_move: Either "Attribute Matrix" or "Data Array" (or the actual type such as simpl.AttributeMatrix or simpl.IDataArray)
    \nsource_paths: A list of full Data Array Paths to the objects being moved (or a list of list/tuple of strings)
    \ndestination: The full Data Array Path where the object is moved to (or a list/tuple of strings)
    """
    if len(source_paths) < 1:
        print("Invalid or insufficient source paths")
        return -1
    err = 0
    for source_path in source_paths:
        err = MoveData(dca, what_to_move, source_path, destination_path)
    return err

def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        pass
 
    try:
        import unicodedata
        unicodedata.numeric(s)
        return True
    except (TypeError, ValueError):
        pass
 
    return False


# Custom enumerations for Python

class ArrayHandling(IntEnum):
    CopyArrays = 0
    MoveArrays = 1


class FeatureGeneration(IntEnum):
    GenerateFeatures = 0
    AlreadyHaveFeatures = 1


class SaveShapeDescArrays(IntEnum):
    DoNotSave = 0
    SaveToNewAttrMatrix = 1
    AppendToExistingAttrMatrix = 2


class Delimiter(IntEnum):
    COMMA = 0
    SEMICOLON = 1
    COLON = 2
    TAB = 3


class Hemisphere(IntEnum):
    Northern = 0
    Southern = 1


class WhatToMove(IntEnum):
    AttributeMatrix = 0
    AttributeArray = 1


class AngleRepresentation(IntEnum):
    Radians = 0
    Degrees = 1
    Invalid = 2


class BadDataOperation(IntEnum):
    Dilate = 0
    Erode = 1


class ReferenceOrientation(IntEnum):
    AverageOrientation = 0
    OrientationAtFeatureCentroid = 1


class DistributionFitType(IntEnum):
    Beta = 0
    Lognormal = 1
    Power = 2

	
class ObjectToCopy(IntEnum):
    DataContainer = 0
    AttributeMatrix = 1
    AttributeArray = 2
	