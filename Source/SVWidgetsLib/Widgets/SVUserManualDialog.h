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

#ifndef _svusermanualdialog_h_
#define _svusermanualdialog_h_

#include "SVWidgetsLib/SVWidgetsLib.h"

#include <QtWidgets/QDialog>

#include <QtWebEngineWidgets/QWebEngineView>

#include "ui_SVUserManualDialog.h"

/**
 * @brief The SVUserManualDialog class
 */
class SVWidgetsLib_EXPORT SVUserManualDialog : public QDialog, public Ui::SVUserManualDialog
{
  Q_OBJECT

public:
  /**
  * @brief Destructor
  * @return
  */
  virtual ~SVUserManualDialog();

  /**
  * @brief Static instance to retrieve the global instance of this class
  * @return
  */
  static SVUserManualDialog* Instance(QWidget* parent = nullptr);

  /**
  * @brief Static function to launch the help dialog with the given URL
  * @return
  */
  static void LaunchHelpDialog(QUrl url);

  /**
  * @brief Static function to launch the help dialog with the given filter's classname
  * @param className The name of the Filter's class in C++ as given by the "getNameOfClass()" static function of the filter.
  * @return
  */
  static void LaunchHelpDialog(QString className);

  /**
  * @brief getWebView
  * @return QWebEngineView*
  */
  QWebEngineView* getWebView();

protected:
  SVUserManualDialog(QWidget* parent = nullptr);

protected slots:
  void on_backBtn_clicked();
  void on_forwardBtn_clicked();
  void on_refreshBtn_clicked();
  void updateButtons(bool ok);

private:
  static SVUserManualDialog* self;

  QWebEngineView* m_WebView;

  QAction* m_CloseAction = nullptr;

  SVUserManualDialog(const SVUserManualDialog&) = delete; // Copy Constructor Not Implemented
  void operator=(const SVUserManualDialog&) = delete;     // Operator '=' Not Implemented
};

#endif /* _svusermanualdialog_h_ */
