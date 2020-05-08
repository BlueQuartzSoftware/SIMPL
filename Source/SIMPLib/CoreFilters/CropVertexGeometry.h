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

/**
 * @brief The CropVertexGeometry class. See [Filter documentation](@ref cropvertexgeometry) for details.
 */
class SIMPLib_EXPORT CropVertexGeometry : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(CropVertexGeometry SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(CropVertexGeometry)
  PYB11_FILTER_NEW_MACRO(CropVertexGeometry)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(DataArrayPath CroppedDataContainerName READ getCroppedDataContainerName WRITE setCroppedDataContainerName)
  PYB11_PROPERTY(float XMin READ getXMin WRITE setXMin)
  PYB11_PROPERTY(float YMin READ getYMin WRITE setYMin)
  PYB11_PROPERTY(float ZMin READ getZMin WRITE setZMin)
  PYB11_PROPERTY(float XMax READ getXMax WRITE setXMax)
  PYB11_PROPERTY(float YMax READ getYMax WRITE setYMax)
  PYB11_PROPERTY(float ZMax READ getZMax WRITE setZMax)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = CropVertexGeometry;
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
   * @brief Returns the name of the class for CropVertexGeometry
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CropVertexGeometry
   */
  static QString ClassName();

  ~CropVertexGeometry() override;

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
   * @brief Setter property for CroppedDataContainerName
   */
  void setCroppedDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for CroppedDataContainerName
   * @return Value of CroppedDataContainerName
   */
  DataArrayPath getCroppedDataContainerName() const;

  Q_PROPERTY(DataArrayPath CroppedDataContainerName READ getCroppedDataContainerName WRITE setCroppedDataContainerName)

  /**
   * @brief Setter property for XMin
   */
  void setXMin(float value);
  /**
   * @brief Getter property for XMin
   * @return Value of XMin
   */
  float getXMin() const;

  Q_PROPERTY(float XMin READ getXMin WRITE setXMin)

  /**
   * @brief Setter property for YMin
   */
  void setYMin(float value);
  /**
   * @brief Getter property for YMin
   * @return Value of YMin
   */
  float getYMin() const;

  Q_PROPERTY(float YMin READ getYMin WRITE setYMin)

  /**
   * @brief Setter property for ZMin
   */
  void setZMin(float value);
  /**
   * @brief Getter property for ZMin
   * @return Value of ZMin
   */
  float getZMin() const;

  Q_PROPERTY(float ZMin READ getZMin WRITE setZMin)

  /**
   * @brief Setter property for XMax
   */
  void setXMax(float value);
  /**
   * @brief Getter property for XMax
   * @return Value of XMax
   */
  float getXMax() const;

  Q_PROPERTY(float XMax READ getXMax WRITE setXMax)

  /**
   * @brief Setter property for YMax
   */
  void setYMax(float value);
  /**
   * @brief Getter property for YMax
   * @return Value of YMax
   */
  float getYMax() const;

  Q_PROPERTY(float YMax READ getYMax WRITE setYMax)

  /**
   * @brief Setter property for ZMax
   */
  void setZMax(float value);
  /**
   * @brief Getter property for ZMax
   * @return Value of ZMax
   */
  float getZMax() const;

  Q_PROPERTY(float ZMax READ getZMax WRITE setZMax)

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
  CropVertexGeometry();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DataArrayPath m_DataContainerName = {};
  DataArrayPath m_CroppedDataContainerName = {};
  float m_XMin = {};
  float m_YMin = {};
  float m_ZMin = {};
  float m_XMax = {};
  float m_YMax = {};
  float m_ZMax = {};

  QList<QString> m_AttrMatList;

public:
  CropVertexGeometry(const CropVertexGeometry&) = delete;            // Copy Constructor Not Implemented
  CropVertexGeometry(CropVertexGeometry&&) = delete;                 // Move Constructor Not Implemented
  CropVertexGeometry& operator=(const CropVertexGeometry&) = delete; // Copy Assignment Not Implemented
  CropVertexGeometry& operator=(CropVertexGeometry&&) = delete;      // Move Assignment Not Implemented
};
