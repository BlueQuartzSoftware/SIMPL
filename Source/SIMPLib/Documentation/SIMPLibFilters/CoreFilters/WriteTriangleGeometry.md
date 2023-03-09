# Export Triangle Geometry  #


## Group (Subgroup) ##

IO (Output)

## Description ##

This filter creates 2 files from the Vertices and Triangles from a Triangle or Quad Geometry. The basic form of the files is simple ASCII where the *vertices* are a "Shared Vertex List" and the *triangles* or *quads* consist of 3 or 4 values where each value is the index into the **vertex** list.

**Example Nodes File**

	# All lines starting with '#' are comments
	# DREAM.3D Nodes file
	# DREAM.3D Version 6.1.92.0613671
	# Node Data is X Y Z space delimited.
	Index,X,Y,Z
	1 0.00000  0.00000  0.00000
	2 0.00000  0.50000  0.00000
	3 0.00000  0.00000  0.50000
	4 0.00000  0.50000  0.50000
	             ..

**Example Triangles File**

	# All lines starting with '#' are comments
	# DREAM.3D Triangle file
	# DREAM.3D Version 6.1.92.0613671
	# Each Triangle consists of 3 Node Ids.
	# NODE IDs START AT 0.
	Index,V1,V2,V3
	1 0 1 2
	2 1 3 2
	3 0 2 4
	4 4 2 5
	5 0 4 1
	6 4 6 1
	


![Rendering of Nodes from above file example](Images/WriteTriangleGeometry_Example.png)

## Parameters ##

| Name | Type |
|----------|--------|
| Output Nodes File | Output File Path |
| Output Triangles File | Output File Path |
| Comment Chararcter | String |
| Delimiter | String |


## Required Geometry ##

Triangle

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | None | N/A | N/A | **Data Container** in which to place the created **Triangle Geometry** |
| DataArrayPath List | |  | | List of DataArrayPaths of Vertex data that should also be written |
| DataArrayPath List | |  | | List of DataArrayPaths of Face data that should also be written |

## Created Objects ##

None

## Example Pipelines ##


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


