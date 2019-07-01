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
#include <QtWidgets/QMenu>

#include "SVWidgetsLib/Widgets/SVStyle.h"
#include "SVWidgetsLib/QtSupport/QtSSettings.h"

namespace Constants
{
  const QString StatusBarGroup = "Status Bar Widget";
  const QString VisibilityGroup = "Button Visibility";
  const QString FilterListPref = "Filter List";
  const QString FilterLibraryPref = "Filter Library";
  const QString BookmarksPref = "Bookmarks";
  const QString PipelinePref = "Pipeline";
  const QString IssuesPref = "Issues";
  const QString DataBrowserPref = "Data Browser";
  const QString ConsolePref = "Console";
  const QString RenderPropPref = "Render Properties";
  const QString VisualFiltersPref = "Visual Filters";
  const QString VisualFilterSettingsPref = "Visual Filter Settings";
}

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

  issuesBtn->setVisible(false);
  consoleBtn->setVisible(false);
  dataBrowserBtn->setVisible(false);
  pipelineBtn->setVisible(false);
  filterListBtn->setVisible(false);
  filterLibraryBtn->setVisible(false);
  bookmarksBtn->setVisible(false);
  renderPropBtn->setVisible(false);
  visualFiltersBtn->setVisible(false);
  visualFilterSettingsBtn->setVisible(false);

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

  prefs->beginGroup(Constants::StatusBarGroup);

  prefs->beginGroup(Constants::VisibilityGroup);
  m_FilterListBtnToggleAction->setChecked(prefs->value(Constants::FilterListPref, QVariant(false)).toBool());
  m_FilterLibraryBtnToggleAction->setChecked(prefs->value(Constants::FilterLibraryPref, QVariant(false)).toBool());
  m_BookmarksBtnToggleAction->setChecked(prefs->value(Constants::BookmarksPref, QVariant(false)).toBool());
  m_PipelineBtnToggleAction->setChecked(prefs->value(Constants::PipelinePref, QVariant(false)).toBool());
  m_IssuesBtnToggleAction->setChecked(prefs->value(Constants::IssuesPref, QVariant(true)).toBool());
  //m_DataBrowserBtnToggleAction->setChecked(prefs->value(Constants::DataBrowserPref, QVariant(true)).toBool());
  m_ConsoleBtnToggleAction->setChecked(prefs->value(Constants::ConsolePref, QVariant(true)).toBool());
  m_RenderPropBtnToggleAction->setChecked(prefs->value(Constants::RenderPropPref, QVariant(true)).toBool());
  m_VisualFiltersBtnToggleAction->setChecked(prefs->value(Constants::VisualFiltersPref, QVariant(true)).toBool());
  m_VisualFilterSettingsBtnToggleAction->setChecked(prefs->value(Constants::VisualFilterSettingsPref, QVariant(true)).toBool());
  prefs->endGroup();

  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::writeSettings()
{
  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings());

  prefs->beginGroup(Constants::StatusBarGroup);

  prefs->beginGroup(Constants::VisibilityGroup);
  prefs->setValue(Constants::FilterListPref, m_FilterListBtnToggleAction->isChecked());
  prefs->setValue(Constants::FilterLibraryPref, m_FilterLibraryBtnToggleAction->isChecked());
  prefs->setValue(Constants::BookmarksPref, m_BookmarksBtnToggleAction->isChecked());
  prefs->setValue(Constants::PipelinePref, m_PipelineBtnToggleAction->isChecked());
  prefs->setValue(Constants::IssuesPref, m_IssuesBtnToggleAction->isChecked());
  //prefs->setValue(Constants::DataBrowserPref, m_DataBrowserBtnToggleAction->isChecked());
  prefs->setValue(Constants::ConsolePref, m_ConsoleBtnToggleAction->isChecked());
  prefs->setValue(Constants::RenderPropPref, m_RenderPropBtnToggleAction->isChecked());
  prefs->setValue(Constants::VisualFiltersPref, m_VisualFiltersBtnToggleAction->isChecked());
  prefs->setValue(Constants::VisualFilterSettingsPref, m_VisualFilterSettingsBtnToggleAction->isChecked());
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
void StatusBarWidget::renderPropVisibilityChanged(bool b)
{
  renderPropBtn->blockSignals(true);
  renderPropBtn->setChecked(b);
  renderPropBtn->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::visualFiltersVisibilityChanged(bool b)
{
  visualFiltersBtn->blockSignals(true);
  visualFiltersBtn->setChecked(b);
  visualFiltersBtn->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatusBarWidget::visualFilterSettingsVisibilityChanged(bool b)
{
  visualFilterSettingsBtn->blockSignals(true);
  visualFilterSettingsBtn->setChecked(b);
  visualFilterSettingsBtn->blockSignals(false);
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
      issuesBtn->setVisible(true);
      connect(issuesBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::issuesVisibilityChanged);
      m_IssuesBtnToggleAction = addButtonVisibilityAction(issuesBtn);
      break;
    }
    case Button::Console:
    {
      consoleBtn->setVisible(true);
      connect(consoleBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::consoleVisibilityChanged);
      m_ConsoleBtnToggleAction = addButtonVisibilityAction(consoleBtn);
      break;
    }
    case Button::DataStructure:
    {
      dataBrowserBtn->setVisible(true);
      connect(dataBrowserBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::dataBrowserVisibilityChanged);
      m_DataBrowserBtnToggleAction = addButtonVisibilityAction(dataBrowserBtn);
      break;
    }
    case Button::Pipeline:
    {
      pipelineBtn->setVisible(true);
      connect(pipelineBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::pipelineVisibilityChanged);
      m_PipelineBtnToggleAction = addButtonVisibilityAction(pipelineBtn);
      break;
    }
    case Button::FilterList:
    {
      filterListBtn->setVisible(true);
      connect(filterListBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::filterListVisibilityChanged);
      m_FilterListBtnToggleAction = addButtonVisibilityAction(filterListBtn);
      break;
    }
    case Button::FilterLibrary:
    {
      filterLibraryBtn->setVisible(true);
      connect(filterLibraryBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::filterLibraryVisibilityChanged);
      m_FilterLibraryBtnToggleAction = addButtonVisibilityAction(filterLibraryBtn);
      break;
    }
    case Button::Bookmarks:
    {
      bookmarksBtn->setVisible(true);
      connect(bookmarksBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::bookmarksVisibilityChanged);
      m_BookmarksBtnToggleAction = addButtonVisibilityAction(bookmarksBtn);
      break;
    }
    case Button::RenderProperties:
      renderPropBtn->setVisible(true);
      connect(renderPropBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::renderPropVisibilityChanged);
      m_RenderPropBtnToggleAction = addButtonVisibilityAction(renderPropBtn);
      break;
    case Button::VisualFilters:
      visualFiltersBtn->setVisible(true);
      connect(visualFiltersBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::visualFiltersVisibilityChanged);
      m_VisualFiltersBtnToggleAction = addButtonVisibilityAction(visualFiltersBtn);
      break;
    case Button::VisualFilterSettings:
      visualFilterSettingsBtn->setVisible(true);
      connect(visualFilterSettingsBtn, &StatusBarButton::toggled, dock->toggleViewAction(), &QAction::trigger);
      connect(dock->toggleViewAction(), &QAction::toggled, this, &StatusBarWidget::visualFilterSettingsVisibilityChanged);
      m_VisualFilterSettingsBtnToggleAction = addButtonVisibilityAction(visualFilterSettingsBtn);
      break;
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
