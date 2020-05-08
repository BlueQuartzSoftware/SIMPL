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
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/CoreFilters/util/ASCIIWizardData.hpp"

class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

/**
 * @brief The ReadASCIIData class. See [Filter documentation](@ref ReadASCIIData) for details.
 */
class SIMPLib_EXPORT ReadASCIIData : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ReadASCIIData SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ReadASCIIData)
  PYB11_FILTER_NEW_MACRO(ReadASCIIData)
  PYB11_PROPERTY(ASCIIWizardData WizardData READ getWizardData WRITE setWizardData)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ReadASCIIData;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;

  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for ReadASCIIData
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ReadASCIIData
   */
  static QString ClassName();

  ~ReadASCIIData() override;

  /**
   * @brief Setter property for WizardData
   */
  void setWizardData(const ASCIIWizardData& value);
  /**
   * @brief Getter property for WizardData
   * @return Value of WizardData
   */
  ASCIIWizardData getWizardData() const;

  Q_PROPERTY(ASCIIWizardData WizardData READ getWizardData WRITE setWizardData)

  enum ErrorCodes
  {
    EMPTY_FILE = -100,
    EMPTY_ATTR_MATRIX = -101,
    INCONSISTENT_TUPLES = -102,
    INCONSISTENT_COLS = -103,
    CONVERSION_FAILURE = -104,
    DUPLICATE_NAMES = -105,
    INVALID_ARRAY_TYPE = -106
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
   * @brief readFilterParametersFromJson Reads the filter parameters from a file
   * @param reader Reader that is used to read the parameters from a file
   */
  void readFilterParameters(QJsonObject& obj) override;

  /**
   * @brief writeFilterParametersToJson Writes the filter parameters to a file
   * @param root The root json object
   */
  void writeFilterParameters(QJsonObject& obj) const override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
   * @brief Updates any DataArrayPath properties from the old path to a new path
   * For DataArrayPaths longer than the given path, only the specified values are modified
   * @param renamePath
   */
  void renameDataArrayPath(const DataArrayPath::RenameType& renamePath) override;

protected:
  ReadASCIIData();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  ASCIIWizardData m_WizardData = {};

  QMap<int, IDataArrayShPtrType> m_ASCIIArrayMap;

public:
  ReadASCIIData(const ReadASCIIData&) = delete;            // Copy Constructor Not Implemented
  ReadASCIIData(ReadASCIIData&&) = delete;                 // Move Constructor Not Implemented
  ReadASCIIData& operator=(const ReadASCIIData&) = delete; // Copy Assignment Not Implemented
  ReadASCIIData& operator=(ReadASCIIData&&) = delete;      // Move Assignment Not Implemented
};
