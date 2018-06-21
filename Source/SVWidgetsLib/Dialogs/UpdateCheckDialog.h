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

#ifndef _UpdateCheckDialog_h_
#define _UpdateCheckDialog_h_

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QSettings>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>

#include "SVWidgetsLib/QtSupport/QtSSettings.h"


#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Dialogs/UpdateCheck.h"


#include "ui_UpdateCheckDialog.h"

class QNetworkAccessManager;
class QNetworkReply;
class UpdateCheck;
class UpdateCheckData;

/**
 * @brief The UpdateCheckDialog class
 */
class SVWidgetsLib_EXPORT UpdateCheckDialog : public QDialog, private Ui::UpdateCheckDialog
{
    Q_OBJECT

  public:
    UpdateCheckDialog(UpdateCheck::SIMPLVersionData_t versionData, QWidget* parent = nullptr);
    virtual ~UpdateCheckDialog();

    enum UpdateType
    {
      UpdateCheckDaily,
      UpdateCheckWeekly,
      UpdateCheckMonthly,
      UpdateCheckManual
    };

    enum DialogState
    {
      SimpleDialog,
      DefaultDialog
    };

    int getWhenToCheck();

    QString getUpdatePreferencesPath();
    QLabel* getCurrentVersionLabel();
    QLabel* getLatestVersionLabel();
    QString getAppName();
    QLabel* getFeedbackTextLabel();

    QRadioButton* getAutomaticallyBtn();
    QRadioButton* getManuallyBtn();
    QComboBox* getHowOftenComboBox();
    QPushButton* getCheckNowBtn();

    static QString GetUpdatePreferencesGroup();
    static QString GetUpdateCheckKey();


    void setLastCheckDateTime(QDateTime lastDateTime);
    void setWhenToCheck(int whenToCheck);
    void setUpdateWebSite(QString url);
    void setApplicationName(QString name);

    void readUpdatePreferences(QtSSettings& prefs);
    void writeUpdatePreferences(QtSSettings& prefs);

    /**
    * @brief Hides the UI items necessary to change the dialog into a simple prompt
    * that notifies of new updates and only provides a link to check the website.
    */
    void toSimpleUpdateCheckDialog();

    /**
    * @brief Shows the UI items necessary to change the dialog into its
    * default, more complex prompt that allows the user to change updates
    * to manual or automatic, and press a "Check Now" button to manually
    * check for updates.
    */
    void toDefaultUpdateCheckDialog();

    void setupGui();

  public slots:
    void on_checkNowBtn_clicked();
    void on_howOften_currentIndexChanged(int);
    void on_automatically_toggled(bool);
    void on_manually_toggled(bool);

  protected slots:
    void LatestVersionReplied(UpdateCheckData*);

  signals:
    void finished();
    void hasMessage(const QString& message);
    void updateFound();

  private:
    QDateTime         m_LastCheckDateTime;
    int             m_WhenToCheck;
    QString           m_UpdatePreferencesPath;
    QString           m_UpdateWebSite;
    UpdateCheck*        m_UpdateCheck;
    DialogState         m_DialogState;
    UpdateCheck::SIMPLVersionData_t m_VersionData;

    QAction*            m_CloseAction = nullptr;

    UpdateCheckDialog(const UpdateCheckDialog&) = delete; // Copy Constructor Not Implemented
    void operator=(const UpdateCheckDialog&) = delete;    // Move assignment Not Implemented
};



#endif /* _UpdateCheckDialog_H_ */

