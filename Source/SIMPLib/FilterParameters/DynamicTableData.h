/* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
#pragma once

#include <QtCore/QObject>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

/**
 * @brief
 * @author
 * @version
 */
class SIMPLib_EXPORT DynamicTableData : public QObject
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(DynamicTableData)
  PYB11_STATIC_CREATION(Create ARGS std::vector<std::vector<double>> std::list<std::string> std::list<std::string>)
  PYB11_PROPERTY(QStringList ColHeaders READ getColHeaders WRITE setColHeaders)
  PYB11_PROPERTY(QStringList RowHeaders READ getRowHeaders WRITE setRowHeaders)
  PYB11_PROPERTY(std::vector<std::vector<double>> TableData READ getTableData WRITE setTableData)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  DynamicTableData();
  DynamicTableData(int nRows, int nCols);
  DynamicTableData(int nRows, int nCols, const QStringList& rHeaders, const QStringList& cHeaders);
  DynamicTableData(const std::vector<std::vector<double>>& data, const QStringList& rHeaders = QStringList(), const QStringList& cHeaders = QStringList());

  ~DynamicTableData() override;

  static DynamicTableData Create(const std::vector<std::vector<double>>& dims, const std::list<std::string>& rHeaders, const std::list<std::string>& cHeaders);

  /**
   * @brief Setter property for ColHeaders
   */
  void setColHeaders(const QStringList& value);
  /**
   * @brief Getter property for ColHeaders
   * @return Value of ColHeaders
   */
  QStringList getColHeaders() const;

  /**
   * @brief Setter property for RowHeaders
   */
  void setRowHeaders(const QStringList& value);
  /**
   * @brief Getter property for RowHeaders
   * @return Value of RowHeaders
   */
  QStringList getRowHeaders() const;

  /**
   * @brief Setter property for DynamicRows
   */
  void setDynamicRows(bool value);
  /**
   * @brief Getter property for DynamicRows
   * @return Value of DynamicRows
   */
  bool getDynamicRows() const;

  /**
   * @brief Setter property for DynamicCols
   */
  void setDynamicCols(bool value);
  /**
   * @brief Getter property for DynamicCols
   * @return Value of DynamicCols
   */
  bool getDynamicCols() const;

  /**
   * @brief Setter property for MinRows
   */
  void setMinRows(int value);
  /**
   * @brief Getter property for MinRows
   * @return Value of MinRows
   */
  int getMinRows() const;

  /**
   * @brief Setter property for MinCols
   */
  void setMinCols(int value);
  /**
   * @brief Getter property for MinCols
   * @return Value of MinCols
   */
  int getMinCols() const;

  /**
   * @brief Setter property for DefaultRowCount
   */
  void setDefaultRowCount(int value);
  /**
   * @brief Getter property for DefaultRowCount
   * @return Value of DefaultRowCount
   */
  int getDefaultRowCount() const;

  /**
   * @brief Setter property for DefaultColCount
   */
  void setDefaultColCount(int value);
  /**
   * @brief Getter property for DefaultColCount
   * @return Value of DefaultColCount
   */
  int getDefaultColCount() const;

  /**
   * @brief This deserializes a string of data and returns the original 2D array.
   */
  static std::vector<std::vector<double>> DeserializeData(QString dataStr, int nRows, int nCols, char delimiter);

  /**
   * @brief This does the reverse of the flattenData function.  It expands the data back into a 2D array.
   */
  static std::vector<std::vector<double>> ExpandData(std::vector<double> orig, size_t nRows, size_t nCols);

  /**
   * @brief This deserializes a string of headers and returns the original QStringList.
   */
  static QStringList DeserializeHeaders(QString headersStr, char delimiter);

  /**
   * @brief This returns a serialized string of the data, iterating through columns first.
   */
  QString serializeData(char delimiter) const;

  /**
   * @brief This returns a serialized string of the row headers list.
   */
  QString serializeRowHeaders(char delimiter) const;

  /**
   * @brief This returns a serialized string of the column headers list.
   */
  QString serializeColumnHeaders(char delimiter) const;

  /**
   * @brief This returns a flattened vector of the data.
   */
  QVector<double> flattenData() const;

  /**
   * @brief Writes the contents of the proxy to the json object 'json'
   * @param json
   * @return
   */
  void writeJson(QJsonObject& json) const;

  /**
   * @brief Reads the contents of the json object 'json' into the proxy
   * @param json
   * @return
   */
  bool readJson(QJsonObject& json);

  /**
   * @brief Table data getter and setter
   */
  std::vector<std::vector<double>> getTableData() const;
  void setTableData(const std::vector<std::vector<double>>& data);

  /**
   * @brief Calculates and returns the number of rows
   */
  int getNumRows() const;

  /**
   * @brief Calculates and returns the number of columns
   */
  int getNumCols() const;

  /**
   * @brief Checks if the DynamicTableData object is empty.
   */
  bool isEmpty();

  DynamicTableData(const DynamicTableData& rhs);
  DynamicTableData& operator=(const DynamicTableData& rhs);
  bool operator==(const DynamicTableData& rhs) const;
  bool operator!=(const DynamicTableData& rhs) const;

private:
  QStringList m_ColHeaders = {};
  QStringList m_RowHeaders = {};
  bool m_DynamicRows = {};
  bool m_DynamicCols = {};
  int m_MinRows = {};
  int m_MinCols = {};
  int m_DefaultRowCount = {};
  int m_DefaultColCount = {};

  std::vector<std::vector<double>> m_TableData;

  /**
   * @brief Writes the contents of the data to a QJsonObject
   * @param data
   * @return QJsonArray
   */
  void writeData(QJsonObject& object) const;

  /**
   * @brief Reads the contents of the QJsonObject into a 2D array
   * @param object
   * @return 2D array
   */
  std::vector<std::vector<double>> readData(QJsonObject object);

  /**
   * @brief Checks that the dimensions between all variables are the same.  If not, adjusts dimensions
   * to match numRows and numCols.
   */
  void checkAndAdjustDimensions();
};

Q_DECLARE_METATYPE(DynamicTableData)
