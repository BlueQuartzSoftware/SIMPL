/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MontageSelection.h"

#include <algorithm>

namespace
{
const QString RowChar = "r";
const QString ColChar = "c";
const QString Prefix = "PrefixStr";
const QString Suffix = "SuffixStr";
const QString Padding = "Padding";
const QString RowStart = "RowStart";
const QString RowEnd = "RowEnd";
const QString ColStart = "ColStart";
const QString ColEnd = "ColEnd";
} // namespace

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSelection::MontageSelection()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSelection::MontageSelection(const QString& prefix, const QString& suffix, int padding, int rowStart, int rowEnd, int colStart, int colEnd)
: m_Prefix(prefix)
, m_Suffix(suffix)
, m_Padding(padding)
, m_RowStart(rowStart)
, m_RowEnd(rowEnd)
, m_ColStart(colStart)
, m_ColEnd(colEnd)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSelection MontageSelection::Create(const std::string& prefix, const std::string& suffix, int padding, int rowStart, int rowEnd, int colStart, int colEnd)
{
  QString prefixStr = QString::fromStdString(prefix);
  QString suffixStr = QString::fromStdString(suffix);
  return MontageSelection(prefixStr, suffixStr, padding, rowStart, rowEnd, colStart, colEnd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSelection::~MontageSelection() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MontageSelection::getPrefix() const
{
  return m_Prefix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MontageSelection::getSuffix() const
{
  return m_Suffix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelection::setPrefix(const QString& prefix)
{
  m_Prefix = prefix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelection::setSuffix(const QString& suffix)
{
  m_Suffix = suffix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MontageSelection::getPadding() const
{
  return m_Padding;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelection::setPadding(int padding)
{
  m_Padding = std::max(padding, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MontageSelection::getRowStart() const
{
  return m_RowStart;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelection::setRowStart(int rowStart)
{
  m_RowStart = rowStart;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MontageSelection::getRowEnd() const
{
  return m_RowEnd;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelection::setRowEnd(int rowEnd)
{
  m_RowEnd = std::max(rowEnd, m_RowStart);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MontageSelection::getColStart() const
{
  return m_ColStart;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelection::setColStart(int colStart)
{
  m_ColStart = std::max(colStart, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MontageSelection::getColEnd() const
{
  return m_ColEnd;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelection::setColEnd(int colEnd)
{
  m_ColEnd = std::max(colEnd, m_ColStart);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelection::setRowRange(int rowStart, int rowEnd)
{
  setRowStart(rowStart);
  setRowEnd(rowEnd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelection::setColRange(int colStart, int colEnd)
{
  setColStart(colStart);
  setColEnd(colEnd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MontageSelection::getDataContainerName(int row, int col) const
{
  const QString rowStr = QString::number(row).rightJustified(m_Padding, '0');
  const QString colStr = QString::number(col).rightJustified(m_Padding, '0');
  return m_Prefix + ::RowChar + rowStr + ::ColChar + colStr + m_Suffix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList MontageSelection::getDataContainerNamesCombOrder() const
{
  QStringList dcNames;
  for(int row = m_RowStart; row <= m_RowEnd; row++)
  {
    const QString rowStr = QString::number(row).rightJustified(m_Padding, '0');
    for(int col = m_ColStart; col <= m_ColEnd; col++)
    {
      const QString colStr = QString::number(col).rightJustified(m_Padding, '0');
      dcNames.push_back(m_Prefix + ::RowChar + rowStr + ::ColChar + colStr + m_Suffix);
    }
  }
  return dcNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList MontageSelection::getDataContainerNamesSnakeOrder() const
{
  QStringList dcNames;
  bool snakeBack = false;
  for(int row = m_RowStart; row <= m_RowEnd; row++)
  {
    const QString rowStr = QString::number(row).rightJustified(m_Padding, '0');
    if(snakeBack)
    {
      for(int col = m_ColEnd; col <= m_ColStart; col--)
      {
        const QString colStr = QString::number(col).rightJustified(m_Padding, '0');
        dcNames.push_back(m_Prefix + ::RowChar + rowStr + ::ColChar + colStr + m_Suffix);
      }
    }
    else
    {
      for(int col = m_ColStart; col <= m_ColEnd; col++)
      {
        const QString colStr = QString::number(col).rightJustified(m_Padding, '0');
        dcNames.push_back(m_Prefix + ::RowChar + rowStr + ::ColChar + colStr + m_Suffix);
      }
    }
    snakeBack = !snakeBack;
  }
  return dcNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject MontageSelection::toJson() const
{
  QJsonObject object;
  object[::Prefix] = getPrefix();
  object[::Suffix] = getSuffix();
  object[::Padding] = getPadding();
  object[::RowStart] = getRowStart();
  object[::RowEnd] = getRowEnd();
  object[::ColStart] = getColStart();
  object[::ColEnd] = getColEnd();

  return object;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSelection::fromJson(const QJsonObject& json)
{
  setPrefix(json[::Prefix].toString());
  setSuffix(json[::Suffix].toString());
  setPadding(json[::Padding].toInt());
  setRowStart(json[::RowStart].toInt());
  setRowEnd(json[::RowEnd].toInt());
  setColStart(json[::ColStart].toInt());
  setColEnd(json[::ColEnd].toInt());
}
