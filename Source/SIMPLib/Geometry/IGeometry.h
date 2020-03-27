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

#include <QMutex>
#include <QtCore/QMap>
#include <QtCore/QString>

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Observable.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/DynamicListArray.hpp"
#include "SIMPLib/Geometry/ITransformContainer.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

class AttributeMatrix;
using AttributeMatrixShPtrType = std::shared_ptr<AttributeMatrix>;

// -----------------------------------------------------------------------------
// Typedefs
// -----------------------------------------------------------------------------
using MeshIndexType = size_t;
using MeshIndexArrayType = DataArray<MeshIndexType>;
using SharedVertexList = FloatArrayType;
using SharedEdgeList = MeshIndexArrayType;
using SharedTriList = MeshIndexArrayType;
using SharedQuadList = MeshIndexArrayType;
using SharedTetList = MeshIndexArrayType;
using SharedHexList = MeshIndexArrayType;
using SharedFaceList = MeshIndexArrayType;
using ElementDynamicList = DynamicListArray<uint16_t, MeshIndexType>;

/**
 * @brief Macro to silence compiler warnings for unused parameters in methods.
 */
#define SIMPL_NOT_USED(x)

/**
 * @brief The IGeometry class
 */
class SIMPLib_EXPORT IGeometry : public Observable
{
  // This line MUST be first when exposing a class and properties to Python

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(IGeometry)
  PYB11_SHARED_POINTERS(IGeometry)
  PYB11_ENUMERATION(Type)
  PYB11_ENUMERATION(VtkCellType)
  PYB11_ENUMERATION(LengthUnit)
  PYB11_PROPERTY(QString Name READ getName WRITE setName)
  PYB11_METHOD(Type getGeometryType)
  PYB11_METHOD(QString getGeometryTypeAsString)
  PYB11_METHOD(QString getInfoString ARGS formatType)
  PYB11_METHOD(uint32_t getXdmfGridType)
  PYB11_METHOD(uint32_t getUnitDimensionality)
  PYB11_PROPERTY(uint32_t SpatialDimensionality READ getSpatialDimensionality WRITE setSpatialDimensionality)
  PYB11_METHOD(Pointer deepCopy ARGS forceNoAllocate)
  PYB11_METHOD(void initializeWithZeros)
  PYB11_METHOD(void addOrReplaceAttributeMatrix ARGS Name AttributeMatrix)
  PYB11_METHOD(AttributeMatrix getAttributeMatrix ARGS Name)
  PYB11_METHOD(AttributeMatrix removeAttributeMatrix ARGS Name)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = IGeometry;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for IGeometry
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for IGeometry
   */
  static QString ClassName();

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

  enum class LengthUnit : EnumType
  {
    Yoctometer,
    Zeptometer,
    Attometer,
    Femtometer,
    Picometer,
    Nanometer,
    Micrometer,
    Millimeter,
    Centimeter,
    Decimeter,
    Meter,
    Decameter,
    Hectometer,
    Kilometer,
    Megameter,
    Gigameter,
    Terameter,
    Petameter,
    Exameter,
    Zettameter,
    Yottameter,
    Angstrom,
    Mil,
    Inch,
    Foot,
    Mile,
    Fathom,
    Unspecified = 100U,
    Unknown = 101U
  };

  using VtkCellTypes = QVector<VtkCellType>;
  using Types = QVector<Type>;

  /**
   * @brief AttributeMatrixMap_t
   */
  using AttributeMatrixMap_t = QMap<QString, AttributeMatrixShPtrType>;

  /**
   * @brief stringToType
   * @param str
   * @return
   */
  static IGeometry::Type StringToType(const QString& str);

  /**
   * @brief typeToString
   * @param t
   * @return
   */
  static QString TypeToString(IGeometry::Type t);

  /**
   * @brief stringToLengthUnit
   * @param str
   * @return
   */
  static IGeometry::LengthUnit StringToLengthUnit(const QString& str);

  /**
   * @brief lengthUnitToString
   * @param t
   * @return
   */
  static QString LengthUnitToString(IGeometry::LengthUnit t);

  /**
   * @brief GetAllLengthUnitStrings Returns all the Length Units as a vector of strings. This would be suitable to display as a list to a user.
   * @return
   */
  static QVector<QString> GetAllLengthUnitStrings();

  /**
   * @brief Setter property for TimeValue
   */
  void setTimeValue(float value);
  /**
   * @brief Getter property for TimeValue
   * @return Value of TimeValue
   */
  float getTimeValue() const;

  /**
   * @brief Setter property for EnableTimeSeries
   */
  void setEnableTimeSeries(bool value);
  /**
   * @brief Getter property for EnableTimeSeries
   * @return Value of EnableTimeSeries
   */
  bool getEnableTimeSeries() const;

  /**
   * @brief Setter property for TransformContainer
   */
  void setTransformContainer(const ITransformContainer::Pointer& value);
  /**
   * @brief Getter property for TransformContainer
   * @return Value of TransformContainer
   */
  ITransformContainer::Pointer getTransformContainer() const;

  /**
   * @brief Setter property for Units
   */
  void setUnits(const IGeometry::LengthUnit& value);
  /**
   * @brief Getter property for Units
   * @return Value of Units
   */
  IGeometry::LengthUnit getUnits() const;

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
  virtual ElementDynamicList::Pointer getElementsContainingVert() const = 0;

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
  virtual ElementDynamicList::Pointer getElementNeighbors() const = 0;

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
  virtual size_t getNumberOfElements() const = 0;

  /**
   * @brief findElementSizes
   * @return
   */
  virtual int findElementSizes() = 0;

  /**
   * @brief getElementSizes
   * @return
   */
  virtual FloatArrayType::Pointer getElementSizes() const = 0;

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
  virtual FloatArrayType::Pointer getElementCentroids() const = 0;

  /**
   * @brief deleteElementCentroids
   */
  virtual void deleteElementCentroids() = 0;

  /**
   * @brief getParametricCenter
   * @param pCoords
   */
  virtual void getParametricCenter(double pCoords[3]) const = 0;

  /**
   * @brief getShapeFunctions
   * @param pCoords
   * @param derivs
   */
  virtual void getShapeFunctions(double pCoords[3], double* shape) const = 0;

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
  virtual QString getName() const final;

  /**
   * @brief getGeometryType
   * @return
   */
  virtual Type getGeometryType() const final;

  /**
   * @brief getGeometryTypeAsString
   * @return
   */
  virtual QString getGeometryTypeAsString() const final;

  /**
   * @brief getInfoString
   * @return Returns a formatted string that contains general infomation about
   * the instance of the object.
   */
  virtual QString getInfoString(SIMPL::InfoStringFormat) const = 0;

  /**
   * @brief getXdmfGridType
   * @return
   */
  virtual unsigned int getXdmfGridType() const final;

  /**
   * @brief getUnitDimensionality
   * @return
   */
  virtual unsigned int getUnitDimensionality() const final;

  /**
   * @brief setSpatialDimensionality
   * @param spatialDims
   */
  virtual void setSpatialDimensionality(unsigned int spatialDims) final;

  /**
   * @brief getSpatialDimensionality
   * @return
   */
  virtual unsigned int getSpatialDimensionality() const final;

  /**
   * @brief writeGeometryToHDF5
   * @param parentId
   * @param writeXdmf
   * @return
   */
  virtual int writeGeometryToHDF5(hid_t parentId, bool writeXdmf) const = 0;

  /**
   * @brief writeXdmf
   * @param out
   * @param dcName
   * @param hdfFileName
   * @return
   */
  virtual int writeXdmf(QTextStream& out, QString dcName, QString hdfFileName) const = 0;

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
  virtual Pointer deepCopy(bool forceNoAllocate = false) const = 0;

  /**
   * @brief initializeWithZeros
   */
  virtual void initializeWithZeros() = 0;

  /**
   * @brief addOrReplaceAttributeMatrix
   */
  virtual void addOrReplaceAttributeMatrix(const QString& name, AttributeMatrixShPtrType data) = 0;

  /**
   * @brief getAttributeMatrix
   * @param name
   * @return
   */
  virtual AttributeMatrixShPtrType getAttributeMatrix(const QString& name) const final;

  /**
   * @brief removeAttributeMatrix
   * @param name
   * @return
   */
  virtual AttributeMatrixShPtrType removeAttributeMatrix(const QString& name) final;

protected:

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

public:
  IGeometry(const IGeometry&) = delete;            // Copy Constructor Not Implemented
  IGeometry(IGeometry&&) = delete;                 // Move Constructor Not Implemented
  IGeometry& operator=(const IGeometry&) = delete; // Copy Assignment Not Implemented
  IGeometry& operator=(IGeometry&&) = delete;      // Move Assignment Not Implemented

protected:
  QString m_GeometryTypeName;
  Type m_GeometryType = Type::Unknown;
  unsigned int m_XdmfGridType = SIMPL::XdmfGridType::UnknownGrid;
  unsigned int m_UnitDimensionality = 0;
  unsigned int m_SpatialDimensionality = 0;
  int64_t m_ProgressCounter = 0;
  AttributeMatrixMap_t m_AttributeMatrices;

private:
  float m_TimeValue = 0.0f;
  bool m_EnableTimeSeries = false;
  ITransformContainer::Pointer m_TransformContainer = {};
  IGeometry::LengthUnit m_Units = LengthUnit::Unspecified;
  QString m_Name;
  QMutex m_Mutex;
};
