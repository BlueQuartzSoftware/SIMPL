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

#include <QMutex>
#include <QtCore/QMap>
#include <QtCore/QString>

#include "SIMPLib/Common/Observable.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/DynamicListArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/Geometry/ITransformContainer.h"
#include "SIMPLib/SIMPLib.h"

class QTextStream;

// -----------------------------------------------------------------------------
// Typedefs
// -----------------------------------------------------------------------------
typedef FloatArrayType SharedVertexList;
typedef Int64ArrayType SharedEdgeList;
typedef Int64ArrayType SharedTriList;
typedef Int64ArrayType SharedQuadList;
typedef Int64ArrayType SharedTetList;
typedef Int64ArrayType SharedHexList;
typedef Int64ArrayType SharedFaceList;
typedef UInt16Int64DynamicListArray ElementDynamicList;

/**
 * @brief The IGeometry class
 */
class SIMPLib_EXPORT IGeometry : public Observable
{
  // This line MUST be first when exposing a class and properties to Python
  PYB11_CREATE_BINDINGS(IGeometry)

  PYB11_ENUMERATION(Type)
  PYB11_ENUMERATION(VtkCellType)

  PYB11_PROPERTY(QString Name READ getName WRITE setName)
  PYB11_METHOD(Type getGeometryType)
  PYB11_METHOD(QString getGeometryTypeAsString)
  PYB11_METHOD(QString getInfoString ARGS formatType)
  PYB11_PROPERTY(QString MessagePrefix READ getMessagePrefix WRITE setMessagePrefix)
  PYB11_PROPERTY(QString MessageTitle READ getMessageTitle WRITE setMessageTitle)
  PYB11_PROPERTY(QString MessageLabel READ getMessageLabel WRITE setMessageLabel)
  PYB11_METHOD(uint32_t getXdmfGridType)
  PYB11_METHOD(uint32_t getUnitDimensionality)
  PYB11_PROPERTY(uint32_t SpatialDimensionality READ getSpatialDimensionality WRITE setSpatialDimensionality)
  PYB11_METHOD(Pointer deepCopy ARGS forceNoAllocate)
  PYB11_METHOD(void initializeWithZeros)
  PYB11_METHOD(void addAttributeMatrix ARGS Name AttributeMatrix)
  PYB11_METHOD(AttributeMatrix getAttributeMatrix ARGS Name)
  PYB11_METHOD(AttributeMatrix removeAttributeMatrix ARGS Name)

  public:
    SIMPL_SHARED_POINTERS(IGeometry)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(IGeometry, Observable)

    IGeometry();
    ~IGeometry() override;

    using EnumType = unsigned int;

    /**
     * @brief The VtkCellType enum
     */
    enum class VtkCellType : EnumType
    {
       Image = 11,
       RectGrid = 11,
       Vertex = 1,
       Edge = 3,
       Triangle = 5,
       Quad = 9,
       Tetrahedral = 10,
       Hexahedral = 12,
       Unknown = 999,
       Any = 4294967295U
    };

    /**
     * @ brief The Type enum
     */
    enum class Type : EnumType
    {
      Image,
      RectGrid,
      Vertex,
      Edge,
      Triangle,
      Quad,
      Tetrahedral,
      Hexahedral,
      Unknown = 999,
      Any = 4294967295U
    };

    using VtkCellTypes = QVector <VtkCellType>;
    using Types = QVector<Type>;

    /**
     * @brief AttributeMatrixMap_t
     */
    typedef QMap<QString, AttributeMatrix::Pointer> AttributeMatrixMap_t;


    SIMPL_INSTANCE_PROPERTY(float, TimeValue)
    SIMPL_INSTANCE_PROPERTY(bool, EnableTimeSeries)
    SIMPL_INSTANCE_PROPERTY(ITransformContainer::Pointer, TransformContainer)

// -----------------------------------------------------------------------------
// Connectivity
// -----------------------------------------------------------------------------

    /**
     * @brief findElementsContainingVert
     * @return
     */
    virtual int findElementsContainingVert() = 0;

    /**
     * @brief getElementsContainingVert
     * @return
     */
    virtual ElementDynamicList::Pointer getElementsContainingVert() = 0;

    /**
     * @brief deleteElementsContainingVert
     */
    virtual void deleteElementsContainingVert() = 0;

    /**
     * @brief findElementNeighbors
     * @return
     */
    virtual int findElementNeighbors() = 0;

    /**
     * @brief getElementNeighbors
     * @return
     */
    virtual ElementDynamicList::Pointer getElementNeighbors() = 0;

    /**
     * @brief deleteElementNeighbors
     */
    virtual void deleteElementNeighbors() = 0;

// -----------------------------------------------------------------------------
// Topology
// -----------------------------------------------------------------------------

    /**
     * @brief getNumberOfElements
     * @return
     */
    virtual size_t getNumberOfElements() = 0;

    /**
     * @brief findElementSizes
     * @return
     */
    virtual int findElementSizes() = 0;

    /**
     * @brief getElementSizes
     * @return
     */
    virtual FloatArrayType::Pointer getElementSizes() = 0;

    /**
     * @brief deleteElementSizes
     */
    virtual void deleteElementSizes() = 0;

    /**
     * @brief findElementCentroids
     * @return
     */
    virtual int findElementCentroids() = 0;

    /**
     * @brief getElementCentroids
     * @return
     */
    virtual FloatArrayType::Pointer getElementCentroids() = 0;

    /**
     * @brief deleteElementCentroids
     */
    virtual void deleteElementCentroids() = 0;

    /**
     * @brief getParametricCenter
     * @param pCoords
     */
    virtual void getParametricCenter(double pCoords[3]) = 0;

    /**
     * @brief getShapeFunctions
     * @param pCoords
     * @param derivs
     */
    virtual void getShapeFunctions(double pCoords[3], double* shape) = 0;

    /**
     * @brief findDerivatives
     * @param field
     * @param derivatives
     */
    virtual void findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable) = 0;

// -----------------------------------------------------------------------------
// Generic
// -----------------------------------------------------------------------------

    /**
     * @brief setName
     * @param name
     */
    virtual void setName(const QString& name) final;

    /**
     * @brief getName
     * @return
     */
    virtual QString getName() final;

    /**
     * @brief getGeometryType
     * @return
     */
    virtual Type getGeometryType() final;

    /**
     * @brief getGeometryTypeAsString
     * @return
     */
    virtual QString getGeometryTypeAsString() final;

    /**
     * @brief getInfoString
     * @return Returns a formatted string that contains general infomation about
     * the instance of the object.
     */
    virtual QString getInfoString(SIMPL::InfoStringFormat) = 0;

    /**
     * @brief setMessagePrefix
     * @param prefix
     */
    virtual void setMessagePrefix(const QString& prefix) final;

    /**
     * @brief getMessagePrefix
     * @return
     */
    virtual QString getMessagePrefix() final;

    /**
     * @brief setMessageTitle
     * @param title
     */
    virtual void setMessageTitle(const QString& title) final;

    /**
     * @brief getMessageTitle
     * @return
     */
    virtual QString getMessageTitle() final;

    /**
     * @brief setMessageLabel
     * @param label
     */
    virtual void setMessageLabel(const QString& label) final;

    /**
     * @brief getMessageLabel
     * @return
     */
    virtual QString getMessageLabel() final;

    /**
     * @brief getXdmfGridType
     * @return
     */
    virtual unsigned int getXdmfGridType() final;

    /**
     * @brief getUnitDimensionality
     * @return
     */
    virtual unsigned int getUnitDimensionality() final;

    /**
     * @brief setSpatialDimensionality
     * @param spatialDims
     */
    virtual void setSpatialDimensionality(unsigned int spatialDims) final;

    /**
     * @brief getSpatialDimensionality
     * @return
     */
    virtual unsigned int getSpatialDimensionality() final;

    /**
     * @brief writeGeometryToHDF5
     * @param parentId
     * @param writeXdmf
     * @return
     */
    virtual int writeGeometryToHDF5(hid_t parentId, bool writeXdmf) = 0;

    /**
     * @brief writeXdmf
     * @param out
     * @param dcName
     * @param hdfFileName
     * @return
     */
    virtual int writeXdmf(QTextStream& out, QString dcName, QString hdfFileName) = 0;

    /**
     * @brief readGeometryFromHDF5
     * @param parentId
     * @param preflight
     * @return
     */
    virtual int readGeometryFromHDF5(hid_t parentId, bool preflight) = 0;

    /**
     * @brief deepCopy
     * @return
     */
    virtual Pointer deepCopy(bool forceNoAllocate = false) = 0;

    /**
     * @brief initializeWithZeros
     */
    virtual void initializeWithZeros() = 0;

    /**
     * @brief addAttributeMatrix
     */
    virtual void addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data) = 0;

    /**
     * @brief getAttributeMatrix
     * @param name
     * @return
     */
    virtual AttributeMatrix::Pointer getAttributeMatrix(const QString& name) final;

    /**
     * @brief removeAttributeMatrix
     * @param name
     * @return
     */
    virtual AttributeMatrix::Pointer removeAttributeMatrix(const QString& name) final;

  protected:
    QString m_Name;
    QString m_GeometryTypeName;
    QString m_MessagePrefix;
    QString m_MessageTitle;
    QString m_MessageLabel;
    Type m_GeometryType = Type::Unknown;
    unsigned int m_XdmfGridType = SIMPL::XdmfGridType::UnknownGrid;
    unsigned int m_UnitDimensionality = 0;
    unsigned int m_SpatialDimensionality = 0;

    AttributeMatrixMap_t m_AttributeMatrices;

    QMutex m_Mutex;
    int64_t m_ProgressCounter;

    /**
     * @brief sendThreadSafeProgressMessage
     * @param counter
     * @param max
     */
    virtual void sendThreadSafeProgressMessage(int64_t counter, int64_t max) final;

    /**
     * @brief setElementsContaingVert
     * @param elementsContaingVert
     */
    virtual void setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert) = 0;

    /**
     * @brief setElementNeighbors
     * @param elementNeighbors
     */
    virtual void setElementNeighbors(ElementDynamicList::Pointer elementsNeighbors) = 0;

    /**
     * @brief setElementCentroids
     * @param elementCentroids
     */
    virtual void setElementCentroids(FloatArrayType::Pointer elementCentroids) = 0;

    /**
     * @brief setElementSizes
     * @param elementSizes
     */
    virtual void setElementSizes(FloatArrayType::Pointer elementSizes) = 0;

  private:
    IGeometry(const IGeometry&) = delete;      // Copy Constructor Not Implemented
    void operator=(const IGeometry&) = delete; // Move assignment Not Implemented
};
