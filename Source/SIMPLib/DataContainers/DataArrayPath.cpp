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

#include "DataArrayPath.h"

#include <type_traits>

#include <QtCore/QJsonObject>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

namespace
{
template <class Container>
bool ValidateVectorImpl(const Container& other)
{
  static_assert(std::is_same_v<typename Container::value_type, DataArrayPath>);

  using size_type = typename Container::size_type;

  if(other.size() <= 1)
  {
    return true;
  }

  QString dcName = other.front().getDataContainerName();
  QString amName = other.front().getAttributeMatrixName();

  for(size_type i = 1; i < other.size(); i++)
  {
    if(other.at(i).getDataContainerName() != dcName || other.at(i).getAttributeMatrixName() != amName)
    {
      return false;
    }
  }

  return true;
}

template <class OutputContainer, class InputContainer>
OutputContainer GetDataArrayNamesImpl(const InputContainer& paths)
{
  static_assert(std::is_same_v<typename InputContainer::value_type, DataArrayPath>);
  static_assert(std::is_same_v<typename OutputContainer::value_type, QString>);

  OutputContainer arrayNames;
  arrayNames.reserve(paths.size());
  for(const DataArrayPath& path : paths)
  {
    arrayNames.push_back(path.getDataArrayName());
  }

  return arrayNames;
}

template <class Container>
DataArrayPath GetAttributeMatrixPathImpl(const Container& paths)
{
  static_assert(std::is_same_v<typename Container::value_type, DataArrayPath>);

  if(paths.empty())
  {
    return DataArrayPath();
  }

  return DataArrayPath(paths.front().getDataContainerName(), paths.front().getAttributeMatrixName(), "");
}
} // namespace

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::HashType DataArrayPath::GetHash(const QString& name)
{
  if(name.isEmpty())
  {
    return 0;
  }

  std::hash<std::string> hashFn;
  return hashFn(name.toStdString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDebug operator<<(QDebug out, const DataArrayPath& v)
{
  out << v.getDataContainerName() << "|" << v.getAttributeMatrixName() << "|" << v.getDataArrayName();
  return out;
}

/* ############## Start API/Public Implementation ########################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath(const QString& dcName, const QString& amName, const QString& daName)
: m_DataContainerName(dcName)
, m_AttributeMatrixName(amName)
, m_DataArrayName(daName)
, m_DataContainerHash(GetHash(dcName))
, m_AttributeMatrixHash(GetHash(amName))
, m_DataArrayHash(GetHash(daName))
{
  updateDataType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath(const QString& path)
{
  QStringList tokens = path.split(SIMPL::PathSep);

  if(!tokens.empty())
  {
    setDataContainerName(tokens.at(0));
  }
  if(tokens.size() > 1)
  {
    setAttributeMatrixName(tokens.at(1));
  }
  if(tokens.size() > 2)
  {
    setDataArrayName(tokens.at(2));
  }

  updateDataType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath(const DataArrayPath& rhs)
{
  m_DataContainerName = rhs.m_DataContainerName;
  m_AttributeMatrixName = rhs.m_AttributeMatrixName;
  m_DataArrayName = rhs.m_DataArrayName;
  m_DataContainerHash = rhs.m_DataContainerHash;
  m_AttributeMatrixHash = rhs.m_AttributeMatrixHash;
  m_DataArrayHash = rhs.m_DataArrayHash;

  updateDataType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::~DataArrayPath() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<DataArrayPath> DataArrayPath::ConvertToQVector(QStringList& paths)
{
  QVector<DataArrayPath> vPaths;
  for(const QString sPath : paths)
  {
    DataArrayPath p(sPath);
    vPaths.push_back(p);
  }
  return vPaths;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath& DataArrayPath::operator=(const DataArrayPath& rhs)
{
  m_DataContainerName = rhs.m_DataContainerName;
  m_AttributeMatrixName = rhs.m_AttributeMatrixName;
  m_DataArrayName = rhs.m_DataArrayName;
  m_DataContainerHash = rhs.m_DataContainerHash;
  m_AttributeMatrixHash = rhs.m_AttributeMatrixHash;
  m_DataArrayHash = rhs.m_DataArrayHash;

  updateDataType();
  return *this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::operator==(const DataArrayPath& rhs) const
{
  return m_DataContainerHash == rhs.m_DataContainerHash && m_AttributeMatrixHash == rhs.m_AttributeMatrixHash && m_DataArrayHash == rhs.m_DataArrayHash;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::operator!=(const DataArrayPath& rhs) const
{
  bool b = (*this == rhs);
  return !b;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::operator<(const DataArrayPath& rhs) const
{
  return serialize() < rhs.serialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayPath::serialize(const QString& delimiter) const
{
  QString s = "";
  if(!m_DataContainerName.isEmpty())
  {
    s = m_DataContainerName;
    if(!m_AttributeMatrixName.isEmpty())
    {
      s = s.append(delimiter);
      s = s.append(m_AttributeMatrixName);
      if(!m_DataArrayName.isEmpty())
      {
        s = s.append(delimiter);
        s = s.append(m_DataArrayName);
      }
    }
  }

  return s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataArrayPath::Deserialize(const QString& str, const QString& delimiter)
{
  if(str.isEmpty())
  {
    return DataArrayPath();
  }

  int start = 0;
  QStringList daParts;
  int tokenIndex = 0;

  while(tokenIndex >= 0)
  {
    tokenIndex = str.indexOf(delimiter, start);
    QString part = str.mid(start, tokenIndex - start);
    daParts.push_back(part);
    start = tokenIndex + delimiter.size();
  }

  DataArrayPath path;
  switch(daParts.size())
  {
  case 3:
    path.setDataArrayName(daParts[2]);
  case 2:
    path.setAttributeMatrixName(daParts[1]);
  case 1:
    path.setDataContainerName(daParts[0]);
    break;
  }

  path.updateDataType();
  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::setDataContainerName(const QString& name)
{
  m_DataContainerName = name;
  m_DataContainerHash = GetHash(m_DataContainerName);
  updateDataType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::setAttributeMatrixName(const QString& name)
{
  m_AttributeMatrixName = name;
  m_AttributeMatrixHash = GetHash(m_AttributeMatrixName);
  updateDataType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::setDataArrayName(const QString& name)
{
  m_DataArrayName = name;
  m_DataArrayHash = GetHash(m_DataArrayName);
  updateDataType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::updateDataType()
{
  const bool dcEmpty = m_DataContainerName.isEmpty();
  const bool amEmpty = m_AttributeMatrixName.isEmpty();
  const bool daEmpty = m_DataArrayName.isEmpty();

  if(dcEmpty)
  {
    m_DataType = DataArrayPathHelper::DataType::None;
    return;
  }
  if(amEmpty)
  {
    if(daEmpty)
    {
      m_DataType = DataArrayPathHelper::DataType::DataContainer;
      return;
    }
  }
  else
  {
    if(daEmpty)
    {
      m_DataType = DataArrayPathHelper::DataType::AttributeMatrix;
      return;
    }
    if(!amEmpty)
    {
      m_DataType = DataArrayPathHelper::DataType::DataArray;
      return;
    }
  }

  m_DataType = DataArrayPathHelper::DataType::None;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> DataArrayPath::toQVector() const
{
  QVector<QString> v(3);
  v[0] = getDataContainerName();
  v[1] = getAttributeMatrixName();
  v[2] = getDataArrayName();
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DataArrayPath::GetDataArrayNames(const QVector<DataArrayPath>& paths)
{
  return GetDataArrayNamesImpl<QList<QString>>(paths);
}

// -----------------------------------------------------------------------------
std::vector<QString> DataArrayPath::GetDataArrayNames(const std::vector<DataArrayPath>& paths)
{
  return GetDataArrayNamesImpl<std::vector<QString>>(paths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataArrayPath::GetAttributeMatrixPath(const QVector<DataArrayPath>& paths)
{
  return GetAttributeMatrixPathImpl(paths);
}

// -----------------------------------------------------------------------------
DataArrayPath DataArrayPath::GetAttributeMatrixPath(const std::vector<DataArrayPath>& paths)
{
  return GetAttributeMatrixPathImpl(paths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataArrayPath::toQStringList() const
{
  QStringList l;
  l << getDataContainerName() << getAttributeMatrixName() << getDataArrayName();
  return l;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::update(const QString& dcName, const QString& amName, const QString& daName)
{
  setDataContainerName(dcName);
  setAttributeMatrixName(amName);
  setDataArrayName(daName);
  updateDataType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::hasSameDataContainer(const DataArrayPath& other) const
{
  return m_DataContainerHash == other.m_DataContainerHash;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::hasSameAttributeMatrix(const DataArrayPath& other) const
{
  return m_AttributeMatrixHash == other.m_AttributeMatrixHash;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::hasSameDataArray(const DataArrayPath& other) const
{
  return m_DataArrayHash == other.m_DataArrayHash;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::hasSameAttributeMatrixPath(const DataArrayPath& other) const
{
  return (hasSameDataContainer(other) && hasSameAttributeMatrix(other));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::isSubset(const DataArrayPath& other) const
{
  // Check types
  const DataArrayPathHelper::DataType type = getDataType();
  const DataArrayPathHelper::DataType otherType = other.getDataType();

  // If both are of the same DataType, require identical paths
  if(type == otherType)
  {
    return (*this) == other;
  }

  switch(otherType)
  {
  case DataArrayPathHelper::DataType::DataArray:
    // [[fallthrough]];
  case DataArrayPathHelper::DataType::AttributeMatrix:
    if(type == DataArrayPathHelper::DataType::DataArray)
    {
      return false;
    }
    // [[fallthrough]];
  case DataArrayPathHelper::DataType::DataContainer:
    if(type == DataArrayPathHelper::DataType::AttributeMatrix || type == DataArrayPathHelper::DataType::DataArray)
    {
      return false;
    }
    break;
  case DataArrayPathHelper::DataType::None:
    if(type != DataArrayPathHelper::DataType::None)
    {
      return false;
    }
  }

  bool valid = true;
  switch(type)
  {
  case DataArrayPathHelper::DataType::DataArray:
    valid &= m_DataArrayHash == other.m_DataArrayHash;
    // [[fallthrough]];
  case DataArrayPathHelper::DataType::AttributeMatrix:
    valid &= m_AttributeMatrixHash == other.m_AttributeMatrixHash;
    // [[fallthrough]];
  case DataArrayPathHelper::DataType::DataContainer:
    valid &= m_DataContainerHash == other.m_DataContainerHash;
    break;
  case DataArrayPathHelper::DataType::None:
    break;
  }

  return valid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::ValidateVector(const QVector<DataArrayPath>& other)
{
  return ValidateVectorImpl(other);
}

// -----------------------------------------------------------------------------
bool DataArrayPath::ValidateVector(const std::vector<DataArrayPath>& other)
{
  return ValidateVectorImpl(other);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject DataArrayPath::toJsonObject() const
{
  QJsonObject json;
  json["Data Container Name"] = getDataContainerName();
  json["Attribute Matrix Name"] = getAttributeMatrixName();
  json["Data Array Name"] = getDataArrayName();
  return json;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::writeJson(QJsonObject& json) const
{
  json["Data Container Name"] = getDataContainerName();
  json["Attribute Matrix Name"] = getAttributeMatrixName();
  json["Data Array Name"] = getDataArrayName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::readJson(QJsonObject& json)
{
  if(json["Data Container Name"].isString() && json["Attribute Matrix Name"].isString() && json["Data Array Name"].isString())
  {
    setDataContainerName(json["Data Container Name"].toString());
    setAttributeMatrixName(json["Attribute Matrix Name"].toString());
    setDataArrayName(json["Data Array Name"].toString());
    return true;
  }
  return false;
}
