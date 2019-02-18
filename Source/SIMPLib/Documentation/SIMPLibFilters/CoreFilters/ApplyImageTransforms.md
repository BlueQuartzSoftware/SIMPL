# Apply Image Transforms #

## Group (Subgroup) ##

CoreFilters (Image)

## Description ##

This **Filter** updates the geometry of selected data containers by applying the transforms stored in each data container to the data container's origin.  This is an experimental filter and is not yet ready for release to the public.  It is only currently meant to be used to apply Affine 3x3 transforms from each data container's transform container to the data container's origin.

## Parameters ##

None.

## Required Geometry ##

Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Image Data Containers** | N/A | Any | Any | The data containers whose stored transforms will be applied to the origin. |

## Created Objects ##

None.

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users