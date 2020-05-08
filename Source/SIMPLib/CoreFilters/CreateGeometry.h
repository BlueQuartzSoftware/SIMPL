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
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

/**
 * @brief The CreateGeometry class. See [Filter documentation](@ref creategeometry) for details.
 */
class SIMPLib_EXPORT CreateGeometry : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(CreateGeometry SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(CreateGeometry)
  PYB11_FILTER_NEW_MACRO(CreateGeometry)
  PYB11_PROPERTY(int GeometryType READ getGeometryType WRITE setGeometryType)
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
  PYB11_PROPERTY(IntVec3Type Dimensions READ getDimensions WRITE setDimensions)
  PYB11_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)
  PYB11_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)
  PYB11_PROPERTY(QString ImageCellAttributeMatrixName READ getImageCellAttributeMatrixName WRITE setImageCellAttributeMatrixName)
  PYB11_PROPERTY(QString RectGridCellAttributeMatrixName READ getRectGridCellAttributeMatrixName WRITE setRectGridCellAttributeMatrixName)
  PYB11_PROPERTY(QString VertexAttributeMatrixName0 READ getVertexAttributeMatrixName0 WRITE setVertexAttributeMatrixName0)
  PYB11_PROPERTY(QString VertexAttributeMatrixName1 READ getVertexAttributeMatrixName1 WRITE setVertexAttributeMatrixName1)
  PYB11_PROPERTY(QString VertexAttributeMatrixName2 READ getVertexAttributeMatrixName2 WRITE setVertexAttributeMatrixName2)
  PYB11_PROPERTY(QString VertexAttributeMatrixName3 READ getVertexAttributeMatrixName3 WRITE setVertexAttributeMatrixName3)
  PYB11_PROPERTY(QString VertexAttributeMatrixName4 READ getVertexAttributeMatrixName4 WRITE setVertexAttributeMatrixName4)
  PYB11_PROPERTY(QString EdgeAttributeMatrixName READ getEdgeAttributeMatrixName WRITE setEdgeAttributeMatrixName)
  PYB11_PROPERTY(QString FaceAttributeMatrixName0 READ getFaceAttributeMatrixName0 WRITE setFaceAttributeMatrixName0)
  PYB11_PROPERTY(QString FaceAttributeMatrixName1 READ getFaceAttributeMatrixName1 WRITE setFaceAttributeMatrixName1)
  PYB11_PROPERTY(QString TetCellAttributeMatrixName READ getTetCellAttributeMatrixName WRITE setTetCellAttributeMatrixName)
  PYB11_PROPERTY(bool TreatWarningsAsErrors READ getTreatWarningsAsErrors WRITE setTreatWarningsAsErrors)
  PYB11_PROPERTY(bool ArrayHandling READ getArrayHandling WRITE setArrayHandling)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = CreateGeometry;
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
   * @brief Returns the name of the class for CreateGeometry
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CreateGeometry
   */
  static QString ClassName();

  ~CreateGeometry() override;

  static const int k_CopyArrays = 0;
  static const int k_MoveArrays = 1;

  /**
   * @brief Setter property for GeometryType
   */
  void setGeometryType(int value);
  /**
   * @brief Getter property for GeometryType
   * @return Value of GeometryType
   */
  int getGeometryType() const;

  Q_PROPERTY(int GeometryType READ getGeometryType WRITE setGeometryType)

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
   * @brief Setter property for Dimensions
   */
  void setDimensions(const IntVec3Type& value);
  /**
   * @brief Getter property for Dimensions
   * @return Value of Dimensions
   */
  IntVec3Type getDimensions() const;

  Q_PROPERTY(IntVec3Type Dimensions READ getDimensions WRITE setDimensions)

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
   * @brief Setter property for ImageCellAttributeMatrixName
   */
  void setImageCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for ImageCellAttributeMatrixName
   * @return Value of ImageCellAttributeMatrixName
   */
  QString getImageCellAttributeMatrixName() const;

  Q_PROPERTY(QString ImageCellAttributeMatrixName READ getImageCellAttributeMatrixName WRITE setImageCellAttributeMatrixName)

  /**
   * @brief Setter property for RectGridCellAttributeMatrixName
   */
  void setRectGridCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for RectGridCellAttributeMatrixName
   * @return Value of RectGridCellAttributeMatrixName
   */
  QString getRectGridCellAttributeMatrixName() const;

  Q_PROPERTY(QString RectGridCellAttributeMatrixName READ getRectGridCellAttributeMatrixName WRITE setRectGridCellAttributeMatrixName)

  /**
   * @brief Setter property for VertexAttributeMatrixName0
   */
  void setVertexAttributeMatrixName0(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName0
   * @return Value of VertexAttributeMatrixName0
   */
  QString getVertexAttributeMatrixName0() const;

  Q_PROPERTY(QString VertexAttributeMatrixName0 READ getVertexAttributeMatrixName0 WRITE setVertexAttributeMatrixName0)

  /**
   * @brief Setter property for VertexAttributeMatrixName1
   */
  void setVertexAttributeMatrixName1(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName1
   * @return Value of VertexAttributeMatrixName1
   */
  QString getVertexAttributeMatrixName1() const;

  Q_PROPERTY(QString VertexAttributeMatrixName1 READ getVertexAttributeMatrixName1 WRITE setVertexAttributeMatrixName1)

  /**
   * @brief Setter property for VertexAttributeMatrixName2
   */
  void setVertexAttributeMatrixName2(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName2
   * @return Value of VertexAttributeMatrixName2
   */
  QString getVertexAttributeMatrixName2() const;

  Q_PROPERTY(QString VertexAttributeMatrixName2 READ getVertexAttributeMatrixName2 WRITE setVertexAttributeMatrixName2)

  /**
   * @brief Setter property for VertexAttributeMatrixName3
   */
  void setVertexAttributeMatrixName3(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName3
   * @return Value of VertexAttributeMatrixName3
   */
  QString getVertexAttributeMatrixName3() const;

  Q_PROPERTY(QString VertexAttributeMatrixName3 READ getVertexAttributeMatrixName3 WRITE setVertexAttributeMatrixName3)

  /**
   * @brief Setter property for VertexAttributeMatrixName4
   */
  void setVertexAttributeMatrixName4(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName4
   * @return Value of VertexAttributeMatrixName4
   */
  QString getVertexAttributeMatrixName4() const;

  Q_PROPERTY(QString VertexAttributeMatrixName4 READ getVertexAttributeMatrixName4 WRITE setVertexAttributeMatrixName4)

  /**
   * @brief Setter property for VertexAttributeMatrixName5
   */
  void setVertexAttributeMatrixName5(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName5
   * @return Value of VertexAttributeMatrixName5
   */
  QString getVertexAttributeMatrixName5() const;

  Q_PROPERTY(QString VertexAttributeMatrixName5 READ getVertexAttributeMatrixName5 WRITE setVertexAttributeMatrixName5)

  /**
   * @brief Setter property for EdgeAttributeMatrixName
   */
  void setEdgeAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for EdgeAttributeMatrixName
   * @return Value of EdgeAttributeMatrixName
   */
  QString getEdgeAttributeMatrixName() const;

  Q_PROPERTY(QString EdgeAttributeMatrixName READ getEdgeAttributeMatrixName WRITE setEdgeAttributeMatrixName)

  /**
   * @brief Setter property for FaceAttributeMatrixName0
   */
  void setFaceAttributeMatrixName0(const QString& value);
  /**
   * @brief Getter property for FaceAttributeMatrixName0
   * @return Value of FaceAttributeMatrixName0
   */
  QString getFaceAttributeMatrixName0() const;

  Q_PROPERTY(QString FaceAttributeMatrixName0 READ getFaceAttributeMatrixName0 WRITE setFaceAttributeMatrixName0)

  /**
   * @brief Setter property for FaceAttributeMatrixName1
   */
  void setFaceAttributeMatrixName1(const QString& value);
  /**
   * @brief Getter property for FaceAttributeMatrixName1
   * @return Value of FaceAttributeMatrixName1
   */
  QString getFaceAttributeMatrixName1() const;

  Q_PROPERTY(QString FaceAttributeMatrixName1 READ getFaceAttributeMatrixName1 WRITE setFaceAttributeMatrixName1)

  /**
   * @brief Setter property for TetCellAttributeMatrixName
   */
  void setTetCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for TetCellAttributeMatrixName
   * @return Value of TetCellAttributeMatrixName
   */
  QString getTetCellAttributeMatrixName() const;

  Q_PROPERTY(QString TetCellAttributeMatrixName READ getTetCellAttributeMatrixName WRITE setTetCellAttributeMatrixName)

  /**
   * @brief Setter property for HexCellAttributeMatrixName
   */
  void setHexCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for HexCellAttributeMatrixName
   * @return Value of HexCellAttributeMatrixName
   */
  QString getHexCellAttributeMatrixName() const;

  Q_PROPERTY(QString HexCellAttributeMatrixName READ getHexCellAttributeMatrixName WRITE setHexCellAttributeMatrixName)

  /**
   * @brief Setter property for TreatWarningsAsErrors
   */
  void setTreatWarningsAsErrors(bool value);
  /**
   * @brief Getter property for TreatWarningsAsErrors
   * @return Value of TreatWarningsAsErrors
   */
  bool getTreatWarningsAsErrors() const;

  Q_PROPERTY(bool TreatWarningsAsErrors READ getTreatWarningsAsErrors WRITE setTreatWarningsAsErrors)

  /**
   * @brief Setter property for ArrayHandling
   */
  void setArrayHandling(bool value);
  /**
   * @brief Getter property for ArrayHandling
   * @return Value of ArrayHandling
   */
  bool getArrayHandling() const;

  Q_PROPERTY(bool ArrayHandling READ getArrayHandling WRITE setArrayHandling)

  QString getBoxDimensions();
  Q_PROPERTY(QString BoxDimensions READ getBoxDimensions)

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
  CreateGeometry();
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
  std::weak_ptr<DataArray<size_t>> m_EdgesPtr;
  size_t* m_Edges = nullptr;
  std::weak_ptr<DataArray<size_t>> m_TrisPtr;
  size_t* m_Tris = nullptr;
  std::weak_ptr<DataArray<size_t>> m_QuadsPtr;
  size_t* m_Quads = nullptr;
  std::weak_ptr<DataArray<size_t>> m_TetsPtr;
  size_t* m_Tets = nullptr;
  std::weak_ptr<DataArray<size_t>> m_HexesPtr;
  size_t* m_Hexes = nullptr;

  int m_GeometryType = {};
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
  IntVec3Type m_Dimensions = {};
  FloatVec3Type m_Origin = {};
  FloatVec3Type m_Spacing = {};
  QString m_ImageCellAttributeMatrixName = {};
  QString m_RectGridCellAttributeMatrixName = {};
  QString m_VertexAttributeMatrixName0 = {};
  QString m_VertexAttributeMatrixName1 = {};
  QString m_VertexAttributeMatrixName2 = {};
  QString m_VertexAttributeMatrixName3 = {};
  QString m_VertexAttributeMatrixName4 = {};
  QString m_VertexAttributeMatrixName5 = {};
  QString m_EdgeAttributeMatrixName = {};
  QString m_FaceAttributeMatrixName0 = {};
  QString m_FaceAttributeMatrixName1 = {};
  QString m_TetCellAttributeMatrixName = {};
  QString m_HexCellAttributeMatrixName = {};
  bool m_TreatWarningsAsErrors = {};
  bool m_ArrayHandling = {};

  size_t m_NumVerts;

public:
  CreateGeometry(const CreateGeometry&) = delete;            // Copy Constructor Not Implemented
  CreateGeometry(CreateGeometry&&) = delete;                 // Move Constructor Not Implemented
  CreateGeometry& operator=(const CreateGeometry&) = delete; // Copy Assignment Not Implemented
  CreateGeometry& operator=(CreateGeometry&&) = delete;      // Move Assignment Not Implemented
};
