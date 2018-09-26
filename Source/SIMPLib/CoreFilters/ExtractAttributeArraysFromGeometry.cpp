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

#include "ExtractAttributeArraysFromGeometry.h"

#include <cassert>
#include <cstring>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedDataContainerSelectionFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/HexahedralGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractAttributeArraysFromGeometry::ExtractAttributeArraysFromGeometry()
: m_DataContainerName("")
, m_SharedVertexListArrayPath0(SIMPL::Defaults::VertexDataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, "VertexCoordinates")
, m_SharedVertexListArrayPath1(SIMPL::Defaults::EdgeDataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, "VertexCoordinates")
, m_SharedVertexListArrayPath2(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, "VertexCoordinates")
, m_SharedVertexListArrayPath3(SIMPL::Defaults::QuadDataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, "VertexCoordinates")
, m_SharedVertexListArrayPath4(SIMPL::Defaults::TetrahedralDataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, "VertexCoordinates")
, m_SharedVertexListArrayPath5(SIMPL::Defaults::HexahedralDataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, "VertexCoordinates")
, m_SharedEdgeListArrayPath(SIMPL::Defaults::EdgeDataContainerName, SIMPL::Defaults::EdgeAttributeMatrixName, "EdgeConnectivity")
, m_SharedTriListArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, "TriangleConnectivity")
, m_SharedQuadListArrayPath(SIMPL::Defaults::QuadDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, "QuadConnectivity")
, m_SharedTetListArrayPath(SIMPL::Defaults::TetrahedralDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "TetConnectivity")
, m_SharedHexListArrayPath(SIMPL::Defaults::HexahedralDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "HexConnectivity")
, m_XBoundsArrayPath("", "", "XBounds")
, m_YBoundsArrayPath("", "", "YBounds")
, m_ZBoundsArrayPath("", "", "ZBounds")
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractAttributeArraysFromGeometry::~ExtractAttributeArraysFromGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractAttributeArraysFromGeometry::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractAttributeArraysFromGeometry::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    LinkedDataContainerSelectionFilterParameter::Pointer parameter = LinkedDataContainerSelectionFilterParameter::New();
    parameter->setHumanLabel("Geometry");
    parameter->setPropertyName("DataContainerName");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(ExtractAttributeArraysFromGeometry, this, DataContainerName));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(ExtractAttributeArraysFromGeometry, this, DataContainerName));
    QStringList linkedProps = {
        "XBoundsArrayPath",           "YBoundsArrayPath",        "ZBoundsArrayPath", // RectGridGeom
        "SharedVertexListArrayPath0",                                                // VertexGeom
        "SharedVertexListArrayPath1", "SharedEdgeListArrayPath",                     // EdgeGeom
        "SharedVertexListArrayPath2", "SharedTriListArrayPath",                      // TriangleGeom
        "SharedVertexListArrayPath3", "SharedQuadListArrayPath",                     // QuadGeom
        "SharedVertexListArrayPath4", "SharedTetListArrayPath",                      // TetrahedralGeom
        "SharedVertexListArrayPath5", "SharedHexListArrayPath"                       // HexahedralGeom
    }; 
    parameter->setLinkedProperties(linkedProps);
    parameter->setCategory(FilterParameter::Parameter);
    IGeometry::Types geomTypes = {IGeometry::Type::RectGrid, IGeometry::Type::Vertex, IGeometry::Type::Edge, IGeometry::Type::Triangle, IGeometry::Type::Quad, IGeometry::Type::Tetrahedral, IGeometry::Type::Hexahedral};
    parameter->setDefaultGeometryTypes(geomTypes);
    parameters.push_back(parameter);
  }
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Any, IGeometry::Type::RectGrid);
    parameters.push_back(
        SIMPL_NEW_DA_CREATION_FP("X Bounds", XBoundsArrayPath, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req, static_cast<int32_t>(IGeometry::Type::RectGrid)));
    parameters.push_back(
        SIMPL_NEW_DA_CREATION_FP("Y Bounds", YBoundsArrayPath, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req, static_cast<int32_t>(IGeometry::Type::RectGrid)));
    parameters.push_back(
        SIMPL_NEW_DA_CREATION_FP("Z Bounds", ZBoundsArrayPath, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req, static_cast<int32_t>(IGeometry::Type::RectGrid)));
  }
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Vertex, IGeometry::Type::Vertex);
    parameters.push_back(
        SIMPL_NEW_DA_CREATION_FP("Vertex List", SharedVertexListArrayPath0, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req, static_cast<int32_t>(IGeometry::Type::Vertex)));
  }
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Vertex, IGeometry::Type::Edge);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Shared Vertex List", SharedVertexListArrayPath1, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req,
                                                  static_cast<int32_t>(IGeometry::Type::Edge)));
    req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Edge, IGeometry::Type::Edge);
    parameters.push_back(
        SIMPL_NEW_DA_CREATION_FP("Edge List", SharedEdgeListArrayPath, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req, static_cast<int32_t>(IGeometry::Type::Edge)));
  }
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Vertex, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Shared Vertex List", SharedVertexListArrayPath2, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req,
                                                  static_cast<int32_t>(IGeometry::Type::Triangle)));
    req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(
        SIMPL_NEW_DA_CREATION_FP("Triangle List", SharedTriListArrayPath, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req, static_cast<int32_t>(IGeometry::Type::Triangle)));
  }
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Vertex, IGeometry::Type::Quad);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Shared Vertex List", SharedVertexListArrayPath3, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req,
                                                  static_cast<int32_t>(IGeometry::Type::Quad)));
    req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Face, IGeometry::Type::Quad);
    parameters.push_back(
        SIMPL_NEW_DA_CREATION_FP("Quadrilateral List", SharedQuadListArrayPath, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req, static_cast<int32_t>(IGeometry::Type::Quad)));
  }
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Vertex, IGeometry::Type::Tetrahedral);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Shared Vertex List", SharedVertexListArrayPath4, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req,
                                                  static_cast<int32_t>(IGeometry::Type::Tetrahedral)));
    req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Tetrahedral);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Tetrahedral List", SharedTetListArrayPath, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req,
                                                  static_cast<int32_t>(IGeometry::Type::Tetrahedral)));
  }
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Vertex, IGeometry::Type::Hexahedral);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Shared Vertex List", SharedVertexListArrayPath5, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req,
      static_cast<int32_t>(IGeometry::Type::Hexahedral)));
    req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Hexahedral);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Hexahedral List", SharedHexListArrayPath, FilterParameter::RequiredArray, ExtractAttributeArraysFromGeometry, req,
      static_cast<int32_t>(IGeometry::Type::Hexahedral)));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractAttributeArraysFromGeometry::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setSharedVertexListArrayPath0(reader->readDataArrayPath("SharedVertexListArrayPath0", getSharedVertexListArrayPath0()));
  setSharedVertexListArrayPath1(reader->readDataArrayPath("SharedVertexListArrayPath1", getSharedVertexListArrayPath1()));
  setSharedVertexListArrayPath2(reader->readDataArrayPath("SharedVertexListArrayPath2", getSharedVertexListArrayPath2()));
  setSharedVertexListArrayPath3(reader->readDataArrayPath("SharedVertexListArrayPath3", getSharedVertexListArrayPath3()));
  setSharedVertexListArrayPath4(reader->readDataArrayPath("SharedVertexListArrayPath4", getSharedVertexListArrayPath4()));
  setSharedVertexListArrayPath5(reader->readDataArrayPath("SharedVertexListArrayPath5", getSharedVertexListArrayPath5()));
  setSharedEdgeListArrayPath(reader->readDataArrayPath("SharedEdgeListArrayPath", getSharedEdgeListArrayPath()));
  setSharedTriListArrayPath(reader->readDataArrayPath("SharedTriListArrayPath", getSharedTriListArrayPath()));
  setSharedQuadListArrayPath(reader->readDataArrayPath("SharedQuadListArrayPath", getSharedQuadListArrayPath()));
  setSharedTetListArrayPath(reader->readDataArrayPath("SharedTetListArrayPath", getSharedTetListArrayPath()));
  setSharedHexListArrayPath(reader->readDataArrayPath("SharedHexListArrayPath", getSharedHexListArrayPath()));
  setXBoundsArrayPath(reader->readDataArrayPath("XBoundsArrayPath", getXBoundsArrayPath()));
  setYBoundsArrayPath(reader->readDataArrayPath("YBoundsArrayPath", getYBoundsArrayPath()));
  setZBoundsArrayPath(reader->readDataArrayPath("ZBoundsArrayPath", getZBoundsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractAttributeArraysFromGeometry::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();

  IGeometry::Pointer igeom = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry, AbstractFilter>(this, getDataContainerName());

  if(getErrorCondition() < 0)
  {
    return;
  }

  IGeometry::Type geomType = igeom->getGeometryType();

  switch(geomType)
  {
  case IGeometry::Type::RectGrid: // RectGridGeom
  {
    RectGridGeom::Pointer rectgrid = std::static_pointer_cast<RectGridGeom>(igeom);

    QVector<IDataArray::Pointer> xarrays;
    QVector<IDataArray::Pointer> yarrays;
    QVector<IDataArray::Pointer> zarrays;
    xarrays.push_back(rectgrid->getXBounds());
    yarrays.push_back(rectgrid->getYBounds());
    zarrays.push_back(rectgrid->getZBounds());

    QVector<size_t> cDims(1, 1);

    m_XBoundsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getXBoundsArrayPath(), 0, cDims);
    if(m_XBoundsPtr.lock())
    {
      m_XBounds = m_XBoundsPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      xarrays.push_back(m_XBoundsPtr.lock());
    }

    m_YBoundsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getYBoundsArrayPath(), 0, cDims);
    if(m_YBoundsPtr.lock())
    {
      m_YBounds = m_YBoundsPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      yarrays.push_back(m_YBoundsPtr.lock());
    }

    m_ZBoundsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getZBoundsArrayPath(), 0, cDims);
    if(m_ZBoundsPtr.lock())
    {
      m_ZBounds = m_ZBoundsPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      zarrays.push_back(m_ZBoundsPtr.lock());
    }

    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, xarrays);
    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, yarrays);
    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, zarrays);

    break;
  }
  case IGeometry::Type::Vertex: // VertexGeom
  {
    VertexGeom::Pointer vertex = std::static_pointer_cast<VertexGeom>(igeom);

    QVector<IDataArray::Pointer> arrays;
    arrays.push_back(vertex->getVertices());

    QVector<size_t> cDims(1, 3);

    m_VertsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getSharedVertexListArrayPath0(), 0, cDims);
    if(m_VertsPtr.lock())
    {
      m_Verts = m_VertsPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      arrays.push_back(m_VertsPtr.lock());
    }

    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, arrays);

    break;
  }
  case IGeometry::Type::Edge: // EdgeGeom
  {
    EdgeGeom::Pointer edge = std::static_pointer_cast<EdgeGeom>(igeom);

    QVector<IDataArray::Pointer> varrays;
    QVector<IDataArray::Pointer> earrays;
    varrays.push_back(edge->getVertices());
    earrays.push_back(edge->getEdges());

    QVector<size_t> cDims(1, 3);

    m_VertsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getSharedVertexListArrayPath1(), 0, cDims);
    if(m_VertsPtr.lock())
    {
      m_Verts = m_VertsPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      varrays.push_back(m_VertsPtr.lock());
    }

    cDims[0] = 2;

    m_EdgesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(this, getSharedEdgeListArrayPath(), 0, cDims);
    if(m_EdgesPtr.lock())
    {
      m_Edges = m_EdgesPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      earrays.push_back(m_EdgesPtr.lock());
    }

    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, varrays);
    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, earrays);

    break;
  }
  case IGeometry::Type::Triangle: // TriangleGeom
  {
    TriangleGeom::Pointer triangle = std::static_pointer_cast<TriangleGeom>(igeom);

    QVector<IDataArray::Pointer> varrays;
    QVector<IDataArray::Pointer> earrays;
    varrays.push_back(triangle->getVertices());
    earrays.push_back(triangle->getTriangles());

    QVector<size_t> cDims(1, 3);

    m_VertsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getSharedVertexListArrayPath2(), 0, cDims);
    if(m_VertsPtr.lock())
    {
      m_Verts = m_VertsPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      varrays.push_back(m_VertsPtr.lock());
    }

    m_TrisPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(this, getSharedTriListArrayPath(), 0, cDims);
    if(m_TrisPtr.lock())
    {
      m_Tris = m_TrisPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      earrays.push_back(m_TrisPtr.lock());
    }

    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, varrays);
    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, earrays);

    break;
  }
  case IGeometry::Type::Quad: // QuadGeom
  {
    QuadGeom::Pointer quad = std::static_pointer_cast<QuadGeom>(igeom);

    QVector<IDataArray::Pointer> varrays;
    QVector<IDataArray::Pointer> earrays;
    varrays.push_back(quad->getVertices());
    earrays.push_back(quad->getQuads());

    QVector<size_t> cDims(1, 3);

    m_VertsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getSharedVertexListArrayPath3(), 0, cDims);
    if(m_VertsPtr.lock())
    {
      m_Verts = m_VertsPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      varrays.push_back(m_VertsPtr.lock());
    }

    cDims[0] = 4;

    m_QuadsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(this, getSharedQuadListArrayPath(), 0, cDims);
    if(m_QuadsPtr.lock())
    {
      m_Quads = m_QuadsPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      earrays.push_back(m_QuadsPtr.lock());
    }

    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, varrays);
    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, earrays);

    break;
  }
  case IGeometry::Type::Tetrahedral: // TetrahedralGeom
  {
    TetrahedralGeom::Pointer tet = std::static_pointer_cast<TetrahedralGeom>(igeom);

    QVector<IDataArray::Pointer> varrays;
    QVector<IDataArray::Pointer> earrays;
    varrays.push_back(tet->getVertices());
    earrays.push_back(tet->getTetrahedra());

    QVector<size_t> cDims(1, 3);

    m_VertsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getSharedVertexListArrayPath4(), 0, cDims);
    if(m_VertsPtr.lock())
    {
      m_Verts = m_VertsPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      varrays.push_back(m_VertsPtr.lock());
    }

    cDims[0] = 4;

    m_TetsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(this, getSharedTetListArrayPath(), 0, cDims);
    if(m_TetsPtr.lock())
    {
      m_Tets = m_TetsPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      earrays.push_back(m_TetsPtr.lock());
    }

    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, varrays);
    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, earrays);

    break;
  }
  case IGeometry::Type::Hexahedral: // HexahedralGeom
  {
    HexahedralGeom::Pointer hex = std::static_pointer_cast<HexahedralGeom>(igeom);

    QVector<IDataArray::Pointer> varrays;
    QVector<IDataArray::Pointer> earrays;
    varrays.push_back(hex->getVertices());
    earrays.push_back(hex->getHexahedra());

    QVector<size_t> cDims(1, 3);

    m_VertsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getSharedVertexListArrayPath5(), 0, cDims);
    if (m_VertsPtr.lock())
    {
      m_Verts = m_VertsPtr.lock()->getPointer(0);
    }
    if (getErrorCondition() >= 0)
    {
      varrays.push_back(m_VertsPtr.lock());
    }

    cDims[0] = 8;

    m_TetsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(this, getSharedHexListArrayPath(), 0, cDims);
    if (m_TetsPtr.lock())
    {
      m_Tets = m_TetsPtr.lock()->getPointer(0);
    }
    if (getErrorCondition() >= 0)
    {
      earrays.push_back(m_TetsPtr.lock());
    }

    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, varrays);
    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, earrays);

    break;
  }
  default:
  {
    QString ss = QObject::tr("Selected Data Container (%1) does not contain a valid geometry\n"
                             "Geometry Type: %2\n"
                             "Valid Geometry Types: Rectilinear Grid, Vertex, Edge, Triangle, Quadrilateral, Tetrahedral")
                     .arg(getDataContainerName())
                     .arg(igeom->getGeometryTypeAsString());
    setErrorCondition(-701);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    break;
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractAttributeArraysFromGeometry::preflight()
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
void ExtractAttributeArraysFromGeometry::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  IGeometry::Pointer igeom = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry, AbstractFilter>(this, getDataContainerName());
  IGeometry::Type geomType = igeom->getGeometryType();

  switch(geomType)
  {
  case IGeometry::Type::RectGrid: // RectGridGeom
  {
    RectGridGeom::Pointer rectgrid = std::static_pointer_cast<RectGridGeom>(igeom);

    FloatArrayType::Pointer xbounds = rectgrid->getXBounds();
    FloatArrayType::Pointer ybounds = rectgrid->getYBounds();
    FloatArrayType::Pointer zbounds = rectgrid->getZBounds();

    assert(xbounds->getSize() == m_XBoundsPtr.lock()->getSize());
    assert(ybounds->getSize() == m_YBoundsPtr.lock()->getSize());
    assert(zbounds->getSize() == m_ZBoundsPtr.lock()->getSize());

    std::memcpy(m_XBounds, xbounds->getPointer(0), xbounds->getSize() * sizeof(float));
    std::memcpy(m_YBounds, ybounds->getPointer(0), ybounds->getSize() * sizeof(float));
    std::memcpy(m_ZBounds, zbounds->getPointer(0), zbounds->getSize() * sizeof(float));

    break;
  }
  case IGeometry::Type::Vertex: // VertexGeom
  {
    VertexGeom::Pointer vertex = std::static_pointer_cast<VertexGeom>(igeom);

    SharedVertexList::Pointer verts = vertex->getVertices();

    assert(verts->getSize() == m_VertsPtr.lock()->getSize());

    std::memcpy(m_Verts, verts->getPointer(0), verts->getSize() * sizeof(float));

    break;
  }
  case IGeometry::Type::Edge: // EdgeGeom
  {
    EdgeGeom::Pointer edge = std::static_pointer_cast<EdgeGeom>(igeom);

    SharedVertexList::Pointer verts = edge->getVertices();
    SharedEdgeList::Pointer edges = edge->getEdges();

    assert(verts->getSize() == m_VertsPtr.lock()->getSize());
    assert(edges->getSize() == m_EdgesPtr.lock()->getSize());

    std::memcpy(m_Verts, verts->getPointer(0), verts->getSize() * sizeof(float));
    std::memcpy(m_Edges, edges->getPointer(0), edges->getSize() * sizeof(int64_t));

    break;
  }
  case IGeometry::Type::Triangle: // TriangleGeom
  {
    TriangleGeom::Pointer triangle = std::static_pointer_cast<TriangleGeom>(igeom);

    SharedVertexList::Pointer verts = triangle->getVertices();
    SharedTriList::Pointer tris = triangle->getTriangles();

    assert(verts->getSize() == m_VertsPtr.lock()->getSize());
    assert(tris->getSize() == m_TrisPtr.lock()->getSize());

    std::memcpy(m_Verts, verts->getPointer(0), verts->getSize() * sizeof(float));
    std::memcpy(m_Tris, tris->getPointer(0), tris->getSize() * sizeof(int64_t));

    break;
  }
  case IGeometry::Type::Quad: // QuadGeom
  {
    QuadGeom::Pointer quad = std::static_pointer_cast<QuadGeom>(igeom);

    SharedVertexList::Pointer verts = quad->getVertices();
    SharedQuadList::Pointer quads = quad->getQuads();

    assert(verts->getSize() == m_VertsPtr.lock()->getSize());
    assert(quads->getSize() == m_QuadsPtr.lock()->getSize());

    std::memcpy(m_Verts, verts->getPointer(0), verts->getSize() * sizeof(float));
    std::memcpy(m_Quads, quads->getPointer(0), quads->getSize() * sizeof(int64_t));

    break;
  }
  case IGeometry::Type::Tetrahedral: // TetrahedralGeom
  {
    TetrahedralGeom::Pointer tet = std::static_pointer_cast<TetrahedralGeom>(igeom);

    SharedVertexList::Pointer verts = tet->getVertices();
    SharedTetList::Pointer tets = tet->getTetrahedra();

    assert(verts->getSize() == m_VertsPtr.lock()->getSize());
    assert(tets->getSize() == m_TetsPtr.lock()->getSize());

    std::memcpy(m_Verts, verts->getPointer(0), verts->getSize() * sizeof(float));
    std::memcpy(m_Tets, tets->getPointer(0), tets->getSize() * sizeof(int64_t));

    break;
  }
  case IGeometry::Type::Hexahedral: // HexahedralGeom
  {
    HexahedralGeom::Pointer hex = std::static_pointer_cast<HexahedralGeom>(igeom);

    SharedVertexList::Pointer verts = hex->getVertices();
    SharedTetList::Pointer tets = hex->getHexahedra();

    assert(verts->getSize() == m_VertsPtr.lock()->getSize());
    assert(tets->getSize() == m_TetsPtr.lock()->getSize());

    std::memcpy(m_Verts, verts->getPointer(0), verts->getSize() * sizeof(float));
    std::memcpy(m_Tets, tets->getPointer(0), tets->getSize() * sizeof(int64_t));

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
AbstractFilter::Pointer ExtractAttributeArraysFromGeometry::newFilterInstance(bool copyFilterParameters) const
{
  ExtractAttributeArraysFromGeometry::Pointer filter = ExtractAttributeArraysFromGeometry::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractAttributeArraysFromGeometry::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractAttributeArraysFromGeometry::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractAttributeArraysFromGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractAttributeArraysFromGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ExtractAttributeArraysFromGeometry::getUuid()
{
  return QUuid("{2060a933-b6f5-50fd-9382-a008a5cef17f}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractAttributeArraysFromGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GeometryFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractAttributeArraysFromGeometry::getHumanLabel() const
{
  return "Extract Attribute Arrays from Geometry";
}
