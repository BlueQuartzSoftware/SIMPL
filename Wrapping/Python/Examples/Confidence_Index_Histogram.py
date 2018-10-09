# Based on Confidence Index Histogram example
# 

# These are the simpl_py python modules

import dream3d
import dream3d.core as d3d
from dream3d.dream3d import simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd
import dream3d.dream3d.orientationanalysis as orientationanalysis
import dream3d.orientation_analysis as orientationanalysis_pythonic
import dream3d.dream3d.sampling as sampling
import dream3d.sampling as sampling_pythonic
import dream3d.dream3d.statistics as statistics
import dream3d.statistics as statistics_pythonic

def ConfidenceIndexHistogramTest():
  # Create Data Container Array
  dca = simpl.DataContainerArray.New()

  # ReadAngData
  err = orientationanalysis_pythonic.read_ang_data(dca, "Small IN100 Slice 1", "Phase Data", "EBSD Scan Data",
   False, sd.GetBuildDirectory() + "/Debug/Data/SmallIN100/Slice_1.ang")
  if err < 0:
    print("ReadAngData ErrorCondition: %d" % err)

  err = sampling_pythonic.rotate_sample_ref_frame(dca, simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", ""),
   simpl.FloatVec3(0.0, 1.0, 0.0), 180.0, False)
  if err < 0:
    print("RotateSampleRefFrame ErrorCondition: %d" % err)

  err = orientationanalysis_pythonic.rotate_euler_ref_frame(dca, simpl.FloatVec3(0.0, 0.0, 1.0), 90.0,
   simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "EulerAngles"))
  if err < 0:
    print("RotateEulerRefFrame ErrorCondition: %d" % err)

  # Create the selected thresholds / comparison inputs for MultiThresholdObjects filter
  selectedThresholds = simpl.ComparisonInputs()
  selectedThresholds.addInput("Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index", 0, 0)
  
  err = sc.MultiThresholdObjects(dca, "Mask", [("Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index", "<", 0)])
  if err < 0:
    print("MultiThresholdObjects ErrorCondition: %d" % err)

  # Replace Value in Array (Conditional)
  err = d3d.conditional_set_value(dca, simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index"), 
  simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "Mask"), 0)
  if err < 0:
    print("ConditionalSetValue ErrorCondition: %d" % err)

  # Calculate Frequence Histogram
  err = statistics_pythonic.calculate_array_histogram(dca, simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index"), 
  50, 0, 1, False, 0, "Histograms", "CI_Histogram", False, "")
  if err < 0:
    print("CalculatArrayHistogram ErrorCondition: %d" % err)

  # Export ASCII Data
  selectedDataArrayPaths = [simpl.DataArrayPath("Small IN100 Slice 1", "Histograms", "CI_Histogram")]
  err = d3d.write_ascii_data(dca, selectedDataArrayPaths, sd.GetBuildDirectory() + "/Debug/Data/Output/Histograms", 0, ".csv", 1)
  if err < 0:
    print("WriteAsciiData ErrorCondition: %d" % err)

"""
Main entry point for python script
"""
if __name__ == "__main__":
  ConfidenceIndexHistogramTest()