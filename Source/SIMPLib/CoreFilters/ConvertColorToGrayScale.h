#pragma once
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

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"

/**
 * @brief The ConvertColorToGrayScale class. See [Filter documentation](@ref ConvertColorToGrayScale) for details.
 */
class SIMPLib_EXPORT ConvertColorToGrayScale : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ConvertColorToGrayScale SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ConvertColorToGrayScale)
  PYB11_FILTER_NEW_MACRO(ConvertColorToGrayScale)
  PYB11_PROPERTY(int ConversionAlgorithm READ getConversionAlgorithm WRITE setConversionAlgorithm)
  PYB11_PROPERTY(FloatVec3Type ColorWeights READ getColorWeights WRITE setColorWeights)
  PYB11_PROPERTY(int ColorChannel READ getColorChannel WRITE setColorChannel)
  PYB11_PROPERTY(QVector<DataArrayPath> InputDataArrayVector READ getInputDataArrayVector WRITE setInputDataArrayVector)
  PYB11_PROPERTY(bool CreateNewAttributeMatrix READ getCreateNewAttributeMatrix WRITE setCreateNewAttributeMatrix)
  PYB11_PROPERTY(QString OutputAttributeMatrixName READ getOutputAttributeMatrixName WRITE setOutputAttributeMatrixName)
  PYB11_PROPERTY(QString OutputArrayPrefix READ getOutputArrayPrefix WRITE setOutputArrayPrefix)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ConvertColorToGrayScale;
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
   * @brief Returns the name of the class for ConvertColorToGrayScale
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ConvertColorToGrayScale
   */
  static QString ClassName();

  ~ConvertColorToGrayScale() override;

  using EnumType = unsigned int;
  enum class ConversionType : EnumType
  {
    Luminosity = 0,
    Average = 1,
    Lightness = 2,
    SingleChannel = 3
  };

  /**
   * @brief Setter property for ConversionAlgorithm
   */
  void setConversionAlgorithm(int value);
  /**
   * @brief Getter property for ConversionAlgorithm
   * @return Value of ConversionAlgorithm
   */
  int getConversionAlgorithm() const;

  Q_PROPERTY(int ConversionAlgorithm READ getConversionAlgorithm WRITE setConversionAlgorithm)

  /**
   * @brief Setter property for ColorWeights
   */
  void setColorWeights(const FloatVec3Type& value);
  /**
   * @brief Getter property for ColorWeights
   * @return Value of ColorWeights
   */
  FloatVec3Type getColorWeights() const;

  Q_PROPERTY(FloatVec3Type ColorWeights READ getColorWeights WRITE setColorWeights)

  /**
   * @brief Setter property for ColorChannel
   */
  void setColorChannel(int value);
  /**
   * @brief Getter property for ColorChannel
   * @return Value of ColorChannel
   */
  int getColorChannel() const;

  Q_PROPERTY(int ColorChannel READ getColorChannel WRITE setColorChannel)

  /**
   * @brief Setter property for InputDataArrayVector
   */
  void setInputDataArrayVector(const QVector<DataArrayPath>& value);
  /**
   * @brief Getter property for InputDataArrayVector
   * @return Value of InputDataArrayVector
   */
  QVector<DataArrayPath> getInputDataArrayVector() const;

  Q_PROPERTY(QVector<DataArrayPath> InputDataArrayVector READ getInputDataArrayVector WRITE setInputDataArrayVector)

  /**
   * @brief Setter property for CreateNewAttributeMatrix
   */
  void setCreateNewAttributeMatrix(bool value);
  /**
   * @brief Getter property for CreateNewAttributeMatrix
   * @return Value of CreateNewAttributeMatrix
   */
  bool getCreateNewAttributeMatrix() const;

  Q_PROPERTY(bool CreateNewAttributeMatrix READ getCreateNewAttributeMatrix WRITE setCreateNewAttributeMatrix)

  /**
   * @brief Setter property for OutputAttributeMatrixName
   */
  void setOutputAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for OutputAttributeMatrixName
   * @return Value of OutputAttributeMatrixName
   */
  QString getOutputAttributeMatrixName() const;

  Q_PROPERTY(QString OutputAttributeMatrixName READ getOutputAttributeMatrixName WRITE setOutputAttributeMatrixName)

  /**
   * @brief Setter property for OutputArrayPrefix
   */
  void setOutputArrayPrefix(const QString& value);
  /**
   * @brief Getter property for OutputArrayPrefix
   * @return Value of OutputArrayPrefix
   */
  QString getOutputArrayPrefix() const;

  Q_PROPERTY(QString OutputArrayPrefix READ getOutputArrayPrefix WRITE setOutputArrayPrefix)

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

protected:
  ConvertColorToGrayScale();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  int m_ConversionAlgorithm = {};
  FloatVec3Type m_ColorWeights = {};
  int m_ColorChannel = {};
  QVector<DataArrayPath> m_InputDataArrayVector = {};
  bool m_CreateNewAttributeMatrix = {};
  QString m_OutputAttributeMatrixName = {};
  QString m_OutputArrayPrefix = {};

  QVector<DataArrayPath> m_OutputArrayPaths;

public:
  ConvertColorToGrayScale(const ConvertColorToGrayScale&) = delete;            // Copy Constructor Not Implemented
  ConvertColorToGrayScale(ConvertColorToGrayScale&&) = delete;                 // Move Constructor Not Implemented
  ConvertColorToGrayScale& operator=(const ConvertColorToGrayScale&) = delete; // Copy Assignment Not Implemented
  ConvertColorToGrayScale& operator=(ConvertColorToGrayScale&&) = delete;      // Move Assignment Not Implemented
};
