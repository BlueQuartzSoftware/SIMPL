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
#pragma once

#include <QtWidgets/QWidget>

#include "ui_TupleTableWidget.h"

class DynamicTableFilterParameter;

/**
 * @brief
 * @author
 * @version
 */
class TupleTableWidget : public QWidget, private Ui::TupleTableWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  TupleTableWidget(QWidget* parent = nullptr);

  ~TupleTableWidget() override;

  /**
   * @brief This method does additional GUI widget connections
   */
  void setupGui();

  std::vector<size_t> getData();

  void addTupleDimensions(std::vector<size_t> tupleDims);
  void clearTupleDimensions();

  bool didUseEdit();

public Q_SLOTS:
  void on_addTupleBtn_clicked();
  void on_deleteTupleBtn_clicked();

protected:
  void addColumn(int value);

protected Q_SLOTS:
  void on_tupleTable_itemChanged(QTableWidgetItem* item);

Q_SIGNALS:
  void tupleDimsChanged(std::vector<size_t> tupleDims);

private:
  void updateDynamicButtons();

  bool m_UserEdited = false;

public:
  TupleTableWidget(const TupleTableWidget&) = delete;            // Copy Constructor Not Implemented
  TupleTableWidget(TupleTableWidget&&) = delete;                 // Move Constructor Not Implemented
  TupleTableWidget& operator=(const TupleTableWidget&) = delete; // Copy Assignment Not Implemented
  TupleTableWidget& operator=(TupleTableWidget&&) = delete;      // Move Assignment Not Implemented
};
