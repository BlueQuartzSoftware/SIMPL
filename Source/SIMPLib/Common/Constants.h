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
constexpr Rgb RGB_MASK = 0x00ffffff; // masks RGB values
const QString PathSep("|");
constexpr uint8_t Unchecked = 0;
constexpr uint8_t PartiallyChecked = 1;
constexpr uint8_t Checked = 2;

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
constexpr uint32_t LowtoHigh = 0;
constexpr uint32_t HightoLow = 1;
constexpr uint32_t UnknownRefFrameZDirection = 2;
} // namespace RefFrameZDir

namespace TypeNames
{
const QString Bool("bool");
const QString Float("float");
const QString Double("double");
const QString Int8("int8_t");
const QString UInt8("uint8_t");
const QString Int16("int16_t");
const QString UInt16("uint16_t");
const QString Int32("int32_t");
const QString UInt32("uint32_t");
const QString Int64("int64_t");
const QString UInt64("uint64_t");
const QString SizeT("size_t");
const QString String("string");
const QString StatsDataArray("StatsDataArray");
const QString NeighborList("NeighborList<T>");
const QString StringArray("StringDataArray");
const QString Unknown("Unknown");
const QString SupportedTypeList(TypeNames::Bool + ", " + TypeNames::StringArray + ", " + TypeNames::Int8 + ", " + TypeNames::UInt8 + ", " + TypeNames::Int16 + ", " + TypeNames::UInt16 + ", " +
                                TypeNames::Int32 + ", " + TypeNames::UInt32 + ", " + TypeNames::Int64 + ", " + TypeNames::UInt64 + ", " + TypeNames::Float + ", " + TypeNames::Double + ", " +
                                TypeNames::SizeT);
} // namespace TypeNames

namespace TypeEnums
{
constexpr int32_t Int8 = 0;
constexpr int32_t UInt8 = 1;
constexpr int32_t Int16 = 2;
constexpr int32_t UInt16 = 3;
constexpr int32_t Int32 = 4;
constexpr int32_t UInt32 = 5;
constexpr int32_t Int64 = 6;
constexpr int32_t UInt64 = 7;
constexpr int32_t Float = 8;
constexpr int32_t Double = 9;
constexpr int32_t Bool = 10;
constexpr int32_t SizeT = 11;

constexpr int32_t UnknownType = 12;
const QString SupportedTypeList(TypeNames::Bool + ", " + TypeNames::Int8 + ", " + TypeNames::UInt8 + ", " + TypeNames::Int16 + ", " + TypeNames::UInt16 + ", " + TypeNames::Int32 + ", " +
                                TypeNames::UInt32 + ", " + TypeNames::Int64 + ", " + TypeNames::UInt64 + ", " + TypeNames::Float + ", " + TypeNames::Double + ", " + TypeNames::SizeT);
} // namespace TypeEnums

namespace NumericTypes
{
namespace Names
{
const QString Int8("signed int 8 bit");
const QString UInt8("unsigned int 8 bit");
const QString Int16("signed int 16 bit");
const QString UInt16("unsigned int 16 bit");
const QString Int32("signed int 32 bit");
const QString UInt32("unsigned int 32 bit");
const QString Int64("signed int 64 bit");
const QString UInt64("unsigned int 64 bit");
const QString Float("Float 32 bit");
const QString Double("Double 64 bit");
const QString Bool("Bool");
const QString SizeT("size_t");
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

const QString SupportedTypeList(NumericTypes::Names::Int8 + ", " + NumericTypes::Names::UInt8 + ", " + NumericTypes::Names::Int16 + ", " + NumericTypes::Names::UInt16 + ", " +
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
const QString DAPSettingsHeader("Path");
}

namespace Defaults
{
const QString None("None");
const QString AnyPrimitive("Any");
const size_t AnyComponentSize = std::numeric_limits<size_t>::max();
// constexpr uint32_t AnyAttributeMatrix = std::numeric_limits<uint32_t>::max();
// constexpr uint32_t AnyGeometry = std::numeric_limits<uint32_t>::max();

const QString AttributeMatrixName("AttributeMatrix");
const QString ElementAttributeMatrixName("ElementAttributeMatrix");
const QString FeatureAttributeMatrixName("FeatureAttributeMatrix");
const QString EnsembleAttributeMatrixName("EnsembleAttributeMatrix");

const QString ImageDataContainerName("ImageDataContainer");
const QString NewImageDataContainerName("NewImageDataContainer");
const QString TriangleDataContainerName("TriangleDataContainer");
const QString QuadDataContainerName("QuadDataContainer");
const QString TetrahedralDataContainerName("TetrahedralDataContainer");
const QString HexahedralDataContainerName("HexahedralDataContainer");

const QString VertexDataContainerName("VertexDataContainer");
const QString VertexAttributeMatrixName("VertexData");
const QString VertexFeatureAttributeMatrixName("VertexFeatureData");
const QString VertexEnsembleAttributeMatrixName("VertexEnsembleData");

const QString EdgeDataContainerName("EdgeDataContainer");
const QString EdgeAttributeMatrixName("EdgeData");
const QString EdgeFeatureAttributeMatrixName("EdgeFeatureData");
const QString EdgeEnsembleAttributeMatrixName("EdgeEnsembleData");

const QString SurfaceDataContainerName("SurfaceDataContainer");
const QString FaceAttributeMatrixName("FaceData");
const QString FaceFeatureAttributeMatrixName("FaceFeatureData");
const QString FaceEnsembleAttributeMatrixName("FaceEnsembleData");

const QString VolumeDataContainerName("VolumeDataContainer");
const QString NewVolumeDataContainerName("NewVolumeDataContainer");
const QString CellAttributeMatrixName("CellData");
const QString NewCellAttributeMatrixName("NewCellData");
const QString CellFeatureAttributeMatrixName("CellFeatureData");
const QString NewCellFeatureAttributeMatrixName("NewCellFeatureData");
const QString CellEnsembleAttributeMatrixName("CellEnsembleData");

const QString VoxelDataName("VoxelData");

const QString SyntheticVolumeDataContainerName("SyntheticVolumeDataContainer");
const QString StatsGenerator("StatsGeneratorDataContainer");

const QString SomePath("SomeDataContainer|SomeAttributeMatrix|SomeDataArray");

const QString GenericBundleName("GenericBundle");
const QString TimeSeriesBundleName("TimeSeriesBundle");

const QString GenericBundleAttributeMatrixName("GenericBundleAttributeMatrix");
const QString TimeSeriesBundleAttributeMatrixName("TimeSeriesBundleAttributeMatrix");

const QString DataContainerName("DataContainer");
const QString NewDataContainerName("NewDataContainer");
const QString NewAttributeMatrixName("NewAttributeMatrixName");
} // namespace Defaults

namespace PipelineVersionNumbers
{
constexpr int32_t CurrentVersion(6);
}

namespace FilterGroups
{
const QString CoreFilters("Core");
const QString Generic("Generic");
const QString IOFilters("IO");
const QString ProcessingFilters("Processing");
const QString ReconstructionFilters("Reconstruction");
const QString SamplingFilters("Sampling");
const QString StatisticsFilters("Statistics");
const QString SyntheticBuildingFilters("Synthetic Building");
const QString SurfaceMeshingFilters("Surface Meshing");
const QString Utilities("Utilities");
const QString CustomFilters("Custom");
const QString Unsupported("Unsupported");
} // namespace FilterGroups

namespace FilterSubGroups
{
const QString EnsembleStatsFilters("Ensemble");
const QString MemoryManagementFilters("Memory/Management");
const QString SpatialFilters("Spatial");
const QString StatisticsFilters("Statistics");
const QString FeatureIdentificationFilters("FeatureIdentification");
const QString OutputFilters("Output");
const QString InputFilters("Input");
const QString ImageFilters("Image");
const QString CleanupFilters("Cleanup");
const QString ThresholdFilters("Threshold");
const QString RegularizationFilters("Regularization");
const QString ConversionFilters("Conversion");
const QString FusionFilters("Fusion");
const QString WarpingFilters("Warping");
const QString AlignmentFilters("Alignment");
const QString SegmentationFilters("Segmentation");
const QString GroupingFilters("Grouping");
const QString CropCutFilters("Croping/Cutting");
const QString RotationTransformationFilters("Rotating/Transforming");
const QString ResolutionFilters("Spacing");
const QString MorphologicalFilters("Morphological");
const QString PackingFilters("Packing");
const QString CrystallographyFilters("Crystallography");
const QString GenerationFilters("Generation");
const QString SmoothingFilters("Smoothing");
const QString CurvatureFilters("Curvature");
const QString ConnectivityArrangementFilters("Connectivity/Arrangement");
const QString MappingFilters("Mapping");
const QString MiscFilters("Misc");
const QString GeometryFilters("Geometry");
const QString ImportFilters("Import");
const QString ExportFilters("Export");
} // namespace FilterSubGroups

namespace GeneralData
{
const QString CombinedData("CombinedData");
const QString ThresholdArray("ThresholdArray");
const QString Mask("Mask");
} // namespace GeneralData

namespace CellData
{
const QString AxisAngles("AxisAngles");
const QString BC("BandContrasts");
const QString BandContrast("BandContrast");
const QString CellPhases("Phases");
const QString ConfidenceIndex("Confidence Index");
const QString CAxisLocation("CAxisLocation");
const QString ConfidenceIndexNoSpace("ConfidenceIndex");
const QString Current("Current");
const QString DislocationTensors("DislocationTensors");
const QString EulerAngles("EulerAngles");
const QString EulerColor("EulerColor");
const QString FarFeatureQuats("FarFeatureQuats");
const QString FarFeatureZoneIds("FarFeatureZoneIds");
const QString FitQuality("FitQuality");
const QString FlatImageData("FlatImageData");
const QString GBEuclideanDistances("GBEuclideanDistances");
const QString GBManhattanDistances("GBManhattanDistances");
const QString GlobAlpha("GlobAlpha");
const QString GoodVoxels("GoodVoxels");
const QString FeatureIds("FeatureIds");
const QString FeatureReferenceCAxisMisorientations("FeatureReferenceCAxisMisorientations");
const QString FeatureReferenceMisorientations("FeatureReferenceMisorientations");
const QString IPFColor("IPFColor");
const QString ImageData("ImageData");
const QString ImageQuality("Image Quality");
const QString ImageQualityNoSpace("ImageQuality");
const QString KernelAverageMisorientations("KernelAverageMisorientations");
const QString Mask("Mask");
const QString MotionDirection("MotionDirection");
const QString MicroTexVolFrac("MicroTexVolFrac");
const QString MisorientationColor("MisorientationColor");
const QString ParentDensity("ParentDensity");
const QString MTRgKAM("MTRgKAM");
const QString NearestNeighbors("NearestNeighbors");
const QString ParentIds("ParentIds");
const QString Phases("Phases");
const QString ProjectedImageMin("ProjectedImageMin");
const QString ProjectedImageMax("ProjectedImageMax");
const QString ProjectedImageAvg("ProjectedImageAvg");
const QString ProjectedImageStd("ProjectedImageStd");
const QString ProjectedImageVar("ProjectedImageVar");
const QString QPEuclideanDistances("QPEuclideanDistances");
const QString QPManhattanDistances("QPManhattanDistances");
const QString Quats("Quats");
const QString RodriguesColor("RodriguesColor");
const QString RodriguesVectors("RodriguesVectors");
const QString SolidMeshNodes("SolidMeshNodes");
const QString SineParams("SineParams");
const QString SolidMeshTetrahedrons("SolidMeshTetrahedrons");
const QString Speed("Speed");
const QString SurfaceMeshCells("SurfaceMeshCells");
const QString BoundaryCells("BoundaryCells");
const QString TJManhattanDistances("TJManhattanDistances");
const QString TJEuclideanDistances("TJEuclideanDistances");
const QString VectorColor("VectorColor");
const QString VectorData("VectorData");
const QString Histogram("Histogram");
} // namespace CellData

namespace FeatureData
{
const QString FeatureID("Feature_ID");
const QString Active("Active");
const QString AspectRatios("AspectRatios");
const QString AvgCAxes("AvgCAxes");
const QString AvgCAxisMisalignments("AvgCAxisMisalignments");
const QString LocalCAxisMisalignments("LocalCAxisMisalignments");
const QString UnbiasedLocalCAxisMisalignments("UnbiasedLocalCAxisMisalignments");
const QString AvgQuats("AvgQuats");
const QString AxisEulerAngles("AxisEulerAngles");
const QString AxisLengths("AxisLengths");
const QString BasalLoadingFactor("BasalLoadingFactor");
const QString BiasedFeatures("BiasedFeatures");
const QString CAxisMisalignmentList("CAxisMisalignmentList");
const QString Centroids("Centroids");
const QString ClusteringList("ClusteringList");
const QString ElasticStrains("ElasticStrains");
const QString EquivalentDiameters("EquivalentDiameters");
const QString SaltykovEquivalentDiameters("SaltykovEquivalentDiameters");
const QString EulerAngles("EulerAngles");
const QString AvgEulerAngles("AvgEulerAngles");
const QString F1List("F1List");
const QString F1sptList("F1sptList");
const QString F7List("F7List");
const QString FarFeatureOrientations("FarFeatureOrientations");
const QString FeaturePhases("Phases");
const QString GoodFeatures("GoodFeatures");
const QString FeatureAvgCAxisMisorientations("FeatureAvgCAxisMisorientations");
const QString FeatureAvgMisorientations("FeatureAvgMisorientations");
const QString FeatureStdevCAxisMisorientations("FeatureStdevCAxisMisorientations");
const QString KernelAvgMisorientations("KernelAvgMisorientations");
const QString LMG("LMG");
const QString LargestCrossSections("LargestCrossSections");
const QString Mask("Mask");
const QString MTRdensity("MTRdensity");
const QString MTRgKAM("MTRgKAM");

const QString MisorientationList("MisorientationList");
const QString NeighborList("NeighborList");
const QString NeighborhoodList("NeighborhoodList");
const QString Neighborhoods("Neighborhoods");
const QString NumCells("NumCells");
const QString NumElements("NumElements");
const QString NumFeaturesPerParent("NumFeaturesPerParent");
const QString NumNeighbors("NumNeighbors");
const QString Omega3s("Omega3s");
const QString ParentIds("ParentIds");
const QString Phases("Phases");
const QString Poles("Poles");
const QString RGBs("RGBs");
const QString Schmids("Schmids");
const QString SharedSurfaceAreaList("SharedSurfaceAreaList");
const QString SlipSystems("SlipSystems");
const QString SurfaceAreaVol("SurfaceAreaVolumeRatio");
const QString SurfaceFeatures("SurfaceFeatures");
const QString SurfaceElementFractions("SurfaceElementFractions");
const QString Volumes("Volumes");
const QString AvgMisorientations("AvgMisorientations");
const QString mPrimeList("mPrimeList");
const QString NumBins("NumBins");
const QString ScalarAverages("ScalarAverages");

} // namespace FeatureData

namespace EnsembleData
{
const QString NumFeatures("NumFeatures");
const QString VolFractions("VolFractions");
const QString TotalSurfaceAreas("TotalSurfaceAreas");
const QString CrystalSymmetry("Crystal Symmetry");
const QString CrystalStructures("CrystalStructures");
const QString PhaseTypes("PhaseTypes");
const QString BravaisLattice("BravaisLattice");
const QString PrecipitateFractions("PrecipitateFractions");
const QString ShapeTypes("ShapeTypes");
const QString Statistics("Statistics");
const QString PhaseName("PhaseName");
const QString LatticeConstants("LatticeConstants");
const QString GBCD("GBCD");
const QString GBCDdimensions("GBCDdimensions");
const QString FitParameters("FitParameters");
const QString MaterialName("MaterialName");
} // namespace EnsembleData

namespace VertexData
{
const QString AtomVelocities("AtomVelocities");
const QString AtomTypes("AtomTypes");
const QString AtomFeatureLabels("AtomFeatureLabels");
const QString NumberOfArms("NumberOfArms");
const QString NodeConstraints("NodeConstraints");
const QString SurfaceMeshNodes("Nodes");
const QString SurfaceMeshNodeType("NodeType");
const QString SurfaceMeshNodeNormals("NodeNormals");
const QString SurfaceMeshNodeFaces("NodeFaces");
} // namespace VertexData

namespace FaceData
{
const QString SurfaceMeshFaces("Faces");
const QString SurfaceMeshFaceIPFColors("IPFColors");
const QString SurfaceMeshFaceMisorientationColors("MisorientationColors");
const QString SurfaceMeshFaceSchuhMisorientationColors("SchuhMisorientationColors");
const QString SurfaceMeshFaceLabels("FaceLabels");
const QString SurfaceMeshFacePhases("Phases");
const QString SurfaceMeshF1s("F1s");
const QString SurfaceMeshF1spts("F1spts");
const QString SurfaceMeshF7s("F7s");
const QString SurfaceMeshmPrimes("mPrimes");
const QString SurfaceMeshVoxels("SurfaceMeshVoxels");
const QString SurfaceMeshFaceCentroids("FaceCentroids");
const QString SurfaceMeshFaceAreas("FaceAreas");
const QString SurfaceMeshTwinBoundary("TwinBoundary");
const QString SurfaceMeshTwinBoundaryIncoherence("TwinBoundaryIncoherence");
const QString SurfaceMeshTwinBoundarySchmidFactors("TwinBoundarySchmidFactors");
const QString SurfaceMeshFaceDihedralAngles("FaceDihedralAngles");
const QString SurfaceMeshFaceNormals("FaceNormals");
const QString SurfaceMeshFeatureFaceId("FeatureFaceId");
const QString SurfaceMeshGaussianCurvatures("GaussianCurvatures");
const QString SurfaceMeshMeanCurvatures("MeanCurvatures");
const QString SurfaceMeshPrincipalCurvature1("PrincipalCurvature1");
const QString SurfaceMeshPrincipalCurvature2("PrincipalCurvature2");
const QString SurfaceMeshPrincipalDirection1("PrincipalDirection1");
const QString SurfaceMeshPrincipalDirection2("PrincipalDirection2");
} // namespace FaceData

namespace EdgeData
{
const QString DislocationIds("DislocationIds");
const QString BurgersVectors("BurgersVectors");
const QString SlipPlaneNormals("SlipPlaneNormals");
const QString SurfaceMeshEdges("SurfaceMeshEdges");
const QString SurfaceMeshUniqueEdges("SurfaceMeshUniqueEdges");
const QString SurfaceMeshInternalEdges("SurfaceMeshInternalEdges");
const QString SurfaceMeshTriangleEdges("SurfaceMeshTriangleEdges");
const QString SurfaceMeshEdgeFaces("SurfaceMeshEdgeFaces");
} // namespace EdgeData

namespace Settings
{
const QString Library("Filter Library");
const QString PrebuiltPipelines("Prebuilt Pipelines");
const QString FavoritePipelines("Favorite Pipelines");
const QString PipelineBuilderGroup("PipelineBuilder");
const QString FavoriteConfig("favorite_config");
const QString NumFilters("Number_Filters");
const QString PipelineName("Name");
const QString FilterName("Filter_Name");
const QString FilterUuid("Filter_Uuid");
const QString FilterVersion("FilterVersion");
const QString HumanLabel("Filter_Human_Label");
const QString FilterEnabled("Filter_Enabled");
const QString GroupName("GroupName");
const QString SubGroupName("SubGroupName");
const QString BrandingString("BrandingString");
const QString CompiledLibraryName("CompiledLibraryName");
const QString Version("Version");
const QString PipelineBuilderGeomertry("PipelineBuilderGeometry");
} // namespace Settings

namespace HDF5
{
const QString FileVersion("7.0");
const QString FileVersionName("FileVersion");
const QString DREAM3DVersion("DREAM3D Version");
const QString ObjectType("ObjectType");
const QString Rank("Rank");
const QString TupleDimensions("TupleDimensions");
const QString ComponentDimensions("ComponentDimensions");
const QString AxisDimensions("Tuple Axis Dimensions");
const QString DataArrayVersion("DataArrayVersion");
} // namespace HDF5

namespace StringConstants
{
const QString Alpha("Alpha");
const QString Angle("Angle");

const QString Average("Average");
const QString Axis("Axis");
const QString AxisODFWeights("AxisODF-Weights");
const QString AxisOrientation("AxisOrientation");
const QString Beta("Beta");
const QString BetaDistribution("Beta Distribution");
const QString BinNumber("BinNumber");
const QString BinCount("Bin Count");
const QString BinStepSize("Bin Step Size");
const QString BoundaryArea("BoundaryArea");
const QString BoundaryStatsData("BoundaryStatsData");
const QString CellNeighbors("CellNeighbors");

const QString CellsContainingVert("CellsContainingVert");
const QString CellsName("Cells");
const QString CrystalStructure("CrystalStructure");
const QString DataContainerGroupName("DataContainers");
const QString DataContainerBundleGroupName("DataContainerBundles");
const QString DataContainerNames("DataContainerNames");
const QString MetaDataArrays("MetaDataArrays");
const QString MontageGroupName("Montages");
const QString DataContainerType("DataContainerType");
const QString AttributeMatrixType("AttributeMatrixType");

const QString Dims("Dims");
const QString DistributionType("Distribution Type");

const QString EdgeNeighbors("EdgeNeighbors");
const QString EdgesContainingVert("EdgesContainingVert");
const QString EdgesName("Edges");
const QString EdgeCentroids("EdgeCentroids");
const QString EdgeLengths("EdgeLengths");
const QString Euler1("Euler 1");
const QString Euler2("Euler 2");
const QString Euler3("Euler 3");
const QString Exp_k("K");

const QString FaceNeighbors("FaceNeighbors");
const QString FacesContainingVert("FacesContainingVert");
const QString FacesName("Faces");
const QString TrianglesName("Triangles");
const QString TriangleNeighbors("TriangleNeighbors");
const QString TrianglesContainingVert("TrianglesContainingVert");
const QString TriangleCentroids("TriangleCentroids");
const QString TriangleAreas("TriangleAreas");
const QString Frequencies("Frequencies");

const QString QuadsName("Quadrilaterals");
const QString QuadNeighbors("QuadrilateralNeighbors");
const QString QuadsContainingVert("QuadrilateralsContainingVerts");
const QString QuadCentroids("QuadrilateralCentroids");
const QString QuadAreas("QuadrilateralAreas");

const QString TetsName("Tetrahedra");
const QString TetNeighbors("TetrahedralNeighbors");
const QString TetsContainingVert("TetrahedraContainingVerts");
const QString TetCentroids("TetrahedralCentroids");
const QString TetVolumes("TetrahedralVolumes");

const QString HexasName("Hexahedra");
const QString HexNeighbors("HexahedralNeighbors");
const QString HexasContainingVert("HexahedraContainingVerts");
const QString HexCentroids("HexahedralCentroids");
const QString HexVolumes("HexahedralVolumes");

const QString VoxelSizes("VoxelSizes");
const QString VertexSizes("VertexSizes");

const QString GBCD("GBCD");
const QString FeatureAvgDisorientation("FeatureAvgDisorientation");
const QString Feature_Diameter_Info("Feature_Diameter_Info");
const QString Feature_SizeVBoverA_Distributions("FeatureSize Vs B Over A Distributions");
const QString Feature_SizeVCoverA_Distributions("FeatureSize Vs C Over A Distributions");
const QString Feature_SizeVNeighbors_Distributions("FeatureSize Vs Neighbors Distributions");
const QString Feature_SizeVOmega3_Distributions("FeatureSize Vs Omega3 Distributions");
const QString Feature_SizeVClustering_Distributions("FeatureSize Vs Clustering Distributions");
const QString Feature_Size_Distribution("FeatureSize Distribution");
const QString KernelAvgDisorientation("KernelAvgDisorientation");
const QString LogNormalDistribution("Log Normal Distribution");
const QString MDFWeights("MDF-Weights");
const QString MatrixStatsData("MatrixStatsData");
const QString MeshFaceNeighborLists("MeshFaceNeighborLists");
const QString MicroTextureBins("MicroTextureBins");
const QString MinimumValue("Minimum Value");
const QString MinMaxNoOfBins("MinMaxNoOfBins");
const QString MisorientationBins("MisorientationBins");
const QString Neighbor_Feature_ID_List("Neighbor_Feature_ID_List");

const QString ODF("ODF");
const QString ODFWeights("ODF-Weights");

const QString ParentPhase("Parent Phase");
const QString PhaseFraction("PhaseFraction");
const QString PhaseType("PhaseType");
const QString PipelineGroupName("Pipeline");
const QString PipelineVersionName("Pipeline Version");
const QString PipelineCurrentName("Current Pipeline");
const QString PowerLawDistribution("Power Law Distribution");
const QString PrecipitateBoundaryFraction("Precipitate Boundary Fraction");
const QString PrecipitateStatsData("PrecipitateStatsData");
const QString PrimaryStatsData("PrimaryStatsData");
const QString PresetName("PresetName");

const QString RadialDistFunc("Radial Distribution Function");
const QString RdfMinDistance("Min");
const QString RdfMaxDistance("Max");
const QString RdfBoxDims("BoxDims");
const QString RdfBoxRes("BoxRes");

const QString Sigma("Sigma");
const QString StandardDeviation("Standard Deviation");
const QString Statistics("Statistics");
const QString Stats("Stats");
const QString StatsData("StatsData");
const QString StatsType("StatsType");

const QString TransformationStatsData("TransformationStatsData");
const QString UnknownDistribution("Unknown Distribution");

const QString VerticesName("Vertices");
const QString VertsName("Verts");
const QString ImageName("Image");

const QString Weight("Weight");

const QString MetaData("_META_DATA");

const QString Name("Name");
const QString Index("Index");
} // namespace StringConstants

namespace CellType
{
const QString Quadrilateral("Quadrilateral");
const QString Tetrahedron("Tetrahedron");
const QString Triangle("Triangle");
const QString Face("Face");
const QString Edge("Edge");
const QString Vertex("Vertex");
const QString Node("Node");
} // namespace CellType

// namespace PhaseType
//{
//  //constexpr uint32_t PrimaryPhase = 0;              //!<
//  //constexpr uint32_t PrecipitatePhase = 1;          //!<
//  //constexpr uint32_t TransformationPhase = 2;       //!<
//  //constexpr uint32_t MatrixPhase = 3;              //!<
//  //constexpr uint32_t BoundaryPhase = 4;              //!<
//  //constexpr uint32_t Unknown = 999;    //!<

//  const QString Primary("Primary");
//  const QString Precipitate("Precipitate");
//  const QString Transformation("Transformation");
//  const QString Matrix("Matrix");
//  const QString Boundary("Boundary");
//  const QString UnknownPhase("Unknown");
//}

namespace AlignmentMethod
{
constexpr uint32_t OuterBoundary = 0;            //!<
constexpr uint32_t Misorientation = 1;           //!<
constexpr uint32_t MutualInformation = 2;        //!<
constexpr uint32_t Count = 3;                    //!<
constexpr uint32_t UnknownAlignmentMethod = 999; //!<
} // namespace AlignmentMethod

namespace StatisticsType
{
constexpr uint32_t Feature_SizeVBoverA = 0;      //!<
constexpr uint32_t Feature_SizeVCoverA = 1;      //!<
constexpr uint32_t Feature_SizeVNeighbors = 2;   //!<
constexpr uint32_t Feature_SizeVOmega3 = 3;      //!<
constexpr uint32_t Feature_SizeVClustering = 4;  //!<
constexpr uint32_t UnknownStatisticsGroup = 999; //!<

} // namespace StatisticsType

namespace DistributionType
{
constexpr uint32_t Beta = 0;                    //!<
constexpr uint32_t LogNormal = 1;               //!<
constexpr uint32_t Power = 2;                   //!<
constexpr uint32_t RDFFrequency = 3;            //!<
constexpr uint32_t RDFMaxMin = 4;               //!<
constexpr uint32_t UnknownDistributionType = 5; //!<
constexpr uint32_t Count = 6;                   //!<

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
constexpr uint32_t DegreesToRadians = 0; //!<
constexpr uint32_t RadiansToDegrees = 1; //!<
} // namespace EulerAngleConversionType

namespace FlattenImageMethod
{
constexpr uint32_t Lightness = 0;  //!<
constexpr uint32_t Average = 1;    //!<
constexpr uint32_t Luminosity = 2; //!<
} // namespace FlattenImageMethod

namespace EulerFrameRotationAxis
{
constexpr uint32_t RD = 0;   //!<
constexpr uint32_t TD = 1;   //!<
constexpr uint32_t ND = 2;   //!<
constexpr uint32_t None = 3; //!<
} // namespace EulerFrameRotationAxis

namespace SampleFrameRotationAxis
{
constexpr uint32_t X = 0;    //!<
constexpr uint32_t Y = 1;    //!<
constexpr uint32_t Z = 2;    //!<
constexpr uint32_t None = 3; //!<
} // namespace SampleFrameRotationAxis

namespace RefFrameRotationAngle
{
constexpr uint32_t Ninety = 0;     //!<
constexpr uint32_t oneEighty = 1;  //!<
constexpr uint32_t twoSeventy = 2; //!<
constexpr uint32_t Mirror = 3;     //!<
constexpr uint32_t Zero = 4;       //!<
} // namespace RefFrameRotationAngle

namespace Overlap
{
constexpr uint32_t Rigid = 0;            //!<
constexpr uint32_t Progressive = 1;      //!<
constexpr uint32_t UnknownOverlap = 999; //!<
} // namespace Overlap

//  namespace DataContainerType
//  {
//    constexpr uint32_t VolumeDataContainer = 0; //!<
//    constexpr uint32_t SurfaceDataContainer = 1; //!<
//    constexpr uint32_t EdgeDataContainer = 2; //!<
//    constexpr uint32_t VertexDataContainer = 3; //!<
//    constexpr uint32_t UnknownDataContainer = 999; //!<
//  }

//  namespace AttributeMatrixType
//  {
//    constexpr uint32_t Vertex = 0; //!<
//    constexpr uint32_t Edge = 1; //!<
//    constexpr uint32_t Face = 2; //!<
//    constexpr uint32_t Cell = 3; //!<
//    constexpr uint32_t VertexFeature = 4; //!<
//    constexpr uint32_t EdgeFeature = 5; //!<
//    constexpr uint32_t FaceFeature = 6; //!<
//    constexpr uint32_t CellFeature = 7; //!<
//    constexpr uint32_t VertexEnsemble = 8; //!<
//    constexpr uint32_t EdgeEnsemble = 9; //!<
//    constexpr uint32_t FaceEnsemble = 10; //!<
//    constexpr uint32_t CellEnsemble = 11; //!<
//    constexpr uint32_t MetaData = 12; //!<
//    constexpr uint32_t Generic = 13; //!<
//    constexpr uint32_t Unknown = 999; //!<
//  }

//  namespace AttributeMatrixObjectType
//  {
//    constexpr uint32_t Element = 0;
//    constexpr uint32_t Feature = 1;
//    constexpr uint32_t Ensemble = 2;
//    constexpr uint32_t Any = 3;
//  }

namespace Geometry
{
const QString Geometry("_SIMPL_GEOMETRY");
const QString GeometryName("GeometryName");
const QString GeometryType("GeometryType");
const QString GeometryTypeName("GeometryTypeName");
const QString NumberOfTuples("NumberOfTuples");
const QString UnitDimensionality("UnitDimensionality");
const QString SpatialDimensionality("SpatialDimensionality");

const QString AnyGeometry("AnyGeometry");
const QString UnknownGeometry("UnkownGeometry");
const QString ImageGeometry("ImageGeometry");
const QString RectGridGeometry("RectGridGeometry");
const QString VertexGeometry("VertexGeometry");
const QString EdgeGeometry("EdgeGeometry");
const QString TriangleGeometry("TriangleGeometry");
const QString QuadGeometry("QuadrilateralGeometry");
const QString TetrahedralGeometry("TetrahedralGeometry");
const QString HexahedralGeometry("HexahedralGeometry");

const QString xBoundsList("xBounds");
const QString yBoundsList("yBounds");
const QString zBoundsList("zBounds");
const QString SharedVertexList("SharedVertexList");
const QString SharedEdgeList("SharedEdgeList");
const QString SharedTriList("SharedTriList");
const QString SharedQuadList("SharedQuadList");
const QString SharedTetList("SharedTetList");
const QString SharedHexList("SharedHexList");
const QString UnsharedEdgeList("UnsharedEdgeList");
const QString UnsharedFaceList("UnsharedFaceList");

const QString TransformContainerGroup("TransformContainerGroup");
const QString TransformContainer("TransformContainer");
const QString CompositeTransformContainer("CompositeTransformContainer");
const QString TransformContainerTypeName("TransformContainerTypeName");
const QString UnknownTransformContainer("UnknownTransformContainer");
const QString TransformContainerParameters("TransformParameters");
const QString TransformContainerFixedParameters("TransformFixedParameters");
const QString TransformContainerTypeAsString("TransformType");
const QString TransformContainerMovingName("TransformMovingName");
const QString TransformContainerReferenceName("TransformReferenceName");

const QString k_Yoctometer("Yoctometer");
const QString k_Zeptometer("Zeptometer");
const QString k_Attometer("Attometer");
const QString k_Femtometer("Femtometer");
const QString k_Picometer("Picometer");
const QString k_Nanometer("Nanometer");
const QString k_Micrometer("Micrometer");
const QString k_Millimeter("Millimeter");
const QString k_Centimeter("Centimeter");
const QString k_Decimeter("Decimeter");
const QString k_Meter("Meter");
const QString k_Decameter("Decameter");
const QString k_Hectometer("Hectometer");
const QString k_Kilometer("Kilometer");
const QString k_Megameter("Megameter");
const QString k_Gigameter("Gigameter");
const QString k_Terameter("Terameter");
const QString k_Petameter("Petameter");
const QString k_Exameter("Exameter");
const QString k_Zettameter("Zettameter");
const QString k_Yottameter("Yottameter");
const QString k_Angstrom("Angstrom");
const QString k_Mil("Mil");
const QString k_Inch("Inch");
const QString k_Foot("Foot");
const QString k_Mile("Mile");
const QString k_Fathom("Fathom");
const QString k_Unspecified("Unspecified");
const QString k_Unknown("Unknown");
} // namespace Geometry

//  namespace GeometryType
//  {
//    constexpr uint32_t ImageGeometry = 11;
//    constexpr uint32_t RectGridGeometry = 11;
//    constexpr uint32_t VertexGeometry = 1;
//    constexpr uint32_t EdgeGeometry = 3;
//    constexpr uint32_t TriangleGeometry = 5;
//    constexpr uint32_t QuadGeometry = 9;
//    constexpr uint32_t TetrahedralGeometry = 10;
//    constexpr uint32_t UnknownGeometry = 999;
//  }

namespace XdmfGridType
{
constexpr uint32_t PolyData = 0;
constexpr uint32_t RectilinearGrid = 1;
constexpr uint32_t UnknownGrid = 0xFFFFFFFF;
} // namespace XdmfGridType

namespace XdmfCenterType
{
const QString Node("Node");
const QString Edge("Edge");
const QString Face("Face");
const QString Cell("Cell");
const QString Grid("Grid");
} // namespace XdmfCenterType

// namespace ShapeType
//{
//  constexpr uint32_t EllipsoidShape = 0; //!<
//  constexpr uint32_t SuperEllipsoidShape = 1; //!<
//  constexpr uint32_t CubeOctahedronShape = 2; //!<
//  constexpr uint32_t CylinderAShape = 3; //!<
//  constexpr uint32_t CylinderBShape = 4; //!<
//  constexpr uint32_t CylinderCShape = 5; //!<
//  constexpr uint32_t ShapeTypeEnd = 6;
//  constexpr uint32_t UnknownShapeType = 999; //!<
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
const QString H5VoxelFile("VoxelData.h5voxel");

const QString VisualizationVizFile("Visualization.vtk");           // 11
const QString DownSampledVizFile("DownSampled_Visualization.vtk"); // 11
const QString HDF5FeatureFile("Features.h5feature");
} // namespace Reconstruction

namespace SyntheticBuilder
{
const QString FeatureDataFile("FeatureData.csv");
const QString H5VoxelFile("VoxelData.h5voxel");

const QString VisualizationVizFile("Visualization.vtk");
const QString HDF5FeatureFile("Features.h5feature");

const QString ErrorFile("Error.txt");
const QString VtkFile("Test.vtk");
} // namespace SyntheticBuilder

/*   Surface Meshing Related   */
namespace SurfaceMesh
{
namespace NodeType
{
constexpr int8_t Unused = 0;
constexpr int8_t Default = 2;
constexpr int8_t TriplePoint = 3;
constexpr int8_t QuadPoint = 4;
constexpr int8_t SurfaceDefault = 12;
constexpr int8_t SurfaceTriplePoint = 13;
constexpr int8_t SurfaceQuadPoint = 14;
} // namespace NodeType
namespace NodeId
{
constexpr int8_t Unused = -1;
}
} // namespace SurfaceMesh

namespace SolidMeshing
{
/* Solid Meshing Related */
const QString MeshFile("solid_mesh_v5_1.vtk");
const QString MeshFile2("solid_mesh_v5_2.vtk");
const QString ElementQualityFile("element_quality_measures_v5.txt");
const QString VoxelsFile("voxels_v5.txt");
} // namespace SolidMeshing

namespace Union
{
namespace Strings
{
const QString And("And");
const QString Or("Or");
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
const QString LessThan("<");
const QString GreaterThan(">");
const QString Equal("=");
const QString NotEqual("!=");
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
constexpr int32_t Horizontal = 0;
constexpr int32_t Vertical = 1;
constexpr int32_t Square = 2;
}; // namespace Layout
} // namespace SIMPL

namespace Core
{
const QString CoreBaseName("Core");
}

namespace Test
{
const QString TestPluginFile("TestPlugin");
const QString TestPluginDisplayName("Test Plugin");
const QString TestBaseName("Test");
} // namespace Test

Q_DECLARE_METATYPE(SIMPL::NumericTypes::Type)
Q_DECLARE_METATYPE(SIMPL::ScalarTypes::Type)
Q_DECLARE_METATYPE(SIMPL::DelimiterTypes::Type)

#if 0
namespace Generic
{
  const QString GenericPluginFile("GenericPlugin");
  const QString GenericPluginDisplayName("Generic Plugin");
  const QString GenericBaseName("Generic");
}

namespace IO
{
  const QString IOPluginFile("IOPlugin");
  const QString IOPluginDisplayName("IO Plugin");
  const QString IOBaseName("IO");
}
#endif
