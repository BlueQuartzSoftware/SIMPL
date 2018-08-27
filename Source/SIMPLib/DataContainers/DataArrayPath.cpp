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

#include <QtCore/QJsonObject>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataContainers/DataContainerArray.h"

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
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath(const QString& path)
{
  QStringList tokens = path.split(SIMPL::PathSep);

  if(tokens.size() > 0)
  {
    m_DataContainerName = tokens.at(0);
  }
  if(tokens.size() > 1)
  {
    m_AttributeMatrixName = tokens.at(1);
  }
  if(tokens.size() > 2)
  {
    m_DataArrayName = tokens.at(2);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath(const DataArrayPath& rhs)
{
  m_DataContainerName = rhs.getDataContainerName();
  m_AttributeMatrixName = rhs.getAttributeMatrixName();
  m_DataArrayName = rhs.getDataArrayName();
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
  foreach(const QString sPath, paths)
  {
    DataArrayPath p(sPath);
    vPaths.push_back(p);
  }
  return vPaths;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::operator=(const DataArrayPath& rhs)
{
  m_DataContainerName = rhs.getDataContainerName();
  m_AttributeMatrixName = rhs.getAttributeMatrixName();
  m_DataArrayName = rhs.getDataArrayName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::operator==(const DataArrayPath& rhs) const
{
  if(m_DataContainerName == rhs.getDataContainerName() && m_AttributeMatrixName == rhs.getAttributeMatrixName() && m_DataArrayName == rhs.getDataArrayName())
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayPath::serialize(QString delimiter) const
{
  QString s = "";
  if(m_DataContainerName.isEmpty() == false)
  {
    s = m_DataContainerName;
    if(m_AttributeMatrixName.isEmpty() == false)
    {
      s = s.append(delimiter);
      s = s.append(m_AttributeMatrixName);
      if(m_DataArrayName.isEmpty() == false)
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
DataArrayPath DataArrayPath::Deserialize(QString str, QString delimiter)
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

  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::CheckRenamePath(DataContainerArrayShPtr oldDca, DataContainerArrayShPtr newDca, DataArrayPath oldPath, DataArrayPath newPath)
{
  // If the paths are not possible renames, return false
  if(false == oldPath.possibleRename(newPath))
  {
    return false;
  }

  // Check that the DataArrayPath targets are compatibles
  DataContainer::Pointer oldDc = oldDca->getDataContainer(oldPath.getDataContainerName());
  DataContainer::Pointer newDc = newDca->getDataContainer(newPath.getDataContainerName());

  // Data Container required
  if(oldDc && newDc)
  {
    IGeometry::Pointer oldGeom = oldDc->getGeometry();
    IGeometry::Pointer newGeom = newDc->getGeometry();

    bool hasGeom = oldGeom && newGeom;
    if((hasGeom && oldGeom->getGeometryType() == newGeom->getGeometryType()) || false == hasGeom)
    {
      // No Attribute Matrix path
      if(oldPath.getAttributeMatrixName().isEmpty() && newPath.getAttributeMatrixName().isEmpty())
      {
        return true;
      }

      AttributeMatrix::Pointer oldAm = oldDc->getAttributeMatrix(oldPath.getAttributeMatrixName());
      AttributeMatrix::Pointer newAm = newDc->getAttributeMatrix(newPath.getAttributeMatrixName());

      bool hasAttributeMatrix = oldAm && newAm;
      if(hasAttributeMatrix && oldAm->getType() == newAm->getType() && oldAm->getNumberOfTuples() == newAm->getNumberOfTuples())
      {
        // No Data Array path
        if(oldPath.getDataArrayName().isEmpty() && newPath.getDataArrayName().isEmpty())
        {
          return true;
        }

        IDataArray::Pointer oldDa = oldAm->getAttributeArray(oldPath.getDataArrayName());
        IDataArray::Pointer newDa = newAm->getAttributeArray(newPath.getDataArrayName());

        bool hasDataArray = oldDa && newDa;
        bool hasSameType = oldDa->getTypeAsString() == newDa->getTypeAsString();
        bool hasSameCompDims = oldDa->getComponentDimensions() == newDa->getComponentDimensions();
        bool isNeighborList = hasSameType && newDa->getTypeAsString().startsWith("NeighborList");
        if(hasDataArray && hasSameType  && (hasSameCompDims || isNeighborList))
        {
          return true;
        }
      }
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::RenameContainer DataArrayPath::CheckForRenamedPaths(DataContainerArrayShPtr oldDca, DataContainerArrayShPtr newDca, std::list<DataArrayPath> oldPaths, std::list<DataArrayPath> newPaths)
{
  RenameContainer container;
  std::list<DataArrayPath> duplicatedPaths;
  std::list<DataArrayPath> usedNewPaths;

  // For each older path, check for any matching new paths.  If only one new path matches, add it as a possibility
  for(DataArrayPath oldPath : oldPaths)
  {
    // If the same path exists in both oldPaths and newPaths, it was not renamed
    if(std::find(newPaths.begin(), newPaths.end(), oldPath) != newPaths.end())
    {
      continue;
    }

    // Find any potential renames in newPaths for the given oldPath
    std::list<DataArrayPath> matches;
    for(DataArrayPath newPath : newPaths)
    {
      // If the same path exists in both oldPaths and newPaths, it was not renamed
      if(std::find(oldPaths.begin(), oldPaths.end(), newPath) != oldPaths.end())
      {
        continue;
      }

      // Check that all geometries, AttributeMatrices, and DataArrays are compatible
      if(CheckRenamePath(oldDca, newDca, oldPath, newPath))
      {
        matches.push_back(newPath);
      }
    }
    // If this path was already used, mark it as duplicate and move on
    if(matches.size() == 1)
    {
      DataArrayPath newPath = (*matches.begin());
      if(usedNewPaths.end() != std::find(usedNewPaths.begin(), usedNewPaths.end(), newPath))
      {
        duplicatedPaths.push_back(newPath);
      }
      else
      {
        usedNewPaths.push_back(newPath);
        container.push_back(RenameType(oldPath, newPath));
      }
    }
  }

  // Remove items with duplicated paths
  for(auto iter = container.begin(); iter != container.end(); )
  {
    DataArrayPath checkPath = std::get<1>(*iter);
    if(duplicatedPaths.end() != std::find(duplicatedPaths.begin(), duplicatedPaths.end(), checkPath))
    {
      iter = container.erase(iter);
    }
    else
    {
      iter++;
    }
  }

  return container;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataType DataArrayPath::getDataType()
{
  if(getDataContainerName().isEmpty())
  {
    return DataType::None;
  }
  else if(getAttributeMatrixName().isEmpty() && getDataArrayName().isEmpty())
  {
    return DataType::DataContainer;
  }
  else if(getDataArrayName().isEmpty())
  {
    return DataType::AttributeMatrix;
  }
  else if(false == getAttributeMatrixName().isEmpty())
  {
    return DataType::DataArray;
  }

  return DataType::None;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> DataArrayPath::toQVector()
{
  QVector<QString> v(3);
  v[0] = m_DataContainerName;
  v[1] = m_AttributeMatrixName;
  v[2] = m_DataArrayName;
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::isEmpty() const
{
  if(m_DataContainerName.isEmpty() && m_AttributeMatrixName.isEmpty() && m_DataArrayName.isEmpty())
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::isValid() const
{
  if(m_DataContainerName.isEmpty() == false && m_AttributeMatrixName.isEmpty() == false && m_DataArrayName.isEmpty() == false)
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DataArrayPath::GetDataArrayNames(const QVector<DataArrayPath>& paths)
{
  QList<QString> arrayNames;
  for(int i = 0; i < paths.count(); i++)
  {
    arrayNames.push_back(paths.at(i).getDataArrayName());
  }

  return arrayNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataArrayPath::GetAttributeMatrixPath(const QVector<DataArrayPath>& paths)
{
  if(paths.isEmpty())
  {
    return DataArrayPath();
  }
  else
  {
    return DataArrayPath(paths.first().getDataContainerName(), paths.first().getAttributeMatrixName(), "");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataArrayPath::toQStringList() const
{
  QStringList l;
  l << m_DataContainerName << m_AttributeMatrixName << m_DataArrayName;
  return l;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::update(const QString& dcName, const QString& amName, const QString& daName)
{
  m_DataContainerName = dcName;
  m_AttributeMatrixName = amName;
  m_DataArrayName = daName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::hasSameDataContainer(const DataArrayPath& other) const
{
  return (m_DataContainerName == other.m_DataContainerName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::hasSameAttributeMatrix(const DataArrayPath& other) const
{
  return (m_AttributeMatrixName == other.m_AttributeMatrixName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::hasSameDataArray(const DataArrayPath& other) const
{
  return (m_DataArrayName == other.m_DataArrayName);
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
bool DataArrayPath::possibleRename(const DataArrayPath& updated) const
{
  // Empty DataArrayPaths are not considered renames
  // Neither are DataArrayPaths of different lengths
  if(updated.getDataContainerName().isEmpty() || getDataContainerName().isEmpty())
  {
    return false;
  }
  else if(updated.getAttributeMatrixName().isEmpty() != getAttributeMatrixName().isEmpty())
  {
    return false;
  }
  else if(updated.getDataArrayName().isEmpty() != getDataArrayName().isEmpty())
  {
    return false;
  }

  // Check number of differences
  int differences = 0;
  if(false == hasSameDataArray(updated))
  {
    differences++;
  }
  if(false == hasSameAttributeMatrix(updated))
  {
    differences++;
  }
  if(false == hasSameDataContainer(updated))
  {
    differences++;
  }

  if(1 == differences)
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::updatePath(const DataArrayPath::RenameType& renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  // Check for differences with original path
  if(false == hasSameDataArray(oldPath) && false == oldPath.getDataArrayName().isEmpty())
  {
    return false;
  }
  else if(false == hasSameAttributeMatrix(oldPath) && false == oldPath.getAttributeMatrixName().isEmpty())
  {
    return false;
  }
  else if(false == hasSameDataContainer(oldPath) && false == oldPath.getDataContainerName().isEmpty())
  {
    return false;
  }
  else if(oldPath.getDataContainerName().isEmpty() || newPath.getDataContainerName().isEmpty())
  {
    return false;
  }

  // Substitude in the new DataArrayPath
  if(hasSameDataContainer(oldPath))
  {
    setDataContainerName(newPath.getDataContainerName());

    if(hasSameAttributeMatrix(oldPath))
    {
      setAttributeMatrixName(newPath.getAttributeMatrixName());

      if(hasSameDataArray(oldPath))
      {
        setDataArrayName(newPath.getDataArrayName());
      }
    }
  }
  
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::ValidateVector(const QVector<DataArrayPath>& other)
{
  QString dcName = "";
  QString amName = "";

  if(other.isEmpty())
  {
    return true;
  }
  else
  {
    dcName = other.first().getDataContainerName();
    amName = other.first().getAttributeMatrixName();
  }

  for(int i = 0; i < other.size(); ++i)
  {
    if(other.at(i).getDataContainerName() != dcName || other.at(i).getAttributeMatrixName() != amName)
    {
      return false;
    }
  }

  return true;
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
    m_DataContainerName = json["Data Container Name"].toString();
    m_AttributeMatrixName = json["Attribute Matrix Name"].toString();
    m_DataArrayName = json["Data Array Name"].toString();
    return true;
  }
  return false;
}
