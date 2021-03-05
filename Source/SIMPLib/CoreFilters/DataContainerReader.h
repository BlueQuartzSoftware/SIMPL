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

#include <QtCore/QDateTime>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
class DataContainerArray;
using DataContainerArrayShPtrType = std::shared_ptr<DataContainerArray>;

class SIMPLH5DataReader;

/**
 * @brief The DataContainerReader class. See [Filter documentation](@ref datacontainerreader) for details.
 */
class SIMPLib_EXPORT DataContainerReader : public AbstractFilter
{
  Q_OBJECT

  // This line MUST be first when exposing a class and properties to Python

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(DataContainerReader SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(DataContainerReader)
  PYB11_FILTER_NEW_MACRO(DataContainerReader)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(bool OverwriteExistingDataContainers READ getOverwriteExistingDataContainers WRITE setOverwriteExistingDataContainers)
  PYB11_PROPERTY(DataContainerArrayProxy InputFileDataContainerArrayProxy READ getInputFileDataContainerArrayProxy WRITE setInputFileDataContainerArrayProxy)
  PYB11_METHOD(DataContainerArrayProxy readDataContainerArrayStructure ARGS path)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = DataContainerReader;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<DataContainerReader> New();

  /**
   * @brief Returns the name of the class for DataContainerReader
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for DataContainerReader
   */
  static QString ClassName();

  ~DataContainerReader() override;

  /**
   * @brief Setter property for InputFile
   */
  void setInputFile(const QString& value);
  /**
   * @brief Getter property for InputFile
   * @return Value of InputFile
   */
  QString getInputFile() const;

  Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

  /**
   * @brief Setter property for OverwriteExistingDataContainers
   */
  void setOverwriteExistingDataContainers(bool value);
  /**
   * @brief Getter property for OverwriteExistingDataContainers
   * @return Value of OverwriteExistingDataContainers
   */
  bool getOverwriteExistingDataContainers() const;

  Q_PROPERTY(bool OverwriteExistingDataContainers READ getOverwriteExistingDataContainers WRITE setOverwriteExistingDataContainers)

  /**
   * @brief Setter property for LastFileRead
   */
  void setLastFileRead(const QString& value);
  /**
   * @brief Getter property for LastFileRead
   * @return Value of LastFileRead
   */
  QString getLastFileRead() const;

  Q_PROPERTY(QString LastFileRead READ getLastFileRead WRITE setLastFileRead)

  /**
   * @brief Setter property for LastRead
   */
  void setLastRead(const QDateTime& value);
  /**
   * @brief Getter property for LastRead
   * @return Value of LastRead
   */
  QDateTime getLastRead() const;

  Q_PROPERTY(QDateTime LastRead READ getLastRead WRITE setLastRead)

  /**
   * @brief Setter property for InputFileDataContainerArrayProxy
   */
  void setInputFileDataContainerArrayProxy(const DataContainerArrayProxy& value);
  /**
   * @brief Getter property for InputFileDataContainerArrayProxy
   * @return Value of InputFileDataContainerArrayProxy
   */
  DataContainerArrayProxy getInputFileDataContainerArrayProxy() const;

  Q_PROPERTY(DataContainerArrayProxy InputFileDataContainerArrayProxy READ getInputFileDataContainerArrayProxy WRITE setInputFileDataContainerArrayProxy)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief This method is called just before the writeFilterParameters() completes
   * @param obj The json object to add the filter parameters into
   * @param rootObject The parent object of **obj**
   */
  void preWriteFilterParameters(QJsonObject& obj, QJsonObject& rootObject) override;

  /**
   * @brief writeFilterParameters Reimplemented from @see AbstractFilter class
   */
  void writeFilterParameters(QJsonObject& obj) const override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(QJsonObject& obj) override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
   * @brief readExistingPipelineFromFile Reads the existing pipeline that is stored in the file and store it
   * in the class instance for later writing to another SIMPLView data file
   * @param fileId HDF5 group Id for pipeline
   * @return Integer error value
   */
  int readExistingPipelineFromFile(hid_t fileId);

  /**
   * @brief writeExistingPipelineToFile Writes the filter parameters of the existing pipline to a
   * SIMPLView file
   * @param obj json object
   * @param index The index that the inserted pipeline should start at.
   * @return
   */
  int writeExistingPipelineToFile(QJsonObject& obj, int index);

  /**
   * @brief readDataContainerArrayStructure Reads the structure of the DataContainerArray from the HDF5 based .dream3d file. For this method to work
   * the member variable for the file path should have been set prior to calling this method.
   * @param path The file path to the SIMPLView file
   * @return DataContainerArrayProxy instance
   */
  DataContainerArrayProxy readDataContainerArrayStructure(const QString& path);

  /**
   * @brief syncProxies Combines the file and cached proxies if they are out-of-sync
   * @return
   */
  bool syncProxies();

protected:
  DataContainerReader();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief readData
   * @param proxy
   * @return
   */
  DataContainerArrayShPtrType readData(DataContainerArrayProxy& proxy);

  /**
   * @brief readMontageGroup
   * @param dca
   * @return
   */
  DataContainerArray::MontageCollection readMontageGroup(const DataContainerArray::Pointer& dca);

protected Q_SLOTS:
  /**
   * @brief Cleans up the filter after execution
   */
  void cleanupFilter() override;

private:
  QString m_InputFile = {""};
  bool m_OverwriteExistingDataContainers = {false};
  QString m_LastFileRead = {""};
  QDateTime m_LastRead = {QDateTime::currentDateTime()};
  DataContainerArrayProxy m_InputFileDataContainerArrayProxy = {};

  FilterPipeline::Pointer m_PipelineFromFile;

public:
  DataContainerReader(const DataContainerReader&) = delete;            // Copy Constructor Not Implemented
  DataContainerReader(DataContainerReader&&) = delete;                 // Move Constructor Not Implemented
  DataContainerReader& operator=(const DataContainerReader&) = delete; // Copy Assignment Not Implemented
  DataContainerReader& operator=(DataContainerReader&&) = delete;      // Move Assignment Not Implemented
};
