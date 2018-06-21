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

#ifndef _statusBarWidget_h_
#define _statusBarWidget_h_

#include <QtWidgets/QFrame>

#include "ui_StatusBarWidget.h"

class QDockWidget;
class QMainWindow;

class SVWidgetsLib_EXPORT StatusBarWidget : public QFrame, private Ui::StatusBarWidget
{

    Q_OBJECT

  public:
    StatusBarWidget(QWidget* parent = nullptr);
    virtual ~StatusBarWidget();


    using EnumType = unsigned int;

    enum class Button : EnumType
    {
      Issues = 0,
      Console = 1,
      DataStructure = 2,
      Pipeline = 3,
      FilterList = 4,
      FilterLibrary = 5,
      Bookmarks = 6
    };

    /**
     * @brief readSettings
     */
    void readSettings();

    /**
     * @brief writeSettings
     */
    void writeSettings();

    /**
     * @brief setButtonAction
     * @param action
     * @param btn
     */
    void setButtonAction(QDockWidget *dock, Button btn);

    /**
     * @brief updateStyle
     */
    void updateStyle();

    /**
     * @brief generateStyleSheet
     * @param error
     * @return
     */
    QString generateStyleSheet(bool error);

  public slots:
    /**
     * @brief issuesVisibilityChanged
     * @param b
     */
    void issuesVisibilityChanged(bool b);
    /**
     * @brief consoleVisibilityChanged
     * @param b
     */
    void consoleVisibilityChanged(bool b);
    /**
     * @brief dataBrowserVisibilityChanged
     * @param b
     */
    void dataBrowserVisibilityChanged(bool b);

    /**
     * @brief pipelineVisibilityChanged
     * @param b
     */
    void pipelineVisibilityChanged(bool b);

    /**
     * @brief filterListVisibilityChanged
     * @param b
     */
    void filterListVisibilityChanged(bool b);

    /**
     * @brief filterLibraryVisibilityChanged
     * @param b
     */
    void filterLibraryVisibilityChanged(bool b);

    /**
     * @brief bookmarksVisibilityChanged
     * @param b
     */
    void bookmarksVisibilityChanged(bool b);
    
    /**
     * @brief issuesTableHasErrors
     * @param b
     */
    void issuesTableHasErrors(bool b, int errCount, int warnCount);

  protected:

    /**
     * @brief setupGui
     */
    void setupGui();

  private:
    QMenu* m_ButtonListMenu = nullptr;

    QAction* m_IssuesBtnToggleAction = nullptr;
    QAction* m_ConsoleBtnToggleAction = nullptr;
    QAction* m_DataBrowserBtnToggleAction = nullptr;
    QAction* m_PipelineBtnToggleAction = nullptr;
    QAction* m_FilterListBtnToggleAction = nullptr;
    QAction* m_FilterLibraryBtnToggleAction = nullptr;
    QAction* m_BookmarksBtnToggleAction = nullptr;

    /**
     * @brief addButtonVisibilityAction
     * @param button
     */
    QAction* addButtonVisibilityAction(StatusBarButton* button);

    StatusBarWidget(const StatusBarWidget&) = delete; // Copy Constructor Not Implemented
    void operator=(const StatusBarWidget&) = delete;  // Move assignment Not Implemented
};

#endif /* _statusBarWidget_H_ */
