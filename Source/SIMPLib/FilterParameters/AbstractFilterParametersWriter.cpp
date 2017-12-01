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

#include "AbstractFilterParametersWriter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilterParametersWriter::AbstractFilterParametersWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilterParametersWriter::~AbstractFilterParametersWriter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, const QString value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, const QVector<QString> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, const QStringList value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, int8_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, int16_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, int32_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, int64_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, uint8_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, uint16_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, uint32_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, uint64_t value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, float value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, double value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, QVector<int8_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, QVector<int16_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, QVector<int32_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, QVector<int64_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, QVector<uint8_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, QVector<uint16_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, QVector<uint32_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, QVector<uint64_t> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, QVector<float> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, QVector<double> value)
{
  Q_UNUSED(name)
  Q_UNUSED(value)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, IntVec3_t v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, FloatVec3_t v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, Float2ndOrderPoly_t v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, Float3rdOrderPoly_t v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, Float4thOrderPoly_t v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, FileListInfo_t v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, ComparisonInput_t comp)
{
  Q_UNUSED(name)
  Q_UNUSED(comp)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, ComparisonInputs v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, AxisAngleInput_t v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
// These are actually written as a binary 4x1 float array as an attribute
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, AxisAngleInput_t v, int vectorPos)
{
  Q_UNUSED(name)
  Q_UNUSED(vectorPos)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, QVector<AxisAngleInput_t> v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeArraySelections(const QString name, QSet<QString> v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, const DataContainerArrayProxy& v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, const DataArrayPath& v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, const QVector<DataArrayPath>& v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, const DynamicTableData& v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilterParametersWriter::writeValue(const QString name, const QPair<double, double>& v)
{
  Q_UNUSED(name)
  Q_UNUSED(v)

  // This should never be executed
  return -1;
}
