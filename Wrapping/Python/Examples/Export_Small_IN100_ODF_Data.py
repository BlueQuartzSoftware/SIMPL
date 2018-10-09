#
# Example pipeline : Export Small IN100 ODF Data
#

import time

# These are the simpl_py python modules

import dream3d
import dream3d.core as d3d
import dream3d.dream3d.simpl as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd
import dream3d.dream3d.orientationanalysis as orientationanalysis
import dream3d.orientation_analysis as orientationanalysis_pythonic
import dream3d.dream3d.sampling as sampling
import dream3d.sampling as sampling_pythonic


def Dream3DTest():
    runPipeline = False

    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    if runPipeline:
        PipelineTest(dca)
    else:
        PythonicTest(dca)


def PipelineTest(dca):
    # ReadAngData
    # Read in the SmallIN100 data
    print("# --- ReadAngData ")
    readAngData = orientationanalysis.ReadAngData.New()
    readAngData.setDataContainerArray(dca)
    readAngData.InputFile = sd.GetBuildDirectory() + "/Debug/Data/SmallIN100/Slice_1.ang"
    readAngData.DataContainerName = "Small IN100 Slice 1"
    readAngData.CellAttributeMatrixName = "EBSD Scan Data"
    readAngData.CellEnsembleAttributeMatrixName = "Phase Data"
    readAngData.preflight()
    print("  Preflight Error Code: %s" % readAngData.ErrorCondition)
    filterName = readAngData.NameOfClass
    if filterName != "ReadAngData":
        print("  Error: Filter class name is not correct. %s != ReadAngData" % filterName)

    # RotateEulerRefFrame
    #
    print("# --- RotateEulerRefFrame ")
    rotateEulerRefFrame = orientationanalysis.RotateEulerRefFrame.New()
    rotateEulerRefFrame.setDataContainerArray(dca)
    rotateEulerRefFrame.RotationAngle = 90.0
    rotateEulerRefFrame.RotationAxis = simpl.FloatVec3(0.0, 0.0, 1.0)
    rotateEulerRefFrame.CellEulerAnglesArrayPath = simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data",
                                                                       "EulerAngles")
    rotateEulerRefFrame.preflight()
    print("  Preflight Error Code: %s" % rotateEulerRefFrame.ErrorCondition)
    filterName = rotateEulerRefFrame.NameOfClass
    if filterName != "RotateEulerRefFrame":
        print("  Error: Filter class name is not correct. %s != RotateEulerRefFrame" % filterName)

    # RotateSampleRefFrame
    #
    print("# --- RotateSampleRefFrame ")
    rotateSampleRefFrame = sampling.RotateSampleRefFrame.New()
    rotateSampleRefFrame.setDataContainerArray(dca)
    rotateSampleRefFrame.RotationAngle = 180.0
    rotateSampleRefFrame.RotationAxis = simpl.FloatVec3(0.0, 1.0, 0.0)
    rotateSampleRefFrame.CellAttributeMatrixPath = simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "")
    rotateSampleRefFrame.preflight()
    print("  Preflight Error Code: %s" % rotateSampleRefFrame.ErrorCondition)
    filterName = rotateSampleRefFrame.NameOfClass
    if filterName != "RotateSampleRefFrame":
        print("  Error: Filter class name is not correct. %s != RotateSampleRefFrame" % filterName)

    # MultiThresholdObjects
    #
    print("# --- MultiThresholdObjects ")
    multiThresholdObjects = simpl.MultiThresholdObjects.New()
    multiThresholdObjects.setDataContainerArray(dca)
    multiThresholdObjects.DestinationArrayName = "Mask"
    selectedThresholds = simpl.ComparisonInputs()
    selectedThresholds.addInput("Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index", 1, 0.1)
    selectedThresholds.addInput("Small IN100 Slice 1", "EBSD Scan Data", "Image Quality", 1, 120)
    multiThresholdObjects.SelectedThresholds = selectedThresholds
    multiThresholdObjects.preflight()
    print("  Preflight Error Code: %s" % multiThresholdObjects.ErrorCondition)
    filterName = multiThresholdObjects.NameOfClass
    if filterName != "MultiThresholdObjects":
        print("  Error: Filter class name is not correct. %s != MultiThresholdObjects" % filterName)

    # WriteStatsGenOdfAngleFile
    #
    print("# --- WriteStatsGenOdfAngleFile ")
    writeStatsGenOdfAngleFile = orientationanalysis.WriteStatsGenOdfAngleFile.New()
    writeStatsGenOdfAngleFile.setDataContainerArray(dca)
    writeStatsGenOdfAngleFile.OutputFile = sd.GetTestTempDirectory() + "/StatsGenODF.txt"
    writeStatsGenOdfAngleFile.Weight = 1
    writeStatsGenOdfAngleFile.Sigma = 1
    writeStatsGenOdfAngleFile.Delimiter = 2
    writeStatsGenOdfAngleFile.CellPhasesArrayPath = simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data",
                                                                        "Phases")
    writeStatsGenOdfAngleFile.CellEulerAnglesArrayPath = simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data",
                                                                             "EulerAngles")
    writeStatsGenOdfAngleFile.GoodVoxelsArrayPath = simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "Mask")
    writeStatsGenOdfAngleFile.ConvertToDegrees = True
    writeStatsGenOdfAngleFile.UseGoodVoxels = True
    writeStatsGenOdfAngleFile.preflight()
    print("  Preflight Error Code: %s" % writeStatsGenOdfAngleFile.ErrorCondition)
    filterName = writeStatsGenOdfAngleFile.NameOfClass
    if filterName != "WriteStatsGenOdfAngleFile":
        print("  Error: Filter class name is not correct. %s != WriteStatsGenOdfAngleFile" % filterName)

    # Construct the pipeline
    pipeline = simpl.FilterPipeline.New()
    pipeline.Name = ("New Pipeline")
    print("Pipeline Name: %s " % pipeline.Name)

    pipeline.pushBack(readAngData)
    pipeline.pushBack(rotateEulerRefFrame)
    pipeline.pushBack(rotateSampleRefFrame)
    pipeline.pushBack(multiThresholdObjects)
    pipeline.pushBack(writeStatsGenOdfAngleFile)

    filterCount = pipeline.size()
    print("Filter Count: %d" % filterCount)

    err = pipeline.preflightPipeline()
    print("Preflight ErrorCondition: %d" % err)

    # Run the pipeline
    dataContainer = pipeline.run()
    err = pipeline.ErrorCondition
    print("Execute ErrorCondition: %d" % err)

    pipeline.popFront()
    filterCount = pipeline.size()
    print("Filter Count: %d" % filterCount)

    time.sleep(2)


def PythonicTest(dca):
    err = orientationanalysis_pythonic.read_ang_data(dca, "Small IN100 Slice 1", "Phase Data", "EBSD Scan Data",
                                                     False,
                                                     sd.GetBuildDirectory() + "/Debug/Data/SmallIN100/Slice_1.ang")
    if err < 0:
        print("ReadAngData ErrorCondition: %d" % err)

    err = orientationanalysis_pythonic.rotate_euler_ref_frame(dca, simpl.FloatVec3(0.0, 0.0, 1.0), 90.0,
                                                              simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                  "EBSD Scan Data", "EulerAngles"))
    if err < 0:
        print("RotateEulerRefFrame ErrorCondition: %d" % err)

    err = sampling_pythonic.rotate_sample_ref_frame(dca,
                                                    simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", ""),
                                                    simpl.FloatVec3(0.0, 1.0, 0.0), 180.0, False)
    if err < 0:
        print("RotateSampleRefFrame ErrorCondition: %d" % err)

    # Create the selected thresholds / comparison inputs for MultiThresholdObjects filter
    selectedThresholds = simpl.ComparisonInputs()
    selectedThresholds.addInput("Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index", 1, 0.1)
    selectedThresholds.addInput("Small IN100 Slice 1", "EBSD Scan Data", "Image Quality", 1, 120)

    err = d3d.multi_threshold_objects(dca, "Mask", selectedThresholds)
    if err < 0:
        print("MultiThresholdObjects ErrorCondition: %d" % err)

    am = dca.getAttributeMatrix(simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", ""))
    dataArray = am.getAttributeArray("EulerAngles")
    rawdata = dataArray.Data
    # print(rawdata)
    err = orientationanalysis_pythonic.write_stats_gen_odf_angle_file(dca,
                                                                      sd.GetTestTempDirectory() + "/StatsGenODF.txt",
                                                                      1, 1, 2,
                                                                      simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                          "EBSD Scan Data", "Phases"),
                                                                      simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                          "EBSD Scan Data",
                                                                                          "EulerAngles"),
                                                                      simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                          "EBSD Scan Data", "Mask"),
                                                                      True, True)
    if err < 0:
        print("WriteStatsGenOdfAngleFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    Dream3DTest()
    print("Test Complete")
