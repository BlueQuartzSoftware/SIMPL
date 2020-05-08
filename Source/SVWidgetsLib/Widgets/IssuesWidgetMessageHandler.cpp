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

#include "IssuesWidgetMessageHandler.h"

#include <QtCore/QUrl>
#include <QtCore/QTextStream>

#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QLabel>

#include "SIMPLib/Messages/FilterErrorMessage.h"
#include "SIMPLib/Messages/FilterWarningMessage.h"

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
IssuesWidgetMessageHandler::IssuesWidgetMessageHandler(IssuesWidget* issuesWidget, int* count, int* errCount, int* warningCount)
: m_IssuesWidget(issuesWidget)
, m_Count(count)
, m_ErrCount(errCount)
, m_WarningCount(warningCount)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidgetMessageHandler::processMessage(const FilterErrorMessage* msg) const
{
  QColor msgColor = QColor(255, 191, 193);
  addItemToIssuesTable(msg->getClassName(), msg->getHumanLabel(), msg->getPipelineIndex(), msg->getMessageText(), msg->getCode(), msgColor);

  m_ErrCount[0]++;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidgetMessageHandler::processMessage(const FilterWarningMessage* msg) const
{
  QColor msgColor = QColor(251, 254, 137);
  addItemToIssuesTable(msg->getClassName(), msg->getHumanLabel(), msg->getPipelineIndex(), msg->getMessageText(), msg->getCode(), msgColor);

  m_WarningCount[0]++;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesWidgetMessageHandler::addItemToIssuesTable(const QString& className, const QString& humanLabel, int pipelineIndex, const QString& msg, int code, QColor msgColor) const
{
  QBrush msgBrush = QBrush(msgColor);

  // Create error hyperlink
  QTableWidgetItem* codeWidgetItem = nullptr;
  QTableWidgetItem* indexWidgetItem = nullptr;

  QTableWidgetItem* filterNameWidgetItem = new QTableWidgetItem(humanLabel);
  filterNameWidgetItem->setTextAlignment(Qt::AlignCenter);
  QTableWidgetItem* descriptionWidgetItem = new QTableWidgetItem(msg);
  codeWidgetItem = new QTableWidgetItem(QString::number(code));
  indexWidgetItem = new QTableWidgetItem(QString::number(pipelineIndex + 1));

  codeWidgetItem->setTextAlignment(Qt::AlignCenter);

  filterNameWidgetItem->setBackground(msgBrush);
  descriptionWidgetItem->setBackground(msgBrush);
  codeWidgetItem->setBackground(msgBrush);

  QTableWidget* issuesTable = m_IssuesWidget->getIssuesTable();
  int row = issuesTable->rowCount();

  issuesTable->insertRow(row);

  QLabel* hyperlinkLabel = createHyperlinkLabel(className, humanLabel);
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

  m_Count[0]++;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLabel* IssuesWidgetMessageHandler::createHyperlinkLabel(const QString& filterClassName, const QString& filterHumanLabel) const
{
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
