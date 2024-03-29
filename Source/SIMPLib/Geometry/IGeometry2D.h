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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Geometry/IGeometry.h"

/**
 * @brief The IGeometry2D class extends IGeometry for elements with a topology of 2D
 */
class SIMPLib_EXPORT IGeometry2D : public IGeometry
{
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(IGeometry2D SUPERCLASS IGeometry)
  PYB11_SHARED_POINTERS(IGeometry2D)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = IGeometry2D;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for IGeometry2D
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for IGeometry2D
   */
  static QString ClassName();

  IGeometry2D();
  ~IGeometry2D() override;

  // -----------------------------------------------------------------------------
  // Inherited from SharedVertexOps
  // -----------------------------------------------------------------------------

  /**
   * @brief resizeVertexList
   * @param newNumVertices
   */
  virtual void resizeVertexList(size_t newNumVertices) = 0;

  /**
   * @brief setVertices
   * @param vertices
   */
  virtual void setVertices(SharedVertexList::Pointer vertices) = 0;

  /**
   * @brief getVertices
   * @return
   */
  virtual SharedVertexList::Pointer getVertices() const = 0;

  /**
   * @brief setCoords
   * @param vertId
   * @param coords
   */
  virtual void setCoords(size_t vertId, float coords[3]) = 0;

  /**
   * @brief getCoords
   * @param vertId
   * @param coords
   */
  virtual void getCoords(size_t vertId, float coords[3]) const = 0;

  /**
   * @brief getVertexPointer
   * @param i
   * @return
   */
  virtual float* getVertexPointer(size_t i) const = 0;

  /**
   * @brief getNumberOfVertices
   * @return
   */
  virtual size_t getNumberOfVertices() const = 0;

  // -----------------------------------------------------------------------------
  // Inherited from SharedEdgeOps
  // -----------------------------------------------------------------------------

  /**
   * @brief resizeEdgeList
   * @param newNumEdges
   */
  virtual void resizeEdgeList(size_t newNumEdges) = 0;

  /**
   * @brief getEdges
   * @return
   */
  virtual SharedEdgeList::Pointer getEdges() const = 0;

  /**
   * @brief setVerts
   * @param edgeId
   * @param verts
   */
  virtual void setVertsAtEdge(size_t edgeId, size_t verts[2]) = 0;

  /**
   * @brief getVerts
   * @param edgeId
   * @param verts
   */
  virtual void getVertsAtEdge(size_t edgeId, size_t verts[2]) const = 0;

  /**
   * @brief getVertCoordsAtEdge
   * @param edgeId
   * @param vert1
   * @param vert2
   */
  virtual void getVertCoordsAtEdge(size_t edgeId, float vert1[3], float vert2[3]) const = 0;

  /**
   * @brief getEdgePointer
   * @param i
   * @return
   */
  virtual size_t* getEdgePointer(size_t i) const = 0;

  /**
   * @brief getNumberOfEdges
   * @return
   */
  virtual size_t getNumberOfEdges() const = 0;

  // -----------------------------------------------------------------------------
  // Connectivity
  // -----------------------------------------------------------------------------

  /**
   * @brief findElementEdges
   * @return
   */
  virtual int findEdges() = 0;

  /**
   * @brief deleteElementEdges
   */
  virtual void deleteEdges() = 0;

  // -----------------------------------------------------------------------------
  // Topology
  // -----------------------------------------------------------------------------

  /**
   * @brief findUnsharedEdges
   */
  virtual int findUnsharedEdges() = 0;

  /**
   * @brief getUnsharedEdges
   * @return
   */
  virtual SharedEdgeList::Pointer getUnsharedEdges() const = 0;

  /**
   * @brief deleteUnsharedEdges
   */
  virtual void deleteUnsharedEdges() = 0;

protected:
  /**
   * @brief setEdges
   * @param edges
   */
  virtual void setEdges(SharedEdgeList::Pointer edges) = 0;

  /**
   * @brief setUnsharedEdges
   * @param bEdgeList
   */
  virtual void setUnsharedEdges(SharedEdgeList::Pointer bEdgeList) = 0;

public:
  IGeometry2D(const IGeometry2D&) = delete;            // Copy Constructor Not Implemented
  IGeometry2D(IGeometry2D&&) = delete;                 // Move Constructor Not Implemented
  IGeometry2D& operator=(const IGeometry2D&) = delete; // Copy Assignment Not Implemented
  IGeometry2D& operator=(IGeometry2D&&) = delete;      // Move Assignment Not Implemented
};
