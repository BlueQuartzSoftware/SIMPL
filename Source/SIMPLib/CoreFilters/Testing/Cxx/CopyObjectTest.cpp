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

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/HexahedralGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

class CopyObjectTest
{

public:
  CopyObjectTest() = default;
  ~CopyObjectTest() = default;
  CopyObjectTest(const CopyObjectTest&) = delete;            // Copy Constructor
  CopyObjectTest(CopyObjectTest&&) = delete;                 // Move Constructor
  CopyObjectTest& operator=(const CopyObjectTest&) = delete; // Copy Assignment
  CopyObjectTest& operator=(CopyObjectTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the CopyObjectTest Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The CopyObjectTest Requires the use of the " << m_FilterName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  std::shared_ptr<DataArray<T>> createDataArray(const QString& daName, const std::vector<std::vector<T>>& data, const std::vector<size_t>& tupleDims, const std::vector<size_t>& cDims)
  {
    typename DataArray<T>::Pointer da = DataArray<T>::CreateArray(tupleDims, cDims, daName, true);
    for(size_t i = 0; i < da->getNumberOfTuples(); i++)
    {
      da->setTuple(i, data[i]);
    }
    return da;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  std::shared_ptr<DataArray<T>> createDataArray(const QString& daName, const std::vector<size_t>& tupleDims, const std::vector<size_t>& cDims)
  {
    typename DataArray<T>::Pointer da = DataArray<T>::CreateArray(tupleDims, cDims, daName, true);
    for(size_t i = 0; i < da->getSize(); i++)
    {
      da->setValue(i, static_cast<T>(i));
    }
    return da;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  void checkDataArray(IDataArray::Pointer originalData, IDataArray::Pointer newData, bool isRenamed)
  {
    if(originalData == IDataArray::NullPointer() || newData == IDataArray::NullPointer())
    {
      bool bothNullPtr = (originalData == newData);
      DREAM3D_REQUIRE_EQUAL(bothNullPtr, true)
    }
    else
    {
      if(!isRenamed)
      {
        DREAM3D_REQUIRE_EQUAL(originalData->getName(), newData->getName())
      }

      QString oldType = originalData->getTypeAsString();
      QString newType = newData->getTypeAsString();
      DREAM3D_REQUIRE_EQUAL(newType, oldType)

      DREAM3D_REQUIRE_EQUAL(originalData->getNumberOfTuples(), newData->getNumberOfTuples())
      DREAM3D_REQUIRE_EQUAL(originalData->getNumberOfComponents(), newData->getNumberOfComponents())
      DREAM3D_REQUIRE_EQUAL(originalData->getSize(), newData->getSize())

      bool sameTupleDims = (originalData->getComponentDimensions() == newData->getComponentDimensions());
      DREAM3D_REQUIRE_EQUAL(sameTupleDims, true)

      typename DataArray<T>::Pointer daOriginal = std::dynamic_pointer_cast<DataArray<T>>(originalData);
      typename DataArray<T>::Pointer daNew = std::dynamic_pointer_cast<DataArray<T>>(newData);

      for(int i = 0; i < daOriginal->getSize(); i++)
      {
        T* valueOld = daOriginal->getPointer(i);
        T* valueNew = daNew->getPointer(i);
        DREAM3D_REQUIRE_EQUAL(valueNew[0], valueOld[0])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void checkAttributeMatrix(AttributeMatrix::Pointer oldAM, AttributeMatrix::Pointer newAM, bool isRenamed)
  {
    if(!isRenamed)
    {
      DREAM3D_REQUIRE_EQUAL(newAM->getName(), oldAM->getName())
    }

    bool sameTupleDims = (newAM->getTupleDimensions() == oldAM->getTupleDimensions());
    DREAM3D_REQUIRE_EQUAL(sameTupleDims, true)

    DREAM3D_REQUIRE_EQUAL(newAM->getNumberOfTuples(), oldAM->getNumberOfTuples())

    bool sameType = (newAM->getType() == oldAM->getType());
    DREAM3D_REQUIRE_EQUAL(sameType, true)

    QList<QString> oldArrayNames = oldAM->getAttributeArrayNames();
    QList<QString> newArrayNames = newAM->getAttributeArrayNames();

    bool sameArrayNames = (newArrayNames == oldArrayNames);
    DREAM3D_REQUIRE_EQUAL(sameArrayNames, true)

    for(int i = 0; i < newArrayNames.size(); i++)
    {
      IDataArray::Pointer oldArray = oldAM->getAttributeArray(oldArrayNames[i]);
      IDataArray::Pointer newArray = newAM->getAttributeArray(newArrayNames[i]);

      QString oldType = oldArray->getTypeAsString();
      QString newType = newArray->getTypeAsString();
      DREAM3D_REQUIRE_EQUAL(newType, oldType)

      if(newType == "int8_t")
      {
        checkDataArray<int8_t>(oldArray, newArray, false);
      }
      else if(newType == "uint8_t")
      {
        checkDataArray<uint8_t>(oldArray, newArray, false);
      }
      else if(newType == "int16_t")
      {
        checkDataArray<int16_t>(oldArray, newArray, false);
      }
      else if(newType == "uint16_t")
      {
        checkDataArray<uint16_t>(oldArray, newArray, false);
      }
      else if(newType == "int32_t")
      {
        checkDataArray<int32_t>(oldArray, newArray, false);
      }
      else if(newType == "uint32_t")
      {
        checkDataArray<uint32_t>(oldArray, newArray, false);
      }
      else if(newType == "int64_t")
      {
        checkDataArray<int64_t>(oldArray, newArray, false);
      }
      else if(newType == "uint64_t")
      {
        checkDataArray<uint64_t>(oldArray, newArray, false);
      }
      else if(newType == "float")
      {
        checkDataArray<float>(oldArray, newArray, false);
      }
      else if(newType == "double")
      {
        checkDataArray<double>(oldArray, newArray, false);
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void checkElementDynamicList(ElementDynamicList::Pointer oldList, ElementDynamicList::Pointer newList)
  {
    if(oldList == ElementDynamicList::NullPointer() || newList == ElementDynamicList::NullPointer())
    {
      bool bothNullPtr = (oldList == newList);
      DREAM3D_REQUIRE_EQUAL(bothNullPtr, true)
    }
    else
    {
      DREAM3D_REQUIRE_EQUAL(newList->size(), oldList->size())

      for(size_t i = 0; i < oldList->size(); i++)
      {
        ElementDynamicList::ElementList oldElementList = oldList->getElementList(i);
        ElementDynamicList::ElementList newElementList = newList->getElementList(i);

        DREAM3D_REQUIRE_EQUAL(newElementList.ncells, oldElementList.ncells)

        for(uint16_t j = 0; j < oldElementList.ncells; j++)
        {
          DREAM3D_REQUIRE_EQUAL(newElementList.cells[j], oldElementList.cells[j])
        }
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void checkGeometry(IGeometry::Pointer oldGeom, IGeometry::Pointer newGeom)
  {
    if(oldGeom == IGeometry::NullPointer() || newGeom == IGeometry::NullPointer())
    {
      bool bothNullPtr = (oldGeom == newGeom);
      DREAM3D_REQUIRE_EQUAL(bothNullPtr, true)
    }
    else
    {
      DREAM3D_REQUIRE_EQUAL(newGeom->getName(), oldGeom->getName())

      IGeometry::Type oldType = oldGeom->getGeometryType();
      IGeometry::Type newType = newGeom->getGeometryType();

      bool sameType = (newType == oldType);
      DREAM3D_REQUIRE_EQUAL(sameType, true)

      FloatArrayType::Pointer oldElementSizes = oldGeom->getElementSizes();
      FloatArrayType::Pointer newElementSizes = newGeom->getElementSizes();
      checkDataArray<float>(oldElementSizes, newElementSizes, false);

      DREAM3D_REQUIRE_EQUAL(newGeom->getSpatialDimensionality(), oldGeom->getSpatialDimensionality())

      if(newType == IGeometry::Type::Image)
      {
        ImageGeom::Pointer oldImageGeom = std::dynamic_pointer_cast<ImageGeom>(oldGeom);
        ImageGeom::Pointer newImageGeom = std::dynamic_pointer_cast<ImageGeom>(newGeom);

        SizeVec3Type oldDims = oldImageGeom->getDimensions();
        SizeVec3Type newDims = newImageGeom->getDimensions();
        bool sameDims = (oldDims == newDims);
        DREAM3D_REQUIRE_EQUAL(sameDims, true)

        FloatVec3Type oldRes = oldImageGeom->getSpacing();
        FloatVec3Type newRes = newImageGeom->getSpacing();
        bool sameRes = (oldRes == newRes);
        DREAM3D_REQUIRE_EQUAL(sameRes, true)

        FloatVec3Type oldOrigin = oldImageGeom->getOrigin();
        FloatVec3Type newOrigin = newImageGeom->getOrigin();
        bool sameOrigin = (oldOrigin == newOrigin);
        DREAM3D_REQUIRE_EQUAL(sameOrigin, true)
      }
      else if(newType == IGeometry::Type::RectGrid)
      {
        RectGridGeom::Pointer oldRectGridGeom = std::dynamic_pointer_cast<RectGridGeom>(oldGeom);
        RectGridGeom::Pointer newRectGridGeom = std::dynamic_pointer_cast<RectGridGeom>(newGeom);

        SizeVec3Type oldDims = oldRectGridGeom->getDimensions();
        SizeVec3Type newDims = newRectGridGeom->getDimensions();
        bool sameDims = (oldDims == newDims);
        DREAM3D_REQUIRE_EQUAL(sameDims, true)

        FloatArrayType::Pointer oldXBounds = oldRectGridGeom->getXBounds();
        FloatArrayType::Pointer newXBounds = newRectGridGeom->getXBounds();
        checkDataArray<float>(oldXBounds, newXBounds, false);

        FloatArrayType::Pointer oldYBounds = oldRectGridGeom->getYBounds();
        FloatArrayType::Pointer newYBounds = newRectGridGeom->getYBounds();
        checkDataArray<float>(oldYBounds, newYBounds, false);

        FloatArrayType::Pointer oldZBounds = oldRectGridGeom->getZBounds();
        FloatArrayType::Pointer newZBounds = newRectGridGeom->getZBounds();
        checkDataArray<float>(oldZBounds, newZBounds, false);
      }
      else if(newType == IGeometry::Type::Vertex)
      {
        VertexGeom::Pointer oldVertexGeom = std::dynamic_pointer_cast<VertexGeom>(oldGeom);
        VertexGeom::Pointer newVertexGeom = std::dynamic_pointer_cast<VertexGeom>(newGeom);

        FloatArrayType::Pointer oldVertices = oldVertexGeom->getVertices();
        FloatArrayType::Pointer newVertices = newVertexGeom->getVertices();
        checkDataArray<float>(oldVertices, newVertices, false);
      }
      else if(newType == IGeometry::Type::Edge)
      {
        EdgeGeom::Pointer oldEdgeGeom = std::dynamic_pointer_cast<EdgeGeom>(oldGeom);
        EdgeGeom::Pointer newEdgeGeom = std::dynamic_pointer_cast<EdgeGeom>(newGeom);

        SharedEdgeList::Pointer oldEdges = oldEdgeGeom->getEdges();
        SharedEdgeList::Pointer newEdges = newEdgeGeom->getEdges();
        checkDataArray<MeshIndexType>(oldEdges, newEdges, false);

        FloatArrayType::Pointer oldVertices = oldEdgeGeom->getVertices();
        FloatArrayType::Pointer newVertices = newEdgeGeom->getVertices();
        checkDataArray<float>(oldVertices, newVertices, false);

        ElementDynamicList::Pointer oldElementsContainingVert = oldEdgeGeom->getElementsContainingVert();
        ElementDynamicList::Pointer newElementsContainingVert = newEdgeGeom->getElementsContainingVert();
        checkElementDynamicList(oldElementsContainingVert, newElementsContainingVert);

        ElementDynamicList::Pointer oldElementNeighbors = oldEdgeGeom->getElementNeighbors();
        ElementDynamicList::Pointer newElementNeighbors = newEdgeGeom->getElementNeighbors();
        checkElementDynamicList(oldElementNeighbors, newElementNeighbors);

        FloatArrayType::Pointer oldElementCentroids = oldEdgeGeom->getElementCentroids();
        FloatArrayType::Pointer newElementCentroids = newEdgeGeom->getElementCentroids();
        checkDataArray<float>(oldElementCentroids, newElementCentroids, false);
      }
      else if(newType == IGeometry::Type::Triangle)
      {
        TriangleGeom::Pointer oldTriangleGeom = std::dynamic_pointer_cast<TriangleGeom>(oldGeom);
        TriangleGeom::Pointer newTriangleGeom = std::dynamic_pointer_cast<TriangleGeom>(newGeom);

        SharedTriList::Pointer oldTriangles = oldTriangleGeom->getTriangles();
        SharedTriList::Pointer newTriangles = newTriangleGeom->getTriangles();
        checkDataArray<MeshIndexType>(oldTriangles, newTriangles, false);

        FloatArrayType::Pointer oldVertices = oldTriangleGeom->getVertices();
        FloatArrayType::Pointer newVertices = newTriangleGeom->getVertices();
        checkDataArray<float>(oldVertices, newVertices, false);

        SharedEdgeList::Pointer oldEdges = oldTriangleGeom->getEdges();
        SharedEdgeList::Pointer newEdges = newTriangleGeom->getEdges();
        checkDataArray<MeshIndexType>(oldEdges, newEdges, false);

        SharedEdgeList::Pointer oldUnsharedEdges = oldTriangleGeom->getUnsharedEdges();
        SharedEdgeList::Pointer newUnsharedEdges = newTriangleGeom->getUnsharedEdges();
        checkDataArray<MeshIndexType>(oldUnsharedEdges, newUnsharedEdges, false);

        ElementDynamicList::Pointer oldElementsContainingVert = oldTriangleGeom->getElementsContainingVert();
        ElementDynamicList::Pointer newElementsContainingVert = newTriangleGeom->getElementsContainingVert();
        checkElementDynamicList(oldElementsContainingVert, newElementsContainingVert);

        ElementDynamicList::Pointer oldElementNeighbors = oldTriangleGeom->getElementNeighbors();
        ElementDynamicList::Pointer newElementNeighbors = newTriangleGeom->getElementNeighbors();
        checkElementDynamicList(oldElementNeighbors, newElementNeighbors);

        FloatArrayType::Pointer oldElementCentroids = oldTriangleGeom->getElementCentroids();
        FloatArrayType::Pointer newElementCentroids = newTriangleGeom->getElementCentroids();
        checkDataArray<float>(oldElementCentroids, newElementCentroids, false);
      }
      else if(newType == IGeometry::Type::Quad)
      {
        QuadGeom::Pointer oldQuadGeom = std::dynamic_pointer_cast<QuadGeom>(oldGeom);
        QuadGeom::Pointer newQuadGeom = std::dynamic_pointer_cast<QuadGeom>(newGeom);

        SharedQuadList::Pointer oldQuads = oldQuadGeom->getQuads();
        SharedQuadList::Pointer newQuads = newQuadGeom->getQuads();
        checkDataArray<MeshIndexType>(oldQuads, newQuads, false);

        FloatArrayType::Pointer oldVertices = oldQuadGeom->getVertices();
        FloatArrayType::Pointer newVertices = newQuadGeom->getVertices();
        checkDataArray<float>(oldVertices, newVertices, false);

        SharedEdgeList::Pointer oldEdges = oldQuadGeom->getEdges();
        SharedEdgeList::Pointer newEdges = newQuadGeom->getEdges();
        checkDataArray<MeshIndexType>(oldEdges, newEdges, false);

        SharedEdgeList::Pointer oldUnsharedEdges = oldQuadGeom->getUnsharedEdges();
        SharedEdgeList::Pointer newUnsharedEdges = newQuadGeom->getUnsharedEdges();
        checkDataArray<MeshIndexType>(oldUnsharedEdges, newUnsharedEdges, false);

        ElementDynamicList::Pointer oldElementsContainingVert = oldQuadGeom->getElementsContainingVert();
        ElementDynamicList::Pointer newElementsContainingVert = newQuadGeom->getElementsContainingVert();
        checkElementDynamicList(oldElementsContainingVert, newElementsContainingVert);

        ElementDynamicList::Pointer oldElementNeighbors = oldQuadGeom->getElementNeighbors();
        ElementDynamicList::Pointer newElementNeighbors = newQuadGeom->getElementNeighbors();
        checkElementDynamicList(oldElementNeighbors, newElementNeighbors);

        FloatArrayType::Pointer oldElementCentroids = oldQuadGeom->getElementCentroids();
        FloatArrayType::Pointer newElementCentroids = newQuadGeom->getElementCentroids();
        checkDataArray<float>(oldElementCentroids, newElementCentroids, false);
      }
      else if(newType == IGeometry::Type::Tetrahedral)
      {
        TetrahedralGeom::Pointer oldTetrahedralGeom = std::dynamic_pointer_cast<TetrahedralGeom>(oldGeom);
        TetrahedralGeom::Pointer newTetrahedralGeom = std::dynamic_pointer_cast<TetrahedralGeom>(newGeom);

        SharedTetList::Pointer oldTetrahedra = oldTetrahedralGeom->getTetrahedra();
        SharedTetList::Pointer newTetrahedra = newTetrahedralGeom->getTetrahedra();
        checkDataArray<MeshIndexType>(oldTetrahedra, newTetrahedra, false);

        FloatArrayType::Pointer oldVertices = oldTetrahedralGeom->getVertices();
        FloatArrayType::Pointer newVertices = newTetrahedralGeom->getVertices();
        checkDataArray<float>(oldVertices, newVertices, false);

        SharedEdgeList::Pointer oldEdges = oldTetrahedralGeom->getEdges();
        SharedEdgeList::Pointer newEdges = newTetrahedralGeom->getEdges();
        checkDataArray<MeshIndexType>(oldEdges, newEdges, false);

        SharedEdgeList::Pointer oldUnsharedEdges = oldTetrahedralGeom->getUnsharedEdges();
        SharedEdgeList::Pointer newUnsharedEdges = newTetrahedralGeom->getUnsharedEdges();
        checkDataArray<MeshIndexType>(oldUnsharedEdges, newUnsharedEdges, false);

        SharedTriList::Pointer oldTriangles = oldTetrahedralGeom->getTriangles();
        SharedTriList::Pointer newTriangles = newTetrahedralGeom->getTriangles();
        checkDataArray<MeshIndexType>(oldTriangles, newTriangles, false);

        SharedTriList::Pointer oldUnsharedFaces = oldTetrahedralGeom->getUnsharedFaces();
        SharedTriList::Pointer newUnsharedFaces = newTetrahedralGeom->getUnsharedFaces();
        checkDataArray<MeshIndexType>(oldUnsharedFaces, newUnsharedFaces, false);

        ElementDynamicList::Pointer oldElementsContainingVert = oldTetrahedralGeom->getElementsContainingVert();
        ElementDynamicList::Pointer newElementsContainingVert = newTetrahedralGeom->getElementsContainingVert();
        checkElementDynamicList(oldElementsContainingVert, newElementsContainingVert);

        ElementDynamicList::Pointer oldElementNeighbors = oldTetrahedralGeom->getElementNeighbors();
        ElementDynamicList::Pointer newElementNeighbors = newTetrahedralGeom->getElementNeighbors();
        checkElementDynamicList(oldElementNeighbors, newElementNeighbors);

        FloatArrayType::Pointer oldElementCentroids = oldTetrahedralGeom->getElementCentroids();
        FloatArrayType::Pointer newElementCentroids = newTetrahedralGeom->getElementCentroids();
        checkDataArray<float>(oldElementCentroids, newElementCentroids, false);
      }
      else if(newType == IGeometry::Type::Hexahedral)
      {
        HexahedralGeom::Pointer oldHexahedralGeom = std::dynamic_pointer_cast<HexahedralGeom>(oldGeom);
        HexahedralGeom::Pointer newHexahedralGeom = std::dynamic_pointer_cast<HexahedralGeom>(newGeom);

        SharedHexList::Pointer oldHexahedra = oldHexahedralGeom->getHexahedra();
        SharedHexList::Pointer newHexahedra = newHexahedralGeom->getHexahedra();
        checkDataArray<MeshIndexType>(oldHexahedra, newHexahedra, false);

        FloatArrayType::Pointer oldVertices = oldHexahedralGeom->getVertices();
        FloatArrayType::Pointer newVertices = newHexahedralGeom->getVertices();
        checkDataArray<float>(oldVertices, newVertices, false);

        SharedEdgeList::Pointer oldEdges = oldHexahedralGeom->getEdges();
        SharedEdgeList::Pointer newEdges = newHexahedralGeom->getEdges();
        checkDataArray<MeshIndexType>(oldEdges, newEdges, false);

        SharedEdgeList::Pointer oldUnsharedEdges = oldHexahedralGeom->getUnsharedEdges();
        SharedEdgeList::Pointer newUnsharedEdges = newHexahedralGeom->getUnsharedEdges();
        checkDataArray<MeshIndexType>(oldUnsharedEdges, newUnsharedEdges, false);

        SharedQuadList::Pointer oldQuads = oldHexahedralGeom->getQuads();
        SharedQuadList::Pointer newQuads = newHexahedralGeom->getQuads();
        checkDataArray<MeshIndexType>(oldQuads, newQuads, false);

        SharedQuadList::Pointer oldUnsharedFaces = oldHexahedralGeom->getUnsharedFaces();
        SharedQuadList::Pointer newUnsharedFaces = newHexahedralGeom->getUnsharedFaces();
        checkDataArray<MeshIndexType>(oldUnsharedFaces, newUnsharedFaces, false);

        ElementDynamicList::Pointer oldElementsContainingVert = oldHexahedralGeom->getElementsContainingVert();
        ElementDynamicList::Pointer newElementsContainingVert = newHexahedralGeom->getElementsContainingVert();
        checkElementDynamicList(oldElementsContainingVert, newElementsContainingVert);

        ElementDynamicList::Pointer oldElementNeighbors = oldHexahedralGeom->getElementNeighbors();
        ElementDynamicList::Pointer newElementNeighbors = newHexahedralGeom->getElementNeighbors();
        checkElementDynamicList(oldElementNeighbors, newElementNeighbors);

        FloatArrayType::Pointer oldElementCentroids = oldHexahedralGeom->getElementCentroids();
        FloatArrayType::Pointer newElementCentroids = newHexahedralGeom->getElementCentroids();
        checkDataArray<float>(oldElementCentroids, newElementCentroids, false);
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void checkDataContainer(DataContainer::Pointer oldDC, DataContainer::Pointer newDC)
  {
    IGeometry::Pointer oldGeom = oldDC->getGeometry();
    IGeometry::Pointer newGeom = oldDC->getGeometry();

    checkGeometry(oldGeom, newGeom);

    DREAM3D_REQUIRE_EQUAL(newDC->getNumAttributeMatrices(), oldDC->getNumAttributeMatrices())

    QList<QString> oldMatrixNames = oldDC->getAttributeMatrixNames();
    QList<QString> newMatrixNames = oldDC->getAttributeMatrixNames();

    bool sameArrayNames = (newMatrixNames == oldMatrixNames);
    DREAM3D_REQUIRE_EQUAL(sameArrayNames, true)

    // DataContainer::Container_t oldAttrMatrices = oldDC->getAttributeMatrices();
    // DataContainer::Container_t newAttrMatrices = newDC->getAttributeMatrices();

    for(int i = 0; i < newMatrixNames.size(); i++)
    {
      AttributeMatrix::Pointer oldAM = *oldDC->find(oldMatrixNames[i]);
      AttributeMatrix::Pointer newAM = *newDC->find(newMatrixNames[i]);
      checkAttributeMatrix(oldAM, newAM, false);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCase(DataContainerArray::Pointer dca, DataArrayPath dap, int objectToCopy, const QString& copyName)
  {
    // Create Filter
    std::cout << "  TestCase: " << dca->getName().toStdString() << " " << dap.serialize("/").toStdString() << " " << copyName.toStdString();

    if(dca->getDataContainer(dap)->getGeometry().get() != nullptr)
    {
      std::cout << "   " << dca->getDataContainer(dap)->getGeometry()->getName().toStdString();
    }
    std::cout << std::endl;
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer extractAttributeArraysFromGeometry = filterFactory->create();

    extractAttributeArraysFromGeometry->setDataContainerArray(dca);

    // Setup Filter

    QVariant var;

    var.setValue(objectToCopy);
    bool propWasSet = extractAttributeArraysFromGeometry->setProperty("ObjectToCopy", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(copyName);
    propWasSet = extractAttributeArraysFromGeometry->setProperty("CopiedObjectName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    if(objectToCopy == 0) // DataContainer
    {
      var.setValue(dap);
      propWasSet = extractAttributeArraysFromGeometry->setProperty("DataContainerToCopy", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)
    }
    else if(objectToCopy == 1) // AttributeMatrix
    {
      var.setValue(dap);
      propWasSet = extractAttributeArraysFromGeometry->setProperty("AttributeMatrixToCopy", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)
    }
    else if(objectToCopy == 2) // DataArray
    {
      var.setValue(dap);
      propWasSet = extractAttributeArraysFromGeometry->setProperty("AttributeArrayToCopy", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)
    }

    // Run Filter

    extractAttributeArraysFromGeometry->execute();
    DREAM3D_REQUIRED(extractAttributeArraysFromGeometry->getErrorCode(), >=, 0);

    // Check Filter Results

    if(objectToCopy == 0) // DataContainer
    {
      DataContainer::Pointer dc = dca->getDataContainer(dap);
      DataContainer::Pointer copiedDC = dca->getDataContainer(copyName);

      checkDataContainer(dc, copiedDC);

      // Remove copied DataContainer

      dca->removeDataContainer(copyName);
    }
    else if(objectToCopy == 1) // AttributeMatrix
    {
      DataContainer::Pointer dc = dca->getDataContainer(dap.getDataContainerName());
      AttributeMatrix::Pointer am = dc->getAttributeMatrix(dap);

      QList<QString> names = dc->getAttributeMatrixNames();

      DREAM3D_REQUIRE_EQUAL(names.contains(copyName), true)

      AttributeMatrix::Pointer copiedAM = dc->getAttributeMatrix(copyName);

      checkAttributeMatrix(am, copiedAM, true);

      // Remove copied AttributeMatrix

      dc->removeAttributeMatrix(copyName);
    }
    else if(objectToCopy == 2) // DataArray
    {
      DataContainer::Pointer dc = dca->getDataContainer(dap.getDataContainerName());
      AttributeMatrix::Pointer am = dc->getAttributeMatrix(dap.getAttributeMatrixName());
      IDataArray::Pointer da = am->getAttributeArray(dap.getDataArrayName());

      QList<QString> names = am->getAttributeArrayNames();

      DREAM3D_REQUIRE_EQUAL(names.contains(copyName), true)

      IDataArray::Pointer copiedDA = am->getAttributeArray(copyName);

      QString oldType = da->getTypeAsString();
      QString newType = copiedDA->getTypeAsString();
      DREAM3D_REQUIRE_EQUAL(newType, oldType)

      if(newType == "int8_t")
      {
        checkDataArray<int8_t>(da, copiedDA, true);
      }
      else if(newType == "uint8_t")
      {
        checkDataArray<uint8_t>(da, copiedDA, true);
      }
      else if(newType == "int16_t")
      {
        checkDataArray<int16_t>(da, copiedDA, true);
      }
      else if(newType == "uint16_t")
      {
        checkDataArray<uint16_t>(da, copiedDA, true);
      }
      else if(newType == "int32_t")
      {
        checkDataArray<int32_t>(da, copiedDA, true);
      }
      else if(newType == "uint32_t")
      {
        checkDataArray<uint32_t>(da, copiedDA, true);
      }
      else if(newType == "int64_t")
      {
        checkDataArray<int64_t>(da, copiedDA, true);
      }
      else if(newType == "uint64_t")
      {
        checkDataArray<uint64_t>(da, copiedDA, true);
      }
      else if(newType == "float")
      {
        checkDataArray<float>(da, copiedDA, true);
      }
      else if(newType == "double")
      {
        checkDataArray<double>(da, copiedDA, true);
      }

      // Remove copied DataArray

      am->removeAttributeArray(copyName);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCopyObjectTest()
  {
    // Names and constants

    static const QString k_DataContainerName("DataContainer");
    static const QString k_EmptyDataContainerName("EmptyDataContainer");
    static const QString k_EmptyAttributeMatrixName("EmptyAttributeMatrix");
    static const QString k_AttributeMatrix1Name("AttributeMatrix1");
    static const QString k_AttributeMatrix2Name("AttributeMatrix2");
    static const QString k_EmptyDataArrayName("EmptyDataArray");
    static const QString k_CopiedObjectName("CopiedObject");

    static const QString k_VerticesDAName("VertexCoordinates");
    static const QString k_ElementListDAName("Connectivity");

    static const QString k_XBoundsDAName("XBounds");
    static const QString k_YBoundsDAName("YBounds");
    static const QString k_ZBoundsDAName("ZBounds");

    static const QString k_uint8ArrayName("uint8Array");
    static const QString k_int8ArrayName("int8Array");
    static const QString k_uint16ArrayName("uint16Array");
    static const QString k_int16ArrayName("int16Array");
    static const QString k_uint32ArrayName("uint32Array");
    static const QString k_int32ArrayName("int32Array");
    static const QString k_uint64ArrayName("uint64Array");
    static const QString k_int64ArrayName("int64Array");
    static const QString k_floatArrayName("floatDataArray");
    static const QString k_doubleArrayName("doubleDataArray");

    // Create DataContainerArray

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create DataContainer

    DataContainer::Pointer dc = DataContainer::New(k_DataContainerName);
    dca->addOrReplaceDataContainer(dc);

    DataContainer::Pointer emptyDC = DataContainer::New(k_EmptyDataContainerName);
    dca->addOrReplaceDataContainer(emptyDC);

    // Create AttributeMatrix

    AttributeMatrix::Pointer am1 = AttributeMatrix::New(m_Dims8, k_AttributeMatrix1Name, AttributeMatrix::Type::Vertex);
    dc->addOrReplaceAttributeMatrix(am1);

    AttributeMatrix::Pointer am2 = AttributeMatrix::New(m_Dims8, k_AttributeMatrix2Name, AttributeMatrix::Type::Cell);
    dc->addOrReplaceAttributeMatrix(am2);

    AttributeMatrix::Pointer emptyAM = AttributeMatrix::New(m_Dims1, k_EmptyAttributeMatrixName, AttributeMatrix::Type::Generic);
    dc->addOrReplaceAttributeMatrix(emptyAM);

    // Create DataArray

    DataArray<uint8_t>::Pointer daUint8 = createDataArray<uint8_t>(k_uint8ArrayName, m_Dims8, m_Dims4);
    DataArray<int8_t>::Pointer daInt8 = createDataArray<int8_t>(k_int8ArrayName, m_Dims8, m_Dims4);
    DataArray<uint16_t>::Pointer daUint16 = createDataArray<uint16_t>(k_uint16ArrayName, m_Dims8, m_Dims4);
    DataArray<int16_t>::Pointer daInt16 = createDataArray<int16_t>(k_int16ArrayName, m_Dims8, m_Dims4);
    DataArray<uint32_t>::Pointer daUint32 = createDataArray<uint32_t>(k_uint32ArrayName, m_Dims8, m_Dims4);
    DataArray<int32_t>::Pointer daInt32 = createDataArray<int32_t>(k_int32ArrayName, m_Dims8, m_Dims4);
    DataArray<uint64_t>::Pointer daUint64 = createDataArray<uint64_t>(k_uint64ArrayName, m_Dims8, m_Dims4);
    DataArray<int64_t>::Pointer daInt64 = createDataArray<int64_t>(k_int64ArrayName, m_Dims8, m_Dims4);
    DataArray<float>::Pointer daFloat = createDataArray<float>(k_floatArrayName, m_Dims8, m_Dims4);
    DataArray<double>::Pointer daDouble = createDataArray<double>(k_doubleArrayName, m_Dims8, m_Dims4);

    // Create Geometry

    // Image

    ImageGeom::Pointer imageGeom = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    imageGeom->setDimensions(SizeVec3Type(5, 5, 5));
    imageGeom->setSpacing(FloatVec3Type(5, 5, 5));
    imageGeom->setOrigin(FloatVec3Type(5, 5, 5));
    imageGeom->setName("Image Geom");

    // RectGrid

    DataArray<float>::Pointer daXBounds = createDataArray<float>(k_XBoundsDAName, m_Dims4, m_Dims1);
    DataArray<float>::Pointer daYBounds = createDataArray<float>(k_YBoundsDAName, m_Dims4, m_Dims1);
    DataArray<float>::Pointer daZBounds = createDataArray<float>(k_ZBoundsDAName, m_Dims4, m_Dims1);

    RectGridGeom::Pointer rectGridGeom = RectGridGeom::CreateGeometry(SIMPL::Geometry::RectGridGeometry);
    rectGridGeom->setXBounds(daXBounds);
    rectGridGeom->setYBounds(daYBounds);
    rectGridGeom->setZBounds(daZBounds);
    rectGridGeom->setName("rectGridGeom");

    // Vertex

    std::vector<std::vector<float>> vertices = {{1.0f, 1.0f, 0.0f}, {3.0f, 1.0f, 0.0f}};

    DataArray<float>::Pointer daVert = createDataArray<float>(k_VerticesDAName, vertices, m_Dims2, m_Dims3);

    VertexGeom::Pointer vertexGeom = VertexGeom::CreateGeometry(daVert, SIMPL::Geometry::VertexGeometry);
    vertexGeom->setName("vertexGeom");

    // Edge

    vertices = {{1.0f, 1.0f, 0.0f}, {3.0f, 1.0f, 0.0f}};
    std::vector<std::vector<MeshIndexType>> elements = {{0, 1}};

    DataArray<float>::Pointer daEdgeVert = createDataArray<float>(k_VerticesDAName, vertices, m_Dims2, m_Dims3);
    SharedEdgeList::Pointer daEdgeList = createDataArray<MeshIndexType>(k_ElementListDAName, elements, m_Dims1, m_Dims2);

    EdgeGeom::Pointer edgeGeom = EdgeGeom::CreateGeometry(daEdgeList, daEdgeVert, SIMPL::Geometry::EdgeGeometry);

    // Triangle

    vertices = {{1.0f, 1.0f, 0.0f}, {3.0f, 1.0f, 0.0f}, {2.0f, 3.0f, 0.0f}};
    elements = {{0, 1, 2}};

    DataArray<float>::Pointer daTriVert = createDataArray<float>(k_VerticesDAName, vertices, m_Dims3, m_Dims3);
    SharedTriList::Pointer daTriList = createDataArray<MeshIndexType>(k_ElementListDAName, elements, m_Dims1, m_Dims3);

    TriangleGeom::Pointer triGeom = TriangleGeom::CreateGeometry(daTriList, daTriVert, SIMPL::Geometry::TriangleGeometry);
    triGeom->setName("triGeom");

    // Quad

    vertices = {{1.0f, 1.0f, 0.0f}, {3.0f, 1.0f, 0.0f}, {2.0f, 3.0f, 0.0f}, {2.0f, 2.0f, 0.0f}};
    elements = {{0, 1, 2, 3}};

    DataArray<float>::Pointer daQuadVert = createDataArray<float>(k_VerticesDAName, vertices, m_Dims4, m_Dims3);
    SharedQuadList::Pointer daQuadList = createDataArray<MeshIndexType>(k_ElementListDAName, elements, m_Dims1, m_Dims4);

    QuadGeom::Pointer quadGeom = QuadGeom::CreateGeometry(daQuadList, daQuadVert, SIMPL::Geometry::QuadGeometry);
    quadGeom->setName("quadGeom");

    // Tetrahedral

    vertices = {{1.0f, 1.0f, 1.55f}, {3.0f, 1.0f, 1.55f}, {2.0f, 3.0f, 1.55f}, {2.0f, 2.0f, 3.55f}};

    elements = {{0, 1, 2, 3}};

    DataArray<float>::Pointer daTetVert = createDataArray<float>(k_VerticesDAName, vertices, m_Dims4, m_Dims3);
    SharedTetList::Pointer daTetList = createDataArray<MeshIndexType>(k_ElementListDAName, elements, m_Dims1, m_Dims4);

    TetrahedralGeom::Pointer tetGeom = TetrahedralGeom::CreateGeometry(daTetList, daTetVert, SIMPL::Geometry::TetrahedralGeometry);
    tetGeom->setName("tetGeom");

    // Hexahedral

    vertices = {{1.0f, 1.0f, 1.55f}, {3.0f, 1.0f, 1.55f}, {2.0f, 3.0f, 1.55f}, {2.0f, 2.0f, 3.55f}, {2.5f, 1.0f, 1.55f}, {4.3f, 1.0f, 1.55f}, {5.1f, 3.0f, 1.55f}, {7.63f, 2.0f, 3.55f}};

    elements = {{0, 1, 2, 3, 4, 5, 6, 7}};

    DataArray<float>::Pointer daHexVert = createDataArray<float>(k_VerticesDAName, vertices, m_Dims8, m_Dims3);
    SharedHexList::Pointer daHexList = createDataArray<MeshIndexType>(k_ElementListDAName, elements, m_Dims1, m_Dims8);

    HexahedralGeom::Pointer hexGeom = HexahedralGeom::CreateGeometry(daHexList, daHexVert, SIMPL::Geometry::HexahedralGeometry);
    hexGeom->setName("hexGeom");

    // Add DataArrays to AttributeMatrices

    am1->insertOrAssign(daUint8);
    am1->insertOrAssign(daInt8);
    am1->insertOrAssign(daUint16);
    am1->insertOrAssign(daInt16);
    am1->insertOrAssign(daUint32);

    am2->insertOrAssign(daInt32);
    am2->insertOrAssign(daUint64);
    am2->insertOrAssign(daInt64);
    am2->insertOrAssign(daFloat);
    am2->insertOrAssign(daDouble);

    dc->setGeometry(hexGeom);

    // Test Copy DataContainers

    TestCase(dca, DataArrayPath(k_DataContainerName), 0, k_CopiedObjectName);

    TestCase(dca, DataArrayPath(k_EmptyDataContainerName), 0, k_CopiedObjectName);

    emptyDC->setGeometry(imageGeom);
    TestCase(dca, DataArrayPath(k_EmptyDataContainerName), 0, k_CopiedObjectName);

    emptyDC->setGeometry(rectGridGeom);
    TestCase(dca, DataArrayPath(k_EmptyDataContainerName), 0, k_CopiedObjectName);

    emptyDC->setGeometry(vertexGeom);
    TestCase(dca, DataArrayPath(k_EmptyDataContainerName), 0, k_CopiedObjectName);

    emptyDC->setGeometry(edgeGeom);
    TestCase(dca, DataArrayPath(k_EmptyDataContainerName), 0, k_CopiedObjectName);

    emptyDC->setGeometry(triGeom);
    TestCase(dca, DataArrayPath(k_EmptyDataContainerName), 0, k_CopiedObjectName);

    emptyDC->setGeometry(quadGeom);
    TestCase(dca, DataArrayPath(k_EmptyDataContainerName), 0, k_CopiedObjectName);

    emptyDC->setGeometry(tetGeom);
    TestCase(dca, DataArrayPath(k_EmptyDataContainerName), 0, k_CopiedObjectName);

    hexGeom->findEdges();
    hexGeom->findElementCentroids();
    hexGeom->findElementNeighbors();
    hexGeom->findElementsContainingVert();
    hexGeom->findElementSizes();
    hexGeom->findFaces();
    hexGeom->findUnsharedEdges();
    hexGeom->findUnsharedFaces();
    emptyDC->setGeometry(hexGeom);
    TestCase(dca, DataArrayPath(k_EmptyDataContainerName), 0, k_CopiedObjectName);

    // Test Copy AttributeMatrices

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix1Name, ""), 1, k_CopiedObjectName);

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix2Name, ""), 1, k_CopiedObjectName);

    emptyDC->addOrReplaceAttributeMatrix(emptyAM);
    TestCase(dca, DataArrayPath(k_EmptyDataContainerName, k_EmptyAttributeMatrixName, ""), 1, k_CopiedObjectName);

    // Test Copy DataArrays

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix1Name, k_uint8ArrayName), 2, k_CopiedObjectName);

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix1Name, k_int8ArrayName), 2, k_CopiedObjectName);

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix1Name, k_uint16ArrayName), 2, k_CopiedObjectName);

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix1Name, k_int16ArrayName), 2, k_CopiedObjectName);

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix1Name, k_uint32ArrayName), 2, k_CopiedObjectName);

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix2Name, k_int32ArrayName), 2, k_CopiedObjectName);

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix2Name, k_uint64ArrayName), 2, k_CopiedObjectName);

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix2Name, k_int64ArrayName), 2, k_CopiedObjectName);

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix2Name, k_floatArrayName), 2, k_CopiedObjectName);

    TestCase(dca, DataArrayPath(k_DataContainerName, k_AttributeMatrix2Name, k_doubleArrayName), 2, k_CopiedObjectName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#### CopyObjectTest Starting ####" << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestCopyObjectTest())
  }

private:
  QString m_FilterName = QString("CopyObject");
  std::vector<size_t> m_Dims1 = {1};
  std::vector<size_t> m_Dims2 = {2};
  std::vector<size_t> m_Dims3 = {3};
  std::vector<size_t> m_Dims4 = {4};
  std::vector<size_t> m_Dims8 = {8};
};
