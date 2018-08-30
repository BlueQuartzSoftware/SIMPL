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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Geometry/IGeometry3D.h"

/**
 * @brief The TetrahedralGeom class represents a collection of tetrahedra
 */
class SIMPLib_EXPORT TetrahedralGeom : public IGeometry3D
{
  public:

    SIMPL_SHARED_POINTERS(TetrahedralGeom)
    SIMPL_STATIC_NEW_MACRO(TetrahedralGeom)
     SIMPL_TYPE_MACRO_SUPER_OVERRIDE(TetrahedralGeom, Observable)

    ~TetrahedralGeom() override;

    /**
     * @brief CreateGeometry
     * @param numTets
     * @param vertices
     * @param name
     * @return
     */
    static Pointer CreateGeometry(int64_t numTets, SharedVertexList::Pointer vertices, const QString& name, bool allocate = true);

    /**
     * @brief CreateGeometry
     * @param tets
     * @param vertices
     * @param name
     * @return
     */
    static Pointer CreateGeometry(SharedTetList::Pointer tets, SharedVertexList::Pointer vertices, const QString& name);

// -----------------------------------------------------------------------------
// Inherited from SharedVertexOps
// -----------------------------------------------------------------------------

    /**
     * @brief CreateSharedVertexList
     * @param numVertices
     * @return
     */
    static SharedVertexList::Pointer CreateSharedVertexList(int64_t numVertices, bool allocate = true);

// -----------------------------------------------------------------------------
// Inherited from SharedEdgeOps
// -----------------------------------------------------------------------------

    /**
     * @brief CreateSharedEdgeList
     * @param numEdges
     * @return
     */
    static SharedEdgeList::Pointer CreateSharedEdgeList(int64_t numEdges, bool allocate = true);

// -----------------------------------------------------------------------------
// Inherited from SharedTriOps
// -----------------------------------------------------------------------------

    /**
     * @brief CreateSharedTriList
     * @param numTris
     * @return
     */
    static SharedTriList::Pointer CreateSharedTriList(int64_t numTris, bool allocate = true);

    /**
     * @brief resizeTriList
     * @param newNumTris
     */
    void resizeTriList(int64_t newNumTris);

    /**
     * @brief setTriangles
     * @param triangles
     */
    void setTriangles(SharedTriList::Pointer triangles);

    /**
     * @brief getTriangles
     * @return
     */
    SharedTriList::Pointer getTriangles();

    /**
     * @brief setVertsAtTri
     * @param triId
     * @param verts
     */
    void setVertsAtTri(int64_t triId, int64_t verts[3]);

    /**
     * @brief getVertsAtTri
     * @param triId
     * @param verts
     */
    void getVertsAtTri(int64_t triId, int64_t verts[3]);

    /**
     * @brief getVertCoordsAtTri
     * @param triId
     * @param vert1
     * @param vert2
     * @param vert3
     */
    void getVertCoordsAtTri(int64_t triId, float vert1[3], float vert2[3], float vert3[3]);

    /**
     * @brief getTriPointer
     * @param i
     * @return
     */
    int64_t* getTriPointer(int64_t i);

    /**
     * @brief getNumberOfTris
     * @return
     */
    int64_t getNumberOfTris();

// -----------------------------------------------------------------------------
// Inherited from SharedTetOps
// -----------------------------------------------------------------------------

    /**
     * @brief CreateSharedTetList
     * @param numTets
     * @return
     */
    static SharedTriList::Pointer CreateSharedTetList(int64_t numTets, bool allocate = true);

    /**
     * @brief resizeTetList
     * @param newNumTets
     */
    void resizeTetList(int64_t newNumTets);

    /**
     * @brief setTetrahedra
     * @param tets
     */
    void setTetrahedra(SharedTetList::Pointer tets);

   /**
    * @brief getTetrahedra
    * @return
    */
    SharedTetList::Pointer getTetrahedra();

    /**
     * @brief setVertsAtTet
     * @param tetId
     * @param verts
     */
    void setVertsAtTet(int64_t tetId, int64_t verts[4]);

    /**
     * @brief getVertsAtTet
     * @param tetId
     * @param verts
     */
    void getVertsAtTet(int64_t tetId, int64_t verts[4]);

    /**
     * @brief getVertCoordsAtTet
     * @param tetId
     * @param vert1
     * @param vert2
     * @param vert3
     */
    void getVertCoordsAtTet(int64_t tetId, float vert1[3], float vert2[3], float vert3[3], float vert4[3]);

    /**
     * @brief getTetPointer
     * @param i
     * @return
     */
    int64_t* getTetPointer(int64_t i);

    /**
     * @brief getNumberOfTets
     * @return
     */
    int64_t getNumberOfTets();

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
    size_t getNumberOfElements() override;

    /**
     * @brief findElementSizes
     * @return
     */
    int findElementSizes() override;

    /**
     * @brief getElementSizes
     * @return
     */
    FloatArrayType::Pointer getElementSizes() override;

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
    ElementDynamicList::Pointer getElementsContainingVert() override;

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
    ElementDynamicList::Pointer getElementNeighbors() override;

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
    FloatArrayType::Pointer getElementCentroids() override;

    /**
     * @brief deleteElementCentroids
     */
    void deleteElementCentroids() override;

    /**
     * @brief getParametricCenter
     * @param pCoords
     */
    void getParametricCenter(double pCoords[3]) override;

    /**
     * @brief getShapeFunctions
     * @param pCoords
     * @param shape
     */
    void getShapeFunctions(double pCoords[3], double* shape) override;

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
    QString getInfoString(SIMPL::InfoStringFormat format) override;

    /**
     * @brief writeGeometryToHDF5
     * @param parentId
     * @param writeXdmf
     * @return
     */
    int writeGeometryToHDF5(hid_t parentId, bool writeXdmf) override;

    /**
     * @brief writeXdmf
     * @param out
     * @param dcName
     * @param hdfFileName
     * @return
     */
    int writeXdmf(QTextStream& out, QString dcName, QString hdfFileName) override;

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
    IGeometry::Pointer deepCopy(bool forceNoAllocate = false) override;

    /**
     * @brief addAttributeMatrix
     */
    void addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data) override;

// -----------------------------------------------------------------------------
// Inherited from IGeometry3D
// -----------------------------------------------------------------------------

    /**
     * @brief resizeVertexList
     * @param newNumVertices
     */
    void resizeVertexList(int64_t newNumVertices) override;

    /**
     * @brief setVertices
     * @param vertices
     */
    void setVertices(SharedVertexList::Pointer vertices) override;

    /**
     * @brief getVertices
     * @return
     */
    SharedVertexList::Pointer getVertices() override;

    /**
     * @brief setCoords
     * @param vertId
     * @param coords
     */
    void setCoords(int64_t vertId, float coords[3]) override;

    /**
     * @brief getCoords
     * @param vertId
     * @param coords
     */
    void getCoords(int64_t vertId, float coords[3]) override;

    /**
     * @brief getVertexPointer
     * @param i
     * @return
     */
    float* getVertexPointer(int64_t i) override;

    /**
     * @brief getNumberOfVertices
     * @return
     */
    int64_t getNumberOfVertices() override;

    /**
     * @brief resizeEdgeList
     * @param newNumEdges
     */
    void resizeEdgeList(int64_t newNumEdges) override;

    /**
     * @brief getEdges
     * @return
     */
    SharedEdgeList::Pointer getEdges() override;

    /**
     * @brief setVerts
     * @param edgeId
     * @param verts
     */
    void setVertsAtEdge(int64_t edgeId, int64_t verts[2]) override;

    /**
     * @brief getVerts
     * @param edgeId
     * @param verts
     */
    void getVertsAtEdge(int64_t edgeId, int64_t verts[2]) override;

    /**
     * @brief getVertCoordsAtEdge
     * @param edgeId
     * @param vert1
     * @param vert2
     */
    void getVertCoordsAtEdge(int64_t edgeId, float vert1[3], float vert2[3]) override;

    /**
     * @brief getEdgePointer
     * @param i
     * @return
     */
    int64_t* getEdgePointer(int64_t i) override;

    /**
     * @brief getNumberOfEdges
     * @return
     */
    int64_t getNumberOfEdges() override;

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
     * @brief findFaces
     * @return
     */
    int findFaces() override;

    /**
     * @brief deleteFaces
     */
    void deleteFaces() override;

    /**
     * @brief findUnsharedEdges
     */
    int findUnsharedEdges() override;

    /**
     * @brief getUnsharedEdges
     * @return
     */
    SharedEdgeList::Pointer getUnsharedEdges() override;

    /**
     * @brief deleteUnsharedEdges
     */
    void deleteUnsharedEdges() override;

    /**
     * @brief findUnsharedFaces
     */
    int findUnsharedFaces() override;

    /**
     * @brief getUnsharedFaces
     * @return
     */
    SharedEdgeList::Pointer getUnsharedFaces() override;

    /**
     * @brief deleteUnsharedFaces
     */
    void deleteUnsharedFaces() override;

  protected:

    TetrahedralGeom();

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

    /**
     * @brief setUnsharedFaces
     * @param bFaceList
     */
    void setUnsharedFaces(SharedFaceList::Pointer bFaceList) override;

  private:
    SharedVertexList::Pointer m_VertexList;
    SharedEdgeList::Pointer m_EdgeList;
    SharedEdgeList::Pointer m_UnsharedEdgeList;
    SharedTriList::Pointer m_TriList;
    SharedTriList::Pointer m_UnsharedTriList;
    SharedTetList::Pointer m_TetList;
    ElementDynamicList::Pointer m_TetsContainingVert;
    ElementDynamicList::Pointer m_TetNeighbors;
    FloatArrayType::Pointer m_TetCentroids;
    FloatArrayType::Pointer m_TetSizes;

    friend class FindTetDerivativesImpl;

    TetrahedralGeom(const TetrahedralGeom&) = delete; // Copy Constructor Not Implemented
    void operator=(const TetrahedralGeom&) = delete;  // Move assignment Not Implemented
};



