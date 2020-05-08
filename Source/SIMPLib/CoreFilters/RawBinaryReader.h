/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Common/Constants.h"

class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

/**
 * @brief The RawBinaryReader class. See [Filter documentation](@ref rawbinaryreader) for details.
 */
class SIMPLib_EXPORT RawBinaryReader : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(RawBinaryReader SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(RawBinaryReader)
  PYB11_FILTER_NEW_MACRO(RawBinaryReader)
  PYB11_PROPERTY(DataArrayPath CreatedAttributeArrayPath READ getCreatedAttributeArrayPath WRITE setCreatedAttributeArrayPath)
  PYB11_PROPERTY(SIMPL::NumericTypes::Type ScalarType READ getScalarType WRITE setScalarType)
  PYB11_PROPERTY(int Endian READ getEndian WRITE setEndian)
  PYB11_PROPERTY(int NumberOfComponents READ getNumberOfComponents WRITE setNumberOfComponents)
  PYB11_PROPERTY(uint64_t SkipHeaderBytes READ getSkipHeaderBytes WRITE setSkipHeaderBytes)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = RawBinaryReader;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<RawBinaryReader> New();

  /**
   * @brief Returns the name of the class for RawBinaryReader
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for RawBinaryReader
   */
  static QString ClassName();

  ~RawBinaryReader() override;

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
   * @brief Setter property for Endian
   */
  void setEndian(int value);
  /**
   * @brief Getter property for Endian
   * @return Value of Endian
   */
  int getEndian() const;

  Q_PROPERTY(int Endian READ getEndian WRITE setEndian)

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
   * @brief Setter property for SkipHeaderBytes
   */
  void setSkipHeaderBytes(uint64_t value);
  /**
   * @brief Getter property for SkipHeaderBytes
   * @return Value of SkipHeaderBytes
   */
  uint64_t getSkipHeaderBytes() const;

  Q_PROPERTY(uint64_t SkipHeaderBytes READ getSkipHeaderBytes WRITE setSkipHeaderBytes)

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
  RawBinaryReader();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DataArrayPath m_CreatedAttributeArrayPath = {};
  SIMPL::NumericTypes::Type m_ScalarType = {};
  int m_Endian = {};
  int m_NumberOfComponents = {};
  uint64_t m_SkipHeaderBytes = {};
  QString m_InputFile = {};

  IDataArrayShPtrType m_Array;

public:
  RawBinaryReader(const RawBinaryReader&) = delete;            // Copy Constructor Not Implemented
  RawBinaryReader(RawBinaryReader&&) = delete;                 // Move Constructor Not Implemented
  RawBinaryReader& operator=(const RawBinaryReader&) = delete; // Copy Assignment Not Implemented
  RawBinaryReader& operator=(RawBinaryReader&&) = delete;      // Move Assignment Not Implemented
};
