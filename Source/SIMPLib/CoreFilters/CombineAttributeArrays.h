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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class IDataArray;
using IDataArrayWkPtrType = std::weak_ptr<IDataArray>;

/**
 * @brief The CombineAttributeArrays class. See [Filter documentation](@ref combineattributearrays) for details.
 */
class SIMPLib_EXPORT CombineAttributeArrays : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(CombineAttributeArrays SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(CombineAttributeArrays)
  PYB11_FILTER_NEW_MACRO(CombineAttributeArrays)
  PYB11_PROPERTY(std::vector<DataArrayPath> SelectedDataArrayPaths READ getSelectedDataArrayPaths WRITE setSelectedDataArrayPaths)
  PYB11_PROPERTY(QString StackedDataArrayName READ getStackedDataArrayName WRITE setStackedDataArrayName)
  PYB11_PROPERTY(bool NormalizeData READ getNormalizeData WRITE setNormalizeData)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = CombineAttributeArrays;
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
   * @brief Returns the name of the class for CombineAttributeArrays
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CombineAttributeArrays
   */
  static QString ClassName();

  ~CombineAttributeArrays() override;

  /**
   * @brief Setter property for SelectedDataArrayPaths
   */
  void setSelectedDataArrayPaths(const std::vector<DataArrayPath>& value);
  /**
   * @brief Getter property for SelectedDataArrayPaths
   * @return Value of SelectedDataArrayPaths
   */
  std::vector<DataArrayPath> getSelectedDataArrayPaths() const;

  Q_PROPERTY(DataArrayPathVec SelectedDataArrayPaths READ getSelectedDataArrayPaths WRITE setSelectedDataArrayPaths)

  /**
   * @brief Setter property for StackedDataArrayName
   */
  void setStackedDataArrayName(const QString& value);
  /**
   * @brief Getter property for StackedDataArrayName
   * @return Value of StackedDataArrayName
   */
  QString getStackedDataArrayName() const;

  Q_PROPERTY(QString StackedDataArrayName READ getStackedDataArrayName WRITE setStackedDataArrayName)

  /**
   * @brief Setter property for NormalizeData
   */
  void setNormalizeData(bool value);
  /**
   * @brief Getter property for NormalizeData
   * @return Value of NormalizeData
   */
  bool getNormalizeData() const;

  Q_PROPERTY(bool NormalizeData READ getNormalizeData WRITE setNormalizeData)

  /**
   * @brief Setter property for MoveValues
   */
  void setMoveValues(bool value);
  /**
   * @brief Getter property for MoveValues
   * @return Value of MoveValues
   */
  bool getMoveValues() const;

  Q_PROPERTY(bool MoveValues READ getMoveValues WRITE setMoveValues)

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

  /**
   * @brief Returns the list of deleted data paths.
   * @return
   */
  std::list<DataArrayPath> getDeletedPaths() override;

protected:
  CombineAttributeArrays();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  IDataArrayWkPtrType m_StackedDataPtr;

  std::vector<DataArrayPath> m_SelectedDataArrayPaths = {};
  QString m_StackedDataArrayName = {SIMPL::GeneralData::CombinedData};
  bool m_NormalizeData = {false};
  bool m_MoveValues = {false};

  QVector<IDataArrayWkPtrType> m_SelectedWeakPtrVector;

public:
  CombineAttributeArrays(const CombineAttributeArrays&) = delete;            // Copy Constructor Not Implemented
  CombineAttributeArrays(CombineAttributeArrays&&) = delete;                 // Move Constructor Not Implemented
  CombineAttributeArrays& operator=(const CombineAttributeArrays&) = delete; // Copy Assignment Not Implemented
  CombineAttributeArrays& operator=(CombineAttributeArrays&&) = delete;      // Move Assignment Not Implemented
};
