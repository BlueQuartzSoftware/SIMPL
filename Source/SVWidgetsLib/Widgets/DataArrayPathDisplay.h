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
 *    United States Air Force Prime Contract FA8650-15-D-5280
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QSharedPointer>
#include <QtWidgets/QWidget>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

namespace Ui
{
class DataArrayPathDisplay;
}

/**
 * @class DataArrayPathDisplay DataArrayPathDisplay.h
 * SVWidgetsLib/Widgets/DataArrayPathDisplay.h
 * @brief This class handles DataArrayPath selection and mouse over signals for filter parameter widgets.
 */
class DataArrayPathDisplay : public QWidget
{
  Q_OBJECT

public:
  static const QColor GetActiveColor(DataArrayPathHelper::DataType type);
  static const QColor GetCheckedColor(DataArrayPathHelper::DataType type);

  DataArrayPathDisplay(QWidget* parent = nullptr);
  ~DataArrayPathDisplay() override = default;

  /**
   * @brief Sets the DataArrayPath displayed
   * @param dcName
   * @param amName
   * @param daName
   */
  void setDataArrayPath(const QString& dcName, const QString& amName = "", const QString& daName = "");

  /**
   * @brief Sets the DataArrayPath displayed
   * @param dataArrayPath
   */
  void setDataArrayPath(const DataArrayPath& dataArrayPath);

  /**
   * @brief Returns the DataArrayPath displayed.
   * @return
   */
  DataArrayPath getDataArrayPath() const;

  /**
   * @brief Returns the DataType for the displayed DataArrayPath
   * @return
   */
  DataArrayPathHelper::DataType getDataType() const;

  /**
   * @brief Sets the filter the selection widget operates on
   * @param filter
   */
  void setFilter(AbstractFilter* filter);

  /**
   * @brief Handles operations performed before Preflight()
   */
  void beforePreflight();

  /**
   * @brief Checks path validity
   */
  void afterPreflight();

  /**
   * @brief Override the minimumSizeHint for extending the widget enough to paint the active color
   */
  QSize minimumSizeHint() const override;

  /**
   * @brief Override the minimumSizeHint for extending the widget enough to paint the active color
   */
  QSize sizeHint() const override;

  /**
   * @brief Returns the property name set for this widget
   * @return
   */
  QString getPropertyName();

  /**
   * @brief Sets the property name
   * @param propName
   */
  void setPropertyName(QString propName);

protected:
  /**
   * @brief Performs initial setup for the GUI
   */
  void setupGui();

  void updateStyle();

  /**
   * @brief Returns the X margin
   * @return
   */
  int getXMargin() const;

  /**
   * @brief Returns the Y margin
   * @return
   */
  int getYMargin() const;

  /**
   * @brief Returns the contents rect after styling.
   * @return
   */
  QRect getStyledContentsRect() const;

  /**
   * @brief Returns the contents rect after styling.
   * @return
   */
  QRect getStyledBorderRect() const;

  /**
   * @brief Returns the updated size hint based on the superclass's size hint
   */
  QSize updatedSizeHint(QSize sizeHint) const;

private:
  AbstractFilter* m_Filter = nullptr;
  QString m_PropName;
  QSharedPointer<Ui::DataArrayPathDisplay> m_Ui;
  DataArrayPathHelper::DataType m_DataType = DataArrayPathHelper::DataType::DataArray;
};
