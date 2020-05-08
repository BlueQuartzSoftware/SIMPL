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
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

/**
 * @brief The SetOriginResolutionImageGeom class. See [Filter documentation](@ref adjustvolumeorigin) for details.
 */
class SIMPLib_EXPORT SetOriginResolutionImageGeom : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(SetOriginResolutionImageGeom SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(SetOriginResolutionImageGeom)
  PYB11_FILTER_NEW_MACRO(SetOriginResolutionImageGeom)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(bool ChangeOrigin READ getChangeOrigin WRITE setChangeOrigin)
  PYB11_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)
  PYB11_PROPERTY(bool ChangeResolution READ getChangeResolution WRITE setChangeResolution)
  PYB11_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = SetOriginResolutionImageGeom;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<SetOriginResolutionImageGeom> New();

  /**
   * @brief Returns the name of the class for SetOriginResolutionImageGeom
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for SetOriginResolutionImageGeom
   */
  static QString ClassName();

  ~SetOriginResolutionImageGeom() override;

  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  DataArrayPath getDataContainerName() const;

  Q_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)

  /**
   * @brief Setter property for ChangeOrigin
   */
  void setChangeOrigin(bool value);
  /**
   * @brief Getter property for ChangeOrigin
   * @return Value of ChangeOrigin
   */
  bool getChangeOrigin() const;

  Q_PROPERTY(bool ChangeOrigin READ getChangeOrigin WRITE setChangeOrigin)

  /**
   * @brief Setter property for Origin
   */
  void setOrigin(const FloatVec3Type& value);
  /**
   * @brief Getter property for Origin
   * @return Value of Origin
   */
  FloatVec3Type getOrigin() const;

  Q_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)

  /**
   * @brief Setter property for ChangeResolution
   */
  void setChangeResolution(bool value);
  /**
   * @brief Getter property for ChangeResolution
   * @return Value of ChangeResolution
   */
  bool getChangeResolution() const;

  Q_PROPERTY(bool ChangeResolution READ getChangeResolution WRITE setChangeResolution)

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
  SetOriginResolutionImageGeom();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief updateSurfaceMesh Updates the mesh coordinates
   */
  void updateSurfaceMesh();

public:
  SetOriginResolutionImageGeom(const SetOriginResolutionImageGeom&) = delete;            // Copy Constructor Not Implemented
  SetOriginResolutionImageGeom(SetOriginResolutionImageGeom&&) = delete;                 // Move Constructor Not Implemented
  SetOriginResolutionImageGeom& operator=(const SetOriginResolutionImageGeom&) = delete; // Copy Assignment Not Implemented
  SetOriginResolutionImageGeom& operator=(SetOriginResolutionImageGeom&&) = delete;      // Move Assignment Not Implemented

private:
  DataArrayPath m_DataContainerName = {};
  bool m_ChangeOrigin = {};
  FloatVec3Type m_Origin = {};
  bool m_ChangeResolution = {};
  FloatVec3Type m_Spacing = {};
};
