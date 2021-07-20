# SIMPL Python Example Ideas #

## Python Wrapping Ideas ##

Geometry:

+ Create a geomtery inside of DREAM3D (could mesh a synthetic, or from a reconstruction) and access the mesh data (Vertices and Triangles) along with the associated data from the python side.

+ Import a mesh on the python side and then pack that mesh into the DREAM.3D data structures. Bonus points to also associate cell or vertex data with the mesh

Data:

+ Create an regular grid geometry with data on the python side of things, pack into a DREAM.3D data structure, run a DREAM.3D filter on the data, return to Python and further manipulate the data

+ Show how to put generic data into the SIMPL Data Structure and then write that to a .dream3d file

## Embedded Python ##

+ Create a python based filter that reads in (IMPORT) Data and creates the proper Data Structure.
+ Create a python based filter that performs an advanced data analsysis or processing using python packages from conda-forge?
+ Create a filter that creates a triangle mesh using SciKits marching cubes algorithm
