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
#pragma once

#include <memory>

#include <hdf5.h>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/PhaseType.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

using VectorOfFloatArray = QVector<FloatArrayType::Pointer>;

#define SD_DEEP_COPY_VECTOR(func)                                                                                                                                                                      \
  {                                                                                                                                                                                                    \
    VectorOfFloatArray arrays = get##func();                                                                                                                                                           \
    arrays = StatsData::DeepCopyVectorOFloatArray(arrays);                                                                                                                                             \
    ptr->set##func(arrays);                                                                                                                                                                            \
  }

/**
 * @class StatsData StatsData.h DREAM3DLib/StatsData/StatsData.h
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
class SIMPLib_EXPORT StatsData
{

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(StatsData)
  PYB11_SHARED_POINTERS(StatsData)
  PYB11_STATIC_NEW_MACRO(StatsData)
  PYB11_METHOD(QString getStatsType)
  PYB11_METHOD(PhaseType::Type getPhaseType)
  PYB11_PROPERTY(float PhaseFraction READ getPhaseFraction WRITE setPhaseFraction)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = StatsData;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for StatsData
   */
  virtual QString getNameOfClass() const;
  /**
   * @brief Returns the name of the class for StatsData
   */
  static QString ClassName();

  virtual ~StatsData();

  /** Allow users to give each StatsData object a custom name
   */
  /**
   * @brief Setter property for Name
   */
  void setName(const QString& value);
  /**
   * @brief Getter property for Name
   * @return Value of Name
   */
  QString getName() const;

  /**
   * @brief Setter property for PhaseFraction
   */
  void setPhaseFraction(float value);
  /**
   * @brief Getter property for PhaseFraction
   * @return Value of PhaseFraction
   */
  float getPhaseFraction() const;

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
   * @brief CreateDeepCopy
   * @param array
   * @return
   */
  static VectorOfFloatArray DeepCopyVectorOFloatArray(VectorOfFloatArray& array);

  /**
   * @brief Creates the Vector of FloatArrayType for a give distribution type
   * and sized according to numBins
   * @param distributionType The type of distribution as laid out in the SIMPL::DistributionType
   * namespace which should be found in Constants.h.
   * @param numBins The number of bins that the array should be sized to
   * @return a VectorOfFloatArray object which could be empty if an unrecognized
   * distribution type is passed in.
   */
  static VectorOfFloatArray CreateCorrelatedDistributionArrays(uint32_t distributionType, size_t numBins);

  /**
   * @brief CreateDistributionArrays
   * @param distributionType
   * @return
   */
  static FloatArrayType::Pointer CreateDistributionArrays(uint32_t distributionType);

  /**
   * @brief deepCopy
   * @return
   */
  virtual Pointer deepCopy();

  /**
   * @brief writeHDF5Data
   * @param groupId
   * @return
   */
  virtual int writeHDF5Data(hid_t groupId);

  /**
   * @brief readHDF5Data
   * @param groupId
   * @return
   */
  virtual int readHDF5Data(hid_t groupId);

  /**
   * @brief writeJson
   * @param json
   */
  virtual void writeJson(QJsonObject& json);

  /**
   * @brief readJson
   * @param json
   */
  virtual void readJson(const QJsonObject& json);

  /**
   * @brief getStatsType
   * @return
   */
  virtual QString getStatsType();

  /**
   * @brief getPhaseType
   * @return
   */
  virtual PhaseType::Type getPhaseType();

  /**
   * @brief generateJsonArrayFromDataArray
   * @param data
   * @return
   */
  template <typename T>
  QJsonArray generateJsonArrayFromDataArray(typename DataArray<T>::Pointer data)
  {
    QJsonArray json;
    if(nullptr != data.get())
    {
      for(size_t i = 0; i < data->getSize(); i++)
      {
        json.append(QJsonValue(static_cast<double>(data->getValue(i))));
      }
    }
    return json;
  }

  /**
   * @brief generateJsonArrays
   * @param arrays
   * @param json
   */
  void generateJsonArrays(VectorOfFloatArray& arrays, QJsonObject& json);

  /**
   * @brief decodeDistributionType
   * @param disType
   * @return
   */
  QString decodeDistributionType(int disType);

  /**
   * @brief writeJsonDistributionArrays
   * @param json
   * @param arrays
   * @param key
   * @param disType
   */
  void writeJsonDistributionArrays(QJsonObject& json, VectorOfFloatArray arrays, QString key, int disType);

  /**
   *  @brief encodeDistributionType
   *  @param distTypeName
   *  @return
   */
  static int EncodeDistributionType(QString distTypeName);

  /**
   * @brief ParseFloat3Vec
   * @param json
   * @param key
   * @param values
   * @param defaultValue
   * @return
   */
  static int ParseFloat3Vec(const QJsonObject& json, const QString& key, std::array<float, 3>& values, float defaultValue = 0.0f);

  /**
   * @brief ReadJsonDistributionArrays
   * @param json
   * @param key
   * @param disType
   * @return
   */
  static VectorOfFloatArray ReadJsonDistributionArrays(const QJsonObject& json, const QString& key, int& disType);

  /**
   * @brief arrayNamesFromDistributionType
   * @param disType
   * @return
   */
  static QStringList ArrayNamesFromDistributionType(const QString& disType);

  /**
   * @brief arrayNamesFromDistributionType
   * @param disType
   * @return
   */
  static VectorOfFloatArray ReadJsonVectorOfFloatsArrays(const QJsonObject& json, const QString& key);

protected:
  StatsData();

private:
  QString m_Name = "StatsData";
  float m_PhaseFraction = 0.0f;

public:
  StatsData(const StatsData&) = delete;            // Copy Constructor Not Implemented
  StatsData(StatsData&&) = delete;                 // Move Constructor Not Implemented
  StatsData& operator=(const StatsData&) = delete; // Copy Assignment Not Implemented
  StatsData& operator=(StatsData&&) = delete;      // Move Assignment Not Implemented
};
