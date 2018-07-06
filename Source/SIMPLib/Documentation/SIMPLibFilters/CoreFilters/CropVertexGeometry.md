Crop Geometry (Vertex) 
=============

## Group (Subgroup) ##

DREAM3D Review (Cropping/Cutting)

## Description ##

This **Filter** crops a **Vertex Geometry** by the given bounding box.  Unlike the [cropping of an Image](@ref cropimagegeometry), it is unknown until run time how the **Geometry** will be changed by the cropping operation.  Therefore, this **Filter** requires that a new **Data Container** be created to contain the cropped **Vertex Geometry**.  This new **Data Container** will contain copies of any **Feature** or **Ensemble** **Attribute Matrices** from the original **Data Container**.  Additionally, all **Vertex** data will be copied, with tuples _removed_ for any **Vertices** outside the bounding box.  The user must supply a name for the cropped **Data Container**, but all other copied objects (**Attribute Matrices** and **Attribute Arrays**) will retain the same names as the original source.

_Note:_ Since it cannot be known before run time how many **Vertices** will be removed during cropping, the new **Vertex Geometry** and all associated **Vertex** data to be copied will be initialized to have size 0.  Any **Feature** or **Ensemble** information will retain the same dimensions and size.     

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| X Min | float | Lower X bound of the volume to crop out |
| Y Min | float | Lower Y bound of the volume to crop out |
| Z Min | float | Lower Z bound of the volume to crop out |
| X Max | float | Upper X bound of the volume to crop out |
| Y Max | float | Upper Y bound of the volume to crop out |
| Z Max | float | Upper Z bound of the volume to crop out |

## Required Geometry ###

Vertex

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | VertexDataContainer | N/A | N/A | **Data Container** holding the **Vertex Geometry** to crop |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Cropped **Data Container** | CroppedDataContainer | N/A | N/A | **Data Container** holding the cropped **Vertex Geometry** and any copied **Attribute Matrices** and **Attribute Arrays** |

## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
