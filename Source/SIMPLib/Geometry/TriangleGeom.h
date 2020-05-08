/* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
#include "SIMPLib/Geometry/IGeometry2D.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

/**
 * @brief The TriangleGeom class represents a collection of triangles
 */
class SIMPLib_EXPORT TriangleGeom : public IGeometry2D
{
  // clang-format off
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(TriangleGeom SUPERCLASS IGeometry2D)
  PYB11_SHARED_POINTERS(TriangleGeom)
  PYB11_STATIC_NEW_MACRO(TriangleGeom)
  PYB11_METHOD(void setVertices ARGS vertices)
  PYB11_METHOD(SharedVertextList::Pointer getVertices)
  //PYB11_METHOD(void setEdges ARGS edges)
  PYB11_METHOD(SharedEdgeList::Pointer getEdges)
  PYB11_METHOD(void setCoords ARGS vertId coords)
  PYB11_METHOD(void getCoords ARGS vertId coords)
  PYB11_METHOD(size_t getNumberOfVertices)
  PYB11_METHOD(size_t getNumberOfEdges)
  PYB11_END_BINDINGS()
  // End Python bindings declarations
  // clang-format on

public:
  using Self = TriangleGeom;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for TriangleGeom
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for TriangleGeom
   */
  static QString ClassName();

  ~TriangleGeom() override;

  /**
   * @brief CreateGeometry
   * @param numTriangles
   * @param vertices
   * @param name
   * @return
   */
  static Pointer CreateGeometry(size_t numTriangles, const SharedVertexList::Pointer& vertices, const QString& name, bool allocate = true);

  /**
   * @brief CreateGeometry
   * @param triangles
   * @param vertices
   * @param name
   * @return
   */
  static Pointer CreateGeometry(const SharedTriList::Pointer& triangles, const SharedVertexList::Pointer& vertices, const QString& name);

  // -----------------------------------------------------------------------------
  // Inherited from SharedVertexOps
  // -----------------------------------------------------------------------------

  /**
   * @brief CreateSharedVertexList
   * @param numVertices
   * @return
   */
  static SharedVertexList::Pointer CreateSharedVertexList(size_t numVertices, bool allocate = true);

  // -----------------------------------------------------------------------------
  // Inherited from SharedEdgeOps
  // -----------------------------------------------------------------------------

  /**
   * @brief CreateSharedEdgeList
   * @param numEdges
   * @return
   */
  static SharedEdgeList::Pointer CreateSharedEdgeList(size_t numEdges, bool allocate = true);

  // -----------------------------------------------------------------------------
  // Inherited from SharedTriOps
  // -----------------------------------------------------------------------------

  /**
   * @brief CreateSharedTriLost
   * @param numTris
   * @return
   */
  static SharedTriList::Pointer CreateSharedTriList(size_t numTris, bool allocate = true);

  /**
   * @brief resizeTriList
   * @param newNumTris
   */
  void resizeTriList(size_t newNumTris);

  /**
   * @brief setTriangles
   * @param triangles
   */
  void setTriangles(SharedTriList::Pointer triangles);

  /**
   * @brief getTriangles
   * @return
   */
  SharedTriList::Pointer getTriangles() const;

  /**
   * @brief setVertsAtTri
   * @param triId
   * @param verts
   */
  void setVertsAtTri(size_t triId, size_t verts[3]);

  /**
   * @brief getVertsAtTri
   * @param triId
   * @param verts
   */
  void getVertsAtTri(size_t triId, size_t verts[3]) const;

  /**
   * @brief getVertCoordsAtTri
   * @param triId
   * @param vert1
   * @param vert2
   * @param vert3
   */
  void getVertCoordsAtTri(size_t triId, float vert1[3], float vert2[3], float vert3[3]) const;

  /**
   * @brief getPointer
   * @param i
   * @return
   */
  size_t* getTriPointer(size_t i) const;

  /**
   * @brief getNumberOfTris
   * @return
   */
  size_t getNumberOfTris() const;

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
  // Inherited from IGeometry2D
  // -----------------------------------------------------------------------------

  /**
   * @brief resizeVertexList
   * @param newNumVertices
   */
  void resizeVertexList(size_t newNumVertices) override;

  /**
   * @brief setVertices
   * @param vertices
   */
  void setVertices(SharedVertexList::Pointer vertices) override;

  /**
   * @brief getVertices
   * @return
   */
  SharedVertexList::Pointer getVertices() const override;

  /**
   * @brief setCoords
   * @param vertId
   * @param coords
   */
  void setCoords(size_t vertId, float coords[3]) override;

  /**
   * @brief getCoords
   * @param vertId
   * @param coords
   */
  void getCoords(size_t vertId, float coords[3]) const override;

  /**
   * @brief getVertexPointer
   * @param i
   * @return
   */
  float* getVertexPointer(size_t i) const override;

  /**
   * @brief getNumberOfVertices
   * @return
   */
  size_t getNumberOfVertices() const override;

  /**
   * @brief resizeEdgeList
   * @param newNumEdges
   */
  void resizeEdgeList(size_t newNumEdges) override;

  /**
   * @brief getEdges
   * @return
   */
  SharedEdgeList::Pointer getEdges() const override;

  /**
   * @brief setVerts
   * @param edgeId
   * @param verts
   */
  void setVertsAtEdge(size_t edgeId, size_t verts[2]) override;

  /**
   * @brief getVerts
   * @param edgeId
   * @param verts
   */
  void getVertsAtEdge(size_t edgeId, size_t verts[2]) const override;

  /**
   * @brief getVertCoordsAtEdge
   * @param edgeId
   * @param vert1
   * @param vert2
   */
  void getVertCoordsAtEdge(size_t edgeId, float vert1[3], float vert2[3]) const override;

  /**
   * @brief getEdgePointer
   * @param i
   * @return
   */
  size_t* getEdgePointer(size_t i) const override;

  /**
   * @brief getNumberOfEdges
   * @return
   */
  size_t getNumberOfEdges() const override;

  /**
   * @brief findElementEdges
   * @return
   */
  int findEdges() override;

  /**
   * @brief deleteElementEdges
   */
  void deleteEdges() override;

  /**
   * @brief findUnsharedEdges
   */
  int findUnsharedEdges() override;

  /**
   * @brief getUnsharedEdges
   * @return
   */
  SharedEdgeList::Pointer getUnsharedEdges() const override;

  /**
   * @brief deleteUnsharedEdges
   */
  void deleteUnsharedEdges() override;

protected:
  TriangleGeom();

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

  /**
   * @brief setEdges
   * @param edges
   */
  void setEdges(SharedEdgeList::Pointer edges) override;

  /**
   * @brief setUnsharedEdges
   * @param bEdgeList
   */
  void setUnsharedEdges(SharedEdgeList::Pointer bEdgeList) override;

private:
  SharedVertexList::Pointer m_VertexList;
  SharedEdgeList::Pointer m_EdgeList;
  SharedEdgeList::Pointer m_UnsharedEdgeList;
  SharedTriList::Pointer m_TriList;
  ElementDynamicList::Pointer m_TrianglesContainingVert;
  ElementDynamicList::Pointer m_TriangleNeighbors;
  FloatArrayType::Pointer m_TriangleCentroids;
  FloatArrayType::Pointer m_TriangleSizes;

  friend class FindTriangleDerivativesImpl;

public:
  TriangleGeom(const TriangleGeom&) = delete;            // Copy Constructor Not Implemented
  TriangleGeom(TriangleGeom&&) = delete;                 // Move Constructor Not Implemented
  TriangleGeom& operator=(const TriangleGeom&) = delete; // Copy Assignment Not Implemented
  TriangleGeom& operator=(TriangleGeom&&) = delete;      // Move Assignment Not Implemented
};
