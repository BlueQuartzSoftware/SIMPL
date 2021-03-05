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

#include "DataContainer.h"

#include <QtCore/QTextStream>

#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/DataContainers/DataContainerProxy.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/GeometryHelpers.h"
#include "SIMPLib/Geometry/HexahedralGeom.h"
#include "SIMPLib/Geometry/IGeometry.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"
#include "SIMPLib/Utilities/STLUtilities.hpp"

#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::DataContainer() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::DataContainer(const QString& name)
: IDataStructureContainerNode(name)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::~DataContainer() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainer::New(const QString& name)
{
  if(name.isEmpty())
  {
    return DataContainer::NullPointer();
  }
  DataContainer::Pointer sharedPtr(new DataContainer(name));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainer::New(const DataArrayPath& path)
{
  if(path.isEmpty())
  {
    return DataContainer::NullPointer();
  }
  DataContainer::Pointer sharedPtr(new DataContainer(path.getDataContainerName()));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainer::createNewDataContainer(const QString& name)
{
  DataContainer::Pointer p = DataContainer::New(name);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::ReadDataContainerStructure(hid_t dcArrayGroupId, DataContainerArrayProxy& proxy, SIMPLH5DataReaderRequirements* req, const QString& h5InternalPath)
{
  QList<QString> dataContainers;
  QH5Utilities::getGroupObjects(dcArrayGroupId, H5Utilities::CustomHDFDataTypes::Group, dataContainers);
  for(QString dataContainerName : dataContainers)
  {
    hid_t containerGid = H5Gopen(dcArrayGroupId, dataContainerName.toLatin1().constData(), H5P_DEFAULT);
    if(containerGid < 0)
    {
      continue;
    }
    H5ScopedGroupSentinel sentinel(containerGid, false);

    DataContainerProxy dcProxy(dataContainerName);
    dcProxy.setName(dataContainerName);
    dcProxy.setFlag(Qt::Unchecked);

    uint32_t geometryType;
    herr_t err = QH5Lite::readScalarAttribute(containerGid, SIMPL::Geometry::Geometry, SIMPL::Geometry::GeometryType, geometryType);
    if(err >= 0)
    {
      dcProxy.setDCType(static_cast<unsigned int>(geometryType));
      if(req != nullptr)
      {
        IGeometry::Types geomTypes = req->getDCGeometryTypes();
        if(geomTypes.empty() || SIMPL::contains(geomTypes, static_cast<IGeometry::Type>(geometryType)))
        {
          dcProxy.setFlag(Qt::Checked);
        }
      }
    }
    QString h5Path = h5InternalPath + "/" + dataContainerName;
    // Read the Attribute Matricies for this Data Container
    AttributeMatrix::ReadAttributeMatrixStructure(containerGid, &dcProxy, req, h5Path);

    // Insert the DataContainerProxy proxy into the DataContainerArrayProxy
    proxy.insertDataContainer(dcProxy.getName(), dcProxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataContainer::getDataArrayPath() const
{
  return DataArrayPath(getName(), "", "");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::setGeometry(const IGeometry::Pointer& geometry)
{
  m_Geometry = geometry;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer DataContainer::getGeometry() const
{
  return m_Geometry;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainer::createAndAddAttributeMatrix(const std::vector<size_t>& tDims, const QString& attrMatName, AttributeMatrix::Type attrType)
{
  AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, attrMatName, attrType);
  addOrReplaceAttributeMatrix(attrMat);
  return attrMat; // Return the shared pointer
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainer::removeAttributeMatrix(const QString& name)
{
  auto it = find(name);
  if(it == end())
  {
    // DO NOT return a NullPointer for any reason other than "AttributeMatrix was not found"
    return AttributeMatrix::NullPointer();
  }
  AttributeMatrix::Pointer p = (*it);
  erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainer::renameAttributeMatrix(const QString& oldname, const QString& newname, bool overwrite)
{
  auto am = getChildByName(oldname);
  return am && am->setName(newname);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::clearAttributeMatrices()
{
  clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::NameList DataContainer::getAttributeMatrixNames() const
{
  return getNamesOfChildren();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::writeAttributeMatricesToHDF5(hid_t parentId) const
{
  int err;
  hid_t attributeMatrixId;
  for(auto iter = begin(); iter != end(); ++iter)
  {
    auto attrMat = (*iter);
    const QString amName = attrMat->getName();

    err = QH5Utilities::createGroupsFromPath(amName, parentId);
    if(err < 0)
    {
      return err;
    }
    attributeMatrixId = H5Gopen(parentId, attrMat->getName().toLatin1().data(), H5P_DEFAULT);
    H5ScopedGroupSentinel gSentinel(attributeMatrixId, false);

    AttributeMatrix::EnumType attrMatType = static_cast<AttributeMatrix::EnumType>(attrMat->getType());
    err = QH5Lite::writeScalarAttribute(parentId, amName, SIMPL::StringConstants::AttributeMatrixType, attrMatType);
    if(err < 0)
    {
      return err;
    }
    hsize_t size = attrMat->getTupleDimensions().size();
    err = QH5Lite::writePointerAttribute(parentId, amName, SIMPL::HDF5::TupleDimensions, 1, &size, attrMat->getTupleDimensions().data());
    if(err < 0)
    {
      return err;
    }
    err = attrMat->writeAttributeArraysToHDF5(attributeMatrixId);
    if(err < 0)
    {
      return err;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::readAttributeMatricesFromHDF5(bool preflight, hid_t dcGid, DataContainerProxy& dcProxy)
{
  int err = 0;
  std::vector<size_t> tDims;

  DataContainerProxy::StorageType& attrMatsToRead = dcProxy.getAttributeMatricies();
  AttributeMatrix::Type amType = AttributeMatrix::Type::Unknown;
  QString amName;
  for(QMap<QString, AttributeMatrixProxy>::iterator iter = attrMatsToRead.begin(); iter != attrMatsToRead.end(); ++iter)
  {
    if(iter.value().getFlag() == Qt::Unchecked)
    {
      continue;
    }
    amName = iter.key();

    uint32_t amTypeTmp = static_cast<uint32_t>(AttributeMatrix::Type::Unknown);
    err = QH5Lite::readScalarAttribute(dcGid, amName, SIMPL::StringConstants::AttributeMatrixType, amTypeTmp);
    if(err < 0)
    {
      return -1;
    }

    err = QH5Lite::readVectorAttribute(dcGid, amName, SIMPL::HDF5::TupleDimensions, tDims);
    if(err < 0)
    {
      return -1;
    }

    hid_t amGid = H5Gopen(dcGid, amName.toLatin1().data(), H5P_DEFAULT);
    if(amGid < 0)
    {
      return -1;
    }

    if(getAttributeMatrix(amName) == nullptr)
    {
      amType = static_cast<AttributeMatrix::Type>(amTypeTmp);
      AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, amName, amType);
      addOrReplaceAttributeMatrix(am);
    }

    AttributeMatrixProxy amProxy = iter.value();
    err = getAttributeMatrix(amName)->readAttributeArraysFromHDF5(amGid, preflight, &amProxy);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      return -1;
    }
  }

  err |= H5Gclose(dcGid);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainer::deepCopy(bool forceNoAllocate) const
{
  DataContainer::Pointer dcCopy = DataContainer::New(getName());
  dcCopy->setName(getName());

  if(m_Geometry.get() != nullptr)
  {
    IGeometry::Pointer geomCopy = m_Geometry->deepCopy(forceNoAllocate);
    dcCopy->setGeometry(geomCopy);
  }

  const auto attrMatrices = getChildren();
  for(const auto& am : attrMatrices)
  {
    AttributeMatrix::Pointer attrMat = am->deepCopy(forceNoAllocate);
    dcCopy->addOrReplaceAttributeMatrix(attrMat);
  }

  return dcCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::writeMeshToHDF5(hid_t dcGid, bool writeXdmf) const
{
  int err;
  hid_t geometryId;
  err = QH5Utilities::createGroupsFromPath(SIMPL::Geometry::Geometry, dcGid);
  if(err < 0)
  {
    return err;
  }
  geometryId = H5Gopen(dcGid, SIMPL::Geometry::Geometry.toLatin1().data(), H5P_DEFAULT);
  if(geometryId < 0)
  {
    return -1;
  }
  H5ScopedGroupSentinel gSentinel(geometryId, false);

  if(nullptr == m_Geometry.get())
  {
    err = QH5Lite::writeScalarAttribute(dcGid, SIMPL::Geometry::Geometry, SIMPL::Geometry::GeometryType, static_cast<AttributeMatrix::EnumType>(IGeometry::Type::Unknown));
    if(err < 0)
    {
      return err;
    }
    err = QH5Lite::writeStringAttribute(dcGid, SIMPL::Geometry::Geometry, SIMPL::Geometry::GeometryTypeName, SIMPL::Geometry::UnknownGeometry);
    if(err < 0)
    {
      return err;
    }
  }
  else
  {
    err = QH5Lite::writeScalarAttribute(dcGid, SIMPL::Geometry::Geometry, SIMPL::Geometry::GeometryType, static_cast<AttributeMatrix::EnumType>(m_Geometry->getGeometryType()));
    if(err < 0)
    {
      return err;
    }
    err = QH5Lite::writeStringAttribute(dcGid, SIMPL::Geometry::Geometry, SIMPL::Geometry::GeometryTypeName, m_Geometry->getGeometryTypeAsString());
    if(err < 0)
    {
      return err;
    }
    err = QH5Lite::writeStringAttribute(dcGid, SIMPL::Geometry::Geometry, SIMPL::Geometry::GeometryName, m_Geometry->getName());
    if(err < 0)
    {
      return err;
    }
    err = QH5Lite::writeScalarAttribute(dcGid, SIMPL::Geometry::Geometry, SIMPL::Geometry::UnitDimensionality, m_Geometry->getUnitDimensionality());
    if(err < 0)
    {
      return err;
    }
    err = QH5Lite::writeScalarAttribute(dcGid, SIMPL::Geometry::Geometry, SIMPL::Geometry::SpatialDimensionality, m_Geometry->getSpatialDimensionality());
    if(err < 0)
    {
      return err;
    }
    err = m_Geometry->IGeometry::writeGeometryToHDF5(geometryId, writeXdmf);
    if(err < 0)
    {
      return err;
    }
    err = m_Geometry->writeGeometryToHDF5(geometryId, writeXdmf);
    if(err < 0)
    {
      return err;
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::writeXdmf(QTextStream& out, const QString& hdfFileName) const
{
  if(nullptr == m_Geometry.get())
  {
    return -1;
  }

  m_Geometry->writeXdmf(out, getName(), hdfFileName);
  IGeometry::Type geomType = m_Geometry->getGeometryType();

  // Loop over each AttributeMatrix and write the meta data to the Xdmf file
  QString xdmfCenter = "";
  for(auto iter = begin(); iter != end(); ++iter)
  {
    xdmfCenter = "";
    AttributeMatrix::Pointer attrMat = (*iter);
    AttributeMatrix::Type amType = attrMat->getType();
    switch(geomType)
    {
    case IGeometry::Type::Vertex:
      switch(amType)
      {
      // FIXME: There are more AttributeMatrix Types that should be implemented
      case AttributeMatrix::Type::Vertex:
        xdmfCenter = SIMPL::XdmfCenterType::Node;
        break;
      default:
        break;
      }
      break;
    case IGeometry::Type::Edge:
      switch(amType)
      {
      // FIXME: There are more AttributeMatrix Types that should be implemented
      case AttributeMatrix::Type::Vertex:
        xdmfCenter = SIMPL::XdmfCenterType::Node;
        break;
      case AttributeMatrix::Type::Edge:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      default:
        break;
      }
      break;
    case IGeometry::Type::Triangle:
      switch(amType)
      {
      // FIXME: There are more AttributeMatrix Types that should be implemented
      case AttributeMatrix::Type::Vertex:
        xdmfCenter = SIMPL::XdmfCenterType::Node;
        break;
      case AttributeMatrix::Type::Edge:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Face:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Cell:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      default:
        break;
      }
      break;
    case IGeometry::Type::Quad:
      switch(amType)
      {
      // FIXME: There are more AttributeMatrix Types that should be implemented
      case AttributeMatrix::Type::Vertex:
        xdmfCenter = SIMPL::XdmfCenterType::Node;
        break;
      case AttributeMatrix::Type::Edge:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Face:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Cell:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      default:
        break;
      }
      break;
    case IGeometry::Type::Tetrahedral:
      switch(amType)
      {
      // FIXME: There are more AttributeMatrix Types that should be implemented
      case AttributeMatrix::Type::Vertex:
        xdmfCenter = SIMPL::XdmfCenterType::Node;
        break;
      case AttributeMatrix::Type::Edge:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Face:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Cell:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      default:
        break;
      }
      break;
    case IGeometry::Type::Hexahedral:
      switch(amType)
      {
      // FIXME: There are more AttributeMatrix Types that should be implemented
      case AttributeMatrix::Type::Vertex:
        xdmfCenter = SIMPL::XdmfCenterType::Node;
        break;
      case AttributeMatrix::Type::Edge:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Face:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Cell:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      default:
        break;
      }
      break;
    case IGeometry::Type::Image:
      switch(amType)
      {
      // FIXME: There are more AttributeMatrix Types that should be implemented
      case AttributeMatrix::Type::Vertex:
        xdmfCenter = SIMPL::XdmfCenterType::Node;
        break;
      case AttributeMatrix::Type::Edge:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Face:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Cell:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      default:
        break;
      }
      break;
    case IGeometry::Type::RectGrid:
      switch(amType)
      {
      // FIXME: There are more AttributeMatrix Types that should be implemented
      case AttributeMatrix::Type::Vertex:
        xdmfCenter = SIMPL::XdmfCenterType::Node;
        break;
      case AttributeMatrix::Type::Edge:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Face:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      case AttributeMatrix::Type::Cell:
        xdmfCenter = SIMPL::XdmfCenterType::Cell;
        break;
      default:
        break;
      }
      break;
    case IGeometry::Type::Unknown:
      break;
    case IGeometry::Type::Any:
      break;
    }

    if(!xdmfCenter.isEmpty())
    {
      QString xdmfText = attrMat->generateXdmfText(xdmfCenter, getName(), hdfFileName, m_Geometry->getXdmfGridType());
      out << xdmfText;
    }
  }

  writeXdmfFooter(out);

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::writeXdmfFooter(QTextStream& xdmf) const
{
  xdmf << "  </Grid>"
       << "\n";
  xdmf << "  <!-- *************** END OF " << getName() << " *************** -->"
       << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::readMeshDataFromHDF5(hid_t dcGid, bool preflight)
{
  herr_t err = 0;
  QString geometryTypeName = SIMPL::Geometry::UnknownGeometry;

  err = QH5Lite::readStringAttribute(dcGid, SIMPL::Geometry::Geometry, SIMPL::Geometry::GeometryTypeName, geometryTypeName);
  if(err < 0)
  {
    return err;
  }

  hid_t geometryId = H5Gopen(dcGid, SIMPL::Geometry::Geometry.toLatin1().data(), H5P_DEFAULT);
  if(geometryId < 0)
  {
    return -1;
  }
  H5ScopedGroupSentinel gSentinel(geometryId, false);

  IGeometry::Pointer geomPtr = IGeometry::NullPointer();

  if(nullptr == m_Geometry.get())
  {
    if(geometryTypeName.compare(SIMPL::Geometry::ImageGeometry) == 0)
    {
      ImageGeom::Pointer image = ImageGeom::New();
      err = image->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, image);
      setGeometry(image);
    }
    else if(geometryTypeName.compare(SIMPL::Geometry::RectGridGeometry) == 0)
    {
      RectGridGeom::Pointer rectGrid = RectGridGeom::New();
      err = rectGrid->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, rectGrid);
      setGeometry(rectGrid);
    }
    else if(geometryTypeName.compare(SIMPL::Geometry::VertexGeometry) == 0)
    {
      VertexGeom::Pointer vertices = VertexGeom::New();
      err = vertices->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, vertices);
      setGeometry(vertices);
    }
    else if(geometryTypeName.compare(SIMPL::Geometry::EdgeGeometry) == 0)
    {
      EdgeGeom::Pointer edges = EdgeGeom::New();
      err = edges->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, edges);
      setGeometry(edges);
    }
    else if(geometryTypeName.compare(SIMPL::Geometry::TriangleGeometry) == 0)
    {
      TriangleGeom::Pointer triangles = TriangleGeom::New();
      err = triangles->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, triangles);
      setGeometry(triangles);
    }
    else if(geometryTypeName.compare(SIMPL::Geometry::QuadGeometry) == 0)
    {
      QuadGeom::Pointer quads = QuadGeom::New();
      err = quads->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, quads);
      setGeometry(quads);
    }
    else if(geometryTypeName.compare(SIMPL::Geometry::TetrahedralGeometry) == 0)
    {
      TetrahedralGeom::Pointer tets = TetrahedralGeom::New();
      err = tets->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, tets);
      setGeometry(tets);
    }
    else if(geometryTypeName.compare(SIMPL::Geometry::HexahedralGeometry) == 0)
    {
      HexahedralGeom::Pointer hexas = HexahedralGeom::New();
      err = hexas->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, hexas);
      setGeometry(hexas);
    }
    else if(geometryTypeName.compare(SIMPL::Geometry::UnknownGeometry) == 0)
    {
      setGeometry(geomPtr);
    }
    else
    {
      setGeometry(geomPtr);
    }
    // If no error in previous steps.
    if(err >= 0)
    {
      err = m_Geometry->IGeometry::readGeometryFromHDF5(geometryId, preflight);
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<DataArrayPath> DataContainer::getAllDataArrayPaths() const
{
  QVector<DataArrayPath> paths;
  const auto attributeMatrices = getChildren();
  for(const auto& am : attributeMatrices)
  {
    QString amName = am->getName();
    NameList aaNames = am->getAttributeArrayNames();

    for(QString aaName : aaNames)
    {
      DataArrayPath dap(getName(), amName, aaName);
      paths.push_back(dap);
    }
  }

  return paths;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainer::getInfoString(SIMPL::InfoStringFormat format) const
{
  if(format == SIMPL::HtmlFormat)
  {
    return getToolTipGenerator().generateHTML();
  }

  QString info;
  QTextStream ss(&info);
  ss << "Requested InfoStringFormat is not supported. " << format;
  return info;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ToolTipGenerator DataContainer::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;

  toolTipGen.addTitle("Data Container Info");
  toolTipGen.addValue("Name", getName());
  toolTipGen.addValue("Attribute Matrix Count", QString::number(getNumAttributeMatrices()));

  if(getGeometry().get() != nullptr)
  {
    toolTipGen.append(getGeometry()->getToolTipGenerator());
  }

  return toolTipGen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixShPtr DataContainer::getPrereqAttributeMatrix(AbstractFilter* filter, const QString& attributeMatrixName, int err) const
{
  QString ss;
  AttributeMatrixShPtr attributeMatrix(nullptr);
  // Make sure the name is not empty for teh AttributeMatrix and the AttributeArray Name. This would be detected below
  // in the call to get either one BUT the reason for the failure would not be evident so we make these explicit checks
  // here and send back nice error messages to ther user/programmer.
  if(attributeMatrixName.isEmpty())
  {
    if(filter != nullptr)
    {
      ss = QObject::tr("DataContainer:'%1' The name of the AttributeMatrix was empty. Please provide a name for this AttributeMatrix").arg(getName());
      filter->setErrorCondition(err * 1000, ss);
    }
    return attributeMatrix;
  }
  // Now attempt to get the AttributeMatrix which could still come back nullptr because the name does not match.
  attributeMatrix = getAttributeMatrix(attributeMatrixName);
  if(nullptr == attributeMatrix.get())
  {
    if(filter != nullptr)
    {
      ss = QObject::tr("DataContainer:'%1' An AttributeMatrix with name '%2' does not exist and is required for this filter to execute.").arg(getName()).arg(attributeMatrixName);
      filter->setErrorCondition(err * 1020, ss);
    }
    return attributeMatrix;
  }
  if(!attributeMatrix->validateAttributeArraySizes())
  {
    if(filter != nullptr)
    {
      ss = QObject::tr("DataContainer:'%1' AttributeMatrix: '%2' Attribute Matrix has Attribute Arrays with mismatched number of objects.").arg(getName()).arg(attributeMatrixName);
      filter->setErrorCondition(err * 1030, ss);
    }
    return attributeMatrix;
  }
  return attributeMatrix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixShPtr DataContainer::createNonPrereqAttributeMatrix(AbstractFilter* filter, const DataArrayPath& path, const std::vector<size_t>& tDims, AttributeMatrix::Type amType,
                                                                   RenameDataPath::DataID_t id)
{
  return createNonPrereqAttributeMatrix(filter, path.getAttributeMatrixName(), tDims, amType, id);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixShPtr DataContainer::createNonPrereqAttributeMatrix(AbstractFilter* filter, const DataArrayPath& path, const SizeVec3Type& tDims, AttributeMatrix::Type amType,
                                                                   RenameDataPath::DataID_t id)
{
  std::vector<size_t> tupleDims;
  for(const auto& value : tDims)
  {
    tupleDims.push_back(value);
  }
  return createNonPrereqAttributeMatrix(filter, path.getAttributeMatrixName(), tupleDims, amType, id);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixShPtr DataContainer::createNonPrereqAttributeMatrix(AbstractFilter* filter, const QString& attributeMatrixName, const std::vector<size_t>& tDims, AttributeMatrix::Type amType,
                                                                   RenameDataPath::DataID_t id)
{
  QString ss;
  if(attributeMatrixName.isEmpty())
  {
    if(filter != nullptr)
    {
      ss = QObject::tr("The name of the Attribute Matrix was empty. Please provide a name for this Attribute Matrix.");
      filter->setErrorCondition(-10011, ss);
    }
    return nullptr;
  }

  if(attributeMatrixName.contains('/'))
  {
    if(filter != nullptr)
    {
      ss = QObject::tr("The AttributeMatrix '%1' has forward slashes in its name").arg(attributeMatrixName);
      filter->setErrorCondition(-10012, ss);
    }
    return nullptr;
  }

  if(attributeMatrixName.compare(SIMPL::Geometry::Geometry) == 0)
  {
    if(filter != nullptr)
    {
      ss = QObject::tr("%1 is a protected name.  Please provide a different name for this Attribute Matrix.").arg(SIMPL::Geometry::Geometry);
      filter->setErrorCondition(-10013, ss);
    }
    return nullptr;
  }
  AttributeMatrixShPtr attributeMatrix = getAttributeMatrix(attributeMatrixName);
  if(nullptr == attributeMatrix.get())
  {
    attributeMatrix = createAndAddAttributeMatrix(tDims, attributeMatrixName, amType);
    // Check if path was renamed
    RenameDataPath::AlertFilterCreatedPath(filter, id, DataArrayPath(getName(), attributeMatrixName, ""));
    return attributeMatrix;
  }
  if(filter != nullptr) // If the filter object is NOT null (is valid) then set the error condition and send an error message
  {
    ss = QObject::tr("An Attribute Matrix already exists with the name %1.").arg(attributeMatrixName);
    filter->setErrorCondition(-10014, ss);
  }
  return attributeMatrix;
}

// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainer::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
QString DataContainer::getNameOfClass() const
{
  return QString("DataContainer");
}

// -----------------------------------------------------------------------------
QString DataContainer::ClassName()
{
  return QString("DataContainer");
}
