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

#include "StatusBarWidget.h"

#include <QtCore/QDebug>

#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>

#include "SVWidgetsLib/Widgets/SVStyle.h"
#include "SVWidgetsLib/QtSupport/QtSSettings.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatusBarWidget::StatusBarWidget(QWidget* parent)
  : QFrame(parent)
{
  this->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatusBarWidget::~StatusBarWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::setupGui()
{
//  QString style = generateStyleSheet(false);
//  consoleBtn->setStyleSheet(style);
//  issuesBtn->setStyleSheet(style);
//  dataBrowserBtn->setStyleSheet(style);

  m_ButtonListMenu = new QMenu(buttonListBtn);

  connect(buttonListBtn, &QPushButton::clicked, [=] {
    m_ButtonListMenu->exec(QCursor::pos());
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::readSettings()
{
  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings());

  prefs->beginGroup("Status Bar Widget");

  prefs->beginGroup("Button Visibility");
  m_FilterListBtnToggleAction->setChecked(prefs->value("Filter List", QVariant(false)).toBool());
  m_FilterLibraryBtnToggleAction->setChecked(prefs->value("Filter Library", QVariant(false)).toBool());
  m_BookmarksBtnToggleAction->setChecked(prefs->value("Bookmarks", QVariant(false)).toBool());
  m_PipelineBtnToggleAction->setChecked(prefs->value("Pipeline", QVariant(true)).toBool());
  m_IssuesBtnToggleAction->setChecked(prefs->value("Issues", QVariant(true)).toBool());
  m_DataBrowserBtnToggleAction->setChecked(prefs->value("Data Browser", QVariant(true)).toBool());
  m_ConsoleBtnToggleAction->setChecked(prefs->value("Console", QVariant(true)).toBool());
  prefs->endGroup();

  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::writeSettings()
{
  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings());

  prefs->beginGroup("Status Bar Widget");

  prefs->beginGroup("Button Visibility");
  prefs->setValue("Filter List", m_FilterListBtnToggleAction->isChecked());
  prefs->setValue("Filter Library", m_FilterLibraryBtnToggleAction->isChecked());
  prefs->setValue("Bookmarks", m_BookmarksBtnToggleAction->isChecked());
  prefs->setValue("Pipeline", m_PipelineBtnToggleAction->isChecked());
  prefs->setValue("Issues", m_IssuesBtnToggleAction->isChecked());
  prefs->setValue("Data Browser", m_DataBrowserBtnToggleAction->isChecked());
  prefs->setValue("Console", m_ConsoleBtnToggleAction->isChecked());
  prefs->endGroup();

  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::updateStyle()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatusBarWidget::generateStyleSheet(bool error)
{
  QFont font = SVStyle::Instance()->GetBrandingLabelFont();
  QString fontString;
  QTextStream fontStringStream(&fontString);

  fontStringStream << "font: " << font.weight() << " ";
#if defined(Q_OS_MAC)
  fontStringStream << font.pointSize();
#elif defined(Q_OS_WIN)
  fontStringStream << font.pointSize();
#else
  fontStringStream << font.pointSize();
#endif
  fontStringStream << "pt \"" << font.family() << "\";";

  QString style;
  QTextStream ss(&style);

  QColor offBgColor(200, 200, 200);
  QColor onBgColor(90, 90, 90);
  QColor offTextColor(50, 50, 50);
  QColor onTextColor(240, 240, 240);
  QColor offBorderColor(120, 120, 120);
  QColor onBorderColor(200, 200, 200);

  if(error)
  {
    offBgColor = QColor(255, 150, 150);
    onBgColor = QColor(220, 60, 60);
    offBorderColor = QColor(220, 0, 0);
    onBorderColor = QColor(200, 0, 0);
  }

  int borderRadius = 3;

  ss << "QPushButton  {";
  ss << fontString;
  ss << "background-color: " << offBgColor.name() << ";";
  ss << "color:" << offTextColor.name() << ";";
  ss << "border: 1px solid " << offBorderColor.name() << ";";
  ss << "border-radius: " << borderRadius << "px;";
  ss << "padding: 1 8 1 8px;";
  ss << "margin: 2 2 2 2px;";
  ss << "}";

  ss << "QPushButton:hover  {";
  ss << fontString;
  ss << "background-color: " << offBgColor.name() << ";";
  ss << "color:" << offTextColor.name() << ";";
  ss << "border: 2px solid " << offBorderColor.name() << ";";
  ss << "border-radius: " << borderRadius << "px;";
  ss << "padding: 1 8 1 8px;";
  ss << "margin: 1 1 1 1px;";
  ss << "}";

  ss << "QPushButton:checked {";
  ss << fontString;
  ss << "background-color: " << onBgColor.name() << ";";
  ss << "color:" << onTextColor.name() << ";";
  ss << "border: 1px solid " << onBorderColor.name() << ";";
  ss << "border-radius: " << borderRadius << "px;";
  ss << "padding: 1 8 1 8px;";
  ss << "margin: 2 2 2 2px;";
  ss << "}";

  ss << "QPushButton:checked:hover  {";
  ss << fontString;
  ss << "background-color: " << onBgColor.name() << ";";
  ss << "color:" << onTextColor.name() << ";";
  ss << "border: 1px solid " << onBorderColor.name() << ";";
  ss << "border-radius: " << borderRadius << "px;";
  ss << "padding: 1 8 1 8px;";
  ss << "margin: 1 1 1 1px;";
  ss << "}";

  return style;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::issuesVisibilityChanged(bool b)
{
  issuesBtn->blockSignals(true);
  issuesBtn->setChecked(b);
  issuesBtn->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::consoleVisibilityChanged(bool b)
{
  consoleBtn->blockSignals(true);
  consoleBtn->setChecked(b);
  consoleBtn->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::dataBrowserVisibilityChanged(bool b)
{
  dataBrowserBtn->blockSignals(true);
  dataBrowserBtn->setChecked(b);
  dataBrowserBtn->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::pipelineVisibilityChanged(bool b)
{
  pipelineBtn->blockSignals(true);
  pipelineBtn->setChecked(b);
  pipelineBtn->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::filterListVisibilityChanged(bool b)
{
  filterListBtn->blockSignals(true);
  filterListBtn->setChecked(b);
  filterListBtn->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::filterLibraryVisibilityChanged(bool b)
{
  filterLibraryBtn->blockSignals(true);
  filterLibraryBtn->setChecked(b);
  filterLibraryBtn->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::bookmarksVisibilityChanged(bool b)
{
  bookmarksBtn->blockSignals(true);
  bookmarksBtn->setChecked(b);
  bookmarksBtn->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::setButtonAction(QDockWidget* dock, Button btn)
{
  switch(btn)
  {
    case Button::Issues:
    {
      connect(issuesBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::issuesVisibilityChanged);
      m_IssuesBtnToggleAction = addButtonVisibilityAction(issuesBtn);
      break;
    }
    case Button::Console:
    {
      connect(consoleBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::consoleVisibilityChanged);
      m_ConsoleBtnToggleAction = addButtonVisibilityAction(consoleBtn);
      break;
    }
    case Button::DataStructure:
    {
      connect(dataBrowserBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::dataBrowserVisibilityChanged);
      m_DataBrowserBtnToggleAction = addButtonVisibilityAction(dataBrowserBtn);
      break;
    }
    case Button::Pipeline:
    {
      connect(pipelineBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::pipelineVisibilityChanged);
      m_PipelineBtnToggleAction = addButtonVisibilityAction(pipelineBtn);
      break;
    }
    case Button::FilterList:
    {
      connect(filterListBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::filterListVisibilityChanged);
      m_FilterListBtnToggleAction = addButtonVisibilityAction(filterListBtn);
      break;
    }
    case Button::FilterLibrary:
    {
      connect(filterLibraryBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::filterLibraryVisibilityChanged);
      m_FilterLibraryBtnToggleAction = addButtonVisibilityAction(filterLibraryBtn);
      break;
    }
    case Button::Bookmarks:
    {
      connect(bookmarksBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::bookmarksVisibilityChanged);
      m_BookmarksBtnToggleAction = addButtonVisibilityAction(bookmarksBtn);
      break;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* StatusBarWidget::addButtonVisibilityAction(StatusBarButton* button)
{
  QAction* action = new QAction(button->text());
  action->setCheckable(true);
  action->setChecked(true);
  connect(action, &QAction::toggled, button, &StatusBarButton::setVisible);
  m_ButtonListMenu->addAction(action);

  return action;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::issuesTableHasErrors(bool /* b */, int errCount, int warnCount)
{
  issuesBtn->setErrorBadgeCount(errCount);
  issuesBtn->setWarningBadgeCount(warnCount);
}
