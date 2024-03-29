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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DynamicTableData.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QSize>
#include <QtCore/QTextStream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData()
: m_DynamicRows(false)
, m_DynamicCols(false)
, m_MinRows(0)
, m_MinCols(0)
, m_DefaultRowCount(0)
, m_DefaultColCount(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(int nRows, int nCols)
: m_DynamicRows(false)
, m_DynamicCols(false)
, m_MinRows(0)
, m_MinCols(0)
, m_DefaultRowCount(0)
, m_DefaultColCount(0)
{
  std::vector<std::vector<double>> data(nRows, std::vector<double>(nCols, 0));
  m_TableData = data;

  QStringList rHeaders, cHeaders;

  for(int i = 0; i < nRows; i++)
  {
    rHeaders << QString::number(i);
  }

  for(int i = 0; i < nCols; i++)
  {
    cHeaders << QString::number(i);
  }

  m_RowHeaders = rHeaders;
  m_ColHeaders = cHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(int nRows, int nCols, const QStringList& rHeaders, const QStringList& cHeaders)
: m_DynamicRows(false)
, m_DynamicCols(false)
, m_MinRows(0)
, m_MinCols(0)
, m_DefaultRowCount(0)
, m_DefaultColCount(0)
{
  std::vector<std::vector<double>> data(nRows, std::vector<double>(nCols, 0));
  m_TableData = data;

  m_RowHeaders = rHeaders;
  m_ColHeaders = cHeaders;

  // Adjust dimensions if they are not all the same
  checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(const std::vector<std::vector<double>>& data, const QStringList& rHeaders, const QStringList& cHeaders)
: m_DynamicRows(false)
, m_DynamicCols(false)
, m_MinRows(0)
, m_MinCols(0)
, m_DefaultRowCount(0)
, m_DefaultColCount(0)
{
  m_TableData = data;
  m_RowHeaders = rHeaders;
  m_ColHeaders = cHeaders;

  // Adjust dimensions if they are not all the same
  checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData DynamicTableData::Create(const std::vector<std::vector<double>>& dims, const std::list<std::string>& rHeaders, const std::list<std::string>& cHeaders)
{
  QStringList rowHdrs;
  for(const auto& rowHdr : rHeaders)
  {
    rowHdrs.append(QString::fromStdString(rowHdr));
  }
  QStringList colHdrs;
  for(const auto& colHdr : cHeaders)
  {
    colHdrs.append(QString::fromStdString(colHdr));
  }
  return DynamicTableData(dims, rowHdrs, colHdrs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::~DynamicTableData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DynamicTableData::isEmpty()
{
  return !(!m_TableData.empty() || !m_RowHeaders.empty() || m_ColHeaders.empty());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::checkAndAdjustDimensions()
{
  QSize dataSize(m_TableData.size(), 0);
  QSize headerSize(m_RowHeaders.size(), m_ColHeaders.size());

  if(!m_TableData.empty())
  {
    dataSize.setHeight(m_TableData[0].size());
  }

  if(dataSize == headerSize)
  {
    return;
  }

  /* The header dimensions do not equal the data dimensions.
     The data dimensions will be used and will overwrite the current header dimensions.
     This may result in data loss.
  */
  int nRows = dataSize.width();
  int nCols = dataSize.height();

  // If row header dimension is greater than default row dimension, remove the extra headers
  if(m_RowHeaders.size() > nRows)
  {
    while(m_RowHeaders.size() > nRows)
    {
      m_RowHeaders.pop_back();
    }
  }
  // If column header dimension is greater than default column dimension, remove the extra headers
  if(m_ColHeaders.size() > nCols)
  {
    while(m_ColHeaders.size() > nCols)
    {
      m_ColHeaders.pop_back();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableData::serializeData(char delimiter) const
{
  QString str;
  QTextStream ss(&str);

  for(int row = 0; row < m_TableData.size(); row++)
  {
    for(int col = 0; col < m_TableData[row].size(); col++)
    {
      ss << m_TableData[row][col] << delimiter;
    }
  }
  str.chop(1); // Get rid of the last, unnecessary delimiter

  return str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::vector<double>> DynamicTableData::DeserializeData(QString dataStr, int nRows, int nCols, char delimiter)
{
  std::vector<std::vector<double>> data(nRows, std::vector<double>(nCols));
  int row = 0, col = 0;

  if(dataStr.isEmpty())
  {
    return data;
  }

  int start = 0;
  int tokenIndex = 0;

  while(tokenIndex >= 0)
  {
    tokenIndex = dataStr.indexOf(delimiter, start);
    QString valueStr = dataStr.mid(start, tokenIndex - start);
    double value = valueStr.toDouble();
    data[row][col] = value;
    start = tokenIndex + 1;

    col++;
    if(col == nCols)
    {
      row++;
      col = 0;
    }
  }

  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableData::serializeRowHeaders(char delimiter) const
{
  QString str = "";
  QTextStream ss(&str);

  for(int i = 0; i < m_RowHeaders.size(); i++)
  {
    ss << m_RowHeaders[i];
    ss << delimiter;
  }
  str.chop(1); // Get rid of the last, unnecessary delimiter

  return str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableData::serializeColumnHeaders(char delimiter) const
{
  QString str = "";
  QTextStream ss(&str);

  for(int i = 0; i < m_ColHeaders.size(); i++)
  {
    ss << m_ColHeaders[i];
    ss << delimiter;
  }
  str.chop(1); // Get rid of the last, unnecessary delimiter

  return str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DynamicTableData::DeserializeHeaders(QString headersStr, char delimiter)
{
  QStringList headers;

  if(headersStr.isEmpty())
  {
    return headers;
  }

  int start = 0;
  int tokenIndex = 0;

  while(tokenIndex >= 0)
  {
    tokenIndex = headersStr.indexOf(delimiter, start);
    QString header = headersStr.mid(start, tokenIndex);
    headers.push_back(header);
    start = tokenIndex + 1;
  }

  return headers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> DynamicTableData::flattenData() const
{
  int numRows = getNumRows();
  int numCols = getNumCols();

  QVector<double> flat(numRows * numCols);
  for(int row = 0; row < numRows; row++)
  {
    for(int col = 0; col < numCols; col++)
    {
      flat[row * numCols + col] = m_TableData[row][col];
    }
  }

  return flat;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::vector<double>> DynamicTableData::ExpandData(std::vector<double> orig, size_t nRows, size_t nCols)
{
  std::vector<std::vector<double>> expand(nRows, std::vector<double>(nCols));

  if(orig.size() != nRows * nCols)
  {
    // Something went wrong
    return expand;
  }

  for(size_t row = 0; row < nRows; row++)
  {
    for(size_t col = 0; col < nCols; col++)
    {
      expand[row][col] = orig[row * nCols + col];
    }
  }

  return expand;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::writeJson(QJsonObject& json) const
{
  writeData(json);

  QJsonArray rHeaders;
  for(QString header : m_RowHeaders)
  {
    rHeaders.push_back(header);
  }
  json["Row Headers"] = rHeaders;

  QJsonArray cHeaders;
  for(QString header : m_ColHeaders)
  {
    cHeaders.push_back(header);
  }
  json["Column Headers"] = cHeaders;

  json["HasDynamicRows"] = m_DynamicRows;
  json["HasDynamicCols"] = m_DynamicCols;
  json["MinRowCount"] = m_MinRows;
  json["MinColCount"] = m_MinCols;
  json["DefaultRowCount"] = m_DefaultRowCount;
  json["DefaultColCount"] = m_DefaultColCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DynamicTableData::readJson(QJsonObject& json)
{
  if(json.contains("Dynamic Table Data"))
  {
    QJsonObject obj = json["Dynamic Table Data"].toObject();
    m_TableData = readData(obj);
  }
  else
  {
    m_TableData = readData(json);
  }

  QJsonArray rHeaders = json["Row Headers"].toArray();
  for(QJsonValue val : rHeaders)
  {
    if(val.isString())
    {
      m_RowHeaders.push_back(val.toString());
    }
  }

  QJsonArray cHeaders = json["Column Headers"].toArray();
  for(QJsonValue val : cHeaders)
  {
    if(val.isString())
    {
      m_ColHeaders.push_back(val.toString());
    }
  }

  m_DynamicRows = json["HasDynamicRows"].toBool();
  m_DynamicCols = json["HasDynamicCols"].toBool();
  m_MinRows = json["MinRowCount"].toInt();
  m_MinCols = json["MinColCount"].toInt();
  m_DefaultRowCount = json["DefaultRowCount"].toInt();
  m_DefaultColCount = json["DefaultColCount"].toInt();

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::writeData(QJsonObject& object) const
{
  QJsonArray rows;
  for(const std::vector<double>& vector : m_TableData)
  {
    QJsonArray cols;
    for(double val : vector)
    {
      cols.push_back(val);
    }
    rows.push_back(cols);
  }

  object["Table Data"] = rows;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::vector<double>> DynamicTableData::readData(QJsonObject object)
{
  std::vector<std::vector<double>> data;
  if(object["Table Data"].isArray())
  {
    QJsonArray rowArray = object["Table Data"].toArray();
    data.resize(rowArray.size());

    for(int row = 0; row < rowArray.size(); row++)
    {
      QJsonValue rowObj = rowArray.at(row);
      if(rowObj.isArray())
      {
        QJsonArray colArray = rowObj.toArray();
        data[row].resize(colArray.size());

        for(int col = 0; col < colArray.size(); col++)
        {
          QJsonValue colObj = colArray.at(col);

          if(colObj.isDouble())
          {
            data[row][col] = colObj.toDouble();
          }
        }
      }
    }
    return data;
  }
  return std::vector<std::vector<double>>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::vector<double>> DynamicTableData::getTableData() const
{
  return m_TableData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setTableData(const std::vector<std::vector<double>>& data)
{
  m_TableData = data;

  // Adjust dimensions
  checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DynamicTableData::getNumRows() const
{
  return static_cast<int>(m_TableData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DynamicTableData::getNumCols() const
{
  if(!m_TableData.empty())
  {
    return static_cast<int>(m_TableData[0].size());
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(const DynamicTableData& rhs)
{
  m_TableData = rhs.m_TableData;
  m_RowHeaders = rhs.m_RowHeaders;
  m_ColHeaders = rhs.m_ColHeaders;
  m_DynamicRows = rhs.m_DynamicRows;
  m_DynamicCols = rhs.m_DynamicCols;
  m_MinRows = rhs.m_MinRows;
  m_MinCols = rhs.m_MinCols;
  m_DefaultRowCount = rhs.m_DefaultRowCount;
  m_DefaultColCount = rhs.m_DefaultColCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData& DynamicTableData::operator=(const DynamicTableData& rhs)
{
  m_TableData = rhs.m_TableData;
  m_RowHeaders = rhs.m_RowHeaders;
  m_ColHeaders = rhs.m_ColHeaders;
  m_DynamicRows = rhs.m_DynamicRows;
  m_DynamicCols = rhs.m_DynamicCols;
  m_MinRows = rhs.m_MinRows;
  m_MinCols = rhs.m_MinCols;
  m_DefaultRowCount = rhs.m_DefaultRowCount;
  m_DefaultColCount = rhs.m_DefaultColCount;
  return *this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DynamicTableData::operator==(const DynamicTableData& rhs) const
{
  if(m_RowHeaders == rhs.m_RowHeaders && m_ColHeaders == rhs.m_ColHeaders && m_DynamicRows == rhs.m_DynamicRows && m_DynamicCols == rhs.m_DynamicCols && m_MinRows == rhs.m_MinRows &&
     m_MinCols == rhs.m_MinCols && m_DefaultRowCount == rhs.m_DefaultRowCount && m_DefaultColCount == rhs.m_DefaultColCount)
  {
    for(int i = 0; i < m_TableData.size(); i++)
    {
      for(int j = 0; j < m_TableData[i].size(); j++)
      {
        if(m_TableData[i][j] != rhs.m_TableData[i][j])
        {
          return false;
        }
      }
    }
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DynamicTableData::operator!=(const DynamicTableData& rhs) const
{
  if(m_RowHeaders == rhs.m_RowHeaders && m_ColHeaders == rhs.m_ColHeaders && m_DynamicRows == rhs.m_DynamicRows && m_DynamicCols == rhs.m_DynamicCols && m_MinRows == rhs.m_MinRows &&
     m_MinCols == rhs.m_MinCols && m_DefaultRowCount == rhs.m_DefaultRowCount && m_DefaultColCount == rhs.m_DefaultColCount)
  {
    for(int i = 0; i < m_TableData.size(); i++)
    {
      for(int j = 0; j < m_TableData[i].size(); j++)
      {
        if(m_TableData[i][j] != rhs.m_TableData[i][j])
        {
          return true;
        }
      }
    }
  }
  else
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
void DynamicTableData::setColHeaders(const QStringList& value)
{
  m_ColHeaders = value;
}

// -----------------------------------------------------------------------------
QStringList DynamicTableData::getColHeaders() const
{
  return m_ColHeaders;
}

// -----------------------------------------------------------------------------
void DynamicTableData::setRowHeaders(const QStringList& value)
{
  m_RowHeaders = value;
}

// -----------------------------------------------------------------------------
QStringList DynamicTableData::getRowHeaders() const
{
  return m_RowHeaders;
}

// -----------------------------------------------------------------------------
void DynamicTableData::setDynamicRows(bool value)
{
  m_DynamicRows = value;
}

// -----------------------------------------------------------------------------
bool DynamicTableData::getDynamicRows() const
{
  return m_DynamicRows;
}

// -----------------------------------------------------------------------------
void DynamicTableData::setDynamicCols(bool value)
{
  m_DynamicCols = value;
}

// -----------------------------------------------------------------------------
bool DynamicTableData::getDynamicCols() const
{
  return m_DynamicCols;
}

// -----------------------------------------------------------------------------
void DynamicTableData::setMinRows(int value)
{
  m_MinRows = value;
}

// -----------------------------------------------------------------------------
int DynamicTableData::getMinRows() const
{
  return m_MinRows;
}

// -----------------------------------------------------------------------------
void DynamicTableData::setMinCols(int value)
{
  m_MinCols = value;
}

// -----------------------------------------------------------------------------
int DynamicTableData::getMinCols() const
{
  return m_MinCols;
}

// -----------------------------------------------------------------------------
void DynamicTableData::setDefaultRowCount(int value)
{
  m_DefaultRowCount = value;
}

// -----------------------------------------------------------------------------
int DynamicTableData::getDefaultRowCount() const
{
  return m_DefaultRowCount;
}

// -----------------------------------------------------------------------------
void DynamicTableData::setDefaultColCount(int value)
{
  m_DefaultColCount = value;
}

// -----------------------------------------------------------------------------
int DynamicTableData::getDefaultColCount() const
{
  return m_DefaultColCount;
}
