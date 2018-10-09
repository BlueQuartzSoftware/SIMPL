Create Geometry 
=============

## Group (Subgroup) ##

DREAM3D Review (Geometry)

## Description ##

This **Filter** creates a **Geometry** object within a **Data Container**.  The **Data Container** must not already contain a **Geometry**; empty **Data Containers** may be created using the [Create Data Container](@ref createdatacontainer) **Filter**.  This **Filter** will also create the necessary **Element Attribute Matrices** on which to store **Attribute Arrays**.  The type of **Attribute Matrices** created depends on the kind of **Geometry** being created:

| Type             | Attribute Matrices |
|------------------|--------------------|
| Image | Cell | 
| Rectilinear Grid | Cell |
| Vertex | Vertex | 
| Edge | Vertex + Edge | 
| Triangle | Vertex + Face |
| Quadrilateral | Vertex + Face |
| Tetrahedral | Vertex + Cell |
| Hexahedral | Vertex + Cell |

### Understanding Geometries ###

This **Filter** requires the user to enter information that defines the topological information for the chosen **Geometry**.  Choosing valid information for a given **Geometry** necessitates an understanding of how **DREAM.3D** stores and interprets this information.  A general overview of the data model used in **DREAM.3D** may be found [here](@ref datastructure).  More specific information for **Geometry** objects is provided below.

#### Grid Geometries ####

##### Image #####

An **Image Geometry** is a _grid-like_ **Geometry**, and is the simplest and most widely used of the basic **Geometry** types.  An **Image Geometry** is a _regular, rectilinear grid_; if the _dimenionality_ of the image is _d_, then only _3*d_ numbers are needed to completely define the **Geometry**: three _d_-vectors for the _dimensions_, _origin_, and _resolution_.  Dimensions define the extents of the grid, the resolution defines the spacing between grid planes for each orthogonal direction (constant along a given direction), and the origin defines the physical location of the _bottom left_ grid point in _d_-dimensional space.  The dimensions are stored as _unsigned 64-bit integers_, while the origin and resolution are stored as _32-bit floats_. 

All **Image Geometries** in **DREAM.3D** are defined using 3-vectors (3D images).  A 2D image is assumed when one of the dimension values is exactly 1; the 2D image is then considered a plane.  Most **DREAM.3D** **Filters** will properly take account for the **Image** dimension if it matters (for example, the [Find Feature Shapes](@ref findshapes) **Filter** accounts for whether the **Image** is 2D or 3D when computing values such as _aspect ratios_ or _axis Euler angles_).  No dimension may be negative or equal to 0.  The resolution and origin have no value restrictions.  This **Filter** requires the user to enter the nine values for the dimenions, origin, and resolution.

Since all **Image Geometries** are implicitly 3D (even when plane-like), the fundamental building-block of an image is a _voxel_, which is a 3D object; therefore, the basic **Element** type for an **Image Geometry** is **Cell**.  **Attribute Arrays** associated with **Image Cells** are assumed to raster _x-y-z_, fastest to slowest.

##### Rectilinear Grid #####

A **Rectilinear Grid Geometry** is a _grid-like_ **Geometry**.  Similar to an **Image Geometry**, a **Rectilinear Grid Geometry** has grid extents (dimensions), but is allowed to have variable _resolution_ along each orthogonal direction.  The **Geometry** then requires a total of (x<sub>dim</sub> + 1) + (y<sub>dim</sub> + 1) + (z<sub>dim</sub> + 1) numbers to define the topology.  The values are stored in three separate arrays termed the _x bounds_, _y bounds_, and _z bounds_.  These bounds arrays store the spatial location of all the planes along a given orthogonal direction.  The spacing for a given plane (equivalent to the resolution for an **Image Geometry**) is then the difference between two of these contiguous array values.  An origin does not need to be defined for a **Rectilinear Grid Geometry**, since the grid's location in space is explicitly encoded in its bounds arrays.  This **Filter** requires the user to select **Attribute Arrays** that define the three bounds arrays.  These arrays must be _single component, 32-bit float_ arrays.  Additionally, the values for each of the bounds arrays must be _strictly increasing_, which guarantees that computing the spacing for a given plane yields a postive value. 

A **Rectilinear Grid Geometry** may be defined as 2D; the associated bounds array for the plane dimension is then exactly two.  No bounds arrays may have less than two values.  Since all **Rectilinear Grid Geometries** are implicitly 3D (even when plane-like), the fundamental building-block of an image is a _voxel_, which is a 3D object; therefore, the basic **Element** type for an **Image Geometry** is **Cell**.  **Attribute Arrays** associated with **Rectilinear Grid Cells** are assumed to raster _x-y-z_, fastest to slowest.

#### Unstructured and Mesh-Like Geometries ####

##### Vertex #####

A **Vertex Geometry** is an _unstructured_ **Geometry**.  An unstructured **Geometry** requires explicit definition of point coordinates.  Sometimes referred to as a _point cloud_, a **Vertex Geometry** is simply a collection of points.  Defining this topology requires a total number of values equal to _d_ times the total number of points, where _d_ is the dimensionality of the point cloud; within **DREAM.3D**, _d_ is always taken to be three.  The point coordinates are stored as _32-bit floats_; no other range restrictions are enforced.  This **Filter** requires the user to select an **Attribute Array** that defines these point coordinates.  The array must have _three components_ and consist of _32-bit floats_.  The number of _tuples_ in the array defines the number of vertices in the resulting **Vertex Geometry**.  

The fundamental **Element** type of a **Vertex Geometry** is _vertices_.  Data stored in a **Vertex Attribute Matrix** is ordered according to **Vertex** _Ids_.  Therefore, the n<sup>th</sup> tuple in the supplied **Vertex** list corresponds to the data stored in the n<sup>th</sup> column of the **Vertex Attribute Matrix**.  By convetion, **Vertex** Ids are _zero indexed_.

##### Mesh-Like Geometries #####

The following **Geometries** are considered _mesh-like_, and all share similar features concerning their storage and interpretation.  A mesh-like **Geometry** is an unstructured **Geometry** that additionally requires explicit definition of the connectivity of its **Elements** and its **Vertices**.  The **Element** type defines the kind of **Geometry** and the number of **Vertices** needed to define that **Element**:

| Name             | Element Type | Number of Vertices Per Element |
|------------------|--------------------|--------------------|
| Edge | line | 2 | 
| Triangle | triangle | 3 |
| Quadrilateral | quadrilateral | 4 |
| Tetrahedral | tetrahedron | 4 |
| Hexahedral | hexahedron | 8 |

The storage scheme adopted by **DREAM.3D** requires at least two arrays to define mesh-like **Geometries**: a list of **Vertices** (i.e., the vertex coordinates) and the **Element** connectivities (i.e., which vertices belong to a given **Element**).  To maintain simplicity, flexibility, and small memory overhead, **DREAM.3D** uses the concept of _shared vertex lists_.  In this paradigm, the vertex coordinates are stored only once per _unique_ vertex.  Consider a **Quadrilateral Geometry** that consists of just two squares that share one side.  In this example, there are exactly _six_ unique vertices.  The **Attribute Array** that defines the coordinates of these **Vertices** would then have six _tuples_, with three values at each tuple (the x, y, and z positions of that **Vertex**).  Writing each tuple on one line, the array could look like this:

    0.0 0.0 0.0 // Vertex Id 0
    1.0 0.0 0.0 // Vertex Id 1
    0.0 1.0 0.0 // Vertex Id 2
    1.0 1.0 0.0 // Vertex Id 3
    2.0 0.0 0.0 // Vertex Id 4
    2.0 1.0 0.0 // Vertex Id 5

**Element** connectivities are stored in **Attribute Arrays** that have a number of tuples equal to the total number of **Elements**, with a number of components at each tuple equal to the number of vertices per element.  In this example, a quadrilateral list would have two tuples, with four values stored at each tuple (the four vertex Ids that define that quadrilateral).  When defining **Elements**, the order in which the **Vertex** Ids are listed, called the _winding_, is important, since this ordering defines the direction of the normal.  By convention, the _right hand rule_ used.  Thus, given the above vertex positions, the following list of **Vertex** Ids defines two quadrilaterals whose normals point along the positive z direction:

    0 1 3 2 // Quad Id 0
    1 4 5 3 // Quad Id 1
    
Creating any mesh-like **Geometry** requires the user to supply two arrays: one that defines the vertex coordinates (the _shared vertex list_), which is a three component array of floats; and one that defines the **Element** connectivities, which is a n-component array (where n is the number of vertices per element) of _signed 64-bit integers_.  Note that any **Element** Id values (**Vertex** or otherwise) are _zero indexed_.

The shared list schema for mesh storage has the benefit of being space efficient, time efficient when iterating in sequence over vertices or elements, and capable of storing _nonmanifold_ meshes.  An example of a nonmanifold mesh is a **Triangle Geometry** that has more than two triangles sharing the same edge.  This specific example of nonmanifold meshes occurs frequently in **DREAM.3D** surface meshes of polycrystals, where many nonmanifold entities may exist (i.e., triple lines and quad points).  A significant downside of shared lists is that computing adjacency information, such as the neighbors of a given element or the elements that share a vertex, requires iterating over the entire **Geometry**; other mesh data structures avoid this limitation.  Additionally, since the lists are stored as **Attribute Arrays**, which hold information contiguously in memory, adding or removing vertices or elements is tedious and potentially slow.  

Note that although the default interpretation of lists that define mesh-like **Geometries** is shared, no undefined behavior should be observed if the information is not stored shared (i.e., if the same **Vertex** is stored more than once with a different Id).  Additionally, not all **Vertices** are required to be associated with an **Element**.  The primary requirement is that the largest **Vertex** Ids listed in the **Element** list must not be larger than the total number of **Vertices**.  
    
##### Edge #####

An **Edge Geometry** is the simplest _mesh-like_ **Geometry**, consisting of a collection of edges connecting two vertices.  Creating an **Edge Geometry** requires supplying a shared **Vertex** list and an **Edge** list.

##### Triangle #####

A **Triangle Geometry** is a _mesh-like_ **Geometry**, consisting of a collection of triangles connecting three vertices; it is a type of _surface mesh_.  Creating a **Triangle Geometry** requires supplying a shared **Vertex** list and a **Triangle** list.

##### Quadrilateral #####

A **Quadrilateral Geometry** is a _mesh-like_ **Geometry**, consisting of a collection of quadrilaterals connecting four vertices; it is a type of _surface mesh_.  Creating a **Quadrilateral Geometry** requires supplying a shared **Vertex** list and a **Quadrilateral** list.

##### Tetrahedral #####

A **Tetrahedral Geometry** is a _mesh-like_ **Geometry**, consisting of a collection of tetrahedra connecting four vertices; it is a type of _volume mesh_.  Creating a **Tetrahedral Geometry** requires supplying a shared **Vertex** list and a **Tetrahedral** list.  The winding that define tetrahedra require one additional convention to complement the right hand rule.  By convention, the first three vertices define the tetrahedra _base_; the winding of these vertices by the right hand rule defines a normal that points _towards the fourth vertex_.  This convention is useful since applying it consistently allows for the volume of the tetrahedra to be _signed_, which is important for determining if a tetrahedron is "inverted".

##### Hexahedral #####

A **Hexahedral Geometry** is a _mesh-like_ **Geometry**, consisting of a collection of hexahedra connecting eight vertices; it is a type of _volume mesh_.  Creating a **Hexahedral Geometry** requires supplying a shared **Vertex** list and a **Hexahedral** list.

### Defining Geometries with Attribute Arrays ###


### For **Geometries** that require the selection of **Attribute Arrays** (all **Geometries** except **Image**), the arrays will be _copied_ to create the new **Geometry**.  Therefore, any operations on the original array will not affect the topology of the **Geometry**, and any geometric operations will not affect the original array. This behavior can be adjusted in the filter by using the _Array Handling_ boolean. ###

This **Filter** will validate that the arrays selected to define a **Geometry** "make sense", given the above information for how **Geometries** are stored in **DREAM.3D** (for example, no dimension for an **Image** may be less than or equal to zero, no bounds arrays for a **Rectilinear Grid** may have less than two values, and no **Vertex** Ids stored in a shared **Element** list may be larger than the total number of **Vertices** in the shared **Vertex** list).  The checks that require accessing the actual array values (as opposed to just descriptive information) will be performed at run time.  By default, these checks will only produce warnings, allowing the **Pipeline** to continue; the user may opt to change these warnings to errors by selecting the _Treat Geometry Warnings as Errors_ option. 

Generally, arrays used by this **Filter** to create **Geometries** must be supplied by the user.  One method to import geometric information into **DREAM.3D** is to read the information in from a text file using the [Import ASCII Data](@ref readasciidata) **Filter**.  For example, imagine having an external simulation code that creates a tetrahedral volume mesh with two associated field values, one stored on the mesh vertices and one stored on the mesh tetrahedra.  It is possible to import this mesh and corresponding information into **DREAM.3D** for further analysis.  The user must supply at least two files: one that contains the vertex information and one that contains the tetrahedra information.  The vertex file would contain, on each line, the three coordinates of the vertex and the value of the field array on that vertex.  It may, for example, look like this:

    # Some header information
    # Some more header information
    x_pos y_pos z_pos value
    1.235 2.323 1.562 465.2
    -12.3 3.456 2.323 567.4
    3.450 9.782 6.567 120.2
    .....
    
In this above example, the vertex information begins on line 4; thus, line 4 defines **Vertex** Id 0, line 5 defines **Vertex** Id 1, etc.  Similarly, a file containing information about tetrahedra is needed:

    # Some header information
    # Some more header information
    vert_0 vert_1 vert_2 vert_3 value
    1      2      0      3      12.42
    2      7      5      4      14.71
    6      9      7      8      16.78
    .....  

 Again, the real information begins on line 4, which defines the connectivity for **Tetrahedra** Id 0.  These Id values refer to the **Vertex** Ids from the first file (for example, a **Vertex** Id of 0 corresponds to the information on line 4 of the first file).  Remember to consider the _right hand rule_ when dealing with mesh-like **Geometries**, as this will affect the **Vertex** ordering!
 
 Assuming it is possible to get the mesh into files similar to the above ones, it is straightforward to import the information into **DREAM.3D**.  First, [create an emtpy Data Container](@ref createdatacontainer).  Then, run the [Import ASCII Data](@ref readasciidata) **Filter** for the vertex file.  In the above example, line 3 could be used as headers to define the array names (remember that **Vertex** positions must be of type _float_!).  Allow the reader wizard to create an **Attribute Matrix** in which to store the arrays.  Repeat the process with another [Import ASCII Data](@ref readasciidata) **Filter** to read the tetrahedra information (remember in this case that Id values must be of type _int64\_t_!).  At this point, there will be two **Attribute Matrices** in the **Data Container**, one with 4 arrays (the **Vertex** information) and one with 5 arrays (the **Tetrahedra** information). The [Create Geometry](@ref creategeometry) **Filter** wants the **Vertex** list as a three component array and the **Tetrahedra** list as a four component array.  To combine the individual arrays into ones of the proper component dimension, run the [Combine Attribute Arrays](@ref combineattributearrays) **Filter**.  At this point, the [Create Geometry](@ref creategeometry) **Filter** may be used to create a **Tetrahedral Geometry** using the combined arrays.  After this **Filter**, the [Move Data](@ref movedata) **Filter** may be used to move the arrays that represent the values stored on the **Vertices** and **Tetrahedra** into the created **Vertex** and **Cell Attribute Matrices**.
 
 When creating **Geometries**, remember to consider all the various rules for how a **Geometry** is stored and interpreted.  In particular, remeber that **Element** Ids are always zero indexed, mesh-like **Geometries** obey the _right hand rule_ for windings and normal directions, and **Element** lists are by default considered _shared_.  Note that although the storage scheme used by **DREAM.3D** (shared lists) is highly generic, some **Filters** may assume that the **Geometry** is reasonably _well formed_.
 
## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Geometry Type | Enumeration | The type of **Geometry** to create |
| Treat Geometry Warnings as Errors | bool | Whether run time warnings for **Geometries** should be treated as errors |
| Array Handling | bool | Determines if the arrays that make up the geometry primitives should be **Moved** or **Copied** to the created Geometry object. |
| Dimensions | size_t (3x) | The number of cells in each of the X, Y, Z directions, if _Image_ is chosen |
| Origin | float (3x) | The origin of each of the axes in X, Y, Z order, if _Image_ is chosen |
| Resolution | float (3x) | The length scale of each voxel/pixel, if _Image_ is chosen |

## Required Geometry ###

None

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | None | N/A | N/A | **Data Container** in which to place the created **Geometry** |
| Various **Attribute Arrays** | None | float/int64_t | 1/2/3/4 | Various **Attribute Arrays** used to define the **Geometry** topology; see the above documentation for a detailed discussion of what kinds of information are needed for each **Geometry** |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Various **Attribute Matrices** | None | Vertex/Edge/Face/Cell | N/A | Various **Attribute Matrices** used to store information for the new **Geometry**; see the above documentation for a detailed discussion of what kinds of **Attribute Matrices** are created for each **Geometry** |

## Example Pipelines ##

+ CreateVertexGeometry
+ CreateTriangleGeometry
+ CreateEdgeGeometry
+ CreateQuadGeometry
+ CreateRectilinearGrid

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users

