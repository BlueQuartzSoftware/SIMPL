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

#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
#include <QtCore/QDir>
#include <QtCore/QFileSystemWatcher>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/FilterLibraryTreeWidget.h"
#include "SVWidgetsLib/Widgets/BookmarksItem.h"

enum IndexState
{
  Error,
  Default
};

class QtSSettings;
class BookmarksItem;

class SVWidgetsLib_EXPORT BookmarksModel : public QAbstractItemModel
{
    Q_OBJECT

  public:
    SIMPL_TYPE_MACRO(BookmarksModel)

    enum ErrorCodes
    {
      UNRECOGNIZED_EXT = -1
    };

    enum Roles
    {
      PathRole = Qt::UserRole + 1,
      ExpandedRole,
      ErrorsRole,
      ItemTypeRole
    };

    ~BookmarksModel();

    /**
     * @brief Instance
     * @return
     */
    static BookmarksModel* Instance();

    /**
    * @brief toJsonObject
    */
    QJsonObject toJsonObject();

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;

    bool isEmpty();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

    bool moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationChild) Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

    bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;

    BookmarksItem* getRootItem();

    void addFileToTree(QString& path, QModelIndex& specifiedParent);

    QStringList getFilePaths();

    QModelIndexList findIndexByPath(QString filePath);

    void setFileSystemWatcher(QFileSystemWatcher* watcher);
    QFileSystemWatcher* getFileSystemWatcher();

    /**
     * @brief readPrebuiltPipelines
     */
    void readPrebuiltPipelines();

    /**
     * @brief readBookmarksFromPrefsFile
     */
    void readBookmarksFromPrefsFile();

    /**
     * @brief writeBookmarksToPrefsFile
     */
    void writeBookmarksToPrefsFile();

  public slots:
    /**
     * @brief addTreeItem
     * @param parent
     * @param favoriteTitle
     * @param icon
     * @param favoritePath
     * @param insertIndex
     * @param type
     * @param isExpanded
     * @return
     */
    QModelIndex addTreeItem(QModelIndex parent,
                    QString& favoriteTitle,
                    QIcon icon,
                    QString favoritePath,
                    int insertIndex, BookmarksItem::ItemType type,
                    bool isExpanded);

  protected:
    BookmarksModel(QObject* parent = nullptr);

    void initialize();

    QDir findPipelinesDirectory();

    void addPipelinesRecursively(QDir currentDir, QModelIndex parent, QJsonObject prebuiltsObj, QString iconFileName,
                                 bool allowEditing, QStringList filters, FilterLibraryTreeWidget::ItemType itemType);

  protected slots:
    void updateRowState(const QString& path);
    void updateModel(const QModelIndex& topLeft, const QModelIndex& bottomRight);

  private:
    static BookmarksModel*    self;
    bool m_LoadingModel = false;

    BookmarksItem*            rootItem;
    QFileSystemWatcher*       m_Watcher;

    BookmarksItem* getItem(const QModelIndex& index) const;

    QStringList getFilePaths(BookmarksItem* item);

    QModelIndexList findIndexByPath(const QModelIndex& index, QString filePath);

    /**
    * @brief fromJsonObject
    * @param modelObject
    */
    void fromJsonObject(QJsonObject modelObject);

    QJsonObject wrapModel(QModelIndex index);
    void unwrapModel(QString objectName, QJsonObject object, QModelIndex parentIndex);

    /**
    * @brief getBookmarksPrefsPath
    */
    QString getBookmarksPrefsPath();

    /**
     * @brief getBookmarksPrefsObject
     * @return
     */
    QJsonObject getBookmarksPrefsObject();

    /**
     * @brief loadModel
     */
    void loadModel();

    BookmarksModel(const BookmarksModel&);    // Copy Constructor Not Implemented
    void operator=(const BookmarksModel&);    // Move assignment Not Implemented
};

