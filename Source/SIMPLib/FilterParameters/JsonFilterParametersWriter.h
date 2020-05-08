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

#include <QtCore/QString>

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @class
 * @brief
 *
 * @date Jan 17, 2012
 * @version 1.0
 */
class SIMPLib_EXPORT JsonFilterParametersWriter : public AbstractFilterParametersWriter
{
public:
  using Self = JsonFilterParametersWriter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief JsonFilterParametersWriter
   * @param fileName
   * @param pipelineName
   */
  JsonFilterParametersWriter(QString& fileName, QString& pipelineName);

  ~JsonFilterParametersWriter() override;

  /**
   * @brief Returns the name of the class for JsonFilterParametersWriter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for JsonFilterParametersWriter
   */
  static QString ClassName();

  /**
   * @brief Setter property for FileName
   */
  void setFileName(const QString& value);
  /**
   * @brief Getter property for FileName
   * @return Value of FileName
   */
  QString getFileName() const;

  /**
   * @brief Setter property for PipelineName
   */
  void setPipelineName(const QString& value);
  /**
   * @brief Getter property for PipelineName
   * @return Value of PipelineName
   */
  QString getPipelineName() const;

  /**
   * @brief WritePipelineToFile This function will write a pipeline to a
   * JSON file. The file path passed in <b>WILL BE OVER WRITTEN</b> by this
   * function <b>WITHOUT WARNING</b>
   * @param pipeline The pipeline to be written
   * @param filePath The file path to write
   * @param pipelineName The name of the pipeline (Typically the name of the file)
   * @param expandPipeline Expand any embeded pipelines that are in DataContainerReader filters
   * @param obs Any observer that we can pass error/warning messages back to in case something goes wrong.
   * @return
   */
  int writePipelineToFile(FilterPipeline::Pointer pipeline, QString filePath, QString pipelineName, bool expandPipelines, QList<IObserver*> obs = QList<IObserver*>()) override;

  /**
   * @brief WritePipelineToString This function will write a pipeline to a QString.
   * @param pipeline The pipeline to be written
   * @param pipelineName The name of the pipeline (Typically the name of the file)
   * @param expandPipeline Expand any embeded pipelines that are in DataContainerReader filters
   * @param obs Any observer that we can pass error/warning messages back to in case something goes wrong.
   * @return The pipeline as a QString
   */
  QString writePipelineToString(FilterPipeline::Pointer pipeline, QString pipelineName, bool expandPipeline, QList<IObserver*> obs = QList<IObserver*>());

  /**
   * @brief openFilterGroup
   * @param unused
   * @param index
   * @return
   */
  int openFilterGroup(AbstractFilter* unused, int index) override;

  /**
   * @brief closeFilterGroup
   * @return
   */
  int closeFilterGroup() override;

protected:
  JsonFilterParametersWriter();

private:
  int m_CurrentIndex = 0;
  int m_MaxFilterIndex = -1;
  QString m_FileName = {};
  QString m_PipelineName = {};
  QJsonObject m_Root;
  QJsonObject m_CurrentFilterIndex;

  /**
   * @brief Setter property for MaxFilterIndex
   */
  void setMaxFilterIndex(int value);
  /**
   * @brief Getter property for MaxFilterIndex
   * @return Value of MaxFilterIndex
   */
  int getMaxFilterIndex() const;

  /**
   * @brief getCurrentGroupObject
   * @return
   */
  QJsonObject& getCurrentGroupObject();

  /**
   * @brief populateWriter
   * @param pipeline
   * @param pipelineName
   * @param obs
   * @return
   */
  int populateWriter(FilterPipeline::Pointer pipeline, QString pipelineName, bool expandPipeline, QList<IObserver*> obs);

  /**
   * @brief writePipeline
   */
  void writePipeline();

  /**
   * @brief clearWriter
   */
  void clearWriter();

  /**
   * @brief toDocument
   * @return
   */
  QJsonDocument toDocument();

  /**
   * @brief generateIndexString Generates a possibly padded string that represents
   * the current filter index. Used during the JSON document creation.
   * @param currentIndex The numerical index that the string should be generated for.
   * @return
   */
  QString generateIndexString(int currentIndex);

public:
  JsonFilterParametersWriter(const JsonFilterParametersWriter&) = delete;            // Copy Constructor Not Implemented
  JsonFilterParametersWriter(JsonFilterParametersWriter&&) = delete;                 // Move Constructor Not Implemented
  JsonFilterParametersWriter& operator=(const JsonFilterParametersWriter&) = delete; // Copy Assignment Not Implemented
  JsonFilterParametersWriter& operator=(JsonFilterParametersWriter&&) = delete;      // Move Assignment Not Implemented
};
