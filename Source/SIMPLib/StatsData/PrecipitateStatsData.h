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

#include <hdf5.h>

#include "SIMPLib/Common/PhaseType.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/StatsData/StatsData.h"
#include "SIMPLib/Math/RadialDistributionFunction.h"
#include "SIMPLib/Math/RdfData.h"

typedef QVector<FloatArrayType::Pointer> VectorOfFloatArray;

/**
 * @class PrecipitateStatsData PrecipitateStatsData.h DREAM3DLib/StatsData/PrecipitateStatsData.h
 * @brief This class holds the statistical data for a single phase of a microstructure.
 * There are several statistics that are held by this class in a varying number of
 * storage types. Some types have specific ordering of the sub arrays with in them. Those
 * are enumerated here:@n
 * For the distributions a QVector holds objects of the DataArray<float>::Pointer which is
 * typedef'ed to FloatArrayType @see DataArray.hpp. The order of the FloatArrayType
 * objects in the QVector is as follows:
 *   <b>Beta Distribution</b>@n
 *    @li Alpha
 *    @li Beta
 *    <b>Log Normal Distribution</b>@n
 *    @li Average
 *    @li Standard Deviation
 *    <b>Power Law Distribution</b>@n
 *    @li Alpha
 *    @li K
 *    @li Beta
 *
 * The Feature Diameter Info is an array of 3 floats where the values are encoded as:@n
 * @li [0] = Average
 * @li [1] = Max
 * @li [2] = Min
 *
 * The Feature Size Distribution is an array of 2 floats where the values are encoded as:@n
 * @li [0] = Average
 * @li [1] = Standard Deviation
 *
 *
 * @date Feb 9, 2012
 * @version 1.0
 */
class SIMPLib_EXPORT PrecipitateStatsData : public StatsData
{

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(PrecipitateStatsData SUPERCLASS StatsData)
  PYB11_SHARED_POINTERS(PrecipitateStatsData)
  PYB11_STATIC_NEW_MACRO(PrecipitateStatsData)
  PYB11_PROPERTY(float BinStepSize READ getBinStepSize WRITE setBinStepSize)
  PYB11_PROPERTY(float MaxFeatureDiameter READ getMaxFeatureDiameter WRITE setMaxFeatureDiameter)
  PYB11_PROPERTY(float MinFeatureDiameter READ getMinFeatureDiameter WRITE setMinFeatureDiameter)
  PYB11_PROPERTY(VectorOfFloatArray FeatureSizeDistribution READ getFeatureSizeDistribution WRITE setFeatureSizeDistribution)
  PYB11_PROPERTY(uint32_t FeatureSize_DistType READ getFeatureSize_DistType WRITE setFeatureSize_DistType)
  PYB11_METHOD(PhaseType::Type getPhaseType)
  PYB11_METHOD(FloatArrayType::Pointer generateBinNumbers)
  PYB11_PROPERTY(float PhaseFraction READ getPhaseFraction WRITE setPhaseFraction)
  PYB11_PROPERTY(float BoundaryArea READ getBoundaryArea WRITE setBoundaryArea)
  PYB11_PROPERTY(float PrecipBoundaryFraction READ getPrecipBoundaryFraction WRITE setPrecipBoundaryFraction)
  PYB11_METHOD(QString getStatsType)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = PrecipitateStatsData;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for PrecipitateStatsData
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for PrecipitateStatsData
   */
  static QString ClassName();

  ~PrecipitateStatsData() override;

  /**
   * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
   * can be a primitive like char, float, int or the name of a class.
   * @return
   */
  void getXdmfTypeAndSize(QString& xdmfTypeName, int& precision)
  {
    xdmfTypeName = getNameOfClass();
    precision = 0;
  }

  /**
   * @brief Setter property for BoundaryArea
   */
  void setBoundaryArea(float value);
  /**
   * @brief Getter property for BoundaryArea
   * @return Value of BoundaryArea
   */
  float getBoundaryArea() const;

  /**
   * @brief Setter property for PrecipBoundaryFraction
   */
  void setPrecipBoundaryFraction(float value);
  /**
   * @brief Getter property for PrecipBoundaryFraction
   * @return Value of PrecipBoundaryFraction
   */
  float getPrecipBoundaryFraction() const;

  QString getStatsType() override;
  PhaseType::Type getPhaseType() override;

  /**
   * @breif this will generate the Bin Numbers values;
   */
  FloatArrayType::Pointer generateBinNumbers();
  /**
   * @brief The values are encoded into 3 floats: Average, Max, Min
   */
  /**
   * @brief Setter property for FeatureDiameterInfo
   */
  void setFeatureDiameterInfo(const std::array<float, 3>& value);
  /**
   * @brief Setter property for FeatureDiameterInfo
   */
  void setFeatureDiameterInfo(float v0, float v1, float v2);
  /**
   * @brief Getter property for FeatureDiameterInfo
   * @return Value of FeatureDiameterInfo
   */
  std::array<float, 3> getFeatureDiameterInfo() const;
  void getFeatureDiameterInfo(float* data) const;

  void setBinStepSize(float v)
  {
    m_FeatureDiameterInfo[0] = v;
  }
  float getBinStepSize()
  {
    return m_FeatureDiameterInfo[0];
  }

  void setMaxFeatureDiameter(float v)
  {
    m_FeatureDiameterInfo[1] = v;
  }
  float getMaxFeatureDiameter()
  {
    return m_FeatureDiameterInfo[1];
  }

  void setMinFeatureDiameter(float v)
  {
    m_FeatureDiameterInfo[2] = v;
  }
  float getMinFeatureDiameter()
  {
    return m_FeatureDiameterInfo[2];
  }

  /**
   * @brief The values are encoded into float arrays
   */
  /**
   * @brief Setter property for FeatureSizeDistribution
   */
  void setFeatureSizeDistribution(const VectorOfFloatArray& value);
  /**
   * @brief Getter property for FeatureSizeDistribution
   * @return Value of FeatureSizeDistribution
   */
  VectorOfFloatArray getFeatureSizeDistribution() const;

  /**
   * @brief Setter property for FeatureSize_DistType
   */
  void setFeatureSize_DistType(uint32_t value);
  /**
   * @brief Getter property for FeatureSize_DistType
   * @return Value of FeatureSize_DistType
   */
  uint32_t getFeatureSize_DistType() const;

  /**
   * @brief Setter property for RadialDistFunction
   */
  void setRadialDistFunction(const RdfData::Pointer& value);
  /**
   * @brief Getter property for RadialDistFunction
   * @return Value of RadialDistFunction
   */
  RdfData::Pointer getRadialDistFunction() const;

  /**
   * @brief Setter property for BinNumbers
   */
  void setBinNumbers(const FloatArrayType::Pointer& value);
  /**
   * @brief Getter property for BinNumbers
   * @return Value of BinNumbers
   */
  FloatArrayType::Pointer getBinNumbers() const;

  size_t getNumberOfBins()
  {
    return (m_BinNumbers.get() == nullptr) ? 0 : m_BinNumbers->getSize();
  }

  /**
   * @brief Setter property for FeatureSize_BOverA
   */
  void setFeatureSize_BOverA(const VectorOfFloatArray& value);
  /**
   * @brief Getter property for FeatureSize_BOverA
   * @return Value of FeatureSize_BOverA
   */
  VectorOfFloatArray getFeatureSize_BOverA() const;

  /**
   * @brief Setter property for BOverA_DistType
   */
  void setBOverA_DistType(uint32_t value);
  /**
   * @brief Getter property for BOverA_DistType
   * @return Value of BOverA_DistType
   */
  uint32_t getBOverA_DistType() const;

  /**
   * @brief Setter property for FeatureSize_COverA
   */
  void setFeatureSize_COverA(const VectorOfFloatArray& value);
  /**
   * @brief Getter property for FeatureSize_COverA
   * @return Value of FeatureSize_COverA
   */
  VectorOfFloatArray getFeatureSize_COverA() const;

  /**
   * @brief Setter property for COverA_DistType
   */
  void setCOverA_DistType(uint32_t value);
  /**
   * @brief Getter property for COverA_DistType
   * @return Value of COverA_DistType
   */
  uint32_t getCOverA_DistType() const;

  /**
   * @brief Setter property for FeatureSize_Clustering
   */
  void setFeatureSize_Clustering(const VectorOfFloatArray& value);
  /**
   * @brief Getter property for FeatureSize_Clustering
   * @return Value of FeatureSize_Clustering
   */
  VectorOfFloatArray getFeatureSize_Clustering() const;

  /**
   * @brief Setter property for Clustering_DistType
   */
  void setClustering_DistType(uint32_t value);
  /**
   * @brief Getter property for Clustering_DistType
   * @return Value of Clustering_DistType
   */
  uint32_t getClustering_DistType() const;

  /**
   * @brief Setter property for FeatureSize_Omegas
   */
  void setFeatureSize_Omegas(const VectorOfFloatArray& value);
  /**
   * @brief Getter property for FeatureSize_Omegas
   * @return Value of FeatureSize_Omegas
   */
  VectorOfFloatArray getFeatureSize_Omegas() const;

  /**
   * @brief Setter property for Omegas_DistType
   */
  void setOmegas_DistType(uint32_t value);
  /**
   * @brief Getter property for Omegas_DistType
   * @return Value of Omegas_DistType
   */
  uint32_t getOmegas_DistType() const;

  /**
   * @brief Setter property for MisorientationBins
   */
  void setMisorientationBins(const FloatArrayType::Pointer& value);
  /**
   * @brief Getter property for MisorientationBins
   * @return Value of MisorientationBins
   */
  FloatArrayType::Pointer getMisorientationBins() const;

  /* 3 Vectors: Angles, Axis, Weights */
  /**
   * @brief Setter property for MDF_Weights
   */
  void setMDF_Weights(const VectorOfFloatArray& value);
  /**
   * @brief Getter property for MDF_Weights
   * @return Value of MDF_Weights
   */
  VectorOfFloatArray getMDF_Weights() const;

  /**
   * @brief Setter property for ODF
   */
  void setODF(const FloatArrayType::Pointer& value);
  /**
   * @brief Getter property for ODF
   * @return Value of ODF
   */
  FloatArrayType::Pointer getODF() const;

  /* 5 Vectors: Euler 1, Euler 2, Euler 3, Sigma, Weights */
  /**
   * @brief Setter property for ODF_Weights
   */
  void setODF_Weights(const VectorOfFloatArray& value);
  /**
   * @brief Getter property for ODF_Weights
   * @return Value of ODF_Weights
   */
  VectorOfFloatArray getODF_Weights() const;

  /**
   * @brief Setter property for AxisOrientation
   */
  void setAxisOrientation(const FloatArrayType::Pointer& value);
  /**
   * @brief Getter property for AxisOrientation
   * @return Value of AxisOrientation
   */
  FloatArrayType::Pointer getAxisOrientation() const;

  /* 5 Vectors: Euler 1, Euler 2, Euler 3, Sigma, Weights */
  /**
   * @brief Setter property for AxisODF_Weights
   */
  void setAxisODF_Weights(const VectorOfFloatArray& value);
  /**
   * @brief Getter property for AxisODF_Weights
   * @return Value of AxisODF_Weights
   */
  VectorOfFloatArray getAxisODF_Weights() const;

  /**
   * @brief deepCopy
   * @return
   */
  StatsData::Pointer deepCopy() override;

  /**
   * @brief writeHDF5Data
   * @param groupId
   * @return
   */
  int writeHDF5Data(hid_t groupId) override;

  /**
   * @brief readHDF5Data
   * @param groupId
   * @return
   */
  int readHDF5Data(hid_t groupId) override;

  /**
   * @brief writeJson
   * @param json
   */
  void writeJson(QJsonObject& json) override;

  /**
   * @brief readJson
   * @param json
   */
  void readJson(const QJsonObject& json) override;

protected:
  PrecipitateStatsData();

public:
  PrecipitateStatsData(const PrecipitateStatsData&) = delete;            // Copy Constructor Not Implemented
  PrecipitateStatsData(PrecipitateStatsData&&) = delete;                 // Move Constructor Not Implemented
  PrecipitateStatsData& operator=(const PrecipitateStatsData&) = delete; // Copy Assignment Not Implemented
  PrecipitateStatsData& operator=(PrecipitateStatsData&&) = delete;      // Move Assignment Not Implemented

private:
  std::array<float, 3> m_FeatureDiameterInfo = {0.1f, 1.0f, 0.1f};

  float m_BoundaryArea = {};
  float m_PrecipBoundaryFraction = 0.0f;
  VectorOfFloatArray m_FeatureSizeDistribution = {};
  uint32_t m_FeatureSize_DistType = SIMPL::DistributionType::LogNormal;
  RdfData::Pointer m_RadialDistFunction = {};
  FloatArrayType::Pointer m_BinNumbers = {};
  VectorOfFloatArray m_FeatureSize_BOverA = {};
  uint32_t m_BOverA_DistType = SIMPL::DistributionType::Beta;
  VectorOfFloatArray m_FeatureSize_COverA = {};
  uint32_t m_COverA_DistType = SIMPL::DistributionType::Beta;
  VectorOfFloatArray m_FeatureSize_Clustering = {};
  uint32_t m_Clustering_DistType = SIMPL::DistributionType::LogNormal;
  VectorOfFloatArray m_FeatureSize_Omegas = {};
  uint32_t m_Omegas_DistType = SIMPL::DistributionType::Beta;
  FloatArrayType::Pointer m_MisorientationBins = {};
  VectorOfFloatArray m_MDF_Weights = {};
  FloatArrayType::Pointer m_ODF = {};
  VectorOfFloatArray m_ODF_Weights = {};
  FloatArrayType::Pointer m_AxisOrientation = {};
  VectorOfFloatArray m_AxisODF_Weights = {};
};
