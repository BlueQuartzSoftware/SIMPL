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


#ifndef _simplviewtoolbox_h_
#define _simplviewtoolbox_h_

#include <QtWidgets/QMainWindow>

#include "SVWidgetsLib/SVWidgetsLib.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

//-- UIC generated Header
#include "ui_SIMPLViewToolbox.h"

class BookmarksToolboxWidget;
class FilterListToolboxWidget;
class FilterLibraryToolboxWidget;

class SVWidgetsLib_EXPORT SIMPLViewToolbox : public QMainWindow, private Ui::SIMPLViewToolbox
{
    Q_OBJECT

  public:
    SIMPL_INSTANCE_PROPERTY(QAction*, ActionShowToolbox)
    SIMPL_INSTANCE_PROPERTY(QAction*, ActionShowBookmarks)
    SIMPL_INSTANCE_PROPERTY(QAction*, ActionShowFilterList)
    SIMPL_INSTANCE_PROPERTY(QAction*, ActionShowFilterLibrary)

    virtual ~SIMPLViewToolbox();

    static SIMPLViewToolbox* Instance(QWidget* parent = nullptr, Qt::WindowFlags flags = nullptr);

    enum ToolboxTabs
    {
      FilterLibrary,
      FilterList,
      Bookmarks
    };

    void setupGui();

    BookmarksToolboxWidget* getBookmarksWidget();
    FilterListToolboxWidget* getFilterListWidget();
    FilterLibraryToolboxWidget* getFilterLibraryWidget();

    void readSettings();
    void readWindowSettings(QtSSettings* prefs);

    void writeSettings();
    void writeWindowSettings(QtSSettings* prefs);

    void setCurrentTab(ToolboxTabs tab);

  protected:
    SIMPLViewToolbox(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    void changeEvent(QEvent* event);
    void closeEvent(QCloseEvent* event);

  protected slots:
    void actionShowFilterLibrary_triggered(bool enabled);
    void actionShowFilterList_triggered(bool enabled);
    void actionShowBookmarks_triggered(bool enabled);

  signals:
    void toolboxChangedState();

  private:
    static SIMPLViewToolbox*                              self;

    SIMPLViewToolbox(const SIMPLViewToolbox&) = delete; // Copy Constructor Not Implemented
    void operator=(const SIMPLViewToolbox&) = delete;   // Move assignment Not Implemented
};

#endif // SIMPLViewToolbox_H
