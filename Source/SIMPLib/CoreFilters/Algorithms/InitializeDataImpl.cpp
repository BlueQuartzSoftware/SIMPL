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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "InitializeDataImpl.h"

#include <chrono>
#include <iostream>
#include <random>
#include <thread>

#include "SIMPLib/CoreFilters/InitializeData.h"
#include "SIMPLib/DataArrays/IDataArray.h"

namespace Detail
{
template <typename T>
class UniformDistribution
{
public:
  virtual T generateValue() = 0;
};

template <typename T>
class UniformIntDistribution : public UniformDistribution<T>
{
public:
  UniformIntDistribution(T rangeMin, T rangeMax)
  {
    std::random_device randomDevice;               // Will be used to obtain a seed for the random number engine
    m_Generator = std::mt19937_64(randomDevice()); // Standard mersenne_twister_engine seeded with rd()
    std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
    m_Generator.seed(seed);
    m_Distribution = std::uniform_int_distribution<>(rangeMin, rangeMax);
  }

  T generateValue() override
  {
    return m_Distribution(m_Generator);
  }

private:
  std::uniform_int_distribution<> m_Distribution;
  std::mt19937_64 m_Generator;
};

template <typename T>
class UniformRealsDistribution : public UniformDistribution<T>
{
public:
  UniformRealsDistribution(T rangeMin, T rangeMax)
  {
    std::random_device randomDevice;               // Will be used to obtain a seed for the random number engine
    m_Generator = std::mt19937_64(randomDevice()); // Standard mersenne_twister_engine seeded with rd()
    std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
    m_Generator.seed(seed);
    m_Distribution = std::uniform_real_distribution<T>(rangeMin, rangeMax);
  }

  T generateValue() override
  {
    return m_Distribution(m_Generator);
  }

private:
  std::uniform_real_distribution<T> m_Distribution;
  std::mt19937_64 m_Generator;
};

template <typename T>
class UniformBoolDistribution : public UniformDistribution<T>
{
public:
  UniformBoolDistribution()
  {
    std::random_device randomDevice;               // Will be used to obtain a seed for the random number engine
    m_Generator = std::mt19937_64(randomDevice()); // Standard mersenne_twister_engine seeded with rd()
    std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
    m_Generator.seed(seed);
    m_Distribution = std::uniform_int_distribution<>(0, 1);
  }

  T generateValue() override
  {
    int temp = m_Distribution(m_Generator);
    return (temp != 0);
  }

private:
  std::uniform_int_distribution<> m_Distribution;
  std::mt19937_64 m_Generator;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool isPointInBounds(int64_t i, int64_t j, int64_t k, const std::array<int64_t, 6>& bounds)
{
  return (i >= bounds[0] && i <= bounds[1] && j >= bounds[2] && j <= bounds[3] && k >= bounds[4] && k <= bounds[5]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void initializeArray(IDataArray::Pointer p, const std::array<int64_t, 3>& dims, const std::array<int64_t, 6>& bounds, Detail::UniformDistribution<T>& distribution, T manualValue,
                     InitializeData::InitChoices initType, bool invertData)
{
  std::array<int64_t, 6> searchingBounds = bounds;
  if(invertData)
  {
    searchingBounds = {0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1};
  }

  for(int64_t k = searchingBounds[4]; k <= searchingBounds[5]; k++)
  {
    for(int64_t j = searchingBounds[2]; j <= searchingBounds[3]; j++)
    {
      for(int64_t i = searchingBounds[0]; i <= searchingBounds[1]; i++)
      {
        if(invertData && isPointInBounds(i, j, k, bounds))
        {
          continue;
        }

        size_t index = (k * dims[0] * dims[1]) + (j * dims[0]) + i;

        if(initType == InitializeData::Manual)
        {
          p->initializeTuple(index, &manualValue);
        }
        else
        {
          T value = distribution.generateValue();
          p->initializeTuple(index, &value);
        }
      }
    }
  }
}
} // namespace Detail

// -----------------------------------------------------------------------------
InitializeDataImpl::InitializeDataImpl(InitializeData* filter, IDataArrayShPtrType p, const std::array<int64_t, 3>& dims, const std::array<int64_t, 6>& bounds, int initType, bool invertData,
                                       double initValue, FPRangePair initRange)
: m_Filter(filter)
, m_TargetArray(p)
, m_Dims(dims)
, m_Bounds(bounds)
, m_InitType(initType)
, m_InvertData(invertData)
, m_InitValue(initValue)
, m_InitRange(initRange)
{
}

// -----------------------------------------------------------------------------
InitializeDataImpl::~InitializeDataImpl() = default;

// -----------------------------------------------------------------------------
void InitializeDataImpl::operator()() const
{
  QString type = m_TargetArray->getTypeAsString();
  if(type == "int8_t")
  {
    initializeArrayWithInts<int8_t>();
  }
  else if(type == "int16_t")
  {
    initializeArrayWithInts<int16_t>();
  }
  else if(type == "int32_t")
  {
    initializeArrayWithInts<int32_t>();
  }
  else if(type == "int64_t")
  {
    initializeArrayWithInts<int64_t>();
  }
  else if(type == "uint8_t")
  {
    initializeArrayWithInts<uint8_t>();
  }
  else if(type == "uint16_t")
  {
    initializeArrayWithInts<uint16_t>();
  }
  else if(type == "uint32_t")
  {
    initializeArrayWithInts<uint32_t>();
  }
  else if(type == "uint64_t")
  {
    initializeArrayWithInts<uint64_t>();
  }
  else if(type == "float")
  {
    initializeArrayWithReals<float>();
  }
  else if(type == "double")
  {
    initializeArrayWithReals<double>();
  }
  else if(type == "bool")
  {
    initializeArrayWithBools();
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Delay the execution to avoid the exact same seedings for each array
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void InitializeDataImpl::initializeArrayWithInts() const
{
  std::pair<T, T> range = getRange<T>();
  Detail::UniformIntDistribution<T> distribution(range.first, range.second);
  T manualValue = static_cast<T>(m_InitValue);
  Detail::initializeArray(m_TargetArray, m_Dims, m_Bounds, distribution, manualValue, static_cast<InitializeData::InitChoices>(m_InitType), m_InvertData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void InitializeDataImpl::initializeArrayWithReals() const
{
  std::pair<T, T> range = getRange<T>();
  Detail::UniformRealsDistribution<T> distribution(range.first, range.second);
  T manualValue = static_cast<T>(m_InitValue);
  Detail::initializeArray(m_TargetArray, m_Dims, m_Bounds, distribution, manualValue, static_cast<InitializeData::InitChoices>(m_InitType), m_InvertData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeDataImpl::initializeArrayWithBools() const
{
  Detail::UniformBoolDistribution<bool> distribution;
  bool manualValue = (m_InitValue != 0);
  Detail::initializeArray(m_TargetArray, m_Dims, m_Bounds, distribution, manualValue, static_cast<InitializeData::InitChoices>(m_InitType), m_InvertData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
std::pair<T, T> InitializeDataImpl::getRange() const
{
  if(m_InitType == InitializeData::InitChoices::RandomWithRange)
  {
    return m_InitRange;
  }
  return std::make_pair(std::numeric_limits<T>().min(), std::numeric_limits<T>().max());
}
