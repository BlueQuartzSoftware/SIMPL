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
    self->readPrebuiltPipelines();
    self->readBookmarksFromPrefsFile();
  }

  return self;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::initialize()
{
  QVector<QVariant> vector;
  vector.push_back("Name");
  vector.push_back("Path");
  rootItem = new BookmarksItem(vector);

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
    QModelIndex nameIndex = index(indexList[i].row(), BookmarksItem::Name, indexList[i].parent());

    // Set the itemHasError variable
    setData(nameIndex, !fi.exists(), Qt::UserRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksModel::columnCount(const QModelIndex& parent) const
{
  return rootItem->columnCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex BookmarksModel::sibling(int row, int column, const QModelIndex& currentIndex) const
{
  if(currentIndex.column() == BookmarksItem::Name)
  {
    return index(currentIndex.row(), BookmarksItem::Path, currentIndex.parent());
  }
  else
  {
    return index(currentIndex.row(), BookmarksItem::Name, currentIndex.parent());
  }
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

  BookmarksItem* item = getItem(index);

  if(role == Qt::DisplayRole)
  {
    return item->data(index.column());
  }
  else if(role == Qt::UserRole)
  {
    return item->getItemHasErrors();
  }
  else if(role == Qt::BackgroundRole)
  {
    if(item->getItemHasErrors() == true)
    {
      return QColor(235, 110, 110);
    }
    else
    {
      return QVariant();
    }
  }
  else if(role == Qt::ForegroundRole)
  {
    if(item->getItemHasErrors() == true)
    {
      return QColor(240, 240, 240);
    }
    else
    {
      return QColor(Qt::black);
    }
  }
  else if(role == Qt::ToolTipRole)
  {
    QString path = item->data(1).toString();
    QFileInfo info(path);
    if(info.exists() == false)
    {
      QString tooltip = "'" + this->index(index.row(), BookmarksItem::Path, index.parent()).data().toString() +
                        "' was not found on the file system.\nYou can either locate the file or delete the entry from the table.";
      return tooltip;
    }
    else
    {
      if(info.suffix().compare("json") == 0)
      {
        QString html = JsonFilterParametersReader::HtmlSummaryFromFile(path, nullptr);
        return html;
      }
      else
      {
        return path;
      }
    }
  }
  else if(role == Qt::DecorationRole)
  {
    QModelIndex nameIndex = this->index(index.row(), BookmarksItem::Name, index.parent());
    if(nameIndex == index)
    {
      BookmarksItem* item = getItem(index);
      return item->getIcon();
    }
    else
    {
      return QVariant();
    }
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
  QString name = item->data(BookmarksItem::Name).toString();
  if(item->data(BookmarksItem::Path).toString().isEmpty())
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
QVariant BookmarksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    return rootItem->data(section);
  }

  return QVariant();
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
  success = parentItem->insertChildren(position, rows, rootItem->columnCount());
  endInsertRows();

  writeBookmarksToPrefsFile();

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

  writeBookmarksToPrefsFile();

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
    QModelIndex srcIndex = index(i, BookmarksItem::Name, sourceParent);
    BookmarksItem* srcItem = getItem(srcIndex);

    destParentItem->insertChild(destinationChild, srcItem);
    srcItem->setParent(destParentItem);
    srcParentItem->removeChild(i);
  }

  endMoveRows();

  writeBookmarksToPrefsFile();

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
  bool result = false;

  if(role == Qt::UserRole)
  {
    result = item->setItemHasErrors(value.value<bool>());
  }
  else if(role == Qt::DecorationRole)
  {
    result = item->setIcon(value.value<QIcon>());
  }
  else if(role == Qt::ToolTipRole)
  {
    result = item->setItemTooltip(value.toString());
  }
  else
  {
    result = item->setData(index.column(), value);
  }

  if(result)
  {
    emit dataChanged(index, index);

    writeBookmarksToPrefsFile();
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::setExpanded(const QModelIndex& index, bool value)
{
  BookmarksItem* item = getItem(index);
  item->setExpanded(value);

  writeBookmarksToPrefsFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::isExpanded(const QModelIndex& index)
{
  BookmarksItem* item = getItem(index);
  return item->isExpanded();
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
  QModelIndex newNameIndex = index(rowPos, BookmarksItem::Name, specifiedParent);

  if(fi.isFile())
  {
    QString name = fi.baseName();
    setData(newNameIndex, name, Qt::DisplayRole);
  }
  else
  {
    QDir dir(path);
    setData(newNameIndex, dir.dirName(), Qt::DisplayRole);
  }

  if(fi.isFile())
  {
    QModelIndex newPathIndex = index(rowPos, BookmarksItem::Path, specifiedParent);
    setData(newPathIndex, path, Qt::DisplayRole);
    setData(newNameIndex, QIcon(":/bookmark.png"), Qt::DecorationRole);
  }
  else
  {
    setData(newNameIndex, QIcon(":/folder_blue.png"), Qt::DecorationRole);

    QStringList filters;
    filters << "*.dream3d"
            << "*.ini"
            << "*.txt"
            << "*.json";
    QDirIterator iter(path, filters, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    while(iter.hasNext())
    {
      QString nextPath = iter.next();
      addFileToTree(nextPath, newNameIndex);
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
    QModelIndex childIndex = index(i, BookmarksItem::Name, QModelIndex());
    QString name = childIndex.data().toString();

    if(name.compare("Prebuilt Pipelines") != 0)
    {
      QJsonObject childObj = wrapModel(childIndex);
      treeObj[name] = childObj;
    }
  }

  return treeObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject BookmarksModel::wrapModel(QModelIndex currentIndex)
{
  QJsonObject obj;

  QString name = index(currentIndex.row(), BookmarksItem::Name, currentIndex.parent()).data().toString();
  QString path = index(currentIndex.row(), BookmarksItem::Path, currentIndex.parent()).data().toString();

  for(int i = 0; i < rowCount(currentIndex); i++)
  {
    QModelIndex childIndex = index(i, BookmarksItem::Name, currentIndex);
    QString childName = childIndex.data().toString();

    QJsonObject childObj = wrapModel(childIndex);
    obj[childName] = childObj;
  }

  if(flags(currentIndex).testFlag(Qt::ItemIsDropEnabled) == true)
  {
    obj.insert("Expanded", isExpanded(currentIndex));
  }
  else
  {
    obj.insert("Path", path);
  }

  return obj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::unwrapModel(QString objectName, QJsonObject object, QModelIndex parentIndex)
{
  int row = rowCount(parentIndex);
  insertRow(row, parentIndex);
  QModelIndex nameIndex = index(row, BookmarksItem::Name, parentIndex);
  QModelIndex pathIndex = index(row, BookmarksItem::Path, parentIndex);

  QString path = object["Path"].toString();
  bool expanded = object["Expanded"].toBool();

  QFileInfo fi(path);
  if(path.isEmpty() == false)
  {
    setData(nameIndex, QIcon(":/bookmark.png"), Qt::DecorationRole);
    if(fi.exists() == false)
    {
      // Set the itemHasError variable
      setData(nameIndex, true, Qt::UserRole);
    }
  }
  else
  {
    setData(nameIndex, QIcon(":/folder_blue.png"), Qt::DecorationRole);
  }

  path = QDir::toNativeSeparators(path);

  setData(nameIndex, objectName, Qt::DisplayRole);
  setData(pathIndex, path, Qt::DisplayRole);
  setExpanded(nameIndex, expanded);
  setExpanded(pathIndex, expanded);

  QStringList keys = object.keys();
  keys.sort(Qt::CaseInsensitive);
  for(int i = 0; i < keys.size(); i++)
  {
    QJsonValue val = object.value(keys[i]);
    if(val.isObject())
    {
      unwrapModel(keys[i], val.toObject(), nameIndex);
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
    QString filePath = item->data(BookmarksItem::Path).toString();
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
    QModelIndex child = index(i, BookmarksItem::Path, QModelIndex());
    if(rowCount(child) <= 0 && child.data().toString() == filePath)
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
  QModelIndex actual = index(current.row(), BookmarksItem::Name, current.parent());

  QModelIndexList list;
  for(int i = 0; i < rowCount(actual); i++)
  {
    QModelIndex pathIndex = index(i, BookmarksItem::Path, actual);

    if(rowCount(pathIndex) <= 0 && pathIndex.data().toString() == filePath)
    {
      list.append(pathIndex);
    }

    QModelIndexList subList = findIndexByPath(pathIndex, filePath);
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
    QString path = index(topLeft.row(), BookmarksItem::Path, topLeft.parent()).data().toString();
    QFileInfo fi(path);
    if(nullptr != m_Watcher && path.isEmpty() == false && fi.exists())
    {
      m_Watcher->addPath(path);
    }
  }
  else if(bottomRight.isValid())
  {
    QString path = index(bottomRight.row(), BookmarksItem::Path, bottomRight.parent()).data().toString();
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
  QSharedPointer<QtSSettings> prefs = QSharedPointer<QtSSettings>(new QtSSettings(getBookmarksPrefsPath()));

  if(prefs->contains("Bookmarks"))
  {
    prefs->beginGroup("Bookmarks");

    QJsonObject modelObj = prefs->value("Bookmarks Model", QJsonObject());
    fromJsonObject(modelObj);

    prefs->endGroup();
  }
  else
  {
    // If no bookmarks were found in the new location, check the old location
    prefs->beginGroup("DockWidgetSettings");
    prefs->beginGroup("Bookmarks Dock Widget");

    QJsonObject modelObj = prefs->value("Bookmarks Model", QJsonObject());
    fromJsonObject(modelObj);

    prefs->endGroup();
    prefs->endGroup();
  }
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
void BookmarksModel::readPrebuiltPipelines()
{
  QDir pipelinesDir = findPipelinesDirectory();
  QString pPath = pipelinesDir.absolutePath();
  BookmarksModel* model = BookmarksModel::Instance();

  FilterLibraryTreeWidget::ItemType itemType = FilterLibraryTreeWidget::Leaf_Item_Type;
  QString iconFileName(":/bookmark.png");
  bool allowEditing = false;
  QStringList fileExtension;
  fileExtension.append("*.json");

  // Add top-level folder and then load up all the pipelines into the folder
  QString dirName = "Prebuilt Pipelines";
  addTreeItem(QModelIndex(), dirName, QIcon(":/folder_blue.png"), pPath, 0, false);
  QModelIndex index = model->index(0, BookmarksItem::Name, QModelIndex());
  addPipelinesRecursively(pipelinesDir, index, iconFileName, allowEditing, fileExtension, itemType);
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
void BookmarksModel::addPipelinesRecursively(QDir currentDir, QModelIndex parent, QString iconFileName, bool allowEditing, QStringList filters, FilterLibraryTreeWidget::ItemType itemType)
{
  BookmarksModel* model = BookmarksModel::Instance();

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
      int row = model->rowCount(parent);
      QString baseName = fi.baseName();
      addTreeItem(parent, baseName, QIcon(":/folder_blue.png"), fi.absoluteFilePath(), row, false);
      nextIndex = model->index(row, BookmarksItem::Name, parent);
      addPipelinesRecursively(QDir(fi.absoluteFilePath()), nextIndex, iconFileName, allowEditing, filters, itemType); // Recursive call
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
    int row = model->rowCount(parent);
    addTreeItem(parent, itemName, QIcon(iconFileName), itemInfo.absoluteFilePath(), row, false);
    nextIndex = model->index(row, BookmarksItem::Name, parent);

//    QString htmlFormattedString = generateHtmlFilterListFromPipelineFile(itemInfo.absoluteFilePath());
//    model->setData(nextIndex, htmlFormattedString, Qt::ToolTipRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksModel::addTreeItem(QModelIndex parent, QString& favoriteTitle, QIcon icon, QString favoritePath, int insertIndex, bool isExpanded)
{
  favoritePath = QDir::toNativeSeparators(favoritePath);

  QFileInfo fileInfo(favoritePath);
  QString ext = fileInfo.completeSuffix();
  if(fileInfo.isFile() && ext != "dream3d" && ext != "json" && ext != "ini" && ext != "txt")
  {
    return UNRECOGNIZED_EXT;
  }

  // Add a new Item to the Tree
  insertRow(insertIndex, parent);
  QModelIndex nameIndex = index(insertIndex, BookmarksItem::Name, parent);
  setData(nameIndex, favoriteTitle, Qt::DisplayRole);
  QModelIndex pathIndex = index(insertIndex, BookmarksItem::Path, parent);
  setData(pathIndex, favoritePath, Qt::DisplayRole);
  setData(nameIndex, icon, Qt::DecorationRole);

  sort(BookmarksItem::Name, Qt::AscendingOrder);

  if(favoritePath.isEmpty())
  {
    // This is a node
    if(isExpanded)
    {
      setExpanded(nameIndex, true);
      setExpanded(pathIndex, true);
    }
    else
    {
      setExpanded(nameIndex, false);
      setExpanded(pathIndex, false);
    }
  }
  else
  {
    getFileSystemWatcher()->addPath(favoritePath);
  }

  return 0;
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
