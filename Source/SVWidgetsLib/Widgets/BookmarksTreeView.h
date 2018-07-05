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


#ifndef _bookmarkstreeview_h_
#define _bookmarkstreeview_h_

#include <QtWidgets/QApplication>

#include <QtCore/QJsonObject>

#include <QtWidgets/QMenu>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>

#include "SVWidgetsLib/SVWidgetsLib.h"

#include "SVWidgetsLib/Widgets/BookmarksModel.h"


class PipelineBuilderWidget;
class QAction;
class QTreeWidgetItem;

class SVWidgetsLib_EXPORT BookmarksTreeView : public QTreeView
{
    Q_OBJECT

  public:
    enum ItemType
    {
      Node_Item_Type = 1,
      Leaf_Item_Type = 2,
      Unknown_Item_Type = 3
    };

    SIMPL_GET_PROPERTY(QAction*, ActionRenameBookmark)
    SIMPL_GET_PROPERTY(QAction*, ActionRemoveBookmark)
    SIMPL_GET_PROPERTY(QAction*, ActionShowBookmarkInFileSystem)
    SIMPL_GET_PROPERTY(QAction*, ActionClearBookmarks)
    SIMPL_GET_PROPERTY(QAction*, ActionAddBookmark)
    SIMPL_GET_PROPERTY(QAction*, ActionAddBookmarkFolder)
    SIMPL_GET_PROPERTY(QAction*, ActionLocateFile)
    SIMPL_GET_PROPERTY(QAction*, ActionOpenBookmark)
    SIMPL_GET_PROPERTY(QAction*, ActionExecuteBookmark)

    /**
    * @brief BookmarksTreeView
    * @param parent
    */
    BookmarksTreeView(QWidget* parent = nullptr);

    /**
    * @brief ~BookmarksTreeView()
    */
    ~BookmarksTreeView();

    /**
    * @brief setModel
    * @param model
    */
    void setModel(QAbstractItemModel* model) Q_DECL_OVERRIDE;

    /**
    * @brief BookmarksToolboxWidget::addBookmark
    * @param filePath
    * @param parent
    */
    void addBookmark(const QString& filePath, const QModelIndex& parent);

    /**
    * @brief filterOutDescendants
    * @param indexLis
    */
    QModelIndexList filterOutDescendants(QModelIndexList indexList);

  public slots:
    void collapseIndex(const QModelIndex& index);
    void expandIndex(const QModelIndex& index);

  protected:
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent* event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent* event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent* event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent* event) Q_DECL_OVERRIDE;

    void currentChanged(const QModelIndex& current, const QModelIndex& previous) Q_DECL_OVERRIDE;

    /**
    * @brief Adds the actions in the actionList parameter to the right-click menu
    */
    void addActionList(QList<QAction*> actionList);

  protected slots:
    void listenAddBookmarkTriggered();
    void listenAddBookmarkFolderTriggered();
    void listenRenameBookmarkTriggered();
    void listenRemoveBookmarkTriggered();
    void listenShowBookmarkInFileSystemTriggered();
    void listenExecuteBookmarkTriggered();
    void listenClearBookmarksTriggered();
    void listenOpenBookmarkTriggered();

  signals:
    void currentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
    void folderChangedState(const QModelIndex& index, bool expand);

    void raiseBookmarksWidget();
    void locateBookmarkTriggered();

    void newSIMPLViewInstanceTriggered(const QString &filePath = "", bool execute = false);

    void fireWriteSettings();
    void updateStatusBar(const QString& msg);

  private slots:

    /**
    * @brief mousePressEvent
    * @param event
    */
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    void requestContextMenu(const QPoint &pos);

  private:
    QPoint                                        m_StartPos;
    QString                                       m_OpenDialogLastFilePath;
    QMenu                                         m_Menu;
    QList<QAction*>                               m_NodeActions;
    QList<QAction*>                               m_LeafActions;
    QList<QAction*>                               m_LeafErrorActions;
    QList<QAction*>                               m_DefaultActions;
    QList<QPersistentModelIndex>                  m_IndexesBeingDragged;
    QPersistentModelIndex                         m_ActiveIndexBeingDragged;
    QModelIndex                                   m_TopLevelItemPlaceholder;

    QAction* m_ActionRenameBookmark = new QAction("Rename Pipeline", this);
    QAction* m_ActionRemoveBookmark = new QAction("Remove Bookmark", this);
    QAction* m_ActionShowBookmarkInFileSystem = new QAction(this);
    QAction* m_ActionClearBookmarks = new QAction("Clear Bookmarks", this);
    QAction* m_ActionAddBookmark = new QAction("Add Bookmark", this);
    QAction* m_ActionAddBookmarkFolder = new QAction("Add Bookmark Folder", this);
    QAction* m_ActionLocateFile = new QAction("Locate File", this);
    QAction* m_ActionOpenBookmark = new QAction("Open Bookmark", this);
    QAction* m_ActionExecuteBookmark = new QAction("Execute Bookmark", this);

    void performDrag();
    void expandChildren(const QModelIndex& parent, BookmarksModel* model);
};

#endif /* _BookmarksTreeView_H_ */
