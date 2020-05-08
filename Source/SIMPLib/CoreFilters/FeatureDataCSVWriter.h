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
 * @brief The FeatureDataCSVWriter class. See [Filter documentation](@ref featuredatacsvwriter) for details.
 */
class SIMPLib_EXPORT FeatureDataCSVWriter : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(FeatureDataCSVWriter SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(FeatureDataCSVWriter)
  PYB11_FILTER_NEW_MACRO(FeatureDataCSVWriter)
  PYB11_PROPERTY(DataArrayPath CellFeatureAttributeMatrixPath READ getCellFeatureAttributeMatrixPath WRITE setCellFeatureAttributeMatrixPath)
  PYB11_PROPERTY(QString FeatureDataFile READ getFeatureDataFile WRITE setFeatureDataFile)
  PYB11_PROPERTY(bool WriteNeighborListData READ getWriteNeighborListData WRITE setWriteNeighborListData)
  PYB11_PROPERTY(int DelimiterChoice READ getDelimiterChoice WRITE setDelimiterChoice)
  PYB11_PROPERTY(bool WriteNumFeaturesLine READ getWriteNumFeaturesLine WRITE setWriteNumFeaturesLine)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = FeatureDataCSVWriter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<FeatureDataCSVWriter> New();

  /**
   * @brief Returns the name of the class for FeatureDataCSVWriter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FeatureDataCSVWriter
   */
  static QString ClassName();

  ~FeatureDataCSVWriter() override;

  /**
   * @brief Setter property for CellFeatureAttributeMatrixPath
   */
  void setCellFeatureAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellFeatureAttributeMatrixPath
   * @return Value of CellFeatureAttributeMatrixPath
   */
  DataArrayPath getCellFeatureAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath CellFeatureAttributeMatrixPath READ getCellFeatureAttributeMatrixPath WRITE setCellFeatureAttributeMatrixPath)

  /**
   * @brief Setter property for FeatureDataFile
   */
  void setFeatureDataFile(const QString& value);
  /**
   * @brief Getter property for FeatureDataFile
   * @return Value of FeatureDataFile
   */
  QString getFeatureDataFile() const;

  Q_PROPERTY(QString FeatureDataFile READ getFeatureDataFile WRITE setFeatureDataFile)

  /**
   * @brief Setter property for WriteNeighborListData
   */
  void setWriteNeighborListData(bool value);
  /**
   * @brief Getter property for WriteNeighborListData
   * @return Value of WriteNeighborListData
   */
  bool getWriteNeighborListData() const;

  Q_PROPERTY(bool WriteNeighborListData READ getWriteNeighborListData WRITE setWriteNeighborListData)

  /**
   * @brief Setter property for DelimiterChoice
   */
  void setDelimiterChoice(int value);
  /**
   * @brief Getter property for DelimiterChoice
   * @return Value of DelimiterChoice
   */
  int getDelimiterChoice() const;

  Q_PROPERTY(int DelimiterChoice READ getDelimiterChoice WRITE setDelimiterChoice)

  /**
   * @brief Setter property for WriteNumFeaturesLine
   */
  void setWriteNumFeaturesLine(bool value);
  /**
   * @brief Getter property for WriteNumFeaturesLine
   * @return Value of WriteNumFeaturesLine
   */
  bool getWriteNumFeaturesLine() const;

  Q_PROPERTY(bool WriteNumFeaturesLine READ getWriteNumFeaturesLine WRITE setWriteNumFeaturesLine)

  /**
   * @brief Setter property for Delimiter
   */
  void setDelimiter(char value);
  /**
   * @brief Getter property for Delimiter
   * @return Value of Delimiter
   */
  char getDelimiter() const;

  /**
   * @brief getDelimiterChoiceInt Returns the corresponding int from the enum SIMPL::DelimiterTypes::Type for DelimiterChoice
   */
  int getDelimiterChoiceInt() const;

  /**
   * @brief setDelimiterChoiceInt Sets DelimiterChoice to the enum type from SIMPL::DelimiterTypes::Type corresponding to the int parameter
   */
  void setDelimiterChoiceInt(int value);

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
  FeatureDataCSVWriter();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

public:
  FeatureDataCSVWriter(const FeatureDataCSVWriter&) = delete;            // Copy Constructor Not Implemented
  FeatureDataCSVWriter(FeatureDataCSVWriter&&) = delete;                 // Move Constructor Not Implemented
  FeatureDataCSVWriter& operator=(const FeatureDataCSVWriter&) = delete; // Copy Assignment Not Implemented
  FeatureDataCSVWriter& operator=(FeatureDataCSVWriter&&) = delete;      // Move Assignment Not Implemented

private:
  DataArrayPath m_CellFeatureAttributeMatrixPath = {};
  QString m_FeatureDataFile = {};
  bool m_WriteNeighborListData = {};
  int m_DelimiterChoice = {};
  bool m_WriteNumFeaturesLine = {};
  char m_Delimiter = {};
};
