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

#pragma once

#include <memory>

#include <array>
#include <vector>

#include <QtCore/QJsonObject>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"

/**
 * @brief The RdfData class
 */
class SIMPLib_EXPORT RdfData
{
public:
  using Self = RdfData;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for RdfData
   */
  QString getNameOfClass() const;
  /**
   * @brief Returns the name of the class for RdfData
   */
  static QString ClassName();

  virtual ~RdfData();

  /**
   * @brief Setter property for Frequencies
   */
  void setFrequencies(const std::vector<float>& value);
  /**
   * @brief Getter property for Frequencies
   * @return Value of Frequencies
   */
  std::vector<float> getFrequencies() const;

  /**
   * @brief Setter property for MinDistance
   */
  void setMinDistance(float value);
  /**
   * @brief Getter property for MinDistance
   * @return Value of MinDistance
   */
  float getMinDistance() const;

  /**
   * @brief Setter property for MaxDistance
   */
  void setMaxDistance(float value);
  /**
   * @brief Getter property for MaxDistance
   * @return Value of MaxDistance
   */
  float getMaxDistance() const;

  /**
   * @brief Setter property for NumberOfBins
   */
  void setNumberOfBins(int value);
  /**
   * @brief Getter property for NumberOfBins
   * @return Value of NumberOfBins
   */
  int getNumberOfBins() const;

  /**
   * @brief Setter property for DistributionType
   */
  void setDistributionType(const QString& value);
  /**
   * @brief Getter property for DistributionType
   * @return Value of DistributionType
   */
  QString getDistributionType() const;

  /**
   * @brief Setter property for BoxSize
   */
  void setBoxSize(const std::array<float, 3>& value);
  /**
   * @brief Setter property for BoxSize
   */
  void setBoxSize(float v0, float v1, float v2);
  /**
   * @brief Getter property for BoxSize
   * @return Value of BoxSize
   */
  std::array<float, 3> getBoxSize() const;

  /**
   * @brief Setter property for BoxResolution
   */
  void setBoxResolution(const std::array<float, 3>& value);
  /**
   * @brief Setter property for BoxResolution
   */
  void setBoxResolution(float v0, float v1, float v2);
  /**
   * @brief Getter property for BoxResolution
   * @return Value of BoxResolution
   */
  std::array<float, 3> getBoxResolution() const;

  int getFrequencyCount();

  /**
   * @brief deepCopy
   * @return
   */
  Pointer deepCopy();

  /**
   * @brief readJson
   * @param json
   * @return
   */
  int readJson(const QJsonObject& json);

  /**
   * @brief writeJson
   * @param json
   * @return
   */
  int writeJson(QJsonObject& json);

protected:
  RdfData();

private:
  std::array<float, 3> m_BoxSize = {100.0f, 100.0f, 100.0f};
  std::array<float, 3> m_BoxResolution{0.1f, 0.1f, 0.1f};

  std::vector<float> m_Frequencies = {};
  float m_MinDistance = 0.0f;
  float m_MaxDistance = 0.0f;
  int m_NumberOfBins = 50;
  QString m_DistributionType = SIMPL::StringConstants::UnknownDistribution;

  RdfData(const RdfData&) = delete;        // Copy Constructor Not Implemented
  void operator=(const RdfData&) = delete; // Move assignment Not Implemented
};
