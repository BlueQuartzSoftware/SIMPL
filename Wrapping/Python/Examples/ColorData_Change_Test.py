"""

Pipeline example based on Small IN100 Synthetic where IPF Color values are changed
using Numpy and re-added to the attribute matrix

"""

import dream3d
import dream3d.core as d3d
import dream3d.dream3d.simpl as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd
import dream3d.dream3d.syntheticbuilding as syntheticbuilding
import dream3d.synthetic_building as syntheticbuilding_pythonic
import dream3d.dream3d.statistics as statistics
import dream3d.statistics as statistics_pythonic
import dream3d.dream3d.orientationanalysis as orientationanalysis
import dream3d.orientation_analysis as orientationanalysis_pythonic
import dream3d.dream3d.generic as generic
import dream3d.generic as generic_pythonic

# Numpy for some data management assistance
import numpy as np
import datetime as datetime

def ColorDataChangeTest():

    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy("Small IN100").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("Confidence Index").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("CriticalField").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("EulerAngles").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("FeatureIds").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("Fit").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("GBManhattanDistances").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("IPFColor").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("Image Quality").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("Mask").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("ParentIds").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("Phases").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("QPManhattanDistances").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("Quats").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("SEM Signal").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("TJManhattanDistances").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("IPFColor").flag = 2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy("CrystalStructures").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy("LatticeConstants").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy("MaterialName").flag = 2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Active").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("AspectRatios").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("AvgEuler").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("AxisEulerAngles").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("AxisLengths").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Centroids").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("CriticalFields").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("EquivalentDiameters").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("NeighborList").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("NeighborhoodList").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Neighborhoods").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("NumElements").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("NumNeighbors").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("NumNeighbors2").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Omega3s").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("ParentIds").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Phases").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Shape Volumes").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("SharedSurfaceAreaList").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Size Volumes").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Sphericity").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("SurfaceAreaVolumeRatio").flag = 2
	
    now_time = datetime.datetime.now()
    now_time_seconds = now_time.hour * 3600 + now_time.minute * 60 + now_time.second
    dt = simpl.DateTime(now_time.year, now_time.month, now_time.day, now_time_seconds)
    err = d3d.data_container_reader(dca, sd.GetBuildDirectory() + "/Debug/Data/Output/Synthetic/06_SmallIN100Synthetic.dream3d",
                                False, "", dt, dcap)
    if err < 0:
      print("DataContainerReader ErrorCondition %d" % err)
      print("Exiting...")
      return

    # Change the color data
    am = dca.getAttributeMatrix(simpl.DataArrayPath("Small IN100", "EBSD Scan Data", ""))
    dataArray = am.getAttributeArray("IPFColor")
    compDims = dataArray.getComponentDimensions()
    tupleDims = am.TupleDimensions
    data = dataArray.Data
	
    # Zero array
    # np_array = np.zeros_like(data).astype('uint8').reshape(dataArray.getNumberOfTuples(), dataArray.getNumberOfComponents())
	
    # Unchanged array
    np_array = np.asanyarray(data).reshape(dataArray.getNumberOfTuples(), dataArray.getNumberOfComponents())
    for row in np_array:
        if row[0] > 0:
            row[0] = row[0] / 2
        else:
            row[0] = 255
        row[1] = 128
        row[2] = 0
    newdata = sc.ConvertToDataArray("IPFColor", np_array, compDims[0])
    sc.RemoveArray(dca, ["Small IN100", "EBSD Scan Data", "IPFColor"])
    am.addAttributeArray("IPFColor", newdata[1])

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Debug/Data/Output/Synthetic/06_SmallIN100Synthetic_ColorChanged.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)

"""
Main entry point for python script
"""
if __name__ == "__main__":
    ColorDataChangeTest()


