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
#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/Geometry/IGeometryGrid.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

/**
 * @brief The ImageGeom class represents a structured rectlinear grid
 */
class SIMPLib_EXPORT ImageGeom : public IGeometryGrid
{
  // clang-format off
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ImageGeom SUPERCLASS IGeometryGrid)
  PYB11_SHARED_POINTERS(ImageGeom)
  PYB11_STATIC_NEW_MACRO(ImageGeom)
  PYB11_STATIC_CREATION(CreateGeometry ARGS QString)
  PYB11_ENUMERATION(ErrorType)
  PYB11_METHOD(void setDimensions OVERLOAD size_t,x size_t,y size_t,z)
  PYB11_METHOD(SizeVec3Type getDimensions)
  PYB11_METHOD(void setSpacing OVERLOAD float,x float,y float,z)
  PYB11_METHOD(FloatVec3Type getSpacing)
  PYB11_METHOD(size_t getXPoints)
  PYB11_METHOD(size_t getYPoints)
  PYB11_METHOD(size_t getZPoints)
  PYB11_METHOD(void setOrigin OVERLOAD float,x float,y float,z)
  PYB11_METHOD(FloatVec3Type getOrigin)
  PYB11_METHOD(FloatVec6Type getBoundingBox OVERLOAD CONST_METHOD)
  PYB11_END_BINDINGS()
  // End Python bindings declarations
  // clang-format on

public:
  using Self = ImageGeom;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for ImageGeom
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ImageGeom
   */
  static QString ClassName();

  ~ImageGeom() override;

  using EnumType = unsigned int;

  enum class ErrorType : EnumType
  {
    XOutOfBoundsLow = 0,
    XOutOfBoundsHigh = 1,
    YOutOfBoundsLow = 2,
    YOutOfBoundsHigh = 3,
    ZOutOfBoundsLow = 4,
    ZOutOfBoundsHigh = 5,
    IndexOutOfBounds = 6,
    NoError = 7
  };

  /**
   * @brief CreateGeometry
   * @param name
   * @return
   */
  static Pointer CreateGeometry(const QString& name);

  /**
   * @brief Sets/Gets the Spacing property
   */
  FloatVec3Type getSpacing() const;
  void setSpacing(const FloatVec3Type& spacing);
  void setSpacing(float x, float y, float z);

  /**
   * @brief Sets/Gets the Origin property
   */
  FloatVec3Type getOrigin() const;
  void setOrigin(const FloatVec3Type& origin);
  void setOrigin(float x, float y, float z);

  /**
   * @brief getBoundingBox
   * @param boundingBox The bounding box will be stored in the input argument in the following order:
   * xMin, xMax, yMin, yMax, zMin, zMax
   */
  void getBoundingBox(float* boundingBox);

  /**
   * @brief getBoundingBox
   * @param boundingBox The bounding box will be stored in the input argument in the following order:
   * xMin, xMax, yMin, yMax, zMin, zMax
   */
  FloatVec6Type getBoundingBox() const;

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
  SizeVec3Type getDimensions() const override;

  void setDimensions(const SizeVec3Type& dims) override;
  void setDimensions(size_t x, size_t y, size_t z);

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

  // -----------------------------------------------------------------------------
  // Misc. ImageGeometry Methods
  // -----------------------------------------------------------------------------
  /**
   * @brief computeCellIndex This method will compute the X, Y & Z Index based
   * on a given set of coordinates.
   *
   * If an ImageGeometry has dimensions 10x20x30 with a resolution of
   * 0.5 x 0.5 x 0.5 and an Origin of 4.0, 6.0, 10.0 then following examples
   * are calculated:
   *
   * float coords[3] = {4.5f, 9.23f, 12.78f};
   *
   * The X, Y, & Z Cell indices would be
   * Cell[0] = 1
   * Cell[1] = 6
   * Cell[2] = 5
   *
   * and the raw index into any Cell array would be 1061.
   *
   * @param coords The Coords to check
   * @param index The returned cell indices
   * @return Int error code. There can be multiple failure mechanisms when doing
   * this calculation. Any return value != ErrorType::NoError is a failure to compute the indices.
   */
  ErrorType computeCellIndex(const float coords[], size_t index[3]);

  /**
   * @brief computeCellIndex This method will compute the X, Y & Z Index based
   * on a given set of coordinates.
   *
   * If an ImageGeometry has dimensions 10x20x30 with a resolution of
   * 0.5 x 0.5 x 0.5 and an Origin of 4.0, 6.0, 10.0 then following examples
   * are calculated:
   *
   * float coords[3] = {4.5f, 9.23f, 12.78f};
   *
   * The X, Y, & Z Cell indices would be
   * Cell[0] = 1
   * Cell[1] = 6
   * Cell[2] = 5
   *
   * and the raw index into any Cell array would be 1061.
   *
   * @param coords The Coords to check
   * @param index The returned index into a scalar array
   * @return Int error code. There can be multiple failure mechanisms when doing
   * this calculation. Any return value != ErrorType::NoError is a failure to compute the indices.
   */
  ErrorType computeCellIndex(const float coords[3], size_t& index);

protected:
  ImageGeom();

  /**
   * @brief gatherMetaData
   * @param parentid
   * @param volDims
   * @param spacing
   * @param origin
   * @param spatialDims
   * @param geomName
   * @param preflight
   * @return
   */
  int gatherMetaData(hid_t parentid, size_t volDims[3], float spacing[3], float origin[3], unsigned int spatialDims, const QString& geomName, bool preflight);

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
  FloatArrayType::Pointer m_VoxelSizes;

  FloatVec3Type m_Spacing;
  FloatVec3Type m_Origin;
  SizeVec3Type m_Dimensions;

  friend class FindImageDerivativesImpl;

public:
  ImageGeom(const ImageGeom&) = delete;            // Copy Constructor Not Implemented
  ImageGeom(ImageGeom&&) = delete;                 // Move Constructor Not Implemented
  ImageGeom& operator=(const ImageGeom&) = delete; // Copy Assignment Not Implemented
  ImageGeom& operator=(ImageGeom&&) = delete;      // Move Assignment Not Implemented
};
