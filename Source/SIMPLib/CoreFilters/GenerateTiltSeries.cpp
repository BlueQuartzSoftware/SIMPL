/*
 * Your License or Copyright can go here
 */

#include "GenerateTiltSeries.h"

#include <thread>

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/ITK/Dream3DTemplateAliasMacro.h"
#include "SIMPLib/ITK/SimpleITKEnums.h"
#include "SIMPLib/ITK/itkInPlaceDream3DDataToImageFilter.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "SIMPLib/SIMPLibVersion.h"

#include <cmath>

#ifndef DREAM3D_PASSIVE_ROTATION
#define DREAM3D_PASSIVE_ROTATION 1
#endif

#ifndef ROTATIONS_CONSTANTS
#define ROTATIONS_CONSTANTS
namespace Rotations
{
namespace Constants
{
#if DREAM3D_PASSIVE_ROTATION
static const float epsijk = 1.0f;
// static const double epsijkd = 1.0;
#elif DREAM3D_ACTIVE_ROTATION
static const float epsijk = -1.0f;
static const double epsijkd = -1.0;
#endif
} // namespace Constants
} // namespace Rotations

#endif

#define GTS_GENERATE_DEBUG_ARRAYS 1

namespace Detail
{

const QString k_AttributeMatrixName("Slice Data");
const QString k_ImageGridNamePattern("Rotation_%1");

class ResampleGrid
{
public:
  using AxisAngleType = std::array<float, 4>;
  using CoordinateType = FloatVec3Type;
  using OrientationMatrixType = std::vector<float>;

  ResampleGrid(GenerateTiltSeries* filter, const FloatArrayType::Pointer& inputGrid, const DataContainer::Pointer& outDc, const AxisAngleType& rotationAxis, size_t gridIndex)
  : m_Filter(filter)
  , m_Coords(inputGrid)
  , m_OutputDC(outDc)
  , m_RotationAxis(rotationAxis)
  , m_GridIndex(gridIndex)
  {
  }
  ~ResampleGrid() = default;

  ResampleGrid(const ResampleGrid&) = default;
  ResampleGrid(ResampleGrid&&) noexcept = default;
  ResampleGrid& operator=(const ResampleGrid&) = delete; // Copy Assignment Not Implemented
  ResampleGrid& operator=(ResampleGrid&&) = delete;      // Move Assignment Not Implemented

  void operator()() const
  {

    //========= Input Data Structure Elements
    DataContainerArray::Pointer dca = m_Filter->getDataContainerArray();
    IDataArray::Pointer inputData = dca->getPrereqIDataArrayFromPath<IDataArray>(m_Filter, m_Filter->getInputDataArrayPath());
    DataArrayPath inputDataArrayPath = m_Filter->getInputDataArrayPath();
    DataContainer::Pointer inputDC = dca->getDataContainer(inputDataArrayPath.getDataContainerName());
    ImageGeom::Pointer inputImageGeom = inputDC->getGeometryAs<ImageGeom>();
    FloatVec6Type inBounds = inputImageGeom->getBoundingBox();
    FloatVec3Type inCenter = {(inBounds[1] - inBounds[0]) / 2.0f + inBounds[0], (inBounds[3] - inBounds[2]) / 2.0f + inBounds[2], (inBounds[5] - inBounds[4]) / 2.0f + inBounds[4]};

    //  printf("inCenter %f %f %f\n", inCenter[0], inCenter[1], inCenter[2]);

    //========= Output Data Structure Elements
    ImageGeom::Pointer outputImageGeom = m_OutputDC->getGeometryAs<ImageGeom>();
    FloatVec6Type bounds = outputImageGeom->getBoundingBox();
    FloatVec3Type center = {(bounds[1] - bounds[0]) / 2.0f + bounds[0], (bounds[3] - bounds[2]) / 2.0f + bounds[2], (bounds[5] - bounds[4]) / 2.0f + bounds[4]};

    //  printf("outCenter %f %f %f\n", center[0], center[1], center[2]);

    AttributeMatrix::Pointer attrMat = m_OutputDC->getAttributeMatrix(Detail::k_AttributeMatrixName);
    IDataArray::Pointer outputData = attrMat->getAttributeArray(m_Filter->getInputDataArrayPath().getDataArrayName());
    outputData->initializeWithZeros();
#if GTS_GENERATE_DEBUG_ARRAYS
    SizeTArrayType::Pointer inputVoxIndexXYZ = SizeTArrayType::CreateArray(outputData->getNumberOfTuples(), {3}, "Input Voxel XYZ Index", true);
    inputVoxIndexXYZ->initializeWithZeros();
    attrMat->insertOrAssign(inputVoxIndexXYZ);

    SizeTArrayType::Pointer inputVoxIndex = SizeTArrayType::CreateArray(outputData->getNumberOfTuples(), {1}, "Input Voxel Index", true);
    inputVoxIndex->initializeWithZeros();
    attrMat->insertOrAssign(inputVoxIndex);

    FloatArrayType::Pointer inputVoxCoord = FloatArrayType::CreateArray(outputData->getNumberOfTuples(), {3}, "Input Voxel Coord", true);
    inputVoxCoord->initializeWithZeros();
    attrMat->insertOrAssign(inputVoxCoord);
#endif
    // Generate the Rotation Matrix
    OrientationMatrixType om = ax2om<AxisAngleType, OrientationMatrixType>(m_RotationAxis);
#if GTS_GENERATE_DEBUG_ARRAYS
    FloatArrayType::Pointer copyOfGrid = std::dynamic_pointer_cast<FloatArrayType>(m_Coords->deepCopy());
#endif
    ImageGeom::ErrorType err;
    size_t numTuples = m_Coords->getNumberOfTuples();
    //  int32_t numComp = m_Coords->getNumberOfComponents();
    for(size_t tupleIndex = 0; tupleIndex < numTuples; tupleIndex++)
    {
      size_t outputVoxelIndex = 0;
      size_t inputVoxelIndex = 0;

      // Get the original point as a copy
      FloatVec3Type inCoord(m_Coords->getComponent(tupleIndex, 0), m_Coords->getComponent(tupleIndex, 1), m_Coords->getComponent(tupleIndex, 2));
#if GTS_GENERATE_DEBUG_ARRAYS
      inputVoxCoord->setTuple(tupleIndex, inCoord.data());
#endif
      // Find the voxel that data will be copied into
      err = outputImageGeom->computeCellIndex(inCoord.data(), outputVoxelIndex);
      if(err != ImageGeom::ErrorType::NoError)
      {
        continue;
      }
      //   std::cout << tupleIndex << "\tCoord:" << inCoord[0] << ", " << inCoord[1] << ", " << inCoord[2];

      // Transform the Point via translation to move it to a relative position to (0,0,0)
      inCoord[0] = inCoord[0] - center[0];
      inCoord[1] = inCoord[1] - center[1];
      inCoord[2] = inCoord[2] - center[2];

      // Transform the point using the Orientation Matrix
      CoordinateType outCoord = transformCoordinate<OrientationMatrixType, FloatVec3Type>(om, inCoord);

      // Translate back to the actual grid
      outCoord[0] = outCoord[0] + center[0];
      outCoord[1] = outCoord[1] + center[1];
      outCoord[2] = outCoord[2] + center[2];

#if GTS_GENERATE_DEBUG_ARRAYS
      copyOfGrid->setTuple(tupleIndex, outCoord.data());
#endif

      //    std::cout << "\t InputVoxelIndex: " << inputVoxelIndices[0] << ", " << inputVoxelIndices[1] << ", " << inputVoxelIndices[2] << std::endl;
#if GTS_GENERATE_DEBUG_ARRAYS
      SizeVec3Type inputVoxelIndices;
      err = inputImageGeom->computeCellIndex(outCoord.data(), inputVoxelIndices.data());
      inputVoxIndexXYZ->setTuple(tupleIndex, inputVoxelIndices.data());
#endif
      err = inputImageGeom->computeCellIndex(outCoord.data(), inputVoxelIndex);

#if GTS_GENERATE_DEBUG_ARRAYS
      inputVoxIndex->setTuple(tupleIndex, &inputVoxelIndex);
#endif
      if(err != ImageGeom::ErrorType::NoError)
      {
        continue;
      }
      else
      {
        outputData->copyFromArray(outputVoxelIndex, inputData, inputVoxelIndex, 1);
      }
    }

#if GTS_GENERATE_DEBUG_ARRAYS
    // Write out the sampling grid
    DataContainer::Pointer gridGeomDC = DataContainer::New(QString("Vertex Grid %1").arg(m_GridIndex));
    VertexGeom::Pointer gridVertGeom = VertexGeom::CreateGeometry(copyOfGrid->getNumberOfTuples(), "Grid Geometry", true);
    gridVertGeom->setVertices(copyOfGrid);
    gridGeomDC->setGeometry(gridVertGeom);
    dca->insertOrAssign(gridGeomDC);
#endif
  }

  template <typename T, typename K>
  K transformCoordinate(const T& orientationMatrix, const K& coord) const
  {
    K outCoord = {0, 0, 0};

    outCoord[0] = orientationMatrix[0] * coord[0] + orientationMatrix[1] * coord[1] + orientationMatrix[2] * coord[2];
    outCoord[1] = orientationMatrix[3] * coord[0] + orientationMatrix[4] * coord[1] + orientationMatrix[5] * coord[2];
    outCoord[2] = orientationMatrix[6] * coord[0] + orientationMatrix[7] * coord[1] + orientationMatrix[8] * coord[2];

    return outCoord;
  }

  template <typename InputType, typename OutputType>
  OutputType ax2om(const InputType& a) const
  {
    OutputType res(9);
    typename OutputType::value_type q = 0.0L;
    typename OutputType::value_type c = 0.0L;
    typename OutputType::value_type s = 0.0L;
    typename OutputType::value_type omc = 0.0L;

    c = cos(a[3]);
    s = sin(a[3]);

    omc = 1.0f - c;

    res[0] = a[0] * a[0] * omc + c;
    res[4] = a[1] * a[1] * omc + c;
    res[8] = a[2] * a[2] * omc + c;
    size_t _01 = 1;
    size_t _10 = 3;
    size_t _12 = 5;
    size_t _21 = 7;
    size_t _02 = 2;
    size_t _20 = 6;
    // Check to see if we need to transpose
    if(Rotations::Constants::epsijk == 1.0f)
    {
      _01 = 3;
      _10 = 1;
      _12 = 7;
      _21 = 5;
      _02 = 6;
      _20 = 2;
    }

    q = omc * a[0] * a[1];
    res[_01] = q + s * a[2];
    res[_10] = q - s * a[2];
    q = omc * a[1] * a[2];
    res[_12] = q + s * a[0];
    res[_21] = q - s * a[0];
    q = omc * a[2] * a[0];
    res[_02] = q - s * a[1];
    res[_20] = q + s * a[1];

    return res;
  }

private:
  GenerateTiltSeries* m_Filter = nullptr;
  FloatArrayType::Pointer m_Coords;
  DataContainer::Pointer m_OutputDC;
  std::array<float, 4> m_RotationAxis;
  size_t m_GridIndex = 0;
};

} // namespace Detail

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateTiltSeries::GenerateTiltSeries()
: m_RotationAxis(0)
, m_Increment(1.0)
, m_Spacing(FloatVec3Type(1.0, 1.0, 1.0))
, m_InputDataArrayPath(DataArrayPath("DataContainer", "AttributeMatrix", "FeatureIds"))
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateTiltSeries::~GenerateTiltSeries() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTiltSeries::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTiltSeries::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Rotation Axis");
    parameter->setPropertyName("RotationAxis");
    
    QVector<QString> choices;          // Please add choices to the choices QVector to finish this widget
    choices << "X Axis"
            << "Y Axis"
            << "Z Axis";
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameter->setSetterCallback(SIMPL_BIND_SETTER(GenerateTiltSeries, this, RotationAxis));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(GenerateTiltSeries, this, RotationAxis));
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Rotation Increment (Deg)", Increment, FilterParameter::Parameter, GenerateTiltSeries));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Resample Spacing", Spacing, FilterParameter::Parameter, GenerateTiltSeries));
  //  DataArrayCreationFilterParameter::RequirementType dacReq;
  //  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Created Array Path", CreatedArrayPath, FilterParameter::Parameter, GenerateTiltSeries, dacReq));
  DataArraySelectionFilterParameter::RequirementType dasReq;
  dasReq.dcGeometryTypes = {IGeometry::Type::Image};
  dasReq.amTypes = {AttributeMatrix::Type::Cell};
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Input Data Array Path", InputDataArrayPath, FilterParameter::Parameter, GenerateTiltSeries, dasReq));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTiltSeries::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  // Validate ImageGeometry and Cell Type for attribute matrix

  // Validate correct range of Rotation Axis (0|1|2)
  DataContainerArray::Pointer dca = getDataContainerArray();
  IDataArray::Pointer inputData = dca->getPrereqIDataArrayFromPath<IDataArray>(this, getInputDataArrayPath());
  if(getErrorCode() < 0)
  {
    return;
  }

  // Generate Data Structure
  std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> gridPair = generateGrid();
  if(nullptr == gridPair.first.get())
  {
    return;
  }
  FloatArrayType::Pointer gridCoords = gridPair.first;
  ImageGeom::Pointer gridGeometry = gridPair.second;
  SizeVec3Type gridDims = gridGeometry->getDimensions();

  float increment = getIncrement();
  size_t gridIndex = 0;

  for(float currentDeg = 0.0f; currentDeg < 180.0f; currentDeg += increment)
  {
    QString gridDCName = QString(Detail::k_ImageGridNamePattern).arg(gridIndex);
    DataContainer::Pointer gridDC = DataContainer::New(gridDCName);
    ImageGeom::Pointer gridImageGeom = std::dynamic_pointer_cast<ImageGeom>(gridGeometry->deepCopy());
    gridDC->setGeometry(gridImageGeom);

    AttributeMatrix::Pointer cellAttr = AttributeMatrix::New({gridDims[0], gridDims[1], gridDims[2]}, Detail::k_AttributeMatrixName, AttributeMatrix::Type::Cell);
    gridDC->insertOrAssign(cellAttr);

    IDataArray::Pointer outputData = inputData->createNewArray(gridDims[0] * gridDims[1] * gridDims[2], {1}, getInputDataArrayPath().getDataArrayName());
    cellAttr->insertOrAssign(outputData);
    getDataContainerArray()->insertOrAssign(gridDC);

    gridIndex++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTiltSeries::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTiltSeries::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  // Compute a grid that forms a plane parallel to the rotation axis and centered halfway back in the volume
  std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> gridPair = generateGrid();
  FloatArrayType::Pointer gridCoords = gridPair.first;
  ImageGeom::Pointer gridGeometry = gridPair.second;
  DataContainerArray::Pointer dca = getDataContainerArray();

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  std::shared_ptr<tbb::task_group> g(new tbb::task_group);
  // C++11 RIGHT HERE....
  int32_t nthreads = static_cast<int32_t>(std::thread::hardware_concurrency()); // Returns ZERO if not defined on this platform
  int32_t threadCount = 0;
#endif
  int32_t rotAxisSelection = getRotationAxis();

  // Now Start Rotating the grid around the axis
  float increment = getIncrement();
  size_t gridIndex = 0;
  for(float currentDeg = 0.0f; currentDeg < 180.0f; currentDeg += increment)
  {
    QString gridDCName = QString(Detail::k_ImageGridNamePattern).arg(gridIndex);
    DataContainer::Pointer gridDC = dca->getDataContainer(gridDCName);

    std::array<float, 4> rotationAxis = {0.0f, 0.0f, 0.0f, 0.0f};
    float radians = currentDeg * SIMPLib::Constants::k_PiOver180;
    if(k_XAxis == rotAxisSelection)
    {
      rotationAxis = {1.0f, 0.0f, 0.0f, radians};
    }
    else if(k_YAxis == rotAxisSelection)
    {
      rotationAxis = {0.0f, 1.0f, 0.0f, radians};
    }
    else if(k_ZAxis == rotAxisSelection)
    {
      rotationAxis = {0.0f, 0.0f, 1.0f, radians};
    }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    g->run(Detail::ResampleGrid(this, gridCoords, gridDC, rotationAxis, gridIndex));
    threadCount++;
    if(threadCount == nthreads)
    {
      g->wait();
      threadCount = 0;
    }
#else
    //    std::cout << "Resample Deg = " << currentDeg << std::endl;
    Detail::ResampleGrid impl(this, gridCoords, gridDC, rotationAxis, gridIndex);
    impl();
#endif

    gridIndex++;
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  // This will spill over if the number of files to process does not divide evenly by the number of threads.
  g->wait();
#endif

#if GTS_GENERATE_DEBUG_ARRAYS
  // Write out the sampling grid
  {
    DataContainer::Pointer gridGeomDC = DataContainer::New("Grid Geom (Vertex)");
    VertexGeom::Pointer gridVertGeom = VertexGeom::CreateGeometry(gridCoords->getNumberOfTuples(), "Grid Geometry", true);
    gridVertGeom->setVertices(gridCoords);
    gridGeomDC->setGeometry(gridVertGeom);
    getDataContainerArray()->insertOrAssign(gridGeomDC);
  }
  {
    DataContainer::Pointer gridGeomDC = DataContainer::New("Grid Geom (Image)");
    gridGeomDC->setGeometry(gridGeometry);
    getDataContainerArray()->insertOrAssign(gridGeomDC);
  }
#endif
}

// -----------------------------------------------------------------------------
std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> GenerateTiltSeries::generateGrid()
{
  DataContainer::Pointer inputDC = getDataContainerArray()->getDataContainer(getInputDataArrayPath().getDataContainerName());
  if(nullptr == inputDC.get())
  {
    return {FloatArrayType::NullPointer(), ImageGeom::NullPointer()};
  }
  ImageGeom::Pointer inputImageGeom = inputDC->getGeometryAs<ImageGeom>();
  //  SizeVec3Type dims = imageGeom->getDimensions();
  FloatVec3Type origin = inputImageGeom->getOrigin();
  FloatVec3Type spacing = inputImageGeom->getSpacing();
  FloatVec6Type bounds = inputImageGeom->getBoundingBox();
  FloatVec3Type center((bounds[1] - bounds[0]) / 2.0f + bounds[0], (bounds[3] - bounds[2]) / 2.0f + bounds[2], (bounds[5] - bounds[4]) / 2.0f + bounds[4]);

  size_t xAxis = 0;
  size_t yAxis = 1;
  size_t zAxis = 2;
  int32_t rotAxisSelection = getRotationAxis();

  if(k_XAxis == rotAxisSelection)
  {
    xAxis = 2;
    yAxis = 1;
    zAxis = 0;
  }
  else if(k_YAxis == rotAxisSelection)
  {
    xAxis = 0;
    yAxis = 2;
    zAxis = 1;
  }
  else if(k_ZAxis == rotAxisSelection)
  {
    xAxis = 0;
    yAxis = 1;
    zAxis = 2;
  }

  float diagonalDistance = 0.0;

  std::array<float, 3> axisLength = {bounds[2 * xAxis + 1] - bounds[2 * xAxis], bounds[2 * yAxis + 1] - bounds[2 * yAxis], bounds[2 * zAxis + 1] - bounds[2 * zAxis]};

  diagonalDistance = std::sqrtf(axisLength[xAxis] * axisLength[xAxis] + axisLength[yAxis] * axisLength[yAxis]);

  std::array<float, 3> newOrigin;

  newOrigin[xAxis] = center[xAxis] - diagonalDistance / 2.0f;
  newOrigin[yAxis] = center[yAxis] - diagonalDistance / 2.0f;
  newOrigin[zAxis] = bounds[zAxis * 2]; // Use the original origin for the 3rd Axis

  // Compute the total number of points along each axis
  FloatVec3Type resampleSpacing = getSpacing();

  float iStart = bounds[zAxis * 2];
  float iEnd = bounds[zAxis * 2 + 1];
  float iIncr = spacing[zAxis];

  float jStart = newOrigin[xAxis];
  float jEnd = newOrigin[xAxis] + diagonalDistance;
  float jIncr = resampleSpacing[xAxis];

  size_t gridPrimAxisDim = static_cast<size_t>(std::ceil((iEnd - iStart) / iIncr));
  size_t gridSecondAxisDim = static_cast<size_t>(std::ceil((jEnd - jStart) / jIncr));
  size_t totalElements = gridPrimAxisDim * gridSecondAxisDim;
  std::array<float, 2> halfSpacing = {resampleSpacing[xAxis] / 2.0f, resampleSpacing[yAxis] / 2.0f};

  ImageGeom::Pointer newGridGeom = ImageGeom::CreateGeometry("Grid Geometry");

  SizeVec3Type newGridGeomDims;
  newGridGeomDims[xAxis] = gridSecondAxisDim;
  newGridGeomDims[yAxis] = 1;
  newGridGeomDims[zAxis] = gridPrimAxisDim;

  FloatVec3Type newGridGeomSpacing;
  newGridGeomSpacing[xAxis] = jIncr;
  newGridGeomSpacing[yAxis] = spacing[yAxis];
  newGridGeomSpacing[zAxis] = iIncr;

  FloatVec3Type newGridGeomOrigin;
  newGridGeomOrigin[xAxis] = center[xAxis] - diagonalDistance / 2.0f;
  newGridGeomOrigin[yAxis] = center[yAxis];
  newGridGeomOrigin[zAxis] = bounds[zAxis * 2];

  newGridGeom->setDimensions(newGridGeomDims);
  newGridGeom->setSpacing(newGridGeomSpacing);
  newGridGeom->setOrigin(newGridGeomOrigin);

  FloatArrayType::Pointer gridCoords = FloatArrayType::CreateArray(totalElements, {3}, "Grid Coords", true);
  size_t tupleIndex = 0;
  for(float i = iStart; i < iEnd; i += iIncr)
  {
    for(float j = jStart; j < jEnd; j += jIncr)
    {
      std::array<float, 3> coords;

      coords[xAxis] = j + halfSpacing[0];
      coords[zAxis] = i + halfSpacing[1];
      coords[yAxis] = origin[yAxis] + axisLength[yAxis] / 2.0f + spacing[yAxis] / 2.0f;

      gridCoords->setTuple(tupleIndex, coords.data());
      tupleIndex++;
    }
  }
  return {gridCoords, newGridGeom};
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateTiltSeries::newFilterInstance(bool copyFilterParameters) const
{
  GenerateTiltSeries::Pointer filter = GenerateTiltSeries::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateTiltSeries::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateTiltSeries::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateTiltSeries::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateTiltSeries::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateTiltSeries::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CropCutFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateTiltSeries::getHumanLabel() const
{
  return "Generate Tilt Series";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid GenerateTiltSeries::getUuid()
{
  return QUuid("{829da805-6d7c-5106-8209-aae1c207de15}");
}
