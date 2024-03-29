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

#include "SVWidgetsLib/QtSupport/QtSFaderWidget.h"

#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SVWidgetsLib/SVWidgetsLib.h"

#include "SVWidgetsLib/ui_DynamicTableWidget.h"

class DynamicTableFilterParameter;
class DynamicTableItemDelegate;

/**
 * @brief
 * @author
 * @version
 */
class SVWidgetsLib_EXPORT DynamicTableWidget : public FilterParameterWidget, private Ui::DynamicTableWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  DynamicTableWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

  ~DynamicTableWidget() override;

  /**
   * @brief This method does additional GUI widget connections
   */
  void setupGui() override;

  /**
   * @brief initializeWidget
   * @param parameter
   * @param filter
   */
  void initializeWidget(FilterParameter* parameter, AbstractFilter* filter);

  void setFilterParameter(FilterParameter* value) override;
  FilterParameter* getFilterParameter() const override;

  std::vector<std::vector<double>> getData();

  void updateButtonStyles();

public Q_SLOTS:
  void on_dynamicTable_cellChanged(int row, int col);
  void on_addRowBtn_clicked();
  void on_deleteRowBtn_clicked();
  void on_addColBtn_clicked();
  void on_deleteColBtn_clicked();
  void filterNeedsInputParameters(AbstractFilter* filter); // When the filter is ready for us to update its input parameter(s) that we are responsible for
  void beforePreflight();                                  // Called just before the "dataCheck()" is called
  void afterPreflight();                                   // Called just after the dataCheck() is called.

private:
  DynamicTableFilterParameter* m_FilterParameter;
  DynamicTableItemDelegate* m_ItemDelegate = nullptr;
  bool m_DidCausePreflight;

  void populateTable();
  void populateHeaders();
  void renumberDynamicHeaders();
  void updateDynamicButtons();

public:
  DynamicTableWidget(const DynamicTableWidget&) = delete;            // Copy Constructor Not Implemented
  DynamicTableWidget(DynamicTableWidget&&) = delete;                 // Move Constructor Not Implemented
  DynamicTableWidget& operator=(const DynamicTableWidget&) = delete; // Copy Assignment Not Implemented
  DynamicTableWidget& operator=(DynamicTableWidget&&) = delete;      // Move Assignment Not Implemented
};
