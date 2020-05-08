/* ============================================================================
 * Copyright (c) 2016 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "AttributeMatrixProxy.h"

#include "SIMPLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy::AttributeMatrixProxy() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy::AttributeMatrixProxy(const QString& am_name, uint8_t read_am, AttributeMatrix::Type am_type)
: m_Flag(read_am)
, m_Name(am_name)
, m_AMType(am_type)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy::AttributeMatrixProxy(const AttributeMatrixProxy& amp)
{
  m_Flag = amp.m_Flag;
  m_Name = amp.m_Name;
  m_AMType = amp.m_AMType;
  m_DataArrays = amp.m_DataArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy::~AttributeMatrixProxy() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy& AttributeMatrixProxy::operator=(const AttributeMatrixProxy& amp) = default;

// -----------------------------------------------------------------------------
AttributeMatrixProxy::AttributeMatrixProxy(AttributeMatrixProxy&&) noexcept = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrixProxy::operator==(const AttributeMatrixProxy& amp) const
{
  return m_Flag == amp.m_Flag && m_Name == amp.m_Name && m_AMType == amp.m_AMType && m_DataArrays == amp.m_DataArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixProxy::writeJson(QJsonObject& json) const
{
  json["Flag"] = static_cast<double>(m_Flag);
  json["Name"] = m_Name;
  json["Type"] = static_cast<double>(m_AMType);
  json["Data Arrays"] = writeMap(m_DataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrixProxy::readJson(QJsonObject& json)
{
  if(json["Flag"].isDouble() && json["Name"].isString() && json["Type"].isDouble() && json["Data Arrays"].isArray())
  {
    if(json["Flag"].toDouble() >= std::numeric_limits<uint8_t>::min() && json["Flag"].toDouble() <= std::numeric_limits<uint8_t>::max())
    {
      m_Flag = static_cast<uint8_t>(json["Flag"].toDouble());
    }
    m_Name = json["Name"].toString();
    if(json["Type"].toDouble() >= std::numeric_limits<unsigned int>::min() && json["Type"].toDouble() <= std::numeric_limits<unsigned int>::max())
    {
      m_AMType = static_cast<AttributeMatrix::Type>(json["Type"].toInt());
    }
    m_DataArrays = readMap(json["Data Arrays"].toArray());
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonArray AttributeMatrixProxy::writeMap(const QMap<QString, DataArrayProxy>& map) const
{
  QJsonArray daArray;
  for(const DataArrayProxy& iter : map)
  {
    QJsonObject obj;
    iter.writeJson(obj);
    daArray.push_back(obj);
  }
  return daArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, DataArrayProxy> AttributeMatrixProxy::readMap(QJsonArray jsonArray)
{
  QMap<QString, DataArrayProxy> map;
  for(const auto& val : jsonArray)
  {
    if(val.isObject())
    {
      DataArrayProxy da;
      da.readJson(val.toObject());
      map.insert(da.getName(), da);
    }
  }
  return map;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy::AMTypeFlag AttributeMatrixProxy::AttributeMatrixTypeToFlag(AttributeMatrix::Type amType)
{
  switch(amType)
  {
  case AttributeMatrix::Type::Any: {
    return Any_AMType;
  }
  case AttributeMatrix::Type::Cell: {
    return Cell_AMType;
  }
  case AttributeMatrix::Type::CellEnsemble: {
    return CellEnsemble_AMType;
  }
  case AttributeMatrix::Type::CellFeature: {
    return CellFeature_AMType;
  }
  case AttributeMatrix::Type::Edge: {
    return Edge_AMType;
  }
  case AttributeMatrix::Type::EdgeEnsemble: {
    return EdgeEnsemble_AMType;
  }
  case AttributeMatrix::Type::EdgeFeature: {
    return EdgeFeature_AMType;
  }
  case AttributeMatrix::Type::Face: {
    return Face_AMType;
  }
  case AttributeMatrix::Type::FaceEnsemble: {
    return FaceEnsemble_AMType;
  }
  case AttributeMatrix::Type::FaceFeature: {
    return FaceFeature_AMType;
  }
  case AttributeMatrix::Type::Generic: {
    return Generic_AMType;
  }
  case AttributeMatrix::Type::MetaData: {
    return MetaData_AMType;
  }
  case AttributeMatrix::Type::Vertex: {
    return Vertex_AMType;
  }
  case AttributeMatrix::Type::VertexEnsemble: {
    return VertexEnsemble_AMType;
  }
  case AttributeMatrix::Type::VertexFeature: {
    return VertexFeature_AMType;
  }
  case AttributeMatrix::Type::Unknown: {
    return Unknown_AMType;
  }
  }

  return Unknown_AMType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixProxy::setFlags(uint8_t flag, DataArrayProxy::PrimitiveTypeFlags primitiveTypes, DataArrayProxy::CompDimsVector compDimsVector)
{
  this->m_Flag = flag;

  for(QMap<QString, DataArrayProxy>::iterator daIter = m_DataArrays.begin(); daIter != m_DataArrays.end(); ++daIter) // DataArray Level
  {
    DataArrayProxy& daProxy = daIter.value();
    DataArrayProxy::PrimitiveTypeFlag pTypeFlag = DataArrayProxy::PrimitiveTypeToFlag(daProxy.getObjectType());
    if((primitiveTypes & pTypeFlag) > 0 || primitiveTypes == DataArrayProxy::PrimitiveTypeFlag::Any_PType)
    {
      if(compDimsVector.empty())
      {
        daProxy.setFlag(flag);
      }
      else
      {
        for(const auto& compDim : compDimsVector)
        {
          if(compDim == daProxy.getCompDims())
          {
            daProxy.setFlag(flag);
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixProxy::updatePath(DataArrayPath::RenameType renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  if(oldPath.getAttributeMatrixName() != newPath.getAttributeMatrixName())
  {
    m_Name = newPath.getAttributeMatrixName();
  }

  if(m_DataArrays.contains(oldPath.getDataArrayName()))
  {
    DataArrayProxy daProxy = m_DataArrays.take(oldPath.getDataArrayName());
    daProxy.updatePath(renamePath);
    m_DataArrays.insert(newPath.getDataArrayName(), daProxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayProxy& AttributeMatrixProxy::getDataArrayProxy(const QString& name)
{
  for(QMap<QString, DataArrayProxy>::iterator iter = m_DataArrays.begin(); iter != m_DataArrays.end(); ++iter) // DataArray Level
  {
    DataArrayProxy& dataArray = iter.value();
    if(dataArray.getName() == name)
    {
      return dataArray;
    }
  }

  DataArrayProxy proxy;
  proxy.setName(name);
  m_DataArrays.insert(proxy.getName(), proxy);
  return m_DataArrays[name];
}

// -----------------------------------------------------------------------------
void AttributeMatrixProxy::insertDataArray(const QString& name, const DataArrayProxy& proxy)
{
  m_DataArrays.insert(name, proxy);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy::StorageType& AttributeMatrixProxy::getDataArrays()
{
  return m_DataArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const AttributeMatrixProxy::StorageType& AttributeMatrixProxy::getDataArrays() const
{
  return m_DataArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixProxy::setDataArrays(const QMap<QString, DataArrayProxy>& proxies)
{
  m_DataArrays = proxies;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrixProxy::getName() const
{
  return m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixProxy::setName(const QString& newName)
{
  m_Name = newName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Type AttributeMatrixProxy::getAMType() const
{
  return m_AMType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixProxy::setAMType(AttributeMatrix::Type newType)
{
  m_AMType = newType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint8_t AttributeMatrixProxy::getFlag() const
{
  return m_Flag;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixProxy::setFlag(uint8_t newFlag)
{
  m_Flag = newFlag;
}
