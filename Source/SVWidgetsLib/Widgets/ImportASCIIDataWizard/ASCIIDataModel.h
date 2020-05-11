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

#include <QtCore/QAbstractTableModel>

class ASCIIDataItem;

class ASCIIDataModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  /**
   * @brief Returns the name of the class for ASCIIDataModel
   */
  QString getNameOfClass() const;
  /**
   * @brief Returns the name of the class for ASCIIDataModel
   */
  static QString ClassName();

  ASCIIDataModel(QObject* parent = nullptr);

  ~ASCIIDataModel() override;

  void clear();
  void clearContents();
  void clearHeaders(Qt::Orientation orientation);

  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::DisplayRole) override;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;
  bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;

  bool insertColumns(int position, int columns, const QModelIndex& parent = QModelIndex()) override;
  bool removeColumns(int position, int columns, const QModelIndex& parent = QModelIndex()) override;

  Qt::ItemFlags flags(const QModelIndex& index) const override;

  bool setData(const QModelIndex& index, const QVariant& value, int role) override;

  QString columnDataType(int column) const;
  void setColumnDataType(int column, const QString& type);

  bool columnHasErrors(int column) const;
  void setColumnHasErrors(int column, bool value);

  QStringList originalStrings();

  QString originalString(int row) const;
  void setOriginalString(int row, const QString& value);

protected:
private:
  static ASCIIDataModel* self;

  QVector<ASCIIDataItem*> m_TableItems;

  QVector<QString> m_HorizontalHeaders;
  QVector<QString> m_VerticalHeaders;

  QVector<QString> m_ColumnDataType;
  QVector<bool> m_ColumnHasErrors;

  ASCIIDataItem* getItem(const QModelIndex& index) const;

public:
  ASCIIDataModel(const ASCIIDataModel&) = delete;            // Copy Constructor Not Implemented
  ASCIIDataModel(ASCIIDataModel&&) = delete;                 // Move Constructor Not Implemented
  ASCIIDataModel& operator=(const ASCIIDataModel&) = delete; // Copy Assignment Not Implemented
  ASCIIDataModel& operator=(ASCIIDataModel&&) = delete;      // Move Assignment Not Implemented
};
