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

#include "AbstractFilterParametersReader.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilterParametersReader::AbstractFilterParametersReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilterParametersReader::~AbstractFilterParametersReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbstractFilterParametersReader::readString(const QString name, QString value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> AbstractFilterParametersReader::readStrings(const QString name, QVector<QString> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QVector<QString>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList AbstractFilterParametersReader::readStringList(const QString name, QStringList value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QStringList();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int8_t AbstractFilterParametersReader::readValue(const QString name, int8_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int16_t AbstractFilterParametersReader::readValue(const QString name, int16_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t AbstractFilterParametersReader::readValue(const QString name, int32_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t AbstractFilterParametersReader::readValue(const QString name, int64_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint8_t AbstractFilterParametersReader::readValue(const QString name, uint8_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint16_t AbstractFilterParametersReader::readValue(const QString name, uint16_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t AbstractFilterParametersReader::readValue(const QString name, uint32_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t AbstractFilterParametersReader::readValue(const QString name, uint64_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float AbstractFilterParametersReader::readValue(const QString name, float value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double AbstractFilterParametersReader::readValue(const QString name, double value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilterParametersReader::readValue(const QString name, bool value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Type AbstractFilterParametersReader::readValue(const QString name, AttributeMatrix::Type value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return AttributeMatrix::Type::Unknown;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int8_t> AbstractFilterParametersReader::readArray(const QString name, QVector<int8_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QVector<int8_t>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int16_t> AbstractFilterParametersReader::readArray(const QString name, QVector<int16_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QVector<int16_t>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int32_t> AbstractFilterParametersReader::readArray(const QString name, QVector<int32_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QVector<int32_t>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int64_t> AbstractFilterParametersReader::readArray(const QString name, QVector<int64_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QVector<int64_t>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint8_t> AbstractFilterParametersReader::readArray(const QString name, QVector<uint8_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QVector<uint8_t>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint16_t> AbstractFilterParametersReader::readArray(const QString name, QVector<uint16_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QVector<uint16_t>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint32_t> AbstractFilterParametersReader::readArray(const QString name, QVector<uint32_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QVector<uint32_t>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint64_t> AbstractFilterParametersReader::readArray(const QString name, QVector<uint64_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QVector<uint64_t>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float> AbstractFilterParametersReader::readArray(const QString name, QVector<float> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QVector<float>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> AbstractFilterParametersReader::readArray(const QString name, QVector<double> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return QVector<double>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3_t AbstractFilterParametersReader::readIntVec3(const QString name, IntVec3_t defaultValue)
{
  Q_UNUSED(name)
  Q_UNUSED(defaultValue)

  // This should never be executed
  return IntVec3_t();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3_t AbstractFilterParametersReader::readFloatVec3(const QString name, FloatVec3_t defaultValue)
{
  Q_UNUSED(name)
  Q_UNUSED(defaultValue)

  // This should never be executed
  return FloatVec3_t();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Float2ndOrderPoly_t AbstractFilterParametersReader::readFloat2ndOrderPoly(const QString name, Float2ndOrderPoly_t defaultValue)
{
  Q_UNUSED(name)
  Q_UNUSED(defaultValue)

  // This should never be executed
  return Float2ndOrderPoly_t();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Float3rdOrderPoly_t AbstractFilterParametersReader::readFloat3rdOrderPoly(const QString name, Float3rdOrderPoly_t defaultValue)
{
  Q_UNUSED(name)
  Q_UNUSED(defaultValue)

  // This should never be executed
  return Float3rdOrderPoly_t();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Float4thOrderPoly_t AbstractFilterParametersReader::readFloat4thOrderPoly(const QString name, Float4thOrderPoly_t defaultValue)
{
  Q_UNUSED(name)
  Q_UNUSED(defaultValue)

  // This should never be executed
  return Float4thOrderPoly_t();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfo_t AbstractFilterParametersReader::readFileListInfo(const QString name, FileListInfo_t defaultValue)
{
  Q_UNUSED(name)
  Q_UNUSED(defaultValue)

  // This should never be executed
  return FileListInfo_t();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInput_t AbstractFilterParametersReader::readComparisonInput(const QString name, ComparisonInput_t defaultValue, int vectorPos)
{
  Q_UNUSED(name)
  Q_UNUSED(defaultValue)
  Q_UNUSED(vectorPos)

  // This should never be executed
  return ComparisonInput_t();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInputs AbstractFilterParametersReader::readComparisonInputs(const QString name, ComparisonInputs defValue)
{
  Q_UNUSED(name)
  Q_UNUSED(defValue)

  // This should never be executed
  return ComparisonInputs();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInputsAdvanced AbstractFilterParametersReader::readComparisonInputsAdvanced(const QString name, ComparisonInputsAdvanced defValue)
{
  Q_UNUSED(name)
  Q_UNUSED(defValue)

  // This should never be executed
  return ComparisonInputsAdvanced();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleInput_t AbstractFilterParametersReader::readAxisAngle(const QString name, AxisAngleInput_t v, int vectorPos)
{
  Q_UNUSED(name)
  Q_UNUSED(v)
  Q_UNUSED(vectorPos)

  // This should never be executed
  return AxisAngleInput_t();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AxisAngleInput_t> AbstractFilterParametersReader::readAxisAngles(const QString name, QVector<AxisAngleInput_t> v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return QVector<AxisAngleInput_t>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> AbstractFilterParametersReader::readArraySelections(const QString name, QSet<QString> v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return QSet<QString>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy AbstractFilterParametersReader::readDataContainerArrayProxy(const QString& name, DataContainerArrayProxy defValue)
{
  Q_UNUSED(name)
  Q_UNUSED(defValue)

  // This should never be executed
  return DataContainerArrayProxy();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath AbstractFilterParametersReader::readDataArrayPath(const QString& name, DataArrayPath def)
{
  Q_UNUSED(name)
  Q_UNUSED(def)

  // This should never be executed
  return DataArrayPath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<DataArrayPath> AbstractFilterParametersReader::readDataArrayPathVector(const QString& name, QVector<DataArrayPath> def)
{
  Q_UNUSED(name)
  Q_UNUSED(def)

  // This should never be executed
  return QVector<DataArrayPath>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData AbstractFilterParametersReader::readDynamicTableData(const QString& name, DynamicTableData def)
{
  Q_UNUSED(name)
  Q_UNUSED(def)

  // This should never be executed
  return DynamicTableData();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPair<double, double> AbstractFilterParametersReader::readPairOfDoubles(const QString& name, QPair<double, double> v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return QPair<double, double>();
}
