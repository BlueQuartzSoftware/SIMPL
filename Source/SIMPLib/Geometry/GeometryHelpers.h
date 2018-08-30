/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <math.h>

#include <map>
#include <set>

#include <QtCore/QString>

#include "H5Support/QH5Lite.h"
#include "H5Support/H5ScopedErrorHandler.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Geometry/IGeometry.h"
#include "SIMPLib/HDF5/H5DataArrayReader.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/MatrixMath.h"

/**
* @brief This file contains a namespace with classes for manipulating IGeometry objects
*/
namespace GeometryHelpers
{

/**
 * @brief The Translation class provides a short "translation" to convert between
 * enumeration values and a string representation
 */
class Translation
{
public:
  ~Translation();

    /**
     * @brief Translate
     * @param t
     * @return
     */
  static IGeometry::Type StringToType(const QString& str);

  /**
  * @brief StringFromGeometryType Returns a String from a IGeometry::Type enumeration value
  * @param t The value to translate
  * @return A string representation of the value
  */
  static QString TypeToString(IGeometry::Type t);

protected:
  Translation();
};

/**
 * @brief The GeomIO class
 */
class GeomIO
{
public:
  GeomIO() = default;
  virtual ~GeomIO() = default;

  /**
   * @brief ReadMeshFromHDF5
   * @param listName
   * @param parentId
   * @param preflight
   * @return
   */
  template <typename ListType> static typename ListType::Pointer ReadListFromHDF5(const QString& listName, hid_t parentId, bool preflight, herr_t& err)
  {
    QVector<hsize_t> dims;
    H5T_class_t type_class;
    size_t type_size;
    IDataArray::Pointer mesh = IDataArray::NullPointer();
    H5ScopedErrorHandler errorHandler;
    err = QH5Lite::getDatasetInfo(parentId, listName, dims, type_class, type_size);
    if(err < 0)
    {
      err = -2;
      return ListType::NullPointer();
    }
    mesh = H5DataArrayReader::ReadIDataArray(parentId, listName, preflight);
    if(nullptr == mesh.get())
    {
      err = -1;
      return ListType::NullPointer();
    }
    return std::dynamic_pointer_cast<ListType>(mesh);
  }

  /**
   * @brief ReadMetaDataFromHDF5
   * @param parentId
   * @param geometry
   * @return
   */
  static int ReadMetaDataFromHDF5(hid_t parentId, IGeometry::Pointer geometry)
  {
    herr_t err = 0;
    unsigned int spatialDims = 0;
    QString geomName = "";
    err = QH5Lite::readScalarAttribute(parentId, SIMPL::Geometry::Geometry, SIMPL::Geometry::SpatialDimensionality, spatialDims);
    if(err < 0)
    {
      return err;
    }
    err = QH5Lite::readStringAttribute(parentId, SIMPL::Geometry::Geometry, SIMPL::Geometry::GeometryName, geomName);
    if(err < 0)
    {
      return err;
    }
    geometry->setSpatialDimensionality(spatialDims);
    geometry->setName(geomName);

    return 1;
  }

  /**
   * @brief WriteListToHDF5
   * @param parentId
   * @param list
   * @return
   */
  static int WriteListToHDF5(hid_t parentId, IDataArray::Pointer list)
  {
    herr_t err = 0;
    if(list->getNumberOfTuples() == 0)
    {
      return err;
    }
    QVector<size_t> tDims(1, list->getNumberOfTuples());
    err = list->writeH5Data(parentId, tDims);
    return err;
  }

  /**
   * @brief ReadDynamicListFromHDF5
   * @param dynamicListName
   * @param parentId
   * @param numElems
   * @param preflight
   * @return
   */
  template <typename T, typename K>
  static typename DynamicListArray<T, K>::Pointer ReadDynamicListFromHDF5(const QString& dynamicListName, hid_t parentId, size_t numElems, bool preflight, herr_t& err)
  {
    QVector<hsize_t> dims;
    H5T_class_t type_class;
    size_t type_size;
    typename DynamicListArray<T, K>::Pointer dynamicList = DynamicListArray<T, K>::New();
    H5ScopedErrorHandler errorHandler;
    err = QH5Lite::getDatasetInfo(parentId, dynamicListName, dims, type_class, type_size);
    if(err < 0)
    {
      err = -2;
      return dynamicList = DynamicListArray<T, K>::NullPointer();
    }
    if(preflight == true)
    {
      if(err < 0)
      {
        return dynamicList = DynamicListArray<T, K>::NullPointer();
      }
    }
    else
    {
      std::vector<uint8_t> buffer;
      err = QH5Lite::readVectorDataset(parentId, dynamicListName, buffer);
      if(err < 0)
      {
        return dynamicList = DynamicListArray<T, K>::NullPointer();
      }
      dynamicList->deserializeLinks(buffer, numElems);
    }

    return dynamicList;
  }

  /**
   * @brief WriteDynamicListToHDF5
   * @param parentId
   * @param dynamicList
   * @param numElems
   * @param name
   * @return
   */
  template <typename T, typename K> static int WriteDynamicListToHDF5(hid_t parentId, typename DynamicListArray<T, K>::Pointer dynamicList, size_t numElems, const QString& name)
  {
    herr_t err = 0;
    if(numElems == 0)
    {
      return err;
    }
    int32_t rank = 0;
    hsize_t dims[2] = {0, 2ULL};
    size_t total = 0;
    for(size_t v = 0; v < numElems; ++v)
    {
      total += dynamicList->getNumberOfElements(v);
    }

    size_t totalBytes = numElems * sizeof(T) + total * sizeof(K);

    // Allocate a flat array to copy the data into
    QVector<uint8_t> buffer(totalBytes, 0);
    uint8_t* bufPtr = &(buffer.front());
    size_t offset = 0;

    for(size_t v = 0; v < numElems; ++v)
    {
      T nelems = dynamicList->getNumberOfElements(v);
      K* elems = dynamicList->getElementListPointer(v);
      ::memcpy(bufPtr + offset, &nelems, sizeof(T));
      offset += sizeof(T);
      ::memcpy(bufPtr + offset, elems, nelems * sizeof(K));
      offset += nelems * sizeof(K);
    }

    rank = 1;
    dims[0] = totalBytes;

    err = QH5Lite::writePointerDataset(parentId, name, rank, dims, bufPtr);
    return err;
  }
};

/**
 * @brief The Connectivity class
 */
class Connectivity
{
public:
  Connectivity() = default;
  virtual ~Connectivity() = default;

  /**
   * @brief FindElementsContainingVert
   * @param elemList
   * @param dynamicList
   * @param numVerts
   */
  template <typename T, typename K> static void FindElementsContainingVert(typename DataArray<K>::Pointer elemList, typename DynamicListArray<T, K>::Pointer dynamicList, size_t numVerts)
  {
    size_t numElems = elemList->getNumberOfTuples();
    size_t numVertsPerElem = elemList->getNumberOfComponents();

    // Allocate the basic structures
    QVector<T> linkCount(numVerts, 0);
    size_t elemId = 0;

    // Fill out lists with number of references to cells
    typename DataArray<K>::Pointer linkLocPtr = DataArray<K>::CreateArray(numVerts, "_INTERNAL_USE_ONLY_Vertices");
    linkLocPtr->initializeWithValue(0);
    K* linkLoc = linkLocPtr->getPointer(0);
    K* verts = nullptr;

    // vtkPolyData *pdata = static_cast<vtkPolyData *>(data);
    // Traverse data to determine number of uses of each point
    for(elemId = 0; elemId < numElems; elemId++)
    {
      verts = elemList->getTuplePointer(elemId);
      for(size_t j = 0; j < numVertsPerElem; j++)
      {
        linkCount[verts[j]]++;
      }
    }

    // Now allocate storage for the links
    dynamicList->allocateLists(linkCount);

    for(elemId = 0; elemId < numElems; elemId++)
    {
      verts = elemList->getTuplePointer(elemId);
      for(size_t j = 0; j < numVertsPerElem; j++)
      {
        dynamicList->insertCellReference(verts[j], (linkLoc[verts[j]])++, elemId);
      }
    }
  }

  /**
   * @brief FindElementNeighbors
   * @param elemList
   * @param elemsContainingVert
   * @param dynamicList This should be an empty DynamicListArray object. It is not
   * it <b>WILL</b> be cleared and reallocated.
   * @return
   */
  template <typename T, typename K>
  static int FindElementNeighbors(typename DataArray<K>::Pointer elemList, typename DynamicListArray<T, K>::Pointer elemsContainingVert, typename DynamicListArray<T, K>::Pointer dynamicList,
                                  IGeometry::Type geometryType)
  {
    size_t numElems = elemList->getNumberOfTuples();
    size_t numVertsPerElem = elemList->getNumberOfComponents();
    size_t numSharedVerts = 0;
    QVector<T> linkCount(numElems, 0);
    int err = 0;

    switch(geometryType)
    {
    case IGeometry::Type::Edge: // edges
    {
      numSharedVerts = 1;
      break;
    }
    case IGeometry::Type::Triangle: // triangles
    {
      numSharedVerts = 2;
      break;
    }
    case IGeometry::Type::Quad: // quadrilaterals
    {
      numSharedVerts = 2;
      break;
    }
    case IGeometry::Type::Tetrahedral: // tetrahedra
    {
      numSharedVerts = 3;
      break;
    }
    case IGeometry::Type::Hexahedral: // hexahedra
    {
      numSharedVerts = 4;
      break;
    }
    default:
      numSharedVerts = 0;
      break;
    }

    if(numSharedVerts == 0)
    {
      return -1;
    }

    dynamicList->allocateLists(linkCount);

    // Allocate an array of bools that we use each iteration so that we don't put duplicates into the array
    typename DataArray<bool>::Pointer visitedPtr = DataArray<bool>::CreateArray(numElems, "_INTERNAL_USE_ONLY_Visited");
    visitedPtr->initializeWithValue(false);
    bool* visited = visitedPtr->getPointer(0);

    // Reuse this vector for each loop. Avoids re-allocating the memory each time through the loop
    QVector<K> loop_neighbors(32, 0);

    // Build up the element adjacency list now that we have the element links
    for(size_t t = 0; t < numElems; ++t)
    {
      //   qDebug() << "Analyzing Cell " << t << "\n";
      K* seedElem = elemList->getTuplePointer(t);
      for(size_t v = 0; v < numVertsPerElem; ++v)
      {
        //   qDebug() << " vert " << v << "\n";
        T nEs = elemsContainingVert->getNumberOfElements(seedElem[v]);
        K* vertIdxs = elemsContainingVert->getElementListPointer(seedElem[v]);

        for(T vt = 0; vt < nEs; ++vt)
        {
          if(vertIdxs[vt] == static_cast<K>(t))
          {
            continue;
          } // This is the same element as our "source"
          if(visited[vertIdxs[vt]] == true)
          {
            continue;
          } // We already added this element so loop again
          //      qDebug() << "   Comparing Element " << vertIdxs[vt] << "\n";
          K* vertCell = elemList->getTuplePointer(vertIdxs[vt]);
          size_t vCount = 0;
          // Loop over all the vertex indices of this element and try to match numSharedVerts of them to the current loop element
          // If there is numSharedVerts match then that element is a neighbor of the source. If there are more than numVertsPerElem
          // matches then there is a real problem with the mesh and the program is going to return an error.
          for(size_t i = 0; i < numVertsPerElem; i++)
          {
            for(size_t j = 0; j < numVertsPerElem; j++)
            {
              if(seedElem[i] == vertCell[j])
              {
                vCount++;
              }
            }
          }

          // So if our vertex match count is numSharedVerts and we have not visited the element in question then add this element index
          // into the list of vertex indices as neighbors for the source element.
          if(vCount == numSharedVerts)
          {
            // qDebug() << "       Neighbor: " << vertIdxs[vt] << "\n";
            // Use the current count of neighbors as the index
            // into the loop_neighbors vector and place the value of the vertex element at that index
            loop_neighbors[linkCount[t]] = vertIdxs[vt];
            linkCount[t]++; // Increment the count for the next time through
            if(linkCount[t] >= loop_neighbors.size())
            {
              loop_neighbors.resize(loop_neighbors.size() + 10);
            }
            visited[vertIdxs[vt]] = true; // Set this element as visited so we do NOT add it again
          }
        }
      }
      // Reset all the visited cell indexs back to false (zero)
      for(int64_t k = 0; k < linkCount[t]; ++k)
      {
        visited[loop_neighbors[k]] = false;
      }
      // Allocate the array storage for the current edge to hold its vertex list
      dynamicList->setElementList(t, linkCount[t], &(loop_neighbors[0]));
    }

    return err;
  }

  /**
   * @brief Find2DElementEdges
   * @param elemList
   * @param edgeList
   */
  template <typename T> static void Find2DElementEdges(typename DataArray<T>::Pointer elemList, typename DataArray<T>::Pointer edgeList)
  {
    size_t numElems = elemList->getNumberOfTuples();
    size_t numVertsPerElem = elemList->getNumberOfComponents();
    T v0 = 0;
    T v1 = 0;

    std::pair<T, T> edge;
    std::set<std::pair<T, T>> edgeSet;

    for(size_t i = 0; i < numElems; i++)
    {
      T* verts = elemList->getTuplePointer(i);

      for(size_t j = 0; j < numVertsPerElem; j++)
      {
        if(j == (numVertsPerElem - 1))
        {
          if(verts[j] > verts[0])
          {
            v0 = verts[0];
            v1 = verts[j];
          }
          else
          {
            v0 = verts[j];
            v1 = verts[0];
          }
        }
        else
        {
          if(verts[j] > verts[j + 1])
          {
            v0 = verts[j + 1];
            v1 = verts[j];
          }
          else
          {
            v0 = verts[j];
            v1 = verts[j + 1];
          }
        }
        edge = std::make_pair(v0, v1);
        edgeSet.insert(edge);
      }
    }

    typename std::set<std::pair<T, T>>::iterator setIter;
    edgeList->resize(edgeSet.size());
    T* uEdges = edgeList->getPointer(0);
    T index = 0;

    for(setIter = edgeSet.begin(); setIter != edgeSet.end(); ++setIter)
    {
      uEdges[2 * index] = (*setIter).first;
      uEdges[2 * index + 1] = (*setIter).second;
      ++index;
    }
  }

  /**
   * @brief FindTetEdges
   * @param tetList
   * @param edgeList
   */
  template <typename T> static void FindTetEdges(typename DataArray<T>::Pointer tetList, typename DataArray<T>::Pointer edgeList)
  {
    size_t numElems = tetList->getNumberOfTuples();

    std::pair<T, T> edge;
    std::set<std::pair<T, T>> edgeSet;

    for(size_t i = 0; i < numElems; i++)
    {
      T* verts = tetList->getTuplePointer(i);

      std::vector<T> edge0 = {verts[0], verts[1]};
      std::vector<T> edge1 = {verts[0], verts[2]};
      std::vector<T> edge2 = {verts[1], verts[2]};
      std::vector<T> edge3 = {verts[0], verts[3]};
      std::vector<T> edge4 = {verts[1], verts[3]};
      std::vector<T> edge5 = {verts[2], verts[3]};
      std::list<std::vector<T>> edgeList = {edge0, edge1, edge2, edge3, edge4, edge5};

      for(auto&& uEdge : edgeList)
      {
        std::sort(uEdge.begin(), uEdge.end());
        edge = std::make_pair(uEdge[0], uEdge[1]);
        edgeSet.insert(edge);
      }
    }

    typename std::set<std::pair<T, T>>::iterator setIter;
    edgeList->resize(edgeSet.size());
    T* uEdges = edgeList->getPointer(0);
    T index = 0;

    for(setIter = edgeSet.begin(); setIter != edgeSet.end(); ++setIter)
    {
      uEdges[2 * index] = (*setIter).first;
      uEdges[2 * index + 1] = (*setIter).second;
      ++index;
    }
  }

  /**
  * @brief FindHexEdges
  * @param hexList
  * @param edgeList
  */
  template <typename T> static void FindHexEdges(typename DataArray<T>::Pointer hexList, typename DataArray<T>::Pointer edgeList)
  {
    size_t numElems = hexList->getNumberOfTuples();

    std::pair<T, T> edge;
    std::set<std::pair<T, T>> edgeSet;

    for(size_t i = 0; i < numElems; i++)
    {
      T* verts = hexList->getTuplePointer(i);

      std::vector<T> edge0 = {verts[0], verts[1]};
      std::vector<T> edge1 = {verts[1], verts[2]};
      std::vector<T> edge2 = {verts[2], verts[3]};
      std::vector<T> edge3 = {verts[3], verts[0]};

      std::vector<T> edge4 = {verts[0], verts[4]};
      std::vector<T> edge5 = {verts[1], verts[5]};
      std::vector<T> edge6 = {verts[2], verts[6]};
      std::vector<T> edge7 = {verts[3], verts[7]};

      std::vector<T> edge8 =  {verts[4], verts[5]};
      std::vector<T> edge9 =  {verts[5], verts[6]};
      std::vector<T> edge10 = {verts[6], verts[7]};
      std::vector<T> edge11 = {verts[7], verts[4]};

      std::list<std::vector<T>> edgeList = {edge0, edge1, edge2, edge3, edge4, edge5,
                                            edge6, edge7, edge8, edge9, edge10, edge11};

      for(auto&& uEdge : edgeList)
      {
        std::sort(uEdge.begin(), uEdge.end());
        edge = std::make_pair(uEdge[0], uEdge[1]);
        edgeSet.insert(edge);
      }
    }

    typename std::set<std::pair<T, T>>::iterator setIter;
    edgeList->resize(edgeSet.size());
    T* uEdges = edgeList->getPointer(0);
    T index = 0;

    for(setIter = edgeSet.begin(); setIter != edgeSet.end(); ++setIter)
    {
      uEdges[2 * index] = (*setIter).first;
      uEdges[2 * index + 1] = (*setIter).second;
      ++index;
    }
  }

  /**
   * @brief FindTetFaces
   * @param tetList
   * @param edgeList
   */
  template <typename T> static void FindTetFaces(typename DataArray<T>::Pointer tetList, typename DataArray<T>::Pointer faceList)
  {
    size_t numElems = tetList->getNumberOfTuples();

    std::tuple<T, T, T> face;
    std::set<std::tuple<T, T, T>> faceSet;

    for(size_t i = 0; i < numElems; i++)
    {
      T* verts = tetList->getTuplePointer(i);

      std::vector<T> tri0 = {verts[0], verts[1], verts[2]};
      std::vector<T> tri1 = {verts[1], verts[2], verts[3]};
      std::vector<T> tri2 = {verts[0], verts[2], verts[3]};
      std::vector<T> tri3 = {verts[0], verts[1], verts[3]};
      std::list<std::vector<T>> triList = {tri0, tri1, tri2, tri3};

      for(auto&& tri : triList)
      {
        std::sort(tri.begin(), tri.end());
        face = std::make_tuple(tri[0], tri[1], tri[2]);
        faceSet.insert(face);
      }
    }

    typename std::set<std::tuple<T, T, T>>::iterator setIter;
    faceList->resize(faceSet.size());
    T* uFaces = faceList->getPointer(0);
    T index = 0;

    for(setIter = faceSet.begin(); setIter != faceSet.end(); ++setIter)
    {
      uFaces[3 * index] = std::get<0>(*setIter);
      uFaces[3 * index + 1] = std::get<1>(*setIter);
      uFaces[3 * index + 2] = std::get<2>(*setIter);
      ++index;
    }
  }

  /**
  * @brief FindHexFaces
  * @param hexList
  * @param edgeList
  */
  template <typename T> static void FindHexFaces(typename DataArray<T>::Pointer hexList, typename DataArray<T>::Pointer faceList)
  {
    size_t numElems = hexList->getNumberOfTuples();

    std::tuple<T, T, T, T> face;
    std::set<std::tuple<T, T, T, T>> faceSet;

    for(size_t i = 0; i < numElems; i++)
    {
      T* verts = hexList->getTuplePointer(i);

      std::vector<T> quad0 = {verts[0], verts[1], verts[5], verts[4]};
      std::vector<T> quad1 = {verts[1], verts[2], verts[6], verts[5]};
      std::vector<T> quad2 = {verts[2], verts[3], verts[7], verts[6]};
      std::vector<T> quad3 = {verts[3], verts[0], verts[4], verts[7]};
      std::vector<T> quad4 = {verts[0], verts[1], verts[2], verts[3]};
      std::vector<T> quad5 = {verts[4], verts[5], verts[6], verts[7]};

      std::list<std::vector<T>> quadList = {quad0, quad1, quad2, quad3, quad4, quad5};

      for(auto&& quad : quadList)
      {
        std::sort(quad.begin(), quad.end());
        face = std::make_tuple(quad[0], quad[1], quad[2], quad[3]);
        faceSet.insert(face);
      }
    }

    typename std::set<std::tuple<T, T, T, T>>::iterator setIter;
    faceList->resize(faceSet.size());
    T* uFaces = faceList->getPointer(0);
    T index = 0;

    for(setIter = faceSet.begin(); setIter != faceSet.end(); ++setIter)
    {
      uFaces[4 * index] = std::get<0>(*setIter);
      uFaces[4 * index + 1] = std::get<1>(*setIter);
      uFaces[4 * index + 2] = std::get<2>(*setIter);
      uFaces[4 * index + 3] = std::get<3>(*setIter);
      ++index;
    }
  }

  /**
   * @brief Find2DUnsharedEdges
   * @param elemList
   * @param edgeList
   */
  template <typename T> static void Find2DUnsharedEdges(typename DataArray<T>::Pointer elemList, typename DataArray<T>::Pointer edgeList)
  {
    size_t numElems = elemList->getNumberOfTuples();
    size_t numVertsPerElem = elemList->getNumberOfComponents();
    T v0 = 0;
    T v1 = 0;

    std::pair<T, T> edge;
    std::map<std::pair<T, T>, T> edgeMap;

    for(size_t i = 0; i < numElems; i++)
    {
      T* verts = elemList->getTuplePointer(i);

      for(size_t j = 0; j < numVertsPerElem; j++)
      {
        if(j == (numVertsPerElem - 1))
        {
          if(verts[j] > verts[0])
          {
            v0 = verts[0];
            v1 = verts[j];
          }
          else
          {
            v0 = verts[j];
            v1 = verts[0];
          }
        }
        else
        {
          if(verts[j] > verts[j + 1])
          {
            v0 = verts[j + 1];
            v1 = verts[j];
          }
          else
          {
            v0 = verts[j];
            v1 = verts[j + 1];
          }
        }
        edge = std::make_pair(v0, v1);
        edgeMap[edge]++;
      }
    }

    typename std::map<std::pair<T, T>, T>::iterator mapIter = edgeMap.begin();

    while(mapIter != edgeMap.end())
    {
      if((*mapIter).second > 1)
      {
        edgeMap.erase(mapIter++);
      }
      else
      {
        ++mapIter;
      }
    }

    edgeList->resize(edgeMap.size());
    T* bEdges = edgeList->getPointer(0);
    T index = 0;

    for(mapIter = edgeMap.begin(); mapIter != edgeMap.end(); ++mapIter)
    {
      bEdges[2 * index] = (*mapIter).first.first;
      bEdges[2 * index + 1] = (*mapIter).first.second;
      ++index;
    }
  }

  /**
  * @brief FindUnsharedTetEdges
  * @param tetList
  * @param edgeList
  */
  template <typename T> static void FindUnsharedTetEdges(typename DataArray<T>::Pointer tetList, typename DataArray<T>::Pointer edgeList)
  {
    size_t numElems = tetList->getNumberOfTuples();

    std::pair<T, T> edge;
    std::map<std::pair<T, T>, T> edgeMap;

    for(size_t i = 0; i < numElems; i++)
    {
      T* verts = tetList->getTuplePointer(i);

      std::vector<T> edge0 = {verts[0], verts[1]};
      std::vector<T> edge1 = {verts[0], verts[2]};
      std::vector<T> edge2 = {verts[1], verts[2]};
      std::vector<T> edge3 = {verts[0], verts[3]};
      std::vector<T> edge4 = {verts[1], verts[3]};
      std::vector<T> edge5 = {verts[2], verts[3]};
      std::list<std::vector<T>> edgeList = {edge0, edge1, edge2, edge3, edge4, edge5};

      for(auto&& uEdge : edgeList)
      {
        std::sort(uEdge.begin(), uEdge.end());
        edge = std::make_pair(uEdge[0], uEdge[1]);
        edgeMap[edge]++;
      }
    }

    typename std::map<std::pair<T, T>, T>::iterator mapIter = edgeMap.begin();

    while(mapIter != edgeMap.end())
    {
      if((*mapIter).second > 1)
      {
        edgeMap.erase(mapIter++);
      }
      else
      {
        ++mapIter;
      }
    }

    edgeList->resize(edgeMap.size());
    T* bEdges = edgeList->getPointer(0);
    T index = 0;

    for(mapIter = edgeMap.begin(); mapIter != edgeMap.end(); ++mapIter)
    {
      bEdges[2 * index] = (*mapIter).first.first;
      bEdges[2 * index + 1] = (*mapIter).first.second;
      ++index;
    }
  }

  /**
  * @brief FindUnsharedHexEdges
  * @param hexList
  * @param edgeList
  */
  template <typename T> static void FindUnsharedHexEdges(typename DataArray<T>::Pointer hexList, typename DataArray<T>::Pointer edgeList)
  {
    size_t numElems = hexList->getNumberOfTuples();

    std::pair<T, T> edge;
    std::map<std::pair<T, T>, T> edgeMap;

    for(size_t i = 0; i < numElems; i++)
    {
      T* verts = hexList->getTuplePointer(i);

      std::vector<T> edge0 = {verts[0], verts[1]};
      std::vector<T> edge1 = {verts[1], verts[2]};
      std::vector<T> edge2 = {verts[2], verts[3]};
      std::vector<T> edge3 = {verts[3], verts[0]};

      std::vector<T> edge4 = {verts[0], verts[4]};
      std::vector<T> edge5 = {verts[1], verts[5]};
      std::vector<T> edge6 = {verts[2], verts[6]};
      std::vector<T> edge7 = {verts[3], verts[7]};

      std::vector<T> edge8 = {verts[4], verts[5]};
      std::vector<T> edge9 = {verts[5], verts[6]};
      std::vector<T> edge10 = {verts[6], verts[7]};
      std::vector<T> edge11 = {verts[7], verts[4]};

      std::list<std::vector<T>> edgeList = {edge0, edge1, edge2, edge3, edge4, edge5,
                                            edge6, edge7, edge8, edge9, edge10, edge11};

      for(auto&& uEdge : edgeList)
      {
        std::sort(uEdge.begin(), uEdge.end());
        edge = std::make_pair(uEdge[0], uEdge[1]);
        edgeMap[edge]++;
      }
    }

    typename std::map<std::pair<T, T>, T>::iterator mapIter = edgeMap.begin();

    while(mapIter != edgeMap.end())
    {
      if((*mapIter).second > 1)
      {
        edgeMap.erase(mapIter++);
      }
      else
      {
        ++mapIter;
      }
    }

    edgeList->resize(edgeMap.size());
    T* bEdges = edgeList->getPointer(0);
    T index = 0;

    for(mapIter = edgeMap.begin(); mapIter != edgeMap.end(); ++mapIter)
    {
      bEdges[2 * index] = (*mapIter).first.first;
      bEdges[2 * index + 1] = (*mapIter).first.second;
      ++index;
    }
  }

  /**
   * @brief FindUnsharedTetFaces
   * @param tetList
   * @param edgeList
   */
  template <typename T> static void FindUnsharedTetFaces(typename DataArray<T>::Pointer tetList, typename DataArray<T>::Pointer faceList)
  {
    size_t numElems = tetList->getNumberOfTuples();

    std::tuple<T, T, T> face;
    std::map<std::tuple<T, T, T>, T> faceMap;

    for(size_t i = 0; i < numElems; i++)
    {
      T* verts = tetList->getTuplePointer(i);

      std::vector<T> tri0 = {verts[0], verts[1], verts[2]};
      std::vector<T> tri1 = {verts[1], verts[2], verts[3]};
      std::vector<T> tri2 = {verts[0], verts[2], verts[3]};
      std::vector<T> tri3 = {verts[0], verts[1], verts[3]};
      std::list<std::vector<T>> triList = {tri0, tri1, tri2, tri3};

      for(auto&& tri : triList)
      {
        std::sort(tri.begin(), tri.end());
        face = std::make_tuple(tri[0], tri[1], tri[2]);
        faceMap[face]++;
      }
    }

    typename std::map<std::tuple<T, T, T>, T>::iterator mapIter = faceMap.begin();

    while(mapIter != faceMap.end())
    {
      if((*mapIter).second > 1)
      {
        faceMap.erase(mapIter++);
      }
      else
      {
        ++mapIter;
      }
    }

    faceList->resize(faceMap.size());
    T* uFaces = faceList->getPointer(0);
    T index = 0;

    for(mapIter = faceMap.begin(); mapIter != faceMap.end(); ++mapIter)
    {
      uFaces[3 * index] = std::get<0>((*mapIter).first);
      uFaces[3 * index + 1] = std::get<1>((*mapIter).first);
      uFaces[3 * index + 2] = std::get<2>((*mapIter).first);
      ++index;
    }
  }

  /**
  * @brief FindUnsharedHexFaces
  * @param hexList
  * @param edgeList
  */
  template <typename T> static void FindUnsharedHexFaces(typename DataArray<T>::Pointer hexList, typename DataArray<T>::Pointer faceList)
  {
    size_t numElems = hexList->getNumberOfTuples();

    std::tuple<T, T, T, T> face;
    std::map<std::tuple<T, T, T, T>, T> faceMap;

    for(size_t i = 0; i < numElems; i++)
    {
      T* verts = hexList->getTuplePointer(i);

      std::vector<T> quad0 = {verts[0], verts[1], verts[5], verts[4]};
      std::vector<T> quad1 = {verts[1], verts[2], verts[6], verts[5]};
      std::vector<T> quad2 = {verts[2], verts[3], verts[7], verts[6]};
      std::vector<T> quad3 = {verts[3], verts[0], verts[4], verts[7]};
      std::vector<T> quad4 = {verts[0], verts[1], verts[2], verts[3]};
      std::vector<T> quad5 = {verts[4], verts[5], verts[6], verts[7]};

      std::list<std::vector<T>> quadList = {quad0, quad1, quad2, quad3, quad4, quad5};

      for(auto&& quad : quadList)
      {
        std::sort(quad.begin(), quad.end());
        face = std::make_tuple(quad[0], quad[1], quad[2], quad[3]);
        faceMap[face]++;
      }
    }

    typename std::map<std::tuple<T, T, T, T>, T>::iterator mapIter = faceMap.begin();

    while(mapIter != faceMap.end())
    {
      if((*mapIter).second > 1)
      {
        faceMap.erase(mapIter++);
      }
      else
      {
        ++mapIter;
      }
    }

    faceList->resize(faceMap.size());
    T* uFaces = faceList->getPointer(0);
    T index = 0;

    for(mapIter = faceMap.begin(); mapIter != faceMap.end(); ++mapIter)
    {
      uFaces[4 * index] = std::get<0>((*mapIter).first);
      uFaces[4 * index + 1] = std::get<1>((*mapIter).first);
      uFaces[4 * index + 2] = std::get<2>((*mapIter).first);
      uFaces[4 * index + 3] = std::get<3>((*mapIter).first);
      ++index;
    }
  }
};

/**
 * @brief The Topology class
 */
class Topology
{
public:
  Topology() = default;
  virtual ~Topology() = default;

  /**
   * @brief FindElementCentroids
   * @param elemList
   * @param vertices
   * @param elementCentroids
   */
  template <typename T> static void FindElementCentroids(typename DataArray<T>::Pointer elemList, FloatArrayType::Pointer vertices, FloatArrayType::Pointer centroids)
  {
    size_t numElems = elemList->getNumberOfTuples();
    size_t numVertsPerElem = elemList->getNumberOfComponents();
    size_t numDims = 3;
    float* elementCentroids = centroids->getPointer(0);
    float* vertex = vertices->getPointer(0);

    for(size_t i = 0; i < numDims; i++)
    {
      for(size_t j = 0; j < numElems; j++)
      {
        T* Elem = elemList->getTuplePointer(j);
        float vertPos = 0.0;
        for(size_t k = 0; k < numVertsPerElem; k++)
        {
          vertPos += vertex[3 * Elem[k] + i];
        }
        vertPos /= static_cast<float>(numVertsPerElem);
        elementCentroids[numDims * j + i] = vertPos;
      }
    }
  }

  /**
   * @brief Find2DElementAreas
   * @param elemList
   * @param vertices
   * @param areas
   */
  template <typename T> static void Find2DElementAreas(typename DataArray<T>::Pointer elemList, FloatArrayType::Pointer vertices, FloatArrayType::Pointer areas)
  {
    float nx, ny, nz;
    int32_t projection;

    size_t numElems = elemList->getNumberOfTuples();
    int64_t numVertsPerElem = static_cast<int64_t>(elemList->getNumberOfComponents());
    if(numVertsPerElem < 3)
    {
      return;
    }
    float* vertex = vertices->getPointer(0);
    float* elemAreas = areas->getPointer(0);
    float normal[3] = {0.0f, 0.0f, 0.0f};
    std::vector<float> coords(3 * numVertsPerElem, 0.0f);
    float vert0[3] = {0.0f, 0.0f, 0.0f};
    float vert1[3] = {0.0f, 0.0f, 0.0f};
    float vert2[3] = {0.0f, 0.0f, 0.0f};

    for(size_t i = 0; i < numElems; i++)
    {
      float area = 0.0f;
      T* elem = elemList->getTuplePointer(i);

      // Create a contiguous vertex coordinates list
      // This simplifies the pointer arithmetic a bit
      for(int64_t j = 0; j < numVertsPerElem; j++)
      {
        std::copy(vertex + (3 * elem[j]), vertex + (3 * elem[j] + 3), coords.begin() + (3 * j));
      }

      float* coordinates = coords.data();
      GeometryMath::FindPolygonNormal(coordinates, numVertsPerElem, normal);
      MatrixMath::Normalize3x1(normal);

      nx = (normal[0] > 0.0 ? normal[0] : -normal[0]);
      ny = (normal[1] > 0.0 ? normal[1] : -normal[1]);
      nz = (normal[2] > 0.0 ? normal[2] : -normal[2]);
      projection = (nx > ny ? (nx > nz ? 0 : 2) : (ny > nz ? 1 : 2));

      for(int64_t j = 0; j < numVertsPerElem; j++)
      {
        std::copy(coordinates + (3 * j), coordinates + (3 * j + 3), vert0);
        std::copy(coordinates + (3 * ((j + 1) % numVertsPerElem)), coordinates + (3 * ((j + 1) % numVertsPerElem) + 3), vert1);
        std::copy(coordinates + (3 * ((j + 2) % numVertsPerElem)), coordinates + (3 * ((j + 2) % numVertsPerElem) + 3), vert2);

        switch(projection)
        {
        case 0:
        {
          area += coordinates[3 * ((j + 1) % numVertsPerElem) + 1] * (coordinates[3 * ((j + 2) % numVertsPerElem) + 2] - coordinates[3 * j + 2]);
          continue;
        }
        case 1:
        {
          area += coordinates[3 * ((j + 1) % numVertsPerElem) + 0] * (coordinates[3 * ((j + 2) % numVertsPerElem) + 2] - coordinates[3 * j + 2]);
          continue;
        }
        case 2:
        {
          area += coordinates[3 * ((j + 1) % numVertsPerElem) + 0] * (coordinates[3 * ((j + 2) % numVertsPerElem) + 1] - coordinates[3 * j + 1]);
          continue;
        }
        }
      }

      switch(projection)
      {
      case 0:
      {
        area /= (2.0f * nx);
        break;
      }
      case 1:
      {
        area /= (2.0f * ny);
        break;
      }
      case 2:
      {
        area /= (2.0f * nz);
      }
      }
      elemAreas[i] = fabsf(area);
    }
  }

  /**
   * @brief FindTetVolumes
   * @param tetList
   * @param vertices
   * @param volumes
   */
  template <typename T> static void FindTetVolumes(typename DataArray<T>::Pointer tetList, FloatArrayType::Pointer vertices, FloatArrayType::Pointer volumes)
  {
    size_t numTets = tetList->getNumberOfTuples();
    float* vertex = vertices->getPointer(0);
    float* volumePtr = volumes->getPointer(0);

    for(size_t i = 0; i < numTets; i++)
    {
      T* tet = tetList->getTuplePointer(i);
      float vert0[3] = {vertex[3 * tet[0] + 0], vertex[3 * tet[0] + 1], vertex[3 * tet[0] + 2]};
      float vert1[3] = {vertex[3 * tet[1] + 0], vertex[3 * tet[1] + 1], vertex[3 * tet[1] + 2]};
      float vert2[3] = {vertex[3 * tet[2] + 0], vertex[3 * tet[2] + 1], vertex[3 * tet[2] + 2]};
      float vert3[3] = {vertex[3 * tet[3] + 0], vertex[3 * tet[3] + 1], vertex[3 * tet[3] + 2]};

      float vertMatrix[3][3] = {{vert1[0] - vert0[0], vert2[0] - vert0[0], vert3[0] - vert0[0]},
                                {vert1[1] - vert0[1], vert2[1] - vert0[1], vert3[1] - vert0[1]},
                                {vert1[2] - vert0[2], vert2[2] - vert0[2], vert3[2] - vert0[2]}};

      volumePtr[i] = (MatrixMath::Determinant3x3(vertMatrix) / 6.0f);
    }
  }

  /**
  * @brief FindHexVolumes
  * @param hexList
  * @param vertices
  * @param volumes
  */
  template <typename T> static void FindHexVolumes(typename DataArray<T>::Pointer hexList, FloatArrayType::Pointer vertices, FloatArrayType::Pointer volumes)
  {
    size_t numHexas = hexList->getNumberOfTuples();
    float* vertex = vertices->getPointer(0);
    float* volumePtr = volumes->getPointer(0);

    for(size_t i = 0; i < numHexas; i++)
    {
      // Subdivide each hexahedron into 5 tetrahedra & sum their volumes
      std::vector<std::vector<int64_t>> subTets(5, std::vector<int64_t>(4, 0));
      T* hex = hexList->getTuplePointer(i);

      // First tetrahedron from hexahedron vertices (0, 1, 3, 4);
      subTets[0][0] = hex[0];
      subTets[0][1] = hex[1];
      subTets[0][2] = hex[3];
      subTets[0][3] = hex[4];

      // Second tetrahedron from hexahedron vertices (1, 4, 5, 6);
      subTets[1][0] = hex[1];
      subTets[1][1] = hex[4];
      subTets[1][2] = hex[5];
      subTets[1][3] = hex[6];

      // Third tetrahedron from hexahedron vertices (1, 4, 6, 3);
      subTets[2][0] = hex[1];
      subTets[2][1] = hex[3];
      subTets[2][2] = hex[6];
      subTets[2][3] = hex[3];

      // Fourth tetrahedron from hexahedron vertices (1, 3, 6, 2);
      subTets[3][0] = hex[1];
      subTets[3][1] = hex[3];
      subTets[3][2] = hex[6];
      subTets[3][3] = hex[2];

      // Fifth tetrahedron from hexahedron vertices (3, 6, 7, 4);
      subTets[4][0] = hex[3];
      subTets[4][1] = hex[6];
      subTets[4][2] = hex[7];
      subTets[4][3] = hex[4];

      float volume = 0.0f;

      for(auto&& tet : subTets)
      {
        float vert0[3] = {vertex[3 * tet[0] + 0], vertex[3 * tet[0] + 1], vertex[3 * tet[0] + 2]};
        float vert1[3] = {vertex[3 * tet[1] + 0], vertex[3 * tet[1] + 1], vertex[3 * tet[1] + 2]};
        float vert2[3] = {vertex[3 * tet[2] + 0], vertex[3 * tet[2] + 1], vertex[3 * tet[2] + 2]};
        float vert3[3] = {vertex[3 * tet[3] + 0], vertex[3 * tet[3] + 1], vertex[3 * tet[3] + 2]};

        float vertMatrix[3][3] = {{vert1[0] - vert0[0], vert2[0] - vert0[0], vert3[0] - vert0[0]},
                                  {vert1[1] - vert0[1], vert2[1] - vert0[1], vert3[1] - vert0[1]},
                                   {vert1[2] - vert0[2], vert2[2] - vert0[2], vert3[2] - vert0[2]}};

        volume += (MatrixMath::Determinant3x3(vertMatrix) / 6.0f);
      }

      volumePtr[i] = volume;
    }
  }

  /**
  * @brief FindTetJacobians
  * @param tetList
  * @param vertices
  * @param jacobians
  */
  template <typename T> static void FindTetJacobians(typename DataArray<T>::Pointer tetList, FloatArrayType::Pointer vertices, FloatArrayType::Pointer jacobians)
  {
	  size_t numTets = tetList->getNumberOfTuples();
	  float* vertex = vertices->getPointer(0);
	  float* jacobianPtr = jacobians->getPointer(0);

	  for (size_t i = 0; i < numTets; i++)
	  {
		  T* tet = tetList->getTuplePointer(i);
		  //get vert positions
		  float vert0[3] = { vertex[3 * tet[0] + 0], vertex[3 * tet[0] + 1], vertex[3 * tet[0] + 2] };
		  float vert1[3] = { vertex[3 * tet[1] + 0], vertex[3 * tet[1] + 1], vertex[3 * tet[1] + 2] };
		  float vert2[3] = { vertex[3 * tet[2] + 0], vertex[3 * tet[2] + 1], vertex[3 * tet[2] + 2] };
		  float vert3[3] = { vertex[3 * tet[3] + 0], vertex[3 * tet[3] + 1], vertex[3 * tet[3] + 2] };
		  //build jacobian matrix
		  float vertMatrix[3][3] = { { vert1[0] - vert0[0], vert2[0] - vert0[0], vert3[0] - vert0[0] },
		  { vert1[1] - vert0[1], vert2[1] - vert0[1], vert3[1] - vert0[1] },
		  { vert1[2] - vert0[2], vert2[2] - vert0[2], vert3[2] - vert0[2] } };
		  //find jacobian, which is determinant of the jacobian matrix
		  jacobianPtr[i] = MatrixMath::Determinant3x3(vertMatrix);
	  }
  }

  /**
  * @brief FindTetMinDihedralAngles
  * @param tetList
  * @param vertices
  * @param minAngles
  */
  template <typename T> static void FindTetMinDihedralAngles(typename DataArray<T>::Pointer tetList, FloatArrayType::Pointer vertices, FloatArrayType::Pointer minAngles)
  {
	  size_t numTets = tetList->getNumberOfTuples();
	  float* vertex = vertices->getPointer(0);
	  float* minAnglesPtr = minAngles->getPointer(0);

	  for (size_t i = 0; i < numTets; i++)
	  {
		  T* tet = tetList->getTuplePointer(i);
		  //get vert positions
		  float vert0[3] = { vertex[3 * tet[0] + 0], vertex[3 * tet[0] + 1], vertex[3 * tet[0] + 2] };
		  float vert1[3] = { vertex[3 * tet[1] + 0], vertex[3 * tet[1] + 1], vertex[3 * tet[1] + 2] };
		  float vert2[3] = { vertex[3 * tet[2] + 0], vertex[3 * tet[2] + 1], vertex[3 * tet[2] + 2] };
		  float vert3[3] = { vertex[3 * tet[3] + 0], vertex[3 * tet[3] + 1], vertex[3 * tet[3] + 2] };
		  //find 5 edges needed to find 4 face normals
		  float v10[3] = { (vert1[0] - vert0[0]), (vert1[1] - vert0[1]), (vert1[2] - vert0[2]) };
		  float v20[3] = { (vert2[0] - vert0[0]), (vert2[1] - vert0[1]), (vert2[2] - vert0[2]) };
		  float v30[3] = { (vert3[0] - vert0[0]), (vert3[1] - vert0[1]), (vert3[2] - vert0[2]) };
		  float v21[3] = { (vert2[0] - vert1[0]), (vert2[1] - vert1[1]), (vert2[2] - vert1[2]) };
		  float v31[3] = { (vert3[0] - vert1[0]), (vert3[1] - vert1[1]), (vert3[2] - vert1[2]) };
		  //find 4 face-to-face normals
		  float norm1[3] = { (v10[1] * v20[2] - v10[2] * v20[1]), (v10[2] * v20[0] - v10[0] * v20[2]), (v10[0] * v20[1] - v10[1] * v20[0]) };
		  float norm2[3] = { (v30[1] * v10[2] - v30[2] * v10[1]), (v30[2] * v10[0] - v30[0] * v10[2]), (v30[0] * v10[1] - v30[1] * v10[0]) };
		  float norm3[3] = { (v20[1] * v30[2] - v20[2] * v30[1]), (v20[2] * v30[0] - v20[0] * v30[2]), (v20[0] * v30[1] - v20[1] * v30[0]) };
		  float norm4[3] = { (v31[1] * v21[2] - v31[2] * v21[1]), (v31[2] * v21[0] - v31[0] * v21[2]), (v31[0] * v21[1] - v31[1] * v21[0]) };
		  //find the magnitudes of each normal
		  float norm1mag = sqrtf(norm1[0] * norm1[0] + norm1[1] * norm1[1] + norm1[2] * norm1[2]);
		  float norm2mag = sqrtf(norm2[0] * norm2[0] + norm2[1] * norm2[1] + norm2[2] * norm2[2]);
		  float norm3mag = sqrtf(norm3[0] * norm3[0] + norm3[1] * norm3[1] + norm3[2] * norm3[2]);
		  float norm4mag = sqrtf(norm4[0] * norm4[0] + norm4[1] * norm4[1] + norm4[2] * norm4[2]);
		  //find angles between faces
		  float ang1 = (norm1[0] * norm2[0] + norm1[1] * norm2[1] + norm1[2] * norm2[2]) / (norm1mag * norm2mag);
		  float ang2 = (norm1[0] * norm3[0] + norm1[1] * norm3[1] + norm1[2] * norm3[2]) / (norm1mag * norm3mag);
		  float ang3 = (norm1[0] * norm4[0] + norm1[1] * norm4[1] + norm1[2] * norm4[2]) / (norm1mag * norm4mag);
		  float ang4 = (norm2[0] * norm3[0] + norm2[1] * norm3[1] + norm2[2] * norm3[2]) / (norm2mag * norm3mag);
		  float ang5 = (norm2[0] * norm4[0] + norm2[1] * norm4[1] + norm2[2] * norm4[2]) / (norm2mag * norm4mag);
		  float ang6 = (norm3[0] * norm4[0] + norm3[1] * norm4[1] + norm3[2] * norm4[2]) / (norm3mag * norm4mag);
		  //find the maximum ang value, which will be the minimum angle after the acos
		  float minAng = ang1;
		  if (ang2 > minAng) { minAng = ang2; }
		  if (ang3 > minAng) { minAng = ang3; }
		  if (ang4 > minAng) { minAng = ang4; }
		  if (ang5 > minAng) { minAng = ang5; }
		  if (ang6 > minAng) { minAng = ang6; }

		  minAnglesPtr[i] = SIMPLib::Constants::k_180OverPi * acosf(minAng);
	  }
  }
};

/**
 * @brief The Generic class
 */
class Generic
{
public:
  Generic() = default;
  virtual ~Generic() = default;

  /**
   * @brief AverageVertexArrayValues
   * @param elemList
   * @param inVertexArray
   * @param outElemArray
   */
  template <typename T, typename K> static void AverageVertexArrayValues(typename DataArray<T>::Pointer elemList, typename DataArray<K>::Pointer inVertexArray, DataArray<float>::Pointer outElemArray)
  {
    Q_ASSERT(outElemArray->getComponentDimensions() == inVertexArray->getComponentDimensions());
    Q_ASSERT(elemList->getNumberOfTuples() == outElemArray->getNumberOfTuples());

    K* vertArray = inVertexArray->getPointer(0);
    float* elemArray = outElemArray->getPointer(0);

    size_t numElems = outElemArray->getNumberOfTuples();
    size_t numDims = inVertexArray->getNumberOfComponents();
    size_t numVertsPerElem = elemList->getNumberOfComponents();

    for(size_t i = 0; i < numDims; i++)
    {
      for(size_t j = 0; j < numElems; j++)
      {
        T* Elem = elemList->getTuplePointer(j);
        float vertValue = 0.0;
        for(size_t k = 0; k < numVertsPerElem; k++)
        {
          vertValue += vertArray[numDims * Elem[k] + i];
        }
        vertValue /= static_cast<float>(numVertsPerElem);
        elemArray[numDims * j + i] = vertValue;
      }
    }
  }

  /**
   * @brief WeightedAverageVertexArrayValues
   * @param elemList
   * @param vertices
   * @param centroids
   * @param inVertexArray
   * @param outElemArray
   */
  template <typename T, typename K>
  static void WeightedAverageVertexArrayValues(typename DataArray<T>::Pointer elemList, DataArray<float>::Pointer vertices, DataArray<float>::Pointer centroids,
                                               typename DataArray<K>::Pointer inVertexArray, DataArray<float>::Pointer outElemArray)
  {
    Q_ASSERT(outElemArray->getNumberOfTuples() == elemList->getNumberOfTuples());
    Q_ASSERT(outElemArray->getComponentDimensions() == inVertexArray->getComponentDimensions());

    K* vertArray = inVertexArray->getPointer(0);
    float* elemArray = outElemArray->getPointer(0);
    float* elementCentroids = centroids->getPointer(0);
    float* vertex = vertices->getPointer(0);

    size_t numElems = outElemArray->getNumberOfTuples();
    size_t cDims = inVertexArray->getNumberOfComponents();
    size_t numVertsPerElem = elemList->getNumberOfComponents();
    size_t numDims = 3;

    // Vector to hold vertex-centroid distances, 4 per cell
    std::vector<float> vertCentDist(numElems * numVertsPerElem);

    for(size_t i = 0; i < numElems; i++)
    {
      T* Elem = elemList->getTuplePointer(i);
      for(size_t j = 0; j < numVertsPerElem; j++)
      {
        for(size_t k = 0; k < numDims; k++)
        {
          vertCentDist[numVertsPerElem * i + j] += (vertex[numDims * Elem[j] + k] - elementCentroids[numDims * i + k]) * (vertex[numDims * Elem[j] + k] - elementCentroids[numDims * i + k]);
        }
        vertCentDist[numVertsPerElem * i + j] = sqrtf(vertCentDist[numVertsPerElem * i + j]);
      }
    }

    for(size_t i = 0; i < cDims; i++)
    {
      for(size_t j = 0; j < numElems; j++)
      {
        T* Elem = elemList->getTuplePointer(j);
        float vertValue = 0.0;
        float sumDist = 0.0;
        for(size_t k = 0; k < numVertsPerElem; k++)
        {
          vertValue += vertArray[cDims * Elem[k] + i] * vertCentDist[numVertsPerElem * j + k];
          sumDist += vertCentDist[numVertsPerElem * j + k];
        }
        vertValue /= static_cast<float>(sumDist);
        elemArray[cDims * j + i] = vertValue;
      }
    }
  }

  template <typename T, typename K, typename L, typename M>
  static void AverageCellArrayValues(typename DynamicListArray<L, T>::Pointer elemsContainingVert, DataArray<float>::Pointer vertices, typename DataArray<K>::Pointer inElemArray,
                                     typename DataArray<M>::Pointer outVertexArray)
  {
    Q_ASSERT(outVertexArray->getNumberOfTuples() == vertices->getNumberOfTuples());
    Q_ASSERT(outVertexArray->getComponentDimensions() == inElemArray->getComponentDimensions());

    K* elemArray = inElemArray->getPointer(0);
    M* vertArray = outVertexArray->getPointer(0);

    size_t numVerts = vertices->getNumberOfTuples();
    size_t cDims = inElemArray->getNumberOfComponents();

    for(size_t i = 0; i < cDims; i++)
    {
      for(size_t j = 0; j < numVerts; j++)
      {
        L numElemsPerVert = elemsContainingVert->getNumberOfElements(j);
        T* elemIdxs = elemsContainingVert->getElementListPointer(j);
        M vertValue = 0.0;
        double weight = 1.0 / numElemsPerVert;
        for(size_t k = 0; k < numElemsPerVert; k++)
        {
          vertValue += static_cast<M>(elemArray[cDims * elemIdxs[k] + i] * weight);
        }
        vertArray[cDims * j + i] = vertValue;
      }
    }
  }
};
}

