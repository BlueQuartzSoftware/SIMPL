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

#ifndef _attributematrixproxy_h_
#define _attributematrixproxy_h_

#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QMap>


#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataArrayProxy.h"

class SIMPLib_EXPORT AttributeMatrixProxy
{
  public:
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
     * @brief setCheckState
     * @param checkState
     * @param recursive
     */
    void setFlag(uint8_t flag, bool recursive = false);

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
Q_DECLARE_METATYPE(AttributeMatrixProxy)

#endif /* _AttributeMatrixProxy_H_ */

