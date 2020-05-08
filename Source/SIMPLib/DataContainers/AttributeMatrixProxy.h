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

#include <QtCore/QMap>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataArrayProxy.h"

class SIMPLib_EXPORT AttributeMatrixProxy
{
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(AttributeMatrixProxy)
  PYB11_CREATION()
  PYB11_PROPERTY(StorageType DataArrays READ getDataArrays WRITE setDataArrays CONST_GET_OVERLOAD)
  PYB11_PROPERTY(QString Name READ getName WRITE setName)
  PYB11_PROPERTY(AMType AMType READ getAMType WRITE setAMType)
  PYB11_PROPERTY(uint8_t Flag READ getFlag WRITE setFlag)
  PYB11_METHOD(DataArrayProxy getDataArrayProxy ARGS name RETURN_VALUE_POLICY py::return_value_policy::reference)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  // This enumeration is not a class enumeration because it is not possible to
  // do a bit-wise NOT operation on a class enumeration value.  We need to be
  // able to do a bit-wise NOT operation so that we can turn off certain flags.
  // This enumeration allows us to flip integer bits to turn on/off various types.
  enum AMTypeFlag : unsigned int
  {
    None_AMType = 0x0,
    Vertex_AMType = 0x1,
    Face_AMType = 0x2,
    Cell_AMType = 0x4,
    VertexFeature_AMType = 0x8,
    Edge_AMType = 0x10,
    EdgeFeature_AMType = 0x20,
    FaceFeature_AMType = 0x40,
    CellFeature_AMType = 0x80,
    VertexEnsemble_AMType = 0x100,
    EdgeEnsemble_AMType = 0x200,
    FaceEnsemble_AMType = 0x400,
    CellEnsemble_AMType = 0x800,
    MetaData_AMType = 0x1000,
    Generic_AMType = 0x2000,
    Unknown_AMType = 0x4000,
    Any_AMType = 0x7FFF
  };
  Q_DECLARE_FLAGS(AMTypeFlags, AMTypeFlag)

  using StorageType = QMap<QString, DataArrayProxy>;

  /**
   * @brief AttributeMatrixProxy
   */
  AttributeMatrixProxy();

  /**
   * @brief AttributeMatrixProxy
   * @param am_name
   * @param read_am
   * @param am_type
   */
  AttributeMatrixProxy(const QString& am_name, uint8_t read_am = Qt::Checked, AttributeMatrix::Type am_type = AttributeMatrix::Type::Unknown);

  /**
   * @brief Copy Constructor
   */
  AttributeMatrixProxy(const AttributeMatrixProxy& amp);

  virtual ~AttributeMatrixProxy();

  AttributeMatrixProxy(AttributeMatrixProxy&&) noexcept;

  AttributeMatrixProxy& operator=(AttributeMatrixProxy&&) = delete; // Move Assignment Not Implemented

  /**
   * @brief Returns the appropriate flag for the attribute matrix type
   * @param amType The attribute matrix type
   * @return
   */
  static AMTypeFlag AttributeMatrixTypeToFlag(AttributeMatrix::Type m_AMType);

  /**
   * @brief operator = method
   */
  AttributeMatrixProxy& operator=(const AttributeMatrixProxy& amp);

  /**
   * @brief operator == method
   */
  bool operator==(const AttributeMatrixProxy& amp) const;

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
   * @brief setFlags
   * @param flag
   * @param primitiveTypes
   * @param compDimsVector
   */
  void setFlags(uint8_t flag, DataArrayProxy::PrimitiveTypeFlags primitiveTypes = DataArrayProxy::Any_PType, DataArrayProxy::CompDimsVector compDimsVector = DataArrayProxy::CompDimsVector());

  /**
   * @brief Updates the proxy to match a renamed DataArrayPath
   * @param renamePath
   */
  void updatePath(DataArrayPath::RenameType renamePath);

  /**
   * @brief Get the attribute matrices (Python Binding)
   */
  QMap<QString, DataArrayProxy>& getDataArrays();
  const StorageType& getDataArrays() const;

  /**
   * @brief Get the attribute matrices(Python Binding)
   * @param new DataArrayProxy map
   */
  void setDataArrays(const QMap<QString, DataArrayProxy>& proxies);

  /**
   * @brief Get the Data Array proxy
   * @param the name of the data array
   * @return the data array proxy
   */
  DataArrayProxy& getDataArrayProxy(const QString& m_Name);

  /**
   * @brief insertDataArray
   * @param name
   * @param proxy
   */
  void insertDataArray(const QString& m_Name, const DataArrayProxy& proxy);

  /**
   * @brief Get the attribute matrix name (Python Binding)
   */
  QString getName() const;

  /**
   * @brief Set the attribute matrix name (Python Binding)
   * @param new attribute matrix name
   */
  void setName(const QString& m_Name);

  /**
   * @brief Get the attribute matrix type (Python Binding)
   */
  AttributeMatrix::Type getAMType() const;

  /**
   * @brief Set the attribute matrix name (Python Binding)
   * @param new attribute matrix type
   */
  void setAMType(AttributeMatrix::Type);

  /**
   * @brief Get the attribute matrix flag (Python Binding)
   */
  uint8_t getFlag() const;

  /**
   * @brief Set the attribute matrix flag (Python Binding)
   * @param new attribute matrix flag
   */
  void setFlag(uint8_t);

private:
  uint8_t m_Flag = Qt::Unchecked;
  QString m_Name;
  AttributeMatrix::Type m_AMType = AttributeMatrix::Type::Unknown;
  StorageType m_DataArrays;

  /**
   * @brief writeMap
   * @param map
   * @return
   */
  QJsonArray writeMap(const QMap<QString, DataArrayProxy>& map) const;

  /**
   * @brief readMap
   * @param jsonArray
   * @return
   */
  QMap<QString, DataArrayProxy> readMap(QJsonArray jsonArray);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(AttributeMatrixProxy::AMTypeFlags)

Q_DECLARE_METATYPE(AttributeMatrixProxy)
