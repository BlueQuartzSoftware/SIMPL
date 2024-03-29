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
 * VertexGeom re-implements code from the following vtk modules:
 *
 * * vtkVertex.cxx
 *   - re-implemented vtkVertex::GetParametricCenter to VertexGeom::getParametricCenter
 *   - re-implemented vtkVertex::InterpolationDerivs to VertexGeom::getShapeFunctions
 *   - re-implemented vtkVertex::Derivatives to VertexGeom::findDerivatives
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QTextStream>

#include "SIMPLib/Geometry/GeometryHelpers.h"
#include "SIMPLib/Geometry/VertexGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::VertexGeom()
{
  m_GeometryTypeName = SIMPL::Geometry::VertexGeometry;
  m_GeometryType = IGeometry::Type::Vertex;
  m_XdmfGridType = SIMPL::XdmfGridType::PolyData;
  m_UnitDimensionality = 0;
  m_SpatialDimensionality = 3;
  m_VertexList = VertexGeom::CreateSharedVertexList(0);
  m_VertexSizes = FloatArrayType::NullPointer();
  m_ProgressCounter = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::~VertexGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::Pointer VertexGeom::CreateGeometry(size_t numVertices, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  SharedVertexList::Pointer vertices = VertexGeom::CreateSharedVertexList(numVertices, allocate);
  auto d = new VertexGeom();
  d->setVertices(vertices);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::Pointer VertexGeom::CreateGeometry(const SharedVertexList::Pointer& vertices, const QString& name)
{
  if(name.isEmpty())
  {
    return VertexGeom::NullPointer();
  }
  if(vertices.get() == nullptr)
  {
    return VertexGeom::NullPointer();
  }
  auto d = new VertexGeom();
  d->setVertices(vertices);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::addOrReplaceAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if(data->getType() != AttributeMatrix::Type::Vertex)
  {
    // VertexGeom can only accept vertex Attribute Matrices
    return;
  }
  if(data->getNumberOfTuples() != getNumberOfElements())
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
size_t VertexGeom::getNumberOfElements() const
{
  return m_VertexList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::findElementsContainingVert()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer VertexGeom::getElementsContainingVert() const
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert)
{
  Q_UNUSED(elementsContainingVert)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::deleteElementsContainingVert()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::findElementNeighbors()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer VertexGeom::getElementNeighbors() const
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::setElementNeighbors(ElementDynamicList::Pointer elementNeighbors)
{
  Q_UNUSED(elementNeighbors)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::deleteElementNeighbors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::findElementCentroids()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer VertexGeom::getElementCentroids() const
{
  return FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::setElementCentroids(FloatArrayType::Pointer elementCentroids)
{
  Q_UNUSED(elementCentroids)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::deleteElementCentroids()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::findElementSizes()
{
  // Vertices are 0-dimensional (they have no size),
  // so simply splat 0 over the sizes array
  m_VertexSizes = FloatArrayType::CreateArray(getNumberOfElements(), SIMPL::StringConstants::VoxelSizes, true);
  m_VertexSizes->initializeWithValue(0.0f);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer VertexGeom::getElementSizes() const
{
  return m_VertexSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::setElementSizes(FloatArrayType::Pointer elementSizes)
{
  m_VertexSizes = elementSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::deleteElementSizes()
{
  m_VertexSizes = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::getParametricCenter(double pCoords[3]) const
{
  pCoords[0] = 0.0;
  pCoords[1] = 0.0;
  pCoords[2] = 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::getShapeFunctions(double pCoords[3], double* shape) const
{
  (void)pCoords;

  shape[0] = 0.0;
  shape[1] = 0.0;
  shape[2] = 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable)
{
  // The exterior derivative of a point source is zero,
  // so simply splat 0 over the derivatives array
  derivatives->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::writeGeometryToHDF5(hid_t parentId, bool writeXdmf) const
{
  herr_t err = 0;
  std::vector<size_t> tDims(1, 0);

  if(m_VertexList.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_VertexList);
    if(err < 0)
    {
      return err;
    }
    if(writeXdmf)
    {
      std::vector<size_t> cDims(1, 1);
      DataArray<size_t>::Pointer vertsPtr = DataArray<size_t>::CreateArray(getNumberOfVertices(), cDims, SIMPL::StringConstants::VertsName, true);
      size_t* verts = vertsPtr->getPointer(0);
      for(size_t i = 0; i < vertsPtr->getNumberOfTuples(); i++)
      {
        verts[i] = i;
      }
      tDims[0] = vertsPtr->getNumberOfTuples();
      err = vertsPtr->writeH5Data(parentId, tDims);
    }
  }
  if(m_VertexSizes.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_VertexSizes);
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
int VertexGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName) const
{
  herr_t err = 0;

  // Always start the grid
  out << "  <!-- *************** START OF " << dcName << " *************** -->"
      << "\n";
  out << "  <Grid Name=\"" << dcName << R"(" GridType="Uniform">)"
      << "\n";
  if(getEnableTimeSeries())
  {
    out << R"(    <Time TimeType="Single" Value=")" << getTimeValue() << "\"/>\n";
  }

#if 0
  DataArrayPath dap = getTemporalDataPath();
  if(dap.isValid())
  {
    AttributeMatrix::Pointer am = getAttributeMatrix(dap.getAttributeMatrixName());
    if(nullptr != am.get())
    {
      IDataArray::Pointer timeValues = am->getAttributeArray(dap.getDataArrayName());
      Int32ArrayType::Pointer timeValuePtr = std::dynamic_pointer_cast<Int32ArrayType>(timeValues);
      if(nullptr != timeValuePtr.get())
      {
        out << "    <Time TimeType=\"Single\" Value=\"" << timeValuePtr->getValue(0) << "\"/>\n";
      }
    }
  }
#endif

  out << R"(    <Topology TopologyType="Polyvertex" NumberOfElements=")" << getNumberOfVertices() << "\">"
      << "\n";
  out << R"(      <DataItem Format="HDF" NumberType="Int" Dimensions=")" << getNumberOfVertices() << "\">"
      << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << SIMPL::Geometry::Geometry << "/"
      << "Verts"
      << "\n";
  out << "      </DataItem>"
      << "\n";
  out << "    </Topology>"
      << "\n";

  out << "    <Geometry Type=\"XYZ\">"
      << "\n";
  out << R"(      <DataItem Format="HDF"  Dimensions=")" << getNumberOfVertices() << R"( 3" NumberType="Float" Precision="4">)"
      << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << SIMPL::Geometry::Geometry << "/" << SIMPL::Geometry::SharedVertexList << "\n";
  out << "      </DataItem>"
      << "\n";
  out << "    </Geometry>"
      << "\n";
  out << ""
      << "\n";

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VertexGeom::getInfoString(SIMPL::InfoStringFormat format) const
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
ToolTipGenerator VertexGeom::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;

  toolTipGen.addTitle("Geometry Info");
  toolTipGen.addValue("Type", TypeToString(getGeometryType()));
  toolTipGen.addValue("Units", LengthUnitToString(getUnits()));
  toolTipGen.addValue("Number of Vertices", QString::number(getNumberOfVertices()));

  return toolTipGen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  SharedVertexList::Pointer vertices = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedVertexList>(SIMPL::Geometry::SharedVertexList, parentId, preflight, err);
  if(vertices.get() == nullptr)
  {
    return -1;
  }
  FloatArrayType::Pointer vertexSizes = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::VertexSizes, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }

  setVertices(vertices);
  setElementSizes(vertexSizes);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer VertexGeom::deepCopy(bool forceNoAllocate) const
{
  SharedVertexList::Pointer verts = std::dynamic_pointer_cast<SharedVertexList>((getVertices().get() == nullptr) ? nullptr : getVertices()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementSizes = std::dynamic_pointer_cast<FloatArrayType>((getElementSizes().get() == nullptr) ? nullptr : getElementSizes()->deepCopy(forceNoAllocate));

  VertexGeom::Pointer vertexCopy = VertexGeom::CreateGeometry(verts, getName());
  vertexCopy->setElementSizes(elementSizes);
  vertexCopy->setSpatialDimensionality(getSpatialDimensionality());

  return vertexCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#ifdef GEOM_CLASS_NAME
#undef GEOM_CLASS_NAME
#endif

#define GEOM_CLASS_NAME VertexGeom
#include "SIMPLib/Geometry/SharedVertexOps.cpp"

// -----------------------------------------------------------------------------
VertexGeom::Pointer VertexGeom::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
VertexGeom::Pointer VertexGeom::New()
{
  Pointer sharedPtr(new(VertexGeom));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString VertexGeom::getNameOfClass() const
{
  return QString("VertexGeom");
}

// -----------------------------------------------------------------------------
QString VertexGeom::ClassName()
{
  return QString("VertexGeom");
}
