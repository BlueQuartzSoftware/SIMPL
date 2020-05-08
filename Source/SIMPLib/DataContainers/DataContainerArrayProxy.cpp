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

#include "DataContainerArrayProxy.h"

#include <iostream>

#include <QtCore/QTextStream>

#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainerProxy.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::DataContainerArrayProxy() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::DataContainerArrayProxy(const DataContainerArrayProxy& rhs)
{
  m_DataContainers = rhs.m_DataContainers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::DataContainerArrayProxy(DataContainerArray* dca)
{

  if(nullptr == dca)
  {
    return;
  }

  DataContainerArray::Container containers = dca->getDataContainers();
  for(DataContainer::Pointer dataContainer : containers) // Loop on each Data Container
  {
    IGeometry::Pointer geo = dataContainer->getGeometry();
    IGeometry::Type dcType;
    if(geo != IGeometry::NullPointer())
    {
      dcType = geo->getGeometryType();
    }
    else
    {
      dcType = IGeometry::Type::Unknown;
    }
    DataContainerProxy dcProxy(dataContainer->getName(), Qt::Checked, dcType); // Create a new DataContainerProxy

    // Now loop over each AttributeMatrix in the data container that was selected
    DataContainer::Container_t attrMats = dataContainer->getAttributeMatrices();
    for(auto iter = attrMats.begin(); iter != attrMats.end(); ++iter)
    {
      AttributeMatrix::Pointer attrMat = *iter;
      QString amName = attrMat->getName();
      AttributeMatrixProxy amProxy(amName, Qt::Checked, attrMat->getType());

      for(IDataArray::Pointer attrArray : attrMat->getAttributeArrays())
      {
        QString aaName = attrArray->getName();
        QString daPath = dataContainer->getName() + "/" + amName + "/";

        DataArrayProxy daProxy(daPath, aaName, Qt::Checked, attrArray->getTypeAsString(), attrArray->getClassVersion());
        daProxy.setCompDims(attrArray->getComponentDimensions());
        daProxy.setTupleDims(attrMat->getTupleDimensions());
        amProxy.insertDataArray(aaName, daProxy);
      }

      dcProxy.insertAttributeMatrix(amName, amProxy); // Add the new AttributeMatrix to the DataContainerProxy
    }
    m_DataContainers.insert(dcProxy.getName(), dcProxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::~DataContainerArrayProxy() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy& DataContainerArrayProxy::operator=(const DataContainerArrayProxy& rhs) = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::DataContainerArrayProxy(DataContainerArrayProxy&&) noexcept = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy& DataContainerArrayProxy::operator=(DataContainerArrayProxy&&) noexcept = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArrayProxy::operator==(const DataContainerArrayProxy& rhs) const
{
  return m_DataContainers == rhs.m_DataContainers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArrayProxy::operator!=(const DataContainerArrayProxy& rhs) const
{
  return m_DataContainers != rhs.m_DataContainers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataContainerArrayProxy::flattenHeirarchy(Qt::CheckState dcFlag, Qt::CheckState amFlag, Qt::CheckState daFlag)
{
  QStringList strList;
  QList<DataContainerProxy> dcProxies = m_DataContainers.values();
  QListIterator<DataContainerProxy> dcIter(dcProxies);

  for(auto& dcProxy : dcProxies)
  {
    if(dcFlag == Qt::Checked && dcProxy.getFlag() == Qt::Checked)
    {
      DataContainerProxy::StorageType& amMap = dcProxy.getAttributeMatricies();
      QMapIterator<QString, AttributeMatrixProxy> amIter(amMap);
      while(amIter.hasNext()) // AttributeMatrixLevel
      {
        amIter.next();

        AttributeMatrixProxy amProxy = amIter.value();
        if(amFlag == Qt::Checked && amProxy.getFlag() == Qt::Checked)
        {
          AttributeMatrixProxy::StorageType daMap = amProxy.getDataArrays();
          QMapIterator<QString, DataArrayProxy> dIter(daMap);
          while(dIter.hasNext()) // DataArray Level
          {
            dIter.next();

            const DataArrayProxy& daProxy = dIter.value();
            if(daFlag == Qt::Checked && daProxy.getFlag() == Qt::Checked)
            {
              QString path = QString("%1|%2|%3").arg(dcProxy.getName()).arg(amProxy.getName()).arg(daProxy.getName());
              strList << path;
            }
          }
        }
      }
    }
  }
  return strList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::print(const QString& header)
{
  QString str;
  QTextStream out(&str);
  QList<DataContainerProxy> dcProxies = m_DataContainers.values();
  QListIterator<DataContainerProxy> dcIter(dcProxies);
  for(auto dcProxy : dcProxies)
  {
    out << dcProxy.getName() << "|"
        << "  [flag:" << (int)(dcProxy.getFlag()) << "]\n";
    DataContainerProxy::StorageType& amMap = dcProxy.getAttributeMatricies();
    for(auto& amProxy : amMap)
    {
      out << dcProxy.getName() << "|" << amProxy.getName() << "|"
          << "  [flag:" << (int)(amProxy.getFlag()) << "]\n";
      AttributeMatrixProxy::StorageType& daMap = amProxy.getDataArrays();
      QMapIterator<QString, DataArrayProxy> dIter(daMap);
      while(dIter.hasNext()) // DataArray Level
      {
        dIter.next();

        const DataArrayProxy& daProxy = dIter.value();

        out << dcProxy.getName() << "|" << amProxy.getName() << "|" << daProxy.getName() << "  [flag:" << (int)(daProxy.getFlag()) << "]"
            << " [Object Type: " << daProxy.getObjectType() << "] [CompDims: ";
        for(const auto& compDim : daProxy.getCompDims())
        {
          out << compDim << " ";
        }
        out << "]\n";
      }
    }
  }
  std::cout << "---------------- " << header.toStdString() << " ----------------------" << std::endl;
  std::cout << str.toStdString() << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::setFlags(uint8_t flag, DataContainerProxy::DCGeometryTypeFlags dcGeoms, AttributeMatrixProxy::AMTypeFlags amTypes, DataArrayProxy::PrimitiveTypeFlags primitiveTypes,
                                       const DataArrayProxy::CompDimsVector& compDimsVector)
{
  for(QMap<QString, DataContainerProxy>::iterator dcIter = m_DataContainers.begin(); dcIter != m_DataContainers.end(); ++dcIter) // DataContainer Level
  {
    DataContainerProxy& dcProxy = dcIter.value();
    DataContainerProxy::DCGeometryTypeFlag dcGeomFlag = DataContainerProxy::GeometryTypeToFlag(static_cast<IGeometry::Type>(dcProxy.getDCType()));
    if((dcGeoms & dcGeomFlag) > 0 || dcGeoms == DataContainerProxy::DCGeometryTypeFlag::Any_DCGeomType)
    {
      dcProxy.setFlags(flag, amTypes, primitiveTypes, compDimsVector);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::reverseFlags()
{
  for(QMap<QString, DataContainerProxy>::iterator dcIter = m_DataContainers.begin(); dcIter != m_DataContainers.end(); ++dcIter) // DataContainer Level
  {
    DataContainerProxy& dcProxy = dcIter.value();

    dcProxy.setFlag((dcProxy.getFlag() == Qt::Checked ? Qt::Unchecked : Qt::Checked));

    QMap<QString, AttributeMatrixProxy> amProxies = dcProxy.getAttributeMatricies();
    for(auto& amProxy : amProxies)
    {
      amProxy.setFlag((amProxy.getFlag() == Qt::Checked ? Qt::Unchecked : Qt::Checked));

      AttributeMatrixProxy::StorageType daProxies = amProxy.getDataArrays();
      for(auto& daProxy : daProxies)
      {
        if(daProxy.getFlag() == Qt::Checked)
        {
          daProxy.setFlag(Qt::Unchecked);
        }
        else if(daProxy.getFlag() == Qt::Unchecked)
        {
          daProxy.setFlag(Qt::Checked);
        }
      }
    }
    dcProxy.setAttributeMatricies(amProxies);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataContainerArrayProxy::serialize()
{

  QStringList entries;

  for(QMap<QString, DataContainerProxy>::iterator dcIter = m_DataContainers.begin(); dcIter != m_DataContainers.end(); ++dcIter) // DataContainer Level
  {
    DataContainerProxy& dcProxy = dcIter.value();

    QMap<QString, AttributeMatrixProxy> amProxies = dcProxy.getAttributeMatricies();
    for(auto& amProxy : amProxies)
    {
      QMap<QString, DataArrayProxy> daProxies = amProxy.getDataArrays();
      for(const auto& daProxy : daProxies)
      {
        QString str = QString("[PATH=%1|%2|%3][FLAG=%4]").arg(dcProxy.getName()).arg(amProxy.getName()).arg(daProxy.getName()).arg(daProxy.getFlag());
        entries << str;
      }
    }
  }
  return entries;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy DataContainerArrayProxy::MergeProxies(DataContainerArrayProxy fileProxy, DataContainerArrayProxy cacheProxy)
{
  QMap<QString, DataContainerProxy> fDcMap = fileProxy.getDataContainers();
  QMap<QString, DataContainerProxy> cDcMap = cacheProxy.getDataContainers(); // This creates a copy.

  // Add extra items in the file to the cache
  for(QMap<QString, DataContainerProxy>::iterator fDcIter = fDcMap.begin(); fDcIter != fDcMap.end(); ++fDcIter)
  {
    const QString& fileDcName = fDcIter.key();
    DataContainerProxy fileDcProxy = fDcIter.value();

    // If the cache does not have the file dc proxy, add it to the cache
    if(!cDcMap.contains(fileDcName))
    {
      cDcMap.insert(fileDcName, fileDcProxy);
    }

    QMap<QString, AttributeMatrixProxy> fAmMap = fileDcProxy.getAttributeMatricies();
    QMap<QString, AttributeMatrixProxy> cAmMap = cDcMap.find(fileDcName).value().getAttributeMatricies();
    for(QMap<QString, AttributeMatrixProxy>::iterator fAmIter = fAmMap.begin(); fAmIter != fAmMap.end(); ++fAmIter)
    {
      const QString& fileAmName = fAmIter.key();
      AttributeMatrixProxy fileAmProxy = fAmIter.value();
      // If the cache does not have the file am proxy, add it to the cache
      if(!cAmMap.contains(fileAmName))
      {
        cAmMap.insert(fileAmName, fileAmProxy);
      }

      QMap<QString, DataArrayProxy> fDaMap = fileAmProxy.getDataArrays();
      QMap<QString, DataArrayProxy> cDaMap = cAmMap.find(fileAmName).value().getDataArrays();
      for(QMap<QString, DataArrayProxy>::iterator fDaIter = fDaMap.begin(); fDaIter != fDaMap.end(); ++fDaIter)
      {
        const QString& fileDaName = fDaIter.key();
        DataArrayProxy fileDaProxy = fDaIter.value();
        // If the cache does not have the file da proxy, add it to the cache
        if(!cDaMap.contains(fileDaName))
        {
          cDaMap.insert(fileDaName, fileDaProxy);
        }
      }
      cAmMap.find(fileAmName).value().setDataArrays(cDaMap);
    }
  }

  cacheProxy.setDataContainers(cDcMap); // Set the changes to the DataContainerArray back to the cacheProxy

  // Remove items from the cache that are no longer in the file
  QList<QString> dcItemsToDelete;
  for(QMap<QString, DataContainerProxy>::iterator cDcIter = cDcMap.begin(); cDcIter != cDcMap.end(); ++cDcIter)
  {
    const QString& cacheDcName = cDcIter.key();
    DataContainerProxy& cacheDcProxy = cDcIter.value();

    // If the file does not have the cached dc proxy, remove it from the cache
    if(!fDcMap.contains(cacheDcName))
    {
      dcItemsToDelete.push_back(cacheDcName);
    }
    else
    {
      QMap<QString, AttributeMatrixProxy> cAmMap = cacheDcProxy.getAttributeMatricies();
      QMap<QString, AttributeMatrixProxy> fAmMap = fDcMap.find(cacheDcName).value().getAttributeMatricies();

      QList<QString> amItemsToDelete;
      for(QMap<QString, AttributeMatrixProxy>::iterator amIter = cAmMap.begin(); amIter != cAmMap.end(); ++amIter)
      {
        const QString& cacheAmName = amIter.key();
        AttributeMatrixProxy& cacheAmProxy = amIter.value();
        // If the file does not have the cached am proxy, remove it from the cache
        if(!fAmMap.contains(cacheAmName))
        {
          amItemsToDelete.push_back(cacheAmName);
        }
        else
        {
          QMap<QString, DataArrayProxy> cDaMap = cacheAmProxy.getDataArrays();
          QMap<QString, DataArrayProxy> fDaMap = fAmMap.find(cacheAmName).value().getDataArrays();

          QList<QString> daItemsToDelete;
          for(QMap<QString, DataArrayProxy>::iterator daIter = cDaMap.begin(); daIter != cDaMap.end(); ++daIter)
          {
            const QString& cacheDaName = daIter.key();
            DataArrayProxy cacheDaProxy = daIter.value();
            // If the file does not have the cached da proxy, remove it from the cache
            if(!fDaMap.contains(cacheDaName))
            {
              daItemsToDelete.push_back(cacheDaName);
            }
          }
          // Remove extra da entries from cache
          for(const auto& daName : daItemsToDelete)
          {
            cDaMap.remove(daName);
          }
          cacheAmProxy.setDataArrays(cDaMap);
        }
      }
      // Remove extra am entries from cache
      for(const auto& amName : amItemsToDelete)
      {
        cAmMap.remove(amName);
      }
      cacheDcProxy.setAttributeMatricies(cAmMap);
    }
  }
  // Remove extra dc entries from cache
  for(const auto& dcName : dcItemsToDelete)
  {
    cDcMap.remove(dcName);
  }
  cacheProxy.setDataContainers(cDcMap); // Set the changes to the DataContainerArray back to the cacheProxy

  return cacheProxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArrayProxy::contains(const QString& name)
{
  QList<DataContainerProxy> dcProxies = m_DataContainers.values();
  QListIterator<DataContainerProxy> iter(dcProxies);
  while(iter.hasNext())
  {
    const DataContainerProxy dc = iter.next();
    if(dc.getName() == name)
    {
      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerProxy& DataContainerArrayProxy::getDataContainerProxy(const QString& name)
{
  for(QMap<QString, DataContainerProxy>::iterator dcIter = m_DataContainers.begin(); dcIter != m_DataContainers.end(); ++dcIter) // DataContainer Level
  {
    DataContainerProxy& dc = dcIter.value();
    if(dc.getName() == name)
    {
      return dc;
    }
  }

  DataContainerProxy proxy(name);
  m_DataContainers.insert(proxy.getName(), proxy);
  return m_DataContainers[name];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::updatePath(DataArrayPath::RenameType renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  if(m_DataContainers.contains(oldPath.getDataContainerName()))
  {
    DataContainerProxy dcProxy = m_DataContainers.take(oldPath.getDataContainerName());
    dcProxy.updatePath(renamePath);
    m_DataContainers.insert(newPath.getDataContainerName(), dcProxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::writeJson(QJsonObject& json) const
{
  json["Data Containers"] = writeMap(m_DataContainers);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArrayProxy::readJson(QJsonObject& json)
{
  if(json["Data Containers"].isArray())
  {
    m_DataContainers = readMap(json["Data Containers"].toArray());
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonArray DataContainerArrayProxy::writeMap(const QMap<QString, DataContainerProxy>& map) const
{
  QJsonArray dcArray;
  for(const auto& dcProxy : map)
  {
    QJsonObject obj;
    dcProxy.writeJson(obj);
    dcArray.push_back(obj);
  }
  return dcArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, DataContainerProxy> DataContainerArrayProxy::readMap(QJsonArray jsonArray)
{
  QMap<QString, DataContainerProxy> map;
  for(const auto& val : jsonArray)
  {
    if(val.isObject())
    {
      DataContainerProxy dc;
      QJsonObject obj = val.toObject();
      dc.readJson(obj);
      map.insert(dc.getName(), dc);
    }
  }
  return map;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, DataContainerProxy>& DataContainerArrayProxy::getDataContainers()
{
  return m_DataContainers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const DataContainerArrayProxy::StorageType& DataContainerArrayProxy::getDataContainers() const
{
  return m_DataContainers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::setDataContainers(QMap<QString, DataContainerProxy>& newDataContainers)
{
  m_DataContainers = newDataContainers;
}

// -----------------------------------------------------------------------------
void DataContainerArrayProxy::insertDataContainer(const QString& name, DataContainerProxy& proxy)
{
  m_DataContainers.insert(name, proxy);
}
