Write DREAM.3D Data File {#datacontainerwriter}
=============

## Group (Subgroup) ##

IO (Output)

## Description ##

This **Filter** will write the contents of the current data structure to an [HDF5](https://www.hdfgroup.org/HDF5/) based file with the file extension .dream3d. The user can specify whether to write an [Xdmf](http://www.xdmf.org) that allows loading of the data into [ParaView](http://www.paraview.org/) for visualization. 

For more information on these outputs, see the [file formats](@ref supportedfileformats) documentation.


## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Output File | File Path | The outpute .dream3d file path |
| Write Xdmf File (ParaView Compatible File) | bool | Whether to write an Xdmf file for visualization |
 

## Required Geometry ##

Not Applicable

## Required Objects ##

None


## Created Objects ##

None



## Example Pipelines ##

+ (02) SmallIN100 Smooth Mesh
+ (04) SmallIN100 GBCD
+ CreateQuadGeometry
+ (09) Image Segmentation
+ CreateTriangleGeometry
+ CreateRectilinearGrid
+ CreateEdgeGeometry
+ (06) SmallIN100 Synthetic
+ InsertTransformationPhase
+ (01) Single Cubic Phase Equiaxed
+ CreateVertexGeometry
+ (08) Image Initial Visualization
+ (03) SmallIN100 Alignment
+ (04) SmallIN100 Presegmentation Processing
+ (07) SmallIN100 Final Processing
+ (02) Single Hexagonal Phase Equiaxed
+ (06) SmallIN100 Postsegmentation Processing
+ (05) Composite
+ (03) SmallIN100 Mesh Statistics
+ (03) Single Cubic Phase Rolled
+ (01) SmallIN100 Morphological Statistics
+ (02) SmallIN100 Initial Visualization
+ (05) SmallIN100 Segmentation
+ (04) Two Phase Cubic Hexagonal Particles Equiaxed
+ (01) SmallIN100 Quick Mesh
+ (10) SmallIN100 Full Reconstruction
+ CreateLambertSphereSurface
+ (03) SmallIN100 Transfer Field Data
+ (05) SmallIN100 Crystallographic Statistics
+ MassifPipeline
+ (04) SmallIN100 Field Threshold

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


