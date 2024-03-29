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

#include "hdf5.h"

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

/*
 *
 */
class SIMPLib_EXPORT H5FilterParametersReader : public AbstractFilterParametersReader
{
public:
  using Self = H5FilterParametersReader;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for H5FilterParametersReader
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for H5FilterParametersReader
   */
  static QString ClassName();

  ~H5FilterParametersReader() override;

  /**
   * @brief OpenDREAM3DFile This will open the HDF5 based DREAM3D file and open the proper HDF5 internal
   * group that is associated with the pipeline storage
   * @param filepath
   * @param fid The HDF5 file Id that represents the open HDF5 file
   * @return Return a pointer to a newly instantiated instance of this class. A Null Pointer will be returned if
   * the file can not be opened or the group is missing.
   */
  static Pointer OpenDREAM3DFileForReadingPipeline(QString filePath, hid_t& fid);

  /**
   * @brief Setter property for PipelineGroupId
   */
  void setPipelineGroupId(hid_t value);
  /**
   * @brief Getter property for PipelineGroupId
   * @return Value of PipelineGroupId
   */
  hid_t getPipelineGroupId() const;

  hid_t getCurrentGroupId() const;

  int openFilterGroup(AbstractFilter* filter, int index) override;
  int closeFilterGroup() override;

  /**
   * @brief ReadPipelineFromFile
   * @param filePath
   * @return
   */
  FilterPipeline::Pointer readPipelineFromFile(QString filePath, IObserver* obs = nullptr);

  /**
   * @brief readPipelineFromFile
   * @param fid
   * @param obs
   * @return
   */
  FilterPipeline::Pointer readPipelineFromFile(hid_t fid, IObserver* obs = nullptr);

  /**
   * @brief getJsonFromFile
   * @param filePath
   * @param obs
   * @return
   */
  QString getJsonFromFile(QString filePath, IObserver* obs = nullptr);

  QString readString(const QString& name, QString value) override;
  QVector<QString> readStrings(const QString& name, QVector<QString> value) override;
  QStringList readStringList(const QString& name, QStringList value) override;

  int8_t readValue(const QString& name, int8_t value) override;
  int16_t readValue(const QString& name, int16_t value) override;
  int32_t readValue(const QString& name, int32_t value) override;
  int64_t readValue(const QString& name, int64_t value) override;
  uint8_t readValue(const QString& name, uint8_t value) override;
  uint16_t readValue(const QString& name, uint16_t value) override;
  uint32_t readValue(const QString& name, uint32_t value) override;
  uint64_t readValue(const QString& name, uint64_t value) override;
  float readValue(const QString& name, float value) override;
  double readValue(const QString& name, double value) override;
  bool readValue(const QString& name, bool value) override;
  AttributeMatrix::Type readValue(const QString& name, AttributeMatrix::Type value) override;

  QVector<int8_t> readArray(const QString& name, QVector<int8_t> value) override;
  QVector<int16_t> readArray(const QString& name, QVector<int16_t> value) override;
  QVector<int32_t> readArray(const QString& name, QVector<int32_t> value) override;
  QVector<int64_t> readArray(const QString& name, QVector<int64_t> value) override;
  QVector<uint8_t> readArray(const QString& name, QVector<uint8_t> value) override;
  QVector<uint16_t> readArray(const QString& name, QVector<uint16_t> value) override;
  QVector<uint32_t> readArray(const QString& name, QVector<uint32_t> value) override;
  QVector<uint64_t> readArray(const QString& name, QVector<uint64_t> value) override;
  QVector<float> readArray(const QString& name, QVector<float> value) override;
  QVector<double> readArray(const QString& name, QVector<double> value) override;

  IntVec3Type readIntVec3(const QString& name, IntVec3Type v) override;
  FloatVec3Type readFloatVec3(const QString& name, FloatVec3Type v) override;

  Float2ndOrderPolynomial readFloat2ndOrderPoly(const QString& name, Float2ndOrderPolynomial v) override;
  Float3rdOrderPoly_t readFloat3rdOrderPoly(const QString& name, Float3rdOrderPoly_t v) override;
  Float4thOrderPolynomial readFloat4thOrderPoly(const QString& name, Float4thOrderPolynomial v) override;
  StackFileListInfo readFileListInfo(const QString& name, StackFileListInfo v) override;
  ComparisonInput_t readComparisonInput(const QString& name, ComparisonInput_t v, int vectorPos) override;
  ComparisonInputs readComparisonInputs(const QString& name, ComparisonInputs v) override;
  ComparisonInputsAdvanced readComparisonInputsAdvanced(const QString& name, ComparisonInputsAdvanced v) override;
  AxisAngleInput readAxisAngle(const QString& name, AxisAngleInput v, int vectorPos) override;
  QVector<AxisAngleInput> readAxisAngles(const QString& name, QVector<AxisAngleInput> defValue) override;
  QSet<QString> readArraySelections(const QString& name, QSet<QString> v) override;

  DataContainerArrayProxy readDataContainerArrayProxy(const QString& name, DataContainerArrayProxy v) override;
  DataArrayPath readDataArrayPath(const QString& name, DataArrayPath v) override;
  QVector<DataArrayPath> readDataArrayPathVector(const QString& name, QVector<DataArrayPath> def) override;
  DynamicTableData readDynamicTableData(const QString& name, DynamicTableData def) override;

  FPRangePair readPairOfDoubles(const QString& name, const FPRangePair& v) override;

protected:
  H5FilterParametersReader();

private:
  hid_t m_PipelineGroupId = {};

  hid_t m_CurrentGroupId = -1;
  QJsonObject m_PipelineRoot;
  QJsonObject m_CurrentFilterObject;
  int m_Version = -1;
  int m_CurrentIndex = -1;

public:
  H5FilterParametersReader(const H5FilterParametersReader&) = delete;            // Copy Constructor Not Implemented
  H5FilterParametersReader(H5FilterParametersReader&&) = delete;                 // Move Constructor Not Implemented
  H5FilterParametersReader& operator=(const H5FilterParametersReader&) = delete; // Copy Assignment Not Implemented
  H5FilterParametersReader& operator=(H5FilterParametersReader&&) = delete;      // Move Assignment Not Implemented
};
