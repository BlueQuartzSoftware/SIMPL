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

#include "SIMPLib/FilterParameters/AbstractIOFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SVWidgetsLib/QtSupport/QtSFaderWidget.h"
#include "SVWidgetsLib/SVWidgetsLib.h"

#include "SVWidgetsLib/ui_AbstractIOFileWidget.h"

class QLineEdit;
class OutputFileFilterParameter;
class QKeyEvent;

/**
 * @brief
 * @author
 * @version
 */
class SVWidgetsLib_EXPORT AbstractIOFileWidget : public FilterParameterWidget, protected Ui::AbstractIOFileWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  AbstractIOFileWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

  ~AbstractIOFileWidget() override;

  Q_PROPERTY(QPixmap Icon READ getIcon WRITE setIcon)
  void setIcon(const QPixmap& path);
  QPixmap getIcon();

  /**
   * @brief This method does additional GUI widget connections
   */
  void setupGui() override;

public Q_SLOTS:
  void beforePreflight();
  void afterPreflight();
  void filterNeedsInputParameters(AbstractFilter* filter);

  void on_m_LineEdit_fileDropped(const QString& text);
  void on_m_LineEdit_editingFinished();
  void on_m_LineEdit_textChanged(const QString& text);
  void on_m_LineEdit_returnPressed();

protected:
  void setValue(const QString& val);

  QString getValue();

  /**
   * @brief
   * @param event
   */
  void keyPressEvent(QKeyEvent* event) override;

  /**
   * @brief setupMenuField
   */
  void setupMenuField();

private:
  QAction* m_ShowFileAction = nullptr;
  QString m_CurrentText = "";
  QPixmap m_Icon = QPixmap(QLatin1String(":/SIMPL/icons/images/caret-bottom.png"));
  AbstractIOFilterParameter* m_FilterParameter = nullptr;

public:
  AbstractIOFileWidget(const AbstractIOFileWidget&) = delete;            // Copy Constructor Not Implemented
  AbstractIOFileWidget(AbstractIOFileWidget&&) = delete;                 // Move Constructor Not Implemented
  AbstractIOFileWidget& operator=(const AbstractIOFileWidget&) = delete; // Copy Assignment Not Implemented
  AbstractIOFileWidget& operator=(AbstractIOFileWidget&&) = delete;      // Move Assignment Not Implemented
};
