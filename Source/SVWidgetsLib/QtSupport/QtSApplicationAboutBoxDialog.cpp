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

#include "QtSApplicationAboutBoxDialog.h"

#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QTextStream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSApplicationAboutBoxDialog::QtSApplicationAboutBoxDialog(QStringList files, QWidget* parent)
: QDialog(parent)
{
  this->setupUi(this);
  setLicenseFiles(files);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSApplicationAboutBoxDialog::~QtSApplicationAboutBoxDialog() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSApplicationAboutBoxDialog::setApplicationInfo(QString applicationName, QString version)
{
  QString title("About ");
  title.append(applicationName);
  setWindowTitle(title);
  appName->setText(applicationName);
  appVersion->setText(version);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSApplicationAboutBoxDialog::setLicenseFiles(QStringList files)
{
  m_licenseFiles = files;
  licenseCombo->clear();
  m_licenseFiles = files;
  for(int i = 0; i < m_licenseFiles.size(); ++i)
  {
    QString s = m_licenseFiles[i];
    s.remove(0, 2);
    s.remove(".license", Qt::CaseSensitive);
    licenseCombo->addItem(s);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSApplicationAboutBoxDialog::on_licenseCombo_currentIndexChanged(int index)
{
  // qDebug() << "on_licenseCombo_action" << "\n";
  QString resourceFile = m_licenseFiles[licenseCombo->currentIndex()];
  loadResourceFile(resourceFile);
}

// -----------------------------------------------------------------------------
void QtSApplicationAboutBoxDialog::on_okButton_clicked()
{
  this->close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSApplicationAboutBoxDialog::loadResourceFile(const QString qresourceFile)
{
  QFile inputFile(qresourceFile);
  inputFile.open(QIODevice::ReadOnly);
  QTextStream in(&inputFile);
  QString line = in.readAll();
  inputFile.close();

  //  appHelpText->append(line);
  appHelpText->setHtml(line);
  appHelpText->setUndoRedoEnabled(false);
  appHelpText->setUndoRedoEnabled(true);
}
