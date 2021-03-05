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

#include "IssuesWidget.h"

#include <iostream>

#include <QtWidgets/QTableWidgetItem>

#include "SIMPLib/Common/DocRequestManager.h"

#include "SVWidgetsLib/QtSupport/QtSSettings.h"
#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/IssuesWidgetMessageHandler.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "ui_IssuesWidget.h"

namespace
{
const int FilterIndexDefaultColumnWidth = 50;
const int FilterNameDefaultColumnWidth = 150;
const int ErrorCodeDefaultColumnWidth = 50;
} // namespace

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLView::DockWidgetSettings::HideDockSetting IssuesWidget::GetHideDockSetting()
{
  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings());
  prefs->beginGroup(SIMPLView::DockWidgetSettings::GroupName);
  prefs->beginGroup(SIMPLView::DockWidgetSettings::IssuesDockGroupName);

  int showError = static_cast<int>(SIMPLView::DockWidgetSettings::HideDockSetting::Ignore);
  int hideDockSetting = prefs->value(SIMPLView::DockWidgetSettings::KeyName, QVariant(showError)).toInt();
  SIMPLView::DockWidgetSettings::HideDockSetting value = static_cast<SIMPLView::DockWidgetSettings::HideDockSetting>(hideDockSetting);

  prefs->endGroup();
  prefs->endGroup();

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidget::SetHideDockSetting(SIMPLView::DockWidgetSettings::HideDockSetting setting)
{
  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings());
  prefs->beginGroup(SIMPLView::DockWidgetSettings::GroupName);
  prefs->beginGroup(SIMPLView::DockWidgetSettings::IssuesDockGroupName);

  prefs->setValue(SIMPLView::DockWidgetSettings::KeyName, static_cast<int>(setting));

  prefs->endGroup();
  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IssuesWidget::IssuesWidget(QWidget* parent)
: QWidget(parent)
, ui(new Ui::IssuesWidget)
{
  ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IssuesWidget::~IssuesWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidget::setupGui()
{
  ui->errorTableWidget->horizontalHeader()->setSectionResizeMode(FilterIndex, QHeaderView::Fixed);
  ui->errorTableWidget->horizontalHeader()->setSectionResizeMode(FilterName, QHeaderView::Fixed);
  ui->errorTableWidget->horizontalHeader()->setSectionResizeMode(ErrorCode, QHeaderView::Fixed);
  ui->errorTableWidget->horizontalHeader()->setSectionResizeMode(Description, QHeaderView::Stretch);
  ui->errorTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

  ui->errorTableWidget->horizontalHeader()->resizeSection(FilterIndex, FilterIndexDefaultColumnWidth);
  ui->errorTableWidget->horizontalHeader()->resizeSection(FilterName, FilterNameDefaultColumnWidth);
  ui->errorTableWidget->horizontalHeader()->resizeSection(ErrorCode, ErrorCodeDefaultColumnWidth);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidget::on_errorTableWidget_itemClicked(QTableWidgetItem* item)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidget::clearIssues()
{
  ui->errorTableWidget->clearContents();
  ui->errorTableWidget->setRowCount(0);
  m_CachedMessages.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidget::processPipelineMessage(const AbstractMessage::Pointer& msg)
{
  m_CachedMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidget::displayCachedMessages()
{
  int count = 0;
  int warnCount = 0;
  int errCount = 0;

  // Add in the content for the cells of the table.
  for(int j = 0; j < m_CachedMessages.size(); j++)
  {
    IssuesWidgetMessageHandler messageHandler(this, &count, &errCount, &warnCount);
    m_CachedMessages[j]->visit(&messageHandler);
  }

  Q_EMIT tableHasErrors(errCount > 0, errCount, warnCount);

  if(ui->errorTableWidget->rowCount() > 0)
  {
    ui->errorTableWidget->horizontalHeader()->setSectionResizeMode(FilterIndex, QHeaderView::ResizeToContents);
    ui->errorTableWidget->horizontalHeader()->setSectionResizeMode(FilterName, QHeaderView::ResizeToContents);
    ui->errorTableWidget->horizontalHeader()->setSectionResizeMode(ErrorCode, QHeaderView::ResizeToContents);
  }
  else
  {
    ui->errorTableWidget->horizontalHeader()->setSectionResizeMode(FilterIndex, QHeaderView::Fixed);
    ui->errorTableWidget->horizontalHeader()->setSectionResizeMode(FilterName, QHeaderView::Fixed);
    ui->errorTableWidget->horizontalHeader()->setSectionResizeMode(ErrorCode, QHeaderView::Fixed);
    ui->errorTableWidget->horizontalHeader()->resizeSection(FilterIndex, FilterIndexDefaultColumnWidth);
    ui->errorTableWidget->horizontalHeader()->resizeSection(FilterName, FilterNameDefaultColumnWidth);
    ui->errorTableWidget->horizontalHeader()->resizeSection(ErrorCode, ErrorCodeDefaultColumnWidth);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidget::showFilterHelp(const QString& urlString)
{
  QUrl helpURL(urlString);

  DocRequestManager* docRequester = DocRequestManager::Instance();
  docRequester->requestFilterDocUrl(helpURL);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QTableWidget* IssuesWidget::getIssuesTable()
{
  return ui->errorTableWidget;
}
