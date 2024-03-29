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

#include "StandardOutputWidget.h"

#include <iostream>

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>

#include "SVWidgetsLib/QtSupport/QtSSettings.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLView::DockWidgetSettings::HideDockSetting StandardOutputWidget::GetHideDockSetting()
{
  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings());
  prefs->beginGroup(SIMPLView::DockWidgetSettings::GroupName);
  prefs->beginGroup(SIMPLView::DockWidgetSettings::StandardOutputGroupName);

  int showError = static_cast<int>(SIMPLView::DockWidgetSettings::HideDockSetting::Ignore);
  int hideDockSetting = prefs->value(SIMPLView::DockWidgetSettings::KeyName, QVariant(showError)).toInt();
  auto value = static_cast<SIMPLView::DockWidgetSettings::HideDockSetting>(hideDockSetting);

  prefs->endGroup();
  prefs->endGroup();

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardOutputWidget::SetHideDockSetting(SIMPLView::DockWidgetSettings::HideDockSetting setting)
{
  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings());
  prefs->beginGroup(SIMPLView::DockWidgetSettings::GroupName);
  prefs->beginGroup(SIMPLView::DockWidgetSettings::StandardOutputGroupName);

  prefs->setValue(SIMPLView::DockWidgetSettings::KeyName, static_cast<int>(setting));

  prefs->endGroup();
  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StandardOutputWidget::StandardOutputWidget(QWidget* parent)
: QWidget(parent)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StandardOutputWidget::~StandardOutputWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardOutputWidget::requestContextMenu(const QPoint& pos)
{
  QPoint mapped = mapToGlobal(pos);
  m_ContextMenuPoint = mapped;
  QMenu menu;
  QAction* clearOutputAction = new QAction(QString("Clear Output"), &menu);
  connect(clearOutputAction, SIGNAL(triggered()), this, SLOT(clearLog()));
  menu.addAction(clearOutputAction);

  QAction* saveOutput = new QAction(QString("Save Output..."), &menu);
  connect(saveOutput, SIGNAL(triggered()), this, SLOT(saveLog()));
  menu.addAction(saveOutput);

  menu.exec(mapped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardOutputWidget::setupGui()
{
  stdOutTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(stdOutTextEdit, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(requestContextMenu(const QPoint&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardOutputWidget::saveLog()
{
  QString s = QString("Text Files (*.txt *.log);;All Files(*.*)");
  QString defaultName = m_LastPathOpened + QDir::separator() + "Untitled";
  QString filePath = QFileDialog::getSaveFileName(this, tr("Save File As"), defaultName, s);
  if(filePath.isEmpty())
  {
    return;
  }

  filePath = QDir::toNativeSeparators(filePath);

  QFileInfo fi(filePath);
  m_LastPathOpened = fi.path();

  QFile file(filePath);
  if(file.open(QFile::WriteOnly))
  {
    file.write(stdOutTextEdit->toPlainText().toStdString().c_str());
    file.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardOutputWidget::clearLog()
{
  int answer;

  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings());
  bool displayDialog = prefs->value("DisplayClearMessageBox", true).toBool();
  if(displayDialog)
  {
    QCheckBox* cb = new QCheckBox("Do not ask me this again");
    QMessageBox msgBox;
    msgBox.setWindowTitle("Clear Standard Output Log");
    msgBox.setText("Are you sure that you want to clear the standard output log?");
    msgBox.setIcon(QMessageBox::Icon::Warning);
    msgBox.addButton(QMessageBox::No);
    msgBox.addButton(QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    // Takes ownership
    msgBox.setCheckBox(cb);

    answer = msgBox.exec();

    displayDialog = !cb->isChecked();

    prefs->setValue("DisplayClearMessageBox", displayDialog);
  }
  else
  {
    answer = QMessageBox::Yes;
  }

  if(answer == QMessageBox::Yes)
  {
    stdOutTextEdit->clear();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardOutputWidget::appendText(const QString& text)
{
  stdOutTextEdit->append(text);
  stdOutTextEdit->ensureCursorVisible();
}
