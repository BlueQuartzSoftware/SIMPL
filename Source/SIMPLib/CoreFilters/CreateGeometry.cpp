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

#include "CreateGeometry.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Geometry/HexahedralGeom.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateGeometry::CreateGeometry()
: m_GeometryType(0)
, m_DataContainerName("")
, m_SharedVertexListArrayPath0("", "", "")
, m_SharedVertexListArrayPath1("", "", "")
, m_SharedVertexListArrayPath2("", "", "")
, m_SharedVertexListArrayPath3("", "", "")
, m_SharedVertexListArrayPath4("", "", "")
, m_SharedVertexListArrayPath5("", "", "")
, m_SharedEdgeListArrayPath("", "", "")
, m_SharedTriListArrayPath("", "", "")
, m_SharedQuadListArrayPath("", "", "")
, m_SharedTetListArrayPath("", "", "")
, m_SharedHexListArrayPath("", "", "")
, m_XBoundsArrayPath("", "", "")
, m_YBoundsArrayPath("", "", "")
, m_ZBoundsArrayPath("", "", "")
, m_Dimensions({0, 0, 0})
, m_Origin({0.0f, 0.0f, 0.0f})
, m_Resolution({1.0f, 1.0f, 1.0f})
, m_ImageCellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_RectGridCellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_VertexAttributeMatrixName0(SIMPL::Defaults::VertexAttributeMatrixName)
, m_VertexAttributeMatrixName1(SIMPL::Defaults::VertexAttributeMatrixName)
, m_VertexAttributeMatrixName2(SIMPL::Defaults::VertexAttributeMatrixName)
, m_VertexAttributeMatrixName3(SIMPL::Defaults::VertexAttributeMatrixName)
, m_VertexAttributeMatrixName4(SIMPL::Defaults::VertexAttributeMatrixName)
, m_VertexAttributeMatrixName5(SIMPL::Defaults::VertexAttributeMatrixName)
, m_EdgeAttributeMatrixName(SIMPL::Defaults::EdgeAttributeMatrixName)
, m_FaceAttributeMatrixName0(SIMPL::Defaults::FaceAttributeMatrixName)
, m_FaceAttributeMatrixName1(SIMPL::Defaults::FaceAttributeMatrixName)
, m_TetCellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_HexCellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_TreatWarningsAsErrors(false)
, m_ArrayHandling(false)
, m_NumVerts(0)
{
  m_Dimensions.x = 0;
  m_Dimensions.y = 0;
  m_Dimensions.z = 0;

  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateGeometry::~CreateGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGeometry::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Geometry Type");
    parameter->setPropertyName("GeometryType");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(CreateGeometry, this, GeometryType));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(CreateGeometry, this, GeometryType));
    QVector<QString> choices;
    choices.push_back("Image");
    choices.push_back("Rectilinear Grid");
    choices.push_back("Vertex");
    choices.push_back("Edge");
    choices.push_back("Triangle");
    choices.push_back("Quadrilateral");
    choices.push_back("Tetrahedral");
    choices.push_back("Hexahedral");
    parameter->setChoices(choices);
    QStringList linkedProps = {"Dimensions",
                               "Origin",
                               "Resolution",
                               "BoxDimensions",
                               "ImageCellAttributeMatrixName", // ImageGeom
                               "XBoundsArrayPath",
                               "YBoundsArrayPath",
                               "ZBoundsArrayPath",
                               "RectGridCellAttributeMatrixName", // RectGridGeom
                               "SharedVertexListArrayPath0",
                               "VertexAttributeMatrixName0", // VertexGeom
                               "SharedVertexListArrayPath1",
                               "SharedEdgeListArrayPath",
                               "VertexAttributeMatrixName1",
                               "EdgeAttributeMatrixName", // EdgeGeom
                               "SharedVertexListArrayPath2",
                               "SharedTriListArrayPath",
                               "VertexAttributeMatrixName2",
                               "FaceAttributeMatrixName0", // TriangleGeom
                               "SharedVertexListArrayPath3",
                               "SharedQuadListArrayPath",
                               "VertexAttributeMatrixName3",
                               "FaceAttributeMatrixName1", // QuadGeom
                               "SharedVertexListArrayPath4",
                               "SharedTetListArrayPath",
                               "VertexAttributeMatrixName4",
                               "TetCellAttributeMatrixName", // TetrahedralGeom
                               "SharedVertexListArrayPath5",
                               "SharedHexListArrayPath",
                               "VertexAttributeMatrixName5",
                               "HexCellAttributeMatrixName" }; // HexahedralGeom
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_BOOL_FP("Treat Geometry Warnings as Errors", TreatWarningsAsErrors, FilterParameter::Parameter, CreateGeometry));

  {
    QVector<QString> choices = {"Copy Arrays", "Move Arrays"};
    parameters.push_back(SIMPL_NEW_CHOICE_FP("Array Handling", ArrayHandling, FilterParameter::Parameter, CreateGeometry, choices, false));
  }

  DataContainerSelectionFilterParameter::RequirementType req;
  parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container", DataContainerName, FilterParameter::RequiredArray, CreateGeometry, req));
  {
    parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Dimensions", Dimensions, FilterParameter::Parameter, CreateGeometry, 0));
    parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, CreateGeometry, 0));
    parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Resolution", Resolution, FilterParameter::Parameter, CreateGeometry, 0));
    parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", ImageCellAttributeMatrixName, FilterParameter::CreatedArray, CreateGeometry, 0));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("X Bounds", XBoundsArrayPath, FilterParameter::RequiredArray, CreateGeometry, req, 1));
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Y Bounds", YBoundsArrayPath, FilterParameter::RequiredArray, CreateGeometry, req, 1));
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Z Bounds", ZBoundsArrayPath, FilterParameter::RequiredArray, CreateGeometry, req, 1));
    parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", RectGridCellAttributeMatrixName, FilterParameter::CreatedArray, CreateGeometry, 1));

    PreflightUpdatedValueFilterParameter::Pointer param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Box Size in Length Units", BoxDimensions, FilterParameter::Parameter, CreateGeometry);
    param->setReadOnly(true);
    param->setGroupIndex(0);
    parameters.push_back(param);
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Vertex List", SharedVertexListArrayPath0, FilterParameter::RequiredArray, CreateGeometry, req, 2));
    parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix", VertexAttributeMatrixName0, FilterParameter::CreatedArray, CreateGeometry, 2));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Shared Vertex List", SharedVertexListArrayPath1, FilterParameter::RequiredArray, CreateGeometry, req, 3));
    req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int64, 2, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Edge List", SharedEdgeListArrayPath, FilterParameter::RequiredArray, CreateGeometry, req, 3));
    parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix", VertexAttributeMatrixName1, FilterParameter::CreatedArray, CreateGeometry, 3));
    parameters.push_back(SIMPL_NEW_STRING_FP("Edge Attribute Matrix", EdgeAttributeMatrixName, FilterParameter::CreatedArray, CreateGeometry, 3));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Shared Vertex List", SharedVertexListArrayPath2, FilterParameter::RequiredArray, CreateGeometry, req, 4));
    req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int64, 3, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Triangle List", SharedTriListArrayPath, FilterParameter::RequiredArray, CreateGeometry, req, 4));
    parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix", VertexAttributeMatrixName2, FilterParameter::CreatedArray, CreateGeometry, 4));
    parameters.push_back(SIMPL_NEW_STRING_FP("Face Attribute Matrix", FaceAttributeMatrixName0, FilterParameter::CreatedArray, CreateGeometry, 4));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Shared Vertex List", SharedVertexListArrayPath3, FilterParameter::RequiredArray, CreateGeometry, req, 5));
    req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int64, 4, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quadrilateral List", SharedQuadListArrayPath, FilterParameter::RequiredArray, CreateGeometry, req, 5));
    parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix", VertexAttributeMatrixName3, FilterParameter::CreatedArray, CreateGeometry, 5));
    parameters.push_back(SIMPL_NEW_STRING_FP("Face Attribute Matrix", FaceAttributeMatrixName1, FilterParameter::CreatedArray, CreateGeometry, 5));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Shared Vertex List", SharedVertexListArrayPath4, FilterParameter::RequiredArray, CreateGeometry, req, 6));
    req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int64, 4, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Tetrahedral List", SharedTetListArrayPath, FilterParameter::RequiredArray, CreateGeometry, req, 6));
    parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix", VertexAttributeMatrixName4, FilterParameter::CreatedArray, CreateGeometry, 6));
    parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", TetCellAttributeMatrixName, FilterParameter::CreatedArray, CreateGeometry, 6));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Shared Vertex List", SharedVertexListArrayPath5, FilterParameter::RequiredArray, CreateGeometry, req, 7));
    req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int64, 8, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Hexahedral List", SharedHexListArrayPath, FilterParameter::RequiredArray, CreateGeometry, req, 7));
    parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix", VertexAttributeMatrixName5, FilterParameter::CreatedArray, CreateGeometry, 7));
    parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", HexCellAttributeMatrixName, FilterParameter::CreatedArray, CreateGeometry, 7));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGeometry::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGeometry::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerName());

  if(getErrorCondition() < 0)
  {
    return;
  }

  if(dc->getGeometry())
  {
    QString ss = QObject::tr("Selected Data Container already contains a Geometry");
    setErrorCondition(-701);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  switch(getGeometryType())
  {
  case 0: // ImageGeom
  {
    if(getDimensions().x <= 0 || getDimensions().y <= 0 || getDimensions().z <= 0)
    {
      QString ss = QObject::tr("One of the dimensions has a size less than or equal to zero; all dimensions must be positive\n"
                               "X Dimension: %1\n"
                               "Y Dimension: %2\n"
                               "Z Dimension: %3\n")
                       .arg(getDimensions().x)
                       .arg(getDimensions().y)
                       .arg(getDimensions().z);
      setErrorCondition(-390);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    image->setDimensions(getDimensions().x, getDimensions().y, getDimensions().z);
    image->setOrigin(getOrigin().x, getOrigin().y, getOrigin().z);
    image->setResolution(getResolution().x, getResolution().y, getResolution().z);
    dc->setGeometry(image);

    QVector<size_t> tDims = {image->getXPoints(), image->getYPoints(), image->getZPoints()};
    DataArrayPath path(getDataContainerName(), getImageCellAttributeMatrixName(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Cell);
    break;
  }
  case 1: // RectGridGeom
  {
    QVector<size_t> cDims(1, 1);

    m_XBoundsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getXBoundsArrayPath(), cDims);
    if(m_XBoundsPtr.lock())
    {
      m_XBounds = m_XBoundsPtr.lock()->getPointer(0);
    }
    m_YBoundsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getYBoundsArrayPath(), cDims);
    if(m_YBoundsPtr.lock())
    {
      m_YBounds = m_YBoundsPtr.lock()->getPointer(0);
    }
    m_ZBoundsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getZBoundsArrayPath(), cDims);
    if(m_ZBoundsPtr.lock())
    {
      m_ZBounds = m_ZBoundsPtr.lock()->getPointer(0);
    }

    if(getErrorCondition() < 0)
    {
      return;
    }

    if(m_XBoundsPtr.lock()->getNumberOfTuples() < 2 || m_YBoundsPtr.lock()->getNumberOfTuples() < 2 || m_ZBoundsPtr.lock()->getNumberOfTuples() < 2)
    {
      QString ss = QObject::tr("One of the bounds arrays has a size less than two; all sizes must be at least two\n"
                               "X Bounds Size: %1\n"
                               "Y Bounds Size: %2\n"
                               "Z Bounds Size: %3\n")
                       .arg(m_XBoundsPtr.lock()->getNumberOfTuples())
                       .arg(m_YBoundsPtr.lock()->getNumberOfTuples())
                       .arg(m_ZBoundsPtr.lock()->getNumberOfTuples());
      setErrorCondition(-390);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    RectGridGeom::Pointer rectgrid = RectGridGeom::CreateGeometry(SIMPL::Geometry::RectGridGeometry);
    if(getArrayHandling() == k_CopyArrays)
    {
      rectgrid->setXBounds(std::static_pointer_cast<FloatArrayType>(m_XBoundsPtr.lock()->deepCopy(getInPreflight())));
      rectgrid->setYBounds(std::static_pointer_cast<FloatArrayType>(m_YBoundsPtr.lock()->deepCopy(getInPreflight())));
      rectgrid->setZBounds(std::static_pointer_cast<FloatArrayType>(m_ZBoundsPtr.lock()->deepCopy(getInPreflight())));
    }
    else
    {
      rectgrid->setXBounds(m_XBoundsPtr.lock());
      rectgrid->setYBounds(m_YBoundsPtr.lock());
      rectgrid->setZBounds(m_ZBoundsPtr.lock());

      getDataContainerArray()->getAttributeMatrix(getXBoundsArrayPath())->removeAttributeArray(getXBoundsArrayPath().getDataArrayName());
      getDataContainerArray()->getAttributeMatrix(getYBoundsArrayPath())->removeAttributeArray(getYBoundsArrayPath().getDataArrayName());
      getDataContainerArray()->getAttributeMatrix(getZBoundsArrayPath())->removeAttributeArray(getZBoundsArrayPath().getDataArrayName());
    }
    rectgrid->setDimensions(m_XBoundsPtr.lock()->getNumberOfTuples() - 1, m_YBoundsPtr.lock()->getNumberOfTuples() - 1, m_ZBoundsPtr.lock()->getNumberOfTuples() - 1);
    dc->setGeometry(rectgrid);

    QVector<size_t> tDims = {rectgrid->getXPoints(), rectgrid->getYPoints(), rectgrid->getZPoints()};
    DataArrayPath path(getDataContainerName(), getRectGridCellAttributeMatrixName(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Cell);
    break;
  }
  case 2: // VertexGeom
  {
    QVector<size_t> cDims(1, 3);

    FloatArrayType::Pointer verts = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getSharedVertexListArrayPath0(), cDims);

    if(getErrorCondition() < 0)
    {
      return;
    }

    VertexGeom::Pointer vertex = VertexGeom::NullPointer();
    if(getArrayHandling() == k_CopyArrays)
    {
      vertex = VertexGeom::CreateGeometry(std::static_pointer_cast<FloatArrayType>(verts->deepCopy(getInPreflight())), SIMPL::Geometry::VertexGeometry);
    }
    else
    {
      vertex = VertexGeom::CreateGeometry(verts, SIMPL::Geometry::VertexGeometry);
      getDataContainerArray()->getAttributeMatrix(getSharedVertexListArrayPath0())->removeAttributeArray(getSharedVertexListArrayPath0().getDataArrayName());
    }
    dc->setGeometry(vertex);

    QVector<size_t> tDims(1, vertex->getNumberOfVertices());
    DataArrayPath path(getDataContainerName(), getVertexAttributeMatrixName0(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Vertex);
    break;
  }
  case 3: // EdgeGeom
  {
    QVector<size_t> cDims(1, 3);

    FloatArrayType::Pointer verts = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getSharedVertexListArrayPath1(), cDims);
    cDims[0] = 2;
    m_EdgesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(this, getSharedEdgeListArrayPath(), cDims);
    if(m_EdgesPtr.lock())
    {
      m_Edges = m_EdgesPtr.lock()->getPointer(0);
    }

    if(getErrorCondition() < 0)
    {
      return;
    }

    EdgeGeom::Pointer edge = EdgeGeom::NullPointer();
    if(getArrayHandling() == k_CopyArrays)
    {
      edge = EdgeGeom::CreateGeometry(std::static_pointer_cast<Int64ArrayType>(m_EdgesPtr.lock()->deepCopy(getInPreflight())),
                                      std::static_pointer_cast<FloatArrayType>(verts->deepCopy(getInPreflight())), SIMPL::Geometry::EdgeGeometry);
    }
    else
    {
      edge = EdgeGeom::CreateGeometry(m_EdgesPtr.lock(), verts, SIMPL::Geometry::EdgeGeometry);
      getDataContainerArray()->getAttributeMatrix(getSharedVertexListArrayPath1())->removeAttributeArray(getSharedVertexListArrayPath1().getDataArrayName());
      getDataContainerArray()->getAttributeMatrix(getSharedEdgeListArrayPath())->removeAttributeArray(getSharedEdgeListArrayPath().getDataArrayName());
    }
    dc->setGeometry(edge);

    m_NumVerts = edge->getNumberOfVertices();
    QVector<size_t> tDims(1, edge->getNumberOfVertices());
    DataArrayPath path(getDataContainerName(), getVertexAttributeMatrixName1(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Vertex);
    tDims[0] = edge->getNumberOfEdges();
    path.update(getDataContainerName(), getEdgeAttributeMatrixName(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Edge);
    break;
  }
  case 4: // TriangleGeom
  {
    QVector<size_t> cDims(1, 3);

    FloatArrayType::Pointer verts = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getSharedVertexListArrayPath2(), cDims);
    m_TrisPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(this, getSharedTriListArrayPath(), cDims);
    if(m_TrisPtr.lock())
    {
      m_Tris = m_TrisPtr.lock()->getPointer(0);
    }

    if(getErrorCondition() < 0)
    {
      return;
    }

    TriangleGeom::Pointer triangle = TriangleGeom::NullPointer();
    if(getArrayHandling() == k_CopyArrays)
    {
      triangle = TriangleGeom::CreateGeometry(std::static_pointer_cast<Int64ArrayType>(m_TrisPtr.lock()->deepCopy(getInPreflight())),
                                              std::static_pointer_cast<FloatArrayType>(verts->deepCopy(getInPreflight())), SIMPL::Geometry::TriangleGeometry);
    }
    else
    {
      triangle = TriangleGeom::CreateGeometry(m_TrisPtr.lock(), verts, SIMPL::Geometry::TriangleGeometry);
      getDataContainerArray()->getAttributeMatrix(getSharedVertexListArrayPath2())->removeAttributeArray(getSharedVertexListArrayPath2().getDataArrayName());
      getDataContainerArray()->getAttributeMatrix(getSharedTriListArrayPath())->removeAttributeArray(getSharedTriListArrayPath().getDataArrayName());
    }
    dc->setGeometry(triangle);

    m_NumVerts = triangle->getNumberOfVertices();
    QVector<size_t> tDims(1, triangle->getNumberOfVertices());
    DataArrayPath path(getDataContainerName(), getVertexAttributeMatrixName2(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Vertex);
    tDims[0] = triangle->getNumberOfTris();
    path.update(getDataContainerName(), getFaceAttributeMatrixName0(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Face);
    break;
  }
  case 5: // QuadGeom
  {
    QVector<size_t> cDims(1, 3);

    FloatArrayType::Pointer verts = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getSharedVertexListArrayPath3(), cDims);
    cDims[0] = 4;
    m_QuadsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(this, getSharedQuadListArrayPath(), cDims);
    if(m_QuadsPtr.lock())
    {
      m_Quads = m_QuadsPtr.lock()->getPointer(0);
    }

    if(getErrorCondition() < 0)
    {
      return;
    }

    QuadGeom::Pointer quadrilateral = QuadGeom::NullPointer();
    if(getArrayHandling() == k_CopyArrays)
    {
      quadrilateral = QuadGeom::CreateGeometry(std::static_pointer_cast<Int64ArrayType>(m_QuadsPtr.lock()->deepCopy(getInPreflight())),
                                               std::static_pointer_cast<FloatArrayType>(verts->deepCopy(getInPreflight())), SIMPL::Geometry::QuadGeometry);
    }
    else
    {
      quadrilateral = QuadGeom::CreateGeometry(m_QuadsPtr.lock(), verts, SIMPL::Geometry::QuadGeometry);
      getDataContainerArray()->getAttributeMatrix(getSharedVertexListArrayPath3())->removeAttributeArray(getSharedVertexListArrayPath3().getDataArrayName());
      getDataContainerArray()->getAttributeMatrix(getSharedQuadListArrayPath())->removeAttributeArray(getSharedQuadListArrayPath().getDataArrayName());
    }
    dc->setGeometry(quadrilateral);

    m_NumVerts = quadrilateral->getNumberOfVertices();
    QVector<size_t> tDims(1, quadrilateral->getNumberOfVertices());
    DataArrayPath path(getDataContainerName(), getVertexAttributeMatrixName3(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Vertex);
    tDims[0] = quadrilateral->getNumberOfQuads();
    path.update(getDataContainerName(), getFaceAttributeMatrixName1(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Face);
    break;
  }
  case 6: // TetrahedralGeom
  {
    QVector<size_t> cDims(1, 3);

    FloatArrayType::Pointer verts = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getSharedVertexListArrayPath4(), cDims);
    cDims[0] = 4;
    m_TetsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(this, getSharedTetListArrayPath(), cDims);
    if(m_TetsPtr.lock())
    {
      m_Tets = m_TetsPtr.lock()->getPointer(0);
    }

    if(getErrorCondition() < 0)
    {
      return;
    }

    TetrahedralGeom::Pointer tetrahedral = TetrahedralGeom::NullPointer();
    if(getArrayHandling() == k_CopyArrays)
    {
      tetrahedral = TetrahedralGeom::CreateGeometry(std::static_pointer_cast<Int64ArrayType>(m_TetsPtr.lock()->deepCopy(getInPreflight())),
                                                    std::static_pointer_cast<FloatArrayType>(verts->deepCopy(getInPreflight())), SIMPL::Geometry::TetrahedralGeometry);
    }
    else
    {
      tetrahedral = TetrahedralGeom::CreateGeometry(m_TetsPtr.lock(), verts, SIMPL::Geometry::TetrahedralGeometry);
      getDataContainerArray()->getAttributeMatrix(getSharedVertexListArrayPath4())->removeAttributeArray(getSharedVertexListArrayPath4().getDataArrayName());
      getDataContainerArray()->getAttributeMatrix(getSharedTetListArrayPath())->removeAttributeArray(getSharedTetListArrayPath().getDataArrayName());
    }

    dc->setGeometry(tetrahedral);

    m_NumVerts = tetrahedral->getNumberOfVertices();
    QVector<size_t> tDims(1, tetrahedral->getNumberOfVertices());
    DataArrayPath path(getDataContainerName(), getVertexAttributeMatrixName4(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Vertex);
    tDims[0] = tetrahedral->getNumberOfTets();
    path.update(getDataContainerName(), getTetCellAttributeMatrixName(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Cell);
    break;
  }
  case 7: // HexahedralGeom
  {
    QVector<size_t> cDims(1, 3);

    FloatArrayType::Pointer verts = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getSharedVertexListArrayPath5(), cDims);
    cDims[0] = 8;
    m_HexesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(this, getSharedHexListArrayPath(), cDims);
    if (m_HexesPtr.lock())
    {
      m_Hexes = m_HexesPtr.lock()->getPointer(0);
    }

    if (getErrorCondition() < 0)
    {
      return;
    }


    HexahedralGeom::Pointer hexahedral = HexahedralGeom::NullPointer();
    if (getArrayHandling() == k_CopyArrays)
    {
      hexahedral = HexahedralGeom::CreateGeometry(std::static_pointer_cast<Int64ArrayType>(m_HexesPtr.lock()->deepCopy(getInPreflight())),
        std::static_pointer_cast<FloatArrayType>(verts->deepCopy(getInPreflight())), SIMPL::Geometry::HexahedralGeometry);
    }
    else
    {
      hexahedral = HexahedralGeom::CreateGeometry(m_HexesPtr.lock(), verts, SIMPL::Geometry::HexahedralGeometry);
      getDataContainerArray()->getAttributeMatrix(getSharedVertexListArrayPath5())->removeAttributeArray(getSharedVertexListArrayPath5().getDataArrayName());
      getDataContainerArray()->getAttributeMatrix(getSharedHexListArrayPath())->removeAttributeArray(getSharedHexListArrayPath().getDataArrayName());
    }

    dc->setGeometry(hexahedral);

    m_NumVerts = hexahedral->getNumberOfVertices();
    QVector<size_t> tDims(1, hexahedral->getNumberOfVertices());
    DataArrayPath path(getDataContainerName(), getVertexAttributeMatrixName5(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Vertex);
    tDims[0] = hexahedral->getNumberOfHexas();
    path.update(getDataContainerName(), getHexCellAttributeMatrixName(), "");
    dc->createNonPrereqAttributeMatrix(this, path, tDims, AttributeMatrix::Type::Cell);
    break;
  }
  default:
  {
    QString ss = QObject::tr("Invalid selection for Geometry type");
    setErrorCondition(-701);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    break;
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGeometry::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGeometry::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  switch(m_GeometryType)
  {
  case 0: // ImageGeom
  {
    // Checked during preflight()
    break;
  }
  case 1: // RectGridGeom
  {
    float xval = m_XBounds[0];
    for(size_t i = 1; i < m_XBoundsPtr.lock()->getNumberOfTuples(); i++)
    {
      if(xval > m_XBounds[i])
      {
        QString ss = QObject::tr("Supplied X Bounds array is not strictly increasing; this results in negative resolutions\n"
                                 "Index %1 Value: %2\n"
                                 "Index %3 Value: %4")
                         .arg(i - 1)
                         .arg(xval)
                         .arg(i)
                         .arg(m_XBounds[i]);
        if(m_TreatWarningsAsErrors)
        {
          setErrorCondition(-1);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }
        else
        {
          setWarningCondition(-1);
          notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
        }
        return;
      }
      xval = m_XBounds[i];
    }

    float yval = m_YBounds[0];
    for(size_t i = 1; i < m_YBoundsPtr.lock()->getNumberOfTuples(); i++)
    {
      if(yval > m_YBounds[i])
      {
        QString ss = QObject::tr("Supplied Y Bounds array is not strictly increasing; this results in negative resolutions\n"
                                 "Index %1 Value: %2\n"
                                 "Index %3 Value: %4")
                         .arg(i - 1)
                         .arg(yval)
                         .arg(i)
                         .arg(m_YBounds[i]);
        if(m_TreatWarningsAsErrors)
        {
          setErrorCondition(-1);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }
        else
        {
          setWarningCondition(-1);
          notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
        }
        return;
      }
      yval = m_YBounds[i];
    }

    float zval = m_ZBounds[0];
    for(size_t i = 1; i < m_ZBoundsPtr.lock()->getNumberOfTuples(); i++)
    {
      if(zval > m_ZBounds[i])
      {
        QString ss = QObject::tr("Supplied Z Bounds array is not strictly increasing; this results in negative resolutions\n"
                                 "Index %1 Value: %2\n"
                                 "Index %3 Value: %4")
                         .arg(i - 1)
                         .arg(zval)
                         .arg(i)
                         .arg(m_ZBounds[i]);
        if(m_TreatWarningsAsErrors)
        {
          setErrorCondition(-1);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }
        else
        {
          setWarningCondition(-1);
          notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
        }
        return;
      }
      zval = m_ZBounds[i];
    }

    break;
  }
  case 2: // VertexGeom
  {
    // Checked during preflight()
    break;
  }
  case 3: // EdgeGeom
  {
    int64_t idx = 0;
    for(size_t i = 0; i < m_EdgesPtr.lock()->getSize(); i++)
    {
      if(m_Edges[i] > idx)
      {
        idx = m_Edges[i];
      }
    }

    if((idx + 1) > m_NumVerts)
    {
      QString ss = QObject::tr("Supplied edge list contains a vertex index larger than the total length of the supplied shared vertex list\n"
                               "Index Value: %1\n"
                               "Number of Vertices: %2")
                       .arg(idx)
                       .arg(m_NumVerts);
      if(m_TreatWarningsAsErrors)
      {
        setErrorCondition(-1);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
      else
      {
        setWarningCondition(-1);
        notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
      }
      return;
    }

    break;
  }
  case 4: // TriangleGeom
  {
    int64_t idx = 0;
    for(size_t i = 0; i < m_TrisPtr.lock()->getSize(); i++)
    {
      if(m_Tris[i] > idx)
      {
        idx = m_Tris[i];
      }
    }

    if((idx + 1) > m_NumVerts)
    {
      QString ss = QObject::tr("Supplied triangle list contains a vertex index larger than the total length of the supplied shared vertex list\n"
                               "Index Value: %1\n"
                               "Number of Vertices: %2")
                       .arg(idx)
                       .arg(m_NumVerts);
      if(m_TreatWarningsAsErrors)
      {
        setErrorCondition(-1);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
      else
      {
        setWarningCondition(-1);
        notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
      }
      return;
    }

    break;
  }
  case 5: // QuadGeom
  {
    int64_t idx = 0;
    for(size_t i = 0; i < m_QuadsPtr.lock()->getSize(); i++)
    {
      if(m_Quads[i] > idx)
      {
        idx = m_Quads[i];
      }
    }

    if((idx + 1) > m_NumVerts)
    {
      QString ss = QObject::tr("Supplied quadrilateral list contains a vertex index larger than the total length of the supplied shared vertex list\n"
                               "Index Value: %1\n"
                               "Number of Vertices: %2")
                       .arg(idx)
                       .arg(m_NumVerts);
      if(m_TreatWarningsAsErrors)
      {
        setErrorCondition(-1);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
      else
      {
        setWarningCondition(-1);
        notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
      }
      return;
    }

    break;
  }
  case 6: // TetrahedralGeom
  {
    int64_t idx = 0;
    for(size_t i = 0; i < m_TetsPtr.lock()->getSize(); i++)
    {
      if(m_Tets[i] > idx)
      {
        idx = m_Tets[i];
      }
    }

    if((idx + 1) > m_NumVerts)
    {
      QString ss = QObject::tr("Supplied tetrahedra list contains a vertex index larger than the total length of the supplied shared vertex list\n"
                               "Index Value: %1\n"
                               "Number of Vertices: %2")
                       .arg(idx)
                       .arg(m_NumVerts);
      if(m_TreatWarningsAsErrors)
      {
        setErrorCondition(-1);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
      else
      {
        setWarningCondition(-1);
        notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
      }
      return;
    }

    break;
  }
  case 7: // HexahedralGeom
  {
    int64_t idx = 0;
    for (size_t i = 0; i < m_HexesPtr.lock()->getSize(); i++)
    {
      if (m_Hexes[i] > idx)
      {
        idx = m_Hexes[i];
      }
    }

    if ((idx + 1) > m_NumVerts)
    {
      QString ss = QObject::tr("Supplied hexahedra list contains a vertex index larger than the total length of the supplied shared vertex list\n"
        "Index Value: %1\n"
        "Number of Vertices: %2")
        .arg(idx)
        .arg(m_NumVerts);
      if (m_TreatWarningsAsErrors)
      {
        setErrorCondition(-1);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
      else
      {
        setWarningCondition(-1);
        notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
      }
      return;
    }

    break;
  }
  default:
  {
    break;
  }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateGeometry::getBoxDimensions()
{
  QString desc;
  QTextStream ss(&desc);
  float halfRes[3] = {m_Resolution.x / 2.0f, m_Resolution.y / 2.0f, m_Resolution.z / 2.0f};
  ss << "Extents:\n"
     << "X Extent: 0 to " << m_Dimensions.x - 1 << " (dimension: " << m_Dimensions.x << ")\n"
     << "Y Extent: 0 to " << m_Dimensions.y - 1 << " (dimension: " << m_Dimensions.y << ")\n"
     << "Z Extent: 0 to " << m_Dimensions.z - 1 << " (dimension: " << m_Dimensions.z << ")\n"
     << "Bounds:\n"
     << "X Range: " << (m_Origin.x - halfRes[0]) << " to " << (m_Origin.x - halfRes[0] + m_Dimensions.x * m_Resolution.x) << " (delta: " << (m_Dimensions.x * m_Resolution.x) << ")\n"
     << "Y Range: " << (m_Origin.y - halfRes[1]) << " to " << (m_Origin.y - halfRes[1] + m_Dimensions.y * m_Resolution.y) << " (delta: " << (m_Dimensions.y * m_Resolution.y) << ")\n"
     << "Z Range: " << (m_Origin.z - halfRes[2]) << " to " << (m_Origin.z - halfRes[2] + m_Dimensions.z * m_Resolution.z) << " (delta: " << (m_Dimensions.z * m_Resolution.z) << ")\n";
  return desc;
  return desc;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateGeometry::newFilterInstance(bool copyFilterParameters) const
{
  CreateGeometry::Pointer filter = CreateGeometry::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateGeometry::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateGeometry::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CreateGeometry::getUuid()
{
  return QUuid("{9ac220b9-14f9-581a-9bac-5714467589cc}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GeometryFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateGeometry::getHumanLabel() const
{
  return "Create Geometry";
}
