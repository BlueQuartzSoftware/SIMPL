try:
    import dream3d.simpl as simpl
    import dream3d.simplpy as d3d
except ImportError:
    import simpl
    import simplpy as d3d

import numpy as np

from enum import IntEnum

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

def ReadDREAM3DFile(data_container_array, input_file):
    """
    Executes the filter DataContainerReader and returns the error. This will read the entire
    file into memory.

    :param DataContainerArray data_container_array: The DataContainerArray that the filter will use.
    :param QString input_file: Sets the InputFile value.
    :return: ErrorCode produced by the filter
    :rtype: int
    """
    
    # Create a DataContainerReader and read the DataStructure into a proxy
    data_container_reader = simpl.DataContainerReader.New()
    dcaProxy = data_container_reader.readDataContainerArrayStructure(input_file)

    # Now that we have our DataContainerProxy, read the file
    data_container_reader = simpl.DataContainerReader.New()
    data_container_reader.setDataContainerArray(data_container_array)
    data_container_reader.InputFile = input_file
    data_container_reader.OverwriteExistingDataContainers = True
    data_container_reader.InputFileDataContainerArrayProxy = dcaProxy
    data_container_reader.execute()
    executeError = data_container_reader.ErrorCode
    return executeError

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
        print('Creating DREAM3D Writer filter....')
    writer = simpl.DataContainerWriter.New()
    writer.OutputFile = (path)
    if verbose:
        print('Writing to file:', path)
    writer.setDataContainerArray(dca)
    writer.execute()
    if(writer.ErrorCode != 0):
        print('Error %d writing DREAM.3D File to path %s' % writer.ErrorCode, path)
    return writer.ErrorCode

def CreateDataArray(name, shape, cDims, type):
    """
    Creates a data array with the attributes listed. The array will be a numpy array in 'C' ordering.

    Keyword arguments:
    name -- The name of the DataArray
    shape -- The dimensions of the DataArray. Note that the array will be flattened into a contiguous array
    cDims -- The component dimensions, i.e., the number of components at each tuple.
    type -- The numpy type of array to create: 8,16,32,64 signed/unsiged and 32/64 floats are supported
    """
    # Create a numpy array of ones to hold our data
    n_tuples = np.prod(shape)

    # Declare the number of components for the array
    if type == np.int8:
        array = simpl.Int8Array(n_tuples, cDims, name, 0)
    elif type == np.uint8:
        array = simpl.UInt8Array(n_tuples, cDims, name, 0)
    elif type == np.int16:
        array = simpl.Int16Array(n_tuples, cDims, name, 0)
    elif type == np.uint16:
        array = simpl.UInt16Array(n_tuples, cDims, name, 0)
    elif type == np.int32:
        array = simpl.Int32Array(n_tuples, cDims, name, 0)
    elif type == np.uint32:
        array = simpl.UInt32Array(n_tuples, cDims, name, 0)
    elif type == np.int64:
        array = simpl.Int64Array(n_tuples, cDims, name, 0)
    elif type == np.uint64:
        array = simpl.UInt64Array(n_tuples, cDims, name, 0)
    elif type == np.float32:
        array = simpl.FloatArray(n_tuples, cDims, name, 0.0)
    elif type == np.double:
        array = simpl.DoubleArray(n_tuples, cDims, name, 0.0)

    return (array.npview(), array)

def CreateDataContainerProxy(dca, data_array_paths):
    """
    Create a DataContainerProxy
    dca: Data Container Array
    data_array_paths: list of DataArrayPaths (or list/tuple equivalent) to be 'checked'
    """
    dcap = simpl.DataContainerArrayProxy()
    checked = 2 # The flag for 'checking' an item
    for data_array_path in data_array_paths:
        data_container_name = ''
        attr_matrix_name = ''
        data_array_name = ''
        if isinstance(data_array_path, (list, tuple)):
            data_container_name = data_array_path[0]
            attr_matrix_name = data_array_path[1]
            data_array_name = data_array_path[2]
        if isinstance(data_array_path, simpl.DataArrayPath):
            data_container_name = data_array_path.DataContainerName
            attr_matrix_name = data_array_path.AttributeMatrixName
            data_array_name = data_array_path.DataArrayName
        if data_container_name == '':
            continue
        elif attr_matrix_name == '':
            dcap.getDataContainerProxy(data_container_name).Flag =checked
        elif data_array_name == '':
            dcap.getDataContainerProxy(data_container_name).getAttributeMatrixProxy(attr_matrix_name).Flag =checked
        else:
            dcap.getDataContainerProxy(data_container_name).getAttributeMatrixProxy(attr_matrix_name) \
            .getDataArrayProxy(data_array_name).Flag =checked
    return dcap

def CreateGeometry(data_container_array, array_handling, geometry_type, data_container_name, treat_warnings_as_errors, dimensions = simpl.IntVec3([0,0,0]),
origin = simpl.FloatVec3([0,0,0]), spacing = simpl.FloatVec3([0,0,0]), cell_attribute_matrix_name='', x_bounds_array_path = simpl.DataArrayPath('', '', ''),
y_bounds_array_path = simpl.DataArrayPath('', '', ''), z_bounds_array_path = simpl.DataArrayPath('', '', ''), 
shared_vertex_list_array_path = simpl.DataArrayPath('', '', ''), vertex_attribute_matrix_name = '', shared_edge_list_array_path = simpl.DataArrayPath('', '', ''),
edge_attribute_matrix_name = '', shared_tri_list_array_path = simpl.DataArrayPath('', '', ''), face_attribute_matrix_name='',
shared_quad_list_array_path = simpl.DataArrayPath('', '', ''), shared_tet_list_array_path = simpl.DataArrayPath('', '', '')):
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
    create_geometry.DataContainerName = simpl.DataArrayPath(data_container_name, '', '')
    create_geometry.GeometryType = geometry_type
    create_geometry.TreatWarningsAsErrors = treat_warnings_as_errors

    # Array Handling
    if not type(array_handling) == ArrayHandling and is_number(array_handling):
        create_geometry.ArrayHandling = array_handling
    elif array_handling == ArrayHandling.CopyArrays or 'copy' in array_handling.toLowerCase():
        create_geometry.ArrayHandling = 0
    elif  array_handling == ArrayHandling.MoveArrays or 'move' in array_handling.toLowerCase():
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
    input_file_path: a string containing the path to the input file
    begin_index: an integer for the line number to start reading in the file
    number_of_lines: an integer of the number of lines to read
    delimiters: a list of characters to parse file with (ie. [',', ':'])
    consecutive_delimiters: a boolean for whether to treat consecutive delimiters as one
    automatic_AM: a boolean for whether to create a new attribute matrix
    selected_path: a DataArrayPath object for the input data
    headers: a list of strings containing header names (ie. ['x', 'y', 'z'])
    attribute_matrix_type: either an AttributeMatrix.Type object or the integer equivalent
    tuple_dimensions: a list of integers for the dimensions of each tuple
    data_types: a list of strings for the data types (ie. ['float', 'float', 'float'])
    """
    wizardData = {
    'inputFilePath': input_file_path,
    'beginIndex': begin_index,
    'numberOfLines': number_of_lines,
    'delimiters': delimiters,    
    'consecutiveDelimiters': consecutive_delimiters,
    'automaticAM': automatic_AM,
    'selectedPath': selected_path,
    'headers': headers,
    'attrMatType': attribute_matrix_type,
    'tupleDimensions': tuple_dimensions,
    'dataTypes': data_types
    }
    return wizardData

def CreateDynamicTableData(data, columnheaders = [], rowheaders = []):
    """
    Creates a DynamicTableData object for use in creation of attribute matrices
    data: a list of lists of integer values for the tuple dimensions of the attribute matrix
    columnheaders: a list of the column headers for the data (optional)
    rowheaders: a list of the row headers for the data (optional)
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
    dca: Data Container Array containing the array to be removed
    path: Either a DataArrayPath object or a list of 3 strings (data container, attribute matrix name, array name)
    Returns a boolean variable of True if successful, False otherwise
    """
    # Validate path object and get names of path elements
    datacontainername = ''
    attrmatrixname = ''
    dataarrayname = ''
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
        print('Invalid path object')
        return False
    
    # Construct Data Container Array Proxy
    dcap = simpl.DataContainerArrayProxy()
    if datacontainername == '':
        return False
    elif attrmatrixname == '' and dataarrayname == '':
        dcap.getDataContainerProxy(datacontainername).Flag =2
    elif dataarrayname == '':
        dcap.getDataContainerProxy(datacontainername).getAttributeMatrixProxy(attrmatrixname).Flag =2
    else:
        dcap.getDataContainerProxy(datacontainername).Flag =0
        dcap.getDataContainerProxy(datacontainername).getAttributeMatrixProxy(attrmatrixname).Flag =0
        dcap.getDataContainerProxy(datacontainername).getAttributeMatrixProxy(attrmatrixname).getDataArrayProxy(dataarrayname).Flag =2

    err = d3d.remove_arrays(dca, dcap)
    if err < 0:
        print('Error condition for Remove Arrays: %d' % err)
        return False
    else:      
        return True

def RemoveArrays(dca, paths):
    """
    Remove the list of data array paths
    dca: Data Container Array containing the arrays to be removed
    paths: List of either DataArrayPath objects or of lists of 3 strings (data container, attribute matrix name, array name)
    Returns a boolean variable of True if successful, False otherwise
    """
    success = True

    for path in paths:
        if not RemoveArray(dca, path):
            success = False
    
    return success

def MultiThresholdObjects(dca, destination_array_name, selected_thresholds):
    """
    Create and run a Multi Threshold Objects filter
    dca: Data Container Array containing the arrays
    destination_array_name: The name of the destination array for storing the results
    selected_thresholds: A list of tuples containing the Data Container Name, Attribute Matrix Name, Data Array Name,  a Comparison Operator, and a Comparison Value
     The comparison operator choices are ['<', '>', '=', '!='] (or their index in this list)
     The comparison value is an double floating point number to compare the values with. 
    """
    comparison_operators = ['<', '>', '=', '!=']
    if len(selected_thresholds) == 0:
        print('Insufficient data passed')
        return
    thresholds = simpl.ComparisonInputs()
    for selected_threshold in selected_thresholds:
        if len(selected_threshold) < 5:
            print('Selected threshold has insufficient data')
            continue
        datacontainername = selected_threshold[0]
        attrmatrixname = selected_threshold[1]
        dataarrayname = selected_threshold[2]
        comparison_operator = selected_threshold[3]
        if not is_number(comparison_operator):
            comparison_operator = comparison_operators.index(selected_threshold[3])
            if comparison_operator < 0:
                print('Invalid comparison operator passed in selected threshold')
                continue
        elif comparison_operator < 0 or comparison_operator > (len(comparison_operators) - 1):
            print('Invalid comparison operator passed in selected threshold')
            continue
        comparison_value = selected_threshold[4]
        if not is_number(comparison_value):
            print('Non-numerical value passed for comparison value')
        thresholds.addInput(datacontainername, attrmatrixname, dataarrayname, comparison_operator, comparison_value)
    
    err = d3d.multi_threshold_objects(dca, destination_array_name, thresholds)
    return err

def MultiThresholdObjects2(dca, source_path, destination_array_name, selected_thresholds):
    """
    Create and run a Multi Threshold Objects filter
    dca: Data Container Array containing the arrays
    source_path: Data Array Path or tuple/list of 3 strings representing a Data Array Path
    destination_array_name: The name of the destination array for storing the results
    selected_thresholds: A list of Comparison Sets that has a DataContainerName, AttributeMatrixName, and a Union Operator (AND, OR)
    A list of tuples containing the AttributeArrayName, a CompOperator, and a CompValue
    The comparison operator choices are ['<', '>', '=', '!='] (or their index in this list)
    The comparison value is an double floating point number to compare the values with. 
    NOTE: Comparison Sets can contain Comparison Sets or Values but Comparison Values cannot contain either
     Place the UnionOperator in front of the Comparison Values in a a Comparison Set
     Comparison Sets should be lists in [] form and Comparison Values should be tuples in () form
    Example:
    ['AND', (CV1), (CV2), ['OR', (CV3, (CV4)]] ==> CV1 && CV2 && (CV3 OR CV4)
    """
    thresholds = simpl.ComparisonInputsAdvanced()
    topLevelComparisonSet = simpl.ComparisonSet()
    for selected_threshold in selected_thresholds:
        if selected_threshold == 'AND':
            topLevelComparisonSet.UnionOperator = 0
        elif selected_threshold == 'OR':
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
        print('Invalid source path')
        return -1        
    
    err = d3d.multi_threshold_objects2(dca, destination_array_name, thresholds)
    if err < 0:
        print('MultiThresholdObjects ErrorCode: %d' % err)
    return err

def BuildSelectedThresholds(selected_threshold):
    """
    Recursive function to build a Comparison Set
    selected_threshold: A nested Comparison Set to be built
    """
    # AND = 0 / OR = 1
    newComparisonSet = simpl.ComparisonSet()
    if selected_threshold[0] == 'AND':
        newComparisonSet.UnionOperator = 0
    elif selected_threshold[0] == 'OR':
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
    dca: Data Container Array containing project data
    what_to_move: Either 'Attribute Matrix' or 'Data Array' (or the actual type such as simpl.AttributeMatrix or simpl.IDataArray)
    source_path: The full Data Array Path to the object being moved (or a list/tuple of strings)
    destination: The full Data Array Path where the object is moved to (or a list/tuple of strings)
    """
    what_to_move_number = 0
    if not isinstance(what_to_move, (simpl.AttributeMatrix, simpl.IDataArray)):
        if what_to_move == 'Data Array' or what_to_move == 'Attribute Array' or what_to_move == WhatToMove.AttributeArray:
            what_to_move_number = 1
        elif what_to_move != 'Attribute Matrix' and what_to_move != WhatToMove.AttributeMatrix:
            print('Invalid type for \'what to move\'')
            return -1
    if not isinstance(source_path, (simpl.DataArrayPath, list, tuple)):
        print('Invalid source path')
        return -1
    if not isinstance(destination_path, (simpl.DataArrayPath, list, tuple)):
        print('Invalid destination path')
        return -1
    
    destination_datacontainername = ''
    if isinstance(destination_path, simpl.DataArrayPath):
        destination_datacontainername = destination_path.DataContainerName
    else:
        destination_datacontainername = destination_path[0]
    destination_attrmatrixname = ''
    if isinstance(destination_path, simpl.DataArrayPath):
        destination_attrmatrixname = destination_path.AttributeMatrixName
    else:
        destination_attrmatrixname = destination_path[1]
    destination_dataarrayname = ''
    if isinstance(destination_path, simpl.DataArrayPath):
        destination_dataarrayname = destination_path.DataArrayName
    else:
        destination_dataarrayname = destination_path[2]
    
    source_datacontainername = ''
    if isinstance(source_path, simpl.DataArrayPath):
        source_datacontainername = source_path.DataContainerName
    else:
        source_datacontainername = source_path[0]
    source_attrmatrixname = ''
    if isinstance(source_path, simpl.DataArrayPath):
        source_attrmatrixname = source_path.AttributeMatrixName
    else:
        source_attrmatrixname = source_path[1]
    source_dataarrayname = ''
    if isinstance(source_path, simpl.DataArrayPath):
        source_dataarrayname = source_path.DataArrayName
    else:
        source_dataarrayname = source_path[2]

    err = d3d.move_data(dca, what_to_move_number, destination_datacontainername, simpl.DataArrayPath(source_datacontainername, source_attrmatrixname, ''),
    simpl.DataArrayPath(destination_datacontainername, destination_attrmatrixname, ''), simpl.DataArrayPath(source_datacontainername, source_attrmatrixname, source_dataarrayname))
    if err < 0:
        print('MoveData ErrorCode %d: ' % err)
    return err

def MoveMultiData(dca, what_to_move, source_paths, destination_path):
    """
    Moves multiple data arrays or attribute matrices to the destination path
    dca: Data Container Array with the data in it
    what_to_move: Either 'Attribute Matrix' or 'Data Array' (or the actual type such as simpl.AttributeMatrix or simpl.IDataArray)
    source_paths: A list of full Data Array Paths to the objects being moved (or a list of list/tuple of strings)
    destination: The full Data Array Path where the object is moved to (or a list/tuple of strings)
    """
    if len(source_paths) < 1:
        print('Invalid or insufficient source paths')
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
