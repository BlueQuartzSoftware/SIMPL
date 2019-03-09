# Pipeline : (04) SmallIN100 GBCD (from EBSD Surface Meshing)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientationanalysis
from dream3d import iopy

import datetime as datetime


def small_in100_gbcd():
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

    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "FaceAreas").flag = 2
    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "FaceDihedralAngles").flag = 2
    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "FaceLabels").flag = 2
    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "FaceNormals").flag = 2
    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "SurfaceMeshFaceIPFColors").flag = 2
    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "SurfaceMeshMisorientationColors").flag = 2

    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceFeatureData").flag = 2

    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("VertexData").getDataArrayProxy(
        "NodeType").flag = 2

    now_time = datetime.datetime.now()
    now_time_seconds = now_time.hour * 3600 + now_time.minute * 60 + now_time.second
    dt = simpl.DateTime(now_time.year, now_time.month, now_time.day, now_time_seconds)
    err = d3d.data_container_reader(dca,
                                    sd.GetBuildDirectory() +
                                    "/Debug/Data/Output/SurfaceMesh/SmallIN100_MeshStats.dream3d",
                                    False, "", dt, dcap)
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Find GBCD
    err = orientationanalysis.find_gbcd(dca, "FaceEnsembleData", 9,
                                        simpl.DataArrayPath("TriangleDataContainer", "FaceData", "FaceLabels"),
                                        simpl.DataArrayPath("TriangleDataContainer", "FaceData", "FaceNormals"),
                                        simpl.DataArrayPath("TriangleDataContainer", "FaceData", "FaceAreas"),
                                        simpl.DataArrayPath("Small IN100", "Grain Data", "AvgEulerAngles"),
                                        simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                                        simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures"),
                                        "GBCD")
    if err < 0:
        print("FindGBCD ErrorCondition %d" % err)

    # Export GBCD Pole Figure (GMT 5)
    err = iopy.visualize_gbcdgmt(dca, sd.GetBuildDirectory() + "/Debug/Data/Output/SmallIN100GBCD/SmallIn100GMT_1.dat",
                                 1, simpl.AxisAngleInput(60, 1, 1, 1),
                                 simpl.DataArrayPath("TriangleDataContainer", "FaceEnsembleData", "GBCD"),
                                 simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures"))
    if err < 0:
        print("VisualizeGBCDGMT ErrorCondition %d" % err)

    # Export GBCD Pole Figure (VTK)
    err = iopy.visualize_gbcd_pole_figure(dca,
                                          sd.GetBuildDirectory() + "/Debug/Data/Output/SmallIN100GBCD/SmallIn100VTK.vtk",
                                          1, simpl.AxisAngleInput(60, 1, 1, 1),
                                          simpl.DataArrayPath("TriangleDataContainer", "FaceEnsembleData", "GBCD"),
                                          simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures"))
    if err < 0:
        print("VisualizeGBCDPoleFigure ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Debug/Data/Output/SurfaceMesh/SmallIN100_GBCD.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_gbcd()
