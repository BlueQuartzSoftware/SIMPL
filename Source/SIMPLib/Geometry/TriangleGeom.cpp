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

/* ============================================================================
 * TriangleGeom re-implements code from the following vtk modules:
 *
 * * vtkTriangle.cxx
 *   - re-implemented vtkTriangle::GetParametricCenter to TriangleGeom::getParametricCenter
 *   - re-implemented vtkTriangle::InterpolationDerivs to TriangleGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - re-implemented vtkGradientFilter template function ComputeCellGradientsUG to
 *     TriangleGeom::findDerivatives
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QTextStream>

#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/DerivativeHelpers.h"
#include "SIMPLib/Geometry/GeometryHelpers.h"
#include "SIMPLib/Utilities/ParallelDataAlgorithm.h"

/**
 * @brief The FindTriangleDerivativesImpl class implements a threaded algorithm that computes the
 * derivative of an arbitrary dimensional field on the underlying triangles
 */
class FindTriangleDerivativesImpl
{
public:
  FindTriangleDerivativesImpl(TriangleGeom* tris, const DoubleArrayType::Pointer& field, const DoubleArrayType::Pointer& derivs)
  : m_Tris(tris)
  , m_Field(field)
  , m_Derivatives(derivs)
  {
  }
  virtual ~FindTriangleDerivativesImpl() = default;

  void compute(size_t start, size_t end) const
  {
    int32_t cDims = m_Field->getNumberOfComponents();
    double* fieldPtr = m_Field->getPointer(0);
    double* derivsPtr = m_Derivatives->getPointer(0);
    double values[3] = {0.0, 0.0, 0.0};
    double derivs[3] = {0.0, 0.0, 0.0};
    size_t verts[3]{0, 0, 0};

    size_t counter = 0;
    size_t totalElements = m_Tris->getNumberOfTris();
    size_t progIncrement = static_cast<size_t>(totalElements / 100);

    for(size_t i = start; i < end; i++)
    {
      m_Tris->getVertsAtTri(i, verts);
      for(size_t j = 0; j < cDims; j++)
      {
        for(size_t k = 0; k < 3; k++)
        {
          values[k] = fieldPtr[cDims * verts[k] + j];
        }
        DerivativeHelpers::TriangleDeriv()(m_Tris, i, values, derivs);
        derivsPtr[i * 3 * cDims + j * 3] = derivs[0];
        derivsPtr[i * 3 * cDims + j * 3 + 1] = derivs[1];
        derivsPtr[i * 3 * cDims + j * 3 + 2] = derivs[2];
      }

      if(counter > progIncrement)
      {
        m_Tris->sendThreadSafeProgressMessage(counter, totalElements);
        counter = 0;
      }
      counter++;
    }
  }

  void operator()(const SIMPLRange& range) const
  {
    compute(range.min(), range.max());
  }

private:
  TriangleGeom* m_Tris;
  DoubleArrayType::Pointer m_Field;
  DoubleArrayType::Pointer m_Derivatives;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::TriangleGeom()
{
  m_GeometryTypeName = SIMPL::Geometry::TriangleGeometry;
  m_GeometryType = IGeometry::Type::Triangle;
  m_XdmfGridType = SIMPL::XdmfGridType::PolyData;
  m_UnitDimensionality = 2;
  m_SpatialDimensionality = 3;
  m_VertexList = TriangleGeom::CreateSharedVertexList(0);
  m_TriList = TriangleGeom::CreateSharedTriList(0);
  m_EdgeList = SharedEdgeList::NullPointer();
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
  m_TrianglesContainingVert = ElementDynamicList::NullPointer();
  m_TriangleNeighbors = ElementDynamicList::NullPointer();
  m_TriangleCentroids = FloatArrayType::NullPointer();
  m_TriangleSizes = FloatArrayType::NullPointer();
  m_ProgressCounter = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::~TriangleGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::Pointer TriangleGeom::CreateGeometry(size_t numTriangles, const SharedVertexList::Pointer& vertices, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  SharedTriList::Pointer triangles = TriangleGeom::CreateSharedTriList(numTriangles, allocate);
  TriangleGeom* d = new TriangleGeom();
  d->setVertices(vertices);
  d->setTriangles(triangles);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::Pointer TriangleGeom::CreateGeometry(const SharedTriList::Pointer& triangles, const SharedVertexList::Pointer& vertices, const QString& name)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  if(vertices.get() == nullptr)
  {
    return TriangleGeom::NullPointer();
  }
  if(triangles.get() == nullptr)
  {
    return TriangleGeom::NullPointer();
  }
  TriangleGeom* d = new TriangleGeom();
  d->setVertices(vertices);
  d->setTriangles(triangles);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
  m_TriList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::addOrReplaceAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if(data->getType() != AttributeMatrix::Type::Vertex && data->getType() != AttributeMatrix::Type::Edge && data->getType() != AttributeMatrix::Type::Face)
  {
    // TriangleGeom can only accept vertex, edge, or face Attribute Matrices
    return;
  }
  if(data->getType() == AttributeMatrix::Type::Vertex && static_cast<size_t>(data->getNumberOfTuples()) != getNumberOfVertices())
  {
    return;
  }
  if(data->getType() == AttributeMatrix::Type::Edge && static_cast<size_t>(data->getNumberOfTuples()) != getNumberOfEdges())
  {
    return;
  }
  if(data->getType() == AttributeMatrix::Type::Face && data->getNumberOfTuples() != getNumberOfElements())
  {
    return;
  }
  // if(data->getName().compare(name) != 0)
  //{
  //  data->setName(name);
  //}
  m_AttributeMatrices[name] = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t TriangleGeom::getNumberOfElements() const
{
  return m_TriList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::findEdges()
{
  m_EdgeList = CreateSharedEdgeList(0);
  GeometryHelpers::Connectivity::Find2DElementEdges<size_t>(m_TriList, m_EdgeList);
  if(m_EdgeList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteEdges()
{
  m_EdgeList = SharedEdgeList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::findElementsContainingVert()
{
  m_TrianglesContainingVert = ElementDynamicList::New();
  GeometryHelpers::Connectivity::FindElementsContainingVert<uint16_t, size_t>(m_TriList, m_TrianglesContainingVert, getNumberOfVertices());
  if(m_TrianglesContainingVert.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer TriangleGeom::getElementsContainingVert() const
{
  return m_TrianglesContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert)
{
  m_TrianglesContainingVert = elementsContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteElementsContainingVert()
{
  m_TrianglesContainingVert = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::findElementNeighbors()
{
  int err = 0;
  if(m_TrianglesContainingVert.get() == nullptr)
  {
    err = findElementsContainingVert();
    if(err < 0)
    {
      return err;
    }
  }
  m_TriangleNeighbors = ElementDynamicList::New();
  err = GeometryHelpers::Connectivity::FindElementNeighbors<uint16_t, size_t>(m_TriList, m_TrianglesContainingVert, m_TriangleNeighbors, IGeometry::Type::Triangle);
  if(m_TriangleNeighbors.get() == nullptr)
  {
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer TriangleGeom::getElementNeighbors() const
{
  return m_TriangleNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::setElementNeighbors(ElementDynamicList::Pointer elementNeighbors)
{
  m_TriangleNeighbors = elementNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteElementNeighbors()
{
  m_TriangleNeighbors = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::findElementCentroids()
{
  std::vector<size_t> cDims(1, 3);
  m_TriangleCentroids = FloatArrayType::CreateArray(getNumberOfTris(), cDims, SIMPL::StringConstants::TriangleCentroids, true);
  GeometryHelpers::Topology::FindElementCentroids<size_t>(m_TriList, m_VertexList, m_TriangleCentroids);
  if(m_TriangleCentroids.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer TriangleGeom::getElementCentroids() const
{
  return m_TriangleCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::setElementCentroids(FloatArrayType::Pointer elementCentroids)
{
  m_TriangleCentroids = elementCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteElementCentroids()
{
  m_TriangleCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::findElementSizes()
{
  std::vector<size_t> cDims(1, 1);
  m_TriangleSizes = FloatArrayType::CreateArray(getNumberOfTris(), cDims, SIMPL::StringConstants::TriangleAreas, true);
  GeometryHelpers::Topology::Find2DElementAreas<size_t>(m_TriList, m_VertexList, m_TriangleSizes);
  if(m_TriangleSizes.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer TriangleGeom::getElementSizes() const
{
  return m_TriangleSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::setElementSizes(FloatArrayType::Pointer elementSizes)
{
  m_TriangleSizes = elementSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteElementSizes()
{
  m_TriangleSizes = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::findUnsharedEdges()
{
  std::vector<size_t> cDims(1, 2);
  m_UnsharedEdgeList = SharedEdgeList::CreateArray(0, cDims, SIMPL::Geometry::UnsharedEdgeList, true);
  GeometryHelpers::Connectivity::Find2DUnsharedEdges<size_t>(m_TriList, m_UnsharedEdgeList);
  if(m_UnsharedEdgeList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedEdgeList::Pointer TriangleGeom::getUnsharedEdges() const
{
  return m_UnsharedEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::setUnsharedEdges(SharedEdgeList::Pointer bEdgeList)
{
  m_UnsharedEdgeList = bEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteUnsharedEdges()
{
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::getParametricCenter(double pCoords[3]) const
{
  pCoords[0] = 1.0 / 3.0;
  pCoords[1] = 1.0 / 3.0;
  pCoords[2] = 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::getShapeFunctions(double pCoords[3], double* shape) const
{
  (void)pCoords;

  // r derivatives
  shape[0] = -1.0;
  shape[1] = 1.0;
  shape[2] = 0.0;

  // s derivatives
  shape[3] = -1.0;
  shape[4] = 0.0;
  shape[5] = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable)
{
  m_ProgressCounter = 0;
  size_t numTris = getNumberOfTris();

  if(observable != nullptr)
  {
    connect(this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), observable, SLOT(processDerivativesMessage(const AbstractMessage::Pointer&)));
  }

  ParallelDataAlgorithm dataAlg;
  dataAlg.setRange(0, numTris);
  dataAlg.execute(FindTriangleDerivativesImpl(this, field, derivatives));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf)) const
{
  herr_t err = 0;

  if(m_VertexList.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_VertexList);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_EdgeList.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_EdgeList);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_TriList.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_TriList);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_UnsharedEdgeList.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_UnsharedEdgeList);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_TriangleCentroids.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_TriangleCentroids);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_TriangleSizes.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_TriangleSizes);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_TriangleNeighbors.get() != nullptr)
  {
    size_t numTris = getNumberOfTris();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, size_t>(parentId, m_TriangleNeighbors, numTris, SIMPL::StringConstants::TriangleNeighbors);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_TrianglesContainingVert.get() != nullptr)
  {
    size_t numVerts = getNumberOfVertices();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, size_t>(parentId, m_TrianglesContainingVert, numVerts, SIMPL::StringConstants::TrianglesContainingVert);
    if(err < 0)
    {
      return err;
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName) const
{
  herr_t err = 0;

  // Always start the grid
  out << "  <!-- *************** START OF " << dcName << " *************** -->"
      << "\n";
  out << "  <Grid Name=\"" << dcName << "\" GridType=\"Uniform\">"
      << "\n";
  if(getEnableTimeSeries())
  {
    out << "    <Time TimeType=\"Single\" Value=\"" << getTimeValue() << "\"/>\n";
  }
  out << "    <Topology TopologyType=\"Triangle\" NumberOfElements=\"" << getNumberOfTris() << "\">"
      << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << getNumberOfTris() << " 3\">"
      << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << SIMPL::Geometry::Geometry << "/" << SIMPL::Geometry::SharedTriList << "\n";
  out << "      </DataItem>"
      << "\n";
  out << "    </Topology>"
      << "\n";

  if(m_VertexList.get() == nullptr)
  {
    out << "<!-- ********************* GEOMETRY ERROR ****************************************\n";
    out << "The Geometry with name '" << getName() << "' in DataContainer '" << dcName << "' \n";
    out << "did not have any vertices assigned.\n";
    out << "The Geometry types will be missing from the Xdmf which will cause issues when\n";
    out << "trying to load the file\n";
    out << " ********************************************************************************** -->\n";
  }
  else
  {
    out << "    <Geometry Type=\"XYZ\">"
        << "\n";
    out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << getNumberOfVertices() << " 3\" NumberType=\"Float\" Precision=\"4\">"
        << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << SIMPL::Geometry::Geometry << "/" << SIMPL::Geometry::SharedVertexList << "\n";
    out << "      </DataItem>"
        << "\n";
    out << "    </Geometry>"
        << "\n";
    out << ""
        << "\n";
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString TriangleGeom::getInfoString(SIMPL::InfoStringFormat format) const
{
  if(format == SIMPL::HtmlFormat)
  {
    return getToolTipGenerator().generateHTML();
  }

  return QString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ToolTipGenerator TriangleGeom::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;

  toolTipGen.addTitle("Geometry Info");
  toolTipGen.addValue("Type", TypeToString(getGeometryType()));
  toolTipGen.addValue("Units", LengthUnitToString(getUnits()));
  toolTipGen.addValue("Number of Triangles", QString::number(getNumberOfTris()));
  toolTipGen.addValue("Number of Vertices", QString::number(getNumberOfVertices()));

  return toolTipGen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  SharedVertexList::Pointer vertices = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedVertexList>(SIMPL::Geometry::SharedVertexList, parentId, preflight, err);

  MeshIndexArrayType::Pointer tris = GeometryHelpers::GeomIO::ReadMeshIndexListFromHDF5(SIMPL::Geometry::SharedTriList, parentId, preflight, err);
  if(tris.get() == nullptr || vertices.get() == nullptr)
  {
    return -1;
  }
  size_t numTris = tris->getNumberOfTuples();
  size_t numVerts = vertices->getNumberOfTuples();

  MeshIndexArrayType::Pointer edges = GeometryHelpers::GeomIO::ReadMeshIndexListFromHDF5(SIMPL::Geometry::SharedEdgeList, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }

  MeshIndexArrayType::Pointer bEdges = GeometryHelpers::GeomIO::ReadMeshIndexListFromHDF5(SIMPL::Geometry::UnsharedEdgeList, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer triCentroids = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::TriangleCentroids, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer triSizes = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::TriangleAreas, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer triNeighbors = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, size_t>(SIMPL::StringConstants::TriangleNeighbors, parentId, numTris, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer trisContainingVert =
      GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, size_t>(SIMPL::StringConstants::TrianglesContainingVert, parentId, numVerts, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }

  setVertices(vertices);
  setEdges(edges);
  setUnsharedEdges(bEdges);
  setTriangles(tris);
  setElementCentroids(triCentroids);
  setElementSizes(triSizes);
  setElementNeighbors(triNeighbors);
  setElementsContainingVert(trisContainingVert);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer TriangleGeom::deepCopy(bool forceNoAllocate) const
{
  SharedTriList::Pointer tris = std::dynamic_pointer_cast<SharedTriList>((getTriangles().get() == nullptr) ? nullptr : getTriangles()->deepCopy(forceNoAllocate));
  SharedVertexList::Pointer verts = std::dynamic_pointer_cast<SharedVertexList>((getVertices().get() == nullptr) ? nullptr : getVertices()->deepCopy(forceNoAllocate));
  SharedEdgeList::Pointer edges = std::dynamic_pointer_cast<SharedEdgeList>((getEdges().get() == nullptr) ? nullptr : getEdges()->deepCopy(forceNoAllocate));
  SharedEdgeList::Pointer unsharedEdges = std::dynamic_pointer_cast<SharedEdgeList>((getUnsharedEdges().get() == nullptr) ? nullptr : getUnsharedEdges()->deepCopy(forceNoAllocate));
  ElementDynamicList::Pointer elementsContainingVert =
      std::dynamic_pointer_cast<ElementDynamicList>((getElementsContainingVert().get() == nullptr) ? nullptr : getElementsContainingVert()->deepCopy(forceNoAllocate));
  ElementDynamicList::Pointer elementNeighbors = std::dynamic_pointer_cast<ElementDynamicList>((getElementNeighbors().get() == nullptr) ? nullptr : getElementNeighbors()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementCentroids = std::dynamic_pointer_cast<FloatArrayType>((getElementCentroids().get() == nullptr) ? nullptr : getElementCentroids()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementSizes = std::dynamic_pointer_cast<FloatArrayType>((getElementSizes().get() == nullptr) ? nullptr : getElementSizes()->deepCopy(forceNoAllocate));

  TriangleGeom::Pointer copy = TriangleGeom::CreateGeometry(tris, verts, getName());

  copy->setEdges(edges);
  copy->setUnsharedEdges(unsharedEdges);
  copy->setElementsContainingVert(elementsContainingVert);
  copy->setElementNeighbors(elementNeighbors);
  copy->setElementCentroids(elementCentroids);
  copy->setElementSizes(elementSizes);
  copy->setSpatialDimensionality(getSpatialDimensionality());

  return copy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#ifdef GEOM_CLASS_NAME
#undef GEOM_CLASS_NAME
#endif
#define GEOM_CLASS_NAME TriangleGeom
#include "SIMPLib/Geometry/SharedEdgeOps.cpp"
#include "SIMPLib/Geometry/SharedTriOps.cpp"
#include "SIMPLib/Geometry/SharedVertexOps.cpp"

// -----------------------------------------------------------------------------
TriangleGeom::Pointer TriangleGeom::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
TriangleGeom::Pointer TriangleGeom::New()
{
  Pointer sharedPtr(new(TriangleGeom));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString TriangleGeom::getNameOfClass() const
{
  return QString("TriangleGeom");
}

// -----------------------------------------------------------------------------
QString TriangleGeom::ClassName()
{
  return QString("TriangleGeom");
}
