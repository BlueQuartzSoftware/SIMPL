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
 * TetrahedralGeom re-implements code from the following vtk modules:
 *
 * * vtkTetra.cxx
 *   - re-implemented vtkTetra::GetParametricCenter to TetrahedralGeom::getParametricCenter
 *   - re-implemented vtkTetra::InterpolationDerivs to TetrahedralGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - re-implemented vtkGradientFilter template function ComputeCellGradientsUG to
 *     TetrahedralGeom::findDerivatives
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SIMPLib/Geometry/TetrahedralGeom.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Geometry/DerivativeHelpers.h"
#include "SIMPLib/Geometry/GeometryHelpers.h"

/**
 * @brief The FindTriangleDerivativesImpl class implements a threaded algorithm that computes the
 * derivative of an arbitrary dimensional field on the underlying triangles
 */
class FindTetDerivativesImpl
{
public:
  FindTetDerivativesImpl(TetrahedralGeom* tets, DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivs)
  : m_Tets(tets)
  , m_Field(field)
  , m_Derivatives(derivs)
  {
  }
  virtual ~FindTetDerivativesImpl() = default;

  void compute(int64_t start, int64_t end) const
  {
    int32_t cDims = m_Field->getNumberOfComponents();
    double* fieldPtr = m_Field->getPointer(0);
    double* derivsPtr = m_Derivatives->getPointer(0);
    double values[4] = {0.0, 0.0, 0.0, 0.0};
    double derivs[3] = {0.0, 0.0, 0.0};
    int64_t verts[4]{0, 0, 0, 0};

    int64_t counter = 0;
    int64_t totalElements = m_Tets->getNumberOfTets();
    int64_t progIncrement = static_cast<int64_t>(totalElements / 100);

    for(int64_t i = start; i < end; i++)
    {
      m_Tets->getVertsAtTet(i, verts);
      for(int32_t j = 0; j < cDims; j++)
      {
        for(size_t k = 0; k < 4; k++)
        {
          values[k] = fieldPtr[cDims * verts[k] + j];
        }
        DerivativeHelpers::TetDeriv()(m_Tets, i, values, derivs);
        derivsPtr[i * 3 * cDims + j * 3] = derivs[0];
        derivsPtr[i * 3 * cDims + j * 3 + 1] = derivs[1];
        derivsPtr[i * 3 * cDims + j * 3 + 2] = derivs[2];
      }

      if(counter > progIncrement)
      {
        m_Tets->sendThreadSafeProgressMessage(counter, totalElements);
        counter = 0;
      }
      counter++;
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<int64_t>& r) const
  {
    compute(r.begin(), r.end());
  }
#endif
private:
  TetrahedralGeom* m_Tets;
  DoubleArrayType::Pointer m_Field;
  DoubleArrayType::Pointer m_Derivatives;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TetrahedralGeom::TetrahedralGeom()
{
  m_GeometryTypeName = SIMPL::Geometry::TetrahedralGeometry;
  m_GeometryType = IGeometry::Type::Tetrahedral;
  m_XdmfGridType = SIMPL::XdmfGridType::PolyData;
  m_MessagePrefix = "";
  m_MessageTitle = "";
  m_MessageLabel = "";
  m_UnitDimensionality = 3;
  m_SpatialDimensionality = 3;
  m_VertexList = TetrahedralGeom::CreateSharedVertexList(0);
  m_TetList = TetrahedralGeom::CreateSharedTetList(0);
  m_TriList = SharedTriList::NullPointer();
  m_EdgeList = SharedEdgeList::NullPointer();
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
  m_UnsharedTriList = SharedTriList::NullPointer();
  m_TetsContainingVert = ElementDynamicList::NullPointer();
  m_TetNeighbors = ElementDynamicList::NullPointer();
  m_TetCentroids = FloatArrayType::NullPointer();
  m_TetSizes = FloatArrayType::NullPointer();
  m_ProgressCounter = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TetrahedralGeom::~TetrahedralGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TetrahedralGeom::Pointer TetrahedralGeom::CreateGeometry(int64_t numTets, SharedVertexList::Pointer vertices, const QString& name, bool allocate)
{
  if(name.isEmpty() == true)
  {
    return NullPointer();
  }
  SharedTetList::Pointer tets = TetrahedralGeom::CreateSharedTetList(numTets, allocate);
  TetrahedralGeom* d = new TetrahedralGeom();
  d->setVertices(vertices);
  d->setTetrahedra(tets);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TetrahedralGeom::Pointer TetrahedralGeom::CreateGeometry(SharedTetList::Pointer tets, SharedVertexList::Pointer vertices, const QString& name)
{
  if(name.isEmpty() == true)
  {
    return NullPointer();
  }
  if(vertices.get() == nullptr)
  {
    return TetrahedralGeom::NullPointer();
  }
  if(tets.get() == nullptr)
  {
    return TetrahedralGeom::NullPointer();
  }
  TetrahedralGeom* d = new TetrahedralGeom();
  d->setVertices(vertices);
  d->setTetrahedra(tets);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
  m_TetList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if(data->getType() != AttributeMatrix::Type::Vertex
     || data->getType() != AttributeMatrix::Type::Edge
     || data->getType() != AttributeMatrix::Type::Face || data->getType() != AttributeMatrix::Type::Cell)
  {
    // TetrahedralGeom can only accept vertex, edge, face or cell Attribute Matrices
    return;
  }
  if(data->getType() == AttributeMatrix::Type::Vertex && static_cast<int64_t>(data->getNumberOfTuples()) != getNumberOfVertices())
  {
    return;
  }
  if(data->getType() == AttributeMatrix::Type::Edge && static_cast<int64_t>(data->getNumberOfTuples()) != getNumberOfEdges())
  {
    return;
  }
  if(data->getType() == AttributeMatrix::Type::Face && static_cast<int64_t>(data->getNumberOfTuples()) != getNumberOfTris())
  {
    return;
  }
  if(data->getType() == AttributeMatrix::Type::Cell && data->getNumberOfTuples() != getNumberOfElements())
  {
    return;
  }
  if(data->getName().compare(name) != 0)
  {
    data->setName(name);
  }
  m_AttributeMatrices[name] = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t TetrahedralGeom::getNumberOfElements()
{
  return m_TetList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TetrahedralGeom::findEdges()
{
  m_EdgeList = CreateSharedEdgeList(0);
  GeometryHelpers::Connectivity::FindTetEdges<int64_t>(m_TetList, m_EdgeList);
  if(m_EdgeList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::deleteEdges()
{
  m_EdgeList = SharedEdgeList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TetrahedralGeom::findFaces()
{
  m_TriList = CreateSharedTriList(0);
  GeometryHelpers::Connectivity::FindTetFaces<int64_t>(m_TetList, m_TriList);
  if(m_TriList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::deleteFaces()
{
  m_TriList = SharedTriList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TetrahedralGeom::findElementsContainingVert()
{
  m_TetsContainingVert = ElementDynamicList::New();
  GeometryHelpers::Connectivity::FindElementsContainingVert<uint16_t, int64_t>(m_TetList, m_TetsContainingVert, getNumberOfVertices());
  if(m_TetsContainingVert.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer TetrahedralGeom::getElementsContainingVert()
{
  return m_TetsContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert)
{
  m_TetsContainingVert = elementsContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::deleteElementsContainingVert()
{
  m_TetsContainingVert = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TetrahedralGeom::findElementNeighbors()
{
  int err = 0;
  if(m_TetsContainingVert.get() == nullptr)
  {
    err = findElementsContainingVert();
    if(err < 0)
    {
      return err;
    }
  }
  m_TetNeighbors = ElementDynamicList::New();
  err = GeometryHelpers::Connectivity::FindElementNeighbors<uint16_t, int64_t>(m_TetList, m_TetsContainingVert, m_TetNeighbors, IGeometry::Type::Tetrahedral);
  if(m_TetNeighbors.get() == nullptr)
  {
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer TetrahedralGeom::getElementNeighbors()
{
  return m_TetNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::setElementNeighbors(ElementDynamicList::Pointer elementNeighbors)
{
  m_TetNeighbors = elementNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::deleteElementNeighbors()
{
  m_TetNeighbors = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TetrahedralGeom::findElementCentroids()
{
  QVector<size_t> cDims(1, 3);
  m_TetCentroids = FloatArrayType::CreateArray(getNumberOfTets(), cDims, SIMPL::StringConstants::TetCentroids);
  GeometryHelpers::Topology::FindElementCentroids<int64_t>(m_TetList, m_VertexList, m_TetCentroids);
  if(m_TetCentroids.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer TetrahedralGeom::getElementCentroids()
{
  return m_TetCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::setElementCentroids(FloatArrayType::Pointer elementCentroids)
{
  m_TetCentroids = elementCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::deleteElementCentroids()
{
  m_TetCentroids = FloatArrayType::NullPointer();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TetrahedralGeom::findElementSizes()
{
  QVector<size_t> cDims(1, 1);
  m_TetSizes = FloatArrayType::CreateArray(getNumberOfTets(), cDims, SIMPL::StringConstants::TetVolumes);
  GeometryHelpers::Topology::FindTetVolumes<int64_t>(m_TetList, m_VertexList, m_TetSizes);
  if(m_TetSizes.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer TetrahedralGeom::getElementSizes()
{
  return m_TetSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::setElementSizes(FloatArrayType::Pointer elementSizes)
{
  m_TetSizes = elementSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::deleteElementSizes()
{
  m_TetSizes = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TetrahedralGeom::findUnsharedEdges()
{
  QVector<size_t> cDims(1, 2);
  m_UnsharedEdgeList = SharedEdgeList::CreateArray(0, cDims, SIMPL::Geometry::UnsharedEdgeList);
  GeometryHelpers::Connectivity::FindUnsharedTetEdges<int64_t>(m_TetList, m_UnsharedEdgeList);
  if(m_UnsharedEdgeList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedEdgeList::Pointer TetrahedralGeom::getUnsharedEdges()
{
  return m_UnsharedEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::setUnsharedEdges(SharedEdgeList::Pointer bEdgeList)
{
  m_UnsharedEdgeList = bEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::deleteUnsharedEdges()
{
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TetrahedralGeom::findUnsharedFaces()
{
  QVector<size_t> cDims(1, 3);
  m_UnsharedTriList = SharedTriList::CreateArray(0, cDims, SIMPL::Geometry::UnsharedFaceList);
  GeometryHelpers::Connectivity::FindUnsharedTetFaces<int64_t>(m_TetList, m_UnsharedTriList);
  if(m_UnsharedTriList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedTriList::Pointer TetrahedralGeom::getUnsharedFaces()
{
  return m_UnsharedTriList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::setUnsharedFaces(SharedFaceList::Pointer bFaceList)
{
  m_UnsharedTriList = bFaceList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::deleteUnsharedFaces()
{
  m_UnsharedTriList = SharedTriList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::getParametricCenter(double pCoords[3])
{
  pCoords[0] = 0.25;
  pCoords[1] = 0.25;
  pCoords[2] = 0.25;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::getShapeFunctions(double pCoords[3], double* shape)
{
  (void)pCoords;

  // r-derivatives
  shape[0] = -1.0;
  shape[1] = 1.0;
  shape[2] = 0.0;
  shape[3] = 0.0;

  // s-derivatives
  shape[4] = -1.0;
  shape[5] = 0.0;
  shape[6] = 1.0;
  shape[7] = 0.0;

  // t-derivatives
  shape[8] = -1.0;
  shape[9] = 0.0;
  shape[10] = 0.0;
  shape[11] = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetrahedralGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable)
{
  m_ProgressCounter = 0;
  int64_t numTets = getNumberOfTets();

  if(observable)
  {
    connect(this, SIGNAL(filterGeneratedMessage(const PipelineMessage&)), observable, SLOT(broadcastPipelineMessage(const PipelineMessage&)));
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<int64_t>(0, numTets), FindTetDerivativesImpl(this, field, derivatives), tbb::auto_partitioner());
  }
  else
#endif
  {
    FindTetDerivativesImpl serial(this, field, derivatives);
    serial.compute(0, numTets);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TetrahedralGeom::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf))
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

  if(m_TetList.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_TetList);
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

  if(m_UnsharedTriList.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_UnsharedTriList);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_TetCentroids.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_TetCentroids);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_TetSizes.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_TetSizes);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_TetNeighbors.get() != nullptr)
  {
    size_t numTets = getNumberOfTets();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_TetNeighbors, numTets, SIMPL::StringConstants::TetNeighbors);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_TetsContainingVert.get() != nullptr)
  {
    size_t numVerts = getNumberOfVertices();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_TetsContainingVert, numVerts, SIMPL::StringConstants::TetsContainingVert);
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
int TetrahedralGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName)
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
  out << "    <Topology TopologyType=\"Tetrahedron\" NumberOfElements=\"" << getNumberOfTets() << "\">"
      << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << getNumberOfTets() << " 4\">"
      << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << SIMPL::Geometry::Geometry << "/" << SIMPL::Geometry::SharedTetList << "\n";
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
QString TetrahedralGeom::getInfoString(SIMPL::InfoStringFormat format)
{
  QString info;
  QTextStream ss(&info);

  if(format == SIMPL::HtmlFormat)
  {
    ss << "<tr bgcolor=\"#FFFCEA\"><th colspan=2>Geometry Info</th></tr>";
    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Type</th><td>" << GeometryHelpers::Translation::TypeToString(getGeometryType()) << "</td></tr>";
    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Number of Tetrahedra</th><td>" << getNumberOfTets() << "</td></tr>";
    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Number of Vertices</th><td>" << getNumberOfVertices() << "</td></tr>";
  }
  else
  {
  }
  return info;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TetrahedralGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  SharedVertexList::Pointer vertices = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedVertexList>(SIMPL::Geometry::SharedVertexList, parentId, preflight, err);
  SharedTetList::Pointer tets = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedTetList>(SIMPL::Geometry::SharedTetList, parentId, preflight, err);
  if(tets.get() == nullptr || vertices.get() == nullptr)
  {
    return -1;
  }
  size_t numTets = tets->getNumberOfTuples();
  size_t numVerts = vertices->getNumberOfTuples();
  SharedTriList::Pointer tris = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedTriList>(SIMPL::Geometry::SharedTriList, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  SharedTriList::Pointer bTris = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedTriList>(SIMPL::Geometry::UnsharedFaceList, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  SharedEdgeList::Pointer edges = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedEdgeList>(SIMPL::Geometry::SharedEdgeList, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  SharedEdgeList::Pointer bEdges = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedEdgeList>(SIMPL::Geometry::UnsharedEdgeList, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer tetCentroids = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::TetCentroids, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer tetSizes = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::TetVolumes, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer tetNeighbors = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, int64_t>(SIMPL::StringConstants::TetNeighbors, parentId, numTets, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer tetsContainingVert = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, int64_t>(SIMPL::StringConstants::TetsContainingVert, parentId, numVerts, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }

  setVertices(vertices);
  setTetrahedra(tets);
  setEdges(edges);
  setUnsharedEdges(bEdges);
  setTriangles(tris);
  setUnsharedFaces(bTris);
  setElementCentroids(tetCentroids);
  setElementSizes(tetSizes);
  setElementNeighbors(tetNeighbors);
  setElementsContainingVert(tetsContainingVert);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer TetrahedralGeom::deepCopy(bool forceNoAllocate)
{
  SharedTetList::Pointer tets = std::dynamic_pointer_cast<SharedTetList>((getTetrahedra().get() == nullptr) ? nullptr : getTetrahedra()->deepCopy(forceNoAllocate));
  SharedVertexList::Pointer verts = std::dynamic_pointer_cast<SharedVertexList>((getVertices().get() == nullptr) ? nullptr : getVertices()->deepCopy(forceNoAllocate));
  SharedTriList::Pointer tris = std::dynamic_pointer_cast<SharedTriList>((getTriangles().get() == nullptr) ? nullptr : getTriangles()->deepCopy(forceNoAllocate));
  SharedEdgeList::Pointer edges = std::dynamic_pointer_cast<SharedEdgeList>((getEdges().get() == nullptr) ? nullptr : getEdges()->deepCopy(forceNoAllocate));
  SharedEdgeList::Pointer unsharedEdges = std::dynamic_pointer_cast<SharedEdgeList>((getUnsharedEdges().get() == nullptr) ? nullptr : getUnsharedEdges()->deepCopy(forceNoAllocate));
  ElementDynamicList::Pointer elementsContainingVert =
      std::dynamic_pointer_cast<ElementDynamicList>((getElementsContainingVert().get() == nullptr) ? nullptr : getElementsContainingVert()->deepCopy(forceNoAllocate));
  ElementDynamicList::Pointer elementNeighbors = std::dynamic_pointer_cast<ElementDynamicList>((getElementNeighbors().get() == nullptr) ? nullptr : getElementNeighbors()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementCentroids = std::dynamic_pointer_cast<FloatArrayType>((getElementCentroids().get() == nullptr) ? nullptr : getElementCentroids()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementSizes = std::dynamic_pointer_cast<FloatArrayType>((getElementSizes().get() == nullptr) ? nullptr : getElementSizes()->deepCopy(forceNoAllocate));

  TetrahedralGeom::Pointer copy = TetrahedralGeom::CreateGeometry(tets, verts, getName());

  copy->setEdges(edges);
  copy->setUnsharedEdges(unsharedEdges);
  copy->setTriangles(tris);
  copy->setUnsharedFaces(getUnsharedFaces());
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
#define GEOM_CLASS_NAME TetrahedralGeom
#include "SIMPLib/Geometry/SharedEdgeOps.cpp"
#include "SIMPLib/Geometry/SharedTetOps.cpp"
#include "SIMPLib/Geometry/SharedTriOps.cpp"
#include "SIMPLib/Geometry/SharedVertexOps.cpp"
