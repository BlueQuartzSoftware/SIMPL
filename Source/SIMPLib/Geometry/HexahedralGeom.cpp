/* ============================================================================
 * HexahedralGeom re-implements code from the following vtk modules:
 *
 * * vtkHexahedron.cxx
 *   - re-implemented vtkHexahedron::GetParametricCenter to HexahedralGeom::getParametricCenter
 *   - re-implemented vtkHexahedron::InterpolationDerivs to HexahedralGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - re-implemented vtkGradientFilter template function ComputeCellGradientsUG to
 *     HexahedralGeom::findDerivatives
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SIMPLib/Geometry/HexahedralGeom.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Geometry/DerivativeHelpers.h"
#include "SIMPLib/Geometry/GeometryHelpers.h"

/**
 * @brief The FindHexDerivativesImpl class implements a threaded algorithm that computes the
 * derivative of an arbitrary dimensional field on the underlying hexahedra
 */
class FindHexDerivativesImpl
{
public:
  FindHexDerivativesImpl(HexahedralGeom* hexas, DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivs)
  : m_Hexas(hexas)
  , m_Field(field)
  , m_Derivatives(derivs)
  {
  }
  virtual ~FindHexDerivativesImpl()
  {
  }

  void compute(int64_t start, int64_t end) const
  {
    int32_t cDims = m_Field->getNumberOfComponents();
    double* fieldPtr = m_Field->getPointer(0);
    double* derivsPtr = m_Derivatives->getPointer(0);
    double values[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double derivs[3] = {0.0, 0.0, 0.0};
    int64_t verts[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    int64_t counter = 0;
    int64_t totalElements = m_Hexas->getNumberOfHexas();
    int64_t progIncrement = static_cast<int64_t>(totalElements / 100);

    for(int64_t i = start; i < end; i++)
    {
      m_Hexas->getVertsAtHex(i, verts);
      for(int32_t j = 0; j < cDims; j++)
      {
        for(size_t k = 0; k < 8; k++)
        {
          values[k] = fieldPtr[cDims * verts[k] + j];
        }
        DerivativeHelpers::HexDeriv()(m_Hexas, i, values, derivs);
        derivsPtr[i * 3 * cDims + j * 3] = derivs[0];
        derivsPtr[i * 3 * cDims + j * 3 + 1] = derivs[1];
        derivsPtr[i * 3 * cDims + j * 3 + 2] = derivs[2];
      }

      if(counter > progIncrement)
      {
        m_Hexas->sendThreadSafeProgressMessage(counter, totalElements);
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
  HexahedralGeom* m_Hexas;
  DoubleArrayType::Pointer m_Field;
  DoubleArrayType::Pointer m_Derivatives;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HexahedralGeom::HexahedralGeom()
{
  m_GeometryTypeName = SIMPL::Geometry::HexahedralGeometry;
  m_GeometryType = IGeometry::Type::Hexahedral;
  m_XdmfGridType = SIMPL::XdmfGridType::PolyData;
  m_MessagePrefix = "";
  m_MessageTitle = "";
  m_MessageLabel = "";
  m_UnitDimensionality = 3;
  m_SpatialDimensionality = 3;
  m_VertexList = HexahedralGeom::CreateSharedVertexList(0);
  m_HexList = HexahedralGeom::CreateSharedHexList(0);
  m_QuadList = SharedQuadList::NullPointer();
  m_EdgeList = SharedEdgeList::NullPointer();
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
  m_UnsharedQuadList = SharedQuadList::NullPointer();
  m_HexasContainingVert = ElementDynamicList::NullPointer();
  m_HexNeighbors = ElementDynamicList::NullPointer();
  m_HexCentroids = FloatArrayType::NullPointer();
  m_HexSizes = FloatArrayType::NullPointer();
  m_ProgressCounter = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HexahedralGeom::~HexahedralGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HexahedralGeom::Pointer HexahedralGeom::CreateGeometry(int64_t numHexas, SharedVertexList::Pointer vertices, const QString& name, bool allocate)
{
  if(name.isEmpty() == true)
  {
    return NullPointer();
  }
  SharedHexList::Pointer hexas = HexahedralGeom::CreateSharedHexList(numHexas, allocate);
  HexahedralGeom* d = new HexahedralGeom();
  d->setVertices(vertices);
  d->setHexahedra(hexas);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HexahedralGeom::Pointer HexahedralGeom::CreateGeometry(SharedHexList::Pointer hexas, SharedVertexList::Pointer vertices, const QString& name)
{
  if(name.isEmpty() == true)
  {
    return NullPointer();
  }
  if(vertices.get() == nullptr)
  {
    return HexahedralGeom::NullPointer();
  }
  if(hexas.get() == nullptr)
  {
    return HexahedralGeom::NullPointer();
  }
  HexahedralGeom* d = new HexahedralGeom();
  d->setVertices(vertices);
  d->setHexahedra(hexas);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
  m_HexList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if(data->getType() != AttributeMatrix::Type::Vertex
     || data->getType() != AttributeMatrix::Type::Edge
     || data->getType() != AttributeMatrix::Type::Face || data->getType() != AttributeMatrix::Type::Cell)
  {
    // HexahedralGeom can only accept vertex, edge, face or cell Attribute Matrices
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
  if(data->getType() == AttributeMatrix::Type::Face && static_cast<int64_t>(data->getNumberOfTuples()) != getNumberOfQuads())
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
size_t HexahedralGeom::getNumberOfElements()
{
  return m_HexList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexahedralGeom::findEdges()
{
  m_EdgeList = CreateSharedEdgeList(0);
  GeometryHelpers::Connectivity::FindHexEdges<int64_t>(m_HexList, m_EdgeList);
  if(m_EdgeList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::deleteEdges()
{
  m_EdgeList = SharedEdgeList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexahedralGeom::findFaces()
{
  m_QuadList = CreateSharedQuadList(0);
  GeometryHelpers::Connectivity::FindHexFaces<int64_t>(m_HexList, m_QuadList);
  if(m_QuadList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::deleteFaces()
{
  m_QuadList = SharedTriList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexahedralGeom::findElementsContainingVert()
{
  m_HexasContainingVert = ElementDynamicList::New();
  GeometryHelpers::Connectivity::FindElementsContainingVert<uint16_t, int64_t>(m_HexList, m_HexasContainingVert, getNumberOfVertices());
  if(m_HexasContainingVert.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer HexahedralGeom::getElementsContainingVert()
{
  return m_HexasContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert)
{
  m_HexasContainingVert = elementsContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::deleteElementsContainingVert()
{
  m_HexasContainingVert = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexahedralGeom::findElementNeighbors()
{
  int err = 0;
  if(m_HexasContainingVert.get() == nullptr)
  {
    err = findElementsContainingVert();
    if(err < 0)
    {
      return err;
    }
  }
  m_HexNeighbors = ElementDynamicList::New();
  err = GeometryHelpers::Connectivity::FindElementNeighbors<uint16_t, int64_t>(m_HexList, m_HexasContainingVert, m_HexNeighbors, IGeometry::Type::Hexahedral);
  if(m_HexNeighbors.get() == nullptr)
  {
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer HexahedralGeom::getElementNeighbors()
{
  return m_HexNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::setElementNeighbors(ElementDynamicList::Pointer elementNeighbors)
{
  m_HexNeighbors = elementNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::deleteElementNeighbors()
{
  m_HexNeighbors = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexahedralGeom::findElementCentroids()
{
  QVector<size_t> cDims(1, 3);
  m_HexCentroids = FloatArrayType::CreateArray(getNumberOfHexas(), cDims, SIMPL::StringConstants::HexCentroids);
  GeometryHelpers::Topology::FindElementCentroids<int64_t>(m_HexList, m_VertexList, m_HexCentroids);
  if(m_HexCentroids.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer HexahedralGeom::getElementCentroids()
{
  return m_HexCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::setElementCentroids(FloatArrayType::Pointer elementCentroids)
{
  m_HexCentroids = elementCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::deleteElementCentroids()
{
  m_HexCentroids = FloatArrayType::NullPointer();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexahedralGeom::findElementSizes()
{
  QVector<size_t> cDims(1, 1);
  m_HexSizes = FloatArrayType::CreateArray(getNumberOfHexas(), cDims, SIMPL::StringConstants::HexVolumes);
  GeometryHelpers::Topology::FindHexVolumes<int64_t>(m_HexList, m_VertexList, m_HexSizes);
  if(m_HexSizes.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer HexahedralGeom::getElementSizes()
{
  return m_HexSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::setElementSizes(FloatArrayType::Pointer elementSizes)
{
  m_HexSizes = elementSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::deleteElementSizes()
{
  m_HexSizes = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexahedralGeom::findUnsharedEdges()
{
  QVector<size_t> cDims(1, 2);
  m_UnsharedEdgeList = SharedEdgeList::CreateArray(0, cDims, SIMPL::Geometry::UnsharedEdgeList);
  GeometryHelpers::Connectivity::FindUnsharedHexEdges<int64_t>(m_HexList, m_UnsharedEdgeList);
  if(m_UnsharedEdgeList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedEdgeList::Pointer HexahedralGeom::getUnsharedEdges()
{
  return m_UnsharedEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::setUnsharedEdges(SharedEdgeList::Pointer bEdgeList)
{
  m_UnsharedEdgeList = bEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::deleteUnsharedEdges()
{
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexahedralGeom::findUnsharedFaces()
{
  QVector<size_t> cDims(1, 4);
  m_UnsharedQuadList = SharedQuadList::CreateArray(0, cDims, SIMPL::Geometry::UnsharedFaceList);
  GeometryHelpers::Connectivity::FindUnsharedHexFaces<int64_t>(m_HexList, m_UnsharedQuadList);
  if(m_UnsharedQuadList.get() == nullptr)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedQuadList::Pointer HexahedralGeom::getUnsharedFaces()
{
  return m_UnsharedQuadList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::setUnsharedFaces(SharedFaceList::Pointer bFaceList)
{
  m_UnsharedQuadList = bFaceList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::deleteUnsharedFaces()
{
  m_UnsharedQuadList = SharedTriList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::getParametricCenter(double pCoords[3])
{
  pCoords[0] = 0.5;
  pCoords[1] = 0.5;
  pCoords[2] = 0.5;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::getShapeFunctions(double pCoords[3], double* shape)
{
  double rm = 0.0;
  double sm = 0.0;
  double tm = 0.0;

  rm = 1.0 - pCoords[0];
  sm = 1.0 - pCoords[1];
  tm = 1.0 - pCoords[2];

  // r-derivatives
  shape[0] = -sm * tm;
  shape[1] = sm * tm;
  shape[2] = pCoords[1] * tm;
  shape[3] = -pCoords[1] * tm;
  shape[4] = -sm * pCoords[2];
  shape[5] = sm * pCoords[2];
  shape[6] = pCoords[1] * pCoords[2];
  shape[7] = -pCoords[1] * pCoords[2];

  // s-derivatives
  shape[8] = -rm * tm;
  shape[9] = -pCoords[0] * tm;
  shape[10] = pCoords[0] * tm;
  shape[11] = rm * tm;
  shape[12] = -rm * pCoords[2];
  shape[13] = -pCoords[0] * pCoords[2];
  shape[14] = pCoords[0] * pCoords[2];
  shape[15] = rm * pCoords[2];

  // t-derivatives
  shape[16] = -rm * sm;
  shape[17] = -pCoords[0] * sm;
  shape[18] = -pCoords[0] * pCoords[1];
  shape[19] = -rm * pCoords[1];
  shape[20] = rm * sm;
  shape[21] = pCoords[0] * sm;
  shape[22] = pCoords[0] * pCoords[1];
  shape[23] = rm * pCoords[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexahedralGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable)
{
  m_ProgressCounter = 0;
  int64_t numHexas = getNumberOfHexas();

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
    tbb::parallel_for(tbb::blocked_range<int64_t>(0, numHexas), FindHexDerivativesImpl(this, field, derivatives), tbb::auto_partitioner());
  }
  else
#endif
  {
    FindHexDerivativesImpl serial(this, field, derivatives);
    serial.compute(0, numHexas);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexahedralGeom::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf))
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

  if(m_HexList.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_HexList);
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

  if(m_UnsharedQuadList.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_UnsharedQuadList);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_HexCentroids.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_HexCentroids);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_HexSizes.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_HexSizes);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_HexNeighbors.get() != nullptr)
  {
    size_t numHexas = getNumberOfHexas();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_HexNeighbors, numHexas, SIMPL::StringConstants::HexNeighbors);
    if(err < 0)
    {
      return err;
    }
  }

  if(m_HexasContainingVert.get() != nullptr)
  {
    size_t numVerts = getNumberOfVertices();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_HexasContainingVert, numVerts, SIMPL::StringConstants::HexasContainingVert);
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
int HexahedralGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName)
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
  out << "    <Topology TopologyType=\"Hexahedron\" NumberOfElements=\"" << getNumberOfHexas() << "\">"
      << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << getNumberOfHexas() << " 8\">"
      << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << SIMPL::Geometry::Geometry << "/" << SIMPL::Geometry::SharedHexList << "\n";
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
QString HexahedralGeom::getInfoString(SIMPL::InfoStringFormat format)
{
  QString info;
  QTextStream ss(&info);

  if(format == SIMPL::HtmlFormat)
  {
    ss << "<tr bgcolor=\"#FFFCEA\"><th colspan=2>Geometry Info</th></tr>";
    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Type</th><td>" << GeometryHelpers::Translation::TypeToString(getGeometryType()) << "</td></tr>";
    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Number of Hexahedra</th><td>" << getNumberOfHexas() << "</td></tr>";
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
int HexahedralGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  SharedVertexList::Pointer vertices = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedVertexList>(SIMPL::Geometry::SharedVertexList, parentId, preflight, err);
  SharedHexList::Pointer hexas = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedHexList>(SIMPL::Geometry::SharedHexList, parentId, preflight, err);
  if(hexas.get() == nullptr || vertices.get() == nullptr)
  {
    return -1;
  }
  size_t numHexas = hexas->getNumberOfTuples();
  size_t numVerts = vertices->getNumberOfTuples();
  SharedQuadList::Pointer quads = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedQuadList>(SIMPL::Geometry::SharedQuadList, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  SharedQuadList::Pointer bQuads = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedQuadList>(SIMPL::Geometry::UnsharedFaceList, parentId, preflight, err);
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
  FloatArrayType::Pointer hexCentroids = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::HexCentroids, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer hexSizes = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::HexVolumes, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer hexNeighbors = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, int64_t>(SIMPL::StringConstants::HexNeighbors, parentId, numHexas, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer hexasContainingVert = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, int64_t>(SIMPL::StringConstants::HexasContainingVert, parentId, numVerts, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }

  setVertices(vertices);
  setHexahedra(hexas);
  setEdges(edges);
  setUnsharedEdges(bEdges);
  setQuads(quads);
  setUnsharedFaces(bQuads);
  setElementCentroids(hexCentroids);
  setElementSizes(hexSizes);
  setElementNeighbors(hexNeighbors);
  setElementsContainingVert(hexasContainingVert);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer HexahedralGeom::deepCopy(bool forceNoAllocate)
{
  SharedHexList::Pointer hexas = std::dynamic_pointer_cast<SharedHexList>((getHexahedra().get() == nullptr) ? nullptr : getHexahedra()->deepCopy(forceNoAllocate));
  SharedVertexList::Pointer verts = std::dynamic_pointer_cast<SharedVertexList>((getVertices().get() == nullptr) ? nullptr : getVertices()->deepCopy(forceNoAllocate));
  SharedQuadList::Pointer quads = std::dynamic_pointer_cast<SharedQuadList>((getQuads().get() == nullptr) ? nullptr : getQuads()->deepCopy(forceNoAllocate));
  SharedQuadList::Pointer unsharedQuads = std::dynamic_pointer_cast<SharedQuadList>((getUnsharedFaces().get() == nullptr) ? nullptr : getUnsharedFaces()->deepCopy(forceNoAllocate));
  SharedEdgeList::Pointer edges = std::dynamic_pointer_cast<SharedEdgeList>((getEdges().get() == nullptr) ? nullptr : getEdges()->deepCopy(forceNoAllocate));
  SharedEdgeList::Pointer unsharedEdges = std::dynamic_pointer_cast<SharedEdgeList>((getUnsharedEdges().get() == nullptr) ? nullptr : getUnsharedEdges()->deepCopy(forceNoAllocate));
  ElementDynamicList::Pointer elementsContainingVert =
      std::dynamic_pointer_cast<ElementDynamicList>((getElementsContainingVert().get() == nullptr) ? nullptr : getElementsContainingVert()->deepCopy(forceNoAllocate));
  ElementDynamicList::Pointer elementNeighbors = std::dynamic_pointer_cast<ElementDynamicList>((getElementNeighbors().get() == nullptr) ? nullptr : getElementNeighbors()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementCentroids = std::dynamic_pointer_cast<FloatArrayType>((getElementCentroids().get() == nullptr) ? nullptr : getElementCentroids()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementSizes = std::dynamic_pointer_cast<FloatArrayType>((getElementSizes().get() == nullptr) ? nullptr : getElementSizes()->deepCopy(forceNoAllocate));

  HexahedralGeom::Pointer copy = HexahedralGeom::CreateGeometry(hexas, verts, getName());

  copy->setEdges(edges);
  copy->setUnsharedEdges(unsharedEdges);
  copy->setQuads(quads);
  copy->setUnsharedFaces(unsharedQuads);
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
#define GEOM_CLASS_NAME HexahedralGeom
#include "SIMPLib/Geometry/SharedEdgeOps.cpp"
#include "SIMPLib/Geometry/SharedHexOps.cpp"
#include "SIMPLib/Geometry/SharedQuadOps.cpp"
#include "SIMPLib/Geometry/SharedVertexOps.cpp"
