/* ============================================================================
 * Copyright (c) 2022-2022 BlueQuartz Software, LLC
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

/**
 * @brief The PadImageGeometry class. See [Filter documentation](@ref padimagegeometry) for details.
 */
class SIMPLib_EXPORT PadImageGeometry : public AbstractFilter
{
  Q_OBJECT

  // clang-format off
  PYB11_BEGIN_BINDINGS(PadImageGeometry SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(PadImageGeometry)
  PYB11_FILTER_NEW_MACRO(PadImageGeometry)
  
  PYB11_END_BINDINGS()
  // clang-format on

public:
  using Self = PadImageGeometry;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  enum ErrorCodes
  {
    X_VALUES_NEGATIVE = -3000,
    Y_VALUES_NEGATIVE = -3001,
    Z_VALUES_NEGATIVE = -3002,
    MISSING_IMG_GEOMETRY = -3003
  };

  /**
   * @brief Setter property for XMinMax
   */
  void setXMinMax(const IntVec2Type& value);

  /**
   * @brief Getter property for XMinMax
   * @return Value of XMinMax
   */
  IntVec2Type getXMinMax() const;
  Q_PROPERTY(IntVec2Type XMinMax READ getXMinMax WRITE setXMinMax)

  /**
   * @brief Setter property for YMinMax
   */
  void setYMinMax(const IntVec2Type& value);

  /**
   * @brief Getter property for YMinMax
   * @return Value of YMinMax
   */
  IntVec2Type getYMinMax() const;
  Q_PROPERTY(IntVec2Type YMinMax READ getYMinMax WRITE setYMinMax)

  /**
   * @brief Setter property for ZMinMax
   */
  void setZMinMax(const IntVec2Type& value);

  /**
   * @brief Getter property for ZMinMax
   * @return Value of ZMinMax
   */
  IntVec2Type getZMinMax() const;
  Q_PROPERTY(IntVec2Type ZMinMax READ getZMinMax WRITE setZMinMax)

  /**
   * @brief Setter property for DefaultFillValue
   */
  void setDefaultFillValue(const int& value);

  /**
   * @brief Getter property for DefaultFillValue
   * @return Value of DefaultFillValue
   */
  int getDefaultFillValue() const;
  Q_PROPERTY(int DefaultFillValue READ getDefaultFillValue WRITE setDefaultFillValue)

  /**
   * @brief Setter property for UpdateOrigin
   */
  void setUpdateOrigin(const bool& value);

  /**
   * @brief Getter property for UpdateOrigin
   * @return Value of UpdateOrigin
   */
  bool getUpdateOrigin() const;
  Q_PROPERTY(bool UpdateOrigin READ getUpdateOrigin WRITE setUpdateOrigin)

  /**
   * @brief Getter property for OldGeometryInformation
   * @return Value of OldGeometryInformation
   */
  QString getOldGeometryInformation();
  Q_PROPERTY(QString OldGeometryInformation READ getOldGeometryInformation)

  /**
   * @brief Getter property for NewGeometryInformation
   * @return Value of NewGeometryInformation
   */
  QString getNewGeometryInformation();
  Q_PROPERTY(QString NewGeometryInformation READ getNewGeometryInformation)

  /**
   * @brief Setter property for AttributeMatrixPath
   */
  void setAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AttributeMatrixPath
   * @return Value of AttributeMatrixPath
   */
  DataArrayPath getAttributeMatrixPath() const;
  Q_PROPERTY(DataArrayPath AttributeMatrixPath READ getAttributeMatrixPath WRITE setAttributeMatrixPath)

  /**
   * @brief Returns the name of the class for PadImageGeometry
   */
  QString getNameOfClass() const override;

  /**
   * @brief Returns the name of the class for PadImageGeometry
   */
  static QString ClassName();

  ~PadImageGeometry() override;

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
  PadImageGeometry();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  IntVec2Type m_XMinMax = {0, 0};
  IntVec2Type m_YMinMax = {0, 0};
  IntVec2Type m_ZMinMax = {0, 0};
  int m_DefaultFillValue = 0;
  bool m_UpdateOrigin = {false};
  DataArrayPath m_AttributeMatrixPath = {"", "", ""};

  AttributeMatrix::Pointer m_OldAttrMatrix = AttributeMatrix::NullPointer();
  ImageGeom::Pointer m_OldGeometry = ImageGeom::NullPointer();

public:
  PadImageGeometry(const PadImageGeometry&) = delete;            // Copy Constructor Not Implemented
  PadImageGeometry& operator=(const PadImageGeometry&) = delete; // Copy Assignment Not Implemented
  PadImageGeometry(PadImageGeometry&&) = delete;                 // Move Constructor Not Implemented
  PadImageGeometry& operator=(PadImageGeometry&&) = delete;      // Move Assignment Not Implemented
};
