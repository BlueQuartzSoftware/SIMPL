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

#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/FilterParameters/RangeFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

/**
 * @brief The InitializeData class. See [Filter documentation](@ref initializedata) for details.
 */
class SIMPLib_EXPORT InitializeData : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(InitializeData SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(InitializeData)
  PYB11_FILTER_NEW_MACRO(InitializeData)
  PYB11_PROPERTY(QVector<DataArrayPath> CellAttributeMatrixPaths READ getCellAttributeMatrixPaths WRITE setCellAttributeMatrixPaths)
  PYB11_PROPERTY(int XMin READ getXMin WRITE setXMin)
  PYB11_PROPERTY(int YMin READ getYMin WRITE setYMin)
  PYB11_PROPERTY(int ZMin READ getZMin WRITE setZMin)
  PYB11_PROPERTY(int XMax READ getXMax WRITE setXMax)
  PYB11_PROPERTY(int YMax READ getYMax WRITE setYMax)
  PYB11_PROPERTY(int ZMax READ getZMax WRITE setZMax)
  PYB11_PROPERTY(int InitType READ getInitType WRITE setInitType)
  PYB11_PROPERTY(bool Random READ getRandom WRITE setRandom)
  PYB11_PROPERTY(double InitValue READ getInitValue WRITE setInitValue)
  PYB11_PROPERTY(FPRangePair InitRange READ getInitRange WRITE setInitRange)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = InitializeData;
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
   * @brief Returns the name of the class for InitializeData
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for InitializeData
   */
  static QString ClassName();

  ~InitializeData() override;

  /**
   * @brief Setter property for CellAttributeMatrixPaths
   */
  void setCellAttributeMatrixPaths(const QVector<DataArrayPath>& value);
  /**
   * @brief Getter property for CellAttributeMatrixPaths
   * @return Value of CellAttributeMatrixPaths
   */
  QVector<DataArrayPath> getCellAttributeMatrixPaths() const;

  Q_PROPERTY(QVector<DataArrayPath> CellAttributeMatrixPaths READ getCellAttributeMatrixPaths WRITE setCellAttributeMatrixPaths)

  /**
   * @brief Setter property for XMin
   */
  void setXMin(int value);
  /**
   * @brief Getter property for XMin
   * @return Value of XMin
   */
  int getXMin() const;

  Q_PROPERTY(int XMin READ getXMin WRITE setXMin)

  /**
   * @brief Setter property for YMin
   */
  void setYMin(int value);
  /**
   * @brief Getter property for YMin
   * @return Value of YMin
   */
  int getYMin() const;

  Q_PROPERTY(int YMin READ getYMin WRITE setYMin)

  /**
   * @brief Setter property for ZMin
   */
  void setZMin(int value);
  /**
   * @brief Getter property for ZMin
   * @return Value of ZMin
   */
  int getZMin() const;

  Q_PROPERTY(int ZMin READ getZMin WRITE setZMin)

  /**
   * @brief Setter property for XMax
   */
  void setXMax(int value);
  /**
   * @brief Getter property for XMax
   * @return Value of XMax
   */
  int getXMax() const;

  Q_PROPERTY(int XMax READ getXMax WRITE setXMax)

  /**
   * @brief Setter property for YMax
   */
  void setYMax(int value);
  /**
   * @brief Getter property for YMax
   * @return Value of YMax
   */
  int getYMax() const;

  Q_PROPERTY(int YMax READ getYMax WRITE setYMax)

  /**
   * @brief Setter property for ZMax
   */
  void setZMax(int value);
  /**
   * @brief Getter property for ZMax
   * @return Value of ZMax
   */
  int getZMax() const;

  Q_PROPERTY(int ZMax READ getZMax WRITE setZMax)

  /**
   * @brief Setter property for InitType
   */
  void setInitType(int value);
  /**
   * @brief Getter property for InitType
   * @return Value of InitType
   */
  int getInitType() const;

  Q_PROPERTY(int InitType READ getInitType WRITE setInitType)

  /**
   * @brief Setter property for Random
   */
  void setRandom(bool value);
  /**
   * @brief Getter property for Random
   * @return Value of Random
   */
  bool getRandom() const;

  Q_PROPERTY(bool Random READ getRandom WRITE setRandom)

  /**
   * @brief Setter property for InitValue
   */
  void setInitValue(double value);
  /**
   * @brief Getter property for InitValue
   * @return Value of InitValue
   */
  double getInitValue() const;

  Q_PROPERTY(double InitValue READ getInitValue WRITE setInitValue)

  /**
   * @brief Setter property for InitRange
   */
  void setInitRange(const FPRangePair& value);
  /**
   * @brief Getter property for InitRange
   * @return Value of InitRange
   */
  FPRangePair getInitRange() const;

  Q_PROPERTY(FPRangePair InitRange READ getInitRange WRITE setInitRange)

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
  InitializeData();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  QVector<DataArrayPath> m_CellAttributeMatrixPaths = {};
  int m_XMin = {};
  int m_YMin = {};
  int m_ZMin = {};
  int m_XMax = {};
  int m_YMax = {};
  int m_ZMax = {};
  int m_InitType = {};
  bool m_Random = {};
  double m_InitValue = {};
  FPRangePair m_InitRange = {};

  enum InitChoices
  {
    Manual,
    Random,
    RandomWithRange
  };

  /**
   * @brief initializeArrayWithInts Initializes the array p with integers, either from the
   * manual value entered in the filter, or with a random number.  This function does not
   * check that the template type actually is an integer, so it will most likely cause
   * unexpected results when passing anything other than an integer as a template parameter.
   * @param p The array that will be initialized
   * @param dims The dimensions of the array p
   */
  template <typename T>
  void initializeArrayWithInts(IDataArrayShPtrType p, int64_t dims[3]);

  /**
   * @brief initializeArrayWithReals Initializes the array p with real numbers, either from the
   * manual value entered in the filter, or with a random number.  This function does not
   * check that the template type actually is a non-integer, so it will most likely cause
   * unexpected results when passing anything other than a float or double as a template
   * parameter.
   * @param p The array that will be initialized
   * @param dims The dimensions of the array p
   */
  template <typename T>
  void initializeArrayWithReals(IDataArrayShPtrType p, int64_t dims[3]);

  /**
   * @brief checkInitialization Checks that the chosen initialization value/range is inside
   * the bounds of the array type
   */
  template <typename T>
  void checkInitialization(IDataArrayShPtrType p);

public:
  InitializeData(const InitializeData&) = delete;            // Copy Constructor Not Implemented
  InitializeData(InitializeData&&) = delete;                 // Move Constructor Not Implemented
  InitializeData& operator=(const InitializeData&) = delete; // Copy Assignment Not Implemented
  InitializeData& operator=(InitializeData&&) = delete;      // Move Assignment Not Implemented
};
