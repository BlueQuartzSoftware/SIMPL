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

#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableWidgetItem>

#include "SIMPLib/Common/DocRequestManager.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/QtSupport/QtSSettings.h"

#ifdef SIMPL_USE_MKDOCS
#define URL_GENERATOR QtSDocServer
#include "SVWidgetsLib/QtSupport/QtSDocServer.h"
#else
#define URL_GENERATOR QtSHelpUrlGenerator
#include "SVWidgetsLib/QtSupport/QtSHelpUrlGenerator.h"
#endif

#include "ui_IssuesWidget.h"

namespace
{
  const int FilterIndexDefaultColumnWidth = 50;
  const int FilterNameDefaultColumnWidth = 150;
  const int ErrorCodeDefaultColumnWidth = 50;
}

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
void IssuesWidget::processPipelineMessage(const PipelineMessage& msg)
{
  m_CachedMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidget::displayCachedMessages()
{
  // Figure out how many error and warning messages that we have. We ignore the rest
  int count = 0;
  int warnCount = 0;
  int errCount = 0;
  for(int i = 0; i < m_CachedMessages.size(); i++)
  {
    PipelineMessage msg = m_CachedMessages[i];
    switch(msg.getType())
    {
    case PipelineMessage::MessageType::Error:
      count++;
      errCount++;
      break;
    case PipelineMessage::MessageType::Warning:
      count++;
      warnCount++;
      break;
    case PipelineMessage::MessageType::StatusMessage:
    case PipelineMessage::MessageType::StandardOutputMessage:
    case PipelineMessage::MessageType::ProgressValue:
    case PipelineMessage::MessageType::StatusMessageAndProgressValue:
    case PipelineMessage::MessageType::UnknownMessageType:
      break;
    }
  }

  emit tableHasErrors(errCount > 0, errCount, warnCount);

  // Now create the correct number of table rows.
  ui->errorTableWidget->setRowCount(count);
  int row = 0;
  bool updateRow = false;
  QBrush msgBrush;
  QColor msgColor;
  QString msgDesc;
  QString msgPrefix;
  int msgCode = -1;
  QTableWidgetItem* filterNameWidgetItem = nullptr;
  QTableWidgetItem* descriptionWidgetItem = nullptr;
  QTableWidgetItem* codeWidgetItem = nullptr;
  QTableWidgetItem* indexWidgetItem = nullptr;
  QLabel* hyperlinkLabel = nullptr;

  // Add in the content for the cells of the table.
  for(int j = 0; j < m_CachedMessages.size(); j++)
  {
    PipelineMessage msg = m_CachedMessages[j];
    // Create error hyperlink
    updateRow = false;
    switch(msg.getType())
    {
    case PipelineMessage::MessageType::Error:
      msgColor = QColor(255, 191, 193);
      updateRow = true;
      break;
    case PipelineMessage::MessageType::Warning:
      msgColor = QColor(251, 254, 137);
      updateRow = true;
      break;
    case PipelineMessage::MessageType::StatusMessage:
    case PipelineMessage::MessageType::StandardOutputMessage:
    case PipelineMessage::MessageType::ProgressValue:
    case PipelineMessage::MessageType::StatusMessageAndProgressValue:
    case PipelineMessage::MessageType::UnknownMessageType:
      break;
    }

    if(updateRow)
    {
      msgBrush = QBrush(msgColor);

      msgDesc = msg.getText();
      msgCode = msg.getCode();
      msgPrefix = msg.getPrefix();

      filterNameWidgetItem = new QTableWidgetItem(msgPrefix);
      filterNameWidgetItem->setTextAlignment(Qt::AlignCenter);
      descriptionWidgetItem = new QTableWidgetItem(msgDesc);
      codeWidgetItem = new QTableWidgetItem(QString::number(msgCode));
      indexWidgetItem = new QTableWidgetItem(QString::number(msg.getPipelineIndex() + 1));

      codeWidgetItem->setTextAlignment(Qt::AlignCenter);

      filterNameWidgetItem->setBackground(msgBrush);
      descriptionWidgetItem->setBackground(msgBrush);
      codeWidgetItem->setBackground(msgBrush);

      hyperlinkLabel = createHyperlinkLabel(msg);
      if(hyperlinkLabel == nullptr)
      {
        ui->errorTableWidget->setItem(row, FilterName, filterNameWidgetItem);
      }
      else
      {
        ui->errorTableWidget->setCellWidget(row, FilterName, hyperlinkLabel);
      }
      ui->errorTableWidget->setItem(row, FilterIndex, indexWidgetItem);
      ui->errorTableWidget->setItem(row, Description, descriptionWidgetItem);
      ui->errorTableWidget->setItem(row, ErrorCode, codeWidgetItem);

      row++;
    }
  }

  if (ui->errorTableWidget->rowCount() > 0)
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
QLabel* IssuesWidget::createHyperlinkLabel(PipelineMessage msg)
{
  QString filterClassName = (msg.getFilterClassName());
  QString filterHumanLabel = (msg.getFilterHumanLabel());
  QString msgPrefix = (msg.getPrefix());

  if(filterClassName.isEmpty() || filterHumanLabel.isEmpty())
  {
    if(filterClassName.isEmpty() == false)
    {
      return new QLabel(filterClassName);
    }
    else if(filterHumanLabel.isEmpty() == false)
    {
      return new QLabel(filterHumanLabel);
    }

    return new QLabel("Unknown Filter Class");
  }


  QUrl filterURL = URL_GENERATOR::GenerateHTMLUrl(filterClassName);
  QString filterHTMLText("<a href=\"");
  filterHTMLText.append(filterURL.toString()).append("\">").append(filterHumanLabel).append("</a>");

  QLabel* hyperlinkLabel = new QLabel(filterHTMLText);
  hyperlinkLabel->setTextFormat(Qt::RichText);
  hyperlinkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
  hyperlinkLabel->setOpenExternalLinks(false);
  connect(hyperlinkLabel, SIGNAL(linkActivated(const QString&)), this, SLOT(showFilterHelp(const QString&)));

  return hyperlinkLabel;
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
