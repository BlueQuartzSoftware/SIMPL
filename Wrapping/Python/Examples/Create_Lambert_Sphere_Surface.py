
import time

# Based on CreateLambertSphereSurface pipeline example
# Creates a LambertSphereSurface using Python/C++ bindings
#
# There are 2 options. Run using the pipeline and building the filters non-pythonic
# The other option is to "build" the pipeline effect through pythonic API and
# executing each filter upon creation. The choice can be selected by changing the value of runPipeline

# These are the simpl_py python modules

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientationanalysis

def LambertSphereCreateTest():
  runPipeline = False
  # Create Data Container Array
  dca = simpl.DataContainerArray.New()
  if runPipeline:
    runPipelineTest(dca)
  else:
    runPythonicTest(dca)

def runPipelineTest(dca):
  # Create Data Container
  dataContainer = simpl.CreateDataContainer.New()
  dataContainer.setDataContainerArray(dca)
  dataContainer.DataContainerName = "ImageDataContainer"
  
  # Create Geometry (Image)
  imageGeom = simpl.CreateImageGeometry.New()
  imageGeom.setDataContainerArray(dca)
  imageGeom.SelectedDataContainer = "ImageDataContainer"
  imageGeom.Dimensions = simpl.IntVec3(101, 101, 1)
  imageGeom.Origin = simpl.FloatVec3(0, 0, 0)
  imageGeom.Resolution = simpl.FloatVec3(1, 1, 1)

  # Create Attribute Matrix
  attributeMatrix = simpl.CreateAttributeMatrix.New()
  attributeMatrix.setDataContainerArray(dca)
  attributeMatrix.AttributeMatrixType = 3
  newRow = simpl.VectorDouble()
  newRow.append(101)
  newRow.append(101)
  newRow.append(1)
  tableData = list()
  tableData.append(newRow)
  attributeMatrix.TupleDimensions = simpl.DynamicTableData(tableData, ["0", "1", "2"], ["0"])
  attributeMatrix.CreatedAttributeMatrix = simpl.DataArrayPath("ImageDataContainer","CellAttributeMatrix","")
  
  # Create Data Array
  dataArray = simpl.CreateDataArray.New()
  dataArray.setDataContainerArray(dca)
  dataArray.ScalarType = simpl.ScalarTypes.UInt8
  dataArray.NumberOfComponents = 1
  dataArray.InitializationType = 0
  dataArray.InitializationValue = "128"
  dataArray.NewArray = simpl.DataArrayPath("ImageDataContainer","CellAttributeMatrix","ScalarValues")
 
  # Create Sphere (Lambert Projection)
  # Note: "Checkboxes" default to true
  createLambertSphere = orientationanalysis.CreateLambertSphere.New()
  createLambertSphere.setDataContainerArray(dca)
  createLambertSphere.Hemisphere = 0
  createLambertSphere.ImageDataArrayPath = simpl.DataArrayPath("ImageDataContainer","CellAttributeMatrix","ScalarValues")
  createLambertSphere.VertexDataContainerName = "VertexDataContainer"
  createLambertSphere.EdgeDataContainerName = "EdgeDataContainer"
  createLambertSphere.TriangleDataContainerName = "TriangleDataContainer"
  createLambertSphere.QuadDataContainerName = "QuadDataContainer"

  # Write DREAM.3D Data File
  dataContainerWriter = simpl.DataContainerWriter.New()
  dataContainerWriter.setDataContainerArray(dca)
  dataContainerWriter.OutputFile = sd.GetTestTempDirectory() + "/LambertSphere.dream3d"
  dataContainerWriter.WriteXdmfFile = True
  dataContainerWriter.WriteTimeSeries = False

  # Create  Pipeline
  pipeline = simpl.FilterPipeline.New()
  pipeline.Name = ("New Pipeline")

  # Add filters (order is important)
  pipeline.pushBack(dataContainer)
  pipeline.pushBack(imageGeom)
  pipeline.pushBack(attributeMatrix)
  pipeline.pushBack(dataArray)
  pipeline.pushBack(createLambertSphere)
  pipeline.pushBack(dataContainerWriter)

  err = pipeline.preflightPipeline()
  if err < 0:
    print("Preflight ErrorCondition: %d" % err)

  pipeline.run()
  err = pipeline.ErrorCondition
  if err < 0:
    print("Execute ErrorCondition: %d" % err)

  time.sleep(2)


def runPythonicTest(dca):
  err = simplpy.create_data_container(dca, "ImageDataContainer")
  if err < 0:
    print("DataContainer ErrorCondition: %d" % err)

  err = simplpy.create_image_geometry(dca, "ImageDataContainer", simpl.IntVec3(101, 101, 1), simpl.FloatVec3(0,0,0), simpl.FloatVec3(1,1,1))
  if err < 0:
    print("ImageGeometry ErrorCondition: %d" % err)

  newRow = simpl.VectorDouble()
  newRow.append(101)
  newRow.append(101)
  newRow.append(1)
  tableData = list()
  tableData.append(newRow)
  err = simplpy.create_attribute_matrix(dca, simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", ""), 3, simpl.DynamicTableData(tableData, ["0", "1", "2"], ["0"]))
  if err < 0:
    print("AttributeMatrix ErrorCondition: %d" % err)
    
  err = simplpy.create_data_array(dca, simpl.ScalarTypes.UInt8, 1, simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", "ScalarValues"),
   simpl.InitializationType.Manual, "128", (0.0,151.1))
  if err < 0:
    print("DataArray ErrorCondition: %d" % err)

  am = dca.getAttributeMatrix(simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", ""))
  dataArray = am.getAttributeArray("ScalarValues")

  err = orientationanalysis.create_lambert_sphere(dca, sc.Hemisphere.Northern, simpl.DataArrayPath("ImageDataContainer","CellAttributeMatrix","ScalarValues"),
   "QuadDataContainer", "TriangleDataContainer", "EdgeDataContainer", "VertexDataContainer", "VertexAttributeMatrix", "EdgeAttributeMatrix", "FaceAttributeMatrix",
    True, True, True, True)
  if err < 0:
    print("LambertSphere ErrorCondition: %d" % err)
  err = simplpy.data_container_writer(dca, sd.GetTestTempDirectory() + "/LambertSphere.dream3d", True, False)
  if err < 0:
    print("DataContainerWriter ErrorCondition: %d" % err)

"""
Main entry point for python script
"""
if __name__ == "__main__":
  LambertSphereCreateTest()
  print("Test Complete")

