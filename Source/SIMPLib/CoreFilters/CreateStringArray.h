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
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

/**
 * @brief The CreateStringArray class. See [Filter documentation](@ref CreateStringArray) for details.
 */
class SIMPLib_EXPORT CreateStringArray : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(CreateStringArray SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(CreateStringArray)
  PYB11_FILTER_NEW_MACRO(CreateStringArray)
  PYB11_PROPERTY(int NumberOfComponents READ getNumberOfComponents WRITE setNumberOfComponents)
  PYB11_PROPERTY(DataArrayPath NewArray READ getNewArray WRITE setNewArray)
  PYB11_PROPERTY(QString InitializationValue READ getInitializationValue WRITE setInitializationValue)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = CreateStringArray;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<CreateStringArray> New();

  /**
   * @brief Returns the name of the class for CreateStringArray
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CreateStringArray
   */
  static QString ClassName();

  ~CreateStringArray() override;

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
   * @brief Setter property for NewArray
   */
  void setNewArray(const DataArrayPath& value);
  /**
   * @brief Getter property for NewArray
   * @return Value of NewArray
   */
  DataArrayPath getNewArray() const;

  Q_PROPERTY(DataArrayPath NewArray READ getNewArray WRITE setNewArray)

  /**
   * @brief Setter property for InitializationValue
   */
  void setInitializationValue(const QString& value);
  /**
   * @brief Getter property for InitializationValue
   * @return Value of InitializationValue
   */
  QString getInitializationValue() const;

  Q_PROPERTY(QString InitializationValue READ getInitializationValue WRITE setInitializationValue)

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
  CreateStringArray();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  int m_NumberOfComponents = {0};
  DataArrayPath m_NewArray = {"", "", ""};
  QString m_InitializationValue = {"Text"};

  IDataArray::WeakPointer m_OutputArrayPtr;

public:
  CreateStringArray(const CreateStringArray&) = delete;            // Copy Constructor Not Implemented
  CreateStringArray(CreateStringArray&&) = delete;                 // Move Constructor Not Implemented
  CreateStringArray& operator=(const CreateStringArray&) = delete; // Copy Assignment Not Implemented
  CreateStringArray& operator=(CreateStringArray&&) = delete;      // Move Assignment Not Implemented
};
