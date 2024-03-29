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

#pragma once

#include <QtWidgets/QDialog>

#include "SVWidgetsLib/SVWidgetsLib.h"

//-- UIC generated Header
#include <ui_QtSApplicationAboutBoxDialog.h>

/**
 * @class QtSApplicationAboutBoxDialog QtSApplicationAboutBoxDialog.h R3D/Common/Qt/QtSApplicationAboutBoxDialog.h
 * @brief This will display a simple dialog box that acts like an "About Box" for
 * showing some basic information about the application being used.
 *
 * @date Sept 14, 2010
 * @version 1.0
 */
class SVWidgetsLib_EXPORT QtSApplicationAboutBoxDialog : public QDialog, private Ui::QtSApplicationAboutBoxDialog
{
  Q_OBJECT

public:
  QtSApplicationAboutBoxDialog(QStringList files, QWidget* parent = nullptr);
  ~QtSApplicationAboutBoxDialog() override;

  /**
   * @brief Sets information about the application
   * @param appName
   * @param appVersion
   */
  void setApplicationInfo(QString appName, QString appVersion);

  /**
   * @brief This will set the list of licenses that can be viewed by the
   * about box. The list should contain values such as ":/SomeLicense.license" which
   * are defined in a Qt resource file (*.qrc).
   * @param files The QString list of files to allow the About Box to load.
   */
  void setLicenseFiles(QStringList files);

protected Q_SLOTS:
  void on_licenseCombo_currentIndexChanged(int index);

  void on_okButton_clicked();

protected:
  /**
   * @brief Loads a file that is stored as a resource in the applications Qt resource
   * system.
   * @param qresourceFile The source file to load of the form ":/SomeFile.xxx"
   */
  void loadResourceFile(const QString qresourceFile);

private:
  QStringList m_licenseFiles;

public:
  QtSApplicationAboutBoxDialog(const QtSApplicationAboutBoxDialog&) = delete;            // Copy Constructor Not Implemented
  QtSApplicationAboutBoxDialog(QtSApplicationAboutBoxDialog&&) = delete;                 // Move Constructor Not Implemented
  QtSApplicationAboutBoxDialog& operator=(const QtSApplicationAboutBoxDialog&) = delete; // Copy Assignment Not Implemented
  QtSApplicationAboutBoxDialog& operator=(QtSApplicationAboutBoxDialog&&) = delete;      // Move Assignment Not Implemented
};
