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
  
  PYB11_CREATE_BINDINGS(DataContainerArrayProxy)

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
    void print(const QString &header = QString(""));

    /**
     * @brief Sets the flags of the proxy items that match the geometry, attribute matrix type, primitive type, and number of components flags that are input as parameters.
     * Flags are only set if their parent flag has been set as well.  For example: a matching attribute matrix's flag will be set if its data container flag has also been set.
     * But its flag won't be set if its data container flag has not been set.
     * @param flag Whether to flag or unflag the matched data containers, attribute matrices, and data arrays
     * @param dcGeoms Data containers with the geometries specified with this flag combination will be flagged/unflagged in the proxy
     * @param amTypes Attribute matrices with the attribute matrix types specified with this flag combination will be flagged/unflagged in the proxy
     * @param primitiveTypes Data arrays with the primitive types specified with this flag combination will be flagged/unflagged in the proxy
     * @param compDimsVector Data arrays with component dimensions in this vector will be flagged/unflagged in the proxy.  If the vector is empty,
     * then any component dimensions are flagged/unflagged.
     */
    void setFlags(uint8_t flag, DataContainerProxy::DCGeometryTypeFlags dcGeoms = DataContainerProxy::Any_DCGeomType,
                  AttributeMatrixProxy::AMTypeFlags amTypes = AttributeMatrixProxy::Any_AMType, DataArrayProxy::PrimitiveTypeFlags primitiveTypes = DataArrayProxy::Any_PType,
                  DataArrayProxy::CompDimsVector compDimsVector = DataArrayProxy::CompDimsVector());

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
     * @brief Updates the proxy to match a renamed DataArrayPath
     * @param renamePath
     */
    void updatePath(DataArrayPath::RenameType renamePath);

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
};

Q_DECLARE_METATYPE(DataContainerArrayProxy)


