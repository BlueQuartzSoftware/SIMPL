#ifndef _hexahedralgeom_h_
#define _hexahedralgeom_h_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Geometry/IGeometry3D.h"

/**
 * @brief The HexahedralGeom class represents a collection of hexahedra
 */
class SIMPLib_EXPORT HexahedralGeom : public IGeometry3D
{
  public:

    SIMPL_SHARED_POINTERS(HexahedralGeom)
    SIMPL_STATIC_NEW_MACRO(HexahedralGeom)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(HexahedralGeom, Observable)

    virtual ~HexahedralGeom();

    /**
     * @brief CreateGeometry
     * @param numHexas
     * @param vertices
     * @param name
     * @return
     */
    static Pointer CreateGeometry(int64_t numHexas, SharedVertexList::Pointer vertices, const QString& name, bool allocate = true);

    /**
     * @brief CreateGeometry
     * @param hexas
     * @param vertices
     * @param name
     * @return
     */
    static Pointer CreateGeometry(SharedHexList::Pointer hexas, SharedVertexList::Pointer vertices, const QString& name);

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
// Inherited from SharedQuadOps
// -----------------------------------------------------------------------------

    /**
     * @brief CreateSharedQuadLost
     * @param numQuads
     * @return
     */
    static SharedQuadList::Pointer CreateSharedQuadList(int64_t numQuads, bool allocate = true);

    /**
     * @brief resizeQuadList
     * @param newNumQuads
     */
    void resizeQuadList(int64_t newNumQuads);

    /**
     * @brief setQuads
     * @param quads
     */
    void setQuads(SharedQuadList::Pointer quads);

    /**
     * @brief getQuads
     * @return
     */
    SharedQuadList::Pointer getQuads();

    /**
     * @brief setVertsAtQuad
     * @param quadId
     * @param verts
     */
    void setVertsAtQuad(int64_t quadId, int64_t verts[4]);

    /**
     * @brief getVertsAtQuad
     * @param quadId
     * @param verts
     */
    void getVertsAtQuad(int64_t quadId, int64_t verts[4]);

    /**
     * @brief getVertCoordsAtQuad
     * @param quadId
     * @param vert1
     * @param vert2
     * @param vert3
     * @param vert4
     */
    void getVertCoordsAtQuad(int64_t quadId, float vert1[3], float vert2[3], float vert3[3], float vert4[3]);

    /**
     * @brief getPointer
     * @param i
     * @return
     */
    int64_t* getQuadPointer(int64_t i);

    /**
     * @brief getNumberOfQuads
     * @return
     */
    int64_t getNumberOfQuads();

// -----------------------------------------------------------------------------
// Inherited from SharedHexOps
// -----------------------------------------------------------------------------

    /**
     * @brief CreateSharedHexList
     * @param numTets
     * @return
     */
    static SharedHexList::Pointer CreateSharedHexList(int64_t numHexas, bool allocate = true);

    /**
     * @brief resizeHexList
     * @param newNumHexas
     */
    void resizeHexList(int64_t newNumHexas);

    /**
     * @brief setHexahedra
     * @param tets
     */
    void setHexahedra(SharedHexList::Pointer hexas);

   /**
    * @brief getHexahedra
    * @return
    */
    SharedHexList::Pointer getHexahedra();

    /**
     * @brief setVertsAtHex
     * @param hexId
     * @param verts
     */
    void setVertsAtHex(int64_t hexId, int64_t verts[7]);

    /**
     * @brief getVertsAtHex
     * @param hexId
     * @param verts
     */
    void getVertsAtHex(int64_t hexId, int64_t verts[7]);

    /**
     * @brief getVertCoordsAtHex
     * @param hexId
     * @param vert1
     * @param vert2
     * @param vert3
     * @param vert4
     * @param vert5
     * @param vert6
     */
    void getVertCoordsAtHex(int64_t hexId, float vert1[3], float vert2[3], float vert3[3], float vert4[3], float vert5[3], float vert6[3], float vert7[3], float vert8[3]);

    /**
     * @brief getHexPointer
     * @param i
     * @return
     */
    int64_t* getHexPointer(int64_t i);

    /**
     * @brief getNumberOfHexas
     * @return
     */
    int64_t getNumberOfHexas();

// -----------------------------------------------------------------------------
// Inherited from IGeometry
// -----------------------------------------------------------------------------

    /**
     * @brief initializeWithZeros
     */
    virtual void initializeWithZeros();

    /**
     * @brief getNumberOfElements
     * @return
     */
    virtual size_t getNumberOfElements();

    /**
     * @brief findElementSizes
     * @return
     */
    virtual int findElementSizes();

    /**
     * @brief getElementSizes
     * @return
     */
    virtual FloatArrayType::Pointer getElementSizes();

    /**
     * @brief deleteElementSizes
     */
    virtual void deleteElementSizes();

    /**
     * @brief findElementsContainingVert
     * @return
     */
    virtual int findElementsContainingVert();

    /**
     * @brief getElementsContainingVert
     * @return
     */
    virtual ElementDynamicList::Pointer getElementsContainingVert();

    /**
     * @brief deleteElementsContainingVert
     */
    virtual void deleteElementsContainingVert();

    /**
     * @brief findElementNeighbors
     * @return
     */
    virtual int findElementNeighbors();

    /**
     * @brief getElementNeighbors
     * @return
     */
    virtual ElementDynamicList::Pointer getElementNeighbors();

    /**
     * @brief deleteElementNeighbors
     */
    virtual void deleteElementNeighbors();

    /**
     * @brief findElementCentroids
     * @return
     */
    virtual int findElementCentroids();

    /**
     * @brief getElementCentroids
     * @return
     */
    virtual FloatArrayType::Pointer getElementCentroids();

    /**
     * @brief deleteElementCentroids
     */
    virtual void deleteElementCentroids();

    /**
     * @brief getParametricCenter
     * @param pCoords
     */
    virtual void getParametricCenter(double pCoords[3]);

    /**
     * @brief getShapeFunctions
     * @param pCoords
     * @param shape
     */
    virtual void getShapeFunctions(double pCoords[3], double* shape);

    /**
     * @brief findDerivatives
     * @param field
     * @param derivatives
     */
    virtual void findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable = nullptr);

    /**
     * @brief getInfoString
     * @return Returns a formatted string that contains general infomation about
     * the instance of the object.
     */
    virtual QString getInfoString(SIMPL::InfoStringFormat format);

    /**
     * @brief writeGeometryToHDF5
     * @param parentId
     * @param writeXdmf
     * @return
     */
    virtual int writeGeometryToHDF5(hid_t parentId, bool writeXdmf);

    /**
     * @brief writeXdmf
     * @param out
     * @param dcName
     * @param hdfFileName
     * @return
     */
    virtual int writeXdmf(QTextStream& out, QString dcName, QString hdfFileName);

    /**
     * @brief readGeometryFromHDF5
     * @param parentId
     * @param preflight
     * @return
     */
    virtual int readGeometryFromHDF5(hid_t parentId, bool preflight);

    /**
     * @brief deepCopy
     * @return
     */
    virtual IGeometry::Pointer deepCopy(bool forceNoAllocate = false) override;

    /**
     * @brief addAttributeMatrix
     */
    virtual void addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data);

// -----------------------------------------------------------------------------
// Inherited from IGeometry3D
// -----------------------------------------------------------------------------

    /**
     * @brief resizeVertexList
     * @param newNumVertices
     */
    virtual void resizeVertexList(int64_t newNumVertices);

    /**
     * @brief setVertices
     * @param vertices
     */
    virtual void setVertices(SharedVertexList::Pointer vertices);

    /**
     * @brief getVertices
     * @return
     */
    virtual SharedVertexList::Pointer getVertices();

    /**
     * @brief setCoords
     * @param vertId
     * @param coords
     */
    virtual void setCoords(int64_t vertId, float coords[3]);

    /**
     * @brief getCoords
     * @param vertId
     * @param coords
     */
    virtual void getCoords(int64_t vertId, float coords[3]);

    /**
     * @brief getVertexPointer
     * @param i
     * @return
     */
    virtual float* getVertexPointer(int64_t i);

    /**
     * @brief getNumberOfVertices
     * @return
     */
    virtual int64_t getNumberOfVertices();

    /**
     * @brief resizeEdgeList
     * @param newNumEdges
     */
    virtual void resizeEdgeList(int64_t newNumEdges);

    /**
     * @brief getEdges
     * @return
     */
    virtual SharedEdgeList::Pointer getEdges();

    /**
     * @brief setVerts
     * @param edgeId
     * @param verts
     */
    virtual void setVertsAtEdge(int64_t edgeId, int64_t verts[2]);

    /**
     * @brief getVerts
     * @param edgeId
     * @param verts
     */
    virtual void getVertsAtEdge(int64_t edgeId, int64_t verts[2]);

    /**
     * @brief getVertCoordsAtEdge
     * @param edgeId
     * @param vert1
     * @param vert2
     */
    virtual void getVertCoordsAtEdge(int64_t edgeId, float vert1[3], float vert2[3]);

    /**
     * @brief getEdgePointer
     * @param i
     * @return
     */
    virtual int64_t* getEdgePointer(int64_t i);

    /**
     * @brief getNumberOfEdges
     * @return
     */
    virtual int64_t getNumberOfEdges();

    /**
     * @brief findElementEdges
     * @return
     */
    virtual int findEdges();

    /**
     * @brief deleteElementEdges
     */
    virtual void deleteEdges();

    /**
     * @brief findFaces
     * @return
     */
    virtual int findFaces();

    /**
     * @brief deleteFaces
     */
    virtual void deleteFaces();

    /**
     * @brief findUnsharedEdges
     */
    virtual int findUnsharedEdges();

    /**
     * @brief getUnsharedEdges
     * @return
     */
    virtual SharedEdgeList::Pointer getUnsharedEdges();

    /**
     * @brief deleteUnsharedEdges
     */
    virtual void deleteUnsharedEdges();

    /**
     * @brief findUnsharedFaces
     */
    virtual int findUnsharedFaces();

    /**
     * @brief getUnsharedFaces
     * @return
     */
    virtual SharedEdgeList::Pointer getUnsharedFaces();

    /**
     * @brief deleteUnsharedFaces
     */
    virtual void deleteUnsharedFaces();

  protected:

    HexahedralGeom();

    /**
     * @brief setElementsContainingVert
     * @param elementsContainingVert
     */
    virtual void setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert);

    /**
     * @brief setElementNeighbors
     * @param elementNeighbors
     */
    virtual void setElementNeighbors(ElementDynamicList::Pointer elementNeighbors);

    /**
     * @brief setElementCentroids
     * @param elementCentroids
     */
    virtual void setElementCentroids(FloatArrayType::Pointer elementCentroids);

    /**
     * @brief setElementSizes
     * @param elementSizes
     */
    virtual void setElementSizes(FloatArrayType::Pointer elementSizes);

    /**
     * @brief setEdges
     * @param edges
     */
    virtual void setEdges(SharedEdgeList::Pointer edges);

    /**
     * @brief setUnsharedEdges
     * @param bEdgeList
     */
    virtual void setUnsharedEdges(SharedEdgeList::Pointer bEdgeList);

    /**
     * @brief setUnsharedFaces
     * @param bFaceList
     */
    virtual void setUnsharedFaces(SharedFaceList::Pointer bFaceList);

  private:
    SharedVertexList::Pointer m_VertexList;
    SharedEdgeList::Pointer m_EdgeList;
    SharedEdgeList::Pointer m_UnsharedEdgeList;
    SharedQuadList::Pointer m_QuadList;
    SharedQuadList::Pointer m_UnsharedQuadList;
    SharedHexList::Pointer m_HexList;
    ElementDynamicList::Pointer m_HexasContainingVert;
    ElementDynamicList::Pointer m_HexNeighbors;
    FloatArrayType::Pointer m_HexCentroids;
    FloatArrayType::Pointer m_HexSizes;

    friend class FindHexDerivativesImpl;

    HexahedralGeom(const HexahedralGeom&) = delete; // Copy Constructor Not Implemented
    void operator=(const HexahedralGeom&) = delete;  // Operator '=' Not Implemented
};


#endif /* _hexahedralgeom_h_ */

