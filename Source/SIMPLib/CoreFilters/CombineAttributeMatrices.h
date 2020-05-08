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
#include "SIMPLib/DataArrays/DataArray.hpp"

/**
 * @brief The CombineAttributeMatrices class. See [Filter documentation](@ref CombineAttributeMatrices) for details.
 */
class SIMPLib_EXPORT CombineAttributeMatrices : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(CombineAttributeMatrices SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(CombineAttributeMatrices)
  PYB11_FILTER_NEW_MACRO(CombineAttributeMatrices)
  PYB11_PROPERTY(DataArrayPath FirstAttributeMatrixPath READ getFirstAttributeMatrixPath WRITE setFirstAttributeMatrixPath)
  PYB11_PROPERTY(DataArrayPath SecondAttributeMatrixPath READ getSecondAttributeMatrixPath WRITE setSecondAttributeMatrixPath)
  PYB11_PROPERTY(DataArrayPath FirstIndexArrayPath READ getFirstIndexArrayPath WRITE setFirstIndexArrayPath)
  PYB11_PROPERTY(DataArrayPath SecondIndexArrayPath READ getSecondIndexArrayPath WRITE setSecondIndexArrayPath)
  PYB11_PROPERTY(QString CombinedAttributeMatrixName READ getCombinedAttributeMatrixName WRITE setCombinedAttributeMatrixName)
  PYB11_PROPERTY(QString NewIndexArrayName READ getNewIndexArrayName WRITE setNewIndexArrayName)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = CombineAttributeMatrices;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<CombineAttributeMatrices> New();

  /**
   * @brief Returns the name of the class for CombineAttributeMatrices
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CombineAttributeMatrices
   */
  static QString ClassName();

  ~CombineAttributeMatrices() override;

  /**
   * @brief Setter property for FirstAttributeMatrixPath
   */
  void setFirstAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FirstAttributeMatrixPath
   * @return Value of FirstAttributeMatrixPath
   */
  DataArrayPath getFirstAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath FirstAttributeMatrixPath READ getFirstAttributeMatrixPath WRITE setFirstAttributeMatrixPath)

  /**
   * @brief Setter property for SecondAttributeMatrixPath
   */
  void setSecondAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SecondAttributeMatrixPath
   * @return Value of SecondAttributeMatrixPath
   */
  DataArrayPath getSecondAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath SecondAttributeMatrixPath READ getSecondAttributeMatrixPath WRITE setSecondAttributeMatrixPath)

  /**
   * @brief Setter property for FirstIndexArrayPath
   */
  void setFirstIndexArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FirstIndexArrayPath
   * @return Value of FirstIndexArrayPath
   */
  DataArrayPath getFirstIndexArrayPath() const;

  Q_PROPERTY(DataArrayPath FirstIndexArrayPath READ getFirstIndexArrayPath WRITE setFirstIndexArrayPath)

  /**
   * @brief Setter property for SecondIndexArrayPath
   */
  void setSecondIndexArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SecondIndexArrayPath
   * @return Value of SecondIndexArrayPath
   */
  DataArrayPath getSecondIndexArrayPath() const;

  Q_PROPERTY(DataArrayPath SecondIndexArrayPath READ getSecondIndexArrayPath WRITE setSecondIndexArrayPath)

  /**
   * @brief Setter property for CombinedAttributeMatrixName
   */
  void setCombinedAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CombinedAttributeMatrixName
   * @return Value of CombinedAttributeMatrixName
   */
  QString getCombinedAttributeMatrixName() const;

  Q_PROPERTY(QString CombinedAttributeMatrixName READ getCombinedAttributeMatrixName WRITE setCombinedAttributeMatrixName)

  /**
   * @brief Setter property for NewIndexArrayName
   */
  void setNewIndexArrayName(const QString& value);
  /**
   * @brief Getter property for NewIndexArrayName
   * @return Value of NewIndexArrayName
   */
  QString getNewIndexArrayName() const;

  Q_PROPERTY(QString NewIndexArrayName READ getNewIndexArrayName WRITE setNewIndexArrayName)

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

  /**
   * @brief Returns the list of deleted data paths.
   * @return
   */
  std::list<DataArrayPath> getDeletedPaths();

protected:
  CombineAttributeMatrices();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief updateFeatureInstancePointers Updates raw feature pointers
   */
  void updateFeatureInstancePointers();

private:
  std::weak_ptr<DataArray<int32_t>> m_FirstIndexPtr;
  int32_t* m_FirstIndex = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SecondIndexPtr;
  int32_t* m_SecondIndex = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_NewIndexPtr;
  int32_t* m_NewIndex = nullptr;

  DataArrayPath m_FirstAttributeMatrixPath = {};
  DataArrayPath m_SecondAttributeMatrixPath = {};
  DataArrayPath m_FirstIndexArrayPath = {};
  DataArrayPath m_SecondIndexArrayPath = {};
  QString m_CombinedAttributeMatrixName = {};
  QString m_NewIndexArrayName = {};

public:
  CombineAttributeMatrices(const CombineAttributeMatrices&) = delete;            // Copy Constructor Not Implemented
  CombineAttributeMatrices(CombineAttributeMatrices&&) = delete;                 // Move Constructor Not Implemented
  CombineAttributeMatrices& operator=(const CombineAttributeMatrices&) = delete; // Copy Assignment Not Implemented
  CombineAttributeMatrices& operator=(CombineAttributeMatrices&&) = delete;      // Move Assignment Not Implemented
};
