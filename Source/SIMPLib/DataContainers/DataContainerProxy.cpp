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

#include "DataContainerProxy.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerProxy::DataContainerProxy() :
  flag(Qt::Unchecked),
  name(""),
  dcType(static_cast<unsigned int>(IGeometry::Type::Any))
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerProxy::DataContainerProxy(const QString& dc_name, const uint8_t& read_dc, IGeometry::Type dc_type) :
  flag(read_dc),
  name(dc_name),
  dcType(static_cast<unsigned int>(dc_type))
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerProxy::DataContainerProxy(const DataContainerProxy& amp)
{
  flag = amp.flag;
  name = amp.name;
  dcType = amp.dcType;
  attributeMatricies = amp.attributeMatricies;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerProxy& DataContainerProxy::operator=(const DataContainerProxy& amp) = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerProxy::operator==(const DataContainerProxy& amp) const
{
  return flag == amp.flag && name == amp.name && dcType == amp.dcType && attributeMatricies == amp.attributeMatricies;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerProxy::updatePath(DataArrayPath::RenameType renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  if(oldPath.getDataContainerName() != newPath.getDataContainerName())
  {
    name = newPath.getDataContainerName();
  }

  if(attributeMatricies.contains(oldPath.getAttributeMatrixName()))
  {
    AttributeMatrixProxy amProxy = attributeMatricies.take(oldPath.getAttributeMatrixName());
    amProxy.updatePath(renamePath);
    attributeMatricies.insert(newPath.getAttributeMatrixName(), amProxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerProxy::writeJson(QJsonObject& json)
{
  json["Flag"] = static_cast<double>(flag);
  json["Name"] = name;
  json["Type"] = static_cast<double>(dcType);
  json["Attribute Matricies"] = writeMap(attributeMatricies);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerProxy::readJson(QJsonObject& json)
{
  if (json["Flag"].isDouble() && json["Name"].isString() && json["Type"].isDouble() && json["Attribute Matricies"].isArray())
  {
    if (json["Flag"].toDouble() >= std::numeric_limits<uint8_t>::min() && json["Flag"].toDouble() <= std::numeric_limits<uint8_t>::max())
    {
      flag = static_cast<uint8_t>(json["Flag"].toDouble());
    }
    name = json["Name"].toString();
    if (json["Type"].toDouble() >= std::numeric_limits<unsigned int>::min() && json["Type"].toDouble() <= std::numeric_limits<unsigned int>::max())
    {
      dcType = static_cast<unsigned int>(json["Type"].toDouble());
    }
    attributeMatricies = readMap(json["Attribute Matricies"].toArray());
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonArray DataContainerProxy::writeMap(QMap<QString, AttributeMatrixProxy> map)
{
  QJsonArray amArray;
  for (QMap<QString, AttributeMatrixProxy>::iterator iter = map.begin(); iter != map.end(); ++iter)
  {
    QJsonObject obj;
    (*iter).writeJson(obj);
    amArray.push_back(obj);
  }
  return amArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, AttributeMatrixProxy> DataContainerProxy::readMap(QJsonArray jsonArray)
{
  QMap<QString, AttributeMatrixProxy> map;
  foreach(QJsonValue val, jsonArray)
  {
    if (val.isObject())
    {
      AttributeMatrixProxy am;
      QJsonObject obj = val.toObject();
      am.readJson(obj);
      map.insert(am.name, am);
    }
  }
  return map;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerProxy::DCGeometryTypeFlag DataContainerProxy::GeometryTypeToFlag(IGeometry::Type geoType)
{
  switch (geoType)
  {
    case IGeometry::Type::Any:
    {
      return Any_DCGeomType;
    }
    case IGeometry::Type::Edge:
    {
      return Edge_DCGeomType;
    }
    case IGeometry::Type::Image:
    {
      return Image_DCGeomType;
    }
    case IGeometry::Type::Quad:
    {
      return Quad_DCGeomType;
    }
    case IGeometry::Type::RectGrid:
    {
      return RectGrid_DCGeomType;
    }
    case IGeometry::Type::Tetrahedral:
    {
      return Tetrahedral_DCGeomType;
    }
    case IGeometry::Type::Triangle:
    {
      return Triangle_DCGeomType;
    }
    case IGeometry::Type::Vertex:
    {
      return Vertex_DCGeomType;
    }
    case IGeometry::Type::Hexahedral:
    {
      return Hexahedral_DCGeomType;
    }
    case IGeometry::Type::Unknown:
    {
      return Unknown_DCGeomType;
    }
  }

  return Unknown_DCGeomType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerProxy::setFlags(uint8_t flag, AttributeMatrixProxy::AMTypeFlags amTypes, DataArrayProxy::PrimitiveTypeFlags primitiveTypes, const DataArrayProxy::CompDimsVector& compDimsVector)
{
  this->flag = flag;

  for(QMap<QString, AttributeMatrixProxy>::iterator amIter = attributeMatricies.begin(); amIter != attributeMatricies.end(); ++amIter) // AttributeMatrix Level
  {
    AttributeMatrixProxy& amProxy = amIter.value();
    AttributeMatrixProxy::AMTypeFlag amTypeFlag = AttributeMatrixProxy::AttributeMatrixTypeToFlag(static_cast<AttributeMatrix::Type>(amProxy.amType));
    if ((amTypes & amTypeFlag) > 0 || amTypes == AttributeMatrixProxy::AMTypeFlag::Any_AMType)
    {
      amProxy.setFlags(flag, primitiveTypes, compDimsVector);
    }
  }
}
