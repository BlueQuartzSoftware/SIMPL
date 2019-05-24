/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
#include "DataArrayProxy.h"

#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayProxy::DataArrayProxy() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayProxy::DataArrayProxy(const QString& da_path, const QString& da_name, uint8_t da_flag, const QString& da_objectType, int da_version)
: m_Flag(da_flag)
, m_Version(da_version)
, m_Path(da_path)
, m_Name(da_name)
, m_ObjectType(da_objectType)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayProxy::DataArrayProxy(const DataArrayProxy& rhs)
{
  m_Flag = rhs.m_Flag;
  m_Version = rhs.m_Version;
  m_Path = rhs.m_Path;
  m_Name = rhs.m_Name;
  m_ObjectType = rhs.m_ObjectType;
  m_TupleDims = rhs.m_TupleDims;
  m_CompDims = rhs.m_CompDims;
}

// -----------------------------------------------------------------------------
DataArrayProxy::~DataArrayProxy() = default;

// -----------------------------------------------------------------------------
DataArrayProxy::DataArrayProxy(DataArrayProxy&& dap) noexcept = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayProxy::writeJson(QJsonObject& json) const
{
  json["Flag"] = static_cast<double>(m_Flag);
  json["Version"] = static_cast<double>(m_Version);
  json["Path"] = m_Path;
  json["Name"] = m_Name;
  json["Object Type"] = m_ObjectType;
  json["Tuple Dimensions"] = writeVector(m_TupleDims);
  json["Component Dimensions"] = writeVector(m_CompDims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayProxy::readJson(const QJsonObject& json)
{
  if(json["Flag"].isDouble() && json["Version"].isDouble() && json["Path"].isString() && json["Name"].isString() && json["Object Type"].isString() && json["Tuple Dimensions"].isArray() &&
     json["Component Dimensions"].isArray())
  {
    if(json["Flag"].toDouble() >= std::numeric_limits<uint8_t>::min() && json["Flag"].toDouble() <= std::numeric_limits<uint8_t>::max())
    {
      m_Flag = static_cast<uint8_t>(json["Flag"].toDouble());
    }
    m_Version = json["Version"].toInt();
    m_Path = json["Path"].toString();
    m_Name = json["Name"].toString();
    m_ObjectType = json["Object Type"].toString();
    m_TupleDims = readVector(json["Tuple Dimensions"].toArray());
    m_CompDims = readVector(json["Component Dimensions"].toArray());
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayProxy::ReadDataArrayStructure(hid_t attrMatGid, QMap<QString, DataArrayProxy>& dataArrays, SIMPLH5DataReaderRequirements* req, QString h5InternalPath)
{

  QList<QString> dataArrayNames;
  QH5Utilities::getGroupObjects(attrMatGid, H5Utilities::H5Support_DATASET | H5Utilities::H5Support_GROUP, dataArrayNames);
  foreach(QString dataArrayName, dataArrayNames)
  {
    DataArrayProxy proxy(h5InternalPath, dataArrayName, SIMPL::Unchecked);

    herr_t err = QH5Lite::readVectorAttribute(attrMatGid, dataArrayName, SIMPL::HDF5::TupleDimensions, proxy.m_TupleDims);
    if(err < 0)
    {
      std::cout << "Error Reading the Tuple Dimensions for DataArray " << dataArrayName.toStdString() << std::endl;
    }

    err = QH5Lite::readVectorAttribute(attrMatGid, dataArrayName, SIMPL::HDF5::ComponentDimensions, proxy.m_CompDims);
    if(err < 0)
    {
      std::cout << "Error Reading the Component Dimensions for DataArray " << dataArrayName.toStdString() << std::endl;
    }

    bool cDimsResult = false;
    if(req != nullptr)
    {
      QVector<QVector<size_t>> cDims = req->getComponentDimensions();
      if(cDims.empty() || cDims.contains(proxy.m_CompDims))
      {
        cDimsResult = true;
      }
    }

    err = QH5Lite::readScalarAttribute(attrMatGid, dataArrayName, SIMPL::HDF5::DataArrayVersion, proxy.m_Version);
    if(err < 0)
    {
      std::cout << "Error Reading the Version for DataArray " << dataArrayName.toStdString() << std::endl;
    }

    err = QH5Lite::readStringAttribute(attrMatGid, dataArrayName, SIMPL::HDF5::ObjectType, proxy.m_ObjectType);
    if(err < 0)
    {
      std::cout << "Error Reading the Object Type for DataArray " << dataArrayName.toStdString() << std::endl;
    }

    if(req != nullptr)
    {
      QVector<QString> daTypes = req->getDATypes();
      if((daTypes.empty() || daTypes.contains(proxy.m_ObjectType)) && cDimsResult)
      {
        proxy.m_Flag = Qt::Checked;
      }
    }

    dataArrays.insert(dataArrayName, proxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayProxy& DataArrayProxy::operator=(const DataArrayProxy& rhs) = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayProxy::operator==(const DataArrayProxy& rhs) const
{
  return m_Flag == rhs.m_Flag && m_Version == rhs.m_Version && m_Path == rhs.m_Path && m_Name == rhs.m_Name && m_ObjectType == rhs.m_ObjectType && m_TupleDims == rhs.m_TupleDims &&
         m_CompDims == rhs.m_CompDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonArray DataArrayProxy::writeVector(QVector<size_t> vector) const
{
  QJsonArray jsonArray;
  for(const auto& num : vector)
  {
    jsonArray.push_back(static_cast<double>(num));
  }
  return jsonArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> DataArrayProxy::readVector(QJsonArray jsonArray)
{
  QVector<size_t> vector;
  for(const auto& val : jsonArray)
  {
    if(val.isDouble())
    {
      if(val.toDouble() >= std::numeric_limits<size_t>::min() && val.toDouble() <= std::numeric_limits<size_t>::max())
      {
        vector.push_back(static_cast<size_t>(val.toDouble()));
      }
    }
  }
  return vector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayProxy::PrimitiveTypeFlag DataArrayProxy::PrimitiveTypeToFlag(const QString& pType)
{
  if(pType == SIMPL::Defaults::AnyPrimitive)
  {
    return Any_PType;
  }
  if(pType == SIMPL::TypeNames::Bool)
  {
    return Bool_PType;
  }
  if(pType == SIMPL::TypeNames::Double)
  {
    return Double_PType;
  }
  if(pType == SIMPL::TypeNames::Float)
  {
    return Float_PType;
  }
  if(pType == SIMPL::TypeNames::Int8)
  {
    return Int8_PType;
  }
  if(pType == SIMPL::TypeNames::Int16)
  {
    return Int16_PType;
  }
  if(pType == SIMPL::TypeNames::Int32)
  {
    return Int32_PType;
  }
  if(pType == SIMPL::TypeNames::Int64)
  {
    return Int64_PType;
  }
  if(pType == SIMPL::TypeNames::NeighborList)
  {
    return NeighborList_PType;
  }
  if(pType == SIMPL::TypeNames::StatsDataArray)
  {
    return StatsDataArray_PType;
  }
  if(pType == SIMPL::TypeNames::String)
  {
    return StringArray_PType;
  }
  if(pType == SIMPL::TypeNames::UInt8)
  {
    return UInt8_PType;
  }
  if(pType == SIMPL::TypeNames::UInt16)
  {
    return UInt16_PType;
  }
  if(pType == SIMPL::TypeNames::UInt32)
  {
    return UInt32_PType;
  }
  if(pType == SIMPL::TypeNames::UInt64)
  {
    return UInt64_PType;
  }
  if(pType == SIMPL::TypeNames::Unknown)
  {
    return Unknown_PType;
  }

  return None_PType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayProxy::updatePath(DataArrayPath::RenameType renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  if(oldPath.getDataArrayName().isEmpty())
  {
    return;
  }

  if(oldPath.getDataArrayName() != newPath.getDataArrayName())
  {
    m_Name = newPath.getDataArrayName();
    m_Path = newPath.serialize();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint8_t DataArrayProxy::getFlag() const
{
  return m_Flag;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayProxy::setFlag(uint8_t newFlag)
{
  m_Flag = newFlag;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayProxy::setName(const QString& name)
{
  m_Name = name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayProxy::getName() const
{
  return m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayProxy::setPath(const QString& objPath)
{
  this->m_Path = objPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayProxy::getPath() const
{
  return m_Path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayProxy::setObjectType(const QString& objType)
{
  this->m_ObjectType = objType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayProxy::getObjectType() const
{
  return m_ObjectType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayProxy::setTupleDims(const QVector<size_t>& tDims)
{
  m_TupleDims = tDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> DataArrayProxy::getTupleDims() const
{
  return m_TupleDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayProxy::setCompDims(const QVector<size_t>& cDims)
{
  m_CompDims = cDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> DataArrayProxy::getCompDims() const
{
  return m_CompDims;
}
