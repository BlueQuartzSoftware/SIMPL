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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;
using IDataArrayWkPtrType = std::weak_ptr<IDataArray>;

/**
 * @brief The WriteASCIIData class. See [Filter documentation](@ref writeasciidata) for details.
 */
class SIMPLib_EXPORT WriteASCIIData : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(WriteASCIIData SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(WriteASCIIData)
  PYB11_FILTER_NEW_MACRO(WriteASCIIData)
  PYB11_PROPERTY(QVector<DataArrayPath> SelectedDataArrayPaths READ getSelectedDataArrayPaths WRITE setSelectedDataArrayPaths)
  PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
  PYB11_PROPERTY(QString OutputFilePath READ getOutputFilePath WRITE setOutputFilePath)
  PYB11_PROPERTY(int Delimiter READ getDelimiter WRITE setDelimiter)
  PYB11_PROPERTY(QString FileExtension READ getFileExtension WRITE setFileExtension)
  PYB11_PROPERTY(int MaxValPerLine READ getMaxValPerLine WRITE setMaxValPerLine)
  PYB11_PROPERTY(int OutputStyle READ getOutputStyle WRITE setOutputStyle)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = WriteASCIIData;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<WriteASCIIData> New();

  /**
   * @brief Returns the name of the class for WriteASCIIData
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for WriteASCIIData
   */
  static QString ClassName();

  ~WriteASCIIData() override;

  /**
   * @brief Setter property for SelectedDataArrayPaths
   */
  void setSelectedDataArrayPaths(const QVector<DataArrayPath>& value);
  /**
   * @brief Getter property for SelectedDataArrayPaths
   * @return Value of SelectedDataArrayPaths
   */
  QVector<DataArrayPath> getSelectedDataArrayPaths() const;

  Q_PROPERTY(QVector<DataArrayPath> SelectedDataArrayPaths READ getSelectedDataArrayPaths WRITE setSelectedDataArrayPaths)

  /**
   * @brief Setter property for OutputPath
   */
  void setOutputPath(const QString& value);
  /**
   * @brief Getter property for OutputPath
   * @return Value of OutputPath
   */
  QString getOutputPath() const;

  Q_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)

  /**
   * @brief Setter property for OutputFilePath
   */
  void setOutputFilePath(const QString& value);
  /**
   * @brief Getter property for OutputFilePath
   * @return Value of OutputFilePath
   */
  QString getOutputFilePath() const;

  Q_PROPERTY(QString OutputFilePath READ getOutputFilePath WRITE setOutputFilePath)

  /**
   * @brief Setter property for Delimiter
   */
  void setDelimiter(int value);
  /**
   * @brief Getter property for Delimiter
   * @return Value of Delimiter
   */
  int getDelimiter() const;

  Q_PROPERTY(int Delimiter READ getDelimiter WRITE setDelimiter)

  /**
   * @brief Setter property for FileExtension
   */
  void setFileExtension(const QString& value);
  /**
   * @brief Getter property for FileExtension
   * @return Value of FileExtension
   */
  QString getFileExtension() const;

  Q_PROPERTY(QString FileExtension READ getFileExtension WRITE setFileExtension)

  /**
   * @brief Setter property for MaxValPerLine
   */
  void setMaxValPerLine(int value);
  /**
   * @brief Getter property for MaxValPerLine
   * @return Value of MaxValPerLine
   */
  int getMaxValPerLine() const;

  Q_PROPERTY(int MaxValPerLine READ getMaxValPerLine WRITE setMaxValPerLine)

  /**
   * @brief Setter property for OutputStyle
   */
  void setOutputStyle(int value);
  /**
   * @brief Getter property for OutputStyle
   * @return Value of OutputStyle
   */
  int getOutputStyle() const;

  Q_PROPERTY(int OutputStyle READ getOutputStyle WRITE setOutputStyle)

  enum DelimiterType
  {
    Comma = 0,
    Semicolon = 1,
    Space = 2,
    Colon = 3,
    Tab = 4
  };

  enum OutputType
  {
    MultiFile = 0,
    SingleFile = 1
  };

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
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  WriteASCIIData();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  QVector<DataArrayPath> m_SelectedDataArrayPaths = {};
  QString m_OutputPath = {};
  QString m_OutputFilePath = {};
  int m_Delimiter = {};
  QString m_FileExtension = {};
  int m_MaxValPerLine = {};
  int m_OutputStyle = {};

  /**
   * @brief lookupDelimiter Returns the char representation for the
   * selected delimiter
   * @return Char for delimiter
   */
  char lookupDelimiter();

  /**
   * @brief Specific function to write string arrays to a text file
   * @param inputData
   */
  void writeStringArray(const IDataArrayShPtrType& inputData, const QString& outputFile, char delimiter);

  /**
   * @brief m_SelectedWeakPtrVector
   */
  QVector<IDataArrayWkPtrType> m_SelectedWeakPtrVector;

  /**
   * @brief writeMultifileOutput
   */
  void writeMultiFileOutput();

  /**
   * @brief writeSingleFileOutput
   */
  void writeSingleFileOutput();

public:
  WriteASCIIData(const WriteASCIIData&) = delete;            // Copy Constructor Not Implemented
  WriteASCIIData(WriteASCIIData&&) = delete;                 // Move Constructor Not Implemented
  WriteASCIIData& operator=(const WriteASCIIData&) = delete; // Copy Assignment Not Implemented
  WriteASCIIData& operator=(WriteASCIIData&&) = delete;      // Move Assignment Not Implemented
};
