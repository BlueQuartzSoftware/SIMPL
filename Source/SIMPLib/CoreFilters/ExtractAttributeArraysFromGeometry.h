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
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Geometry/IGeometry.h"

using MeshIndexType = size_t;

/**
 * @brief The ExtractAttributeArraysFromGeometry class. See [Filter documentation](@ref ExtractAttributeArraysFromGeometry) for details.
 */
class SIMPLib_EXPORT ExtractAttributeArraysFromGeometry : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ExtractAttributeArraysFromGeometry SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ExtractAttributeArraysFromGeometry)
  PYB11_FILTER_NEW_MACRO(ExtractAttributeArraysFromGeometry)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(DataArrayPath SharedVertexListArrayPath0 READ getSharedVertexListArrayPath0 WRITE setSharedVertexListArrayPath0)
  PYB11_PROPERTY(DataArrayPath SharedVertexListArrayPath1 READ getSharedVertexListArrayPath1 WRITE setSharedVertexListArrayPath1)
  PYB11_PROPERTY(DataArrayPath SharedVertexListArrayPath2 READ getSharedVertexListArrayPath2 WRITE setSharedVertexListArrayPath2)
  PYB11_PROPERTY(DataArrayPath SharedVertexListArrayPath3 READ getSharedVertexListArrayPath3 WRITE setSharedVertexListArrayPath3)
  PYB11_PROPERTY(DataArrayPath SharedVertexListArrayPath4 READ getSharedVertexListArrayPath4 WRITE setSharedVertexListArrayPath4)
  PYB11_PROPERTY(DataArrayPath SharedEdgeListArrayPath READ getSharedEdgeListArrayPath WRITE setSharedEdgeListArrayPath)
  PYB11_PROPERTY(DataArrayPath SharedTriListArrayPath READ getSharedTriListArrayPath WRITE setSharedTriListArrayPath)
  PYB11_PROPERTY(DataArrayPath SharedQuadListArrayPath READ getSharedQuadListArrayPath WRITE setSharedQuadListArrayPath)
  PYB11_PROPERTY(DataArrayPath SharedTetListArrayPath READ getSharedTetListArrayPath WRITE setSharedTetListArrayPath)
  PYB11_PROPERTY(DataArrayPath XBoundsArrayPath READ getXBoundsArrayPath WRITE setXBoundsArrayPath)
  PYB11_PROPERTY(DataArrayPath YBoundsArrayPath READ getYBoundsArrayPath WRITE setYBoundsArrayPath)
  PYB11_PROPERTY(DataArrayPath ZBoundsArrayPath READ getZBoundsArrayPath WRITE setZBoundsArrayPath)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ExtractAttributeArraysFromGeometry;
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
   * @brief Returns the name of the class for ExtractAttributeArraysFromGeometry
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ExtractAttributeArraysFromGeometry
   */
  static QString ClassName();

  ~ExtractAttributeArraysFromGeometry() override;

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
   * @brief Setter property for SharedVertexListArrayPath0
   */
  void setSharedVertexListArrayPath0(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedVertexListArrayPath0
   * @return Value of SharedVertexListArrayPath0
   */
  DataArrayPath getSharedVertexListArrayPath0() const;

  Q_PROPERTY(DataArrayPath SharedVertexListArrayPath0 READ getSharedVertexListArrayPath0 WRITE setSharedVertexListArrayPath0)

  /**
   * @brief Setter property for SharedVertexListArrayPath1
   */
  void setSharedVertexListArrayPath1(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedVertexListArrayPath1
   * @return Value of SharedVertexListArrayPath1
   */
  DataArrayPath getSharedVertexListArrayPath1() const;

  Q_PROPERTY(DataArrayPath SharedVertexListArrayPath1 READ getSharedVertexListArrayPath1 WRITE setSharedVertexListArrayPath1)

  /**
   * @brief Setter property for SharedVertexListArrayPath2
   */
  void setSharedVertexListArrayPath2(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedVertexListArrayPath2
   * @return Value of SharedVertexListArrayPath2
   */
  DataArrayPath getSharedVertexListArrayPath2() const;

  Q_PROPERTY(DataArrayPath SharedVertexListArrayPath2 READ getSharedVertexListArrayPath2 WRITE setSharedVertexListArrayPath2)

  /**
   * @brief Setter property for SharedVertexListArrayPath3
   */
  void setSharedVertexListArrayPath3(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedVertexListArrayPath3
   * @return Value of SharedVertexListArrayPath3
   */
  DataArrayPath getSharedVertexListArrayPath3() const;

  Q_PROPERTY(DataArrayPath SharedVertexListArrayPath3 READ getSharedVertexListArrayPath3 WRITE setSharedVertexListArrayPath3)

  /**
   * @brief Setter property for SharedVertexListArrayPath4
   */
  void setSharedVertexListArrayPath4(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedVertexListArrayPath4
   * @return Value of SharedVertexListArrayPath4
   */
  DataArrayPath getSharedVertexListArrayPath4() const;

  Q_PROPERTY(DataArrayPath SharedVertexListArrayPath4 READ getSharedVertexListArrayPath4 WRITE setSharedVertexListArrayPath4)

  /**
   * @brief Setter property for SharedVertexListArrayPath5
   */
  void setSharedVertexListArrayPath5(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedVertexListArrayPath5
   * @return Value of SharedVertexListArrayPath5
   */
  DataArrayPath getSharedVertexListArrayPath5() const;

  Q_PROPERTY(DataArrayPath SharedVertexListArrayPath5 READ getSharedVertexListArrayPath5 WRITE setSharedVertexListArrayPath5)

  /**
   * @brief Setter property for SharedEdgeListArrayPath
   */
  void setSharedEdgeListArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedEdgeListArrayPath
   * @return Value of SharedEdgeListArrayPath
   */
  DataArrayPath getSharedEdgeListArrayPath() const;

  Q_PROPERTY(DataArrayPath SharedEdgeListArrayPath READ getSharedEdgeListArrayPath WRITE setSharedEdgeListArrayPath)

  /**
   * @brief Setter property for SharedTriListArrayPath
   */
  void setSharedTriListArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedTriListArrayPath
   * @return Value of SharedTriListArrayPath
   */
  DataArrayPath getSharedTriListArrayPath() const;

  Q_PROPERTY(DataArrayPath SharedTriListArrayPath READ getSharedTriListArrayPath WRITE setSharedTriListArrayPath)

  /**
   * @brief Setter property for SharedQuadListArrayPath
   */
  void setSharedQuadListArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedQuadListArrayPath
   * @return Value of SharedQuadListArrayPath
   */
  DataArrayPath getSharedQuadListArrayPath() const;

  Q_PROPERTY(DataArrayPath SharedQuadListArrayPath READ getSharedQuadListArrayPath WRITE setSharedQuadListArrayPath)

  /**
   * @brief Setter property for SharedTetListArrayPath
   */
  void setSharedTetListArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedTetListArrayPath
   * @return Value of SharedTetListArrayPath
   */
  DataArrayPath getSharedTetListArrayPath() const;

  Q_PROPERTY(DataArrayPath SharedTetListArrayPath READ getSharedTetListArrayPath WRITE setSharedTetListArrayPath)

  /**
   * @brief Setter property for SharedHexListArrayPath
   */
  void setSharedHexListArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedHexListArrayPath
   * @return Value of SharedHexListArrayPath
   */
  DataArrayPath getSharedHexListArrayPath() const;

  Q_PROPERTY(DataArrayPath SharedHexListArrayPath READ getSharedHexListArrayPath WRITE setSharedHexListArrayPath)

  /**
   * @brief Setter property for XBoundsArrayPath
   */
  void setXBoundsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for XBoundsArrayPath
   * @return Value of XBoundsArrayPath
   */
  DataArrayPath getXBoundsArrayPath() const;

  Q_PROPERTY(DataArrayPath XBoundsArrayPath READ getXBoundsArrayPath WRITE setXBoundsArrayPath)

  /**
   * @brief Setter property for YBoundsArrayPath
   */
  void setYBoundsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for YBoundsArrayPath
   * @return Value of YBoundsArrayPath
   */
  DataArrayPath getYBoundsArrayPath() const;

  Q_PROPERTY(DataArrayPath YBoundsArrayPath READ getYBoundsArrayPath WRITE setYBoundsArrayPath)

  /**
   * @brief Setter property for ZBoundsArrayPath
   */
  void setZBoundsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for ZBoundsArrayPath
   * @return Value of ZBoundsArrayPath
   */
  DataArrayPath getZBoundsArrayPath() const;

  Q_PROPERTY(DataArrayPath ZBoundsArrayPath READ getZBoundsArrayPath WRITE setZBoundsArrayPath)

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
  ExtractAttributeArraysFromGeometry();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<float>> m_XBoundsPtr;
  float* m_XBounds = nullptr;
  std::weak_ptr<DataArray<float>> m_YBoundsPtr;
  float* m_YBounds = nullptr;
  std::weak_ptr<DataArray<float>> m_ZBoundsPtr;
  float* m_ZBounds = nullptr;
  std::weak_ptr<DataArray<float>> m_VertsPtr;
  float* m_Verts = nullptr;
  std::weak_ptr<DataArray<MeshIndexType>> m_EdgesPtr;
  MeshIndexType* m_Edges = nullptr;
  std::weak_ptr<DataArray<MeshIndexType>> m_TrisPtr;
  MeshIndexType* m_Tris = nullptr;
  std::weak_ptr<DataArray<MeshIndexType>> m_QuadsPtr;
  MeshIndexType* m_Quads = nullptr;
  std::weak_ptr<DataArray<MeshIndexType>> m_TetsPtr;
  MeshIndexType* m_Tets = nullptr;

  DataArrayPath m_DataContainerName = {};
  DataArrayPath m_SharedVertexListArrayPath0 = {};
  DataArrayPath m_SharedVertexListArrayPath1 = {};
  DataArrayPath m_SharedVertexListArrayPath2 = {};
  DataArrayPath m_SharedVertexListArrayPath3 = {};
  DataArrayPath m_SharedVertexListArrayPath4 = {};
  DataArrayPath m_SharedVertexListArrayPath5 = {};
  DataArrayPath m_SharedEdgeListArrayPath = {};
  DataArrayPath m_SharedTriListArrayPath = {};
  DataArrayPath m_SharedQuadListArrayPath = {};
  DataArrayPath m_SharedTetListArrayPath = {};
  DataArrayPath m_SharedHexListArrayPath = {};
  DataArrayPath m_XBoundsArrayPath = {};
  DataArrayPath m_YBoundsArrayPath = {};
  DataArrayPath m_ZBoundsArrayPath = {};

public:
  ExtractAttributeArraysFromGeometry(const ExtractAttributeArraysFromGeometry&) = delete;            // Copy Constructor Not Implemented
  ExtractAttributeArraysFromGeometry(ExtractAttributeArraysFromGeometry&&) = delete;                 // Move Constructor Not Implemented
  ExtractAttributeArraysFromGeometry& operator=(const ExtractAttributeArraysFromGeometry&) = delete; // Copy Assignment Not Implemented
  ExtractAttributeArraysFromGeometry& operator=(ExtractAttributeArraysFromGeometry&&) = delete;      // Move Assignment Not Implemented
};
