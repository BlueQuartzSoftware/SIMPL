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

#include "StatsDataArray.h"

#include <QtCore/QList>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "SIMPLib/Common/PhaseType.h"
#include "SIMPLib/StatsData/BoundaryStatsData.h"
#include "SIMPLib/StatsData/MatrixStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"
#include "SIMPLib/HDF5/H5DataArrayWriter.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::StatsDataArray()
: IDataArray(SIMPL::EnsembleData::Statistics)
{
  m_IsAllocated = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::~StatsDataArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::Pointer StatsDataArray::CreateArray(size_t numElements, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  StatsDataArray::Pointer ptr = StatsDataArray::New();
  ptr->setName(name);
  std::vector<PhaseType::Type> phase_types(numElements, PhaseType::Type::Unknown);
  if(allocate && numElements > 0)
  {
    ptr->fillArrayWithNewStatsData(numElements, &(phase_types.front()));
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::Pointer StatsDataArray::CreateArray(size_t numTuples, int rank, const size_t* dims, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  StatsDataArray::Pointer ptr = StatsDataArray::New();
  ptr->setName(name);
  std::vector<PhaseType::Type> phase_types(numTuples, PhaseType::Type::Unknown);
  if(allocate)
  {
    ptr->fillArrayWithNewStatsData(numTuples, &(phase_types.front()));
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::Pointer StatsDataArray::CreateArray(size_t numTuples, const std::vector<size_t>& cDims, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  StatsDataArray::Pointer ptr = StatsDataArray::New();
  ptr->setName(name);
  std::vector<PhaseType::Type> phase_types(numTuples, PhaseType::Type::Unknown);
  if(allocate)
  {
    ptr->fillArrayWithNewStatsData(numTuples, &(phase_types.front()));
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::Pointer StatsDataArray::CreateArray(const std::vector<size_t>& tDims, const std::vector<size_t>& cDims, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  size_t numTuples = tDims[0];
  qint32 size = tDims.size();
  for(qint32 iter = 1; iter < size; iter++)
  {
    numTuples *= tDims[iter];
  }
  StatsDataArray::Pointer ptr = StatsDataArray::New();
  ptr->setName(name);
  std::vector<PhaseType::Type> phase_types(numTuples, PhaseType::Type::Unknown);
  if(allocate)
  {
    ptr->fillArrayWithNewStatsData(numTuples, &(phase_types.front()));
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer StatsDataArray::createNewArray(size_t numElements, int rank, const size_t* dims, const QString& name, bool allocate) const
{
  return StatsDataArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer StatsDataArray::createNewArray(size_t numElements, const std::vector<size_t>& dims, const QString& name, bool allocate) const
{
  return StatsDataArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsDataArray::getFullNameOfClass() const
{
  return QString("Statistics");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::takeOwnership()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::releaseOwnership()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* StatsDataArray::getVoidPointer(size_t i)
{
#ifndef NDEBUG
  if(!m_StatsDataArray.empty())
  {
    Q_ASSERT(i < static_cast<size_t>(m_StatsDataArray.size()));
  }
#endif
  if(i >= this->getNumberOfTuples())
  {
    return nullptr;
  }
  return (void*)(&(m_StatsDataArray[i]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t StatsDataArray::getNumberOfTuples() const
{
  return m_StatsDataArray.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t StatsDataArray::getSize() const
{
  return m_StatsDataArray.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::getNumberOfComponents() const
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<size_t> StatsDataArray::getComponentDimensions() const
{
  std::vector<size_t> dims = {1};
  return dims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t StatsDataArray::getTypeSize() const
{
  return sizeof(StatsData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::eraseTuples(const std::vector<size_t>& idxs)
{
  int err = 0;

  // If nothing is to be erased just return
  if(idxs.empty())
  {
    return 0;
  }

  if(static_cast<size_t>(idxs.size()) >= getNumberOfTuples())
  {
    resizeTuples(0);
    return 0;
  }

  // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
  // off the end of the array and return an error code.
  for(const auto& idx : idxs)
  {
    if(idx >= static_cast<size_t>(m_StatsDataArray.size()))
    {
      return -100;
    }
  }

  QVector<StatsData::Pointer> replacement(m_StatsDataArray.size() - idxs.size());
  size_t idxsIndex = 0;
  size_t rIdx = 0;
  size_t size = static_cast<size_t>(m_StatsDataArray.size());
  for(size_t dIdx = 0; dIdx < size; ++dIdx)
  {
    if(dIdx != idxs[idxsIndex])
    {
      replacement[rIdx] = m_StatsDataArray[dIdx];
      ++rIdx;
    }
    else
    {
      ++idxsIndex;
      if(idxsIndex == static_cast<size_t>(idxs.size()))
      {
        idxsIndex--;
      }
    }
  }
  m_StatsDataArray = replacement;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::copyTuple(size_t currentPos, size_t newPos)
{
  m_StatsDataArray[newPos] = m_StatsDataArray[currentPos];
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StatsDataArray::copyFromArray(size_t destTupleOffset, IDataArray::ConstPointer sourceArray, size_t srcTupleOffset, size_t totalSrcTuples)
{
  if(!m_IsAllocated)
  {
    return false;
  }
  if(m_StatsDataArray.empty())
  {
    return false;
  }
  if(destTupleOffset >= m_StatsDataArray.size())
  {
    return false;
  }
  if(!sourceArray->isAllocated())
  {
    return false;
  }
  const Self* source = dynamic_cast<const Self*>(sourceArray.get());

  if(source == nullptr)
  {
    return false;
  }

  if(sourceArray->getNumberOfComponents() != getNumberOfComponents())
  {
    return false;
  }

  if(srcTupleOffset + totalSrcTuples > sourceArray->getNumberOfTuples())
  {
    return false;
  }

  if(totalSrcTuples * sourceArray->getNumberOfComponents() + destTupleOffset * getNumberOfComponents() > m_StatsDataArray.size())
  {
    return false;
  }

  for(size_t i = srcTupleOffset; i < srcTupleOffset + totalSrcTuples; i++)
  {
    m_StatsDataArray[destTupleOffset + i] = (*source)[i];
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::initializeTuple(size_t i, const void* p)
{
  Q_ASSERT(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::initializeWithZeros()
{
  Q_ASSERT(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer StatsDataArray::deepCopy(bool forceNoAllocate) const
{
  StatsDataArray::Pointer daCopyPtr = StatsDataArray::CreateArray(getNumberOfTuples() * getNumberOfComponents(), getName());

  daCopyPtr->resizeTuples(getNumberOfTuples());
  for(size_t i = 0; i < getNumberOfTuples(); i++)
  {
    // This should be a Deep Copy of each of the StatsData subclasses instead of a reference copy
    if(nullptr != m_StatsDataArray[i])
    {
      daCopyPtr->setStatsData(i, m_StatsDataArray[i]->deepCopy());
    }
    else
    {
      daCopyPtr->setStatsData(i, StatsData::NullPointer());
    }
  }

  return daCopyPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t StatsDataArray::resizeTotalElements(size_t size)
{
  m_StatsDataArray.resize(size);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::resizeTuples(size_t numTuples)
{
  resizeTotalElements(numTuples);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::printTuple(QTextStream& out, size_t i, char delimiter) const
{
  Q_ASSERT(false);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::printComponent(QTextStream& out, size_t i, int j) const
{
  Q_ASSERT(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::writeH5Data(hid_t parentId, const std::vector<size_t>& tDims) const
{
  herr_t err = 0;
  hid_t gid = QH5Utilities::createGroup(parentId, getName());
  if(gid < 0)
  {
    return -1;
  }
  H5ScopedGroupSentinel scopedFileSentinel(&gid, false); // This makes sure our H5Group is closed up as we exit this function

  // We start numbering our phases at 1. Anything in slot 0 is considered "Dummy" or invalid
  for(qint32 i = 1; i < m_StatsDataArray.size(); ++i)
  {
    if(m_StatsDataArray[i] != nullptr)
    {
      QString indexString = QString::number(i);
      hid_t tupleId = QH5Utilities::createGroup(gid, indexString);
      err = QH5Lite::writeStringAttribute(gid, indexString, SIMPL::StringConstants::StatsType, m_StatsDataArray[i]->getStatsType());

      PhaseType::EnumType phaseType = static_cast<PhaseType::EnumType>(m_StatsDataArray[i]->getPhaseType());
      err = QH5Lite::writeScalarAttribute(gid, indexString, SIMPL::StringConstants::PhaseType, phaseType);
      err = m_StatsDataArray[i]->writeHDF5Data(tupleId);
      err = QH5Utilities::closeHDF5Object(tupleId);
    }
  }

  std::vector<size_t> cDims(1, 1);
  err = H5DataArrayWriter::writeDataArrayAttributes<StatsDataArray>(parentId, this, tDims, cDims);

  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::readH5Data(hid_t parentId)
{
  bool ok = false;
  int err = 0;
  QString statsType;
  hid_t gid = QH5Utilities::openHDF5Object(parentId, getName());
  if(gid < 0)
  {
    return err;
  }

  QList<QString> names;
  err = QH5Utilities::getGroupObjects(gid, H5Utilities::CustomHDFDataTypes::Group, names);
  if(err < 0)
  {
    err |= QH5Utilities::closeHDF5Object(gid);
    return err;
  }

  for(QList<QString>::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    int index = 0;
    statsType = "";

    index = QString(*iter).toInt(&ok, 10);
    QH5Lite::readStringAttribute(gid, *iter, SIMPL::StringConstants::StatsType, statsType);
    hid_t statId = QH5Utilities::openHDF5Object(gid, *iter);
    if(statId < 0)
    {
      continue;
      err |= -1;
    }
    if(statsType.compare(SIMPL::StringConstants::PrimaryStatsData) == 0)
    {
      PrimaryStatsData::Pointer data = PrimaryStatsData::New();
      data->readHDF5Data(statId);
      setStatsData(index, data);
    }
    else if(statsType.compare(SIMPL::StringConstants::PrecipitateStatsData) == 0)
    {
      PrecipitateStatsData::Pointer data = PrecipitateStatsData::New();
      data->readHDF5Data(statId);
      setStatsData(index, data);
    }
    else if(statsType.compare(SIMPL::StringConstants::TransformationStatsData) == 0)
    {
      TransformationStatsData::Pointer data = TransformationStatsData::New();
      data->readHDF5Data(statId);
      setStatsData(index, data);
    }
    else if(statsType.compare(SIMPL::StringConstants::MatrixStatsData) == 0)
    {
      MatrixStatsData::Pointer data = MatrixStatsData::New();
      data->readHDF5Data(statId);
      setStatsData(index, data);
    }
    else if(statsType.compare(SIMPL::StringConstants::BoundaryStatsData) == 0)
    {
      BoundaryStatsData::Pointer data = BoundaryStatsData::New();
      data->readHDF5Data(statId);
      setStatsData(index, data);
    }
    else
    {
      qDebug() << "The Type of the stats data could not be read.";
      return -1100;
    }
    err |= QH5Utilities::closeHDF5Object(statId);
  }

  // Do not forget to close the object
  err |= QH5Utilities::closeHDF5Object(gid);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::writeToJson(QJsonObject& jsonRoot, UInt32ArrayType::Pointer crystalStructures) const
{
  int error = 0;

  QJsonObject statsObj{{"Phase Count", static_cast<double>(getNumberOfTuples())}, {"Name", SIMPL::EnsembleData::Statistics}};

  for(size_t i = 1; i < getNumberOfTuples(); i++)
  {
    StatsData::Pointer statsData = getStatsData(i);
    if(nullptr != statsData.get())
    {
      QJsonObject phaseObj;
      statsData->writeJson(phaseObj);
      if(crystalStructures)
      {
        phaseObj.insert(SIMPL::EnsembleData::CrystalSymmetry, static_cast<int>(crystalStructures->getValue(i)));
      }
      statsObj.insert(QString::number(i), phaseObj);
    }
  }

  jsonRoot.insert(QString("StatsDataArray"), statsObj);

  return error;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::readFromJson(const QJsonObject& jsonRoot)
{
  QJsonObject statsObject = jsonRoot["StatsDataArray"].toObject();

  int phaseCount = statsObject["Phase Count"].toInt();
  QString arrayName = statsObject["Name"].toString();
  setName(arrayName);
  resizeTotalElements(phaseCount);

  // Start from index 1. Index 0 is always junk.
  for(int index = 1; index < phaseCount; index++)
  {
    QString phaseAsString = QString::number(index);
    QJsonObject phaseObject = statsObject[phaseAsString].toObject();
    QString statsType = phaseObject[SIMPL::StringConstants::PhaseType].toString();

    if(statsType.compare(PhaseType::PrimaryStr()) == 0)
    {
      PrimaryStatsData::Pointer data = PrimaryStatsData::New();
      data->readJson(phaseObject);
      setStatsData(index, data);
    }
    else if(statsType.compare(PhaseType::PrecipitateStr()) == 0)
    {
      PrecipitateStatsData::Pointer data = PrecipitateStatsData::New();
      data->readJson(phaseObject);
      setStatsData(index, data);
    }
    else if(statsType.compare(PhaseType::TransformationStr()) == 0)
    {
      TransformationStatsData::Pointer data = TransformationStatsData::New();
      data->readJson(phaseObject);
      setStatsData(index, data);
    }
    else if(statsType.compare(PhaseType::MatrixStr()) == 0)
    {
      MatrixStatsData::Pointer data = MatrixStatsData::New();
      data->readJson(phaseObject);
      setStatsData(index, data);
    }
    else if(statsType.compare(PhaseType::BoundaryStr()) == 0)
    {
      BoundaryStatsData::Pointer data = BoundaryStatsData::New();
      data->readJson(phaseObject);
      setStatsData(index, data);
    }
    else
    {
      qDebug() << "While reading a StatsDataArray object from a Json Object the type\n"
               << "of StatsData object did not match any known types. The type retrieved from the\n"
               << "JSON object was '" << statsType << "'. The known types are:";
      qDebug() << SIMPL::StringConstants::PrimaryStatsData;
      qDebug() << SIMPL::StringConstants::PrecipitateStatsData;
      qDebug() << SIMPL::StringConstants::TransformationStatsData;
      qDebug() << SIMPL::StringConstants::MatrixStatsData;
      qDebug() << SIMPL::StringConstants::BoundaryStatsData;
      return -1100;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::writeXdmfAttribute(QTextStream& out, const int64_t* volDims, const QString& hdfFileName, const QString& groupPath, const QString& labelb) const
{
  out << "<!-- Xdmf is not supported for " << getNameOfClass() << " with type " << getTypeAsString() << " --> ";
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsDataArray::getInfoString(SIMPL::InfoStringFormat format) const
{
  if(format == SIMPL::HtmlFormat)
  {
    return getToolTipGenerator().generateHTML();
  }
  else
  {
  }
  return QString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ToolTipGenerator StatsDataArray::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;
  QLocale usa(QLocale::English, QLocale::UnitedStates);

  toolTipGen.addTitle("Attribute Array Info");
  toolTipGen.addValue("Name", getName());
  toolTipGen.addValue("Type", getTypeAsString());
  toolTipGen.addValue("Number of Tuples", usa.toString(static_cast<qlonglong>(getNumberOfTuples())));

  return toolTipGen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsDataArray::getTypeAsString() const
{
  return "StatsDataArray";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::getXdmfTypeAndSize(QString& xdmfTypeName, int& precision) const
{
  xdmfTypeName = getNameOfClass();
  precision = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StatsDataArray::isAllocated() const
{
  return m_IsAllocated;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::clearAll()
{
  m_StatsDataArray.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::setStatsData(int index, StatsData::Pointer statsData)
{
  if(index >= static_cast<int>(m_StatsDataArray.size()))
  {
    size_t old = m_StatsDataArray.size();
    m_StatsDataArray.resize(index + 1);
    // Initialize with zero length Vectors
    for(int i = old; i < m_StatsDataArray.size(); ++i)
    {
      m_StatsDataArray[i] = StatsData::New();
    }
  }
  m_StatsDataArray[index] = statsData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::fillArrayWithNewStatsData(size_t n, PhaseType::Type* phase_types)
{
  m_StatsDataArray.resize(n);
  for(size_t i = 0; i < n; ++i)
  {
    if(phase_types != nullptr)
    {
      if(phase_types[i] == PhaseType::Type::Primary)
      {
        m_StatsDataArray[i] = PrimaryStatsData::New();
      }
      else if(phase_types[i] == PhaseType::Type::Precipitate)
      {
        m_StatsDataArray[i] = PrecipitateStatsData::New();
      }
      else if(phase_types[i] == PhaseType::Type::Transformation)
      {
        m_StatsDataArray[i] = TransformationStatsData::New();
      }
      else if(phase_types[i] == PhaseType::Type::Boundary)
      {
        m_StatsDataArray[i] = BoundaryStatsData::New();
      }
      else if(phase_types[i] == PhaseType::Type::Matrix)
      {
        m_StatsDataArray[i] = MatrixStatsData::New();
      }
      else
      {
        m_StatsDataArray[i] = StatsData::New();
      }
    }
    if(phase_types == nullptr)
    {
      m_StatsDataArray[i] = StatsData::New();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::fillArrayWithNewStatsData(size_t n, PhaseType::EnumType* phase_types)
{
  m_StatsDataArray.resize(n);
  for(size_t i = 0; i < n; ++i)
  {
    if(phase_types != nullptr)
    {
      if(phase_types[i] == static_cast<PhaseType::EnumType>(PhaseType::Type::Primary))
      {
        m_StatsDataArray[i] = PrimaryStatsData::New();
      }
      else if(phase_types[i] == static_cast<PhaseType::EnumType>(PhaseType::Type::Precipitate))
      {
        m_StatsDataArray[i] = PrecipitateStatsData::New();
      }
      else if(phase_types[i] == static_cast<PhaseType::EnumType>(PhaseType::Type::Transformation))
      {
        m_StatsDataArray[i] = TransformationStatsData::New();
      }
      else if(phase_types[i] == static_cast<PhaseType::EnumType>(PhaseType::Type::Boundary))
      {
        m_StatsDataArray[i] = BoundaryStatsData::New();
      }
      else if(phase_types[i] == static_cast<PhaseType::EnumType>(PhaseType::Type::Matrix))
      {
        m_StatsDataArray[i] = MatrixStatsData::New();
      }
      else
      {
        m_StatsDataArray[i] = StatsData::New();
      }
    }
    if(phase_types == nullptr)
    {
      m_StatsDataArray[i] = StatsData::New();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::Pointer StatsDataArray::getStatsData(int idx) const
{
#ifndef NDEBUG
  if(!m_StatsDataArray.empty())
  {
    Q_ASSERT(idx < static_cast<int>(m_StatsDataArray.size()));
  }
#endif
  return m_StatsDataArray[idx];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::Pointer StatsDataArray::operator[](int idx) const
{
#ifndef NDEBUG
  if(!m_StatsDataArray.empty())
  {
    Q_ASSERT(idx < static_cast<int>(m_StatsDataArray.size()));
  }
#endif
  return m_StatsDataArray[idx];
}

// -----------------------------------------------------------------------------
StatsDataArray::Pointer StatsDataArray::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
StatsDataArray::Pointer StatsDataArray::New()
{
  Pointer sharedPtr(new(StatsDataArray));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString StatsDataArray::getNameOfClass() const
{
  return QString("StatsDataArray");
}

// -----------------------------------------------------------------------------
QString StatsDataArray::ClassName()
{
  return QString("StatsDataArray");
}

// -----------------------------------------------------------------------------
void StatsDataArray::setStatsDataArray(const QVector<StatsData::Pointer>& value)
{
  m_StatsDataArray = value;
}

// -----------------------------------------------------------------------------
QVector<StatsData::Pointer> StatsDataArray::getStatsDataArray() const
{
  return m_StatsDataArray;
}

// -----------------------------------------------------------------------------
int StatsDataArray::getClassVersion() const
{
  return 2;
}
