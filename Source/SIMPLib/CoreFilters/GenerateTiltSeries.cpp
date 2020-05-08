/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *    United States Air Force Prime Contract FA8650-10-D-5210
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "GenerateTiltSeries.h"

#include <thread>
#include <cmath>

#define GTS_GENERATE_DEBUG_ARRAYS 0
// If we are writing out all the arrays for debugging then we MUST be single threaded.
#if(GTS_GENERATE_DEBUG_ARRAYS == 1)
#undef SIMPL_USE_PARALLEL_ALGORITHMS
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/task_group.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/SIMPLibVersion.h"

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
constexpr float epsijk = 1.0f;
// static const double epsijkd = 1.0;
#elif DREAM3D_ACTIVE_ROTATION
constexpr float epsijk = -1.0f;
constexpr double epsijkd = -1.0;
#endif
} // namespace Constants
} // namespace Rotations

#endif

namespace Detail
{

const QString k_AttributeMatrixName("Slice Data");

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
    IDataArray::Pointer inputData = dca->getPrereqIDataArrayFromPath(m_Filter, m_Filter->getInputDataArrayPath());
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
    IDataArray::Pointer vertexOutputData = outputData->createNewArray(copyOfGrid->getNumberOfTuples(), outputData->getComponentDimensions(), outputData->getName(), true);
    vertexOutputData->initializeWithZeros();
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

// Set the transformed coordinate into the copy of the grid Vertex List
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
      // This line is OK to have before the error condition is checked because we are writing the value not
      // using it to step into an array. This also allows us to debug the data coming out of this
      // algorithm
      inputVoxIndex->setTuple(tupleIndex, &inputVoxelIndex);
#endif
      if(err != ImageGeom::ErrorType::NoError)
      {
        continue;
      }
      outputData->copyFromArray(outputVoxelIndex, inputData, inputVoxelIndex, 1);
#if GTS_GENERATE_DEBUG_ARRAYS
      // Write the selected cell value array from the input data onto the Vertex Cell Array
      // This allows us to visualize the data more easily in ParaView
      vertexOutputData->copyFromArray(tupleIndex, inputData, inputVoxelIndex, 1);
#endif
    }

#if GTS_GENERATE_DEBUG_ARRAYS
    // Write out the sampling grid
    QString dcName = QString("%1 Vertex Grid %2").arg(m_Filter->getOutputPrefix()).arg(m_GridIndex);
    DataContainer::Pointer gridGeomDC = DataContainer::New(dcName);
    VertexGeom::Pointer gridVertGeom = VertexGeom::CreateGeometry(copyOfGrid->getNumberOfTuples(), "Grid Geometry", true);
    gridVertGeom->setVertices(copyOfGrid);
    gridGeomDC->setGeometry(gridVertGeom);
    dca->insertOrAssign(gridGeomDC);
    AttributeMatrix::Pointer cellAttrMat = AttributeMatrix::New({copyOfGrid->getNumberOfTuples()}, Detail::k_AttributeMatrixName, AttributeMatrix::Type::Vertex);
    gridGeomDC->insertOrAssign(cellAttrMat);
    cellAttrMat->insertOrAssign(vertexOutputData);
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

    QVector<QString> choices; // Please add choices to the choices QVector to finish this widget
    choices << "X Axis"
            << "Y Axis"
            << "Z Axis";
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameter->setSetterCallback(SIMPL_BIND_SETTER(GenerateTiltSeries, this, RotationAxis));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(GenerateTiltSeries, this, RotationAxis));
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Rotation Limits (Start, Stop, Increment) Degrees", RotationLimits, FilterParameter::Parameter, GenerateTiltSeries));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Resample Spacing", Spacing, FilterParameter::Parameter, GenerateTiltSeries));
  //  DataArrayCreationFilterParameter::RequirementType dacReq;
  //  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Created Array Path", CreatedArrayPath, FilterParameter::Parameter, GenerateTiltSeries, dacReq));
  DataArraySelectionFilterParameter::RequirementType dasReq;
  dasReq.dcGeometryTypes = {IGeometry::Type::Image};
  dasReq.amTypes = {AttributeMatrix::Type::Cell};
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Input Data Array Path", InputDataArrayPath, FilterParameter::Parameter, GenerateTiltSeries, dasReq));
  parameters.push_back(SIMPL_NEW_STRING_FP("DataContainer Prefix", OutputPrefix, FilterParameter::Category::Parameter, GenerateTiltSeries));
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
  IDataArray::Pointer inputData = dca->getPrereqIDataArrayFromPath(this, getInputDataArrayPath());
  if(getErrorCode() < 0)
  {
    return;
  }

  // Generate Data Structure
  std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> gridPair;
  if(getRotationAxis() == k_XAxis)
  {
    gridPair = generateXAxisGrid();
  }
  else if(getRotationAxis() == k_YAxis)
  {
    gridPair = generateYAxisGrid();
  }
  else if(getRotationAxis() == k_ZAxis)
  {
    gridPair = generateZAxisGrid();
  }
  if(nullptr == gridPair.first)
  {
    return;
  }
  FloatArrayType::Pointer gridCoords = gridPair.first;
  ImageGeom::Pointer gridGeometry = gridPair.second;
  SizeVec3Type gridDims = gridGeometry->getDimensions();

  size_t gridIndex = 0;

  for(float currentDeg = m_RotationLimits[0]; currentDeg < m_RotationLimits[1]; currentDeg += m_RotationLimits[2])
  {
    QString gridDCName = m_OutputPrefix + QString::number(gridIndex);
    DataContainer::Pointer gridDC = DataContainer::New(gridDCName);
    ImageGeom::Pointer gridImageGeom = std::dynamic_pointer_cast<ImageGeom>(gridGeometry->deepCopy());
    gridDC->setGeometry(gridImageGeom);

    AttributeMatrix::Pointer cellAttr = AttributeMatrix::New({gridDims[0], gridDims[1], gridDims[2]}, Detail::k_AttributeMatrixName, AttributeMatrix::Type::Cell);
    gridDC->insertOrAssign(cellAttr);

    IDataArray::Pointer outputData =
        inputData->createNewArray(gridDims[0] * gridDims[1] * gridDims[2], inputData->getComponentDimensions(), getInputDataArrayPath().getDataArrayName(), !getInPreflight());
    cellAttr->insertOrAssign(outputData);
    getDataContainerArray()->insertOrAssign(gridDC);

    gridIndex++;
  }
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
  std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> gridPair;
  if(getRotationAxis() == k_XAxis)
  {
    gridPair = generateXAxisGrid();
  }
  else if(getRotationAxis() == k_YAxis)
  {
    gridPair = generateYAxisGrid();
  }
  else if(getRotationAxis() == k_ZAxis)
  {
    gridPair = generateZAxisGrid();
  }
  FloatArrayType::Pointer gridCoords = gridPair.first;
  ImageGeom::Pointer gridGeometry = gridPair.second;
  DataContainerArray::Pointer dca = getDataContainerArray();

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  std::shared_ptr<tbb::task_group> g(new tbb::task_group);
  // C++11 RIGHT HERE....
  int32_t nthreads = static_cast<int32_t>(std::thread::hardware_concurrency()); // Returns ZERO if not defined on this platform
  int32_t threadCount = 0;
#endif
  int32_t rotAxisSelection = getRotationAxis();

  // Now Start Rotating the grid around the axis
  size_t gridIndex = 0;
  for(float currentDeg = m_RotationLimits[0]; currentDeg < m_RotationLimits[1]; currentDeg += m_RotationLimits[2])
  {
    QString msg;
    QTextStream out(&msg);
    out << "Generating Tilt " << currentDeg << " (Deg)";
    notifyStatusMessage(msg);
    QString gridDCName = m_OutputPrefix + QString::number(gridIndex);
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
    QString dcName = QString("%1 Grid Geom (Vertex)").arg(getOutputPrefix());
    DataContainer::Pointer gridGeomDC = DataContainer::New(dcName);
    VertexGeom::Pointer gridVertGeom = VertexGeom::CreateGeometry(gridCoords->getNumberOfTuples(), "Grid Geometry", true);
    gridVertGeom->setVertices(gridCoords);
    gridGeomDC->setGeometry(gridVertGeom);
    getDataContainerArray()->insertOrAssign(gridGeomDC);
  }
  {
    QString dcName = QString("%1 Grid Geom (Image)").arg(getOutputPrefix());
    DataContainer::Pointer gridGeomDC = DataContainer::New(dcName);
    gridGeomDC->setGeometry(gridGeometry);
    getDataContainerArray()->insertOrAssign(gridGeomDC);
  }
#endif
}

// -----------------------------------------------------------------------------
std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> GenerateTiltSeries::generateZAxisGrid()
{
  DataContainer::Pointer inputDC = getDataContainerArray()->getDataContainer(getInputDataArrayPath().getDataContainerName());
  if(nullptr == inputDC.get())
  {
    return {FloatArrayType::NullPointer(), ImageGeom::NullPointer()};
  }
  ImageGeom::Pointer inputImageGeom = inputDC->getGeometryAs<ImageGeom>();
  FloatVec3Type origin = inputImageGeom->getOrigin();
  FloatVec3Type spacing = inputImageGeom->getSpacing();
  FloatVec6Type bounds = inputImageGeom->getBoundingBox();
  FloatVec3Type center((bounds[1] - bounds[0]) / 2.0f + bounds[0], (bounds[3] - bounds[2]) / 2.0f + bounds[2], (bounds[5] - bounds[4]) / 2.0f + bounds[4]);

  size_t xAxis = 0;
  size_t yAxis = 1;
  size_t zAxis = 2;
  float diagonalDistance = 0.0;

  // Compute the total number of points along each axis
  FloatVec3Type resampleSpacing = getSpacing();

  std::array<float, 3> axisLength = {0.0f, 0.0f, 0.0f};
  std::array<float, 3> newOrigin = {0.0f, 0.0f, 0.0f};

  float iStart = bounds[zAxis * 2];
  float iEnd = bounds[zAxis * 2 + 1];
  float iIncr = spacing[zAxis];

  float jStart = newOrigin[xAxis];
  float jEnd = newOrigin[xAxis] + diagonalDistance;
  float jIncr = resampleSpacing[xAxis];

  axisLength = {bounds[2 * xAxis + 1] - bounds[2 * xAxis], bounds[2 * yAxis + 1] - bounds[2 * yAxis], bounds[2 * zAxis + 1] - bounds[2 * zAxis]};
  diagonalDistance = std::sqrt(axisLength[0] * axisLength[0] + axisLength[1] * axisLength[1]);

  newOrigin[xAxis] = center[xAxis] - diagonalDistance / 2.0f;
  newOrigin[yAxis] = center[yAxis];
  newOrigin[zAxis] = bounds[zAxis * 2]; // Use the original origin for the 3rd Axis

  iStart = bounds[zAxis * 2];
  iEnd = bounds[zAxis * 2 + 1];
  iIncr = resampleSpacing[zAxis];

  jStart = newOrigin[xAxis];
  jEnd = newOrigin[xAxis] + diagonalDistance;
  jIncr = resampleSpacing[xAxis];

  size_t gridPrimAxisDim = static_cast<size_t>(std::ceil((iEnd - iStart) / iIncr));
  size_t gridSecondAxisDim = static_cast<size_t>(std::ceil((jEnd - jStart) / jIncr));
  size_t totalElements = gridPrimAxisDim * gridSecondAxisDim;
  if(totalElements == 0)
  {
    return {FloatArrayType::NullPointer(), ImageGeom::NullPointer()};
  }

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

  FloatArrayType::Pointer gridCoords = FloatArrayType::CreateArray(totalElements, {3}, "Grid Coords", !getInPreflight());
  if(!getInPreflight())
  {
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
  }
  return {gridCoords, newGridGeom};
}

// -----------------------------------------------------------------------------
std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> GenerateTiltSeries::generateYAxisGrid()
{
  DataContainer::Pointer inputDC = getDataContainerArray()->getDataContainer(getInputDataArrayPath().getDataContainerName());
  if(nullptr == inputDC.get())
  {
    return {FloatArrayType::NullPointer(), ImageGeom::NullPointer()};
  }
  ImageGeom::Pointer inputImageGeom = inputDC->getGeometryAs<ImageGeom>();
  FloatVec3Type origin = inputImageGeom->getOrigin();
  FloatVec3Type spacing = inputImageGeom->getSpacing();
  FloatVec6Type bounds = inputImageGeom->getBoundingBox();
  FloatVec3Type center((bounds[1] - bounds[0]) / 2.0f + bounds[0], (bounds[3] - bounds[2]) / 2.0f + bounds[2], (bounds[5] - bounds[4]) / 2.0f + bounds[4]);

  size_t xAxis = 0;
  size_t yAxis = 1;
  size_t zAxis = 2;
  float diagonalDistance = 0.0;

  // Compute the total number of points along each axis
  FloatVec3Type resampleSpacing = getSpacing();

  std::array<float, 3> axisLength = {0.0f, 0.0f, 0.0f};
  std::array<float, 3> newOrigin = {0.0f, 0.0f, 0.0f};

  axisLength = {bounds[2 * yAxis + 1] - bounds[2 * yAxis], bounds[2 * zAxis + 1] - bounds[2 * zAxis], bounds[2 * xAxis + 1] - bounds[2 * xAxis]};
  diagonalDistance = std::sqrt(axisLength[0] * axisLength[0] + axisLength[1] * axisLength[1]);

  newOrigin[xAxis] = center[xAxis];
  newOrigin[yAxis] = bounds[yAxis * 2];
  newOrigin[zAxis] = center[zAxis] - diagonalDistance / 2.0f;

  float iStart = newOrigin[yAxis];
  float iEnd = bounds[yAxis * 2 + 1];
  float iIncr = resampleSpacing[yAxis];

  float jStart = newOrigin[zAxis];
  float jEnd = newOrigin[zAxis] + diagonalDistance;
  float jIncr = resampleSpacing[zAxis];

  size_t gridPrimAxisDim = static_cast<size_t>(std::ceil((iEnd - iStart) / iIncr));
  size_t gridSecondAxisDim = static_cast<size_t>(std::ceil((jEnd - jStart) / jIncr));
  size_t totalElements = gridPrimAxisDim * gridSecondAxisDim;
  if(totalElements == 0)
  {
    return {FloatArrayType::NullPointer(), ImageGeom::NullPointer()};
  }

  std::array<float, 2> halfSpacing = {resampleSpacing[yAxis] / 2.0f, resampleSpacing[zAxis] / 2.0f};

  ImageGeom::Pointer newGridGeom = ImageGeom::CreateGeometry("Grid Geometry");

  SizeVec3Type newGridGeomDims;
  newGridGeomDims[xAxis] = 1;
  ;
  newGridGeomDims[yAxis] = gridPrimAxisDim;
  newGridGeomDims[zAxis] = gridSecondAxisDim;

  FloatVec3Type newGridGeomSpacing;
  newGridGeomSpacing[xAxis] = spacing[xAxis];
  newGridGeomSpacing[yAxis] = iIncr;
  newGridGeomSpacing[zAxis] = jIncr;

  FloatVec3Type newGridGeomOrigin;
  newGridGeomOrigin[xAxis] = newOrigin[xAxis];
  newGridGeomOrigin[yAxis] = newOrigin[yAxis];
  newGridGeomOrigin[zAxis] = newOrigin[zAxis];

  newGridGeom->setDimensions(newGridGeomDims);
  newGridGeom->setSpacing(newGridGeomSpacing);
  newGridGeom->setOrigin(newGridGeomOrigin);

  FloatArrayType::Pointer gridCoords = FloatArrayType::CreateArray(totalElements, {3}, "Grid Coords", !getInPreflight());
  if(!getInPreflight())
  {
    size_t tupleIndex = 0;
    for(float i = iStart; i < iEnd; i += iIncr)
    {
      for(float j = jStart; j < jEnd; j += jIncr)
      {
        std::array<float, 3> coords;

        coords[xAxis] = origin[xAxis] + axisLength[2] / 2.0f + spacing[xAxis] / 2.0f;
        coords[yAxis] = i + halfSpacing[0];
        coords[zAxis] = j + halfSpacing[1];

        gridCoords->setTuple(tupleIndex, coords.data());

        tupleIndex++;
      }
    }
  }
  return {gridCoords, newGridGeom};
}

// -----------------------------------------------------------------------------
std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> GenerateTiltSeries::generateXAxisGrid()
{
  DataContainer::Pointer inputDC = getDataContainerArray()->getDataContainer(getInputDataArrayPath().getDataContainerName());
  if(nullptr == inputDC.get())
  {
    return {FloatArrayType::NullPointer(), ImageGeom::NullPointer()};
  }
  ImageGeom::Pointer inputImageGeom = inputDC->getGeometryAs<ImageGeom>();
  FloatVec3Type origin = inputImageGeom->getOrigin();
  FloatVec3Type spacing = inputImageGeom->getSpacing();
  FloatVec6Type bounds = inputImageGeom->getBoundingBox();
  FloatVec3Type center((bounds[1] - bounds[0]) / 2.0f + bounds[0], (bounds[3] - bounds[2]) / 2.0f + bounds[2], (bounds[5] - bounds[4]) / 2.0f + bounds[4]);

  size_t xAxis = 0;
  size_t yAxis = 1;
  size_t zAxis = 2;
  float diagonalDistance = 0.0;

  // Compute the total number of points along each axis
  FloatVec3Type resampleSpacing = getSpacing();

  std::array<float, 3> axisLength = {0.0f, 0.0f, 0.0f};
  std::array<float, 3> newOrigin = {0.0f, 0.0f, 0.0f};

  axisLength = {bounds[2 * xAxis + 1] - bounds[2 * xAxis], bounds[2 * yAxis + 1] - bounds[2 * yAxis], bounds[2 * zAxis + 1] - bounds[2 * zAxis]};
  diagonalDistance = std::sqrt(axisLength[xAxis] * axisLength[xAxis] + axisLength[yAxis] * axisLength[yAxis]);

  newOrigin[xAxis] = bounds[xAxis]; // xMin
  newOrigin[yAxis] = center[yAxis] - diagonalDistance / 2.0f;
  newOrigin[zAxis] = center[zAxis];

  float iStart = newOrigin[xAxis];
  float iEnd = bounds[xAxis * 2 + 1];
  float iIncr = resampleSpacing[xAxis];

  float jStart = newOrigin[yAxis];
  float jEnd = newOrigin[yAxis] + diagonalDistance;
  float jIncr = resampleSpacing[yAxis];

  size_t gridPrimAxisDim = static_cast<size_t>(std::ceil((iEnd - iStart) / iIncr));
  size_t gridSecondAxisDim = static_cast<size_t>(std::ceil((jEnd - jStart) / jIncr));
  size_t totalElements = gridPrimAxisDim * gridSecondAxisDim;
  if(totalElements == 0)
  {
    return {FloatArrayType::NullPointer(), ImageGeom::NullPointer()};
  }

  std::array<float, 2> halfSpacing = {resampleSpacing[xAxis] / 2.0f, resampleSpacing[yAxis] / 2.0f};

  ImageGeom::Pointer newGridGeom = ImageGeom::CreateGeometry("Grid Geometry");

  SizeVec3Type newGridGeomDims;
  newGridGeomDims[xAxis] = gridPrimAxisDim;
  ;
  newGridGeomDims[yAxis] = gridSecondAxisDim;
  newGridGeomDims[zAxis] = 1;

  FloatVec3Type newGridGeomSpacing;
  newGridGeomSpacing[xAxis] = iIncr;
  newGridGeomSpacing[yAxis] = jIncr;
  newGridGeomSpacing[zAxis] = spacing[zAxis];

  FloatVec3Type newGridGeomOrigin;
  newGridGeomOrigin[xAxis] = newOrigin[xAxis];
  newGridGeomOrigin[yAxis] = newOrigin[yAxis];
  newGridGeomOrigin[zAxis] = newOrigin[zAxis];

  newGridGeom->setDimensions(newGridGeomDims);
  newGridGeom->setSpacing(newGridGeomSpacing);
  newGridGeom->setOrigin(newGridGeomOrigin);

  FloatArrayType::Pointer gridCoords = FloatArrayType::CreateArray(totalElements, {3}, "Grid Coords", !getInPreflight());
  if(!getInPreflight())
  {
    size_t tupleIndex = 0;
    for(float i = iStart; i < iEnd; i += iIncr)
    {
      for(float j = jStart; j < jEnd; j += jIncr)
      {
        std::array<float, 3> coords;

        coords[xAxis] = i + halfSpacing[0];
        coords[yAxis] = j + halfSpacing[1];
        coords[zAxis] = origin[zAxis] + axisLength[zAxis] / 2.0f + spacing[zAxis] / 2.0f;

        gridCoords->setTuple(tupleIndex, coords.data());

        tupleIndex++;
      }
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
QString GenerateTiltSeries::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateTiltSeries::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateTiltSeries::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateTiltSeries::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateTiltSeries::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CropCutFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateTiltSeries::getHumanLabel() const
{
  return "Generate Tilt Series";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GenerateTiltSeries::getUuid() const
{
  return QUuid("{829da805-6d7c-5106-8209-aae1c207de15}");
}

// -----------------------------------------------------------------------------
GenerateTiltSeries::Pointer GenerateTiltSeries::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GenerateTiltSeries> GenerateTiltSeries::New()
{
  struct make_shared_enabler : public GenerateTiltSeries
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GenerateTiltSeries::getNameOfClass() const
{
  return QString("GenerateTiltSeries");
}

// -----------------------------------------------------------------------------
QString GenerateTiltSeries::ClassName()
{
  return QString("GenerateTiltSeries");
}

// -----------------------------------------------------------------------------
void GenerateTiltSeries::setRotationAxis(int value)
{
  m_RotationAxis = value;
}

// -----------------------------------------------------------------------------
int GenerateTiltSeries::getRotationAxis() const
{
  return m_RotationAxis;
}

// -----------------------------------------------------------------------------
void GenerateTiltSeries::setRotationLimits(const FloatVec3Type& value)
{
  m_RotationLimits = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type GenerateTiltSeries::getRotationLimits() const
{
  return m_RotationLimits;
}

// -----------------------------------------------------------------------------
void GenerateTiltSeries::setSpacing(const FloatVec3Type& value)
{
  m_Spacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type GenerateTiltSeries::getSpacing() const
{
  return m_Spacing;
}

// -----------------------------------------------------------------------------
void GenerateTiltSeries::setInputDataArrayPath(const DataArrayPath& value)
{
  m_InputDataArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateTiltSeries::getInputDataArrayPath() const
{
  return m_InputDataArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateTiltSeries::setOutputPrefix(const QString& value)
{
  m_OutputPrefix = value;
}

// -----------------------------------------------------------------------------
QString GenerateTiltSeries::getOutputPrefix() const
{
  return m_OutputPrefix;
}
