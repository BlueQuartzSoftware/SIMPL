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
#include "SIMPLib/FilterParameters/RangeFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Common/Constants.h"

class IDataArray;
using IDataArrayWkPtrType = std::weak_ptr<IDataArray>;

/**
 * @brief The CreateDataArray class. See [Filter documentation](@ref createdataarray) for details.
 */
class SIMPLib_EXPORT CreateDataArray : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(CreateDataArray SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(CreateDataArray)
  PYB11_FILTER_NEW_MACRO(CreateDataArray)
  PYB11_PROPERTY(SIMPL::ScalarTypes::Type ScalarType READ getScalarType WRITE setScalarType)
  PYB11_PROPERTY(int NumberOfComponents READ getNumberOfComponents WRITE setNumberOfComponents)
  PYB11_PROPERTY(DataArrayPath NewArray READ getNewArray WRITE setNewArray)
  PYB11_PROPERTY(int InitializationType READ getInitializationType WRITE setInitializationType)
  PYB11_PROPERTY(QString InitializationValue READ getInitializationValue WRITE setInitializationValue)
  PYB11_PROPERTY(FPRangePair InitializationRange READ getInitializationRange WRITE setInitializationRange)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = CreateDataArray;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<CreateDataArray> New();

  /**
   * @brief Returns the name of the class for CreateDataArray
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CreateDataArray
   */
  static QString ClassName();

  ~CreateDataArray() override;

  enum InitializationChoices
  {
    Manual,
    RandomWithRange
  };

  /**
   * @brief Setter property for ScalarType
   */
  void setScalarType(SIMPL::ScalarTypes::Type value);
  /**
   * @brief Getter property for ScalarType
   * @return Value of ScalarType
   */
  SIMPL::ScalarTypes::Type getScalarType() const;

  Q_PROPERTY(SIMPL::ScalarTypes::Type ScalarType READ getScalarType WRITE setScalarType)

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
   * @brief Setter property for InitializationType
   */
  void setInitializationType(int value);
  /**
   * @brief Getter property for InitializationType
   * @return Value of InitializationType
   */
  int getInitializationType() const;

  Q_PROPERTY(int InitializationType READ getInitializationType WRITE setInitializationType)

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
   * @brief Setter property for InitializationRange
   */
  void setInitializationRange(const FPRangePair& value);
  /**
   * @brief Getter property for InitializationRange
   * @return Value of InitializationRange
   */
  FPRangePair getInitializationRange() const;

  Q_PROPERTY(FPRangePair InitializationRange READ getInitializationRange WRITE setInitializationRange)

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
  CreateDataArray();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  IDataArrayWkPtrType m_OutputArrayPtr;

  SIMPL::ScalarTypes::Type m_ScalarType = {SIMPL::ScalarTypes::Type::Int8};
  int m_NumberOfComponents = {0};
  DataArrayPath m_NewArray = {"", "", ""};
  int m_InitializationType = {Manual};
  QString m_InitializationValue = {"0"};
  FPRangePair m_InitializationRange = {};

  /**
   * @brief checkInitialization Checks that the chosen initialization value/range is inside
   * the bounds of the array type
   */
  template <typename T>
  void checkInitialization(QString dataArrayName);

  const QString Int8 = "signed   int 8  bit";
  const QString UInt8 = "unsigned int 8  bit";
  const QString Int16 = "signed   int 16 bit";
  const QString UInt16 = "unsigned int 16 bit";
  const QString Int32 = "signed   int 32 bit";
  const QString UInt32 = "unsigned int 32 bit";
  const QString Int64 = "signed   int 64 bit";
  const QString UInt64 = "unsigned int 64 bit";
  const QString Float = "       Float 32 bit";
  const QString Double = "      Double 64 bit";
  const QString Bool = "bool";

public:
  CreateDataArray(const CreateDataArray&) = delete;            // Copy Constructor Not Implemented
  CreateDataArray(CreateDataArray&&) = delete;                 // Move Constructor Not Implemented
  CreateDataArray& operator=(const CreateDataArray&) = delete; // Copy Assignment Not Implemented
  CreateDataArray& operator=(CreateDataArray&&) = delete;      // Move Assignment Not Implemented
};
