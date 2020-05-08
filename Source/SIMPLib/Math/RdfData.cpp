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

#include "RdfData.h"

#include "SIMPLib/Math/RadialDistributionFunction.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/StatsData/StatsData.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RdfData::RdfData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RdfData::~RdfData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RdfData::getFrequencyCount()
{
  return m_Frequencies.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RdfData::Pointer RdfData::deepCopy()
{
  RdfData::Pointer ptr = RdfData::New();
  ptr->setFrequencies(getFrequencies());
  ptr->setMinDistance(getMinDistance());
  ptr->setMaxDistance(getMaxDistance());
  ptr->setDistributionType(getDistributionType());
  std::array<float, 3> temp = {0.0f, 0.0f, 0.0f};
  temp = getBoxSize();
  ptr->setBoxSize(temp);
  temp = getBoxResolution();
  ptr->setBoxResolution(temp);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RdfData::readJson(const QJsonObject& json)
{
  int err = 0;
  std::array<float, 3> boxDims = {0.0f, 0.0f, 0.0f};

  QJsonObject rdfJson = json[SIMPL::StringConstants::RadialDistFunc].toObject();

  QJsonValue jsonValue = rdfJson[SIMPL::StringConstants::RdfMinDistance];
  if(!jsonValue.isUndefined() && jsonValue.isDouble())
  {
    setMinDistance(static_cast<float>(jsonValue.toDouble(0.0)));
  }
  jsonValue = rdfJson[SIMPL::StringConstants::RdfMaxDistance];
  if(!jsonValue.isUndefined() && jsonValue.isDouble())
  {
    setMaxDistance(static_cast<float>(jsonValue.toDouble(0.0)));
  }

  if(StatsData::ParseFloat3Vec(rdfJson, SIMPL::StringConstants::RdfBoxDims, boxDims, 0.0) == 0)
  {
    // Throw warning
  }
  setBoxSize(boxDims);

  std::array<float, 3> boxRes = {0.0f, 0.0f, 0.0f};

  if(StatsData::ParseFloat3Vec(rdfJson, SIMPL::StringConstants::RdfBoxRes, boxRes, 0.0) == 0)
  {
    // Throw warning
  }
  setBoxResolution(boxRes);

  int numBins = 0;
  jsonValue = rdfJson[SIMPL::StringConstants::BinCount];
  if(!jsonValue.isUndefined() && jsonValue.isDouble())
  {
    numBins = jsonValue.toInt(0);
  }
  setNumberOfBins(numBins);

  std::array<float, 3> bd = {boxDims[0], boxDims[1], boxDims[2]};
  std::array<float, 3> br = {boxRes[0], boxRes[1], boxRes[2]};
  std::vector<float> freqs = RadialDistributionFunction::GenerateRandomDistribution(m_MinDistance, m_MaxDistance, numBins, bd, br);
  setFrequencies(freqs);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RdfData::writeJson(QJsonObject& json)
{
  int err = 0;
  QJsonObject rdfJson;
  rdfJson.insert(SIMPL::StringConstants::BinCount, getNumberOfBins());
  rdfJson.insert(SIMPL::StringConstants::RdfMinDistance, getMinDistance());
  rdfJson.insert(SIMPL::StringConstants::RdfMaxDistance, getMaxDistance());
  QJsonArray boxArray;
  std::array<float, 3> boxDims = getBoxSize();
  for(int i = 0; i < 3; i++)
  {
    boxArray.insert(i, boxDims[i]);
  }
  rdfJson.insert(SIMPL::StringConstants::RdfBoxDims, boxArray);

  boxDims = getBoxResolution();
  for(int i = 0; i < 3; i++)
  {
    boxArray.replace(i, boxDims[i]);
  }
  rdfJson.insert(SIMPL::StringConstants::RdfBoxRes, boxArray);

  json.insert(SIMPL::StringConstants::RadialDistFunc, rdfJson);
  return err;
}

// -----------------------------------------------------------------------------
RdfData::Pointer RdfData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
RdfData::Pointer RdfData::New()
{
  Pointer sharedPtr(new(RdfData));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString RdfData::getNameOfClass() const
{
  return QString("RdfData");
}

// -----------------------------------------------------------------------------
QString RdfData::ClassName()
{
  return QString("RdfData");
}

// -----------------------------------------------------------------------------
void RdfData::setFrequencies(const std::vector<float>& value)
{
  m_Frequencies = value;
}

// -----------------------------------------------------------------------------
std::vector<float> RdfData::getFrequencies() const
{
  return m_Frequencies;
}

// -----------------------------------------------------------------------------
void RdfData::setMinDistance(float value)
{
  m_MinDistance = value;
}

// -----------------------------------------------------------------------------
float RdfData::getMinDistance() const
{
  return m_MinDistance;
}

// -----------------------------------------------------------------------------
void RdfData::setMaxDistance(float value)
{
  m_MaxDistance = value;
}

// -----------------------------------------------------------------------------
float RdfData::getMaxDistance() const
{
  return m_MaxDistance;
}

// -----------------------------------------------------------------------------
void RdfData::setNumberOfBins(int value)
{
  m_NumberOfBins = value;
}

// -----------------------------------------------------------------------------
int RdfData::getNumberOfBins() const
{
  return m_NumberOfBins;
}

// -----------------------------------------------------------------------------
void RdfData::setDistributionType(const QString& value)
{
  m_DistributionType = value;
}

// -----------------------------------------------------------------------------
QString RdfData::getDistributionType() const
{
  return m_DistributionType;
}

// -----------------------------------------------------------------------------
void RdfData::setBoxSize(const std::array<float, 3>& value)
{
  m_BoxSize = value;
}

// -----------------------------------------------------------------------------
void RdfData::setBoxSize(float v0, float v1, float v2)
{
  m_BoxSize[0] = v0;
  m_BoxSize[1] = v1;
  m_BoxSize[2] = v2;
}

// -----------------------------------------------------------------------------
std::array<float, 3> RdfData::getBoxSize() const
{
  return m_BoxSize;
}

// -----------------------------------------------------------------------------
void RdfData::setBoxResolution(const std::array<float, 3>& value)
{
  m_BoxResolution = value;
}

// -----------------------------------------------------------------------------
void RdfData::setBoxResolution(float v0, float v1, float v2)
{
  m_BoxResolution[0] = v0;
  m_BoxResolution[1] = v1;
  m_BoxResolution[2] = v2;
}

// -----------------------------------------------------------------------------
std::array<float, 3> RdfData::getBoxResolution() const
{
  return m_BoxResolution;
}
