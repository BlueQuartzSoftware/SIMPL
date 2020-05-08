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

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Geometry/IGeometry.h"
#include "SIMPLib/Geometry/GeometryHelpers.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

/**
 * @brief The VertexGeom class represents a point cloud
 */
class SIMPLib_EXPORT VertexGeom : public IGeometry
{
  // clang-format off
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(VertexGeom SUPERCLASS IGeometry)
  PYB11_SHARED_POINTERS(VertexGeom)
  PYB11_STATIC_NEW_MACRO(VertexGeom)
  PYB11_METHOD(void setVertices SharedVertexList::Pointer,vertices)
  PYB11_METHOD(SharedVertexList::Pointer getVertices)
  PYB11_METHOD(void setCoords size_t,vertId float,coords[3])
  PYB11_METHOD(void getCoords size_t,vertId float,coords[3])
  PYB11_METHOD(size_t getNumberOfVertices)
  PYB11_METHOD(size_t getNumberOfElements)
  PYB11_END_BINDINGS()
  // End Python bindings declarations
  // clang-format on

public:
  using Self = VertexGeom;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for VertexGeom
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for VertexGeom
   */
  static QString ClassName();

  ~VertexGeom() override;

  /**
   * @brief CreateGeometry
   * @param numVertices
   * @param name
   * @return
   */
  static Pointer CreateGeometry(size_t numVertices, const QString& name, bool allocate = true);

  /**
   * @brief CreateGeometry
   * @param vertices
   * @param name
   * @return
   */
  static Pointer CreateGeometry(const SharedVertexList::Pointer& vertices, const QString& name);

  // -----------------------------------------------------------------------------
  // Inherited from SharedVertexOps
  // -----------------------------------------------------------------------------

  /**
   * @brief CreateSharedVertexList
   * @param numVertices
   * @return
   */
  static SharedVertexList::Pointer CreateSharedVertexList(size_t numVertices, bool allocate = true);

  /**
   * @brief resizeVertexList
   * @param newNumVertices
   */
  void resizeVertexList(size_t newNumVertices);

  /**
   * @brief setVertices
   * @param vertices
   */
  void setVertices(SharedVertexList::Pointer vertices);

  /**
   * @brief getVertices
   * @return
   */
  SharedVertexList::Pointer getVertices() const;

  /**
   * @brief setCoords
   * @param vertId
   * @param coords
   */
  void setCoords(size_t vertId, float coords[3]);

  /**
   * @brief getCoords
   * @param vertId
   * @param coords
   */
  void getCoords(size_t vertId, float coords[3]) const;

  /**
   * @brief getVertexPointer
   * @param i
   * @return
   */
  float* getVertexPointer(size_t i) const;

  /**
   * @brief getNumberOfVertices
   * @return
   */
  size_t getNumberOfVertices() const;

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

protected:
  VertexGeom();

  /**
   * @brief setElementsContainingVert
   * @param elementsContainingVert
   */
  void setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert) override;

  /**
   * @brief setElementNeighbors
   * @param elementNeighbors
   */
  void setElementNeighbors(ElementDynamicList::Pointer elementNeighbors) override;

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
  SharedVertexList::Pointer m_VertexList;
  FloatArrayType::Pointer m_VertexSizes;

public:
  VertexGeom(const VertexGeom&) = delete;            // Copy Constructor Not Implemented
  VertexGeom(VertexGeom&&) = delete;                 // Move Constructor Not Implemented
  VertexGeom& operator=(const VertexGeom&) = delete; // Copy Assignment Not Implemented
  VertexGeom& operator=(VertexGeom&&) = delete;      // Move Assignment Not Implemented
};
