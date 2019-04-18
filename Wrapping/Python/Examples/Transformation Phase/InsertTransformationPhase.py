# Insert Transformation Phase
# Based on InsertTransformationPhase example pipeline

from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import simplpy
from dream3d import orientationanalysispy
from dream3d import genericpy
from dream3d import statisticspy
from dream3d import syntheticbuildingpy
from dream3d import transformationphasepy


def insert_transformation_phase():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Generate Primary StatsData
    err = sc.GenerateStatsData(dca, "Primary", "Primary", 0, 1, 0,
                               1, 2.3, 0.4, 4, 3, 10,
                               True, "StatsGeneratorDataContainer",
                               "CellEnsembleData", False,
                               simpl.DataArrayPath("", "", ""),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ["Euler 1", "Euler 2",
                                                                                              "Euler 3", "Weight",
                                                                                              "Sigma"], ["0"]),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ["Angle(w)", "Axis (h)",
                                                                                              "Axis (k)", "Axis (l)",
                                                                                              "Weight (MRD)"], ["0"]),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ["Euler 1", "Euler 2",
                                                                                              "Euler 3", "Weight",
                                                                                              "Sigma"], ["0"]))

    if err < 0:
        print("Primary StatsData ErrorCondition: %d" % err)

    # Initialize Synthetic Volume
    err = syntheticbuildingpy.initialize_synthetic_volume(dca, "SyntheticVolumeDataContainer", "CellData",
                                                          "CellEnsembleData", simpl.IntVec3Type(128, 128, 128),
                                                          simpl.FloatVec3Type(0.5, 0.5, 0.5),
                                                          simpl.FloatVec3Type(0, 0, 0),
                                                          simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                              "CellEnsembleData", "Statistics"),
                                                          simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                              "CellEnsembleData", "PhaseTypes"),
                                                          simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                              "CellEnsembleData", "PhaseName"),
                                                          False)
    if err < 0:
        print("InitializeSyntheticVolume ErrorCondition: %d" % err)

    # Establish Shape Types
    err = syntheticbuildingpy.establish_shape_types(dca,
                                                    simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                        "CellEnsembleData", "PhaseTypes"),
                                                    "ShapeTypes", [simpl.ShapeType.Ellipsoid])
    if err < 0:
        print("EstablishShapeTypes ErrorCondition: %d" % err)

    # Pack Primary Phases
    err = syntheticbuildingpy.pack_primary_phases(dca,
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData",
                                                                      ""),
                                                  "CellFeatureData", "CellEnsembleData", "FeatureIds", "Phases",
                                                  "Phases", "NumFeatures",
                                                  simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                      "CellEnsembleData", "Statistics"),
                                                  simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                      "CellEnsembleData", "PhaseTypes"),
                                                  simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                      "CellEnsembleData", "PhaseName"),
                                                  simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                      "CellEnsembleData", "ShapeTypes"),
                                                  simpl.DataArrayPath("", "", ""), False, 0, "", "", False,
                                                  False, False,
                                                  simpl.DataArrayPath("", "", ""),
                                                  simpl.DataArrayPath("", "", ""))
    if err < 0:
        print("PackPrimaryPhases ErrorCondition: %d" % err)

    # Find Feature Neighbors
    err = statisticspy.find_neighbors(dca, simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellFeatureData", ""),
                                      "SharedSurfaceAreaList", "NeighborList",
                                      simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData",
                                                          "FeatureIds"),
                                      "", "NumNeighbors", "SurfaceFeatures", False, True)
    if err < 0:
        print("FindNeighbors ErrorCondition: %d" % err)

    # Match Crystallography
    err = syntheticbuildingpy.match_crystallography(dca, simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                             "CellEnsembleData", "Statistics"),
                                                    simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                        "CellEnsembleData", "CrystalStructures"),
                                                    simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                        "CellEnsembleData", "PhaseTypes"),
                                                    simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                        "CellData", "FeatureIds"),
                                                    simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                        "CellFeatureData", "Phases"),
                                                    simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                        "CellFeatureData", "SurfaceFeatures"),
                                                    simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                        "CellFeatureData", "NeighborList"),
                                                    simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                        "CellFeatureData", "SharedSurfaceAreaList"),
                                                    simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                        "CellEnsembleData", "NumFeatures"),
                                                    "EulerAngles", "Volumes", "EulerAngles", "AvgQuats", 100000)
    if err < 0:
        print("MatchCrystallography ErrorCondition: %d" % err)

    # Find Feature Centroids
    err = genericpy.find_feature_centroids(dca,
                                           simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                               "CellData", "FeatureIds"),
                                           simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                               "CellFeatureData", "Centroids"))
    if err < 0:
        print("FindFeatureCentroids ErrorCondition: %d" % err)

    # Find Feature Sizes #1
    err = statisticspy.find_sizes(dca, simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellFeatureData", ""),
                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                      "CellData", "FeatureIds"),
                                  "Volumes2", "EquivalentDiameters", "NumElements", False)
    if err < 0:
        print("FindSizes #1 ErrorCondition %d" % err)

    # Insert Transformation Phases
    # Possible candidate for a helper function
    # Possible enums: trans_crystal_struct
    # May want to use a "dictionary" for the numerical portion in helper function
    err = transformationphasepy.insert_transformation_phases(dca,
                                                             1, 1, 60, simpl.FloatVec3Type(1, 1, 1), True, True, 1, 0.2, 1,
                                                             0,
                                                             simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                                 "CellEnsembleData", ""),
                                                             simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                                 "CellFeatureData", ""),
                                                             simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                                 "CellData", "FeatureIds"),
                                                             simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                                 "CellData", "EulerAngles"),
                                                             simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                                 "CellData", "Phases"),
                                                             simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                                 "CellFeatureData", "AvgQuats"),
                                                             simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                                 "CellFeatureData", "Centroids"),
                                                             simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                                 "CellFeatureData",
                                                                                 "EquivalentDiameters"),
                                                             simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                                 "CellFeatureData", "EulerAngles"),
                                                             simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                                 "CellFeatureData", "Phases"),
                                                             "ParentIds",
                                                             simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                                 "CellEnsembleData",
                                                                                 "NumFeaturesPerParent"),
                                                             simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                                 "CellEnsembleData",
                                                                                 "CrystalStructures"),
                                                             simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                                 "CellEnsembleData", "PhaseTypes"),
                                                             simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                                 "CellEnsembleData", "ShapeTypes"),
                                                             simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                                 "CellEnsembleData", "NumFeatures"))
    if err < 0:
        print("InsertTransformationPhases ErrorCondition %d" % err)

    # Delete Data (using helper function)
    sc.RemoveArrays(dca, [("SyntheticVolumeDataContainer", "CellFeatureData", "AvgQuats"),
                          ("SyntheticVolumeDataContainer", "CellFeatureData", "Centroids"),
                          ("SyntheticVolumeDataContainer", "CellFeatureData", "EquivalentDiameters"),
                          ("SyntheticVolumeDataContainer", "CellFeatureData", "EulerAngles"),
                          ("SyntheticVolumeDataContainer", "CellFeatureData", "NeighborList"),
                          ("SyntheticVolumeDataContainer", "CellFeatureData", "NumElements"),
                          ("SyntheticVolumeDataContainer", "CellFeatureData", "NumFeaturesPerParent"),
                          ("SyntheticVolumeDataContainer", "CellFeatureData", "NumNeighbors"),
                          ("SyntheticVolumeDataContainer", "CellFeatureData", "SharedSurfaceAreaList"),
                          ("SyntheticVolumeDataContainer", "CellFeatureData", "SurfaceFeatures"),
                          ("SyntheticVolumeDataContainer", "CellFeatureData", "Volumes"),
                          ("SyntheticVolumeDataContainer", "CellFeatureData", "Volumes2")])

    # Generate IPF Colors
    err = orientationanalysispy.generate_ipf_colors(dca, simpl.FloatVec3Type(0, 0, 1),
                                                    simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                        "CellData", "Phases"),
                                                    simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                        "CellData", "EulerAngles"),
                                                    simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                        "CellEnsembleData", "CrystalStructures"),
                                                    False,
                                                    simpl.DataArrayPath("", "", ""), "IPFColor")
    if err < 0:
        print("GenerateIPFColors ErrorCondition: %d" % err)

    # Find Feature Sizes #2
    err = statisticspy.find_sizes(dca, simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellFeatureData", ""),
                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                      "CellData", "FeatureIds"),
                                  "Volumes2", "EquivalentDiameters", "NumElements", False)
    if err < 0:
        print("FindSizes #2 ErrorCondition %d" % err)

    # Convert Orientation Representation
    err = orientationanalysispy.convert_orientations(dca, simpl.OrientationType.Euler, simpl.OrientationType.Quaternion,
                                                     simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData",
                                                                         "EulerAngles"),
                                                     "Quats")
    if err < 0:
        print("Convert Orientations ErrorCondition %d" % err)

    # Find Feature Average Orientations
    err = orientationanalysispy.find_avg_orientations(dca,
                                                      simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData",
                                                                          "FeatureIds"),
                                                      simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData",
                                                                          "Phases"),
                                                      simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData",
                                                                          "Quats"),
                                                      simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                          "CellEnsembleData", "CrystalStructures"),
                                                      simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                          "CellFeatureData", "AvgQuats"),
                                                      simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                          "CellFeatureData", "AvgEulers"))
    if err < 0:
        print("FindAvgOrientations ErrorCondition: %d" % err)

    # Find Feature Centroids
    err = genericpy.find_feature_centroids(dca, simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData",
                                                                    "FeatureIds"),
                                           simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellFeatureData",
                                                               "Centroids"))
    if err < 0:
        print("FindFeatureCentroids ErrorCondition: %d" % err)

    # Find Feature Neighborhoods
    err = statisticspy.find_neighborhoods(dca, "NeighborhoodList", 1,
                                          simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellFeatureData",
                                                              "EquivalentDiameters"),
                                          simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellFeatureData",
                                                              "Phases"),
                                          simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellFeatureData",
                                                              "Centroids"),
                                          "Neighborhoods")
    if err < 0:
        print("FindNeighborhoods ErrorCondition: %d" % err)

    # Find Feature Neighbors #2
    err = statisticspy.find_neighbors(dca, simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellFeatureData", ""),
                                      "SharedSurfaceAreaList", "NeighborList",
                                      simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData", "FeatureIds"),
                                      "", "NumNeighbors", "SurfaceFeatures", False, False)
    if err < 0:
        print("FindNeighbors #2 ErrorCondition: %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(
        sd.GetBuildDirectory() + "/Debug/Data/Output/TransformationPhase/InsertTransformationPhases.dream3d",
        dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)

    # Export Feature Data as CSV File
    err = simplpy.feature_data_csv_writer(dca,
                                          simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellFeatureData", ""),
                                          sd.GetBuildDirectory() +
                                          "/Debug/Data/Output/TransformationPhase/InsertTransformationPhases.csv",
                                          False, simpl.DelimiterTypes.Comma, True)
    if err < 0:
        print("FeatureDataCsvWriter ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    insert_transformation_phase()
