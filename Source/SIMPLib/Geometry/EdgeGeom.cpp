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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ============================================================================
 * EdgeGeom re-implements code from the following vtk modules:
 *
 * * vtkLine.cxx
 *   - re-implemented vtkLine::GetParametricCenter to EdgeGeom::getParametricCenter
 *   - re-implemented vtkLine::InterpolationDerivs to EdgeGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - re-implemented vtkGradientFilter template function ComputeCellGradientsUG to
 *     EdgeGeom::findDerivatives
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QTextStream>

#include "SIMPLib/Geometry/DerivativeHelpers.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/GeometryHelpers.h"
#include "SIMPLib/Utilities/ParallelDataAlgorithm.h"

/**
 * @brief The FindEdgeDerivativesImpl class implements a threaded algorithm that computes the
 * derivative of an arbitrary dimensional field on the underlying edges
 */
class FindEdgeDerivativesImpl
{
public:
  FindEdgeDerivativesImpl(EdgeGeom* edges, const DoubleArrayType::Pointer& field, const DoubleArrayType::Pointer& derivs)
  : m_Edges(edges)
  , m_Field(field)
  , m_Derivatives(derivs)
  {
  }
  virtual ~FindEdgeDerivativesImpl() = default;

  void compute(size_t start, size_t end) const
  {
    int32_t cDims = m_Field->getNumberOfComponents();
    double* fieldPtr = m_Field->getPointer(0);
    double* derivsPtr = m_Derivatives->getPointer(0);
    double values[2] = {0.0, 0.0};
    double derivs[3] = {0.0, 0.0, 0.0};
    size_t verts[2] = {0, 0};

    size_t counter = 0;
    size_t totalElements = m_Edges->getNumberOfEdges();
    size_t progIncrement = static_cast<size_t>(totalElements / 100);

    for(size_t i = start; i < end; i++)
    {
      m_Edges->getVertsAtEdge(i, verts);
      for(size_t j = 0; j < cDims; j++)
      {
        for(size_t k = 0; k < 2; k++)
        {
          values[k] = fieldPtr[cDims * verts[k] + j];
        }
        DerivativeHelpers::EdgeDeriv()(m_Edges, i, values, derivs);
        derivsPtr[i * 3 * cDims + j * 3] = derivs[0];
        derivsPtr[i * 3 * cDims + j * 3 + 1] = derivs[1];
        derivsPtr[i * 3 * cDims + j * 3 + 2] = derivs[2];
      }

      if(counter > progIncrement)
      {
        m_Edges->sendThreadSafeProgressMessage(counter, totalElements);
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
  EdgeGeom* m_Edges;
  DoubleArrayType::Pointer m_Field;
  DoubleArrayType::Pointer m_Derivatives;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeGeom::EdgeGeom()
{
  m_GeometryTypeName = SIMPL::Geometry::EdgeGeometry;
  m_GeometryType = IGeometry::Type::Edge;
  m_XdmfGridType = SIMPL::XdmfGridType::PolyData;
  m_UnitDimensionality = 1;
  m_SpatialDimensionality = 3;
  m_VertexList = EdgeGeom::CreateSharedVertexList(0);
  m_EdgeList = EdgeGeom::CreateSharedEdgeList(0);
  m_EdgesContainingVert = ElementDynamicList::NullPointer();
  m_EdgeNeighbors = ElementDynamicList::NullPointer();
  m_EdgeCentroids = FloatArrayType::NullPointer();
  m_EdgeSizes = FloatArrayType::NullPointer();
  m_ProgressCounter = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeGeom::~EdgeGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeGeom::Pointer EdgeGeom::CreateGeometry(size_t numEdges, SharedVertexList::Pointer vertices, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  SharedEdgeList::Pointer edges = EdgeGeom::CreateSharedEdgeList(numEdges, allocate);
  EdgeGeom* d = new EdgeGeom();
  d->setVertices(vertices);
  d->setEdges(edges);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeGeom::Pointer EdgeGeom::CreateGeometry(SharedEdgeList::Pointer edges, SharedVertexList::Pointer vertices, const QString& name)
{
  if(name.isEmpty())
  {
    return EdgeGeom::NullPointer();
  }
  if(vertices.get() == nullptr)
  {
    return EdgeGeom::NullPointer();
  }
  if(edges.get() == nullptr)
  {
    return EdgeGeom::NullPointer();
  }
  EdgeGeom* d = new EdgeGeom();
  d->setVertices(vertices);
  d->setEdges(edges);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
  m_EdgeList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::addOrReplaceAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if(data->getType() != AttributeMatrix::Type::Vertex && data->getType() != AttributeMatrix::Type::Edge)
  {
    // EdgeGeom can only accept vertex or edge Attribute Matrices
    return;
  }
  if(data->getType() == AttributeMatrix::Type::Vertex && static_cast<size_t>(data->getNumberOfTuples()) != getNumberOfVertices())
  {
    return;
  }
  if(data->getType() == AttributeMatrix::Type::Edge && data->getNumberOfTuples() != getNumberOfElements())
  {
    return;
  }
  // if(data->getName().compare(name) != 0)
  //{
  //  data->setName(name);
  //}
  m_AttributeMatrices[data->getName()] = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t EdgeGeom::getNumberOfElements() const
{
  return m_EdgeList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::findElementsContainingVert()
{
  m_EdgesContainingVert = ElementDynamicList::New();
  GeometryHelpers::Connectivity::FindElementsContainingVert<uint16_t, size_t>(m_EdgeList, m_EdgesContainingVert, getNumberOfVertices());
  if(m_EdgesContainingVert.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer EdgeGeom::getElementsContainingVert() const
{
  return m_EdgesContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert)
{
  m_EdgesContainingVert = elementsContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteElementsContainingVert()
{
  m_EdgesContainingVert = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::findElementNeighbors()
{
  int err = 0;
  if(m_EdgesContainingVert.get() == nullptr)
  {
    err = findElementsContainingVert();
    if(err < 0)
    {
      return err;
    }
  }
  m_EdgeNeighbors = ElementDynamicList::New();
  err = GeometryHelpers::Connectivity::FindElementNeighbors<uint16_t, size_t>(m_EdgeList, m_EdgesContainingVert, m_EdgeNeighbors, IGeometry::Type::Edge);
  if(m_EdgeNeighbors.get() == nullptr)
  {
    err = -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer EdgeGeom::getElementNeighbors() const
{
  return m_EdgeNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::setElementNeighbors(ElementDynamicList::Pointer elementNeighbors)
{
  m_EdgeNeighbors = elementNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteElementNeighbors()
{
  m_EdgeNeighbors = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::findElementCentroids()
{
  std::vector<size_t> cDims(1, 3);
  m_EdgeCentroids = FloatArrayType::CreateArray(getNumberOfElements(), cDims, SIMPL::StringConstants::EdgeCentroids, true);
  GeometryHelpers::Topology::FindElementCentroids<size_t>(m_EdgeList, m_VertexList, m_EdgeCentroids);
  if(m_EdgeCentroids.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer EdgeGeom::getElementCentroids() const
{
  return m_EdgeCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::setElementCentroids(FloatArrayType::Pointer elementCentroids)
{
  m_EdgeCentroids = elementCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteElementCentroids()
{
  m_EdgeCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::findElementSizes()
{
  m_EdgeSizes = FloatArrayType::CreateArray(getNumberOfElements(), SIMPL::StringConstants::EdgeLengths, true);

  float* sizes = m_EdgeSizes->getPointer(0);
  float vert0[3] = {0.0f, 0.0f, 0.0f};
  float vert1[3] = {0.0f, 0.0f, 0.0f};

  for(size_t i = 0; i < getNumberOfEdges(); i++)
  {
    getVertCoordsAtEdge(i, vert0, vert1);
    float length = 0.0f;
    for(size_t j = 0; j < 3; j++)
    {
      length += (vert0[j] - vert1[j]) * (vert0[j] - vert1[j]);
    }
    sizes[i] = sqrtf(length);
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer EdgeGeom::getElementSizes() const
{
  return m_EdgeSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::setElementSizes(FloatArrayType::Pointer elementSizes)
{
  m_EdgeSizes = elementSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteElementSizes()
{
  m_EdgeSizes = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::getParametricCenter(double pCoords[3]) const
{
  pCoords[0] = 0.5;
  pCoords[1] = 0.0;
  pCoords[2] = 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::getShapeFunctions(double pCoords[3], double* shape) const
{
  (void)pCoords;

  shape[0] = -1.0;
  shape[1] = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable)
{
  m_ProgressCounter = 0;
  size_t numEdges = getNumberOfEdges();

  if(observable != nullptr)
  {
    connect(this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), observable, SLOT(processDerivativesMessage(const AbstractMessage::Pointer&)));
  }

  ParallelDataAlgorithm dataAlg;
  dataAlg.setRange(0, numEdges);
  dataAlg.execute(FindEdgeDerivativesImpl(this, field, derivatives));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf)) const
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

  if(m_EdgeCentroids.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_EdgeCentroids);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_EdgeSizes.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_EdgeSizes);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_EdgeNeighbors.get() != nullptr)
  {
    size_t numEdges = static_cast<size_t>(getNumberOfEdges());
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, size_t>(parentId, m_EdgeNeighbors, numEdges, SIMPL::StringConstants::EdgeNeighbors);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_EdgesContainingVert.get() != nullptr)
  {
    size_t numVerts = static_cast<size_t>(getNumberOfVertices());
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, size_t>(parentId, m_EdgesContainingVert, numVerts, SIMPL::StringConstants::EdgesContainingVert);
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
int EdgeGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName) const
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
  out << "    <Topology TopologyType=\"Polyline\" NodesPerElement=\"2\" NumberOfElements=\"" << getNumberOfEdges() << "\">"
      << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << getNumberOfEdges() << " 2\">"
      << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << SIMPL::Geometry::Geometry << "/" << SIMPL::Geometry::SharedEdgeList << "\n";
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
QString EdgeGeom::getInfoString(SIMPL::InfoStringFormat format) const
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
ToolTipGenerator EdgeGeom::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;

  toolTipGen.addTitle("Geometry Info");
  toolTipGen.addValue("Type", TypeToString(getGeometryType()));
  toolTipGen.addValue("Units", LengthUnitToString(getUnits()));
  toolTipGen.addValue("Number of Edges", QString::number(getNumberOfEdges()));
  toolTipGen.addValue("Number of Vertices", QString::number(getNumberOfVertices()));

  return toolTipGen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  SharedVertexList::Pointer vertices = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedVertexList>(SIMPL::Geometry::SharedVertexList, parentId, preflight, err);

  SharedEdgeList::Pointer edges = GeometryHelpers::GeomIO::ReadMeshIndexListFromHDF5(SIMPL::Geometry::SharedEdgeList, parentId, preflight, err);

  if(edges.get() == nullptr || vertices.get() == nullptr)
  {
    return -1;
  }
  size_t numEdges = edges->getNumberOfTuples();
  size_t numVerts = vertices->getNumberOfTuples();
  FloatArrayType::Pointer edgeCentroids = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::EdgeCentroids, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer edgeSizes = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::EdgeLengths, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer edgeNeighbors = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, MeshIndexType>(SIMPL::StringConstants::EdgeNeighbors, parentId, numEdges, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer edgesContainingVert =
      GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, MeshIndexType>(SIMPL::StringConstants::EdgesContainingVert, parentId, numVerts, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }

  setVertices(vertices);
  setEdges(edges);
  setElementCentroids(edgeCentroids);
  setElementSizes(edgeSizes);
  setElementNeighbors(edgeNeighbors);
  setElementsContainingVert(edgesContainingVert);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer EdgeGeom::deepCopy(bool forceNoAllocate) const
{
  SharedVertexList::Pointer verts = std::dynamic_pointer_cast<SharedVertexList>((getVertices().get() == nullptr) ? nullptr : getVertices()->deepCopy(forceNoAllocate));
  SharedEdgeList::Pointer edges = std::dynamic_pointer_cast<SharedEdgeList>((getEdges().get() == nullptr) ? nullptr : getEdges()->deepCopy(forceNoAllocate));
  ElementDynamicList::Pointer elementsContainingVert =
      std::dynamic_pointer_cast<ElementDynamicList>((getElementsContainingVert().get() == nullptr) ? nullptr : getElementsContainingVert()->deepCopy(forceNoAllocate));
  ElementDynamicList::Pointer elementNeighbors = std::dynamic_pointer_cast<ElementDynamicList>((getElementNeighbors().get() == nullptr) ? nullptr : getElementNeighbors()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementCentroids = std::dynamic_pointer_cast<FloatArrayType>((getElementCentroids().get() == nullptr) ? nullptr : getElementCentroids()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementSizes = std::dynamic_pointer_cast<FloatArrayType>((getElementSizes().get() == nullptr) ? nullptr : getElementSizes()->deepCopy(forceNoAllocate));

  EdgeGeom::Pointer copy = EdgeGeom::CreateGeometry(edges, verts, getName());

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
#define GEOM_CLASS_NAME EdgeGeom
#include "SIMPLib/Geometry/SharedEdgeOps.cpp"
#include "SIMPLib/Geometry/SharedVertexOps.cpp"

// -----------------------------------------------------------------------------
EdgeGeom::Pointer EdgeGeom::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
EdgeGeom::Pointer EdgeGeom::New()
{
  Pointer sharedPtr(new(EdgeGeom));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString EdgeGeom::getNameOfClass() const
{
  return QString("EdgeGeom");
}

// -----------------------------------------------------------------------------
QString EdgeGeom::ClassName()
{
  return QString("EdgeGeom");
}
