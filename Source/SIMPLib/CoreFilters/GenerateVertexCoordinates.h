/* ============================================================================
 * Copyright (c) 2009-2018 BlueQuartz Software, LLC
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
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

/**
 * @brief The GenerateVertexCoordinates class. See [Filter documentation](@ref convertdatacontainergeometry) for details.
 */
class SIMPLib_EXPORT GenerateVertexCoordinates : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(GenerateVertexCoordinates SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(GenerateVertexCoordinates)
  PYB11_FILTER_NEW_MACRO(GenerateVertexCoordinates)
  PYB11_PROPERTY(DataArrayPath SelectedDataContainerName READ getSelectedDataContainerName WRITE setSelectedDataContainerName)
  PYB11_PROPERTY(DataArrayPath CoordinateArrayPath READ getCoordinateArrayPath WRITE setCoordinateArrayPath)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = GenerateVertexCoordinates;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;

  ~GenerateVertexCoordinates() override;

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
   * @brief Returns the name of the class for GenerateVertexCoordinates
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GenerateVertexCoordinates
   */
  static QString ClassName();

  /**
   * @brief Setter property for SelectedDataContainerName
   */
  void setSelectedDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for SelectedDataContainerName
   * @return Value of SelectedDataContainerName
   */
  DataArrayPath getSelectedDataContainerName() const;

  Q_PROPERTY(DataArrayPath SelectedDataContainerName READ getSelectedDataContainerName WRITE setSelectedDataContainerName)

  /**
   * @brief Setter property for VertexDataContainerName
   */
  void setCoordinateArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for VertexDataContainerName
   * @return Value of VertexDataContainerName
   */
  DataArrayPath getCoordinateArrayPath() const;

  Q_PROPERTY(DataArrayPath CoordinateArrayPath READ getCoordinateArrayPath WRITE setCoordinateArrayPath)

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
  GenerateVertexCoordinates();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DataArrayPath m_SelectedDataContainerName = {};
  DataArrayPath m_CoordinateArrayPath = {"", "", ""};
  FloatArrayType::Pointer m_CoordinatesPtr;

public:
  GenerateVertexCoordinates(const GenerateVertexCoordinates&) = delete;            // Copy Constructor Not Implemented
  GenerateVertexCoordinates& operator=(const GenerateVertexCoordinates&) = delete; // Copy Assignment Not Implemented
  GenerateVertexCoordinates(GenerateVertexCoordinates&&) = delete;                 // Move Constructor Not Implemented
  GenerateVertexCoordinates& operator=(GenerateVertexCoordinates&&) = delete;      // Move Assignment Not Implemented
};
