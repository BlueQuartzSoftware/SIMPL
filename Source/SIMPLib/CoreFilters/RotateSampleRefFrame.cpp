/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "RotateSampleRefFrame.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#endif

#include <QtCore/QTextStream>

#include <Eigen/Dense>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/MatrixMath.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/task_group.h>
#endif

namespace ImageRotationUtilities
{
struct RotateArgs
{
  ImageGeom::Pointer origImageGeom;

  int64_t xp = 0;
  int64_t yp = 0;
  int64_t zp = 0;
  float xRes = 0.0f;
  float yRes = 0.0f;
  float zRes = 0.0f;

  ImageGeom::Pointer transformedImageGeom;
  int64_t xpNew = 0;
  int64_t ypNew = 0;
  int64_t zpNew = 0;
  float xResNew = 0.0f;
  float yResNew = 0.0f;
  float zResNew = 0.0f;
  float xMinNew = 0.0f;
  float yMinNew = 0.0f;
  float zMinNew = 0.0f;
};
} // namespace ImageRotationUtilities
namespace
{
using Matrix3fR = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;
using Matrix4fR = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;

// constexpr float k_Threshold = 0.0001f;

const Eigen::Vector3f k_XAxis = Eigen::Vector3f::UnitX();
const Eigen::Vector3f k_YAxis = Eigen::Vector3f::UnitY();
const Eigen::Vector3f k_ZAxis = Eigen::Vector3f::UnitZ();

/**
 * @brief Function to determine the min and max coordinates (bounding box) of the transformed Image Geometry.
 * @param imageGeometry
 * @param transformationMatrix
 * @return
 */
FloatVec6Type DetermineMinMaxCoords(const ImageGeom& imageGeometry, const Matrix4fR& transformationMatrix)
{
  auto origImageGeomBox = imageGeometry.getBoundingBox();
  // clang-format off
  std::vector<FloatVec3Type> imageGeomCornerCoords = {{origImageGeomBox[0], origImageGeomBox[2], origImageGeomBox[4]},
                                                      {origImageGeomBox[1], origImageGeomBox[2], origImageGeomBox[4]},
                                                      {origImageGeomBox[1], origImageGeomBox[3], origImageGeomBox[4]},
                                                      {origImageGeomBox[0], origImageGeomBox[3], origImageGeomBox[4]},
                                                      {origImageGeomBox[0], origImageGeomBox[2], origImageGeomBox[5]},
                                                      {origImageGeomBox[1], origImageGeomBox[2], origImageGeomBox[5]},
                                                      {origImageGeomBox[1], origImageGeomBox[3], origImageGeomBox[5]},
                                                      {origImageGeomBox[0], origImageGeomBox[3], origImageGeomBox[5]}};
  // clang-format on
  FloatVec6Type minMaxValues = {std::numeric_limits<float>::max(),  -std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                -std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),  -std::numeric_limits<float>::max()};

  for(size_t i = 0; i < 8; i++)
  {
    Eigen::Vector4f coords(imageGeomCornerCoords[i][0], imageGeomCornerCoords[i][1], imageGeomCornerCoords[i][2], 1.0F);

    Eigen::Vector4f newCoords = transformationMatrix * coords;

    minMaxValues[0] = std::min(newCoords[0], minMaxValues[0]);
    minMaxValues[1] = std::max(newCoords[0], minMaxValues[1]);

    minMaxValues[2] = std::min(newCoords[1], minMaxValues[2]);
    minMaxValues[3] = std::max(newCoords[1], minMaxValues[3]);

    minMaxValues[4] = std::min(newCoords[2], minMaxValues[4]);
    minMaxValues[5] = std::max(newCoords[2], minMaxValues[5]);
  }
  return minMaxValues;
}

/**
 * @brief Finds the Cosine of the angle between 2 vectors
 * @param a
 * @param b
 * @return
 */
float CosBetweenVectors(const Eigen::Vector3f& a, const Eigen::Vector3f& b)
{
  float normA = a.norm();
  float normB = b.norm();

  if(normA == 0.0f || normB == 0.0f)
  {
    return 1.0f;
  }

  return a.dot(b) / (normA * normB);
}

// -----------------------------------------------------------------------------
/**
 * @brief Function for determining new ImageGeom Spacing between points for scaling
 * @param spacing
 * @param axisNew
 * @return spacing for a given axis.
 */
float DetermineSpacing(const FloatVec3Type& spacing, const Eigen::Vector3f& axisNew)
{
  float xAngle = std::abs(CosBetweenVectors(k_XAxis, axisNew));
  float yAngle = std::abs(CosBetweenVectors(k_YAxis, axisNew));
  float zAngle = std::abs(CosBetweenVectors(k_ZAxis, axisNew));

  std::array<float, 3> axes = {xAngle, yAngle, zAngle};

  auto iter = std::max_element(axes.cbegin(), axes.cend());

  size_t index = std::distance(axes.cbegin(), iter);

  return spacing[index];
}

/**
 * @brief Determines paramaters for image rotation
 * @param imageGeom
 * @param transformationMatrix
 * @return New RotateArgs object
 */
ImageRotationUtilities::RotateArgs CreateRotationArgs(const ImageGeom& imageGeom, const Matrix4fR& transformationMatrix)
{
  const SizeVec3Type origDims = imageGeom.getDimensions();
  const FloatVec3Type spacing = imageGeom.getSpacing();

  Matrix3fR rotationMatrix = transformationMatrix.block(0, 0, 3, 3);

  FloatVec6Type minMaxCoords = DetermineMinMaxCoords(imageGeom, transformationMatrix);

  Eigen::Vector3f xAxisNew = rotationMatrix * k_XAxis;
  Eigen::Vector3f yAxisNew = rotationMatrix * k_YAxis;
  Eigen::Vector3f zAxisNew = rotationMatrix * k_ZAxis;

  float xResNew = DetermineSpacing(spacing, xAxisNew);
  float yResNew = DetermineSpacing(spacing, yAxisNew);
  float zResNew = DetermineSpacing(spacing, zAxisNew);

  MeshIndexType xpNew = static_cast<int64_t>(std::nearbyint((minMaxCoords[1] - minMaxCoords[0]) / xResNew));
  MeshIndexType ypNew = static_cast<int64_t>(std::nearbyint((minMaxCoords[3] - minMaxCoords[2]) / yResNew));
  MeshIndexType zpNew = static_cast<int64_t>(std::nearbyint((minMaxCoords[5] - minMaxCoords[4]) / zResNew));

  ImageRotationUtilities::RotateArgs params;

  params.origImageGeom = ImageGeom::New();
  params.origImageGeom->setSpacing(imageGeom.getSpacing());
  params.origImageGeom->setDimensions(imageGeom.getDimensions());
  params.origImageGeom->setOrigin(imageGeom.getOrigin());

  params.xp = origDims[0];
  params.xRes = spacing[0];
  params.yp = origDims[1];
  params.yRes = spacing[1];
  params.zp = origDims[2];
  params.zRes = spacing[2];

  params.transformedImageGeom = ImageGeom::New();
  params.transformedImageGeom->setSpacing(xResNew, yResNew, zResNew);
  params.transformedImageGeom->setDimensions(xpNew, ypNew, zpNew);
  params.transformedImageGeom->setOrigin(minMaxCoords[0], minMaxCoords[2], minMaxCoords[4]);

  params.xpNew = xpNew;
  params.xResNew = xResNew;
  params.xMinNew = minMaxCoords[0];
  params.ypNew = ypNew;
  params.yResNew = yResNew;
  params.yMinNew = minMaxCoords[2];
  params.zpNew = zpNew;
  params.zResNew = zResNew;
  params.zMinNew = minMaxCoords[4];

  return params;
}

/**
 * @brief The RotateSampleRefFrameImpl class implements a threaded algorithm to do the
 * actual computation of the rotation by applying the rotation to each element
 */
class SampleRefFrameRotator
{
  DataArray<int64_t>::Pointer m_NewIndicesPtr;
  float m_RotMatrixInv[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  bool m_SliceBySlice = false;
  ImageRotationUtilities::RotateArgs m_Params;

public:
  SampleRefFrameRotator(DataArray<int64_t>::Pointer newindices, const ImageRotationUtilities::RotateArgs& args, const Matrix3fR& rotationMatrix, bool sliceBySlice)
  : m_NewIndicesPtr(newindices)
  , m_SliceBySlice(sliceBySlice)
  , m_Params(args)
  {
    // We have to inline the 3x3 Maxtrix transpose here because of the "const" nature of the 'convert' function
    Matrix3fR transpose = rotationMatrix.transpose();
    // Need to use row based Eigen matrix so that the values get mapped to the right place in the raw array
    // Raw array is faster than Eigen
    Eigen::Map<Matrix3fR>(&m_RotMatrixInv[0][0], transpose.rows(), transpose.cols()) = transpose;
  }

  ~SampleRefFrameRotator() = default;

  void convert(int64_t zStart, int64_t zEnd, int64_t yStart, int64_t yEnd, int64_t xStart, int64_t xEnd) const
  {
    int64_t* newindicies = m_NewIndicesPtr->getPointer(0);

    for(int64_t k = zStart; k < zEnd; k++)
    {
      int64_t ktot = (m_Params.xpNew * m_Params.ypNew) * k;
      for(int64_t j = yStart; j < yEnd; j++)
      {
        int64_t jtot = (m_Params.xpNew) * j;
        for(int64_t i = xStart; i < xEnd; i++)
        {
          int64_t index = ktot + jtot + i;
          newindicies[index] = -1;

          float coords[3] = {0.0f, 0.0f, 0.0f};
          float coordsNew[3] = {0.0f, 0.0f, 0.0f};

          coords[0] = (static_cast<float>(i) * m_Params.xResNew) + m_Params.xMinNew;
          coords[1] = (static_cast<float>(j) * m_Params.yResNew) + m_Params.yMinNew;
          coords[2] = (static_cast<float>(k) * m_Params.zResNew) + m_Params.zMinNew;

          MatrixMath::Multiply3x3with3x1(m_RotMatrixInv, coords, coordsNew);

          int64_t colOld = static_cast<int64_t>(std::nearbyint(coordsNew[0] / m_Params.xRes));
          int64_t rowOld = static_cast<int64_t>(std::nearbyint(coordsNew[1] / m_Params.yRes));
          int64_t planeOld = static_cast<int64_t>(std::nearbyint(coordsNew[2] / m_Params.zRes));

          if(m_SliceBySlice)
          {
            planeOld = k;
          }

          if(colOld >= 0 && colOld < m_Params.xp && rowOld >= 0 && rowOld < m_Params.yp && planeOld >= 0 && planeOld < m_Params.zp)
          {
            newindicies[index] = (m_Params.xp * m_Params.yp * planeOld) + (m_Params.xp * rowOld) + colOld;
          }
        }
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range3d<int64_t, int64_t, int64_t>& r) const
  {
    convert(r.pages().begin(), r.pages().end(), r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
  }
#endif
};

} // namespace

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
struct RotateSampleRefFrame::Impl
{
  Matrix4fR m_RotationMatrix = Matrix4fR::Zero();
  ImageRotationUtilities::RotateArgs m_Params;

  void reset()
  {
    m_RotationMatrix.setZero();

    m_Params = ImageRotationUtilities::RotateArgs();
  }
};

namespace RotateSampleRefFrameProgress
{
static size_t s_InstanceIndex = 0;
static std::map<size_t, int64_t> s_ProgressValues;
static std::map<size_t, int64_t> s_LastProgressInt;
} // namespace RotateSampleRefFrameProgress

using Vector3s = Eigen::Array<size_t, 1, 3>;
using Vector3i64 = Eigen::Array<int64_t, 1, 3>;

using Int64Vec3Type = IVec3<int64_t>;

// -----------------------------------------------------------------------------
class RotateSampleRefFrameImpl
{
public:
  RotateSampleRefFrameImpl(RotateSampleRefFrame* filter, IDataArray::Pointer& sourceArray, IDataArray::Pointer& targetArray, const ImageRotationUtilities::RotateArgs& args,
                           const Matrix4fR& rotationMatrix, bool sliceBySlice)
  : m_Filter(filter)
  , m_SourceArray(sourceArray)
  , m_TargetArray(targetArray)
  , m_SliceBySlice(sliceBySlice)
  , m_Params(args)
  , m_TransformationMatrix(rotationMatrix)
  {
  }

  virtual ~RotateSampleRefFrameImpl() = default;

  void convert() const
  {
    IDataArray& sourceArray = *(m_SourceArray.get());

    m_Filter->sendThreadSafeProgressMessage(QString("%1: Transform Starting").arg(sourceArray.getName()));

    Vector3s origImageGeomDims(m_Params.origImageGeom->getDimensions().data());
    Eigen::Vector3f coordsOld(0.0F, 0.0F, 0.0F);

    Vector3s oldGeomIndices = {std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max()};

    Eigen::Vector4f coordsNew;

    Matrix4fR inverseTransform = m_TransformationMatrix.inverse();

    for(int64_t k = 0; k < m_Params.zpNew; k++)
    {
      if(m_Filter->getCancel() || m_Filter->getErrorCode() < 0)
      {
        break;
      }
      int64_t ktot = (m_Params.xpNew * m_Params.ypNew) * k;

      m_Filter->sendThreadSafeProgressMessage(QString("%1: Copying values for slice '%2/%3'").arg(m_SourceArray->getName()).arg(k).arg(m_Params.zpNew));
      for(int64_t j = 0; j < m_Params.ypNew; j++)
      {
        int64_t jtot = (m_Params.xpNew) * j;
        for(int64_t i = 0; i < m_Params.xpNew; i++)
        {
          int64_t newIndex = ktot + jtot + i;
          coordsNew[0] = (static_cast<float>(i) * m_Params.xResNew) + m_Params.xMinNew + 0.5F * m_Params.xResNew;
          coordsNew[1] = (static_cast<float>(j) * m_Params.yResNew) + m_Params.yMinNew + 0.5F * m_Params.yResNew;
          coordsNew[2] = (static_cast<float>(k) * m_Params.zResNew) + m_Params.zMinNew + 0.5F * m_Params.zResNew;
          coordsNew[3] = 1.0F; // We take translation into account

          Eigen::Array4f coordsOld = inverseTransform * coordsNew;

          auto errorResult = m_Params.origImageGeom->computeCellIndex(coordsOld.data(), oldGeomIndices.data());

          // Now we know what voxel the new cell center maps back to in the original geometry.
          if(errorResult == ImageGeom::ErrorType::NoError)
          {
            size_t oldIndex = (origImageGeomDims[0] * origImageGeomDims[1] * oldGeomIndices[2]) + (origImageGeomDims[0] * oldGeomIndices[1]) + oldGeomIndices[0];
            if(!m_TargetArray->copyFromArray(newIndex, m_SourceArray, oldIndex, 1))
            {
              return;
            }
          }
          else
          {
            int var = 0;
            m_TargetArray->initializeTuple(newIndex, &var);
          }
        }
      }
    }

    m_SourceArray->resizeTuples(0);

    m_Filter->sendThreadSafeProgressMessage(QString("%1: Transform Ending").arg(sourceArray.getName()));
  }

  void operator()() const
  {
    convert();
  }

private:
  RotateSampleRefFrame* m_Filter = nullptr;
  IDataArray::Pointer m_SourceArray;
  IDataArray::Pointer m_TargetArray;
  bool m_SliceBySlice = false;
  ImageRotationUtilities::RotateArgs m_Params;
  Matrix4fR m_TransformationMatrix;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::RotateSampleRefFrame()
: p_Impl(std::make_unique<Impl>())
{
  std::vector<std::vector<double>> defaultTable{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};

  m_RotationTable.setTableData(defaultTable);
  m_RotationTable.setDynamicRows(false);
  m_RotationTable.setDynamicCols(false);
  m_RotationTable.setDefaultColCount(3);
  m_RotationTable.setDefaultRowCount(3);
  m_RotationTable.setMinCols(3);
  m_RotationTable.setMinRows(3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::~RotateSampleRefFrame() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Rotation Representation");
    parameter->setPropertyName("RotationRepresentationChoice");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(RotateSampleRefFrame, this, RotationRepresentationChoice));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(RotateSampleRefFrame, this, RotationRepresentationChoice));
    std::vector<QString> choices{"Axis Angle", "Rotation Matrix"};
    parameter->setChoices(choices);
    std::vector<QString> linkedProps{"RotationAngle", "RotationAxis", "RotationTable"};
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Category::Parameter);
    parameters.push_back(parameter);
  }

  // Axis Angle Parameters

  parameters.push_back(SIMPL_NEW_FLOAT_FP("Rotation Angle (Degrees)", RotationAngle, FilterParameter::Category::Parameter, RotateSampleRefFrame, {0}));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Rotation Axis (ijk)", RotationAxis, FilterParameter::Category::Parameter, RotateSampleRefFrame, {0}));

  // Rotation Matrix Parameters

  parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Rotation Matrix", RotationTable, FilterParameter::Category::Parameter, RotateSampleRefFrame, {1}));

  // Required Arrays

  parameters.push_back(SeparatorFilterParameter::Create("Cell Data", FilterParameter::Category::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", CellAttributeMatrixPath, FilterParameter::Category::RequiredArray, RotateSampleRefFrame, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellAttributeMatrixPath(reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath()));
  setRotationAxis(reader->readFloatVec3("RotationAxis", getRotationAxis()));
  setRotationAngle(reader->readValue("RotationAngle", getRotationAngle()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  p_Impl->reset();

  if(getErrorCode() < 0)
  {
    return;
  }

  if(!isRotationRepresentationValid(m_RotationRepresentationChoice))
  {
    QString ss = QObject::tr("Invalid rotation representation");
    setErrorCondition(-45001, ss);
    return;
  }

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom>(this, getCellAttributeMatrixPath().getDataContainerName());
  getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, getCellAttributeMatrixPath(), -301);
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  if(m == nullptr)
  {
    QString ss = QObject::tr("Failed to get DataContainer '%1'").arg(getCellAttributeMatrixPath().getDataContainerName());
    setErrorCondition(-45002, ss);
    return;
  }

  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();

  if(imageGeom == nullptr)
  {
    QString ss = QObject::tr("Failed to get Image Geometry from '%1'").arg(getCellAttributeMatrixPath().getDataContainerName());
    setErrorCondition(-45002, ss);
    return;
  }

  const RotationRepresentation representation = getRotationRepresentation();

  switch(representation)
  {
  case RotationRepresentation::AxisAngle:
  {
    // Convert Degrees to Radians for the last element
    float rotAngle = m_RotationAngle * SIMPLib::Constants::k_PiOver180D;

    // Ensure the axis part is normalized
    FloatVec3Type normalizedAxis = m_RotationAxis;
    MatrixMath::Normalize3x1(normalizedAxis.data());

    float cosTheta = cos(rotAngle);
    float oneMinusCosTheta = 1 - cosTheta;
    float sinTheta = sin(rotAngle);
    float l = normalizedAxis[0];
    float m = normalizedAxis[1];
    float n = normalizedAxis[2];

    // First Row:
    p_Impl->m_RotationMatrix(0) = l * l * (oneMinusCosTheta) + cosTheta;
    p_Impl->m_RotationMatrix(1) = m * l * (oneMinusCosTheta) - (n * sinTheta);
    p_Impl->m_RotationMatrix(2) = n * l * (oneMinusCosTheta) + (m * sinTheta);
    p_Impl->m_RotationMatrix(3) = 0.0F;

    // Second Row:
    p_Impl->m_RotationMatrix(4) = l * m * (oneMinusCosTheta) + (n * sinTheta);
    p_Impl->m_RotationMatrix(5) = m * m * (oneMinusCosTheta) + cosTheta;
    p_Impl->m_RotationMatrix(6) = n * m * (oneMinusCosTheta) - (l * sinTheta);
    p_Impl->m_RotationMatrix(7) = 0.0F;

    // Third Row:
    p_Impl->m_RotationMatrix(8) = l * n * (oneMinusCosTheta) - (m * sinTheta);
    p_Impl->m_RotationMatrix(9) = m * n * (oneMinusCosTheta) + (l * sinTheta);
    p_Impl->m_RotationMatrix(10) = n * n * (oneMinusCosTheta) + cosTheta;
    p_Impl->m_RotationMatrix(11) = 0.0F;

    // Fourth Row:
    p_Impl->m_RotationMatrix(12) = 0.0F;
    p_Impl->m_RotationMatrix(13) = 0.0F;
    p_Impl->m_RotationMatrix(14) = 0.0F;
    p_Impl->m_RotationMatrix(15) = 1.0F;

    break;
  }
  case RotationRepresentation::RotationMatrix: {

    int numTableRows = getRotationTable().getNumRows();
    int numTableCols = getRotationTable().getNumCols();
    if(numTableRows != 4)
    {
      QString ss = QObject::tr("Manually entered transformation matrix must have exactly 4 rows");
      setErrorCondition(-702, ss);
      return;
    }
    if(numTableCols != 4)
    {
      QString ss = QObject::tr("Manually entered transformation matrix must have exactly 4 columns");
      setErrorCondition(-703, ss);
      return;
    }
    std::vector<std::vector<double>> tableData = getRotationTable().getTableData();

    for(size_t rowIndex = 0; rowIndex < numTableRows; rowIndex++)
    {
      std::vector<double> row = tableData[rowIndex];
      for(size_t colIndex = 0; colIndex < numTableCols; colIndex++)
      {
        p_Impl->m_RotationMatrix(rowIndex, colIndex) = static_cast<float>(row[colIndex]);
      }
    }
    break;
  }

  default: {
    QString ss = QObject::tr("Invalid rotation representation");
    setErrorCondition(-45008, ss);
    return;
  }
  }

  p_Impl->m_Params = CreateRotationArgs(*imageGeom, p_Impl->m_RotationMatrix);

  imageGeom->setDimensions(p_Impl->m_Params.transformedImageGeom->getDimensions());
  imageGeom->setOrigin(p_Impl->m_Params.transformedImageGeom->getOrigin());
  imageGeom->setSpacing(p_Impl->m_Params.transformedImageGeom->getSpacing());

  // Resize attribute matrix
  std::vector<size_t> tDims = {static_cast<size_t>(p_Impl->m_Params.xpNew), static_cast<size_t>(p_Impl->m_Params.ypNew), static_cast<size_t>(p_Impl->m_Params.zpNew)};

  QString attrMatName = getCellAttributeMatrixPath().getAttributeMatrixName();
  // Get the List of Array Names FIRST
  QList<QString> selectedCellArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();
  // Now remove the current Cell Attribute Matrix and store it in the instance variable
  m_SourceAttributeMatrix = m->removeAttributeMatrix(attrMatName);
  // Now create a new Attribute Matrix that has the correct Tuple Dims.
  AttributeMatrix::Pointer targetAttributeMatrix = m->createNonPrereqAttributeMatrix(this, attrMatName, tDims, AttributeMatrix::Type::Cell);
  // Loop over all of the original cell data arrays and create new ones and insert that into the new Attribute Matrix.
  // DO NOT ALLOCATE the arrays, even during execute as this could potentially be a LARGE memory hog. Wait until
  // execute to allocate the arrays one at a time, do the copy, then deallocate the old array. This will keep the memory
  // consumption to a minimum.
  for(const auto& attrArrayName : selectedCellArrayNames)
  {
    IDataArray::Pointer p = m_SourceAttributeMatrix->getAttributeArray(attrArrayName);
    auto compDims = p->getComponentDimensions();
    IDataArray::Pointer targetArray = p->createNewArray(tDims[0] * tDims[1] * tDims[2], compDims, p->getName(), false);
    targetAttributeMatrix->addOrReplaceAttributeArray(targetArray);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  if(m == nullptr)
  {
    QString ss = QObject::tr("Failed to get DataContainer '%1'").arg(getCellAttributeMatrixPath().getDataContainerName());
    setErrorCondition(-45101, ss);
    return;
  }

  int64_t newNumCellTuples = p_Impl->m_Params.xpNew * p_Impl->m_Params.ypNew * p_Impl->m_Params.zpNew;

  QString attrMatName = getCellAttributeMatrixPath().getAttributeMatrixName();
  AttributeMatrix::Pointer targetAttributeMatrix = m->getAttributeMatrix(attrMatName);

  QList<QString> voxelArrayNames = targetAttributeMatrix->getAttributeArrayNames();

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  std::shared_ptr<tbb::task_group> g(new tbb::task_group);
  // C++11 RIGHT HERE....
  int32_t nthreads = static_cast<int32_t>(std::thread::hardware_concurrency()); // Returns ZERO if not defined on this platform
  int32_t threadCount = 0;
#endif

  for(const auto& attrArrayName : voxelArrayNames)
  {
    // Needed for Threaded Progress Messages
    m_InstanceIndex = ++RotateSampleRefFrameProgress::s_InstanceIndex;
    RotateSampleRefFrameProgress::s_ProgressValues[m_InstanceIndex] = 0;
    RotateSampleRefFrameProgress::s_LastProgressInt[m_InstanceIndex] = 0;
    m_TotalElements = newNumCellTuples;

    //    auto start = std::chrono::steady_clock::now();
    notifyStatusMessage(QString("Rotating DataArray '%1'").arg(attrArrayName));
    IDataArray::Pointer sourceArray = m_SourceAttributeMatrix->getAttributeArray(attrArrayName);
    IDataArray::Pointer targetArray = targetAttributeMatrix->getAttributeArray(attrArrayName);
    // So this little work-around is because if we just try to resize the DataArray<T> will think the sizes are the same
    // and never actually allocate the data. So we just resize to 1 tuple, and then to the real size.
    targetArray->resizeTuples(1);                // Allocate the memory for this data array
    targetArray->resizeTuples(newNumCellTuples); // Allocate the memory for this data array

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    g->run(RotateSampleRefFrameImpl(this, sourceArray, targetArray, p_Impl->m_Params, p_Impl->m_RotationMatrix, m_SliceBySlice));
    threadCount++;
    if(threadCount == nthreads)
    {
      g->wait();
      threadCount = 0;
    }
#else
    RotateSampleRefFrameImpl impl(this, sourceArray, targetArray, p_Impl->m_Params, p_Impl->m_RotationMatrix, m_SliceBySlice);
    impl();
#endif
  }
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  // This will spill over if the number of DataArrays to process does not divide evenly by the number of threads.
  g->wait();
#endif
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::sendThreadSafeProgressMessage(int64_t counter)
{
  std::lock_guard<std::mutex> guard(m_ProgressMessage_Mutex);

  int64_t& progCounter = RotateSampleRefFrameProgress::s_ProgressValues[m_InstanceIndex];
  progCounter += counter;
  int64_t progressInt = static_cast<int64_t>((static_cast<float>(progCounter) / m_TotalElements) * 100.0f);

  int64_t progIncrement = m_TotalElements / 100;
  int64_t prog = 1;

  int64_t& lastProgressInt = RotateSampleRefFrameProgress::s_LastProgressInt[m_InstanceIndex];

  if(progCounter > prog && lastProgressInt != progressInt)
  {
    QString ss = QObject::tr("Transforming || %1% Completed").arg(progressInt);
    notifyStatusMessage(ss);
    prog += progIncrement;
  }

  lastProgressInt = progressInt;
}

void RotateSampleRefFrame::sendThreadSafeProgressMessage(const QString& message)
{
  static std::mutex mutex;
  std::lock_guard<std::mutex> lock(mutex);
  qint64 currentMillis = QDateTime::currentMSecsSinceEpoch();
  if(currentMillis - m_Millis > 1000)
  {
    notifyStatusMessage(message);
    m_Millis = QDateTime::currentMSecsSinceEpoch();
  }
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer RotateSampleRefFrame::newFilterInstance(bool copyFilterParameters) const
{
  RotateSampleRefFrame::Pointer filter = RotateSampleRefFrame::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RotateSampleRefFrame::getUuid() const
{
  return QUuid("{e25d9b4c-2b37-578c-b1de-cf7032b5ef19}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::RotationTransformationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getHumanLabel() const
{
  return "Rotate Sample Reference Frame";
}

// -----------------------------------------------------------------------------
RotateSampleRefFrame::Pointer RotateSampleRefFrame::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<RotateSampleRefFrame> RotateSampleRefFrame::New()
{
  struct make_shared_enabler : public RotateSampleRefFrame
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getNameOfClass() const
{
  return QString("RotateSampleRefFrame");
}

// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::ClassName()
{
  return QString("RotateSampleRefFrame");
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setCellAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RotateSampleRefFrame::getCellAttributeMatrixPath() const
{
  return m_CellAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationAxis(const FloatVec3Type& value)
{
  m_RotationAxis = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type RotateSampleRefFrame::getRotationAxis() const
{
  return m_RotationAxis;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationAngle(float value)
{
  m_RotationAngle = value;
}

// -----------------------------------------------------------------------------
float RotateSampleRefFrame::getRotationAngle() const
{
  return m_RotationAngle;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setSliceBySlice(bool value)
{
  m_SliceBySlice = value;
}

// -----------------------------------------------------------------------------
bool RotateSampleRefFrame::getSliceBySlice() const
{
  return m_SliceBySlice;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationTable(const DynamicTableData& value)
{
  m_RotationTable = value;
}

// -----------------------------------------------------------------------------
DynamicTableData RotateSampleRefFrame::getRotationTable() const
{
  return m_RotationTable;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationRepresentationChoice(int value)
{
  m_RotationRepresentationChoice = value;
}

// -----------------------------------------------------------------------------
int RotateSampleRefFrame::getRotationRepresentationChoice() const
{
  return m_RotationRepresentationChoice;
}

// -----------------------------------------------------------------------------
RotateSampleRefFrame::RotationRepresentation RotateSampleRefFrame::getRotationRepresentation() const
{
  return static_cast<RotationRepresentation>(m_RotationRepresentationChoice);
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationRepresentation(RotationRepresentation value)
{
  m_RotationRepresentationChoice = static_cast<int>(value);
}

// -----------------------------------------------------------------------------
bool RotateSampleRefFrame::isRotationRepresentationValid(int value) const
{
  return (value >= 0) && (value <= 1);
}
