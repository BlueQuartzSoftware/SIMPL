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

#include "StandardOutputWidget.h"

#include <iostream>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFileDialog>

#include "SVWidgetsLib/QtSupport/QtSSettings.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data

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
void StandardOutputWidget::setupGui()
{  
  clearLogBtn->setDisabled(true);
  saveLogBtn->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardOutputWidget::on_saveLogBtn_clicked()
{
  QString s = QString("Text Files (*.txt *.log);;All Files(*.*)");
  QString defaultName = m_LastPathOpened + QDir::separator() + "Untitled";
  QString filePath = QFileDialog::getSaveFileName(this, tr("Save File As"), defaultName, s);
  if (true == filePath.isEmpty()) { return; }

  filePath = QDir::toNativeSeparators(filePath);

  QFileInfo fi(filePath);
  m_LastPathOpened = fi.path();

  QFile file(filePath);
  if (file.open(QFile::WriteOnly))
  {
    file.write(stdOutTextEdit->toPlainText().toStdString().c_str());
    file.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardOutputWidget::on_clearLogBtn_clicked()
{
  int answer;

  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings());
  bool displayDialog = prefs->value("DisplayClearMessageBox", true).toBool();
  if (displayDialog == true)
  {
    QCheckBox* cb = new QCheckBox("Do not ask me this again");
    QMessageBox msgBox;
    msgBox.setWindowTitle("Clear Standard Output Log");
    msgBox.setText("Are you sure that you want to clear the standard output log?");
    msgBox.setIcon(QMessageBox::Icon::Warning);
    msgBox.addButton(QMessageBox::No);
    msgBox.addButton(QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setCheckBox(cb);

    answer = msgBox.exec();

    displayDialog = !cb->isChecked();
    delete cb;

    prefs->setValue("DisplayClearMessageBox", displayDialog);
  }
  else
  {
    answer = QMessageBox::Yes;
  }

  if (answer == QMessageBox::Yes)
  {
    stdOutTextEdit->clear();

    clearLogBtn->setDisabled(true);
    saveLogBtn->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardOutputWidget::appendText(const QString &text)
{
  stdOutTextEdit->append(text);
  stdOutTextEdit->ensureCursorVisible();
  clearLogBtn->setEnabled(true);
  saveLogBtn->setEnabled(true);
}
