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


#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataArrayProxy.h"

class SIMPLib_EXPORT AttributeMatrixProxy
{
  public:

    // This enumeration is not a class enumeration because it is not possible to
    // do a bit-wise NOT operation on a class enumeration value.  We need to be
    // able to do a bit-wise NOT operation so that we can turn off certain flags.
    // This enumeration allows us to flip integer bits to turn on/off various types.
    enum AMTypeFlag : unsigned int {
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
    AttributeMatrixProxy(QString am_name, uint8_t read_am = Qt::Checked, AttributeMatrix::Type am_type = AttributeMatrix::Type::Unknown);

    /**
    * @brief Copy Constructor
    */
    AttributeMatrixProxy(const AttributeMatrixProxy& amp);

    virtual ~AttributeMatrixProxy();

    /**
     * @brief Returns the appropriate flag for the attribute matrix type
     * @param amType The attribute matrix type
     * @return
     */
    static AMTypeFlag AttributeMatrixTypeToFlag(AttributeMatrix::Type amType);

    /**
    * @brief operator = method
    */
    void operator=(const AttributeMatrixProxy& amp);

    /**
    * @brief operator == method
    */
    bool operator==(const AttributeMatrixProxy& amp) const;

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
     * @param primitiveTypes
     * @param compDimsVector
     */
    void setFlags(uint8_t flag, DataArrayProxy::PrimitiveTypeFlags primitiveTypes = DataArrayProxy::Any_PType, DataArrayProxy::CompDimsVector compDimsVector = DataArrayProxy::CompDimsVector());

    /**
     * @brief Updates the proxy to match a renamed DataArrayPath
     * @param renamePath
     */
    void updatePath(DataArrayPath::RenameType renamePath);

    //----- Our variables, publicly available
    uint8_t flag;
    QString name;
    AttributeMatrix::Type amType;
    QMap<QString, DataArrayProxy> dataArrays;

  private:

    /**
     * @brief writeMap
     * @param map
     * @return
     */
    QJsonArray writeMap(QMap<QString, DataArrayProxy> map);

    /**
     * @brief readMap
     * @param jsonArray
     * @return
     */
    QMap<QString, DataArrayProxy> readMap(QJsonArray jsonArray);

};
Q_DECLARE_OPERATORS_FOR_FLAGS(AttributeMatrixProxy::AMTypeFlags)

Q_DECLARE_METATYPE(AttributeMatrixProxy)


