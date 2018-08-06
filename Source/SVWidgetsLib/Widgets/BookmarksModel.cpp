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

#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"

#include <QtCore/QDir>
#include <QtCore/QDirIterator>

#include "SVWidgetsLib/Widgets/BookmarksModel.h"
#include "SVWidgetsLib/Widgets/BookmarksTreeView.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"
#include "SVWidgetsLib/QtSupport/QtSSettings.h"

#define PREBUILT_PIPELINES_DIR "PrebuiltPipelines"

BookmarksModel* BookmarksModel::self = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksModel::BookmarksModel(QObject* parent)
: QAbstractItemModel(parent)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksModel::~BookmarksModel()
{
  delete rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksModel* BookmarksModel::Instance()
{
  if(self == nullptr)
  {
    self = new BookmarksModel();
    self->loadModel();
    self->writeBookmarksToPrefsFile();  // Write out any "sanity check" changes that were made while loading the model
  }

  return self;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::initialize()
{
  rootItem = new BookmarksItem("");

  m_Watcher = new QFileSystemWatcher(this);

  connect(this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(updateModel(const QModelIndex&, const QModelIndex&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::setFileSystemWatcher(QFileSystemWatcher* watcher)
{
  m_Watcher = watcher;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFileSystemWatcher* BookmarksModel::getFileSystemWatcher()
{
  return m_Watcher;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::updateRowState(const QString& path)
{
  QFileInfo fi(path);

  QModelIndexList indexList = findIndexByPath(path);
  for(int i = 0; i < indexList.size(); i++)
  {
    QModelIndex nameIndex = index(indexList[i].row(), BookmarksItem::Contents, indexList[i].parent());

    // Set the itemHasError variable
    setData(nameIndex, !fi.exists(), static_cast<int>(Roles::ErrorsRole));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksModel::columnCount(const QModelIndex& parent) const
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant BookmarksModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
  {
    return QVariant();
  }

  SVStyle* styles = SVStyle::Instance();

  BookmarksItem* item = getItem(index);

  if(role == Qt::DisplayRole)
  {
    return item->getName();
  }
  else if(role == static_cast<int>(Roles::PathRole))
  {
    return item->getPath();
  }
  else if(role == static_cast<int>(Roles::ErrorsRole))
  {
    return item->getHasErrors();
  }
  else if(role == static_cast<int>(Roles::ExpandedRole))
  {
    return item->isExpanded();
  }

  else if(role == static_cast<int>(Roles::ItemTypeRole))
  {
    return static_cast<int>(item->getItemType());
  }
  else if(role == Qt::ForegroundRole)
  {
    if(item->getHasErrors() == true)
    {
      return styles->getQTreeViewItem_error_color();
    }
    else
    {
      return styles->getQTreeViewItem_color();
    }
  }
  else if(role == Qt::ToolTipRole && item->getItemType() == BookmarksItem::ItemType::Bookmark)
  {
    QString path = item->getPath();
    QFileInfo info(path);
    if(path.isEmpty() == false)
    {
      if(info.exists() == false)
      {
        QString tooltip = tr("'%1' was not found on the file system.\nYou can either locate the file or delete the entry from the table.").arg(item->getPath());
        return tooltip;
      }
      else if(info.suffix().compare("json") == 0)
      {
        QString html = JsonFilterParametersReader::HtmlSummaryFromFile(path, nullptr);
        return html;
      }
    }

    return QVariant();
  }
  else if(role == Qt::DecorationRole)
  {
    return item->getIcon();
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags BookmarksModel::flags(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return 0;
  }

  Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

  BookmarksItem* item = getItem(index);
  QString name = item->getName();
  if(item->getItemType() == BookmarksItem::ItemType::Folder)
  {
    // This is a node
    return (defaultFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
  }
  else
  {
    // This is a leaf
    return (defaultFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem* BookmarksModel::getItem(const QModelIndex& index) const
{
  if(index.isValid())
  {
    BookmarksItem* item = static_cast<BookmarksItem*>(index.internalPointer());
    if(item)
    {
      return item;
    }
  }
  return rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex BookmarksModel::index(int row, int column, const QModelIndex& parent) const
{
  if(parent.isValid() && parent.column() != 0)
  {
    return QModelIndex();
  }

  BookmarksItem* parentItem = getItem(parent);

  BookmarksItem* childItem = parentItem->child(row);
  if(childItem)
  {
    return createIndex(row, column, childItem);
  }
  else
  {
    return QModelIndex();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::insertRows(int position, int rows, const QModelIndex& parent)
{
  BookmarksItem* parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, columnCount());
  endInsertRows();

  if (m_LoadingModel == false)
  {
    writeBookmarksToPrefsFile();
  }

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::removeRows(int position, int rows, const QModelIndex& parent)
{
  BookmarksItem* parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  if (m_LoadingModel == false)
  {
    writeBookmarksToPrefsFile();
  }

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
  beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);

  BookmarksItem* srcParentItem = getItem(sourceParent);
  BookmarksItem* destParentItem = getItem(destinationParent);

  for(int i = sourceRow; i < sourceRow + count; i++)
  {
    QModelIndex srcIndex = index(i, BookmarksItem::Contents, sourceParent);
    BookmarksItem* srcItem = getItem(srcIndex);

    destParentItem->insertChild(destinationChild, srcItem);
    srcItem->setParent(destParentItem);
    srcParentItem->removeChild(i);
  }

  endMoveRows();

  if (m_LoadingModel == false)
  {
    writeBookmarksToPrefsFile();
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex BookmarksModel::parent(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return QModelIndex();
  }

  BookmarksItem* childItem = getItem(index);
  BookmarksItem* parentItem = childItem->parent();

  if(parentItem == rootItem)
  {
    return QModelIndex();
  }

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksModel::rowCount(const QModelIndex& parent) const
{
  BookmarksItem* parentItem = getItem(parent);

  return parentItem->childCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  BookmarksItem* item = getItem(index);

  if(role == Qt::DisplayRole)
  {
    item->setName(value.toString());
  }
  else if(role == static_cast<int>(Roles::PathRole))
  {
    item->setPath(value.toString());
  }
  else if(role == static_cast<int>(Roles::ExpandedRole))
  {
    item->setExpanded(value.toBool());
  }
  else if(role == static_cast<int>(Roles::ErrorsRole))
  {
    item->setHasErrors(value.toBool());
  }
  else if(role == static_cast<int>(Roles::ItemTypeRole))
  {
    item->setItemType(static_cast<BookmarksItem::ItemType>(value.toInt()));
  }
  else if(role == Qt::DecorationRole)
  {
    item->setIcon(value.value<QIcon>());
  }
  else if(role == Qt::ToolTipRole)
  {
    item->setItemTooltip(value.toString());
  }
  else
  {
    return false;
  }

  emit dataChanged(index, index);

  if (m_LoadingModel == false)
  {
    writeBookmarksToPrefsFile();
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem* BookmarksModel::getRootItem()
{
  return rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::isEmpty()
{
  if(rowCount(QModelIndex()) <= 0)
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::addFileToTree(QString& path, QModelIndex& specifiedParent)
{
  path = QDir::toNativeSeparators(path);

  QFileInfo fi(path);

  int rowPos = rowCount(specifiedParent);
  insertRow(rowPos, specifiedParent);
  QModelIndex index = this->index(rowPos, BookmarksItem::Contents, specifiedParent);

  if(fi.isFile())
  {
    QString name = fi.baseName();
    setData(index, name, Qt::DisplayRole);
    setData(index, path, Roles::PathRole);
    setData(index, static_cast<int>(BookmarksItem::ItemType::Bookmark), Roles::ItemTypeRole);
    setData(index, QIcon(":/SIMPL/icons/images/bookmark.png"), Qt::DecorationRole);
  }
  else
  {
    QDir dir(path);
    setData(index, dir.dirName(), Qt::DisplayRole);
    setData(index, static_cast<int>(BookmarksItem::ItemType::Folder), Roles::ItemTypeRole);
    setData(index, QIcon(":/SIMPL/icons/images/folder_blue.png"), Qt::DecorationRole);

    QStringList filters;
    filters << "*.dream3d"
            << "*.ini"
            << "*.txt"
            << "*.json";
    QDirIterator iter(path, filters, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    while(iter.hasNext())
    {
      QString nextPath = iter.next();
      addFileToTree(nextPath, index);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::fromJsonObject(QJsonObject treeObject)
{
  QStringList keys = treeObject.keys();
  keys.sort(Qt::CaseInsensitive);
  for(int i = 0; i < keys.size(); i++)
  {
    if(keys[i].compare("Prebuilt Pipelines") == 0)
    {
      continue;
    }
    QJsonValue val = treeObject.value(keys[i]);
    if(val.isObject())
    {
      unwrapModel(keys[i], val.toObject(), QModelIndex());
    }
  }

  QStringList paths = getFilePaths();
  if(!paths.isEmpty())
  {
    getFileSystemWatcher()->addPaths(paths);
  }
  connect(getFileSystemWatcher(), SIGNAL(fileChanged(const QString&)), SLOT(updateRowState(const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject BookmarksModel::toJsonObject()
{
  BookmarksItem* rootItem = getRootItem();

  QJsonObject treeObj;
  for(int i = 0; i < rootItem->childCount(); i++)
  {
    QModelIndex childIndex = index(i, BookmarksItem::Contents, QModelIndex());
    BookmarksItem* childItem = getItem(childIndex);

    QString name = childItem->getName();

    QJsonObject childObj = wrapModel(childIndex);
    treeObj.insert(name, childObj);
  }

  return treeObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject BookmarksModel::wrapModel(QModelIndex currentIndex)
{
  QJsonObject obj;

  for(int i = 0; i < rowCount(currentIndex); i++)
  {
    QModelIndex childIndex = this->index(i, BookmarksItem::Contents, currentIndex);
    BookmarksItem* childItem = getItem(childIndex);

    QString childName = childItem->getName();

    QJsonObject childObj = wrapModel(childIndex);
    obj[childName] = childObj;
  }

  BookmarksItem* item = getItem(currentIndex);

  QString path = item->getPath();
  obj.insert("Path", path);

  BookmarksItem::ItemType type = item->getItemType();
  obj.insert("Item Type", static_cast<int>(type));

  bool expanded = item->isExpanded();
  obj.insert("Expanded", expanded);

  return obj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::unwrapModel(QString objectName, QJsonObject object, QModelIndex parentIndex)
{
  int row = rowCount(parentIndex);
  insertRow(row, parentIndex);
  QModelIndex index = this->index(row, BookmarksItem::Contents, parentIndex);

  QString path = object["Path"].toString();
  bool expanded = object["Expanded"].toBool();
  BookmarksItem::ItemType itemType = static_cast<BookmarksItem::ItemType>(object["Item Type"].toInt());

  QFileInfo fi(path);

  // Sanity check to make sure that all bookmark items that are read in have the proper type
  if (fi.isFile() && fi.completeSuffix() == "json" && itemType == BookmarksItem::ItemType::Folder)
  {
    itemType = BookmarksItem::ItemType::Bookmark;
  }
  else if (path.isEmpty() && itemType == BookmarksItem::ItemType::Bookmark)
  {
    itemType = BookmarksItem::ItemType::Folder;
  }

  if(itemType == BookmarksItem::ItemType::Bookmark)
  {
    setData(index, QIcon(":/SIMPL/icons/images/bookmark.png"), Qt::DecorationRole);
    if(fi.exists() == false)
    {
      // Set the itemHasError variable
      setData(index, true, static_cast<int>(Roles::ErrorsRole));
    }
  }
  else
  {
    setData(index, QIcon(":/SIMPL/icons/images/folder_blue.png"), Qt::DecorationRole);
  }

  path = QDir::toNativeSeparators(path);

  setData(index, objectName, Qt::DisplayRole);
  setData(index, path, static_cast<int>(Roles::PathRole));
  setData(index, expanded, static_cast<int>(Roles::ExpandedRole));
  setData(index, static_cast<int>(itemType), static_cast<int>(Roles::ItemTypeRole));

  QStringList keys = object.keys();
  keys.sort(Qt::CaseInsensitive);
  for(int i = 0; i < keys.size(); i++)
  {
    QJsonValue val = object.value(keys[i]);
    if(val.isObject())
    {
      unwrapModel(keys[i], val.toObject(), index);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList BookmarksModel::getFilePaths()
{
  return getFilePaths(rootItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList BookmarksModel::getFilePaths(BookmarksItem* item)
{
  QStringList list;
  if(item != rootItem && item->childCount() <= 0)
  {
    QString filePath = item->getPath();
    if(filePath.isEmpty() == false)
    {
      list.append(filePath);
    }
    return list;
  }

  for(int i = 0; i < item->childCount(); i++)
  {
    list.append(getFilePaths(item->child(i)));
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList BookmarksModel::findIndexByPath(QString filePath)
{
  QModelIndexList list;
  for(int i = 0; i < rootItem->childCount(); i++)
  {
    QModelIndex child = index(i, BookmarksItem::Contents);
    if(rowCount(child) <= 0 && data(child, static_cast<int>(Roles::PathRole)).toString() == filePath)
    {
      list.append(child);
    }

    for(int j = 0; j < rowCount(child); j++)
    {
      QModelIndexList subList = findIndexByPath(child, filePath);
      list.append(subList);
    }
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList BookmarksModel::findIndexByPath(const QModelIndex& current, QString filePath)
{
  QModelIndex actual = index(current.row(), BookmarksItem::Contents, current.parent());

  QModelIndexList list;
  for(int i = 0; i < rowCount(actual); i++)
  {
    QModelIndex index = this->index(i, BookmarksItem::Contents, actual);

    if(rowCount(index) <= 0 && data(index, static_cast<int>(Roles::PathRole)).toString() == filePath)
    {
      list.append(index);
    }

    QModelIndexList subList = findIndexByPath(index, filePath);
    list.append(subList);
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::updateModel(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  if(topLeft.isValid())
  {
    QModelIndex index = this->index(topLeft.row(), BookmarksItem::Contents, topLeft.parent());
    QString path = data(index, static_cast<int>(Roles::PathRole)).toString();
    QFileInfo fi(path);
    if(nullptr != m_Watcher && path.isEmpty() == false && fi.exists())
    {
      m_Watcher->addPath(path);
    }
  }
  else if(bottomRight.isValid())
  {
    QModelIndex index = this->index(bottomRight.row(), BookmarksItem::Contents, bottomRight.parent());
    QString path = data(index, static_cast<int>(Roles::PathRole)).toString();
    QFileInfo fi(path);
    if(nullptr != m_Watcher && path.isEmpty() == false && fi.exists())
    {
      m_Watcher->addPath(path);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::readBookmarksFromPrefsFile()
{
  QJsonObject object = getBookmarksPrefsObject();
  fromJsonObject(object);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject BookmarksModel::getBookmarksPrefsObject()
{
  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings(getBookmarksPrefsPath()));

  QJsonObject object;
  if(prefs->contains("Bookmarks"))
  {
    prefs->beginGroup("Bookmarks");

    object = prefs->value("Bookmarks Model", QJsonObject());

    prefs->endGroup();
  }
  else
  {
    // If no bookmarks were found in the new location, check the old location
    prefs->beginGroup("DockWidgetSettings");
    prefs->beginGroup("Bookmarks Dock Widget");

    object = prefs->value("Bookmarks Model", QJsonObject());

    prefs->endGroup();
    prefs->endGroup();
  }

  return object;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::writeBookmarksToPrefsFile()
{
  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings(getBookmarksPrefsPath()));

  prefs->beginGroup("Bookmarks");

  QJsonObject modelObj = toJsonObject();
  prefs->setValue("Bookmarks Model", modelObj);

  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::loadModel()
{
  m_LoadingModel = true;
  readPrebuiltPipelines();
  readBookmarksFromPrefsFile();
  m_LoadingModel = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::readPrebuiltPipelines()
{
  QDir pipelinesDir = findPipelinesDirectory();
  QString pPath = pipelinesDir.absolutePath();

  FilterLibraryTreeWidget::ItemType itemType = FilterLibraryTreeWidget::Leaf_Item_Type;
  QString iconFileName(":/SIMPL/icons/images/bookmark.png");
  bool allowEditing = false;
  QStringList fileExtension;
  fileExtension.append("*.json");

  // Add top-level folder and then load up all the pipelines into the folder
  QString dirName = "Prebuilt Pipelines";
  QJsonObject prefsObject = getBookmarksPrefsObject();
  QJsonObject prebuiltsObject = prefsObject["Prebuilt Pipelines"].toObject();
  bool expanded = prebuiltsObject["Expanded"].toBool();
  QModelIndex index = addTreeItem(QModelIndex(), dirName, QIcon(":/SIMPL/icons/images/folder_blue.png"), pPath, 0, BookmarksItem::ItemType::Folder, expanded);

  addPipelinesRecursively(pipelinesDir, index, prebuiltsObject, iconFileName, allowEditing, fileExtension, itemType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDir BookmarksModel::findPipelinesDirectory()
{
  QString dirName(PREBUILT_PIPELINES_DIR);

  QString appPath = QCoreApplication::applicationDirPath();
  QDir pipelinesDir = QDir(appPath);
#if defined(Q_OS_WIN)
  QFileInfo fi(pipelinesDir.absolutePath() + QDir::separator() + dirName);
  if(fi.exists() == false)
  {
    // The PrebuiltPipelines file does not exist at the default location because we are probably running from visual studio.
    // Try up one more directory
    pipelinesDir.cdUp();
  }
#elif defined(Q_OS_MAC)
  if(pipelinesDir.dirName() == "MacOS")
  {
    pipelinesDir.cdUp();
    // Can we change directory into the "PrebuiltPipeliines" directory at this level.
    QString pbpDir = QString("Resources/%1").arg(PREBUILT_PIPELINES_DIR);
    if (pipelinesDir.cd(pbpDir))
    {
      return pipelinesDir;
    }
    pipelinesDir.cdUp();
    pipelinesDir.cdUp();
    if (pipelinesDir.cd(PREBUILT_PIPELINES_DIR) )
    {
      return pipelinesDir;
    }
  }
#else
  // We are on Linux - I think
  QFileInfo fi(pipelinesDir.absolutePath() + QDir::separator() + dirName);
  // qDebug() << fi.absolutePath();
  // Look for the PREBUILT_PIPELINES_DIR directory in the current app directory
  if(fi.exists() == false)
  {
    // Try up one more directory
    pipelinesDir.cdUp();
  }

#endif

  pipelinesDir = pipelinesDir.absolutePath() + QDir::separator() + dirName;
  return pipelinesDir;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::addPipelinesRecursively(QDir currentDir, QModelIndex parent, QJsonObject prebuiltsObj, QString iconFileName, bool allowEditing, QStringList filters, FilterLibraryTreeWidget::ItemType itemType)
{
  QModelIndex nextIndex;

  // Get a list of all the directories
  QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  if(dirList.size() > 0)
  {
    foreach(QFileInfo fi, dirList)
    {
      // At this point we have the first level of directories and we want to do 2 things:
      // 1.Create an entry in the tree widget with this name
      // 2.drop into the directory and look for all the .json files and add entries for those items.
      // qDebug() << fi.absoluteFilePath() << "\n";
      // Add a tree widget item for this  Group
      // qDebug() << fi.absoluteFilePath();
      int row = rowCount(parent);
      QString baseName = fi.baseName();
      QJsonObject folderPrefsObj = prebuiltsObj[baseName].toObject();
      bool expanded = folderPrefsObj["Expanded"].toBool();
      nextIndex = addTreeItem(parent, baseName, QIcon(":/SIMPL/icons/images/folder_blue.png"), fi.absoluteFilePath(), row, BookmarksItem::ItemType::Folder, expanded);

      addPipelinesRecursively(QDir(fi.absoluteFilePath()), nextIndex, folderPrefsObj, iconFileName, allowEditing, filters, itemType); // Recursive call
    }
  }

  QFileInfoList itemList = currentDir.entryInfoList(filters);
  foreach(QFileInfo itemInfo, itemList)
  {
    QString itemFilePath = itemInfo.absoluteFilePath();
    QString itemName;
    if(itemInfo.suffix().compare("json") == 0)
    {
      QString dVers;
      JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
      jsonReader->readNameOfPipelineFromFile(itemFilePath, itemName, dVers, nullptr);
    }
#if 0
    else if(itemInfo.suffix().compare("ini") == 0 || itemInfo.suffix().compare("txt") == 0)
    {
      QSettings itemPref(itemFilePath, QSettings::IniFormat);
      itemPref.beginGroup(SIMPL::Settings::PipelineBuilderGroup);
      itemName = itemPref.value(SIMPL::Settings::PipelineName).toString();
      itemPref.endGroup();
    }
#endif
    // Add tree widget for this Prebuilt Pipeline
    int row = rowCount(parent);
    addTreeItem(parent, itemName, QIcon(iconFileName), itemInfo.absoluteFilePath(), row, BookmarksItem::ItemType::Bookmark, false);
    nextIndex = index(row, BookmarksItem::Contents, parent);

//    QString htmlFormattedString = generateHtmlFilterListFromPipelineFile(itemInfo.absoluteFilePath());
//    model->setData(nextIndex, htmlFormattedString, Qt::ToolTipRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex BookmarksModel::addTreeItem(QModelIndex parent, QString& favoriteTitle, QIcon icon, QString favoritePath, int insertIndex, BookmarksItem::ItemType type, bool isExpanded)
{
  favoritePath = QDir::toNativeSeparators(favoritePath);

  QFileInfo fileInfo(favoritePath);
  QString ext = fileInfo.completeSuffix();
  if(fileInfo.isFile() && ext != "dream3d" && ext != "json" && ext != "ini" && ext != "txt")
  {
    return QModelIndex();
  }

  // Add a new Item to the Tree
  insertRow(insertIndex, parent);
  QModelIndex index = this->index(insertIndex, BookmarksItem::Contents, parent);
  setData(index, favoriteTitle, Qt::DisplayRole);
  setData(index, favoritePath, static_cast<int>(Roles::PathRole));
  setData(index, icon, Qt::DecorationRole);
  setData(index, static_cast<int>(type), static_cast<int>(Roles::ItemTypeRole));

  sort(BookmarksItem::Contents, Qt::AscendingOrder);

  if(isExpanded)
  {
    setData(index, true, static_cast<int>(Roles::ExpandedRole));
  }
  else
  {
    setData(index, false, static_cast<int>(Roles::ExpandedRole));
  }

  if(type == BookmarksItem::ItemType::Bookmark)
  {
    m_Watcher->addPath(favoritePath);
  }

  return index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BookmarksModel::getBookmarksPrefsPath()
{
  QtSSettings prefs;
  QFileInfo fi(prefs.fileName());
  QString parentPath = fi.absolutePath();

  QString appName = QCoreApplication::applicationName();
  if(appName.isEmpty())
  {
    appName = QString("Application");
  }

#if defined(Q_OS_MAC)

  QString domain = QCoreApplication::organizationDomain();
  if(domain.isEmpty())
  {
    domain = QString("Domain");
  }
  QStringList tokens = domain.split(".");
  QStringListIterator iter(tokens);
  iter.toBack();
  domain = QString("");
  while(iter.hasPrevious())
  {
    domain = domain + iter.previous() + QString(".");
  }

  QString bookmarksPrefsPath = parentPath + "/" + domain + appName + "_BookmarksModel.json";
#else
  QString bookmarksPrefsPath = QDir::toNativeSeparators(parentPath + "/" + appName + "_BookmarksModel.json");
#endif

  return bookmarksPrefsPath;
}
