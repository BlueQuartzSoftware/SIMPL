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
 * QuadGeom re-implements code from the following vtk modules:
 *
 * * vtkQuad.cxx
 *   - re-implemented vtkQuad::GetParametricCenter to QuadGeom::getParametricCenter
 *   - re-implemented vtkQuad::InterpolationDerivs to QuadGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - re-implemented vtkGradientFilter template function ComputeCellGradientsUG to
 *     QuadGeom::findDerivatives
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QTextStream>

#include "SIMPLib/Geometry/QuadGeom.h"

#if defined SIMPL_USE_EIGEN
#include "SIMPLib/Geometry/DerivativeHelpers.h"
#endif
#include "SIMPLib/Geometry/GeometryHelpers.h"
#include "SIMPLib/Utilities/ParallelDataAlgorithm.h"

/**
 * @brief The FindQuadDerivativesImpl class implements a threaded algorithm that computes the
 * derivative of an arbitrary dimensional field on the underlying quadrilaterals
 */
class FindQuadDerivativesImpl
{
public:
  FindQuadDerivativesImpl(QuadGeom* quads, const DoubleArrayType::Pointer& field, const DoubleArrayType::Pointer& derivs)
  : m_Quads(quads)
  , m_Field(field)
  , m_Derivatives(derivs)
  {
  }

  void compute(size_t start, size_t end) const
  {
    int32_t cDims = m_Field->getNumberOfComponents();
    double* fieldPtr = m_Field->getPointer(0);
    double* derivsPtr = m_Derivatives->getPointer(0);
    double values[4] = {0.0, 0.0, 0.0, 0.0};
    double derivs[3] = {0.0, 0.0, 0.0};
    size_t verts[4] = {0, 0, 0, 0};

    size_t counter = 0;
    size_t totalElements = m_Quads->getNumberOfQuads();
    size_t progIncrement = static_cast<size_t>(totalElements / 100);

    for(size_t i = start; i < end; i++)
    {
      m_Quads->getVertsAtQuad(i, verts);
      for(size_t j = 0; j < cDims; j++)
      {
        for(size_t k = 0; k < 4; k++)
        {
          values[k] = fieldPtr[cDims * verts[k] + j];
        }
        DerivativeHelpers::QuadDeriv()(m_Quads, i, values, derivs);
        derivsPtr[i * 3 * cDims + j * 3] = derivs[0];
        derivsPtr[i * 3 * cDims + j * 3 + 1] = derivs[1];
        derivsPtr[i * 3 * cDims + j * 3 + 2] = derivs[2];
      }

      if(counter > progIncrement)
      {
        m_Quads->sendThreadSafeProgressMessage(counter, totalElements);
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
  QuadGeom* m_Quads;
  DoubleArrayType::Pointer m_Field;
  DoubleArrayType::Pointer m_Derivatives;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuadGeom::QuadGeom()
{
  m_GeometryTypeName = SIMPL::Geometry::QuadGeometry;
  m_GeometryType = IGeometry::Type::Quad;
  m_XdmfGridType = SIMPL::XdmfGridType::PolyData;
  m_UnitDimensionality = 2;
  m_SpatialDimensionality = 3;
  m_VertexList = QuadGeom::CreateSharedVertexList(0);
  m_QuadList = QuadGeom::CreateSharedQuadList(0);
  m_EdgeList = SharedEdgeList::NullPointer();
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
  m_QuadsContainingVert = ElementDynamicList::NullPointer();
  m_QuadNeighbors = ElementDynamicList::NullPointer();
  m_QuadCentroids = FloatArrayType::NullPointer();
  m_QuadSizes = FloatArrayType::NullPointer();
  m_ProgressCounter = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuadGeom::~QuadGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuadGeom::Pointer QuadGeom::CreateGeometry(size_t numQuads, const SharedVertexList::Pointer& vertices, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  SharedQuadList::Pointer quads = QuadGeom::CreateSharedQuadList(numQuads, allocate);
  QuadGeom* d = new QuadGeom();
  d->setVertices(vertices);
  d->setQuads(quads);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuadGeom::Pointer QuadGeom::CreateGeometry(const SharedQuadList::Pointer& quads, const SharedVertexList::Pointer& vertices, const QString& name)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  if(vertices.get() == nullptr)
  {
    return QuadGeom::NullPointer();
  }
  if(quads.get() == nullptr)
  {
    return QuadGeom::NullPointer();
  }
  QuadGeom* d = new QuadGeom();
  d->setVertices(vertices);
  d->setQuads(quads);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
  m_QuadList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::addOrReplaceAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if(data->getType() != AttributeMatrix::Type::Vertex && data->getType() != AttributeMatrix::Type::Edge && data->getType() != AttributeMatrix::Type::Face)
  {
    // QuadGeom can only accept vertex, edge, or face Attribute Matrices
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
size_t QuadGeom::getNumberOfElements() const
{
  return m_QuadList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::findEdges()
{
  m_EdgeList = CreateSharedEdgeList(0);
  GeometryHelpers::Connectivity::Find2DElementEdges<size_t>(m_QuadList, m_EdgeList);
  if(m_EdgeList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteEdges()
{
  m_EdgeList = SharedEdgeList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::findElementsContainingVert()
{
  m_QuadsContainingVert = ElementDynamicList::New();
  GeometryHelpers::Connectivity::FindElementsContainingVert<uint16_t, size_t>(m_QuadList, m_QuadsContainingVert, getNumberOfVertices());
  if(m_QuadsContainingVert.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer QuadGeom::getElementsContainingVert() const
{
  return m_QuadsContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert)
{
  m_QuadsContainingVert = elementsContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteElementsContainingVert()
{
  m_QuadsContainingVert = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::findElementNeighbors()
{
  int err = 0;
  if(m_QuadsContainingVert.get() == nullptr)
  {
    err = findElementsContainingVert();
    if(err < 0)
    {
      return err;
    }
  }
  m_QuadNeighbors = ElementDynamicList::New();
  err = GeometryHelpers::Connectivity::FindElementNeighbors<uint16_t, size_t>(m_QuadList, m_QuadsContainingVert, m_QuadNeighbors, IGeometry::Type::Quad);
  if(m_QuadNeighbors.get() == nullptr)
  {
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer QuadGeom::getElementNeighbors() const
{
  return m_QuadNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::setElementNeighbors(ElementDynamicList::Pointer elementNeighbors)
{
  m_QuadNeighbors = elementNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteElementNeighbors()
{
  m_QuadNeighbors = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::findElementCentroids()
{
  std::vector<size_t> cDims(1, 3);
  m_QuadCentroids = FloatArrayType::CreateArray(getNumberOfQuads(), cDims, SIMPL::StringConstants::QuadCentroids, true);
  GeometryHelpers::Topology::FindElementCentroids<size_t>(m_QuadList, m_VertexList, m_QuadCentroids);
  if(m_QuadCentroids.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer QuadGeom::getElementCentroids() const
{
  return m_QuadCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::setElementCentroids(FloatArrayType::Pointer elementCentroids)
{
  m_QuadCentroids = elementCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteElementCentroids()
{
  m_QuadCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::findElementSizes()
{
  std::vector<size_t> cDims(1, 1);
  m_QuadSizes = FloatArrayType::CreateArray(getNumberOfQuads(), cDims, SIMPL::StringConstants::QuadAreas, true);
  GeometryHelpers::Topology::Find2DElementAreas<size_t>(m_QuadList, m_VertexList, m_QuadSizes);
  if(m_QuadSizes.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
// z
// -----------------------------------------------------------------------------
FloatArrayType::Pointer QuadGeom::getElementSizes() const
{
  return m_QuadSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::setElementSizes(FloatArrayType::Pointer elementSizes)
{
  m_QuadSizes = elementSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteElementSizes()
{
  m_QuadSizes = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::findUnsharedEdges()
{
  std::vector<size_t> cDims(1, 2);
  m_UnsharedEdgeList = SharedEdgeList::CreateArray(0, cDims, SIMPL::Geometry::UnsharedEdgeList, true);
  GeometryHelpers::Connectivity::Find2DUnsharedEdges<size_t>(m_QuadList, m_UnsharedEdgeList);
  if(m_UnsharedEdgeList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedEdgeList::Pointer QuadGeom::getUnsharedEdges() const
{
  return m_UnsharedEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::setUnsharedEdges(SharedEdgeList::Pointer bEdgeList)
{
  m_UnsharedEdgeList = bEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteUnsharedEdges()
{
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::getParametricCenter(double pCoords[3]) const
{
  pCoords[0] = 0.5;
  pCoords[1] = 0.5;
  pCoords[2] = 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::getShapeFunctions(double pCoords[3], double* shape) const
{
  double rm = 0.0;
  double sm = 0.0;

  rm = 1.0 - pCoords[0];
  sm = 1.0 - pCoords[1];

  shape[0] = -sm;
  shape[1] = sm;
  shape[2] = pCoords[1];
  shape[3] = -pCoords[1];
  shape[4] = -rm;
  shape[5] = -pCoords[0];
  shape[6] = pCoords[0];
  shape[7] = rm;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable)
{
  m_ProgressCounter = 0;
  size_t numQuads = getNumberOfQuads();

  if(observable != nullptr)
  {
    connect(this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), observable, SLOT(processDerivativesMessage(const AbstractMessage::Pointer&)));
  }

  ParallelDataAlgorithm dataAlg;
  dataAlg.setRange(0, numQuads);
  dataAlg.execute(FindQuadDerivativesImpl(this, field, derivatives));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf)) const
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

  if(m_QuadList.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_QuadList);
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

  if(m_QuadCentroids.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_QuadCentroids);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_QuadSizes.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_QuadSizes);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_QuadNeighbors.get() != nullptr)
  {
    size_t numQuads = getNumberOfQuads();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, size_t>(parentId, m_QuadNeighbors, numQuads, SIMPL::StringConstants::QuadNeighbors);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_QuadsContainingVert.get() != nullptr)
  {
    size_t numVerts = getNumberOfVertices();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, size_t>(parentId, m_QuadsContainingVert, numVerts, SIMPL::StringConstants::QuadsContainingVert);
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
int QuadGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName) const
{
  herr_t err = 0;

  out << "  <!-- *************** START OF " << dcName << " *************** -->"
      << "\n";
  out << "  <Grid Name=\"" << dcName << "\" GridType=\"Uniform\">"
      << "\n";
  if(getEnableTimeSeries())
  {
    out << "    <Time TimeType=\"Single\" Value=\"" << getTimeValue() << "\"/>\n";
  }
  out << "    <Topology TopologyType=\"Quadrilateral\" NumberOfElements=\"" << getNumberOfQuads() << "\">"
      << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << getNumberOfQuads() << " 4\">"
      << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << SIMPL::Geometry::Geometry << "/" << SIMPL::Geometry::SharedQuadList << "\n";
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
QString QuadGeom::getInfoString(SIMPL::InfoStringFormat format) const
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
ToolTipGenerator QuadGeom::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;

  toolTipGen.addTitle("Geometry Info");
  toolTipGen.addValue("Type", TypeToString(getGeometryType()));
  toolTipGen.addValue("Units", LengthUnitToString(getUnits()));
  toolTipGen.addValue("Number of Quads", QString::number(getNumberOfQuads()));
  toolTipGen.addValue("Number of Vertices", QString::number(getNumberOfVertices()));

  return toolTipGen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  SharedVertexList::Pointer vertices = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedVertexList>(SIMPL::Geometry::SharedVertexList, parentId, preflight, err);

  MeshIndexArrayType::Pointer quads = GeometryHelpers::GeomIO::ReadMeshIndexListFromHDF5(SIMPL::Geometry::SharedQuadList, parentId, preflight, err);
  if(quads.get() == nullptr || vertices.get() == nullptr)
  {
    return -1;
  }
  size_t numQuads = quads->getNumberOfTuples();
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
  FloatArrayType::Pointer quadCentroids = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::QuadCentroids, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer quadSizes = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::QuadAreas, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer quadNeighbors = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, MeshIndexType>(SIMPL::StringConstants::QuadNeighbors, parentId, numQuads, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer quadsContainingVert =
      GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, MeshIndexType>(SIMPL::StringConstants::QuadsContainingVert, parentId, numVerts, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }

  setVertices(vertices);
  setEdges(edges);
  setUnsharedEdges(bEdges);
  setQuads(quads);
  setElementCentroids(quadCentroids);
  setElementSizes(quadSizes);
  setElementNeighbors(quadNeighbors);
  setElementsContainingVert(quadsContainingVert);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer QuadGeom::deepCopy(bool forceNoAllocate) const
{
  SharedQuadList::Pointer quads = std::dynamic_pointer_cast<SharedQuadList>((getQuads().get() == nullptr) ? nullptr : getQuads()->deepCopy(forceNoAllocate));
  SharedVertexList::Pointer verts = std::dynamic_pointer_cast<SharedVertexList>((getVertices().get() == nullptr) ? nullptr : getVertices()->deepCopy(forceNoAllocate));
  SharedEdgeList::Pointer edges = std::dynamic_pointer_cast<SharedEdgeList>((getEdges().get() == nullptr) ? nullptr : getEdges()->deepCopy(forceNoAllocate));
  SharedEdgeList::Pointer unsharedEdges = std::dynamic_pointer_cast<SharedEdgeList>((getUnsharedEdges().get() == nullptr) ? nullptr : getUnsharedEdges()->deepCopy(forceNoAllocate));
  ElementDynamicList::Pointer elementsContainingVert =
      std::dynamic_pointer_cast<ElementDynamicList>((getElementsContainingVert().get() == nullptr) ? nullptr : getElementsContainingVert()->deepCopy(forceNoAllocate));
  ElementDynamicList::Pointer elementNeighbors = std::dynamic_pointer_cast<ElementDynamicList>((getElementNeighbors().get() == nullptr) ? nullptr : getElementNeighbors()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementCentroids = std::dynamic_pointer_cast<FloatArrayType>((getElementCentroids().get() == nullptr) ? nullptr : getElementCentroids()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementSizes = std::dynamic_pointer_cast<FloatArrayType>((getElementSizes().get() == nullptr) ? nullptr : getElementSizes()->deepCopy(forceNoAllocate));

  QuadGeom::Pointer copy = QuadGeom::CreateGeometry(quads, verts, getName());

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
#define GEOM_CLASS_NAME QuadGeom
#include "SIMPLib/Geometry/SharedEdgeOps.cpp"
#include "SIMPLib/Geometry/SharedQuadOps.cpp"
#include "SIMPLib/Geometry/SharedVertexOps.cpp"

// -----------------------------------------------------------------------------
QuadGeom::Pointer QuadGeom::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
QuadGeom::Pointer QuadGeom::New()
{
  Pointer sharedPtr(new(QuadGeom));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString QuadGeom::getNameOfClass() const
{
  return QString("QuadGeom");
}

// -----------------------------------------------------------------------------
QString QuadGeom::ClassName()
{
  return QString("QuadGeom");
}
