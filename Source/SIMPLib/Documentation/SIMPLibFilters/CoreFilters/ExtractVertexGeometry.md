# Extract Vertex Geometry #


## Group (Subgroup) ##

Core Filters (Conversion)

## Description ##

This filter will extract all the voxel centers of an Image Geometry or a RectilinearGrid geometry
into a new VertexGeometry. The user is given the option to copy or move cell arrays over to the 
newly created **DataContainer**.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| ArrayHandling | int | 0=Move Arrays, 1=Copy Arrays |
| SelectedDataContainerName | string | Name of the DataContainer that has the Image or RectGrid Geometry object |
| IncludedDataArrayPaths | QVector<DataArrayPath> | List of DataArrayPaths to either copy or move |
| VertexDataContainerName | string | Name of the newly created DataContainer that holds the **VertexGeometry** |

## Required Geometry ##

Required Geometry Type: **ImageGeom** or **RectilinearGrid**

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** |  | ImageGeom or RectGridGeom | N/A | Input DataContainer that holds the geometry to extract |


## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | Set by user | N/A | N/A | Name of output DataContainer |
| **VertexGeometry** | "VertexGeometry |  | N/A | The newly created Vertex Geometry |

## Example Pipelines ##

PrebuiltPipelines/Examples/Extract Vertex Geometry.json

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users