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

#include "SIMPLib/Geometry/IGeometry.h"
#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/QH5Utilities.h"
#include "SIMPLib/Geometry/CompositeTransformContainer.h"
#include "SIMPLib/Geometry/TransformContainer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::IGeometry()
: m_TimeValue(0.0f)
, m_EnableTimeSeries(false)
, m_Units(LengthUnit::Unspecified)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::~IGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Type IGeometry::StringToType(const QString& str)
{
  IGeometry::Type t = IGeometry::Type::Unknown;

  if(str == SIMPL::Geometry::ImageGeometry)
  {
    return IGeometry::Type::Image;
  }
  if(str == SIMPL::Geometry::RectGridGeometry)
  {
    return IGeometry::Type::RectGrid;
  }
  if(str == SIMPL::Geometry::VertexGeometry)
  {
    return IGeometry::Type::Vertex;
  }
  if(str == SIMPL::Geometry::EdgeGeometry)
  {
    return IGeometry::Type::Edge;
  }
  if(str == SIMPL::Geometry::TriangleGeometry)
  {
    return IGeometry::Type::Triangle;
  }
  if(str == SIMPL::Geometry::QuadGeometry)
  {
    return IGeometry::Type::Quad;
  }
  if(str == SIMPL::Geometry::TetrahedralGeometry)
  {
    return IGeometry::Type::Tetrahedral;
  }
  if(str == SIMPL::Geometry::UnknownGeometry)
  {
    return IGeometry::Type::Unknown;
  }
  if(str == SIMPL::Geometry::AnyGeometry)
  {
    return IGeometry::Type::Any;
  }

  return t;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString IGeometry::TypeToString(IGeometry::Type t)
{
  QString geomType;
  // The type values for RectGrid and ImageGeom are the same value (Why?)
  // which causes compile errors with the switch statement so we special
  // case the RectGrid here, and leave it out of the Switch block.
  if(t == IGeometry::Type::RectGrid)
  {
    return SIMPL::Geometry::RectGridGeometry;
  }

  switch(t)
  {

  case IGeometry::Type::Image:
    geomType = SIMPL::Geometry::ImageGeometry;
    break;
  case IGeometry::Type::Vertex:
    geomType = SIMPL::Geometry::VertexGeometry;
    break;
  case IGeometry::Type::Edge:
    geomType = SIMPL::Geometry::EdgeGeometry;
    break;
  case IGeometry::Type::Triangle:
    geomType = SIMPL::Geometry::TriangleGeometry;
    break;
  case IGeometry::Type::Quad:
    geomType = SIMPL::Geometry::QuadGeometry;
    break;
  case IGeometry::Type::Tetrahedral:
    geomType = SIMPL::Geometry::TetrahedralGeometry;
    break;
  case IGeometry::Type::Hexahedral:
    geomType = SIMPL::Geometry::HexahedralGeometry;
    break;
  case IGeometry::Type::RectGrid:
    geomType = SIMPL::Geometry::RectGridGeometry;
    break;
  case IGeometry::Type::Unknown:
    geomType = SIMPL::Geometry::UnknownGeometry;
    break;
  case IGeometry::Type::Any:
    geomType = SIMPL::Geometry::AnyGeometry;
    break;
  }

  return geomType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::LengthUnit IGeometry::StringToLengthUnit(const QString& str)
{
  if(str == SIMPL::Geometry::k_Yoctometer)
  {
    return IGeometry::LengthUnit::Yoctometer;
  }
  if(str == SIMPL::Geometry::k_Zeptometer)
  {
    return IGeometry::LengthUnit::Zeptometer;
  }
  if(str == SIMPL::Geometry::k_Attometer)
  {
    return IGeometry::LengthUnit::Attometer;
  }
  if(str == SIMPL::Geometry::k_Femtometer)
  {
    return IGeometry::LengthUnit::Femtometer;
  }
  if(str == SIMPL::Geometry::k_Picometer)
  {
    return IGeometry::LengthUnit::Picometer;
  }
  if(str == SIMPL::Geometry::k_Nanometer)
  {
    return IGeometry::LengthUnit::Nanometer;
  }
  if(str == SIMPL::Geometry::k_Micrometer)
  {
    return IGeometry::LengthUnit::Micrometer;
  }
  if(str == SIMPL::Geometry::k_Millimeter)
  {
    return IGeometry::LengthUnit::Millimeter;
  }
  if(str == SIMPL::Geometry::k_Centimeter)
  {
    return IGeometry::LengthUnit::Centimeter;
  }
  if(str == SIMPL::Geometry::k_Decimeter)
  {
    return IGeometry::LengthUnit::Decimeter;
  }
  if(str == SIMPL::Geometry::k_Meter)
  {
    return IGeometry::LengthUnit::Meter;
  }
  if(str == SIMPL::Geometry::k_Decameter)
  {
    return IGeometry::LengthUnit::Decameter;
  }
  if(str == SIMPL::Geometry::k_Hectometer)
  {
    return IGeometry::LengthUnit::Hectometer;
  }
  if(str == SIMPL::Geometry::k_Kilometer)
  {
    return IGeometry::LengthUnit::Kilometer;
  }
  if(str == SIMPL::Geometry::k_Megameter)
  {
    return IGeometry::LengthUnit::Megameter;
  }
  if(str == SIMPL::Geometry::k_Gigameter)
  {
    return IGeometry::LengthUnit::Gigameter;
  }
  if(str == SIMPL::Geometry::k_Terameter)
  {
    return IGeometry::LengthUnit::Terameter;
  }
  if(str == SIMPL::Geometry::k_Petameter)
  {
    return IGeometry::LengthUnit::Petameter;
  }
  if(str == SIMPL::Geometry::k_Exameter)
  {
    return IGeometry::LengthUnit::Exameter;
  }
  if(str == SIMPL::Geometry::k_Zettameter)
  {
    return IGeometry::LengthUnit::Zettameter;
  }
  if(str == SIMPL::Geometry::k_Yottameter)
  {
    return IGeometry::LengthUnit::Yottameter;
  }
  if(str == SIMPL::Geometry::k_Angstrom)
  {
    return IGeometry::LengthUnit::Angstrom;
  }
  if(str == SIMPL::Geometry::k_Mil)
  {
    return IGeometry::LengthUnit::Mil;
  }
  if(str == SIMPL::Geometry::k_Inch)
  {
    return IGeometry::LengthUnit::Inch;
  }
  if(str == SIMPL::Geometry::k_Foot)
  {
    return IGeometry::LengthUnit::Foot;
  }
  if(str == SIMPL::Geometry::k_Mile)
  {
    return IGeometry::LengthUnit::Mile;
  }
  if(str == SIMPL::Geometry::k_Fathom)
  {
    return IGeometry::LengthUnit::Fathom;
  }
  if(str == SIMPL::Geometry::k_Unspecified)
  {
    return IGeometry::LengthUnit::Unspecified;
  }
  if(str == SIMPL::Geometry::k_Unknown)
  {
    return IGeometry::LengthUnit::Unknown;
  }

  return IGeometry::LengthUnit::Unknown;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString IGeometry::LengthUnitToString(IGeometry::LengthUnit t)
{
  switch(t)
  {
  case IGeometry::LengthUnit::Yoctometer:
    return SIMPL::Geometry::k_Yoctometer;
    break;
  case IGeometry::LengthUnit::Zeptometer:
    return SIMPL::Geometry::k_Zeptometer;
    break;
  case IGeometry::LengthUnit::Attometer:
    return SIMPL::Geometry::k_Attometer;
    break;
  case IGeometry::LengthUnit::Femtometer:
    return SIMPL::Geometry::k_Femtometer;
    break;
  case IGeometry::LengthUnit::Picometer:
    return SIMPL::Geometry::k_Picometer;
    break;
  case IGeometry::LengthUnit::Nanometer:
    return SIMPL::Geometry::k_Nanometer;
    break;
  case IGeometry::LengthUnit::Micrometer:
    return SIMPL::Geometry::k_Micrometer;
    break;
  case IGeometry::LengthUnit::Millimeter:
    return SIMPL::Geometry::k_Millimeter;
    break;
  case IGeometry::LengthUnit::Centimeter:
    return SIMPL::Geometry::k_Centimeter;
    break;
  case IGeometry::LengthUnit::Decimeter:
    return SIMPL::Geometry::k_Decimeter;
    break;
  case IGeometry::LengthUnit::Meter:
    return SIMPL::Geometry::k_Meter;
    break;
  case IGeometry::LengthUnit::Decameter:
    return SIMPL::Geometry::k_Decameter;
    break;
  case IGeometry::LengthUnit::Hectometer:
    return SIMPL::Geometry::k_Hectometer;
    break;
  case IGeometry::LengthUnit::Kilometer:
    return SIMPL::Geometry::k_Kilometer;
    break;
  case IGeometry::LengthUnit::Megameter:
    return SIMPL::Geometry::k_Megameter;
    break;
  case IGeometry::LengthUnit::Gigameter:
    return SIMPL::Geometry::k_Gigameter;
    break;
  case IGeometry::LengthUnit::Terameter:
    return SIMPL::Geometry::k_Terameter;
    break;
  case IGeometry::LengthUnit::Petameter:
    return SIMPL::Geometry::k_Petameter;
    break;
  case IGeometry::LengthUnit::Exameter:
    return SIMPL::Geometry::k_Exameter;
    break;
  case IGeometry::LengthUnit::Zettameter:
    return SIMPL::Geometry::k_Zettameter;
    break;
  case IGeometry::LengthUnit::Yottameter:
    return SIMPL::Geometry::k_Yottameter;
    break;
  case IGeometry::LengthUnit::Angstrom:
    return SIMPL::Geometry::k_Angstrom;
    break;
  case IGeometry::LengthUnit::Mil:
    return SIMPL::Geometry::k_Mil;
    break;
  case IGeometry::LengthUnit::Inch:
    return SIMPL::Geometry::k_Inch;
    break;
  case IGeometry::LengthUnit::Foot:
    return SIMPL::Geometry::k_Foot;
    break;
  case IGeometry::LengthUnit::Mile:
    return SIMPL::Geometry::k_Mile;
    break;
  case IGeometry::LengthUnit::Fathom:
    return SIMPL::Geometry::k_Fathom;
    break;
  case IGeometry::LengthUnit::Unspecified:
    return SIMPL::Geometry::k_Unspecified;
    break;
  case IGeometry::LengthUnit::Unknown:
    return SIMPL::Geometry::k_Unknown;
    break;
  }
  return SIMPL::Geometry::k_Unknown;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IGeometry::setName(const QString& name)
{
  m_Name = name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString IGeometry::getName()
{
  return m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Type IGeometry::getGeometryType()
{
  return m_GeometryType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString IGeometry::getGeometryTypeAsString()
{
  return m_GeometryTypeName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IGeometry::sendThreadSafeProgressMessage(int64_t counter, int64_t max)
{
  m_Mutex.lock();

  m_ProgressCounter += counter;

  int64_t progIncrement = max / 100;
  int64_t prog = 1;

  if(m_ProgressCounter > prog)
  {
    int64_t progressInt = static_cast<int64_t>((static_cast<float>(m_ProgressCounter) / max) * 100.0f);
    QString ss = m_MessageTitle + QObject::tr(" || %1% Complete").arg(progressInt);
    notifyStatusMessage(m_MessagePrefix, m_MessageLabel, ss);
    prog += progIncrement;
  }

  m_Mutex.unlock();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IGeometry::setMessagePrefix(const QString& name)
{
  m_MessagePrefix = name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString IGeometry::getMessagePrefix()
{
  return m_MessagePrefix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IGeometry::setMessageTitle(const QString& title)
{
  m_MessageTitle = title;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString IGeometry::getMessageTitle()
{
  return m_MessageTitle;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IGeometry::setMessageLabel(const QString& label)
{
  m_MessageLabel = label;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString IGeometry::getMessageLabel()
{
  return m_MessageLabel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int IGeometry::getXdmfGridType()
{
  return m_XdmfGridType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int IGeometry::getUnitDimensionality()
{
  return m_UnitDimensionality;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IGeometry::setSpatialDimensionality(unsigned int spatialDims)
{
  m_SpatialDimensionality = spatialDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int IGeometry::getSpatialDimensionality()
{
  return m_SpatialDimensionality;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer IGeometry::getAttributeMatrix(const QString& name)
{
  AttributeMatrixMap_t::iterator it;
  it = m_AttributeMatrices.find(name);
  if(it == m_AttributeMatrices.end())
  {
    return AttributeMatrix::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer IGeometry::removeAttributeMatrix(const QString& name)
{
  QMap<QString, AttributeMatrix::Pointer>::iterator it;
  it = m_AttributeMatrices.find(name);
  if(it == m_AttributeMatrices.end())
  {
    // DO NOT return a NullPointer for any reason other than "Attribute Matrix was not found"
    return AttributeMatrix::NullPointer();
  }
  AttributeMatrix::Pointer p = it.value();
  m_AttributeMatrices.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int IGeometry::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf))
{
  herr_t err = 0;
  if(m_TransformContainer)
  {
    const char* transformGroup = SIMPL::Geometry::TransformContainerGroup.toLatin1().data();
    err = m_TransformContainer->writeTransformContainerToHDF5(parentId, transformGroup);
  }

  return err;
}

int IGeometry::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  std::string transformTypeName = SIMPL::Geometry::UnknownTransformContainer.toStdString();

  QString transformName = QString::number(0).toLatin1().data();
  err = H5Lite::readStringAttribute(parentId, transformName.toLatin1().data(), SIMPL::Geometry::TransformContainerTypeName.toLatin1().data(), transformTypeName);
  // No transform information found.
  if(err < 0)
  {
    return 0;
  }

  if(transformTypeName == SIMPL::Geometry::TransformContainer.toStdString())
  {
    TransformContainer::Pointer transformContainer = TransformContainer::New();
    err = transformContainer->readTransformContainerFromHDF5(parentId, preflight, transformName.toLatin1().data());
    if(err < 0)
    {
      return err;
    }
    setTransformContainer(transformContainer);
  }
  else if(transformTypeName == SIMPL::Geometry::CompositeTransformContainer.toStdString())
  {
    CompositeTransformContainer::Pointer compositeTransformContainer = CompositeTransformContainer::New();
    err = compositeTransformContainer->readTransformContainerFromHDF5(parentId, preflight, transformName.toLatin1().data());
    if(err < 0)
    {
      return err;
    }
    setTransformContainer(compositeTransformContainer);
  }
  else // SIMPL::Geometry::UnknownTransformContainer
  {
    return -1;
  }

  return err;
}
