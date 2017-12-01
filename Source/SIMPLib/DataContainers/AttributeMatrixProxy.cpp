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
AttributeMatrixProxy::AttributeMatrixProxy()
: flag(0)
, name("")
, amType(AttributeMatrix::Type::Unknown)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy::AttributeMatrixProxy(QString am_name, uint8_t read_am , AttributeMatrix::Type am_type)
: flag(read_am)
, name(am_name)
, amType(am_type)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy::AttributeMatrixProxy(const AttributeMatrixProxy& amp)
{
  flag = amp.flag;
  name = amp.name;
  amType = amp.amType;
  dataArrays = amp.dataArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixProxy::~AttributeMatrixProxy() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixProxy::operator=(const AttributeMatrixProxy& amp)
{
  flag = amp.flag;
  name = amp.name;
  amType = amp.amType;
  dataArrays = amp.dataArrays;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrixProxy::operator==(const AttributeMatrixProxy& amp) const
{
  if(flag == amp.flag && name == amp.name && amType == amp.amType && dataArrays == amp.dataArrays)
  {
    return true;
  }

  return false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixProxy::writeJson(QJsonObject& json)
{
  json["Flag"] = static_cast<double>(flag);
  json["Name"] = name;
  json["Type"] = static_cast<double>(amType);
  json["Data Arrays"] = writeMap(dataArrays);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrixProxy::readJson(QJsonObject& json)
{
  if(json["Flag"].isDouble() && json["Name"].isString() && json["Type"].isDouble() && json["Data Arrays"].isArray())
  {
    if(json["Flag"].toDouble() >= std::numeric_limits<uint8_t>().min() && json["Flag"].toDouble() <= std::numeric_limits<uint8_t>().max())
    {
      flag = static_cast<uint8_t>(json["Flag"].toDouble());
    }
    name = json["Name"].toString();
    if(json["Type"].toDouble() >= std::numeric_limits<unsigned int>().min() && json["Type"].toDouble() <= std::numeric_limits<unsigned int>().max())
    {
      amType = static_cast<AttributeMatrix::Type>(json["Type"].toInt());
    }
    dataArrays = readMap(json["Data Arrays"].toArray());
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonArray AttributeMatrixProxy::writeMap(QMap<QString, DataArrayProxy> map)
{
  QJsonArray daArray;
  for(QMap<QString, DataArrayProxy>::iterator iter = map.begin(); iter != map.end(); ++iter)
  {
    QJsonObject obj;
    (*iter).writeJson(obj);
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
  foreach(QJsonValue val, jsonArray)
  {
    if(val.isObject())
    {
      DataArrayProxy da;
      da.readJson(val.toObject());
      map.insert(da.name, da);
    }
  }
  return map;
}
