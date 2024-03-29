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
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

/*
 *
 */
class SIMPLib_EXPORT H5FilterParametersWriter : public AbstractFilterParametersWriter
{
public:
  using Self = H5FilterParametersWriter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for H5FilterParametersWriter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for H5FilterParametersWriter
   */
  static QString ClassName();

  /**
   * @brief WritePipelineToFile This function will write a pipeline to an HDF5 based
   * DREAM3D file. The file path passed in <b>WILL BE OVER WRITTEN</b> by this
   * function <b>WITHOUT WARNING</b>
   * @param pipeline The pipeline to be written
   * @param filePath The file path to write
   * @param name The name of the pipeline (Typically the nameo of the file)
   * @param obs Any observer that we can pass error/warning messages back to in case something goes wrong.
   * @return
   */
  int writePipelineToFile(FilterPipeline::Pointer pipeline, QString filePath, QString pipelineName, bool expandPipeline, QList<IObserver*> obs = QList<IObserver*>()) override;

  /**
   * @brief Setter property for PipelineGroupId
   */
  void setPipelineGroupId(hid_t value);
  /**
   * @brief Getter property for PipelineGroupId
   * @return Value of PipelineGroupId
   */
  hid_t getPipelineGroupId() const;

  ~H5FilterParametersWriter() override;

  /**
   * @brief Setter property for GroupId
   */
  void setGroupId(hid_t value);
  /**
   * @brief Getter property for GroupId
   * @return Value of GroupId
   */
  hid_t getGroupId() const;

  hid_t getCurrentGroupId() const;

  int openFilterGroup(AbstractFilter* filter, int index) override;
  int closeFilterGroup() override;

  int writeValue(const QString& name, const QString& value) override;
  int writeValue(const QString& name, const QVector<QString>& value) override;
  int writeValue(const QString& name, const QStringList& value) override;

  int writeValue(const QString& name, int8_t value) override;
  int writeValue(const QString& name, int16_t value) override;
  int writeValue(const QString& name, int32_t value) override;
  int writeValue(const QString& name, int64_t value) override;
  int writeValue(const QString& name, uint8_t value) override;
  int writeValue(const QString& name, uint16_t value) override;
  int writeValue(const QString& name, uint32_t value) override;
  int writeValue(const QString& name, uint64_t value) override;
  int writeValue(const QString& name, float value) override;
  int writeValue(const QString& name, double value) override;

  int writeValue(const QString& name, QVector<int8_t> value) override;
  int writeValue(const QString& name, QVector<int16_t> value) override;
  int writeValue(const QString& name, QVector<int32_t> value) override;
  int writeValue(const QString& name, QVector<int64_t> value) override;
  int writeValue(const QString& name, QVector<uint8_t> value) override;
  int writeValue(const QString& name, QVector<uint16_t> value) override;
  int writeValue(const QString& name, QVector<uint32_t> value) override;
  int writeValue(const QString& name, QVector<uint64_t> value) override;
  int writeValue(const QString& name, QVector<float> value) override;
  int writeValue(const QString& name, QVector<double> value) override;

  int writeValue(const QString& name, IntVec3Type v) override;
  int writeValue(const QString& name, FloatVec3Type v) override;

  int writeValue(const QString& name, Float2ndOrderPolynomial v) override;
  int writeValue(const QString& name, Float3rdOrderPoly_t v) override;
  int writeValue(const QString& name, Float4thOrderPolynomial v) override;
  int writeValue(const QString& name, StackFileListInfo v) override;

  int writeValue(const QString& name, ComparisonInput_t v) override;
  int writeValue(const QString& name, ComparisonInputs v) override;

  int writeValue(const QString& name, AxisAngleInput v) override;
  int writeValue(const QString& name, QVector<AxisAngleInput> v) override;

  int writeArraySelections(const QString& name, QSet<QString> v) override;
  int writeValue(const QString& name, DataContainerArrayProxy& dcaProxy) override;
  int writeValue(const QString& name, const DataArrayPath& v) override;
  int writeValue(const QString& name, const QVector<DataArrayPath>& paths) override;

  int writeValue(const QString& name, const DynamicTableData& v) override;

  int writeValue(const QString& name, const QPair<double, double>& v) override;

protected:
  H5FilterParametersWriter();

  int writeValue(const QString& name, AxisAngleInput v, int notUsed) override;

private:
  hid_t m_PipelineGroupId = {};
  hid_t m_GroupId = {};

  hid_t m_CurrentGroupId = -1;

public:
  H5FilterParametersWriter(const H5FilterParametersWriter&) = delete;            // Copy Constructor Not Implemented
  H5FilterParametersWriter(H5FilterParametersWriter&&) = delete;                 // Move Constructor Not Implemented
  H5FilterParametersWriter& operator=(const H5FilterParametersWriter&) = delete; // Copy Assignment Not Implemented
  H5FilterParametersWriter& operator=(H5FilterParametersWriter&&) = delete;      // Move Assignment Not Implemented
};
