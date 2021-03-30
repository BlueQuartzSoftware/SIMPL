# Generate Cell Centers #

This filter will generate an Float Attribute Array that stores the center point of each voxel in an Image Geometry

## Group (Subgroup) ##

Core Filters (Conversion)

## Description ##

This filter will extract all the voxel centers of an Image Geometry or a RectilinearGrid geometry
into a new 3x1 Float Attribute Array.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| SelectedDataContainerName | string | Name of the DataContainer that has the Image or RectGrid Geometry object |
| CoordinateArrayPath | string | Name of the newly created AttributeArray that holds the Voxel Center Values |

## Required Geometry ##

Required Geometry Type: **ImageGeom** or **RectilinearGrid**

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** |  | ImageGeom or RectGridGeom | N/A | Input DataContainer that holds the geometry to extract |


## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| FloatArray | "Cell Centers" |  | N/A | The newly created Voxel Cell Centers |

## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users