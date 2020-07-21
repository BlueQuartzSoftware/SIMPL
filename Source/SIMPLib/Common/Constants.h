/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

/*
  "It's a known, long-standing bug in the compiler system's headers.  For
  some reason the manufacturer, in its infinite wisdom, chose to #define
  macros min() and max() in violation of the upper-case convention and so
  break any legitimate functions with those names, including those in the
  standard C++ library."
  */
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include <limits>
#include <cstdint>

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"

#define DEFAULT_BLOCKSIZE 1048576

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace SIMPL
{

using Rgb = uint32_t;
inline constexpr Rgb RGB_MASK = 0x00ffffff; // masks RGB values
inline const QString PathSep("|");
inline constexpr uint8_t Unchecked = 0;
inline constexpr uint8_t PartiallyChecked = 1;
inline constexpr uint8_t Checked = 2;

enum InfoStringFormat
{
  HtmlFormat = 0,
  MarkDown = 1,
  //      JsonFormat,
  //      TextFormat,
  //      XmlFormat,
  UnknownFormat
};

/** @brief Constants defined for the Stacking order of images into a 3D Volume */
namespace RefFrameZDir
{
inline constexpr uint32_t LowtoHigh = 0;
inline constexpr uint32_t HightoLow = 1;
inline constexpr uint32_t UnknownRefFrameZDirection = 2;
} // namespace RefFrameZDir

namespace TypeNames
{
inline const QString Bool("bool");
inline const QString Float("float");
inline const QString Double("double");
inline const QString Int8("int8_t");
inline const QString UInt8("uint8_t");
inline const QString Int16("int16_t");
inline const QString UInt16("uint16_t");
inline const QString Int32("int32_t");
inline const QString UInt32("uint32_t");
inline const QString Int64("int64_t");
inline const QString UInt64("uint64_t");
inline const QString SizeT("size_t");
inline const QString String("string");
inline const QString StatsDataArray("StatsDataArray");
inline const QString NeighborList("NeighborList<T>");
inline const QString StringArray("StringDataArray");
inline const QString Unknown("Unknown");
inline const QString SupportedTypeList(TypeNames::Bool + ", " + TypeNames::StringArray + ", " + TypeNames::Int8 + ", " + TypeNames::UInt8 + ", " + TypeNames::Int16 + ", " + TypeNames::UInt16 + ", " +
                                TypeNames::Int32 + ", " + TypeNames::UInt32 + ", " + TypeNames::Int64 + ", " + TypeNames::UInt64 + ", " + TypeNames::Float + ", " + TypeNames::Double + ", " +
                                TypeNames::SizeT);
} // namespace TypeNames

namespace TypeEnums
{
inline constexpr int32_t Int8 = 0;
inline constexpr int32_t UInt8 = 1;
inline constexpr int32_t Int16 = 2;
inline constexpr int32_t UInt16 = 3;
inline constexpr int32_t Int32 = 4;
inline constexpr int32_t UInt32 = 5;
inline constexpr int32_t Int64 = 6;
inline constexpr int32_t UInt64 = 7;
inline constexpr int32_t Float = 8;
inline constexpr int32_t Double = 9;
inline constexpr int32_t Bool = 10;
inline constexpr int32_t SizeT = 11;

inline constexpr int32_t UnknownType = 12;
inline const QString SupportedTypeList(TypeNames::Bool + ", " + TypeNames::Int8 + ", " + TypeNames::UInt8 + ", " + TypeNames::Int16 + ", " + TypeNames::UInt16 + ", " + TypeNames::Int32 + ", " +
                                TypeNames::UInt32 + ", " + TypeNames::Int64 + ", " + TypeNames::UInt64 + ", " + TypeNames::Float + ", " + TypeNames::Double + ", " + TypeNames::SizeT);
} // namespace TypeEnums

namespace NumericTypes
{
namespace Names
{
inline const QString Int8("signed int 8 bit");
inline const QString UInt8("unsigned int 8 bit");
inline const QString Int16("signed int 16 bit");
inline const QString UInt16("unsigned int 16 bit");
inline const QString Int32("signed int 32 bit");
inline const QString UInt32("unsigned int 32 bit");
inline const QString Int64("signed int 64 bit");
inline const QString UInt64("unsigned int 64 bit");
inline const QString Float("Float 32 bit");
inline const QString Double("Double 64 bit");
inline const QString Bool("Bool");
inline const QString SizeT("size_t");
} // namespace Names

enum class Type : int32_t
{
  Int8 = 0,
  UInt8,
  Int16,
  UInt16,
  Int32,
  UInt32,
  Int64,
  UInt64,
  Float,
  Double,
  Bool,
  SizeT,
  UnknownNumType
};

inline const QString SupportedTypeList(NumericTypes::Names::Int8 + ", " + NumericTypes::Names::UInt8 + ", " + NumericTypes::Names::Int16 + ", " + NumericTypes::Names::UInt16 + ", " +
                                NumericTypes::Names::Int32 + ", " + NumericTypes::Names::UInt32 + ", " + NumericTypes::Names::Int64 + ", " + NumericTypes::Names::UInt64 + ", " +
                                NumericTypes::Names::Float + ", " + NumericTypes::Names::Double + ", " + NumericTypes::Names::Bool + ", " + NumericTypes::Names::SizeT);
} // namespace NumericTypes

namespace DelimiterTypes
{
enum class Type : int32_t
{
  Comma = 0,
  Semicolon = 1,
  Colon = 2,
  Tab = 3,
  Space = 4
};
}

namespace ScalarTypes
{
enum class Type : int32_t
{
  Int8 = 0,
  UInt8,
  Int16,
  UInt16,
  Int32,
  UInt32,
  Int64,
  UInt64,
  Float,
  Double,
  Bool,
  SizeT
};
}

namespace IO
{
inline const QString DAPSettingsHeader("Path");
}

namespace Defaults
{
inline const QString None("None");
inline const QString AnyPrimitive("Any");
inline const size_t AnyComponentSize = std::numeric_limits<size_t>::max();
// inline constexpr uint32_t AnyAttributeMatrix = std::numeric_limits<uint32_t>::max();
// inline constexpr uint32_t AnyGeometry = std::numeric_limits<uint32_t>::max();

inline const QString AttributeMatrixName("AttributeMatrix");
inline const QString ElementAttributeMatrixName("ElementAttributeMatrix");
inline const QString FeatureAttributeMatrixName("FeatureAttributeMatrix");
inline const QString EnsembleAttributeMatrixName("EnsembleAttributeMatrix");

inline const QString ImageDataContainerName("ImageDataContainer");
inline const QString NewImageDataContainerName("NewImageDataContainer");
inline const QString TriangleDataContainerName("TriangleDataContainer");
inline const QString QuadDataContainerName("QuadDataContainer");
inline const QString TetrahedralDataContainerName("TetrahedralDataContainer");
inline const QString HexahedralDataContainerName("HexahedralDataContainer");

inline const QString VertexDataContainerName("VertexDataContainer");
inline const QString VertexAttributeMatrixName("VertexData");
inline const QString VertexFeatureAttributeMatrixName("VertexFeatureData");
inline const QString VertexEnsembleAttributeMatrixName("VertexEnsembleData");

inline const QString EdgeDataContainerName("EdgeDataContainer");
inline const QString EdgeAttributeMatrixName("EdgeData");
inline const QString EdgeFeatureAttributeMatrixName("EdgeFeatureData");
inline const QString EdgeEnsembleAttributeMatrixName("EdgeEnsembleData");

inline const QString SurfaceDataContainerName("SurfaceDataContainer");
inline const QString FaceAttributeMatrixName("FaceData");
inline const QString FaceFeatureAttributeMatrixName("FaceFeatureData");
inline const QString FaceEnsembleAttributeMatrixName("FaceEnsembleData");

inline const QString VolumeDataContainerName("VolumeDataContainer");
inline const QString NewVolumeDataContainerName("NewVolumeDataContainer");
inline const QString CellAttributeMatrixName("CellData");
inline const QString NewCellAttributeMatrixName("NewCellData");
inline const QString CellFeatureAttributeMatrixName("CellFeatureData");
inline const QString NewCellFeatureAttributeMatrixName("NewCellFeatureData");
inline const QString CellEnsembleAttributeMatrixName("CellEnsembleData");

inline const QString VoxelDataName("VoxelData");

inline const QString SyntheticVolumeDataContainerName("SyntheticVolumeDataContainer");
inline const QString StatsGenerator("StatsGeneratorDataContainer");

inline const QString SomePath("SomeDataContainer|SomeAttributeMatrix|SomeDataArray");

inline const QString GenericBundleName("GenericBundle");
inline const QString TimeSeriesBundleName("TimeSeriesBundle");

inline const QString GenericBundleAttributeMatrixName("GenericBundleAttributeMatrix");
inline const QString TimeSeriesBundleAttributeMatrixName("TimeSeriesBundleAttributeMatrix");

inline const QString DataContainerName("DataContainer");
inline const QString NewDataContainerName("NewDataContainer");
inline const QString NewAttributeMatrixName("NewAttributeMatrixName");
} // namespace Defaults

namespace PipelineVersionNumbers
{
inline constexpr int32_t CurrentVersion(6);
}

namespace FilterGroups
{
inline const QString CoreFilters("Core");
inline const QString Generic("Generic");
inline const QString IOFilters("IO");
inline const QString ProcessingFilters("Processing");
inline const QString ReconstructionFilters("Reconstruction");
inline const QString SamplingFilters("Sampling");
inline const QString StatisticsFilters("Statistics");
inline const QString SyntheticBuildingFilters("Synthetic Building");
inline const QString SurfaceMeshingFilters("Surface Meshing");
inline const QString Utilities("Utilities");
inline const QString CustomFilters("Custom");
inline const QString Unsupported("Unsupported");
} // namespace FilterGroups

namespace FilterSubGroups
{
inline const QString EnsembleStatsFilters("Ensemble");
inline const QString MemoryManagementFilters("Memory/Management");
inline const QString SpatialFilters("Spatial");
inline const QString StatisticsFilters("Statistics");
inline const QString FeatureIdentificationFilters("FeatureIdentification");
inline const QString OutputFilters("Output");
inline const QString InputFilters("Input");
inline const QString ImageFilters("Image");
inline const QString CleanupFilters("Cleanup");
inline const QString ThresholdFilters("Threshold");
inline const QString RegularizationFilters("Regularization");
inline const QString ConversionFilters("Conversion");
inline const QString FusionFilters("Fusion");
inline const QString WarpingFilters("Warping");
inline const QString AlignmentFilters("Alignment");
inline const QString SegmentationFilters("Segmentation");
inline const QString GroupingFilters("Grouping");
inline const QString CropCutFilters("Croping/Cutting");
inline const QString RotationTransformationFilters("Rotating/Transforming");
inline const QString ResolutionFilters("Spacing");
inline const QString MorphologicalFilters("Morphological");
inline const QString PackingFilters("Packing");
inline const QString CrystallographyFilters("Crystallography");
inline const QString GenerationFilters("Generation");
inline const QString SmoothingFilters("Smoothing");
inline const QString CurvatureFilters("Curvature");
inline const QString ConnectivityArrangementFilters("Connectivity/Arrangement");
inline const QString MappingFilters("Mapping");
inline const QString MiscFilters("Misc");
inline const QString GeometryFilters("Geometry");
inline const QString ImportFilters("Import");
inline const QString ExportFilters("Export");
} // namespace FilterSubGroups

namespace GeneralData
{
inline const QString CombinedData("CombinedData");
inline const QString ThresholdArray("ThresholdArray");
inline const QString Mask("Mask");
} // namespace GeneralData

namespace CellData
{
inline const QString AxisAngles("AxisAngles");
inline const QString BC("BandContrasts");
inline const QString BandContrast("BandContrast");
inline const QString CellPhases("Phases");
inline const QString ConfidenceIndex("Confidence Index");
inline const QString CAxisLocation("CAxisLocation");
inline const QString ConfidenceIndexNoSpace("ConfidenceIndex");
inline const QString Current("Current");
inline const QString DislocationTensors("DislocationTensors");
inline const QString EulerAngles("EulerAngles");
inline const QString EulerColor("EulerColor");
inline const QString FarFeatureQuats("FarFeatureQuats");
inline const QString FarFeatureZoneIds("FarFeatureZoneIds");
inline const QString FitQuality("FitQuality");
inline const QString FlatImageData("FlatImageData");
inline const QString GBEuclideanDistances("GBEuclideanDistances");
inline const QString GBManhattanDistances("GBManhattanDistances");
inline const QString GlobAlpha("GlobAlpha");
inline const QString GoodVoxels("GoodVoxels");
inline const QString FeatureIds("FeatureIds");
inline const QString FeatureReferenceCAxisMisorientations("FeatureReferenceCAxisMisorientations");
inline const QString FeatureReferenceMisorientations("FeatureReferenceMisorientations");
inline const QString IPFColor("IPFColor");
inline const QString ImageData("ImageData");
inline const QString ImageQuality("Image Quality");
inline const QString ImageQualityNoSpace("ImageQuality");
inline const QString KernelAverageMisorientations("KernelAverageMisorientations");
inline const QString Mask("Mask");
inline const QString MotionDirection("MotionDirection");
inline const QString MicroTexVolFrac("MicroTexVolFrac");
inline const QString MisorientationColor("MisorientationColor");
inline const QString ParentDensity("ParentDensity");
inline const QString MTRgKAM("MTRgKAM");
inline const QString NearestNeighbors("NearestNeighbors");
inline const QString ParentIds("ParentIds");
inline const QString Phases("Phases");
inline const QString ProjectedImageMin("ProjectedImageMin");
inline const QString ProjectedImageMax("ProjectedImageMax");
inline const QString ProjectedImageAvg("ProjectedImageAvg");
inline const QString ProjectedImageStd("ProjectedImageStd");
inline const QString ProjectedImageVar("ProjectedImageVar");
inline const QString QPEuclideanDistances("QPEuclideanDistances");
inline const QString QPManhattanDistances("QPManhattanDistances");
inline const QString Quats("Quats");
inline const QString RodriguesColor("RodriguesColor");
inline const QString RodriguesVectors("RodriguesVectors");
inline const QString SolidMeshNodes("SolidMeshNodes");
inline const QString SineParams("SineParams");
inline const QString SolidMeshTetrahedrons("SolidMeshTetrahedrons");
inline const QString Speed("Speed");
inline const QString SurfaceMeshCells("SurfaceMeshCells");
inline const QString BoundaryCells("BoundaryCells");
inline const QString TJManhattanDistances("TJManhattanDistances");
inline const QString TJEuclideanDistances("TJEuclideanDistances");
inline const QString VectorColor("VectorColor");
inline const QString VectorData("VectorData");
inline const QString Histogram("Histogram");
} // namespace CellData

namespace FeatureData
{
inline const QString FeatureID("Feature_ID");
inline const QString Active("Active");
inline const QString AspectRatios("AspectRatios");
inline const QString AvgCAxes("AvgCAxes");
inline const QString AvgCAxisMisalignments("AvgCAxisMisalignments");
inline const QString LocalCAxisMisalignments("LocalCAxisMisalignments");
inline const QString UnbiasedLocalCAxisMisalignments("UnbiasedLocalCAxisMisalignments");
inline const QString AvgQuats("AvgQuats");
inline const QString AxisEulerAngles("AxisEulerAngles");
inline const QString AxisLengths("AxisLengths");
inline const QString BasalLoadingFactor("BasalLoadingFactor");
inline const QString BiasedFeatures("BiasedFeatures");
inline const QString CAxisMisalignmentList("CAxisMisalignmentList");
inline const QString Centroids("Centroids");
inline const QString ClusteringList("ClusteringList");
inline const QString ElasticStrains("ElasticStrains");
inline const QString EquivalentDiameters("EquivalentDiameters");
inline const QString SaltykovEquivalentDiameters("SaltykovEquivalentDiameters");
inline const QString EulerAngles("EulerAngles");
inline const QString AvgEulerAngles("AvgEulerAngles");
inline const QString F1List("F1List");
inline const QString F1sptList("F1sptList");
inline const QString F7List("F7List");
inline const QString FarFeatureOrientations("FarFeatureOrientations");
inline const QString FeaturePhases("Phases");
inline const QString GoodFeatures("GoodFeatures");
inline const QString FeatureAvgCAxisMisorientations("FeatureAvgCAxisMisorientations");
inline const QString FeatureAvgMisorientations("FeatureAvgMisorientations");
inline const QString FeatureStdevCAxisMisorientations("FeatureStdevCAxisMisorientations");
inline const QString KernelAvgMisorientations("KernelAvgMisorientations");
inline const QString LMG("LMG");
inline const QString LargestCrossSections("LargestCrossSections");
inline const QString Mask("Mask");
inline const QString MTRdensity("MTRdensity");
inline const QString MTRgKAM("MTRgKAM");

inline const QString MisorientationList("MisorientationList");
inline const QString NeighborList("NeighborList");
inline const QString NeighborhoodList("NeighborhoodList");
inline const QString Neighborhoods("Neighborhoods");
inline const QString NumCells("NumCells");
inline const QString NumElements("NumElements");
inline const QString NumFeaturesPerParent("NumFeaturesPerParent");
inline const QString NumNeighbors("NumNeighbors");
inline const QString Omega3s("Omega3s");
inline const QString ParentIds("ParentIds");
inline const QString Phases("Phases");
inline const QString Poles("Poles");
inline const QString RGBs("RGBs");
inline const QString Schmids("Schmids");
inline const QString SharedSurfaceAreaList("SharedSurfaceAreaList");
inline const QString SlipSystems("SlipSystems");
inline const QString SurfaceAreaVol("SurfaceAreaVolumeRatio");
inline const QString SurfaceFeatures("SurfaceFeatures");
inline const QString SurfaceElementFractions("SurfaceElementFractions");
inline const QString Volumes("Volumes");
inline const QString AvgMisorientations("AvgMisorientations");
inline const QString mPrimeList("mPrimeList");
inline const QString NumBins("NumBins");
inline const QString ScalarAverages("ScalarAverages");

} // namespace FeatureData

namespace EnsembleData
{
inline const QString NumFeatures("NumFeatures");
inline const QString VolFractions("VolFractions");
inline const QString TotalSurfaceAreas("TotalSurfaceAreas");
inline const QString CrystalSymmetry("Crystal Symmetry");
inline const QString CrystalStructures("CrystalStructures");
inline const QString PhaseTypes("PhaseTypes");
inline const QString BravaisLattice("BravaisLattice");
inline const QString PrecipitateFractions("PrecipitateFractions");
inline const QString ShapeTypes("ShapeTypes");
inline const QString Statistics("Statistics");
inline const QString PhaseName("PhaseName");
inline const QString LatticeConstants("LatticeConstants");
inline const QString GBCD("GBCD");
inline const QString GBCDdimensions("GBCDdimensions");
inline const QString FitParameters("FitParameters");
inline const QString MaterialName("MaterialName");
} // namespace EnsembleData

namespace VertexData
{
inline const QString AtomVelocities("AtomVelocities");
inline const QString AtomTypes("AtomTypes");
inline const QString AtomFeatureLabels("AtomFeatureLabels");
inline const QString NumberOfArms("NumberOfArms");
inline const QString NodeConstraints("NodeConstraints");
inline const QString SurfaceMeshNodes("Nodes");
inline const QString SurfaceMeshNodeType("NodeType");
inline const QString SurfaceMeshNodeNormals("NodeNormals");
inline const QString SurfaceMeshNodeFaces("NodeFaces");
} // namespace VertexData

namespace FaceData
{
inline const QString SurfaceMeshFaces("Faces");
inline const QString SurfaceMeshFaceIPFColors("IPFColors");
inline const QString SurfaceMeshFaceMisorientationColors("MisorientationColors");
inline const QString SurfaceMeshFaceSchuhMisorientationColors("SchuhMisorientationColors");
inline const QString SurfaceMeshFaceLabels("FaceLabels");
inline const QString SurfaceMeshFacePhases("Phases");
inline const QString SurfaceMeshF1s("F1s");
inline const QString SurfaceMeshF1spts("F1spts");
inline const QString SurfaceMeshF7s("F7s");
inline const QString SurfaceMeshmPrimes("mPrimes");
inline const QString SurfaceMeshVoxels("SurfaceMeshVoxels");
inline const QString SurfaceMeshFaceCentroids("FaceCentroids");
inline const QString SurfaceMeshFaceAreas("FaceAreas");
inline const QString SurfaceMeshTwinBoundary("TwinBoundary");
inline const QString SurfaceMeshTwinBoundaryIncoherence("TwinBoundaryIncoherence");
inline const QString SurfaceMeshTwinBoundarySchmidFactors("TwinBoundarySchmidFactors");
inline const QString SurfaceMeshFaceDihedralAngles("FaceDihedralAngles");
inline const QString SurfaceMeshFaceNormals("FaceNormals");
inline const QString SurfaceMeshFeatureFaceId("FeatureFaceId");
inline const QString SurfaceMeshGaussianCurvatures("GaussianCurvatures");
inline const QString SurfaceMeshMeanCurvatures("MeanCurvatures");
inline const QString SurfaceMeshPrincipalCurvature1("PrincipalCurvature1");
inline const QString SurfaceMeshPrincipalCurvature2("PrincipalCurvature2");
inline const QString SurfaceMeshPrincipalDirection1("PrincipalDirection1");
inline const QString SurfaceMeshPrincipalDirection2("PrincipalDirection2");
} // namespace FaceData

namespace EdgeData
{
inline const QString DislocationIds("DislocationIds");
inline const QString BurgersVectors("BurgersVectors");
inline const QString SlipPlaneNormals("SlipPlaneNormals");
inline const QString SurfaceMeshEdges("SurfaceMeshEdges");
inline const QString SurfaceMeshUniqueEdges("SurfaceMeshUniqueEdges");
inline const QString SurfaceMeshInternalEdges("SurfaceMeshInternalEdges");
inline const QString SurfaceMeshTriangleEdges("SurfaceMeshTriangleEdges");
inline const QString SurfaceMeshEdgeFaces("SurfaceMeshEdgeFaces");
} // namespace EdgeData

namespace Settings
{
inline const QString Library("Filter Library");
inline const QString PrebuiltPipelines("Prebuilt Pipelines");
inline const QString FavoritePipelines("Favorite Pipelines");
inline const QString PipelineBuilderGroup("PipelineBuilder");
inline const QString FavoriteConfig("favorite_config");
inline const QString NumFilters("Number_Filters");
inline const QString PipelineName("Name");
inline const QString FilterName("Filter_Name");
inline const QString FilterUuid("Filter_Uuid");
inline const QString FilterVersion("FilterVersion");
inline const QString HumanLabel("Filter_Human_Label");
inline const QString FilterEnabled("Filter_Enabled");
inline const QString GroupName("GroupName");
inline const QString SubGroupName("SubGroupName");
inline const QString BrandingString("BrandingString");
inline const QString CompiledLibraryName("CompiledLibraryName");
inline const QString Version("Version");
inline const QString PipelineBuilderGeomertry("PipelineBuilderGeometry");
} // namespace Settings

namespace HDF5
{
inline const QString FileVersion("7.0");
inline const QString FileVersionName("FileVersion");
inline const QString DREAM3DVersion("DREAM3D Version");
inline const QString ObjectType("ObjectType");
inline const QString Rank("Rank");
inline const QString TupleDimensions("TupleDimensions");
inline const QString ComponentDimensions("ComponentDimensions");
inline const QString AxisDimensions("Tuple Axis Dimensions");
inline const QString DataArrayVersion("DataArrayVersion");
} // namespace HDF5

namespace StringConstants
{
inline const QString Alpha("Alpha");
inline const QString Angle("Angle");

inline const QString Average("Average");
inline const QString Axis("Axis");
inline const QString AxisODFWeights("AxisODF-Weights");
inline const QString AxisOrientation("AxisOrientation");
inline const QString Beta("Beta");
inline const QString BetaDistribution("Beta Distribution");
inline const QString BinNumber("BinNumber");
inline const QString BinCount("Bin Count");
inline const QString BinStepSize("Bin Step Size");
inline const QString BoundaryArea("BoundaryArea");
inline const QString BoundaryStatsData("BoundaryStatsData");
inline const QString CellNeighbors("CellNeighbors");

inline const QString CellsContainingVert("CellsContainingVert");
inline const QString CellsName("Cells");
inline const QString CrystalStructure("CrystalStructure");
inline const QString DataContainerGroupName("DataContainers");
inline const QString DataContainerBundleGroupName("DataContainerBundles");
inline const QString DataContainerNames("DataContainerNames");
inline const QString MetaDataArrays("MetaDataArrays");
inline const QString MontageGroupName("Montages");
inline const QString DataContainerType("DataContainerType");
inline const QString AttributeMatrixType("AttributeMatrixType");

inline const QString Dims("Dims");
inline const QString DistributionType("Distribution Type");

inline const QString EdgeNeighbors("EdgeNeighbors");
inline const QString EdgesContainingVert("EdgesContainingVert");
inline const QString EdgesName("Edges");
inline const QString EdgeCentroids("EdgeCentroids");
inline const QString EdgeLengths("EdgeLengths");
inline const QString Euler1("Euler 1");
inline const QString Euler2("Euler 2");
inline const QString Euler3("Euler 3");
inline const QString Exp_k("K");

inline const QString FaceNeighbors("FaceNeighbors");
inline const QString FacesContainingVert("FacesContainingVert");
inline const QString FacesName("Faces");
inline const QString TrianglesName("Triangles");
inline const QString TriangleNeighbors("TriangleNeighbors");
inline const QString TrianglesContainingVert("TrianglesContainingVert");
inline const QString TriangleCentroids("TriangleCentroids");
inline const QString TriangleAreas("TriangleAreas");
inline const QString Frequencies("Frequencies");

inline const QString QuadsName("Quadrilaterals");
inline const QString QuadNeighbors("QuadrilateralNeighbors");
inline const QString QuadsContainingVert("QuadrilateralsContainingVerts");
inline const QString QuadCentroids("QuadrilateralCentroids");
inline const QString QuadAreas("QuadrilateralAreas");

inline const QString TetsName("Tetrahedra");
inline const QString TetNeighbors("TetrahedralNeighbors");
inline const QString TetsContainingVert("TetrahedraContainingVerts");
inline const QString TetCentroids("TetrahedralCentroids");
inline const QString TetVolumes("TetrahedralVolumes");

inline const QString HexasName("Hexahedra");
inline const QString HexNeighbors("HexahedralNeighbors");
inline const QString HexasContainingVert("HexahedraContainingVerts");
inline const QString HexCentroids("HexahedralCentroids");
inline const QString HexVolumes("HexahedralVolumes");

inline const QString VoxelSizes("VoxelSizes");
inline const QString VertexSizes("VertexSizes");

inline const QString GBCD("GBCD");
inline const QString FeatureAvgDisorientation("FeatureAvgDisorientation");
inline const QString Feature_Diameter_Info("Feature_Diameter_Info");
inline const QString Feature_SizeVBoverA_Distributions("FeatureSize Vs B Over A Distributions");
inline const QString Feature_SizeVCoverA_Distributions("FeatureSize Vs C Over A Distributions");
inline const QString Feature_SizeVNeighbors_Distributions("FeatureSize Vs Neighbors Distributions");
inline const QString Feature_SizeVOmega3_Distributions("FeatureSize Vs Omega3 Distributions");
inline const QString Feature_SizeVClustering_Distributions("FeatureSize Vs Clustering Distributions");
inline const QString Feature_Size_Distribution("FeatureSize Distribution");
inline const QString KernelAvgDisorientation("KernelAvgDisorientation");
inline const QString LogNormalDistribution("Log Normal Distribution");
inline const QString MDFWeights("MDF-Weights");
inline const QString MatrixStatsData("MatrixStatsData");
inline const QString MeshFaceNeighborLists("MeshFaceNeighborLists");
inline const QString MicroTextureBins("MicroTextureBins");
inline const QString MinimumValue("Minimum Value");
inline const QString MinMaxNoOfBins("MinMaxNoOfBins");
inline const QString MisorientationBins("MisorientationBins");
inline const QString Neighbor_Feature_ID_List("Neighbor_Feature_ID_List");

inline const QString ODF("ODF");
inline const QString ODFWeights("ODF-Weights");

inline const QString ParentPhase("Parent Phase");
inline const QString PhaseFraction("PhaseFraction");
inline const QString PhaseType("PhaseType");
inline const QString PipelineGroupName("Pipeline");
inline const QString PipelineVersionName("Pipeline Version");
inline const QString PipelineCurrentName("Current Pipeline");
inline const QString PowerLawDistribution("Power Law Distribution");
inline const QString PrecipitateBoundaryFraction("Precipitate Boundary Fraction");
inline const QString PrecipitateStatsData("PrecipitateStatsData");
inline const QString PrimaryStatsData("PrimaryStatsData");
inline const QString PresetName("PresetName");

inline const QString RadialDistFunc("Radial Distribution Function");
inline const QString RdfMinDistance("Min");
inline const QString RdfMaxDistance("Max");
inline const QString RdfBoxDims("BoxDims");
inline const QString RdfBoxRes("BoxRes");

inline const QString Sigma("Sigma");
inline const QString StandardDeviation("Standard Deviation");
inline const QString Statistics("Statistics");
inline const QString Stats("Stats");
inline const QString StatsData("StatsData");
inline const QString StatsType("StatsType");

inline const QString TransformationStatsData("TransformationStatsData");
inline const QString UnknownDistribution("Unknown Distribution");

inline const QString VerticesName("Vertices");
inline const QString VertsName("Verts");
inline const QString ImageName("Image");

inline const QString Weight("Weight");

inline const QString MetaData("_META_DATA");

inline const QString Name("Name");
inline const QString Index("Index");
} // namespace StringConstants

namespace CellType
{
inline const QString Quadrilateral("Quadrilateral");
inline const QString Tetrahedron("Tetrahedron");
inline const QString Triangle("Triangle");
inline const QString Face("Face");
inline const QString Edge("Edge");
inline const QString Vertex("Vertex");
inline const QString Node("Node");
} // namespace CellType

// namespace PhaseType
//{
//  //inline constexpr uint32_t PrimaryPhase = 0;              //!<
//  //inline constexpr uint32_t PrecipitatePhase = 1;          //!<
//  //inline constexpr uint32_t TransformationPhase = 2;       //!<
//  //inline constexpr uint32_t MatrixPhase = 3;              //!<
//  //inline constexpr uint32_t BoundaryPhase = 4;              //!<
//  //inline constexpr uint32_t Unknown = 999;    //!<

//  inline const QString Primary("Primary");
//  inline const QString Precipitate("Precipitate");
//  inline const QString Transformation("Transformation");
//  inline const QString Matrix("Matrix");
//  inline const QString Boundary("Boundary");
//  inline const QString UnknownPhase("Unknown");
//}

namespace AlignmentMethod
{
inline constexpr uint32_t OuterBoundary = 0;            //!<
inline constexpr uint32_t Misorientation = 1;           //!<
inline constexpr uint32_t MutualInformation = 2;        //!<
inline constexpr uint32_t Count = 3;                    //!<
inline constexpr uint32_t UnknownAlignmentMethod = 999; //!<
} // namespace AlignmentMethod

namespace StatisticsType
{
inline constexpr uint32_t Feature_SizeVBoverA = 0;      //!<
inline constexpr uint32_t Feature_SizeVCoverA = 1;      //!<
inline constexpr uint32_t Feature_SizeVNeighbors = 2;   //!<
inline constexpr uint32_t Feature_SizeVOmega3 = 3;      //!<
inline constexpr uint32_t Feature_SizeVClustering = 4;  //!<
inline constexpr uint32_t UnknownStatisticsGroup = 999; //!<

} // namespace StatisticsType

namespace DistributionType
{
inline constexpr uint32_t Beta = 0;                    //!<
inline constexpr uint32_t LogNormal = 1;               //!<
inline constexpr uint32_t Power = 2;                   //!<
inline constexpr uint32_t RDFFrequency = 3;            //!<
inline constexpr uint32_t RDFMaxMin = 4;               //!<
inline constexpr uint32_t UnknownDistributionType = 5; //!<
inline constexpr uint32_t Count = 6;                   //!<

enum ColumnCount
{
  BetaColumnCount = 2,        //!<
  LogNormalColumnCount = 2,   //!<
  PowerLawColumnCount = 2,    //!<
  RawDistDataColumnCount = 1, //!<
  UnknownColumCount = 0       //!<
};

} // namespace DistributionType

namespace EulerAngleConversionType
{
inline constexpr uint32_t DegreesToRadians = 0; //!<
inline constexpr uint32_t RadiansToDegrees = 1; //!<
} // namespace EulerAngleConversionType

namespace FlattenImageMethod
{
inline constexpr uint32_t Lightness = 0;  //!<
inline constexpr uint32_t Average = 1;    //!<
inline constexpr uint32_t Luminosity = 2; //!<
} // namespace FlattenImageMethod

namespace EulerFrameRotationAxis
{
inline constexpr uint32_t RD = 0;   //!<
inline constexpr uint32_t TD = 1;   //!<
inline constexpr uint32_t ND = 2;   //!<
inline constexpr uint32_t None = 3; //!<
} // namespace EulerFrameRotationAxis

namespace SampleFrameRotationAxis
{
inline constexpr uint32_t X = 0;    //!<
inline constexpr uint32_t Y = 1;    //!<
inline constexpr uint32_t Z = 2;    //!<
inline constexpr uint32_t None = 3; //!<
} // namespace SampleFrameRotationAxis

namespace RefFrameRotationAngle
{
inline constexpr uint32_t Ninety = 0;     //!<
inline constexpr uint32_t oneEighty = 1;  //!<
inline constexpr uint32_t twoSeventy = 2; //!<
inline constexpr uint32_t Mirror = 3;     //!<
inline constexpr uint32_t Zero = 4;       //!<
} // namespace RefFrameRotationAngle

namespace Overlap
{
inline constexpr uint32_t Rigid = 0;            //!<
inline constexpr uint32_t Progressive = 1;      //!<
inline constexpr uint32_t UnknownOverlap = 999; //!<
} // namespace Overlap

//  namespace DataContainerType
//  {
//    inline constexpr uint32_t VolumeDataContainer = 0; //!<
//    inline constexpr uint32_t SurfaceDataContainer = 1; //!<
//    inline constexpr uint32_t EdgeDataContainer = 2; //!<
//    inline constexpr uint32_t VertexDataContainer = 3; //!<
//    inline constexpr uint32_t UnknownDataContainer = 999; //!<
//  }

//  namespace AttributeMatrixType
//  {
//    inline constexpr uint32_t Vertex = 0; //!<
//    inline constexpr uint32_t Edge = 1; //!<
//    inline constexpr uint32_t Face = 2; //!<
//    inline constexpr uint32_t Cell = 3; //!<
//    inline constexpr uint32_t VertexFeature = 4; //!<
//    inline constexpr uint32_t EdgeFeature = 5; //!<
//    inline constexpr uint32_t FaceFeature = 6; //!<
//    inline constexpr uint32_t CellFeature = 7; //!<
//    inline constexpr uint32_t VertexEnsemble = 8; //!<
//    inline constexpr uint32_t EdgeEnsemble = 9; //!<
//    inline constexpr uint32_t FaceEnsemble = 10; //!<
//    inline constexpr uint32_t CellEnsemble = 11; //!<
//    inline constexpr uint32_t MetaData = 12; //!<
//    inline constexpr uint32_t Generic = 13; //!<
//    inline constexpr uint32_t Unknown = 999; //!<
//  }

//  namespace AttributeMatrixObjectType
//  {
//    inline constexpr uint32_t Element = 0;
//    inline constexpr uint32_t Feature = 1;
//    inline constexpr uint32_t Ensemble = 2;
//    inline constexpr uint32_t Any = 3;
//  }

namespace Geometry
{
inline const QString Geometry("_SIMPL_GEOMETRY");
inline const QString GeometryName("GeometryName");
inline const QString GeometryType("GeometryType");
inline const QString GeometryTypeName("GeometryTypeName");
inline const QString NumberOfTuples("NumberOfTuples");
inline const QString UnitDimensionality("UnitDimensionality");
inline const QString SpatialDimensionality("SpatialDimensionality");

inline const QString AnyGeometry("AnyGeometry");
inline const QString UnknownGeometry("UnkownGeometry");
inline const QString ImageGeometry("ImageGeometry");
inline const QString RectGridGeometry("RectGridGeometry");
inline const QString VertexGeometry("VertexGeometry");
inline const QString EdgeGeometry("EdgeGeometry");
inline const QString TriangleGeometry("TriangleGeometry");
inline const QString QuadGeometry("QuadrilateralGeometry");
inline const QString TetrahedralGeometry("TetrahedralGeometry");
inline const QString HexahedralGeometry("HexahedralGeometry");

inline const QString xBoundsList("xBounds");
inline const QString yBoundsList("yBounds");
inline const QString zBoundsList("zBounds");
inline const QString SharedVertexList("SharedVertexList");
inline const QString SharedEdgeList("SharedEdgeList");
inline const QString SharedTriList("SharedTriList");
inline const QString SharedQuadList("SharedQuadList");
inline const QString SharedTetList("SharedTetList");
inline const QString SharedHexList("SharedHexList");
inline const QString UnsharedEdgeList("UnsharedEdgeList");
inline const QString UnsharedFaceList("UnsharedFaceList");

inline const QString TransformContainerGroup("TransformContainerGroup");
inline const QString TransformContainer("TransformContainer");
inline const QString CompositeTransformContainer("CompositeTransformContainer");
inline const QString TransformContainerTypeName("TransformContainerTypeName");
inline const QString UnknownTransformContainer("UnknownTransformContainer");
inline const QString TransformContainerParameters("TransformParameters");
inline const QString TransformContainerFixedParameters("TransformFixedParameters");
inline const QString TransformContainerTypeAsString("TransformType");
inline const QString TransformContainerMovingName("TransformMovingName");
inline const QString TransformContainerReferenceName("TransformReferenceName");

inline const QString k_Yoctometer("Yoctometer");
inline const QString k_Zeptometer("Zeptometer");
inline const QString k_Attometer("Attometer");
inline const QString k_Femtometer("Femtometer");
inline const QString k_Picometer("Picometer");
inline const QString k_Nanometer("Nanometer");
inline const QString k_Micrometer("Micrometer");
inline const QString k_Millimeter("Millimeter");
inline const QString k_Centimeter("Centimeter");
inline const QString k_Decimeter("Decimeter");
inline const QString k_Meter("Meter");
inline const QString k_Decameter("Decameter");
inline const QString k_Hectometer("Hectometer");
inline const QString k_Kilometer("Kilometer");
inline const QString k_Megameter("Megameter");
inline const QString k_Gigameter("Gigameter");
inline const QString k_Terameter("Terameter");
inline const QString k_Petameter("Petameter");
inline const QString k_Exameter("Exameter");
inline const QString k_Zettameter("Zettameter");
inline const QString k_Yottameter("Yottameter");
inline const QString k_Angstrom("Angstrom");
inline const QString k_Mil("Mil");
inline const QString k_Inch("Inch");
inline const QString k_Foot("Foot");
inline const QString k_Mile("Mile");
inline const QString k_Fathom("Fathom");
inline const QString k_Unspecified("Unspecified");
inline const QString k_Unknown("Unknown");
} // namespace Geometry

//  namespace GeometryType
//  {
//    inline constexpr uint32_t ImageGeometry = 11;
//    inline constexpr uint32_t RectGridGeometry = 11;
//    inline constexpr uint32_t VertexGeometry = 1;
//    inline constexpr uint32_t EdgeGeometry = 3;
//    inline constexpr uint32_t TriangleGeometry = 5;
//    inline constexpr uint32_t QuadGeometry = 9;
//    inline constexpr uint32_t TetrahedralGeometry = 10;
//    inline constexpr uint32_t UnknownGeometry = 999;
//  }

namespace XdmfGridType
{
inline constexpr uint32_t PolyData = 0;
inline constexpr uint32_t RectilinearGrid = 1;
inline constexpr uint32_t UnknownGrid = 0xFFFFFFFF;
} // namespace XdmfGridType

namespace XdmfCenterType
{
inline const QString Node("Node");
inline const QString Edge("Edge");
inline const QString Face("Face");
inline const QString Cell("Cell");
inline const QString Grid("Grid");
} // namespace XdmfCenterType

// namespace ShapeType
//{
//  inline constexpr uint32_t EllipsoidShape = 0; //!<
//  inline constexpr uint32_t SuperEllipsoidShape = 1; //!<
//  inline constexpr uint32_t CubeOctahedronShape = 2; //!<
//  inline constexpr uint32_t CylinderAShape = 3; //!<
//  inline constexpr uint32_t CylinderBShape = 4; //!<
//  inline constexpr uint32_t CylinderCShape = 5; //!<
//  inline constexpr uint32_t ShapeTypeEnd = 6;
//  inline constexpr uint32_t UnknownShapeType = 999; //!<
//}

enum class Precipitates : uint32_t
{
  NoPrecipitates = 0,       //!<
  BoundaryPrecipitates = 1, //!<
  BulkPrecipitates = 2,     //!<
  UnknownPrecipitates = 999 //!<
};

namespace Reconstruction
{
/*    Reconstruction related */
inline const QString H5VoxelFile("VoxelData.h5voxel");

inline const QString VisualizationVizFile("Visualization.vtk");           // 11
inline const QString DownSampledVizFile("DownSampled_Visualization.vtk"); // 11
inline const QString HDF5FeatureFile("Features.h5feature");
} // namespace Reconstruction

namespace SyntheticBuilder
{
inline const QString FeatureDataFile("FeatureData.csv");
inline const QString H5VoxelFile("VoxelData.h5voxel");

inline const QString VisualizationVizFile("Visualization.vtk");
inline const QString HDF5FeatureFile("Features.h5feature");

inline const QString ErrorFile("Error.txt");
inline const QString VtkFile("Test.vtk");
} // namespace SyntheticBuilder

/*   Surface Meshing Related   */
namespace SurfaceMesh
{
namespace NodeType
{
inline constexpr int8_t Unused = 0;
inline constexpr int8_t Default = 2;
inline constexpr int8_t TriplePoint = 3;
inline constexpr int8_t QuadPoint = 4;
inline constexpr int8_t SurfaceDefault = 12;
inline constexpr int8_t SurfaceTriplePoint = 13;
inline constexpr int8_t SurfaceQuadPoint = 14;
} // namespace NodeType
namespace NodeId
{
inline constexpr int8_t Unused = -1;
}
} // namespace SurfaceMesh

namespace SolidMeshing
{
/* Solid Meshing Related */
inline const QString MeshFile("solid_mesh_v5_1.vtk");
inline const QString MeshFile2("solid_mesh_v5_2.vtk");
inline const QString ElementQualityFile("element_quality_measures_v5.txt");
inline const QString VoxelsFile("voxels_v5.txt");
} // namespace SolidMeshing

namespace Union
{
namespace Strings
{
inline const QString And("And");
inline const QString Or("Or");
} // namespace Strings
enum Enumeration
{
  Operator_And = 0,
  Operator_Or,
  Operator_Unknown
};
} // namespace Union

namespace Comparison
{
namespace Strings
{
inline const QString LessThan("<");
inline const QString GreaterThan(">");
inline const QString Equal("=");
inline const QString NotEqual("!=");
} // namespace Strings
enum Enumeration
{
  Operator_LessThan = 0,
  Operator_GreaterThan,
  Operator_Equal,
  Operator_NotEqual,
  Operator_Unknown
};
} // namespace Comparison

namespace Layout
{
inline constexpr int32_t Horizontal = 0;
inline constexpr int32_t Vertical = 1;
inline constexpr int32_t Square = 2;
}; // namespace Layout
} // namespace SIMPL

namespace Core
{
inline const QString CoreBaseName("Core");
}

namespace Test
{
inline const QString TestPluginFile("TestPlugin");
inline const QString TestPluginDisplayName("Test Plugin");
inline const QString TestBaseName("Test");
} // namespace Test

Q_DECLARE_METATYPE(SIMPL::NumericTypes::Type)
Q_DECLARE_METATYPE(SIMPL::ScalarTypes::Type)
Q_DECLARE_METATYPE(SIMPL::DelimiterTypes::Type)

#if 0
namespace Generic
{
  inline const QString GenericPluginFile("GenericPlugin");
  inline const QString GenericPluginDisplayName("Generic Plugin");
  inline const QString GenericBaseName("Generic");
}

namespace IO
{
  inline const QString IOPluginFile("IOPlugin");
  inline const QString IOPluginDisplayName("IO Plugin");
  inline const QString IOBaseName("IO");
}
#endif
