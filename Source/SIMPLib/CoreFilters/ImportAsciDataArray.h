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

#include <QtCore/QScopedPointer>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Common/Constants.h"

class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

// our PIMPL private class for caching
class ImportAsciDataArrayPrivate;

/**
 * @brief The ImportAsciDataArray class. See [Filter documentation](@ref ImportAsciDataArray) for details.
 */
class SIMPLib_EXPORT ImportAsciDataArray : public AbstractFilter
{
  Q_OBJECT
  Q_DECLARE_PRIVATE(ImportAsciDataArray)

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ImportAsciDataArray SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ImportAsciDataArray)
  PYB11_FILTER_NEW_MACRO(ImportAsciDataArray)
  PYB11_PROPERTY(DataArrayPath CreatedAttributeArrayPath READ getCreatedAttributeArrayPath WRITE setCreatedAttributeArrayPath)
  PYB11_PROPERTY(SIMPL::NumericTypes::Type ScalarType READ getScalarType WRITE setScalarType)
  PYB11_PROPERTY(int NumberOfComponents READ getNumberOfComponents WRITE setNumberOfComponents)
  PYB11_PROPERTY(int SkipHeaderLines READ getSkipHeaderLines WRITE setSkipHeaderLines)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(int Delimiter READ getDelimiter WRITE setDelimiter)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ImportAsciDataArray;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<ImportAsciDataArray> New();

  /**
   * @brief Returns the name of the class for ImportAsciDataArray
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ImportAsciDataArray
   */
  static QString ClassName();

  ~ImportAsciDataArray() override;

  /**
   * @brief Setter property for CreatedAttributeArrayPath
   */
  void setCreatedAttributeArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CreatedAttributeArrayPath
   * @return Value of CreatedAttributeArrayPath
   */
  DataArrayPath getCreatedAttributeArrayPath() const;

  Q_PROPERTY(DataArrayPath CreatedAttributeArrayPath READ getCreatedAttributeArrayPath WRITE setCreatedAttributeArrayPath)

  /**
   * @brief Setter property for ScalarType
   */
  void setScalarType(SIMPL::NumericTypes::Type value);
  /**
   * @brief Getter property for ScalarType
   * @return Value of ScalarType
   */
  SIMPL::NumericTypes::Type getScalarType() const;

  Q_PROPERTY(SIMPL::NumericTypes::Type ScalarType READ getScalarType WRITE setScalarType)

  /**
   * @brief Setter property for NumberOfComponents
   */
  void setNumberOfComponents(int value);
  /**
   * @brief Getter property for NumberOfComponents
   * @return Value of NumberOfComponents
   */
  int getNumberOfComponents() const;

  Q_PROPERTY(int NumberOfComponents READ getNumberOfComponents WRITE setNumberOfComponents)

  /**
   * @brief Setter property for SkipHeaderLines
   */
  void setSkipHeaderLines(int value);
  /**
   * @brief Getter property for SkipHeaderLines
   * @return Value of SkipHeaderLines
   */
  int getSkipHeaderLines() const;

  Q_PROPERTY(int SkipHeaderLines READ getSkipHeaderLines WRITE setSkipHeaderLines)

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
   * @brief Setter property for Delimiter
   */
  void setDelimiter(int value);
  /**
   * @brief Getter property for Delimiter
   * @return Value of Delimiter
   */
  int getDelimiter() const;

  Q_PROPERTY(int Delimiter READ getDelimiter WRITE setDelimiter)

  Q_PROPERTY(QString FirstLine READ getFirstLine)

  /**
   * @brief Setter property for FirstLine
   */
  void setFirstLine(const QString& value);
  /**
   * @brief Getter property for FirstLine
   * @return Value of FirstLine
   */
  QString getFirstLine() const;

  /**
   * @brief Setter property for InputFile_Cache
   */
  void setInputFile_Cache(const QString& value);
  /**
   * @brief Getter property for InputFile_Cache
   * @return Value of InputFile_Cache
   */
  QString getInputFile_Cache() const;

  /**
   * @brief Setter property for LastRead
   */
  void setLastRead(const QDateTime& value);
  /**
   * @brief Getter property for LastRead
   * @return Value of LastRead
   */
  QDateTime getLastRead() const;

  /**
   * @brief Setter property for HeaderLines
   */
  void setHeaderLines(int value);
  /**
   * @brief Getter property for HeaderLines
   * @return Value of HeaderLines
   */
  int getHeaderLines() const;

  enum DelimeterType
  {
    Comma = 0,
    Semicolon = 1,
    Space = 2,
    Colon = 3,
    Tab = 4
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
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  ImportAsciDataArray();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief Converts from the enumeration to a char
   * @return
   */
  char converSelectedDelimiter();

  /**
   * @brief readHeaderPortion
   * @return
   */
  void readHeaderPortion();

private:
  DataArrayPath m_CreatedAttributeArrayPath = {};
  SIMPL::NumericTypes::Type m_ScalarType = {};
  int m_NumberOfComponents = {};
  int m_SkipHeaderLines = {};
  QString m_InputFile = {};
  int m_Delimiter = {};

  QScopedPointer<ImportAsciDataArrayPrivate> const d_ptr;

  IDataArrayShPtrType m_Array;

public:
  ImportAsciDataArray(const ImportAsciDataArray&) = delete;            // Copy Constructor Not Implemented
  ImportAsciDataArray(ImportAsciDataArray&&) = delete;                 // Move Constructor Not Implemented
  ImportAsciDataArray& operator=(const ImportAsciDataArray&) = delete; // Copy Assignment Not Implemented
  ImportAsciDataArray& operator=(ImportAsciDataArray&&) = delete;      // Move Assignment Not Implemented
};
