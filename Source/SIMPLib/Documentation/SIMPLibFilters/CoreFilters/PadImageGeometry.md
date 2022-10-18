# Pad Geometry (Image) #


## Group (Subgroup) ##

Generic (Generic)

## Description ##

This **Filter** pads an image geometry by the given min/max voxels for each dimension in X, Y, and Z, using the default padding value.
It is also possible to optionally update the origin of the image geometry, which prevents the original data from shifting in space.

Example:

Original Geometry
- 100x100x100 with origin (0, 0, 0)

Filter Inputs
- X Min/Max - 0, 30
- Y Min/Max - 0, 30
- Z Min/Max - 30, 0
- Default Padding Value - 1
- Updated Origin - true

New Geometry
- 130x130x130 with origin (-20, -20, -20)
- 1's padded 30 voxels in X+ direction
- 1's padded 30 voxels in Y+ direction
- 1's padded 30 voxels in Z- direction

## Parameters ##

| Name | Type | Description |
|------|------|------|
| X Min & Max Padding | IntVec2 | The X min and X max padding sizes |
| Y Min & Max Padding | IntVec2 | The Y min and Y max padding sizes |
| Z Min & Max Padding | IntVec2 | The Z min and Z max padding sizes |
| Update Origin | bool | Determines whether or not to update the origin |

## Required Geometry ##

Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix** | Attribute Matrix | Any | Any | The attribute matrix inside the image geometry that will be padded |

## Created Objects ##

None

## Example Pipelines ##

None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
