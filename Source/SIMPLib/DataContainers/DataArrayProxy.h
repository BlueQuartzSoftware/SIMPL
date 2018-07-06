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
#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QVector>

#include <hdf5.h>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"

class SIMPLH5DataReaderRequirements;

/**
 * @brief The DataArrayProxy class
 */
class SIMPLib_EXPORT DataArrayProxy
{
public:
    typedef QVector<QVector<size_t> > CompDimsVector;

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

  /**
   * @brief DataArrayProxy
   */
  DataArrayProxy();

  /**
   * @brief DataArrayProxy
   * @param da_path
   * @param da_name
   * @param da_flag
   * @param da_objectType
   * @param da_version
   */
  DataArrayProxy(QString da_path, QString da_name, uint8_t da_flag = SIMPL::Checked, QString da_objectType = "NOT_DEFINED", int da_version = 0);

  /**
  * @brief Copy Constructor
  */
  DataArrayProxy(const DataArrayProxy& rhs);

  /**
   * @brief Returns the appropriate flag for the primitive type
   * @param pType The primitive type
   * @return
   */
  static DataArrayProxy::PrimitiveTypeFlag PrimitiveTypeToFlag(const QString &pType);

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
  bool readJson(const QJsonObject& json);

  /**
   * @brief ReadDataArrayStructure
   * @param attrMatGid
   * @param dataArrays
   * @param h5InternalPath
   */
  static void ReadDataArrayStructure(hid_t attrMatGid, QMap<QString, DataArrayProxy>& dataArrays, SIMPLH5DataReaderRequirements* req, QString h5InternalPath);

  /**
  * @brief Updates the proxy to match a renamed DataArrayPath
  * @param oldPath
  * @param newPath
  */
  void updatePath(DataArrayPath::RenameType renamePath);

  /**
  * @brief operator = method
  */
  void operator=(const DataArrayProxy& rhs);

  /**
  * @brief operator == method
  */
  bool operator==(const DataArrayProxy& rhs) const;

  //----- Our variables, publicly available
  uint8_t flag;
  int version;
  QString path;
  QString name;
  QString objectType;
  QVector<size_t> tupleDims;
  QVector<size_t> compDims;

private:
  QJsonArray writeVector(QVector<size_t> vector);

  QVector<size_t> readVector(QJsonArray jsonArray);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(DataArrayProxy::PrimitiveTypeFlags)

Q_DECLARE_METATYPE(DataArrayProxy)

