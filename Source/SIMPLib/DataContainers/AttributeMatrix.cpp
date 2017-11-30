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

#include "SIMPLib/DataContainers/AttributeMatrix.h"

// C Includes

// C++ Includes
#include <fstream>
#include <iostream>

// HDF5 Includes
#include "H5Support/HDF5ScopedFileSentinel.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

// DREAM3D Includes
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/HDF5/H5DataArrayReader.h"
#include "SIMPLib/HDF5/VTKH5Constants.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/DataContainers/AttributeMatrixProxy.h"
#include "SIMPLib/DataContainers/DataContainerProxy.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::AttributeMatrix(QVector<size_t> tDims, const QString& name, AttributeMatrix::Type attrType)
: m_Name(name)
, m_TupleDims(tDims)
, m_Type(attrType)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::~AttributeMatrix()
{
  // std::cout << "~AttributeMatrix" << std::endl;
}

// -----------------------------------------------------------------------------
QString AttributeMatrix::TypeToString(AttributeMatrix::Type t)
{
  QString out("Unknown");
  switch(t)
  {
  case AttributeMatrix::Type::Vertex:
    out = QString("Vertex");
    break;
  case AttributeMatrix::Type::Edge:
    out = QString("Edge");
    break;
  case AttributeMatrix::Type::Face:
    out = QString("Face");
    break;
  case AttributeMatrix::Type::Cell:
    out = QString("Cell");
    break;
  case AttributeMatrix::Type::VertexFeature:
    out = QString("VertexFeature");
    break;
  case AttributeMatrix::Type::EdgeFeature:
    out = QString("EdgeFeature");
    break;
  case AttributeMatrix::Type::FaceFeature:
    out = QString("FaceFeature");
    break;
  case AttributeMatrix::Type::CellFeature:
    out = QString("CellFeature");
    break;
  case AttributeMatrix::Type::VertexEnsemble:
    out = QString("VertexEnsemble");
    break;
  case AttributeMatrix::Type::EdgeEnsemble:
    out = QString("EdgeEnsemble");
    break;
  case AttributeMatrix::Type::FaceEnsemble:
    out = QString("FaceEnsemble");
    break;
  case AttributeMatrix::Type::CellEnsemble:
    out = QString("CellEnsemble");
    break;
  case AttributeMatrix::Type::MetaData:
    out = QString("MetaData");
    break;
  case AttributeMatrix::Type::Generic:
    out = QString("Generic");
    break;
  case AttributeMatrix::Type::Unknown:
    out = QString("Unknown");
    break;
  case AttributeMatrix::Type::Any:
    out = QString("Any");
    break;
  }
  return out;
}

// -----------------------------------------------------------------------------
AttributeMatrix::Type AttributeMatrix::StringToType(const QString& str)
{
  AttributeMatrix::Type t = AttributeMatrix::Type::Unknown;
  if(str.compare("Vertex") == 0)
  {
    return AttributeMatrix::Type::Vertex;
  }
  if(str.compare("Edge") == 0)
  {
    return AttributeMatrix::Type::Edge;
  }
  if(str.compare("Face") == 0)
  {
    return AttributeMatrix::Type::Face;
  }
  if(str.compare("Cell") == 0)
  {
    return AttributeMatrix::Type::Cell;
  }
  if(str.compare("VertexFeature") == 0)
  {
    return AttributeMatrix::Type::VertexFeature;
  }
  if(str.compare("EdgeFeature") == 0)
  {
    return AttributeMatrix::Type::EdgeFeature;
  }
  if(str.compare("FaceFeature") == 0)
  {
    return AttributeMatrix::Type::FaceFeature;
  }
  if(str.compare("CellFeature") == 0)
  {
    return AttributeMatrix::Type::CellFeature;
  }
  if(str.compare("VertexEnsemble") == 0)
  {
    return AttributeMatrix::Type::VertexEnsemble;
  }
  if(str.compare("EdgeEnsemble") == 0)
  {
    return AttributeMatrix::Type::EdgeEnsemble;
  }
  if(str.compare("FaceEnsemble") == 0)
  {
    return AttributeMatrix::Type::FaceEnsemble;
  }
  if(str.compare("CellEnsemble") == 0)
  {
    return AttributeMatrix::Type::CellEnsemble;
  }
  if(str.compare("MetaData") == 0)
  {
    return AttributeMatrix::Type::MetaData;
  }
  if(str.compare("Generic") == 0)
  {
    return AttributeMatrix::Type::Generic;
  }
  if(str.compare("Unknown") == 0)
  {
    return AttributeMatrix::Type::Unknown;
  }
  if(str.compare("Any") == 0)
  {
    return AttributeMatrix::Type::Any;
  }

  return t;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> AttributeMatrix::GetTypesAsStrings()
{
    QVector<QString> choices;
    choices.push_back("Vertex");
    choices.push_back("Edge");
    choices.push_back("Face");
    choices.push_back("Cell");
    choices.push_back("VertexFeature");
    choices.push_back("EdgeFeature");
    choices.push_back("FaceFeature");
    choices.push_back("CellFeature");
    choices.push_back("VertexEnsemble");
    choices.push_back("EdgeEnsemble");
    choices.push_back("FaceEnsemble");
    choices.push_back("CellEnsemble");
    choices.push_back("MetaData");
    choices.push_back("Generic");
    choices.push_back("Unknown");
    choices.push_back("Any");
    return choices;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::ReadAttributeMatrixStructure(hid_t containerId, DataContainerProxy* dcProxy, SIMPLH5DataReaderRequirements* req, const QString &h5InternalPath)
{
  QList<QString> attributeMatrixNames;
  QH5Utilities::getGroupObjects(containerId, H5Utilities::H5Support_GROUP, attributeMatrixNames);
  foreach(QString attributeMatrixName, attributeMatrixNames)
  {
    if(attributeMatrixName.compare(SIMPL::Geometry::Geometry) != 0)
    {
      if(__SHOW_DEBUG_MSG__)
      {
        std::cout << "    AttributeMatrix: " << attributeMatrixName.toStdString() << std::endl;
      }
      hid_t attrMatGid = H5Gopen(containerId, attributeMatrixName.toLatin1().constData(), H5P_DEFAULT);
      if(attrMatGid < 0)
      {
        continue;
      }
      HDF5ScopedGroupSentinel sentinel(&attrMatGid, true);

      AttributeMatrixProxy amProxy(attributeMatrixName);
      amProxy.name = attributeMatrixName;
      amProxy.flag = Qt::Unchecked;

      AttributeMatrix::EnumType amTypeTmp = static_cast<AttributeMatrix::EnumType>(AttributeMatrix::Type::Unknown);
      herr_t err = QH5Lite::readScalarAttribute(containerId, attributeMatrixName, SIMPL::StringConstants::AttributeMatrixType, amTypeTmp);
      if(err >= 0)
      {
        AttributeMatrix::Types amTypes = req->getAMTypes();
        if (amTypes.size() <= 0 || amTypes.contains(static_cast<AttributeMatrix::Type>(amTypeTmp)))
        {
          amProxy.flag = Qt::Checked;
        }
        amProxy.amType = static_cast<AttributeMatrix::Type>(amTypeTmp);
      }
      else
      {
        std::cout << "Error Reading the AttributeMatrix Type for AttributeMatrix " << attributeMatrixName.toStdString() << std::endl;
      }

      QString h5Path = h5InternalPath + "/" + attributeMatrixName;

      // Read in the names of the Data Arrays that make up the AttributeMatrix
      QMap<QString, DataArrayProxy>& daProxies = amProxy.dataArrays;
      DataArrayProxy::ReadDataArrayStructure(attrMatGid, daProxies, req, h5Path);

      // Insert the AttributeMatrixProxy proxy into the dataContainer proxy
      dcProxy->attributeMatricies.insert(attributeMatrixName, amProxy);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::setType(AttributeMatrix::Type value)
{
  m_Type = value;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Type AttributeMatrix::getType() const
{
  return m_Type;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrix::doesAttributeArrayExist(const QString& name) const
{
  return m_AttributeArrays.contains(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrix::validateAttributeArraySizes()
{
  int64_t arraySize = 0;
  int64_t matrixSize = getNumberOfTuples();
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
  {
    IDataArray::Pointer d = iter.value();
    arraySize = d->getNumberOfTuples();
    if(arraySize != matrixSize)
    {
      return false;
    }
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::addAttributeArray(const QString& name, IDataArray::Pointer data)
{
  if(data->getName().compare(name) != 0)
  {
    qDebug() << "Adding Attribute Array with different array name than key name"
             << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->getName() << "\n";
    data->setName(name);
  }
  if(getNumberOfTuples() != data->getNumberOfTuples())
  {
    qDebug() << "AttributeMatrix::Name: " << getName() << "  dataArray::name:  " << data->getName() << " Type: " << data->getTypeAsString();
    qDebug() << "getNumberOfTuples(): " << getNumberOfTuples() << "  data->getNumberOfTuples(): " << data->getNumberOfTuples();
  }
  Q_ASSERT(getNumberOfTuples() == data->getNumberOfTuples());

  m_AttributeArrays[name] = data;
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer AttributeMatrix::getAttributeArray(const QString& name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it = m_AttributeArrays.find(name);
  if(it == m_AttributeArrays.end())
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer AttributeMatrix::removeAttributeArray(const QString& name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it = m_AttributeArrays.find(name);
  if(it == m_AttributeArrays.end())
  {
    // DO NOT return a NullPointer for any reason other than "Data Array was not found"
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_AttributeArrays.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameErrorCodes AttributeMatrix::renameAttributeArray(const QString& oldname, const QString& newname, bool overwrite)
{
  QMap<QString, IDataArray::Pointer>::iterator itOld;
  QMap<QString, IDataArray::Pointer>::iterator itNew;

  itNew = m_AttributeArrays.find(newname);
  // If new name doesn't exist or we want to overwrite one that does exist...
  if(itNew == m_AttributeArrays.end() || overwrite == true)
  {
    itOld = m_AttributeArrays.find(oldname);
    // If old name doesn't exist...
    if(itOld == m_AttributeArrays.end())
    {
      return OLD_DOES_NOT_EXIST;
    }
    IDataArray::Pointer p = itOld.value();
    p->setName(newname);
    removeAttributeArray(oldname);
    addAttributeArray(newname, p);
    return SUCCESS;
  }
  return NEW_EXISTS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> AttributeMatrix::getTupleDimensions()
{
  return m_TupleDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::setTupleDimensions(QVector<size_t> tupleDims)
{
  resizeAttributeArrays(tupleDims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t AttributeMatrix::getNumberOfTuples()
{
  if (m_TupleDims.size() == 0)
  {
    return 0;
  }

  size_t numTuples = m_TupleDims[0];
  for(int i = 1; i < m_TupleDims.size(); i++)
  {
    numTuples *= m_TupleDims[i];
  }
  return numTuples;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrix::removeInactiveObjects(const QVector<bool> &activeObjects, DataArray<int32_t> *featureIds)
{
  bool acceptableMatrix = false;
  // Only valid for feature or ensemble type matrices
  if(m_Type == AttributeMatrix::Type::VertexFeature || m_Type == AttributeMatrix::Type::VertexEnsemble || m_Type == AttributeMatrix::Type::EdgeFeature ||
     m_Type == AttributeMatrix::Type::EdgeEnsemble || m_Type == AttributeMatrix::Type::FaceFeature || m_Type == AttributeMatrix::Type::FaceEnsemble ||
     m_Type == AttributeMatrix::Type::CellFeature || m_Type == AttributeMatrix::Type::CellEnsemble)
  {
    acceptableMatrix = true;
  }
  size_t totalTuples = getNumberOfTuples();
  if(static_cast<size_t>(activeObjects.size()) == totalTuples && acceptableMatrix == true)
  {
    size_t goodcount = 1;
    QVector<size_t> newNames(totalTuples, 0);
    QVector<size_t> removeList;

    for(qint32 i = 1; i < activeObjects.size(); i++)
    {
      if(activeObjects[i] == false)
      {
        removeList.push_back(i);
        newNames[i] = 0;
      }
      else
      {
        newNames[i] = goodcount;
        goodcount++;
      }
    }

    if(removeList.size() > 0)
    {
      QList<QString> headers = getAttributeArrayNames();
      for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
      {
        IDataArray::Pointer p = getAttributeArray(*iter);
        QString type = p->getTypeAsString();
        if(type.compare("NeighborList<T>") == 0)
        {
          removeAttributeArray(*iter);
        }
        else
        {
          p->eraseTuples(removeList);
        }
      }
      QVector<size_t> tDims(1, (totalTuples - removeList.size()));
      setTupleDimensions(tDims);

      // Loop over all the points and correct all the feature names
      size_t totalPoints = featureIds->getNumberOfTuples();
      int32_t* featureIdPtr = featureIds->getPointer(0);
      for(size_t i = 0; i < totalPoints; i++)
      {
        if(featureIdPtr[i] >= 0 && featureIdPtr[i] < newNames.size())
        {
          featureIdPtr[i] = static_cast<int32_t>(newNames[featureIdPtr[i]]);
        }
      }
    }
  }
  else
  {
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::resizeAttributeArrays(QVector<size_t> tDims)
{
  // int success = 0;
  m_TupleDims = tDims;
  size_t numTuples = m_TupleDims[0];
  for(int i = 1; i < m_TupleDims.size(); i++)
  {
    numTuples *= m_TupleDims[i];
  }

  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
  {
    // std::cout << "Resizing Array '" << (*iter).first << "' : " << success << std::endl;
    IDataArray::Pointer d = iter.value();
    d->resize(numTuples);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::clearAttributeArrays()
{
  m_AttributeArrays.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> AttributeMatrix::getAttributeArrayNames()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
  {
    keys.push_back(iter.key());
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::getNumAttributeArrays() const
{
  return static_cast<int>(m_AttributeArrays.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer AttributeMatrix::deepCopy(bool forceNoAllocate)
{
  AttributeMatrix::Pointer newAttrMat = AttributeMatrix::New(getTupleDimensions(), getName(), getType());

  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
  {
    IDataArray::Pointer d = iter.value();
    IDataArray::Pointer new_d = d->deepCopy(forceNoAllocate);
    if(new_d.get() == nullptr)
    {
      return AttributeMatrix::NullPointer();
    }
    newAttrMat->addAttributeArray(new_d->getName(), new_d);
  }

  return newAttrMat;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::writeAttributeArraysToHDF5(hid_t parentId)
{
  int err;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
  {
    IDataArray::Pointer d = iter.value();
    err = d->writeH5Data(parentId, m_TupleDims);
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
int AttributeMatrix::addAttributeArrayFromHDF5Path(hid_t gid, QString name, bool preflight)
{
  int err = 0;
  QString classType;
  QH5Lite::readStringAttribute(gid, name, SIMPL::HDF5::ObjectType, classType);
  //   qDebug() << groupName << " Array: " << *iter << " with C++ ClassType of " << classType << "\n";
  IDataArray::Pointer dPtr = IDataArray::NullPointer();

  if(classType.startsWith("DataArray") == true)
  {
    dPtr = H5DataArrayReader::ReadIDataArray(gid, name, preflight);
    if(preflight == true)
    {
      dPtr->resize(getNumberOfTuples());
    }
  }
  else if(classType.compare("StringDataArray") == 0)
  {
    dPtr = H5DataArrayReader::ReadStringDataArray(gid, name, preflight);
    if(preflight == true)
    {
      dPtr->resize(getNumberOfTuples());
    }
  }
  else if(classType.compare("vector") == 0)
  {
  }
  else if(classType.compare("NeighborList<T>") == 0)
  {
    dPtr = H5DataArrayReader::ReadNeighborListData(gid, name, preflight);
    if(preflight == true)
    {
      dPtr->resize(getNumberOfTuples());
    }
  }
  else if(name.compare(SIMPL::EnsembleData::Statistics) == 0)
  {
    StatsDataArray::Pointer statsData = StatsDataArray::New();
    statsData->setName(SIMPL::EnsembleData::Statistics);
    statsData->readH5Data(gid);
    dPtr = statsData;
    if(preflight == true)
    {
      dPtr->resize(getNumberOfTuples());
    }
  }

  if(nullptr != dPtr.get())
  {
    addAttributeArray(dPtr->getName(), dPtr);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::readAttributeArraysFromHDF5(hid_t amGid, bool preflight, AttributeMatrixProxy* attrMatProxy)
{
  int err = 0;
  QMap<QString, DataArrayProxy> dasToRead = attrMatProxy->dataArrays;
  QString classType;
  for(QMap<QString, DataArrayProxy>::iterator iter = dasToRead.begin(); iter != dasToRead.end(); ++iter)
  {
    // qDebug() << "Reading the " << iter->name << " Array from the " << m_Name << " Attribute Matrix \n";
    if(iter->flag == SIMPL::Unchecked)
    {
      continue;
    }
    QH5Lite::readStringAttribute(amGid, iter->name, SIMPL::HDF5::ObjectType, classType);
    //   qDebug() << groupName << " Array: " << *iter << " with C++ ClassType of " << classType << "\n";
    IDataArray::Pointer dPtr = IDataArray::NullPointer();

    if(classType.startsWith("DataArray") == true)
    {
      dPtr = H5DataArrayReader::ReadIDataArray(amGid, iter->name, preflight);
    }
    else if(classType.compare("StringDataArray") == 0)
    {
      dPtr = H5DataArrayReader::ReadStringDataArray(amGid, iter->name, preflight);
    }
    else if(classType.compare("vector") == 0)
    {
    }
    else if(classType.compare("NeighborList<T>") == 0)
    {
      dPtr = H5DataArrayReader::ReadNeighborListData(amGid, iter->name, preflight);
    }
    else if(classType.compare("Statistics") == 0)
    {
      StatsDataArray::Pointer statsData = StatsDataArray::New();
      statsData->setName(iter->name);
      statsData->readH5Data(amGid);
      dPtr = statsData;
    }
    //    else if ( (iter->name).compare(SIMPL::EnsembleData::Statistics) == 0)
    //    {
    //      StatsDataArray::Pointer statsData = StatsDataArray::New();
    //      statsData->setName(SIMPL::EnsembleData::Statistics);
    //      statsData->readH5Data(amGid);
    //      dPtr = statsData;
    //    }

    if(nullptr != dPtr.get())
    {
      addAttributeArray(dPtr->getName(), dPtr);
    }
  }
  H5Gclose(amGid); // Close the Cell Group
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::generateXdmfText(const QString& centering, const QString& dataContainerName, const QString& hdfFileName, const uint8_t gridType)
{
  QString xdmfText;
  QString block;
  QTextStream out(&xdmfText);

  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
  {
    IDataArray::Pointer d = iter.value();
    block = writeXdmfAttributeData(d, centering, dataContainerName, hdfFileName, gridType);
    out << block;
  }
  return xdmfText;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::getInfoString(SIMPL::InfoStringFormat format)
{
  QString info;
  QTextStream ss(&info);
  if(format == SIMPL::HtmlFormat)
  {
    ss << "<html><head></head>\n";
    ss << "<body>\n";
    ss << "<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\">\n";
    ss << "<tbody>\n";
    ss << "<tr bgcolor=\"#FFFCEA\"><th colspan=2>Attribute Matrix Info</th></tr>";

    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Name:</th><td>" << getName() << "</td></tr>";

    QString typeString;
    switch(m_Type)
    {
    case AttributeMatrix::Type::Vertex:
      typeString = "Vertex";
      break;
    case AttributeMatrix::Type::Edge:
      typeString = "Edge";
      break;
    case AttributeMatrix::Type::Face:
      typeString = "Face";
      break;
    case AttributeMatrix::Type::Cell:
      typeString = "Cell";
      break;
    case AttributeMatrix::Type::VertexFeature:
      typeString = "Vertex Feature";
      break;
    case AttributeMatrix::Type::EdgeFeature:
      typeString = "Edge Feature";
      break;
    case AttributeMatrix::Type::FaceFeature:
      typeString = "Face Feature";
      break;
    case AttributeMatrix::Type::CellFeature:
      typeString = "Cell Feature";
      break;
    case AttributeMatrix::Type::VertexEnsemble:
      typeString = "Vertex Ensemble";
      break;
    case AttributeMatrix::Type::EdgeEnsemble:
      typeString = "Edge Ensemble";
      break;
    case AttributeMatrix::Type::FaceEnsemble:
      typeString = "Face Ensemble";
      break;
    case AttributeMatrix::Type::CellEnsemble:
      typeString = "Cell Ensemble";
      break;
    case AttributeMatrix::Type::MetaData:
      typeString = "MetaData";
      break;
    case AttributeMatrix::Type::Generic:
      typeString = "Generic";
      break;
    default:
      typeString = "Unknown";
      break;
    }

    QLocale usa(QLocale::English, QLocale::UnitedStates);
    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Type:</th><td>" << typeString << "</td></tr>";
    QString tupleStr = "(";
    for(int i = 0; i < m_TupleDims.size(); i++)
    {
      QString numStr = usa.toString(static_cast<qlonglong>(m_TupleDims[i]));
      tupleStr = tupleStr + numStr;
      if(i < m_TupleDims.size() - 1)
      {
        tupleStr = tupleStr + QString(", ");
      }
    }
    tupleStr = tupleStr + ")";
    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Tuple Dimensions:</th><td>" << tupleStr << "</td></tr>";

    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Attribute Array Count:</th><td>" << getNumAttributeArrays() << "</td></tr>";
    ss << "</tbody></table>\n";
    ss << "</body></html>";
  }
  else
  {
  }
  return info;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::writeXdmfAttributeDataHelper(int numComp, const QString& attrType, const QString& dataContainerName, IDataArray::Pointer array, const QString& centering, int precision,
                                                      const QString& xdmfTypeName, const QString& hdfFileName, const uint8_t gridType)
{
  QString buf;
  QTextStream out(&buf);

  QString tupleStr;
  for(int i = m_TupleDims.size() - 1; i >= 0; i--)
  {
    tupleStr = tupleStr + QString::number(m_TupleDims[i]) + QString(" ");
  }
  QString dimStr = tupleStr + QString::number(array->getNumberOfComponents());
  QString dimStrHalf = tupleStr + QString::number(array->getNumberOfComponents() / 2);

  if(numComp == 1 || numComp == 3 || numComp == 9)
  //  if(numComp == 1 || numComp == 3 || numComp == 6 || numComp == 9)
  {
    out << "    <Attribute Name=\"" << array->getName() << "\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    out << "Center=\"" << centering << "\">"
        << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr << "\" ";
    out << "NumberType=\"" << xdmfTypeName << "\" "
        << "Precision=\"" << precision << "\" >"
        << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << dataContainerName << "/" << getName() << "/" << array->getName() << "\n";
    out << "      </DataItem>"
        << "\n";
    out << "    </Attribute>"
        << "\n";
  }
  else if(numComp == 2 || numComp == 6)
  //  else if(numComp == 2)
  {
    // First Slab
    out << "    <Attribute Name=\"" << array->getName() << " (Feature 0)\" ";
    out << "AttributeType=\"" << attrType << "\" ";

    out << "Center=\"" << centering << "\">"
        << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStrHalf << "\" ";
    out << "Type=\"HyperSlab\" "
        << "Name=\"" << array->getName() << " (Feature 0)\" >"
        << "\n";
    out << "        <DataItem Dimensions=\"3 2\" "
        << "Format=\"XML\" >"
        << "\n";
    out << "          0        0"
        << "\n";
    out << "          1        1"
        << "\n";
    out << "          " << dimStrHalf << " </DataItem>"
        << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr << "\" "
        << "NumberType=\"" << xdmfTypeName << "\" "
        << "Precision=\"" << precision << "\" >"
        << "\n";

    out << "        " << hdfFileName << ":/DataContainers/" << dataContainerName << "/" << getName() << "/" << array->getName() << "\n";
    out << "        </DataItem>"
        << "\n";
    out << "      </DataItem>"
        << "\n";
    out << "    </Attribute>"
        << "\n"
        << "\n";

    // Second Slab
    out << "    <Attribute Name=\"" << array->getName() << " (Feature 1)\" ";
    out << "AttributeType=\"" << attrType << "\" ";

    out << "Center=\"" << centering << "\">"
        << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStrHalf << "\" ";
    out << "Type=\"HyperSlab\" "
        << "Name=\"" << array->getName() << " (Feature 1)\" >"
        << "\n";
    out << "        <DataItem Dimensions=\"3 2\" "
        << "Format=\"XML\" >"
        << "\n";
    out << "          0        " << (array->getNumberOfComponents() / 2) << "\n";
    out << "          1        1"
        << "\n";
    out << "          " << dimStrHalf << " </DataItem>"
        << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr << "\" "
        << "NumberType=\"" << xdmfTypeName << "\" "
        << "Precision=\"" << precision << "\" >"
        << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << dataContainerName << "/" << getName() << "/" << array->getName() << "\n";
    out << "        </DataItem>"
        << "\n";
    out << "      </DataItem>"
        << "\n";
    out << "    </Attribute>"
        << "\n";
  }
  return buf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::writeXdmfAttributeData(IDataArray::Pointer array, const QString& centering, const QString& dataContainerName, const QString& hdfFileName, const uint8_t gridType)
{
  QString xdmfText;
  QTextStream out(&xdmfText);

  int precision = 0;
  QString xdmfTypeName;
  array->getXdmfTypeAndSize(xdmfTypeName, precision);
  if(0 == precision)
  {
    out << "<!-- " << array->getName() << " has unkown type or unsupported type or precision for XDMF to understand"
        << " -->"
        << "\n";
    return xdmfText;
  }
  int numComp = array->getNumberOfComponents();
  QString attrType = "";
  if(numComp == 1)
  {
    attrType = "Scalar";
  }
  // we are assuming a component of 2 is for scalars on either side of a single object (ie faceIds)
  if(numComp == 2)
  {
    attrType = "Scalar";
  }
  if(numComp == 3)
  {
    attrType = "Vector";
  }
  if(numComp == 6)
  {
    attrType = "Vector";
  }
  //  if(numComp == 6) { attrType = "Tensor6"; }
  if(numComp == 9)
  {
    attrType = "Tensor";
  }
  QString block = writeXdmfAttributeDataHelper(numComp, attrType, dataContainerName, array, centering, precision, xdmfTypeName, hdfFileName, gridType);

  out << block;

  return xdmfText;
}
