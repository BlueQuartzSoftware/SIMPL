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

#include <QtCore/QJsonArray>
#include <QtCore/QMap>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/AttributeMatrixProxy.h"
#include "SIMPLib/Geometry/IGeometry.h"

/**
 * @brief The DataContainerProxy class
 */
class SIMPLib_EXPORT DataContainerProxy
{

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(DataContainerProxy)
  PYB11_CREATION()
  PYB11_PROPERTY(StorageType AttributeMatricies READ getAttributeMatricies WRITE setAttributeMatricies CONST_GET_OVERLOAD)
  PYB11_PROPERTY(QString Name READ getName WRITE setName)
  PYB11_PROPERTY(uint8_t Flag READ getFlag WRITE setFlag)
  PYB11_METHOD(AttributeMatrixProxy getAttributeMatrixProxy ARGS name RETURN_VALUE_POLICY py::return_value_policy::reference)
  PYB11_METHOD(void toggleFlag)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  // This enumeration is not a class enumeration because it is not possible to
  // do a bit-wise NOT operation on a class enumeration value.  We need to be
  // able to do a bit-wise NOT operation so that we can turn off certain flags.
  // This enumeration allows us to flip integer bits to turn on/off various types.
  enum DCGeometryTypeFlag : unsigned int
  {
    None_DCGeomType = 0x00,
    Image_DCGeomType = 0x10,
    RectGrid_DCGeomType = 0x20,
    Vertex_DCGeomType = 0x40,
    Edge_DCGeomType = 0x80,
    Triangle_DCGeomType = 0x1000,
    Quad_DCGeomType = 0x2000,
    Tetrahedral_DCGeomType = 0x4000,
    Hexahedral_DCGeomType = 0x8000,
    Unknown_DCGeomType = 0x100000,
    Any_DCGeomType = 0xFF
  };
  Q_DECLARE_FLAGS(DCGeometryTypeFlags, DCGeometryTypeFlag)

  using StorageType = QMap<QString, AttributeMatrixProxy>;

  /**
   * @brief DataContainerProxy
   */
  DataContainerProxy();

  /**
   * @brief DataContainerProxy
   * @param dc_name
   * @param read_dc
   * @param dc_type
   */
  DataContainerProxy(const QString& dc_name, const uint8_t& read_dc = Qt::Checked, IGeometry::Type dc_type = IGeometry::Type::Unknown);

  /**
   * @brief Copy Constructor
   */
  DataContainerProxy(const DataContainerProxy& amp);

  ~DataContainerProxy() = default;

  DataContainerProxy(DataContainerProxy&&) = default;           // Move Constructor Not Implemented
  DataContainerProxy& operator=(DataContainerProxy&&) = delete; // Move Assignment Not Implemented

  /**
   * @brief Returns the appropriate flag for the geometry type
   * @param geoType The data container geometry type
   * @return
   */
  static DataContainerProxy::DCGeometryTypeFlag GeometryTypeToFlag(IGeometry::Type geoType);

  /**
   * @brief operator = method
   */
  DataContainerProxy& operator=(const DataContainerProxy& amp);

  /**
   * @brief operator == method
   */
  bool operator==(const DataContainerProxy& amp) const;

  /**
   * @brief Writes the contents of the proxy to the json object 'json'
   * @param json
   * @return
   */
  void writeJson(QJsonObject& json) const;

  /**
   * @brief Reads the contents of the the json object 'json' into the proxy
   * @param json
   * @return
   */
  bool readJson(QJsonObject& json);

  /**
   * @brief Get the attribute matrices (Python Binding)
   */
  StorageType& getAttributeMatricies();

  /**
   * @brief Get the attribute matrices (Python Binding)
   * @return
   */
  const StorageType& getAttributeMatricies() const;

  /**
   * @brief Get the attribute matrices(Python Binding)
   * @param new AttributeMatrices map
   */
  void setAttributeMatricies(const QMap<QString, AttributeMatrixProxy>&);

  /**
   * @brief insertAttributeMatrix
   * @param name
   * @param proxy
   */
  void insertAttributeMatrix(const QString& name, const AttributeMatrixProxy& proxy);

  /**
   * @brief Get the data container name (Python Binding)
   */
  QString getName() const;

  /**
   * @brief Set the data container name (Python Binding)
   * @param new data container name
   */
  void setName(const QString& aName);

  /**
   * @brief Get the data container flag (Python Binding)
   */
  uint8_t getFlag() const;

  /**
   * @brief Set the data container flag (Python Binding)
   * @param new data container flag
   */
  void setFlag(uint8_t flag);

  /**
   * @brief set DataContainer Type
   * @param dType
   */
  void setDCType(uint32_t dType);

  /**
   * @brief getDCType
   * @return
   */
  uint32_t getDCType() const;

  /**
   * @brief Toggle the data container flag (Python Binding)
   */
  void toggleFlag();

  /**
   * @brief Get the Attribute Matrix proxy
   * @param the name of the attribute matrix
   * @return the attribute matrix proxy
   */
  AttributeMatrixProxy& getAttributeMatrixProxy(const QString& m_Name);

  /**
   * @brief setFlags
   * @param flag
   * @param amTypes
   * @param primitiveTypes
   * @param compDimsVector
   */
  void setFlags(uint8_t m_Flag, AttributeMatrixProxy::AMTypeFlags amTypes = AttributeMatrixProxy::Any_AMType, DataArrayProxy::PrimitiveTypeFlags primitiveTypes = DataArrayProxy::Any_PType,
                const DataArrayProxy::CompDimsVector& compDimsVector = DataArrayProxy::CompDimsVector());

  /**
   * @brief Updates the proxy to match a renamed DataArrayPath
   * @param renamePath
   */
  void updatePath(DataArrayPath::RenameType renamePath);

private:
  uint8_t m_Flag = Qt::Unchecked;
  QString m_Name;
  uint32_t m_DCType = static_cast<uint32_t>(IGeometry::Type::Any);
  StorageType m_AttributeMatrices;

  /**
   * @brief writeMap
   * @param map
   * @return
   */
  QJsonArray writeMap(const QMap<QString, AttributeMatrixProxy>& map) const;

  /**
   * @brief readMap
   * @param jsonArray
   * @return
   */
  QMap<QString, AttributeMatrixProxy> readMap(QJsonArray jsonArray);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(DataContainerProxy::DCGeometryTypeFlags)

Q_DECLARE_METATYPE(DataContainerProxy)
