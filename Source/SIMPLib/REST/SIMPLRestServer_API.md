# SIMPL REST Server API Documentation #

The SIMPL REST server is a proof-of-concept implementation that allows SIMPL/DREAM.3D
to be remotely executed on a different physical machine than the client is running. It is
expected in the future that the API will be added to, modified and improved.

# Configuration #

There is a .ini file that can be edited to select the host address and port that the server will 
listen for connections. Edit this file to match your system. The file is copied from the 
source directory into the binary directory during CMake configuration steps.



# API Discussion #

All endpoints accept HTTP **POST** requests where the _content-type_ is set to _application/json_.

 Some API **end points** do not require a payload where others do.

 The **ExecutePipeline** endpoint accepts an HTTP **POST** request with _content-type_ set to _multipart/form-data_.  This request type is used to execute a pipeline that requires input data files and/or produces output data files.

The request should be sent to the following URL form:

    http://[HOST ADDRESS]:[PORT]/api/v1/[END POINT]

Static web pages and files can be served up by using:

    http://[HOST ADDRESS]:[PORT]/[SESSION_ID]/Filename


|  End Point | API Version | Content Type | Payload Required |
|------------|-------------|--------------|------------------|
| ExecutePipeline | v1 | JSON or Multipart/form-data | YES |
| ListFilterParameters | v1 | JSON | YES |
| LoadedPlugins | v1 | JSON | NO |
| AvailableFilters | v1 | JSON | NO |
| NumFilters | v1 | JSON | NO |
| PluginInfo   | v1 | JSON | YES |
| PreflightPipeline | v1 | JSON | YES |


## /api/v1/LoadedPlugins ##

**Input JSON**

None

**Output JSON**

| KEY | TYPE | Notes |
|-----|-------|-------|
| Plugins | Json Array of Objects | Information about each Plugin |


## /api/v1/PluginInfo ##

**Input JSON**

| KEY | TYPE | Notes |
|-----|-------|-------|
| PluginBaseName | String | Name of a plugin |


**Output JSON**

| KEY | TYPE | Notes |
|-----|-------|-------|
| PluginName | String |  |
| Version | String |  |
| CompatibilityVersion | String |  |
| Vendor | String |  |
| URL | String |  |
| Location | String |  |
| Description | String |  |
| Copyright | String |  |
| License | String |  |


## /api/v1/NumFilters ##

**Input JSON**

None.

**Output JSON**

| KEY | TYPE | Notes |
|-----|-------|-------|
| NumFilters | int | The number of filters available in the running instance |


## /api/v1/AvailableFilters ##

**Input JSON**

None.

**Output JSON**

| KEY | TYPE | Notes |
|-----|-------|-------|
| ErrorCode | int | 0=No error, Negative=Error |
| Filters | Array of Objects | Each object has the "ClassName" and "HumanLabel" keys |

## /api/v1/ListFilterParameters ##

**Input JSON**

| KEY | TYPE | Notes |
|-----|-------|-------|
| ClassName | String | The C++ name of the filter.
| UUID | String | The UUID of the filter |

**Output JSON**

| KEY | TYPE | Notes |
|-----|-------|-------|
| ClassName |  |  |
| FilterParamerers | Array of FilterParameter JSON Objects |

**FilterParameter Json**

| KEY | TYPE | Notes |
|-----|-------|-------|
| FilterParameterName | String | C++ Name of Class  |
| FilterParameterWidget | String | The Name of the C++ Widget class that is used in the GUI |
| FilterParameterCategory | Int | Parameter=0,RequriedArray=1,CreatedArray=2,Uncategorized=3 |
| FilterParameterGroupIndex | Int | FilterParameters can be grouped together  |
| FilterParameterHumanLabel | String | The string to display to the user that describes the input |
| FilterParameterPropertyName | String | The low level name that is used to set the value into the filter |
| FilterParameterReadOnly | BOOL | Is the filter parameter read-only.  |

## /api/v1/PreflightPipeline ##

**Input JSON**

| KEY | TYPE | Notes |
|-----|-------|-------|
| Pipeline | JSON | The pipeline json as DREAM.3D would save it from the application using the DataContainerWriter class |


**Output JSON**

If there are endpoint errors:
| KEY | TYPE | Notes |
|-----|-------|-------|
| SessionID | UUID created for the pipeline | d07f05ce-1389-5f80-8eca-383564b23e28 |
| ErrorCode | INTEGER | Error code returned by the endpoint, indicating if there were any errors parsing the request and sending back a response |
| ErrorMessage | STRING | Error message returned by the endpoint, describing what error occurred while parsing the request and sending back a response |

Otherwise:
| KEY | TYPE | Notes |
|-----|-------|-------|
| Completed | BOOLEAN | Indicates whether the pipeline was completed or not |
| SessionID | UUID created for the pipeline | d07f05ce-1389-5f80-8eca-383564b23e28 |
| Warnings | ARRAY | Warning Messages generated during the preflight of the pipeline |
| Errors | ARRAY | Error messages generated during the preflight of the pipeline |

## /api/v1/ExecutePipeline ##

### JSON ###

#####Input JSON#####

| KEY | TYPE | Notes |
|----------|------------|----------|
| Pipeline | JSON | The pipeline json as DREAM.3D would save it from the application using the DataContainerWriter class |

#####Output JSON#####

If there are endpoint errors:
| KEY | TYPE | Notes |
|-----|-------|-------|
| SessionID | UUID created for the pipeline | d07f05ce-1389-5f80-8eca-383564b23e28 |
| ErrorCode | INTEGER | Error code returned by the endpoint, indicating if there were any errors parsing the request and sending back a response |
| ErrorMessage | STRING | Error message returned by the endpoint, describing what error occurred while parsing the request and sending back a response |

Otherwise:
| KEY | TYPE | Notes |
|-----|-------|-------|
| Completed | BOOLEAN | Indicates whether the pipeline was completed or not |
| SessionID | UUID created for the pipeline | d07f05ce-1389-5f80-8eca-383564b23e28 |
| Warnings | ARRAY | Warning Messages generated during the preflight of the pipeline |
| Errors | ARRAY | Error messages generated during the preflight of the pipeline |

### Multipart/form-data ###

##### Input Multipart/form-data #####

**NOTE**: Each input file's parameter name is its file path, and this exact file path must be used as an input in the Pipeline.

| PARAMETER NAME | TYPE | Notes |
|----------|------------|----------|
| Pipeline | JSON | The pipeline json as DREAM.3D would save it from the application using the DataContainerWriter class |
| PipelineMetadata | JSON | Metadata that is used to indicate which properties in the pipeline's filters contain input file paths, and which contain output file paths (this information is currently not stored in the pipeline file, but probably should be in the future) |
| [Input File 1's Path] | BINARY | Input file 1's binary data |
| [Input File 2's Path] | BINARY | Input file 2's binary data |
| . | . | . |
| . | . | . |
| [Input File N's Path] | BINARY | Input file N's binary data |


##### Output Multipart/form-data #####

**NOTE**: Each output file's parameter name is its file path.  The client can use this file path to write each output file's data to the proper location on the client's local file system.

If there are endpoint errors:
| PARAMETER NAME | TYPE | Notes |
|-----|-------|-------|
| SessionID | UUID created for the pipeline | d07f05ce-1389-5f80-8eca-383564b23e28 |
| ErrorCode | INTEGER | Error code returned by the endpoint, indicating if there were any errors parsing the request and sending back a response |
| ErrorMessage | STRING | Error message returned by the endpoint, describing what error occurred while parsing the request and sending back a response |

Otherwise:
| PARAMETER NAME | TYPE | Notes |
|-----|-------|-------|
| PipelineResponse | JSON | The pipeline's response (see PipelineResponse Json table below) |
| [Output File 1's Path] | BINARY | Output file 1's binary data |
| [Output File 2's Path] | BINARY | Output file 2's binary data |
| . | . | . |
| . | . | . |
| [Output File N's Path] | BINARY | Output file N's binary data |

##### PipelineResponse Json #####

| JSON KEY | TYPE | Notes |
|-----|-------|-------|
| Completed | BOOLEAN | Indicates whether the pipeline was completed or not |
| SessionID | UUID created for the pipeline | d07f05ce-1389-5f80-8eca-383564b23e28 |
| PipelineWarnings | ARRAY | Warning Messages generated during the execution of the pipeline |
| PipelineErrors | ARRAY | Error messages generated during the execution of the pipeline |

##### Example Multipart/form-data Request #####
POST /api/v1/ExecutePipeline HTTP/1.1
Host: 192.168.242.123:8080
Content-Type: multipart/form-data; boundary="boundary_.oOo._Up6iGDNVfLS8dtXu4gmiozmowqzoLN7Z"
MIME-Version: 1.0
Content-Length: 14537
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: en-US,*
User-Agent: Mozilla/5.0

--boundary_.oOo._Up6iGDNVfLS8dtXu4gmiozmowqzoLN7Z
Content-Type: application/json
Content-Disposition: form-data; name="Pipeline"

{
	"0": {
    	"FilterVersion": "1.2.718",
        "Filter_Enabled": true,
        "Filter_Human_Label": "Read DREAM.3D Data File",
        "Filter_Name": "DataContainerReader",
        "Filter_Uuid": "{043cbde5-3878-5718-958f-ae75714df0df}",
        "InputFile": "/Users/JohnDoe/Desktop/InputData.dream3d",
        "InputFileDataContainerArrayProxy": {
        	"Data Containers": [
        	]
    	},
        "OverwriteExistingDataContainers": 0
     },
     "1": {
     	"FilterVersion": "1.2.718",
        "Filter_Enabled": true,
        "Filter_Human_Label": "Write DREAM.3D Data File",
        "Filter_Name": "DataContainerWriter",
        "Filter_Uuid": "{3fcd4c43-9d75-5b86-aad4-4441bc914f37}",
        "OutputFile": "/Users/JohnDoe/Desktop/OutputData.dream3d",
        "WriteTimeSeries": 0,
        "WriteXdmfFile": 0
     },
     "PipelineBuilder": {
     	"Name": "CreateDCAPipeline",
        "Number_Filters": 2,
        "Version": 6
     }
}

--boundary_.oOo._Up6iGDNVfLS8dtXu4gmiozmowqzoLN7Z
Content-Type: application/json
Content-Disposition: form-data; name="PipelineMetadata"

{
	"0": {
    	"InputFile": {
        	"IO_Type": "Input"
        }
    },
    "1": {
    	"OutputFile": {
        	"IO_Type": "Output"
        }
    }
}

--boundary_.oOo._Up6iGDNVfLS8dtXu4gmiozmowqzoLN7Z
Content-Type: application/x-hdf
Content-Disposition: form-data; name="/Users/JohnDoe/Desktop/InputData.dream3d"

\x89HDF\r\n\x1A\n\x00\x00\x00\x00\x00\b\b\x00\x04\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFFk1\x00\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00`\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x88\x00\x00\x00\x00\x00\x00\x00\xA8\x02\x00\x00\x00\x00\x00\x00\x01\x00\x04\x00\x01\x00\x00\x00\x18\x00\x00\x00\x00\x00\x00\x00\x10\x00\x10\x00\x00\x00\x00\x00 \x03\x00\x00\x00\x00\x00\x00\x98\x00\x00\x00\x00\x00\x00\x00TREE\x00\x00\x01\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00x\x06\x00\x00\x00\x00\x00\x00\b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00HEAP\x00\x00\x00\x00X\x00\x00\x00\x00\x00\x00\x00@\x00\x00\x00\x00\x00\x00\x00\xC8\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00Pipeline\x00\x00\x00\x00\x00\x00\x00\x00""DataContainers\x00\x00""DataContainerBundles\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x18\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x11\x00\x10\x00\x00\x00\x00\x00\x88\x00\x00\x00\
x00\x00\x00\x00\xA8\x02\x00\x00\x00\x00\x00\x00\f\x00""0\x00\x00\x00\x00\x00\x01\x00\f\x00\b\x00\b\x00""FileVersion\x00\x00\x00\x00\x00\x13\x00\x00\x00\x04\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00""7.0\x00\x00\x00\x00\x00\f\x00@\x00\x00\x00\x00\x00\x01\x00\x10\x00\b\x00\b\x00""DREAM3D Version\x00\x13\x00\x00\x00\x12\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00""1.2.718.39c4387b4\x00\x00\x00\x00\x00\x00\x00\x01\x00\x04\x00\x01\x00\x00\x00\x18\x00\x00\x00\x00\x00\x00\x00\x10\x00\x10\x00\x00\x00\x00\x00\xC0\x07\x00\x00\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x00\x00\x00TREE\x00\x00\x01\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x88\t\x00\x00\x00\x00\x00\x00\b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00HEAP\x00\x00\x00\x00X\x00\x00\x00\x00\x00\x00\x00\x18\x00\x00\x00\x00\x00\x00\x00 \x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00Pipeline\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00@\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x
00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00SNOD\x01\x00\x03\x00(\x00\x00\x00\x00\x00\x00\x00\xAB.\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\xD3.\x00\x00\x00\x00\x00\x00\xF3""0\x00\x00\x00\x00\x00\x00\x18\x00\x00\x00\x00\x00\x00\x00#\x14\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00K\x14\x00\x00\x00\x00\x00\x00k\x16\x00\x00\x00\x00\x00\x00\b\x00\x00\x00\x00\x00\x00\x00\xB8\x03\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\xE0\x03\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0

--boundary_.oOo._Up6iGDNVfLS8dtXu4gmiozmowqzoLN7Z


