/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QAbstractTableModel>
#include <QtCore/QVariant>
#include <QtCore/QVector>

#include "SIMPLib/Montages/DataContainerGrid.h"
#include "SVWidgetsLib/SVWidgetsLib.h"

class QAbstractItemDelegate;
class DataContainerGrid;

/*
 *
 */
class SVWidgetsLib_EXPORT DataContainerGridModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  DataContainerGridModel(const DataContainerGrid& dcGrid, QObject* parent = 0);

  ~DataContainerGridModel() override;

  /**
   *
   * @param index
   * @return
   */
  Qt::ItemFlags flags(const QModelIndex& index) const override;

  /**
   *
   * @param index
   * @param role
   * @return
   */
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  /**
   *
   * @param parent
   * @return
   */
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  /**
   *
   * @param parent
   * @return
   */
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  /**
   * @brief depthCount
   * @return
   */
  int depthCount() const;

  /**
   * @brief setCurrentDepth
   * @return
   */
  int getCurrentDepth() const;

  /**
   * @brief setCurrentDepth
   * @param depth
   */
  void setCurrentDepth(int depth);

  /**
   *
   * @param index
   * @param value
   * @param role
   * @return
   */
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

  /**
   *
   * @param col
   * @param orientation
   * @param data
   * @param role
   * @return
   */
  bool setHeaderData(int col, Qt::Orientation orientation, const QVariant& data, int role = Qt::EditRole) override;

  /**
   *
   * @param row
   * @param count
   * @param parent
   * @return
   */
  bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

  /**
   *
   * @param row
   * @param count
   * @param parent
   * @return
   */
  bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

  /**
   *
   * @param col
   * @param count
   * @param parent
   * @return
   */
  bool insertColumns(int col, int count, const QModelIndex& parent = QModelIndex()) override;

  /**
   *
   * @param col
   * @param count
   * @param parent
   * @return
   */
  bool removeColumns(int col, int count, const QModelIndex& parent = QModelIndex()) override;

  /**
   * @brief supportedDropActions
   * @return
   */
  Qt::DropActions supportedDropActions() const override;

  /**
   * @brief canDropMimeData
   * @param data
   * @param action
   * @param row
   * @param column
   * @param parent
   */
  bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;

  /**
   * @brief dropMineData
   * @param data
   * @param action
   * @param row
   * @param column
   * @param parent
   */
  bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

  /**
   *
   * @return
   */
  // virtual QAbstractItemDelegate* getItemDelegate();

  /**
   * @brief setTableData
   * @param grid
   */
  void setGridData(DataContainerGrid& grid);

  /**
   * @brief getTableData
   * @param grid
   */
  void getGridData(DataContainerGrid& grid);

Q_SIGNALS:
  void modelChanged();

private:
  DataContainerGrid m_DCGrid;
  int m_CurrentDepth = 0;

public:
  DataContainerGridModel(const DataContainerGridModel&) = delete;            // Copy Constructor Not Implemented
  DataContainerGridModel(DataContainerGridModel&&) = delete;                 // Move Constructor Not Implemented
  DataContainerGridModel& operator=(const DataContainerGridModel&) = delete; // Copy Assignment Not Implemented
  DataContainerGridModel& operator=(DataContainerGridModel&&) = delete;      // Move Assignment Not Implemented
};
