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

#include "StatsData.h"

#include <QtCore/QString>
#include <vector>

#include "H5Support/H5Utilities.h"

#include "SIMPLib/HDF5/H5StatsDataDelegate.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::StatsData() :
m_Name("StatsData")
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::~StatsData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsData::getStatsType()
{
  return SIMPL::StringConstants::StatsData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseType::Type StatsData::getPhaseType()
{
  return PhaseType::Type::Unknown;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorOfFloatArray StatsData::DeepCopyVectorOFloatArray(VectorOfFloatArray& arrays)
{
  size_t count = arrays.size();
  VectorOfFloatArray copies(count);
  for(size_t c = 0; c < count; c++)
  {
    if(nullptr != arrays[c])
    {
      FloatArrayType::Pointer ptr = std::dynamic_pointer_cast<FloatArrayType>(arrays[c]->deepCopy());
      copies[c] = ptr;
    }
  }
  return copies;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorOfFloatArray StatsData::CreateCorrelatedDistributionArrays(uint32_t distributionType, size_t numBins)
{
  VectorOfFloatArray v;
  if(distributionType == SIMPL::DistributionType::Beta)
  {
    v.resize(SIMPL::DistributionType::BetaColumnCount);
    v[0] = FloatArrayType::CreateArray(numBins, SIMPL::StringConstants::Alpha);
    v[0]->initializeWithZeros();
    v[1] = FloatArrayType::CreateArray(numBins, SIMPL::StringConstants::Beta);
    v[1]->initializeWithZeros();
  }
  else if(distributionType == SIMPL::DistributionType::LogNormal)
  {
    v.resize(SIMPL::DistributionType::LogNormalColumnCount);
    v[0] = FloatArrayType::CreateArray(numBins, SIMPL::StringConstants::Average);
    v[0]->initializeWithZeros();
    v[1] = FloatArrayType::CreateArray(numBins, SIMPL::StringConstants::StandardDeviation);
    v[1]->initializeWithZeros();
  }
  else if(distributionType == SIMPL::DistributionType::Power)
  {
    v.resize(SIMPL::DistributionType::PowerLawColumnCount);
    v[0] = FloatArrayType::CreateArray(numBins, SIMPL::StringConstants::Alpha);
    v[0]->initializeWithZeros();
    v[1] = FloatArrayType::CreateArray(numBins, SIMPL::StringConstants::MinimumValue);
    v[1]->initializeWithZeros();
  }
  else if(distributionType == SIMPL::DistributionType::UnknownDistributionType)
  {
    v.resize(SIMPL::DistributionType::UnknownColumCount);
  }
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer StatsData::CreateDistributionArrays(uint32_t distributionType)
{
  FloatArrayType::Pointer v;
  if(distributionType == SIMPL::DistributionType::Beta)
  {
    v = FloatArrayType::CreateArray(SIMPL::DistributionType::BetaColumnCount, SIMPL::StringConstants::Feature_Size_Distribution);
    v->initializeWithZeros();
  }
  else if(distributionType == SIMPL::DistributionType::LogNormal)
  {
    v = FloatArrayType::CreateArray(SIMPL::DistributionType::LogNormalColumnCount, SIMPL::StringConstants::Feature_Size_Distribution);
    v->initializeWithZeros();
  }
  else if(distributionType == SIMPL::DistributionType::Power)
  {
    v = FloatArrayType::CreateArray(SIMPL::DistributionType::PowerLawColumnCount, SIMPL::StringConstants::Feature_Size_Distribution);
    v->initializeWithZeros();
  }
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::Pointer StatsData::deepCopy()
{
  return StatsData::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsData::initialize()
{
  // These should be implemented in the subclasses.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::writeHDF5Data(hid_t groupId)
{
  int err = 0;
  Q_UNUSED(groupId)
  // These should be implemented in the subclasses.
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::readHDF5Data(hid_t groupId)
{
  int err = 0;
  Q_UNUSED(groupId)
  // These should be implemented in the subclasses.
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsData::writeJson(QJsonObject& json)
{
  Q_UNUSED(json)
  // These should be implemented in the subclasses.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsData::readJson(const QJsonObject& json)
{
  Q_UNUSED(json)
  // These should be implemented in the subclasses.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsData::generateJsonArrays(VectorOfFloatArray& arrays, QJsonObject& json)
{

  for(int i = 0; i < arrays.size(); i++)
  {
    DataArray<float>::Pointer array = arrays[i];
    json.insert(array->getName(), generateJsonArrayFromDataArray<float>(array));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsData::decodeDistributionType(int disType)
{
  QString disTypeStr;
  switch(disType)
  {
  case SIMPL::DistributionType::Beta:
    disTypeStr = SIMPL::StringConstants::BetaDistribution;
    break;
  case SIMPL::DistributionType::LogNormal:
    disTypeStr = SIMPL::StringConstants::LogNormalDistribution;
    break;
  //    case SIMPL::DistributionType::Power:
  //      disTypeStr = SIMPL::StringConstants::PowerLawDistribution;
  //      break;
  case SIMPL::DistributionType::UnknownDistributionType:
    disTypeStr = SIMPL::StringConstants::UnknownDistribution;
    break;
  default:
    disTypeStr = SIMPL::StringConstants::UnknownDistribution;
  }
  return disTypeStr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::EncodeDistributionType(QString distTypeName)
{
  int distType = SIMPL::DistributionType::UnknownDistributionType;
  if(distTypeName == SIMPL::StringConstants::BetaDistribution)
  {
    distType = SIMPL::DistributionType::Beta;
  }
  else if(distTypeName == SIMPL::StringConstants::LogNormalDistribution)
  {
    distType = SIMPL::DistributionType::LogNormal;
  }
  else if(distTypeName == SIMPL::StringConstants::UnknownDistribution)
  {
    distType = SIMPL::DistributionType::UnknownDistributionType;
  }

  return distType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList StatsData::ArrayNamesFromDistributionType(const QString& disType)
{
  QStringList disTypeStr;
  if(disType.compare(SIMPL::StringConstants::BetaDistribution) == 0)
  {
    disTypeStr << SIMPL::StringConstants::Alpha << SIMPL::StringConstants::Beta;
  }
  if(disType.compare(SIMPL::StringConstants::LogNormalDistribution) == 0)
  {
    disTypeStr << SIMPL::StringConstants::Average << SIMPL::StringConstants::StandardDeviation;
  }
  //    case SIMPL::DistributionType::Power:
  //      disTypeStr << SIMPL::StringConstants::Alpha << SIMPL::StringConstants::Exp_k << SIMPL::StringConstants::Beta;
  //      break;
  return disTypeStr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsData::writeJsonDistributionArrays(QJsonObject& json, VectorOfFloatArray arrays, QString key, int disType)
{
  QJsonObject obj;
  generateJsonArrays(arrays, obj);

  if(disType != SIMPL::DistributionType::UnknownDistributionType)
  {
    obj.insert(SIMPL::StringConstants::DistributionType, decodeDistributionType(disType));
  }
  json.insert(key, obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> typename DataArray<T>::Pointer decodeDataArrayFromJsonDoubleArray(const QString& name, const QJsonArray& array)
{
  int count = array.count();
  typename DataArray<T>::Pointer dataArray = DataArray<T>::CreateArray(count, name, true);
  for(int i = 0; i < count; i++)
  {
    dataArray->setValue(i, array[i].toDouble(0.0));
  }
  return dataArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> typename DataArray<T>::Pointer decodeDataArrayFromJsonIntArray(const QString& name, const QJsonArray& array)
{
  int count = array.count();
  typename DataArray<T>::Pointer dataArray = DataArray<T>::CreateArray(count, name, true);
  for(int i = 0; i < count; i++)
  {
    dataArray->setValue(i, array[i].toInt(0));
  }
  return dataArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorOfFloatArray StatsData::ReadJsonDistributionArrays(const QJsonObject& json, const QString& key, int& disType)
{
  Q_UNUSED(disType)
  // These should be implemented in the subclasses.
  // We assume that all of the arrays are encoded as "double" values in the JSON
  // file and not ints.
  VectorOfFloatArray arrays;
  QJsonObject jDistribution = json[key].toObject();
  if(jDistribution.isEmpty())
  {
    return arrays;
  }
  // Determine the Distribution Type which will tell us the names of the arrays to use:
  QJsonValue jValue = jDistribution[SIMPL::StringConstants::DistributionType];
  if(jValue.isUndefined())
  {
    return arrays;
  }
  QString disTypeString = jValue.toString(SIMPL::StringConstants::UnknownDistribution);
  disType = StatsData::EncodeDistributionType(disTypeString);

  QStringList arrayNames = StatsData::ArrayNamesFromDistributionType(disTypeString);
  foreach(const QString arrayName, arrayNames)
  {
    QJsonArray data = jDistribution[arrayName].toArray();
    DataArray<float>::Pointer array = decodeDataArrayFromJsonDoubleArray<float>(arrayName, data);
    arrays.push_back(array);
  }

  return arrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorOfFloatArray StatsData::ReadJsonVectorOfFloatsArrays(const QJsonObject& json, const QString& key)
{
  VectorOfFloatArray arrays;
  QJsonObject obj = json[key].toObject();
  QStringList arrayNames = obj.keys();
  foreach(const QString name, arrayNames)
  {
    QJsonValue value = obj[name];
    if(value.isArray())
    {
      QJsonArray array = value.toArray();
      int count = array.count();
      FloatArrayType::Pointer data = FloatArrayType::CreateArray(count, name, true);
      for(int i = 0; i < count; i++)
      {
        data->setValue(i, array[i].toDouble());
      }
      arrays.push_back(data);
    }
  }

  return arrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::ParseFloat3Vec(const QJsonObject& json, const QString key, float* values, float defaultValue)
{
  QJsonValue jsonValue = json[key];
  if(!jsonValue.isUndefined() && jsonValue.isArray())
  {
    QJsonArray array = jsonValue.toArray();
    values[0] = array.at(0).toDouble(0.0f);
    values[1] = array.at(1).toDouble(0.0f);
    values[2] = array.at(2).toDouble(0.0f);
    return 0;
  }
  else
  {
    values[0] = defaultValue;
    values[1] = defaultValue;
    values[2] = defaultValue;
  }
  return -1;
}
