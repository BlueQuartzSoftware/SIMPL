Extract Attribute Arrays from Geometry 
=============

## Group (Subgroup) ##

DREAM3D Review (Geometry)

## Description ##

This **Filter** extracts the **Attribute Arrays** that define the topology of a **Geometry** object and stores them explicitly in the data structure.  This allows information such as **Vertex** coordinates or **Element** to **Vertex** connectivity to be manipulated outside of **Geometries** (e.g, extracting the arrays into the data structure allows for them to be easily [exported to a text file](@ref writeasciidata)).  This **Filter** extracts the folllowing array information for each **Geometry**:

| Type             | Attribute Arrays Extracted |
|------------------|--------------------|
| Rectilinear Grid | x bounds, y bounds, z bounds |
| Vertex | vertex coordinates | 
| Edge | vertex coordinates, edge-to-vertex connectivity | 
| Triangle | vertex coordinates, triangle-to-vertex connectivity |
| Quadrilateral | vertex coordinates, quadrilateral-to-vertex connectivity |
| Tetrahedral | vertex coordinates, tetrahedral-to-vertex connectivity |
| Hexahedral | vertex coordinates, hexahedral-to-vertex connectivity |

**Image Geometries** do not store their topological information using **Attribute Arrays**, since the number of values needed to define an **Image** is small.  For more information on how to understand the arrays produced by this **Filter** (e.g., rectilinear bounds, edge-to-vertex connectivities, etc.), read the [documentation for the Create Geometry Filter](@ref creategeometry).

The user must select destinations for the extracted arrays.  It is strongly recommended to extract the arrays into **Attribute Matrices** associated with the corresponding **Element** types.  For example, the shared **Vertex** lists should be stored in the **Vertex Attribute Matrix** of the original **Data Container**, and the **Triangle** lists should be stored in the **Face Attribute Matrix** of the original **Data Container**.  This helps keep the information organized more easily (the interface will also enforce these selections); the only actual requirement, however, is that the created arrays properly fit in their destination **Attribute Matrix**. 

The bounds arrays extracted from **Rectilinear Grids** are a special case; the size of these arrays does not correspond to the number of **Cells** in the **Rectilinear Grid** (see [here](@ref creategeometry) for more details).  Therefore, the bounds arrays will not properly fit into the standard **Cell Attribute Matrix** of a **Data Container** that contains a **Rectilinear Grid**.  The user will most likely need to manually [create Attribute Matrices](@ref createattributematrix) for each of the bounds arrays (in the general case, this means creating three new **Attribute Matrices**, since the x, y, and z bounds of a **Rectilinear Grid** may all be of different sizes).  

_Note:_ This **Filter** copies the arrays from the selected **Geometry**.  Therefore, any operations on the original **Geometry** will not affect arrays created by this **Filter**, and any operations performed on the new arrays will not affect the original **Geometry**.  The arrays created by this **Filter** are the same kinds of arrays needed to [create a Geometry](@ref creategeometry).  In general, **Geometries** may store information other than simple connectivity (e.g., neighbor connectivities, vertex-to-element connectivities, etc.).  These additional arrays are not extracted by this **Filter**.

## Parameters ##

None

## Required Geometry ###

RectGrid, Vertex, Edge, Triangle, Quadrilateral, Tetrahedral, or Hexahedral

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | None | N/A | N/A | **Data Container** holding the **Geometry** from which to extract **Attribute Arrays**  |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Various **Attribute Arrays** | None | float/int64_t | 1/2/3/4 | Various **Attribute Arrays** used to define the **Geometry** topology; see [this documentation](@ref creategeometry) for a detailed discussion of what kinds of information are held within **Geometry** |

## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
