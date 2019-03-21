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

#include "IssuesWidgetMessageHandler.h"

#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QLabel>

#ifdef SIMPL_USE_MKDOCS
#define URL_GENERATOR QtSDocServer
#include "SVWidgetsLib/QtSupport/QtSDocServer.h"
#else
#define URL_GENERATOR QtSHelpUrlGenerator
#include "SVWidgetsLib/QtSupport/QtSHelpUrlGenerator.h"
#endif

#include "SVWidgetsLib/Widgets/IssuesWidget.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IssuesWidgetMessageHandler::IssuesWidgetMessageHandler(IssuesWidget* issuesWidget)
: m_IssuesWidget(issuesWidget)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidgetMessageHandler::processMessage(GenericMessage msg)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidgetMessageHandler::processMessage(PipelineMessage msg)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidgetMessageHandler::processMessage(FilterMessage msg)
{
  bool updateRow = false;
  QColor msgColor;
  QBrush msgBrush;
  QString msgDesc;
  QString msgPrefix;
  int msgCode = -1;

  // Create error hyperlink
  switch(msg.getType())
  {
  case FilterMessage::MessageType::Error:
    msgColor = QColor(255, 191, 193);
    updateRow = true;
    break;
  case FilterMessage::MessageType::Warning:
    msgColor = QColor(251, 254, 137);
    updateRow = true;
    break;
  case FilterMessage::MessageType::StatusMessage:
  case FilterMessage::MessageType::StandardOutputMessage:
  case FilterMessage::MessageType::ProgressValue:
  case FilterMessage::MessageType::StatusMessageAndProgressValue:
  case FilterMessage::MessageType::UnknownMessageType:
    break;
  }

  if(updateRow)
  {
    msgBrush = QBrush(msgColor);

    msgDesc = msg.getText();
    msgCode = msg.getCode();
    msgPrefix = msg.getPrefix();

    QTableWidgetItem* codeWidgetItem = nullptr;
    QTableWidgetItem* indexWidgetItem = nullptr;

    QTableWidgetItem* filterNameWidgetItem = new QTableWidgetItem(msgPrefix);
    filterNameWidgetItem->setTextAlignment(Qt::AlignCenter);
    QTableWidgetItem* descriptionWidgetItem = new QTableWidgetItem(msgDesc);
    codeWidgetItem = new QTableWidgetItem(QString::number(msgCode));
    indexWidgetItem = new QTableWidgetItem(QString::number(msg.getPipelineIndex() + 1));

    codeWidgetItem->setTextAlignment(Qt::AlignCenter);

    filterNameWidgetItem->setBackground(msgBrush);
    descriptionWidgetItem->setBackground(msgBrush);
    codeWidgetItem->setBackground(msgBrush);

    QTableWidget* issuesTable = m_IssuesWidget->getIssuesTable();
    int row = issuesTable->rowCount();

    QLabel* hyperlinkLabel = createHyperlinkLabel(msg);
    if(hyperlinkLabel == nullptr)
    {
      issuesTable->setItem(row, IssuesWidget::FilterName, filterNameWidgetItem);
    }
    else
    {
      issuesTable->setCellWidget(row, IssuesWidget::FilterName, hyperlinkLabel);
    }
    issuesTable->setItem(row, IssuesWidget::FilterIndex, indexWidgetItem);
    issuesTable->setItem(row, IssuesWidget::Description, descriptionWidgetItem);
    issuesTable->setItem(row, IssuesWidget::ErrorCode, codeWidgetItem);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLabel* IssuesWidgetMessageHandler::createHyperlinkLabel(const FilterMessage& msg)
{
  QString filterClassName = (msg.getFilterClassName());
  QString filterHumanLabel = (msg.getFilterHumanLabel());

  if(filterClassName.isEmpty() || filterHumanLabel.isEmpty())
  {
    if(!filterClassName.isEmpty())
    {
      return new QLabel(filterClassName);
    }
    if(!filterHumanLabel.isEmpty())
    {
      return new QLabel(filterHumanLabel);
    }

    return new QLabel("Unknown Filter Class");
  }

  QUrl filterURL = URL_GENERATOR::GenerateHTMLUrl(filterClassName);
  QString filterHTMLText;
  QTextStream ts(&filterHTMLText);
  ts << "<a style=\"color: " << SVStyle::Instance()->getText_Error_color().name(QColor::HexRgb) << ";\" href=\"";
  ts << filterURL.toString() << "\">" << filterHumanLabel << "</a>";

  QLabel* hyperlinkLabel = new QLabel(filterHTMLText);
  hyperlinkLabel->setTextFormat(Qt::RichText);
  hyperlinkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
  hyperlinkLabel->setOpenExternalLinks(false);
  QObject::connect(hyperlinkLabel, SIGNAL(linkActivated(const QString&)), m_IssuesWidget, SLOT(showFilterHelp(const QString&)));

  return hyperlinkLabel;
}


