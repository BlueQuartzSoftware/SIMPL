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

#ifndef _datacontainerarrayproxy_h_
#define _datacontainerarrayproxy_h_



#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/DataContainerProxy.h"

class DataContainerArray;

/**
 * @brief DataContainerArrayProxy
 */
class SIMPLib_EXPORT DataContainerArrayProxy
{
  typedef QVector<QVector<size_t> > CompDimsVector;

  public:

    /**
     * @brief DataContainerArrayProxy
     */
    DataContainerArrayProxy();

    /**
     * @brief DataContainerArrayProxy
     * @param dca
     */
    explicit DataContainerArrayProxy(DataContainerArray* dca);

    /**
     * @brief DataContainerArrayProxy
     * @param rhs
     */
    DataContainerArrayProxy(const DataContainerArrayProxy& rhs);

    /**
     * @brief ~DataContainerArrayProxy
     */
    virtual ~DataContainerArrayProxy();

    // This enumeration is not a class enumeration because it is not possible to
    // do a bit-wise NOT operation on a class enumeration value.  We need to be
    // able to do a bit-wise NOT operation so that we can turn off certain flags.
    // This enumeration allows us to flip integer bits to turn on/off various types.
    enum PrimitiveTypeFlag : unsigned int {
      None_PType = 0x0,
      Bool_PType = 0x1,
      Float_PType = 0x2,
      Double_PType = 0x4,
      Int8_PType = 0x8,
      UInt8_PType = 0x10,
      Int16_PType = 0x20,
      UInt16_PType = 0x40,
      Int32_PType = 0x80,
      UInt32_PType = 0x100,
      Int64_PType = 0x200,
      UInt64_PType = 0x400,
      StatsDataArray_PType = 0x800,
      NeighborList_PType = 0x1000,
      StringArray_PType = 0x2000,
      Unknown_PType = 0x4000,
      NumericalPrimitives_PType = 0x07FE,
      Any_PType = 0x7FFF
    };
    Q_DECLARE_FLAGS(PrimitiveTypeFlags, PrimitiveTypeFlag)

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

    // This enumeration is not a class enumeration because it is not possible to
    // do a bit-wise NOT operation on a class enumeration value.  We need to be
    // able to do a bit-wise NOT operation so that we can turn off certain flags.
    // This enumeration allows us to flip integer bits to turn on/off various types.
    enum DCGeometryTypeFlag : unsigned int {
      None_DCGeomType = 0x0,
      Image_DCGeomType = 0x1,
      RectGrid_DCGeomType = 0x2,
      Vertex_DCGeomType = 0x4,
      Edge_DCGeomType = 0x8,
      Triangle_DCGeomType = 0x10,
      Quad_DCGeomType = 0x20,
      Tetrahedral_DCGeomType = 0x40,
      Unknown_DCGeomType = 0x80,
      Any_DCGeomType = 0xFF
    };
    Q_DECLARE_FLAGS(DCGeometryTypeFlags, DCGeometryTypeFlag)

    /**
     * @brief MergeProxies
     * @param fileProxy
     * @param cacheProxy
     * @return
     */
    static DataContainerArrayProxy MergeProxies(DataContainerArrayProxy fileProxy, DataContainerArrayProxy cacheProxy);

    /**
     * @brief operator =
     * @param rhs
     */
    void operator=(const DataContainerArrayProxy& rhs);

    /**
     * @brief operator ==
     * @param rhs
     */
    bool operator==(const DataContainerArrayProxy &rhs) const;

    /**
     * @brief operator ==
     * @param rhs
     */
    bool operator!=(const DataContainerArrayProxy &rhs) const;

    /**
     * @brief flattenHeirarchy
     * @param dcFlag
     * @param amFlag
     * @param daFlag
     * @return
     */
    QStringList flattenHeirarchy(Qt::CheckState dcFlag = Qt::Checked, Qt::CheckState amFlag = Qt::Checked, Qt::CheckState daFlag = Qt::Checked);

    /**
     * @brief Print the Heirarchy and attributes of the Proxy
     * @param out
     */
    void print(const QString header = QString(""));

    /**
     * @brief Sets the flags of the proxy items that match the geometry, attribute matrix type, primitive type, and number of components flags that are input as parameters
     * @param flagValue Whether to flag or unflag the matched data containers, attribute matrices, and data arrays
     * @param dcGeoms Data containers with the geometries specified with this flag combination will be flagged/unflagged in the proxy
     * @param amTypes Attribute matrices with the attribute matrix types specified with this flag combination will be flagged/unflagged in the proxy
     * @param primitiveTypes Data arrays with the primitive types specified with this flag combination will be flagged/unflagged in the proxy
     * @param compDimsVector Data arrays with component dimensions in this vector will be flagged/unflagged in the proxy.  If the vector is empty,
     * then any component dimensions are flagged/unflagged.
     */
    void setFlags(Qt::CheckState flagValue, DCGeometryTypeFlags dcGeoms = Any_DCGeomType, AMTypeFlags amTypes = Any_AMType, PrimitiveTypeFlags primitiveTypes = Any_PType, CompDimsVector compDimsVector = CompDimsVector());

    /**
     * @brief reverseFlags
     */
    void reverseFlags();

    /**
     * @brief serialize
     * @return
     */
    QStringList serialize();

    /**
     * @brief Does this contain a DataContainer for the given name
     * @param name
     * @return
     */
    bool contains(const QString& name);

    /**
     * @brief getDataContainerProxy This will return the DataContainerProxy associated
     * with the name given. If a DataContainerProxy with the given name does NOT
     * exist in the DataContainerArrayProxy then a new one will be inserted. If this
     * is NOT what you want, then use the @see contains(QString) function first
     * to make sure the DataContainerArray has the DataContainer that you are
     * looking for.
     * @param name The name of the DataContainer.
     * @return
     */
    DataContainerProxy& getDataContainerProxy(const QString& name);

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

    //----- Our variables, publicly available
    QMap<QString, DataContainerProxy>  dataContainers;

  private:

    /**
    * @brief Writes the contents of the map to a json array
    * @param map
    * @return QJsonArray
    */
    QJsonArray writeMap(QMap<QString, DataContainerProxy> map) const;

    /**
    * @brief Reads the contents of the json array to a QMap
    * @param jsonArray
    * @return
    */
    QMap<QString, DataContainerProxy> readMap(QJsonArray jsonArray);

    /**
     * @brief Returns the appropriate flag for the geometry type
     * @param geoType The data container geometry type
     * @return
     */
    DCGeometryTypeFlag geometryTypeToFlag(IGeometry::Type geoType);

    /**
     * @brief Returns the appropriate flag for the attribute matrix type
     * @param amType The attribute matrix type
     * @return
     */
    AMTypeFlag attributeMatrixTypeToFlag(AttributeMatrix::Type amType);

    /**
     * @brief Returns the appropriate flag for the primitive type
     * @param pType The primitive type
     * @return
     */
    PrimitiveTypeFlag primitiveTypeToFlag(const QString &pType);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DataContainerArrayProxy::PrimitiveTypeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(DataContainerArrayProxy::AMTypeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(DataContainerArrayProxy::DCGeometryTypeFlags)

Q_DECLARE_METATYPE(DataContainerArrayProxy)

#endif /* _DataContainerArrayProxy_H_ */

