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

#include "UpdateCheckDialog.h"

#include <iostream>

#include <QtCore/QDate>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtGui/QDesktopServices>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "SVWidgetsLib/Dialogs/UpdateCheckData.h"



namespace Detail
{
const QString UpdatePreferencesGroup("UpdatePreferences");
const QString UpdateCheckDateKey("LastUpdateCheckDate");
const QString UpdateFrequencyKey("Frequency");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCheckDialog::UpdateCheckDialog(UpdateCheck::SIMPLVersionData_t versionData, QWidget* parent)
: SVDialog(parent)
, m_WhenToCheck(UpdateCheckMonthly)
, m_UpdateCheck(nullptr)
, m_DialogState(DefaultDialog)
, m_VersionData(versionData)
{

  setupUi(this);

  setupGui();

  QtSSettings updatePrefs;

  updatePrefs.beginGroup(Detail::UpdatePreferencesGroup);
  // If the update preferences exist in the preferences file, read them in
  if(updatePrefs.contains(Detail::UpdateFrequencyKey))
  {
    updatePrefs.endGroup();
    readUpdatePreferences(updatePrefs);
  }
  // Otherwise, write and set the defaults
  else
  {
    updatePrefs.endGroup();
    automatically->setChecked(true);
    howOften->setCurrentIndex(UpdateCheckMonthly);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCheckDialog::~UpdateCheckDialog()
{
#if defined(Q_OS_MAC)
  delete m_CloseAction;
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UpdateCheckDialog::GetUpdatePreferencesGroup()
{
  return Detail::UpdatePreferencesGroup;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UpdateCheckDialog::GetUpdateCheckKey()
{
  return Detail::UpdateCheckDateKey;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRadioButton* UpdateCheckDialog::getAutomaticallyBtn()
{
  return automatically;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRadioButton* UpdateCheckDialog::getManuallyBtn()
{
  return manually;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QComboBox* UpdateCheckDialog::getHowOftenComboBox()
{
  return howOften;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPushButton* UpdateCheckDialog::getCheckNowBtn()
{
  return checkNowBtn;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLabel* UpdateCheckDialog::getCurrentVersionLabel()
{
  return currentVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLabel* UpdateCheckDialog::getLatestVersionLabel()
{
  return latestVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UpdateCheckDialog::getAppName()
{
  return m_VersionData.appName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLabel* UpdateCheckDialog::getFeedbackTextLabel()
{
  return feedbackText;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UpdateCheckDialog::getUpdatePreferencesPath()
{
  return m_UpdatePreferencesPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::setLastCheckDateTime(QDateTime lastDateTime)
{
  m_LastCheckDateTime = lastDateTime;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::setApplicationName(QString name)
{
  m_VersionData.appName = name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int UpdateCheckDialog::getWhenToCheck()
{
  return m_WhenToCheck;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::setWhenToCheck(int whenToCheck)

{
  m_WhenToCheck = whenToCheck;
  if(m_WhenToCheck == UpdateCheckDialog::UpdateCheckManual)
  {
    manually->setChecked(true);
    howOften->setEnabled(false);
  }
  else
  {
    automatically->setChecked(true);
    howOften->setCurrentIndex(whenToCheck);
    howOften->setEnabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::setUpdateWebSite(QString url)
{
  m_UpdateWebSite = url;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::setupGui()
{
  latestVersion->setText("Not Checked");
  feedbackText->setText("");

  QString vers = QString("%1.%2.%3").arg(m_VersionData.major).arg(m_VersionData.minor).arg(m_VersionData.patch);
  currentVersion->setText(vers);

  setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

#if defined(Q_OS_MAC)
  m_CloseAction = new QAction(this);
  m_CloseAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
  connect(m_CloseAction, SIGNAL(triggered()), this, SLOT(close()));
  addAction(m_CloseAction);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::on_checkNowBtn_clicked()
{
  checkNowBtn->setEnabled(false);
  feedbackText->setText("Checking for Updates...");
  m_UpdateCheck = new UpdateCheck(m_VersionData, this);

  connect(m_UpdateCheck, SIGNAL(latestVersion(UpdateCheckData*)), this, SLOT(LatestVersionReplied(UpdateCheckData*)));

  m_UpdateCheck->checkVersion(m_UpdateWebSite);
  checkNowBtn->setEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::on_howOften_currentIndexChanged(int index)
{
  if(index == UpdateCheckDialog::UpdateCheckDaily)
  {
    setWhenToCheck(UpdateCheckDialog::UpdateCheckDaily);
  }
  else if(index == UpdateCheckDialog::UpdateCheckWeekly)
  {
    setWhenToCheck(UpdateCheckDialog::UpdateCheckWeekly);
  }
  else if(index == UpdateCheckDialog::UpdateCheckMonthly)
  {
    setWhenToCheck(UpdateCheckDialog::UpdateCheckMonthly);
  }

  QtSSettings updatePrefs;
  writeUpdatePreferences(updatePrefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::on_automatically_toggled(bool boolValue)
{
  if(automatically->isChecked())
  {
    if(howOften->currentIndex() == UpdateCheckDialog::UpdateCheckDaily)
    {
      setWhenToCheck(UpdateCheckDialog::UpdateCheckDaily);
    }
    else if(howOften->currentIndex() == UpdateCheckDialog::UpdateCheckWeekly)
    {
      setWhenToCheck(UpdateCheckDialog::UpdateCheckWeekly);
    }
    else if(howOften->currentIndex() == UpdateCheckDialog::UpdateCheckMonthly)
    {
      setWhenToCheck(UpdateCheckDialog::UpdateCheckMonthly);
    }

    QtSSettings updatePrefs;
    writeUpdatePreferences(updatePrefs);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::on_manually_toggled(bool)
{
  if(manually->isChecked())
  {
    setWhenToCheck(UpdateCheckDialog::UpdateCheckManual);

    QtSSettings updatePrefs;
    writeUpdatePreferences(updatePrefs);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::readUpdatePreferences(QtSSettings& prefs)
{
  // Read in value from preferences file
  prefs.beginGroup(Detail::UpdatePreferencesGroup);
  bool ok = false;
  m_WhenToCheck = static_cast<UpdateType>(prefs.value(Detail::UpdateFrequencyKey, -1).toInt(&ok));
  prefs.endGroup();

  if(m_WhenToCheck == UpdateCheckManual)
  {
    manually->blockSignals(true);
    manually->setChecked(true);
    howOften->setEnabled(false);
    manually->blockSignals(false);
  }
  else
  {
    automatically->blockSignals(true);
    manually->blockSignals(true);
    howOften->blockSignals(true);
    automatically->setChecked(true);
    if(m_WhenToCheck == UpdateCheckDaily)
    {
      howOften->setCurrentIndex(UpdateCheckDaily);
    }
    else if(m_WhenToCheck == UpdateCheckWeekly)
    {
      howOften->setCurrentIndex(UpdateCheckWeekly);
    }
    else
    {
      howOften->setCurrentIndex(UpdateCheckMonthly);
    }
    automatically->blockSignals(false);
    manually->blockSignals(false);
    howOften->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::writeUpdatePreferences(QtSSettings& prefs)
{
  prefs.beginGroup(Detail::UpdatePreferencesGroup);
  prefs.setValue(Detail::UpdateFrequencyKey, m_WhenToCheck);
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::LatestVersionReplied(UpdateCheckData* dataObj)
{
  QString message = dataObj->getMessageDescription();
  feedbackText->setText(message);
  if(!dataObj->hasError())
  {
    // currentVersion->setText( dataObj->getAppString() );
    // setCurrentVersion( dataObj->getAppString() );
    latestVersion->setText(dataObj->getServerString());
  }
  else
  {
    latestVersion->setText("Error!");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::toSimpleUpdateCheckDialog()
{
  // Exit immediately if the dialog is already in simple state
  if(m_DialogState == SimpleDialog)
  {
    return;
  }

  checkNowBtn->setVisible(false);
  automatically->setVisible(false);
  manually->setVisible(false);
  howOften->setVisible(false);
  messageLabel->setText("Update Available!");

  // Update Dialog State
  m_DialogState = SimpleDialog;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckDialog::toDefaultUpdateCheckDialog()
{
  // Exit immediately if the dialog is already in default state
  if(m_DialogState == DefaultDialog)
  {
    return;
  }

  checkNowBtn->setVisible(true);
  automatically->setVisible(true);
  manually->setVisible(true);
  howOften->setVisible(true);
  messageLabel->setText("How would you like to check for updates?");

  // Update Dialog State
  m_DialogState = DefaultDialog;
}
