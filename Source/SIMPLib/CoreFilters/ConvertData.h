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
#include "SIMPLib/Common/Constants.h"

/**
 * @brief The ConvertData class. See [Filter documentation](@ref convertdata) for details.
 */
class SIMPLib_EXPORT ConvertData : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ConvertData SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ConvertData)
  PYB11_FILTER_NEW_MACRO(ConvertData)
  PYB11_PROPERTY(SIMPL::NumericTypes::Type ScalarType READ getScalarType WRITE setScalarType)
  PYB11_PROPERTY(QString OutputArrayName READ getOutputArrayName WRITE setOutputArrayName)
  PYB11_PROPERTY(DataArrayPath SelectedCellArrayPath READ getSelectedCellArrayPath WRITE setSelectedCellArrayPath)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ConvertData;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<ConvertData> New();

  /**
   * @brief Returns the name of the class for ConvertData
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ConvertData
   */
  static QString ClassName();

  ~ConvertData() override;

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
   * @brief Setter property for OutputArrayName
   */
  void setOutputArrayName(const QString& value);
  /**
   * @brief Getter property for OutputArrayName
   * @return Value of OutputArrayName
   */
  QString getOutputArrayName() const;

  Q_PROPERTY(QString OutputArrayName READ getOutputArrayName WRITE setOutputArrayName)

  /**
   * @brief Setter property for SelectedCellArrayPath
   */
  void setSelectedCellArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SelectedCellArrayPath
   * @return Value of SelectedCellArrayPath
   */
  DataArrayPath getSelectedCellArrayPath() const;

  Q_PROPERTY(DataArrayPath SelectedCellArrayPath READ getSelectedCellArrayPath WRITE setSelectedCellArrayPath)

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
  ConvertData();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

public:
  ConvertData(const ConvertData&) = delete;            // Copy Constructor Not Implemented
  ConvertData(ConvertData&&) = delete;                 // Move Constructor Not Implemented
  ConvertData& operator=(const ConvertData&) = delete; // Copy Assignment Not Implemented
  ConvertData& operator=(ConvertData&&) = delete;      // Move Assignment Not Implemented

private:
  SIMPL::NumericTypes::Type m_ScalarType = {};
  QString m_OutputArrayName = {};
  DataArrayPath m_SelectedCellArrayPath = {};
};
