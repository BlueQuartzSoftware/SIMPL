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

#include <QtCore/QJsonParseError>
#include <QtCore/QString>

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The JsonFilterParametersReader class
 */
class SIMPLib_EXPORT JsonFilterParametersReader : public AbstractFilterParametersReader
{
public:
  using Self = JsonFilterParametersReader;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for JsonFilterParametersReader
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for JsonFilterParametersReader
   */
  static QString ClassName();

  ~JsonFilterParametersReader() override;

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
   * @brief Setter property for MaxFilterIndex
   */
  void setMaxFilterIndex(int value);
  /**
   * @brief Getter property for MaxFilterIndex
   * @return Value of MaxFilterIndex
   */
  int getMaxFilterIndex() const;

  /**
   * @brief ReadPipelineFromFile Reads the Json formatted file and returns a FilterPipeline object
   * that contains all the filters that could be found. If a filter can not be found then that filter is simply skipped.
   * If the IObserver is NON-Null then an error message will be passed to it with an error message
   * @param filePath The path to the INI formatted file
   * @param format The format of the file which is anything that QSettings understands
   * @param obs An IObserver object to report errors.
   * @return Shared Pointer to a FilterPipeline Instance
   */
  FilterPipeline::Pointer readPipelineFromFile(QString filePath, IObserver* obs = nullptr);

  /**
   * @brief getJsonFromFile Reads the Json formatted file and returns a json string
   * that contains all the filters that could be found. If a filter can not be found then that filter is simply skipped.
   * If the IObserver is NON-Null then an error message will be passed to it with an error message
   * @param filePath The path to the INI formatted file
   * @param format The format of the file which is anything that QSettings understands
   * @param obs An IObserver object to report errors.
   * @return Shared Pointer to a FilterPipeline Instance
   */
  QString getJsonFromFile(QString filePath, IObserver* obs = nullptr);

  /**
   * @brief ReadPipelineFromString Reads the Json formatted file and returns a FilterPipeline object
   * that contains all the filters that could be found. If a filter can not be found then that filter is simply skipped.
   * If the IObserver is NON-Null then an error message will be passed to it with an error message
   * @param filePath The path to the INI formatted file
   * @param format The format of the file which is anything that QSettings understands
   * @param obs An IObserver object to report errors.
   * @return Shared Pointer to a FilterPipeline Instance
   */
  FilterPipeline::Pointer readPipelineFromString(QString contents, IObserver* obs = nullptr);

  /**
   * @brief Gets the name of the pipeline from a pipeline file
   * @param filePath The absolute path to the pipeline file.
   * @param name Sets the name of the pipeline into this variable
   * @param version Sets the DREAM3D Version into this variable.
   * @param obs Any observer that needs to know about errors.
   * @return
   */
  void readNameOfPipelineFromFile(QString filePath, QString& name, QString& version, IObserver* obs = nullptr);

  /**
   * @brief Generate an html formatted string that summarizes the pipeline file by listing
   * each filter by it's human name and C++ class name.
   * @param filePath The path to the pipeline file
   * @param obs Any observer that needs to know about errors.
   * @return html formatted string
   */
  static QString HtmlSummaryFromFile(QString filePath, IObserver* obs);

  /**
   * @brief Opens the pipeline file for operations
   * @param filePath The path to the pipeline file
   * @return
   */
  QJsonParseError openFile(QString filePath);

  /**
   * @brief setPipelineContents
   * @param contents
   * @return
   */
  int setPipelineContents(QString contents);

  /**
   * @brief closeFile
   */
  void closeFile();

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

  /*
   * @brief getCurrentGroupObject
   * @return
   */
  QJsonObject& getCurrentGroupObject();

  bool containsGroup(QString key);

  int openGroup(QString key);
  int closeGroup();

protected:
  JsonFilterParametersReader();

private:
  QString m_FileName = {};
  int m_MaxFilterIndex = {};

  QJsonObject m_Root;
  QJsonObject m_CurrentFilterIndex;

  FilterPipeline::Pointer readPipeline(IObserver* obs);

public:
  JsonFilterParametersReader(const JsonFilterParametersReader&) = delete;            // Copy Constructor Not Implemented
  JsonFilterParametersReader(JsonFilterParametersReader&&) = delete;                 // Move Constructor Not Implemented
  JsonFilterParametersReader& operator=(const JsonFilterParametersReader&) = delete; // Copy Assignment Not Implemented
  JsonFilterParametersReader& operator=(JsonFilterParametersReader&&) = delete;      // Move Assignment Not Implemented
};
