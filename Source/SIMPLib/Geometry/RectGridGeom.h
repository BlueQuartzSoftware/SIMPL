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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include <QtCore/QTextStream>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/Geometry/IGeometryGrid.h"

/**
 * @brief The RectGridGeom class represents a structured rectlinear grid
 */
class SIMPLib_EXPORT RectGridGeom : public IGeometryGrid
{
public:
  using Self = RectGridGeom;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for RectGridGeom
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for RectGridGeom
   */
  static QString ClassName();

  ~RectGridGeom() override;

  /**
   * @brief CreateGeometry
   * @param name
   * @return
   */
  static Pointer CreateGeometry(const QString& name);

  void setXBounds(const FloatArrayType::Pointer& xBounds);
  void setYBounds(const FloatArrayType::Pointer& yBounds);
  void setZBounds(const FloatArrayType::Pointer& zBounds);

  FloatArrayType::Pointer getXBounds() const;
  FloatArrayType::Pointer getYBounds() const;
  FloatArrayType::Pointer getZBounds() const;

  // -----------------------------------------------------------------------------
  // Inherited from IGeometry
  // -----------------------------------------------------------------------------

  /**
   * @brief initializeWithZeros
   */
  void initializeWithZeros() override;

  /**
   * @brief getNumberOfElements
   * @return
   */
  size_t getNumberOfElements() const override;

  /**
   * @brief findElementSizes
   * @return
   */
  int findElementSizes() override;

  /**
   * @brief getElementSizes
   * @return
   */
  FloatArrayType::Pointer getElementSizes() const override;

  /**
   * @brief deleteElementSizes
   */
  void deleteElementSizes() override;

  /**
   * @brief findElementsContainingVert
   * @return
   */
  int findElementsContainingVert() override;

  /**
   * @brief getElementsContainingVert
   * @return
   */
  ElementDynamicList::Pointer getElementsContainingVert() const override;

  /**
   * @brief deleteElementsContainingVert
   */
  void deleteElementsContainingVert() override;

  /**
   * @brief findElementNeighbors
   * @return
   */
  int findElementNeighbors() override;

  /**
   * @brief getElementNeighbors
   * @return
   */
  ElementDynamicList::Pointer getElementNeighbors() const override;

  /**
   * @brief deleteElementNeighbors
   */
  void deleteElementNeighbors() override;

  /**
   * @brief findElementCentroids
   * @return
   */
  int findElementCentroids() override;

  /**
   * @brief getElementCentroids
   * @return
   */
  FloatArrayType::Pointer getElementCentroids() const override;

  /**
   * @brief deleteElementCentroids
   */
  void deleteElementCentroids() override;

  /**
   * @brief getParametricCenter
   * @param pCoords
   */
  void getParametricCenter(double pCoords[3]) const override;

  /**
   * @brief getShapeFunctions
   * @param pCoords
   * @param shape
   */
  void getShapeFunctions(double pCoords[3], double* shape) const override;

  /**
   * @brief findDerivatives
   * @param field
   * @param derivatives
   */
  void findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable = nullptr) override;

  /**
   * @brief getInfoString
   * @return Returns a formatted string that contains general infomation about
   * the instance of the object.
   */
  QString getInfoString(SIMPL::InfoStringFormat format) const override;

  /**
   * @brief Returns a ToolTipGenerator for creating HTML tooltip tables
   * with values populated to match the geometry.
   * @return
   */
  ToolTipGenerator getToolTipGenerator() const override;

  /**
   * @brief writeGeometryToHDF5
   * @param parentId
   * @param writeXdmf
   * @return
   */
  int writeGeometryToHDF5(hid_t parentId, bool writeXdmf) const override;

  /**
   * @brief writeXdmf
   * @param out
   * @param dcName
   * @param hdfFileName
   * @return
   */
  int writeXdmf(QTextStream& out, QString dcName, QString hdfFileName) const override;

  /**
   * @brief readGeometryFromHDF5
   * @param parentId
   * @param preflight
   * @return
   */
  int readGeometryFromHDF5(hid_t parentId, bool preflight) override;

  /**
   * @brief deepCopy
   * @return
   */
  IGeometry::Pointer deepCopy(bool forceNoAllocate = false) const override;

  /**
   * @brief addOrReplaceAttributeMatrix
   */
  void addOrReplaceAttributeMatrix(const QString& name, AttributeMatrix::Pointer data) override;

  // -----------------------------------------------------------------------------
  // Inherited from IGeometryGrid
  // -----------------------------------------------------------------------------
  void setDimensions(const SizeVec3Type& dims) override;
  SizeVec3Type getDimensions() const override;

  size_t getXPoints() const override;
  size_t getYPoints() const override;
  size_t getZPoints() const override;

  void getPlaneCoords(size_t idx[3], float coords[3]) const override;
  void getPlaneCoords(size_t x, size_t y, size_t z, float coords[3]) const override;
  void getPlaneCoords(size_t idx, float coords[3]) const override;
  void getPlaneCoords(size_t idx[3], double coords[3]) const override;
  void getPlaneCoords(size_t x, size_t y, size_t z, double coords[3]) const override;
  void getPlaneCoords(size_t idx, double coords[3]) const override;

  void getCoords(size_t idx[3], float coords[3]) const override;
  void getCoords(size_t x, size_t y, size_t z, float coords[3]) const override;
  void getCoords(size_t idx, float coords[3]) const override;
  void getCoords(size_t idx[3], double coords[3]) const override;
  void getCoords(size_t x, size_t y, size_t z, double coords[3]) const override;
  void getCoords(size_t idx, double coords[3]) const override;

  std::optional<size_t> getIndex(float xCoord, float yCoord, float zCoord) const override;
  std::optional<size_t> getIndex(double xCoord, double yCoord, double zCoord) const override;

protected:
  RectGridGeom();

  /**
   * @brief Gathers the RectGrid geometry from an HDF5 file
   * @param parentid
   * @param volDims
   * @param spacing
   * @param origin
   * @return
   */
  virtual int gatherMetaData(hid_t parentid, size_t volDims[3], bool preflight);

  /**
   * @brief setElementsContaingVert
   * @param elementsContaingVert
   */
  void setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert) override;

  /**
   * @brief setElementNeighbors
   * @param elementNeighbors
   */
  void setElementNeighbors(ElementDynamicList::Pointer elementsNeighbors) override;

  /**
   * @brief setElementCentroids
   * @param elementCentroids
   */
  void setElementCentroids(FloatArrayType::Pointer elementCentroids) override;

  /**
   * @brief setElementSizes
   * @param elementSizes
   */
  void setElementSizes(FloatArrayType::Pointer elementSizes) override;

private:
  FloatArrayType::Pointer m_xBounds;
  FloatArrayType::Pointer m_yBounds;
  FloatArrayType::Pointer m_zBounds;
  FloatArrayType::Pointer m_VoxelSizes;
  SizeVec3Type m_Dimensions;

  friend class FindRectGridDerivativesImpl;

public:
  RectGridGeom(const RectGridGeom&) = delete;            // Copy Constructor Not Implemented
  RectGridGeom(RectGridGeom&&) = delete;                 // Move Constructor Not Implemented
  RectGridGeom& operator=(const RectGridGeom&) = delete; // Copy Assignment Not Implemented
  RectGridGeom& operator=(RectGridGeom&&) = delete;      // Move Assignment Not Implemented
};
