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
DataArrayProxy::DataArrayProxy()
: flag(SIMPL::Unchecked)
, version(0)
, path("")
, name("")
, objectType("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayProxy::DataArrayProxy(QString da_path, QString da_name, uint8_t da_flag, QString da_objectType, int da_version)
: flag(da_flag)
, version(da_version)
, path(da_path)
, name(da_name)
, objectType(da_objectType)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayProxy::DataArrayProxy(const DataArrayProxy& rhs)
{
  flag = rhs.flag;
  version = rhs.version;
  path = rhs.path;
  name = rhs.name;
  objectType = rhs.objectType;
  tupleDims = rhs.tupleDims;
  compDims = rhs.compDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayProxy::writeJson(QJsonObject& json)
{
  json["Flag"] = static_cast<double>(flag);
  json["Version"] = static_cast<double>(version);
  json["Path"] = path;
  json["Name"] = name;
  json["Object Type"] = objectType;
  json["Tuple Dimensions"] = writeVector(tupleDims);
  json["Component Dimensions"] = writeVector(compDims);
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
      flag = static_cast<uint8_t>(json["Flag"].toDouble());
    }
    version = json["Version"].toInt();
    path = json["Path"].toString();
    name = json["Name"].toString();
    objectType = json["Object Type"].toString();
    tupleDims = readVector(json["Tuple Dimensions"].toArray());
    compDims = readVector(json["Component Dimensions"].toArray());
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
    if(__SHOW_DEBUG_MSG__)
    {
      std::cout << "        DataArray: " << dataArrayName.toStdString() << std::endl;
    }

    DataArrayProxy proxy(h5InternalPath, dataArrayName, SIMPL::Unchecked);

    herr_t err = QH5Lite::readVectorAttribute(attrMatGid, dataArrayName, SIMPL::HDF5::TupleDimensions, proxy.tupleDims);
    if(err < 0)
    {
      std::cout << "Error Reading the Tuple Dimensions for DataArray " << dataArrayName.toStdString() << std::endl;
    }

    err = QH5Lite::readVectorAttribute(attrMatGid, dataArrayName, SIMPL::HDF5::ComponentDimensions, proxy.compDims);
    if(err < 0)
    {
      std::cout << "Error Reading the Component Dimensions for DataArray " << dataArrayName.toStdString() << std::endl;
    }

    bool cDimsResult = false;
    if (req != nullptr)
    {
      QVector<QVector<size_t>> cDims = req->getComponentDimensions();
      if(cDims.empty() || cDims.contains(proxy.compDims))
      {
        cDimsResult = true;
      }
    }

    err = QH5Lite::readScalarAttribute(attrMatGid, dataArrayName, SIMPL::HDF5::DataArrayVersion, proxy.version);
    if(err < 0)
    {
      std::cout << "Error Reading the Version for DataArray " << dataArrayName.toStdString() << std::endl;
    }

    err = QH5Lite::readStringAttribute(attrMatGid, dataArrayName, SIMPL::HDF5::ObjectType, proxy.objectType);
    if(err < 0)
    {
      std::cout << "Error Reading the Object Type for DataArray " << dataArrayName.toStdString() << std::endl;
    }

    if (req != nullptr)
    {
      QVector<QString> daTypes = req->getDATypes();
      if((daTypes.empty() || daTypes.contains(proxy.objectType)) && cDimsResult)
      {
        proxy.flag = Qt::Checked;
      }
    }

    dataArrays.insert(dataArrayName, proxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayProxy::operator=(const DataArrayProxy& rhs)
{
  flag = rhs.flag;
  version = rhs.version;
  path = rhs.path;
  name = rhs.name;
  objectType = rhs.objectType;
  tupleDims = rhs.tupleDims;
  compDims = rhs.compDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayProxy::operator==(const DataArrayProxy& rhs) const
{
  return flag == rhs.flag && version == rhs.version && path == rhs.path && name == rhs.name && objectType == rhs.objectType && tupleDims == rhs.tupleDims && compDims == rhs.compDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonArray DataArrayProxy::writeVector(QVector<size_t> vector)
{
  QJsonArray jsonArray;
  foreach(size_t num, vector)
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
  foreach(QJsonValue val, jsonArray)
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
DataArrayProxy::PrimitiveTypeFlag DataArrayProxy::PrimitiveTypeToFlag(const QString &pType)
{
  if (pType == SIMPL::Defaults::AnyPrimitive)
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
  else if (pType == SIMPL::TypeNames::Int8)
  {
    return Int8_PType;
  }
  else if (pType == SIMPL::TypeNames::Int16)
  {
    return Int16_PType;
  }
  else if (pType == SIMPL::TypeNames::Int32)
  {
    return Int32_PType;
  }
  else if (pType == SIMPL::TypeNames::Int64)
  {
    return Int64_PType;
  }
  else if (pType == SIMPL::TypeNames::NeighborList)
  {
    return NeighborList_PType;
  }
  else if (pType == SIMPL::TypeNames::StatsDataArray)
  {
    return StatsDataArray_PType;
  }
  else if (pType == SIMPL::TypeNames::String)
  {
    return StringArray_PType;
  }
  else if (pType == SIMPL::TypeNames::UInt8)
  {
    return UInt8_PType;
  }
  else if (pType == SIMPL::TypeNames::UInt16)
  {
    return UInt16_PType;
  }
  else if (pType == SIMPL::TypeNames::UInt32)
  {
    return UInt32_PType;
  }
  else if (pType == SIMPL::TypeNames::UInt64)
  {
    return UInt64_PType;
  }
  else if (pType == SIMPL::TypeNames::Unknown)
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
    name = newPath.getDataArrayName();
    path = newPath.serialize();
  }
}
