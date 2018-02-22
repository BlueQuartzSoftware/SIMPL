Find Derivatives 
=====

## Group (Subgroup) ##

Core (Geometry)

## Description ##

This **Filter** computes the spatial derivatives of an input **Attribute Array**.  The output array will have 3 times the number of components of the input array, since the derivatives are computed for each direction.  The algorithm used to compute the derivatives depends on the underlying **Geometry** on which the array lies:

### Image & RectGrid Geometries ###

For the grid-based geometries, the derivatives are computed using centered finite differences on a seven-point stencil.  The grid spacings are properly taken into account during the finite differencing.

### Unstructured Geometries ###

Since a **Vertex Geometry** carries no connectivity, no spatial derivatives are computed.  The output array will consist of zeroes. 

For the **Edge Geometry**, the derivatives are computed using a simple finite difference between values on a given **Edge's** nodes, weighted by the length of the edge.

For all other **Geometries**, the spatial derivatives are computed using the shape functions for the given unit element.  First, the Jacobian matrix for the derivatives of the shape functions for the given unit element is computed using the element's **Vertex** coordinates.  If the unit element is inherently 2D (e.g., a triangle), then it will first be projected from a 3D basis onto a 2D basis for the Jacobian computation.  The nodal values are then interpolated to the unit element's parametric center using the shape functions.  The derivative values are then computed by matrix multiplying the inverse of the Jacobian matrix with the interpolated nodal values.  This results in the derivative being stored on the given unit element.

The above techniques require that the selected **Attribute Array** exist on the **Vertices** of the given unstructured **Geometry**.  If the user selects an array that exists on the primary unit element (e.g., selecting a **Cell** array for a **Tetrahedral Geometry**), then the element values are first interpolated to the vertices of each element.  The **Vertex** values are computed as the average of all element values to which that **Vertex** belongs.

## Parameters ##

None

## Required Geometry ##

Any

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array** | None | Any | Any | The array for which the derivatives will be calculated |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array** | Derivtaives | double | 3 x the input array | The output derivatives array |


## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)










