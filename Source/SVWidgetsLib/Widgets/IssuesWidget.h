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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/Messages/AbstractMessage.h"
#include "SIMPLib/Messages/AbstractMessageHandler.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class FilterListToolboxWidget;
class QLabel;
class QTableWidgetItem;
class QtSSettings;
class QTableWidget;
namespace Ui
{
class IssuesWidget;
}

class IssuesWidgetMessageHandler;

/**
 * @brief The IssuesWidget class
 */
class SVWidgetsLib_EXPORT IssuesWidget : public QWidget, public IObserver
{
  Q_OBJECT
public:
  static SIMPLView::DockWidgetSettings::HideDockSetting GetHideDockSetting();
  static void SetHideDockSetting(SIMPLView::DockWidgetSettings::HideDockSetting setting);

  IssuesWidget(QWidget* parent = nullptr);
  ~IssuesWidget() override;

  friend IssuesWidgetMessageHandler;

  static const int FilterIndex = 0;
  static const int FilterName = 1;
  static const int Description = 2;
  static const int ErrorCode = 3;

  /**
   * @brief getIssuesTable
   * @return
   */
  QTableWidget* getIssuesTable();

public Q_SLOTS:
  void processPipelineMessage(const AbstractMessage::Pointer& msg) override;
  void clearIssues();
  void on_errorTableWidget_itemClicked(QTableWidgetItem* item);
  void displayCachedMessages();

  void showFilterHelp(const QString& url);

Q_SIGNALS:
  void tableHasErrors(bool b, int errCount, int warnCount);
  void showTable(bool b);

protected:
  /**
   * @brief setupGui
   */
  void setupGui();

private:
  QSharedPointer<Ui::IssuesWidget> ui = nullptr;
  std::vector<AbstractMessage::Pointer> m_CachedMessages;

public:
  IssuesWidget(const IssuesWidget&) = delete;            // Copy Constructor Not Implemented
  IssuesWidget(IssuesWidget&&) = delete;                 // Move Constructor Not Implemented
  IssuesWidget& operator=(const IssuesWidget&) = delete; // Copy Assignment Not Implemented
  IssuesWidget& operator=(IssuesWidget&&) = delete;      // Move Assignment Not Implemented
};
