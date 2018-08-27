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
#include "SIMPLib/Geometry/IGeometryGrid.h"

/**
 * @brief The RectGridGeom class represents a structured rectlinear grid
 */
class SIMPLib_EXPORT RectGridGeom : public IGeometryGrid
{
  public:
    SIMPL_SHARED_POINTERS(RectGridGeom)
    SIMPL_STATIC_NEW_MACRO(RectGridGeom)
     SIMPL_TYPE_MACRO_SUPER_OVERRIDE(RectGridGeom, Observable)

    ~RectGridGeom() override;

    /**
     * @brief CreateGeometry
     * @param name
     * @return
     */
    static Pointer CreateGeometry(const QString& name);
    
    void setXBounds(FloatArrayType::Pointer xBounds);
    void setYBounds(FloatArrayType::Pointer yBounds);
    void setZBounds(FloatArrayType::Pointer zBounds);

    inline FloatArrayType::Pointer getXBounds() { return m_xBounds; }
    inline FloatArrayType::Pointer getYBounds() { return m_yBounds; }
    inline FloatArrayType::Pointer getZBounds() { return m_zBounds; }

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
// Inherited from IGeometryGrid
// -----------------------------------------------------------------------------

    SIMPL_INSTANCE_VEC3_PROPERTY_VO(size_t, Dimensions)

    
    size_t getXPoints() override { return m_Dimensions[0]; }
    size_t getYPoints() override { return m_Dimensions[1]; }
    size_t getZPoints() override { return m_Dimensions[2]; }

    void getPlaneCoords(size_t idx[3], float coords[3]) override;
    void getPlaneCoords(size_t x, size_t y, size_t z, float coords[3]) override;
    void getPlaneCoords(size_t idx, float coords[3]) override;
    void getPlaneCoords(size_t idx[3], double coords[3]) override;
    void getPlaneCoords(size_t x, size_t y, size_t z, double coords[3]) override;
    void getPlaneCoords(size_t idx, double coords[3]) override;

    void getCoords(size_t idx[3], float coords[3]) override;
    void getCoords(size_t x, size_t y, size_t z, float coords[3]) override;
    void getCoords(size_t idx, float coords[3]) override;
    void getCoords(size_t idx[3], double coords[3]) override;
    void getCoords(size_t x, size_t y, size_t z, double coords[3]) override;
    void getCoords(size_t idx, double coords[3]) override;

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

    friend class FindRectGridDerivativesImpl;

    RectGridGeom(const RectGridGeom&) = delete;   // Copy Constructor Not Implemented
    void operator=(const RectGridGeom&) = delete; // Move assignment Not Implemented
};



