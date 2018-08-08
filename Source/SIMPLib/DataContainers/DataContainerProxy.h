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


#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QJsonArray>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/AttributeMatrixProxy.h"
#include "SIMPLib/Geometry/IGeometry.h"

/**
 * @brief The DataContainerProxy class
 */
class SIMPLib_EXPORT DataContainerProxy
{
  public:

    // This enumeration is not a class enumeration because it is not possible to
    // do a bit-wise NOT operation on a class enumeration value.  We need to be
    // able to do a bit-wise NOT operation so that we can turn off certain flags.
    // This enumeration allows us to flip integer bits to turn on/off various types.
    enum DCGeometryTypeFlag : unsigned int {
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
    DataContainerProxy(const QString &dc_name, const uint8_t &read_dc = Qt::Checked, IGeometry::Type dc_type = IGeometry::Type::Unknown);

    /**
    * @brief Copy Constructor
    */
    DataContainerProxy(const DataContainerProxy& amp);
    
    ~DataContainerProxy() = default;

    DataContainerProxy(DataContainerProxy&&) = default;      // Move Constructor Not Implemented
    DataContainerProxy& operator=(DataContainerProxy&&) = delete;      // Move Assignment
    

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
    void writeJson(QJsonObject& json);

    /**
    * @brief Reads the contents of the the json object 'json' into the proxy
    * @param json
    * @return
    */
    bool readJson(QJsonObject& json);

    /**
     * @brief setFlags
     * @param flag
     * @param amTypes
     * @param primitiveTypes
     * @param compDimsVector
     */
    void setFlags(uint8_t flag, AttributeMatrixProxy::AMTypeFlags amTypes = AttributeMatrixProxy::Any_AMType,
                  DataArrayProxy::PrimitiveTypeFlags primitiveTypes = DataArrayProxy::Any_PType, const DataArrayProxy::CompDimsVector &compDimsVector = DataArrayProxy::CompDimsVector());

    /**
     * @brief Updates the proxy to match a renamed DataArrayPath
     * @param renamePath
     */
    void updatePath(DataArrayPath::RenameType renamePath);

    //----- Our variables, publicly available
    uint8_t flag;
    QString name;
    unsigned int dcType;
    QMap<QString, AttributeMatrixProxy> attributeMatricies;

  private:

    /**
     * @brief writeMap
     * @param map
     * @return
     */
    QJsonArray writeMap(QMap<QString, AttributeMatrixProxy> map);

    /**
     * @brief readMap
     * @param jsonArray
     * @return
     */
    QMap<QString, AttributeMatrixProxy> readMap(QJsonArray jsonArray);

};
Q_DECLARE_OPERATORS_FOR_FLAGS(DataContainerProxy::DCGeometryTypeFlags)

Q_DECLARE_METATYPE(DataContainerProxy)




