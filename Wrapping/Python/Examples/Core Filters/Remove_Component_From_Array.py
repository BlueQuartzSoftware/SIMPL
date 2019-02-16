# Pipeline : Remove Component From Array Filter Test
# Based on (01) SmallIN100 Quick Mesh (from EBSD Surface Meshing)

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd

import datetime as datetime


def remove_component_from_array_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Confidence Index").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "CriticalField").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "EulerAngles").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "FeatureIds").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "FeatureReferenceMisorientations").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Fit").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "GBManhattanDistances").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "IPFColor").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Image Quality").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "KernelAverageMisorientations").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Mask").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "ParentIds").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Phases").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "QPManhattanDistances").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Quats").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "SEM Signal").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "TJManhattanDistances").flag = 2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "CrystalStructures").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "LatticeConstants").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "MaterialName").flag = 2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Active").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AspectRatios").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AvgEuler").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AvgEulerAngles").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AvgQuats").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AxisEulerAngles").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AxisLengths").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Centroids").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "CriticalFields").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "EquivalentDiameters").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "FeatureAvgMisorientations").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "MisorientationList").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "NeighborList").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "NeighborhoodList").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Neighborhoods").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "NumElements").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "NumNeighbors").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "NumNeighbors2").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Omega3s").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "ParentIds").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Poles").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Phases").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Schmids").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Shape Volumes").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "SharedSurfaceAreaList").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Size Volumes").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "SlipSystems").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Sphericity").flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "SurfaceAreaVolumeRatio").flag = 2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("NewGrain Data").getDataArrayProxy(
        "Active").flag = 2

    now_time = datetime.datetime.now()
    now_time_seconds = now_time.hour * 3600 + now_time.minute * 60 + now_time.second
    dt = simpl.DateTime(now_time.year, now_time.month, now_time.day, now_time_seconds)
    err = simplpy.data_container_reader(dca,
                                        sd.GetBuildDirectory() +
                                        "/Debug/Data/Output/Statistics/SmallIN100_CrystalStats.dream3d",
                                        False, "", dt, dcap)
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)
        
    # Remove Component From Array
    err = simplpy.remove_component_from_array(dca,
                                              simpl.DataArrayPath("Small IN100",
                                                                  "Grain Data",
                                                                  "AxisEulerAngles"), 2, True,
                                              "AxisEulerAngles_RemovedComp",
                                              "AxisEulerAngles_Reduced")
    if err < 0:
        print("RemoveComponentFromArray ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Debug/Data/Output/CoreFilterTests/" +
                              "RemoveComponentFromArray.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    remove_component_from_array_test()
