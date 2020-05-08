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

#include <QtCore/QTextStream>
#include <QtCore/QDebug>

#include "SIMPLib/DataContainers/AttributeMatrix.h"

// C Includes

// C++ Includes
#include <fstream>
#include <iostream>

// HDF5 Includes
#include "H5Support/H5ScopedSentinel.h"
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
AttributeMatrix::AttributeMatrix(const std::vector<size_t>& tDims, const QString& name, AttributeMatrix::Type attrType)
: IDataStructureContainerNode(name)
, m_TupleDims(tDims)
, m_Type(attrType)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::~AttributeMatrix() = default;

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
void AttributeMatrix::ReadAttributeMatrixStructure(hid_t containerId, DataContainerProxy* dcProxy, SIMPLH5DataReaderRequirements* req, const QString& h5InternalPath)
{
  QList<QString> attributeMatrixNames;
  QH5Utilities::getGroupObjects(containerId, H5Utilities::CustomHDFDataTypes::Group, attributeMatrixNames);
  foreach(QString attributeMatrixName, attributeMatrixNames)
  {
    if(attributeMatrixName.compare(SIMPL::Geometry::Geometry) != 0)
    {
      hid_t attrMatGid = H5Gopen(containerId, attributeMatrixName.toLatin1().constData(), H5P_DEFAULT);
      if(attrMatGid < 0)
      {
        continue;
      }
      H5ScopedGroupSentinel sentinel(&attrMatGid, true);

      AttributeMatrixProxy amProxy(attributeMatrixName);
      amProxy.setName(attributeMatrixName);
      amProxy.setFlag(Qt::Unchecked);

      AttributeMatrix::EnumType amTypeTmp = static_cast<AttributeMatrix::EnumType>(AttributeMatrix::Type::Unknown);
      herr_t err = QH5Lite::readScalarAttribute(containerId, attributeMatrixName, SIMPL::StringConstants::AttributeMatrixType, amTypeTmp);
      if(err >= 0)
      {
        if(req != nullptr)
        {
          AttributeMatrix::Types amTypes = req->getAMTypes();
          if(amTypes.empty() || amTypes.contains(static_cast<AttributeMatrix::Type>(amTypeTmp)))
          {
            amProxy.setFlag(Qt::Checked);
          }
        }
        amProxy.setAMType(static_cast<AttributeMatrix::Type>(amTypeTmp));
      }
      else
      {
        std::cout << "Error Reading the AttributeMatrix Type for AttributeMatrix " << attributeMatrixName.toStdString() << std::endl;
      }

      QString h5Path = h5InternalPath + "/" + attributeMatrixName;

      // Read in the names of the Data Arrays that make up the AttributeMatrix
      AttributeMatrixProxy::StorageType daProxies = amProxy.getDataArrays();
      DataArrayProxy::ReadDataArrayStructure(attrMatGid, daProxies, req, h5Path);
      amProxy.setDataArrays(daProxies);

      // Insert the AttributeMatrixProxy proxy into the dataContainer proxy
      dcProxy->insertAttributeMatrix(attributeMatrixName, amProxy);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath AttributeMatrix::getDataArrayPath() const
{
  DataArrayPath path = getParentPath();
  path.setAttributeMatrixName(getName());
  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrix::validateAttributeArraySizes() const
{
  int64_t arraySize = 0;
  int64_t matrixSize = getNumberOfTuples();
  const AttributeMatrix::Container_t& dataArrays = getChildren();
  for(const auto& dataArray : dataArrays)
  {
    arraySize = dataArray->getNumberOfTuples();
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
IDataArray::Pointer AttributeMatrix::removeAttributeArray(const QString& name)
{
  auto it = find(name);
  if(it == end())
  {
    // DO NOT return a NullPointer for any reason other than "Data Array was not found"
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = (*it);
  erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameErrorCodes AttributeMatrix::renameAttributeArray(const QString& oldname, const QString& newname, bool overwrite)
{
  // Cannot rename a DataArray when the name does not change.
  if(oldname == newname)
  {
    return SAME_PATH;
  }

  bool hasNewName = contains(newname);
  if(hasNewName)
  {
    if(overwrite)
    {
      removeAttributeArray(newname);
    }
    else
    {
      return NEW_EXISTS;
    }
  }

  // If new name doesn't exist or we want to overwrite one that does exist...
  auto oldArray = getChildByName(oldname);
  // If old name doesn't exist...
  if(nullptr == oldArray)
  {
    return OLD_DOES_NOT_EXIST;
  }
  if(oldArray->setName(newname))
  {
    return SUCCESS;
  }

  // Rename not successful
  return NEW_EXISTS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<size_t> AttributeMatrix::getTupleDimensions() const
{
  return m_TupleDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::setTupleDimensions(const std::vector<size_t>& tupleDims)
{
  resizeAttributeArrays(tupleDims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t AttributeMatrix::getNumberOfTuples() const
{
  if(m_TupleDims.empty())
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
bool AttributeMatrix::removeInactiveObjects(const QVector<bool>& activeObjects, DataArray<int32_t>* featureIds)
{
  bool acceptableMatrix = false;
  // Only valid for feature or ensemble type matrices
  if(m_Type == AttributeMatrix::Type::VertexFeature || m_Type == AttributeMatrix::Type::VertexEnsemble || m_Type == AttributeMatrix::Type::EdgeFeature ||
     m_Type == AttributeMatrix::Type::EdgeEnsemble || m_Type == AttributeMatrix::Type::FaceFeature || m_Type == AttributeMatrix::Type::FaceEnsemble || m_Type == AttributeMatrix::Type::CellFeature ||
     m_Type == AttributeMatrix::Type::CellEnsemble)
  {
    acceptableMatrix = true;
  }
  size_t totalTuples = getNumberOfTuples();
  if(static_cast<size_t>(activeObjects.size()) == totalTuples && acceptableMatrix)
  {
    size_t goodcount = 1;
    std::vector<size_t> newNames(totalTuples, 0);
    std::vector<size_t> removeList;

    for(qint32 i = 1; i < activeObjects.size(); i++)
    {
      if(!activeObjects[i])
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

    if(!removeList.empty())
    {
      QList<QString> headers = getAttributeArrayNames();
      for(const auto& header : headers)
      {
        IDataArray::Pointer p = getAttributeArray(header);
        QString type = p->getTypeAsString();
        if(type.compare("NeighborList<T>") == 0)
        {
          removeAttributeArray(header);
        }
        else
        {
          p->eraseTuples(removeList);
        }
      }
      std::vector<size_t> tDims(1, (totalTuples - removeList.size()));
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
void AttributeMatrix::resizeAttributeArrays(const std::vector<size_t>& tDims)
{
  m_TupleDims = tDims;
  size_t numTuples = m_TupleDims[0];
  for(int i = 1; i < m_TupleDims.size(); i++)
  {
    numTuples *= m_TupleDims[i];
  }

  const AttributeMatrix::Container_t& dataArrays = getChildren();
  for(const auto& dataArray : dataArrays)
  {
    dataArray->resizeTuples(numTuples);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::clearAttributeArrays()
{
  clear();
}

AttributeMatrix::Container_t AttributeMatrix::getAttributeArrays() const
{
  return getChildren();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::NameList AttributeMatrix::getAttributeArrayNames() const
{
  return getNamesOfChildren();
}

int AttributeMatrix::getNumAttributeArrays() const
{
  return static_cast<int>(size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer AttributeMatrix::deepCopy(bool forceNoAllocate) const
{
  AttributeMatrix::Pointer newAttrMat = AttributeMatrix::New(getTupleDimensions(), getName(), getType());

  const auto& dataArrays = getChildren();
  for(const auto& d : dataArrays)
  {
    IDataArray::Pointer new_d = d->deepCopy(forceNoAllocate);
    if(new_d.get() == nullptr)
    {
      return AttributeMatrix::NullPointer();
    }
    newAttrMat->insertOrAssign(new_d);
  }

  return newAttrMat;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::writeAttributeArraysToHDF5(hid_t parentId) const
{
  int err = 0;

  const auto& dataArrays = getChildren();
  for(const auto& d : dataArrays)
  {
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
int AttributeMatrix::addAttributeArrayFromHDF5Path(hid_t gid, const QString& name, bool preflight)
{
  int err = 0;
  QString classType;
  QH5Lite::readStringAttribute(gid, name, SIMPL::HDF5::ObjectType, classType);
  //   qDebug() << groupName << " Array: " << *iter << " with C++ ClassType of " << classType << "\n";
  IDataArray::Pointer dPtr = IDataArray::NullPointer();

  if(classType.startsWith("DataArray"))
  {
    dPtr = H5DataArrayReader::ReadIDataArray(gid, name, preflight);
    if(preflight)
    {
      dPtr->resizeTuples(getNumberOfTuples());
    }
  }
  else if(classType.compare("StringDataArray") == 0)
  {
    dPtr = H5DataArrayReader::ReadStringDataArray(gid, name, preflight);
    if(preflight)
    {
      dPtr->resizeTuples(getNumberOfTuples());
    }
  }
  else if(classType.compare("vector") == 0)
  {
  }
  else if(classType.compare("NeighborList<T>") == 0)
  {
    dPtr = H5DataArrayReader::ReadNeighborListData(gid, name, preflight);
    if(preflight)
    {
      dPtr->resizeTuples(getNumberOfTuples());
    }
  }
  else if(name.compare(SIMPL::EnsembleData::Statistics) == 0)
  {
    StatsDataArray::Pointer statsData = StatsDataArray::New();
    statsData->setName(SIMPL::EnsembleData::Statistics);
    statsData->readH5Data(gid);
    dPtr = statsData;
    if(preflight)
    {
      dPtr->resizeTuples(getNumberOfTuples());
    }
  }

  if(nullptr != dPtr.get())
  {
    addOrReplaceAttributeArray(dPtr);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::readAttributeArraysFromHDF5(hid_t amGid, bool preflight, AttributeMatrixProxy* attrMatProxy)
{
  int err = 0;
  AttributeMatrixProxy::StorageType dasToRead = attrMatProxy->getDataArrays();
  QString classType;
  for(const auto& daToRead : dasToRead)
  {
    if(daToRead.getFlag() == SIMPL::Unchecked)
    {
      continue;
    }
    QH5Lite::readStringAttribute(amGid, daToRead.getName(), SIMPL::HDF5::ObjectType, classType);
    //   qDebug() << groupName << " Array: " << *iter << " with C++ ClassType of " << classType << "\n";
    IDataArray::Pointer dPtr = IDataArray::NullPointer();

    if(classType.startsWith("DataArray"))
    {
      dPtr = H5DataArrayReader::ReadIDataArray(amGid, daToRead.getName(), preflight);
    }
    else if(classType.compare("StringDataArray") == 0)
    {
      dPtr = H5DataArrayReader::ReadStringDataArray(amGid, daToRead.getName(), preflight);
    }
    else if(classType.compare("vector") == 0)
    {
    }
    else if(classType.compare("NeighborList<T>") == 0)
    {
      dPtr = H5DataArrayReader::ReadNeighborListData(amGid, daToRead.getName(), preflight);
    }
    else if(classType.compare("Statistics") == 0)
    {
      StatsDataArray::Pointer statsData = StatsDataArray::New();
      statsData->setName(daToRead.getName());
      statsData->readH5Data(amGid);
      dPtr = statsData;
    }

    if(nullptr != dPtr.get())
    {
      addOrReplaceAttributeArray(dPtr);
    }
  }
  H5Gclose(amGid); // Close the Cell Group
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::generateXdmfText(const QString& centering, const QString& dataContainerName, const QString& hdfFileName, const uint8_t gridType) const
{
  QString xdmfText;
  QString block;
  QTextStream out(&xdmfText);

  for(auto iter = begin(); iter != end(); ++iter)
  {
    IDataArray::Pointer d = (*iter);
    block = writeXdmfAttributeData(d, centering, dataContainerName, hdfFileName, gridType);
    out << block;
  }
  return xdmfText;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::getInfoString(SIMPL::InfoStringFormat format) const
{
  QString info;
  QTextStream ss(&info);
  if(format == SIMPL::HtmlFormat)
  {
    return getToolTipGenerator().generateHTML();
  }
  else
  {
  }
  return info;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ToolTipGenerator AttributeMatrix::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;

  toolTipGen.addTitle("Attribute Matrix Info");
  toolTipGen.addValue("Name", getName());

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
  toolTipGen.addValue("Type", typeString);

  QLocale usa(QLocale::English, QLocale::UnitedStates);
  QString tupleStr = "(";
  for(int i = 0; i < m_TupleDims.size(); i++)
  {
    QString numStr = usa.toString(static_cast<qlonglong>(m_TupleDims[i]));
    tupleStr = tupleStr + numStr;
    if(i < m_TupleDims.size() - 1)
    {
      tupleStr = tupleStr + QString(" x ");
    }
  }
  tupleStr = tupleStr + ")";
  toolTipGen.addValue("Tuple Dimensions", tupleStr);

  return toolTipGen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::writeXdmfAttributeDataHelper(int numComp, const QString& attrType, const QString& dataContainerName, const IDataArray::Pointer& array, const QString& centering, int precision,
                                                      const QString& xdmfTypeName, const QString& hdfFileName, uint8_t gridType) const
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
    out << R"(      <DataItem Format="HDF" Dimensions=")" << dimStr << "\" ";
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
    out << R"(      <DataItem ItemType="HyperSlab" Dimensions=")" << dimStrHalf << "\" ";
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
    out << R"(        <DataItem Format="HDF" Dimensions=")" << dimStr << "\" "
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
    out << R"(      <DataItem ItemType="HyperSlab" Dimensions=")" << dimStrHalf << "\" ";
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
    out << R"(        <DataItem Format="HDF" Dimensions=")" << dimStr << "\" "
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
QString AttributeMatrix::writeXdmfAttributeData(const IDataArray::Pointer& array, const QString& centering, const QString& dataContainerName, const QString& hdfFileName, const uint8_t gridType) const
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

// -----------------------------------------------------------------------------
AttributeMatrix::Pointer AttributeMatrix::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
QString AttributeMatrix::getNameOfClass() const
{
  return QString("AttributeMatrix");
}

// -----------------------------------------------------------------------------
QString AttributeMatrix::ClassName()
{
  return QString("AttributeMatrix");
}

// -----------------------------------------------------------------------------
void AttributeMatrix::setType(const AttributeMatrix::Type& value)
{
  m_Type = value;
}

// -----------------------------------------------------------------------------
AttributeMatrix::Type AttributeMatrix::getType() const
{
  return m_Type;
}
bool AttributeMatrix::addOrReplaceAttributeArray(const IDataArrayShPtrType& data)
{
  // Can not insert a null IDataArray object
  if(data.get() == nullptr)
  {
    return false;
  }
  if(getNumberOfTuples() != data->getNumberOfTuples())
  {
    qDebug() << "AttributeMatrix::Name: " << getName() << "  dataArray::name:  " << data->getName() << " Type: " << data->getTypeAsString();
    qDebug() << "getNumberOfTuples(): " << getNumberOfTuples() << "  data->getNumberOfTuples(): " << data->getNumberOfTuples();
  }
  Q_ASSERT(getNumberOfTuples() == data->getNumberOfTuples());
  return insertOrAssign(data);
}

IDataArrayShPtrType AttributeMatrix::getAttributeArray(const QString& name) const
{
  return getChildByName(name);
}

IDataArrayShPtrType AttributeMatrix::getAttributeArray(const DataArrayPath& path) const
{
  return getAttributeArray(path.getDataArrayName());
}

bool AttributeMatrix::doesAttributeArrayExist(const QString& name) const
{
  return contains(name);
}

// -----------------------------------------------------------------------------
IDataArray::Pointer AttributeMatrix::getPrereqIDataArray(AbstractFilter* filter, const QString& attributeArrayName, int err) const
{
  QString ss;
  IDataArray::Pointer attributeArray = nullptr;
  // Make sure the name is not empty for the AttributeArrayName. This would be detected below
  // in the call to get either one BUT the reason for the failure would not be evident so we make these explicit checks
  // here and send back nice error messages to ther user/programmer.
  if(attributeArrayName.isEmpty())
  {
    if(filter)
    {
      ss = QObject::tr("AttributeMatrix:'%1' The name of a requested Attribute Array was empty. Please provide a name for this array").arg(getName());
      filter->setErrorCondition(err, ss);
    }
    return attributeArray;
  }
  // Now ask for the actual AttributeArray from the AttributeMatrix
  if(!doesAttributeArrayExist(attributeArrayName))
  {
    if(filter)
    {
      ss = QObject::tr("The AttributeMatrix named '%1' does NOT have a DataArray with name '%2'. This filter requires this DataArray in order to execute.").arg(getName()).arg(attributeArrayName);
      filter->setErrorCondition(err, ss);
    }
    return attributeArray;
  }

  attributeArray = getAttributeArray(attributeArrayName);

  if(attributeArray == nullptr)
  {
    ss = QObject::tr("Unable to cast input array %1 to the necessary type.").arg(attributeArrayName);
    filter->setErrorCondition(err, ss);
  }

  return attributeArray;
}
