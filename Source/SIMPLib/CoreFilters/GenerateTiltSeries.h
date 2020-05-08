/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *    United States Air Force Prime Contract FA8650-10-D-5210
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>
#include <utility>

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLArray.hpp"

/**
 * @brief The GenerateTiltSeries class. See [Filter documentation](@ref GenerateTiltSeries) for details.
 */
class SIMPLib_EXPORT GenerateTiltSeries : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  // clang-format off
  PYB11_BEGIN_BINDINGS(GenerateTiltSeries SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(GenerateTiltSeries)
  PYB11_STATIC_NEW_MACRO(GenerateTiltSeries)
  PYB11_PROPERTY(int RotationAxis READ getRotationAxis WRITE setRotationAxis)
  PYB11_PROPERTY(float RotationLimits READ getRotationLimits WRITE setRotationLimits)
  PYB11_PROPERTY(float Spacing READ getSpacing WRITE setSpacing)
  PYB11_PROPERTY(DataArrayPath InputDataArrayPath READ getInputDataArrayPath WRITE setInputDataArrayPath)
  PYB11_PROPERTY(QString OutputPrefix READ getOutputPrefix WRITE setOutputPrefix)
  PYB11_END_BINDINGS()
  // clang-format on
  // End Python bindings declarations

public:
  using Self = GenerateTiltSeries;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<GenerateTiltSeries> New();

  /**
   * @brief Returns the name of the class for GenerateTiltSeries
   */
  QString getNameOfClass() const override;

  /**
   * @brief Returns the name of the class for GenerateTiltSeries
   */
  static QString ClassName();

  ~GenerateTiltSeries() override;

  static constexpr int32_t k_XAxis = 0;
  static constexpr int32_t k_YAxis = 1;
  static constexpr int32_t k_ZAxis = 2;

  /**
   * @brief Setter property for RotationAxis
   */
  void setRotationAxis(int value);
  /**
   * @brief Getter property for RotationAxis
   * @return Value of RotationAxis
   */
  int getRotationAxis() const;

  Q_PROPERTY(int RotationAxis READ getRotationAxis WRITE setRotationAxis)

  /**
   * @brief Setter property for RotationLimits
   */
  void setRotationLimits(const FloatVec3Type& value);
  /**
   * @brief Getter property for RotationLimits
   * @return Value of RotationLimits
   */
  FloatVec3Type getRotationLimits() const;

  Q_PROPERTY(FloatVec3Type RotationLimits READ getRotationLimits WRITE setRotationLimits)

  /**
   * @brief Setter property for Spacing
   */
  void setSpacing(const FloatVec3Type& value);
  /**
   * @brief Getter property for Spacing
   * @return Value of Spacing
   */
  FloatVec3Type getSpacing() const;

  Q_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)

  /**
   * @brief Setter property for InputDataArrayPath
   */
  void setInputDataArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for InputDataArrayPath
   * @return Value of InputDataArrayPath
   */
  DataArrayPath getInputDataArrayPath() const;

  Q_PROPERTY(DataArrayPath InputDataArrayPath READ getInputDataArrayPath WRITE setInputDataArrayPath)

  /**
   * @brief Setter property for OutputPrefix
   */
  void setOutputPrefix(const QString& value);
  /**
   * @brief Getter property for OutputPrefix
   * @return Value of OutputPrefix
   */
  QString getOutputPrefix() const;

  Q_PROPERTY(QString OutputPrefix READ getOutputPrefix WRITE setOutputPrefix)

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
  GenerateTiltSeries();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> generateXAxisGrid();
  std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> generateYAxisGrid();
  std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> generateZAxisGrid();

private:
  int m_RotationAxis = 0;
  FloatVec3Type m_RotationLimits = FloatVec3Type{0.0f, 180.f, 10.0f};
  FloatVec3Type m_Spacing = FloatVec3Type(1.0, 1.0, 1.0);
  DataArrayPath m_InputDataArrayPath = DataArrayPath("DataContainer", "AttributeMatrix", "FeatureIds");
  QString m_OutputPrefix = {"Rotation_"};

  static constexpr unsigned Dimension = 3;

public:
  GenerateTiltSeries(const GenerateTiltSeries&) = delete;            // Copy Constructor Not Implemented
  GenerateTiltSeries& operator=(const GenerateTiltSeries&) = delete; // Copy Assignment Not Implemented
  GenerateTiltSeries(GenerateTiltSeries&&) = delete;                 // Move Constructor Not Implemented
  GenerateTiltSeries& operator=(GenerateTiltSeries&&) = delete;      // Move Assignment Not Implemented
};
