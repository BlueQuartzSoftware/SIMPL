# SIMPL REST Server API Documentation #

The SIMPL REST server is a proof-of-concept implementation that allows SIMPL/DREAM.3D
to be remotely executed on a different physical machine than the client is running. It is
expected in the future that the API will be added to, modified and improved.

# Configuration #

There is a .ini file that can be edited to select the host address and port that the server will 
listen for connections. Edit this file to match your system. The file is copied from the 
source directory into the binary directory during CMake configuration steps.



# API Discussion #

All requests are through an HTTP **POST** request where the _content-type_ is set to 
_application/json_. Some API **end points** do not require a payload where others do.

The request should be sent to the following URL form:

    http://[HOST ADDRESS]:[PORT]/api/v1/[END POINT]

Static web pages and files can be served up by using:

    http://[HOST ADDRESS]:[PORT]/[SESSION_ID]/Filename


|  End Point | API Version | JSON Payload Required |
|------------|-------------|-----------------------|
| ExecutePipeline | v1 | YES |
| ListFilterParameters | v1 | YES |
| LoadedPlugins | v1 | NO |
| AvailableFilters | v1 | NO |
| NumFilters | v1 | NO  |
| PluginInfo   | v1 | YES |
| PreflightPipeline | v1 | YES |


## /api/v1/LoadedPlugins ##

**Input JSON**

| KEY | VALUE | Notes |
|-----|-------|-------|

**Output JSON**

| KEY | VALUE | Notes |
|-----|-------|-------|
| Plugins | Json Array of Objects | Information about each Plugin |


## /api/v1/PluginInfo ##

**Input JSON**

| KEY | VALUE | Notes |
|-----|-------|-------|
| PluginBaseName | string | Name of a plugin |


**Output JSON**

| KEY | VALUE | Notes |
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

| KEY | VALUE | Notes |
|-----|-------|-------|

**Output JSON**

| KEY | VALUE | Notes |
|-----|-------|-------|
| NumFilters | int | The number of filters available in the running instance |


## /api/v1/AvailableFilters ##

**Input JSON**

| KEY | VALUE | Notes |
|-----|-------|-------|


**Output JSON**

| KEY | VALUE | Notes |
|-----|-------|-------|
| ErrorCode | int | 0=No error, Negative=Error |
| Filters | Array of Objects | Each object has the "ClassName" and "HumanLabel" keys |

## /api/v1/ListFilterParameters ##

**Input JSON**

| KEY | VALUE | Notes |
|-----|-------|-------|
| ClassName | String | The C++ name of the filter.
| UUID | String | The UUID of the filter |

**Output JSON**

| KEY | VALUE | Notes |
|-----|-------|-------|
| ClassName |  |  |
| FilterParamerers | Array of FilterParameter JSON Objects |

** FilterParameter Json**

| KEY | VALUE | Notes |
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

| KEY | VALUE | Notes |
|-----|-------|-------|
| SessionID | UUID style | Optional. If missing a new UUID will be returned in the response JSON. Example UUID : "6cdba502-bdb1-5f96-86ee-0c4df2510aae" |
| ErrorLog | BOOLEAN | Save any errors to a file in the session id directory |
| WarningLog | BOOLEAN | Save any warnings to a file in the sssion id directory |
| StatusLog | BOOLEAN | Save all the standard output from the execution to a file in the session ID directory |
| Pipeline | JSON | The pipeline json as DREAM.3D would save it from the application using the DataContainerWriter class |


**Output JSON**

| KEY | VALUE | Notes |
|-----|-------|-------|
| Completed | true |  |
| OutputLinks | Array of Links to files or folders created when running the pipeline |  |
| SessionID | UUID create for the pipeline | d07f05ce-1389-5f80-8eca-383564b23e28 |
| Warnings | Array | Warning Messages generated during the execution of the pipeline |
| Errors | Array | Error messages generated during the execution of the pipeline |


## /api/v1/ExecutePipeline ##

**Input JSON**

| KEY | VALUE | Notes |
|----------|------------|----------|
| SessionID | UUID style | Optional. If missing a new UUID will be returned in the response JSON. Example UUID : "6cdba502-bdb1-5f96-86ee-0c4df2510aae" |
| ErrorLog | BOOLEAN | Save any errors to a file in the session id directory |
| WarningLog | BOOLEAN | Save any warnings to a file in the sssion id directory |
| StatusLog | BOOLEAN | Save all the standard output from the execution to a file in the session ID directory |
| Pipeline | JSON | The pipeline json as DREAM.3D would save it from the application using the DataContainerWriter class |


**Output JSON**

| KEY | VALUE | Notes |
|-----|-------|-------|
| Completed | true |  |
| OutputLinks | Array of Links to files or folders created when running the pipeline |  |
| SessionID | UUID create for the pipeline | d07f05ce-1389-5f80-8eca-383564b23e28 |
| Warnings | Array | Warning Messages generated during the execution of the pipeline |
| Errors | Array | Error messages generated during the execution of the pipeline |




