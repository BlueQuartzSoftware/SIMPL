/* ============================================================================
 * Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include <QtCore/QStack>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

#include "util/ICalculatorArray.h"

class AttributeMatrix;
using AttributeMatrixShPtrType = std::shared_ptr<AttributeMatrix>;
class CalculatorItem;
using CalculatorItemShPtrType = std::shared_ptr<CalculatorItem>;

/**
 * @brief The ArrayCalculator class. See [Filter documentation](@ref createdatacontainer) for details.
 */
class SIMPLib_EXPORT ArrayCalculator : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ArrayCalculator SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ArrayCalculator)
  PYB11_FILTER_NEW_MACRO(ArrayCalculator)
  PYB11_PROPERTY(DataArrayPath SelectedAttributeMatrix READ getSelectedAttributeMatrix WRITE setSelectedAttributeMatrix)
  PYB11_PROPERTY(QString InfixEquation READ getInfixEquation WRITE setInfixEquation)
  PYB11_PROPERTY(DataArrayPath CalculatedArray READ getCalculatedArray WRITE setCalculatedArray)
  PYB11_PROPERTY(ArrayCalculator::AngleUnits Units READ getUnits WRITE setUnits)
  PYB11_PROPERTY(SIMPL::ScalarTypes::Type ScalarType READ getScalarType WRITE setScalarType)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  enum AngleUnits
  {
    Degrees,
    Radians
  };

  Q_ENUMS(AngleUnits)

  using Self = ArrayCalculator;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<ArrayCalculator> New();

  /**
   * @brief Returns the name of the class for ArrayCalculator
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ArrayCalculator
   */
  static QString ClassName();

  /**
   * @brief Setter property for SelectedAttributeMatrix
   */
  void setSelectedAttributeMatrix(const DataArrayPath& value);
  /**
   * @brief Getter property for SelectedAttributeMatrix
   * @return Value of SelectedAttributeMatrix
   */
  DataArrayPath getSelectedAttributeMatrix() const;

  Q_PROPERTY(DataArrayPath SelectedAttributeMatrix READ getSelectedAttributeMatrix WRITE setSelectedAttributeMatrix)

  /**
   * @brief Setter property for InfixEquation
   */
  void setInfixEquation(const QString& value);
  /**
   * @brief Getter property for InfixEquation
   * @return Value of InfixEquation
   */
  QString getInfixEquation() const;

  Q_PROPERTY(QString InfixEquation READ getInfixEquation WRITE setInfixEquation)

  /**
   * @brief Setter property for CalculatedArray
   */
  void setCalculatedArray(const DataArrayPath& value);
  /**
   * @brief Getter property for CalculatedArray
   * @return Value of CalculatedArray
   */
  DataArrayPath getCalculatedArray() const;

  Q_PROPERTY(DataArrayPath CalculatedArray READ getCalculatedArray WRITE setCalculatedArray)

  /**
   * @brief Setter property for Units
   */
  void setUnits(const ArrayCalculator::AngleUnits& value);
  /**
   * @brief Getter property for Units
   * @return Value of Units
   */
  ArrayCalculator::AngleUnits getUnits() const;

  Q_PROPERTY(AngleUnits Units READ getUnits WRITE setUnits)

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

  ~ArrayCalculator() override;

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

protected:
  ArrayCalculator();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief Converts the given data array to one of the specified scalar type
   * @param inputArray
   * @param scalarType
   * @return
   */
  IDataArrayShPtrType convertArrayType(const IDataArrayShPtrType& inputArray, SIMPL::ScalarTypes::Type scalarType);

private:
  DataArrayPath m_SelectedAttributeMatrix = {};
  QString m_InfixEquation = {};
  DataArrayPath m_CalculatedArray = {};
  ArrayCalculator::AngleUnits m_Units = {};
  SIMPL::ScalarTypes::Type m_ScalarType = {};

  QMap<QString, CalculatorItemShPtrType> m_SymbolMap;
  QStack<ICalculatorArray::Pointer> m_ExecutionStack;

  void createSymbolMap();

  QVector<CalculatorItemShPtrType> parseInfixEquation();
  QVector<CalculatorItemShPtrType> toRPN(QVector<CalculatorItemShPtrType> infixEquation);

  void checkForAmbiguousArrayName(QString itemStr, QString warningMsg);

  /**
   * @brief getRegularExpressionMatches
   * @return
   */
  QVector<QString> getRegularExpressionMatches();

  /**
   * @brief parseNumberArray
   * @param token
   * @param parsedInfix
   */
  void parseNumericValue(QString token, QVector<CalculatorItemShPtrType>& parsedInfix, double number);

  /**
   * @brief parseMinusSign
   * @param strItem
   * @param parsedInfix
   * @param loopIdx
   */
  void parseMinusSign(QString strItem, QVector<CalculatorItemShPtrType>& parsedInfix, int loopIdx);

  /**
   * @brief parseIndexOperator
   * @param token
   * @param parsedInfix
   * @param number
   */
  bool parseIndexOperator(QString token, QVector<CalculatorItemShPtrType>& parsedInfix);

  /**
   * @brief parseCommaOperator
   * @param token
   * @param parsedInfix
   * @return
   */
  bool parseCommaOperator(QString token, QVector<CalculatorItemShPtrType>& parsedInfix);

  /**
   * @brief parseArray
   * @param token
   * @param parsedInfix
   * @param selectedAM
   * @return
   */
  bool parseArray(QString token, QVector<CalculatorItemShPtrType>& parsedInfix, const AttributeMatrixShPtrType& selectedAM);

public:
  ArrayCalculator(const ArrayCalculator&) = delete;            // Copy Constructor Not Implemented
  ArrayCalculator(ArrayCalculator&&) = delete;                 // Move Constructor Not Implemented
  ArrayCalculator& operator=(const ArrayCalculator&) = delete; // Copy Assignment Not Implemented
  ArrayCalculator& operator=(ArrayCalculator&&) = delete;      // Move Assignment Not Implemented
};
