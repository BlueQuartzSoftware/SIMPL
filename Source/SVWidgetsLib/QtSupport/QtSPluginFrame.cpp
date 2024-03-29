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

#include "QtSPluginFrame.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>

#include "QtSupport/QtSFileUtils.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSPluginFrame::QtSPluginFrame(QWidget* parent)
: QFrame(parent)
, m_StatusBar(nullptr)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSPluginFrame::~QtSPluginFrame() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QtSPluginFrame::verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(outFilePath);
  QDir parent(fileinfo.dir());
  //  if (false == parent.exists() )
  //  {
  //    lineEdit->setStyleSheet("border: 1px solid red;");
  //  }
  //  else
  //  {
  //    lineEdit->setStyleSheet("");
  //  }
  return parent.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSPluginFrame::setStatusBar(QStatusBar* statusBar)
{
  this->m_StatusBar = statusBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStatusBar* QtSPluginFrame::statusBar()
{
  return this->m_StatusBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QtSPluginFrame::sanityCheckOutputDirectory(QLineEdit* le, QString msgTitle)
{

  if(le->text().isEmpty())
  {
    QMessageBox::critical(this, msgTitle, "The output directory has NOT been set. Please set a directory path and try again.", QMessageBox::Ok | QMessageBox::Default);
    return false;
  }

  if(!QtSFileUtils::VerifyPathExists(le->text(), le))
  {
    QString msg("The Output Directory '");
    msg.append(le->text()).append("'\ndoes not exist. Would you like to create it?");
    int ret = QMessageBox::warning(this, msgTitle, msg, QMessageBox::Yes | QMessageBox::Default, QMessageBox::No);
    if(ret == QMessageBox::No)
    {
      return false;
    }
    if(ret == QMessageBox::Yes)
    {
      QDir outputDir(le->text());
      if(!outputDir.exists())
      {
        bool ok = outputDir.mkpath(".");
        if(!ok)
        {
          QMessageBox::critical(this, tr("Output Directory Creation"), tr("The output directory could not be created."), QMessageBox::Ok);
          return false;
        }

        return true;
      }
    }
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSPluginFrame::displayDialogBox(QString title, QString text, QMessageBox::Icon icon)
{

  QMessageBox msgBox;
  msgBox.setText(title);
  msgBox.setInformativeText(text);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.setIcon(icon);
  msgBox.exec();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSPluginFrame::writeSettings(QSettings& prefs)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSPluginFrame::readSettings(QSettings& prefs, bool shouldClear)
{
}
