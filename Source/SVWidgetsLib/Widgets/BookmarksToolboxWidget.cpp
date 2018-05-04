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

#include "BookmarksToolboxWidget.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFileInfoList>
#include <QtCore/QUrl>

#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTreeWidgetItem>

#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"

#include "SVWidgetsLib/QtSupport/QtSBookmarkMissingDialog.h"

#include "SVWidgetsLib/Widgets/BookmarksModel.h"
#include "SVWidgetsLib/Widgets/FilterListToolboxWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksToolboxWidget::BookmarksToolboxWidget(QWidget* parent)
: QWidget(parent)
, m_OpenDialogLastFilePath("")
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksToolboxWidget::~BookmarksToolboxWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::connectFilterList(FilterListToolboxWidget* filterListWidget)
{
  connect(this, SIGNAL(filterListGenerated(const QStringList&, bool)), filterListWidget, SLOT(updateFilterList(const QStringList&, bool)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::setupGui()
{
  QString css(" QToolTip {\
              border: 2px solid #434343;\
              padding: 2px;\
              /* border-radius: 3px;*/\
              opacity: 255;\
              background-color: #FFFFFF;\
              color: #000000;\
              }");
  bookmarksTreeView->setStyleSheet(css);

  BookmarksModel* model = BookmarksModel::Instance();

  connect(bookmarksTreeView, SIGNAL(itemWasDropped(QModelIndex, QString&, QIcon, QString, int, bool)), model,
          SLOT(addTreeItem(QModelIndex, QString&, QIcon, QString, int, bool)));

  connect(bookmarksTreeView, &BookmarksTreeView::updateStatusBar, this, &BookmarksToolboxWidget::updateStatusBar);

  connect(bookmarksTreeView, &BookmarksTreeView::newSIMPLViewInstanceTriggered, this, &BookmarksToolboxWidget::bookmarkActivated);

  connect(bookmarksTreeView, &BookmarksTreeView::fireWriteSettings, this, &BookmarksToolboxWidget::fireWriteSettings);

  //bookmarksTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksTreeView* BookmarksToolboxWidget::getBookmarksTreeView()
{
  return bookmarksTreeView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDir BookmarksToolboxWidget::findV4FavoritesDirectory()
{
  // Get the location of the Preferences file. The Favorites are stored in a directory located at that level.
  QString dirName("SIMPLView_Favorites");

  QtSSettings prefs;
  QString prefFile = prefs.fileName();

  QFileInfo prefFileInfo = QFileInfo(prefFile);
  QString parentPath = prefFileInfo.path();
  QString favPath = parentPath + QDir::separator() + dirName;
  QDir pipelinesDir = QDir(favPath);
  return pipelinesDir;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BookmarksToolboxWidget::generateHtmlFilterListFromPipelineFile(QString path)
{
  QString html;

  QFileInfo fi(path);
  if(fi.suffix().compare("json") == 0)
  {
    html = JsonFilterParametersReader::HtmlSummaryFromFile(path, nullptr);
  }

  return html;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList BookmarksToolboxWidget::generateFilterListFromPipelineFile(QString path)
{

  QStringList filterNames;
  QSettings prefs(path, QSettings::IniFormat);

  prefs.beginGroup(SIMPL::Settings::PipelineBuilderGroup);
  bool ok = false;
  int filterCount = prefs.value("Number_Filters").toInt(&ok);
  prefs.endGroup();
  if(false == ok)
  {
    filterCount = 0;
  }
  for(int i = 0; i < filterCount; ++i)
  {
    QString gName = QString::number(i);
    prefs.beginGroup(gName);
    QString filterName = prefs.value("Filter_Name", "").toString();
    filterNames.push_back(filterName);
    prefs.endGroup();
  }
  return filterNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::on_bookmarksTreeView_clicked(const QModelIndex& index)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::on_bookmarksTreeView_doubleClicked(const QModelIndex& index)
{
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndex nameIndex = model->index(index.row(), BookmarksItem::Name, index.parent());
  QModelIndex pathIndex = model->index(index.row(), BookmarksItem::Path, index.parent());

  QString pipelinePath = pathIndex.data().toString();
  if(pipelinePath.isEmpty())
  {
    return; // The user double clicked a folder, so don't do anything
  }

  QFileInfo finfo(pipelinePath);
  if(finfo.isDir())
  {
    return;
  }
  else
  {
    QString path = pathIndex.data().toString();
    QFileInfo fi(path);
    if(fi.exists() == false)
    {
      bookmarksTreeView->blockSignals(true);
      QtSBookmarkMissingDialog* dialog = new QtSBookmarkMissingDialog(this, Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
      connect(dialog, SIGNAL(locateBtnPressed()), bookmarksTreeView, SLOT(on_actionLocateFile_triggered()));
      dialog->setBookmarkName(nameIndex.data().toString());
      dialog->exec();
      delete dialog;
      bookmarksTreeView->blockSignals(false);
    }
    else
    {
      bool itemHasErrors = model->data(pathIndex, Qt::UserRole).value<bool>();
      if(itemHasErrors == true)
      {
        // Set the itemHasError variable, and have the watcher monitor the file again
        model->setData(nameIndex, false, Qt::UserRole);
        model->getFileSystemWatcher()->addPath(path);
      }
      emit bookmarkActivated(pipelinePath);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BookmarksToolboxWidget::writeNewFavoriteFilePath(QString newFavoriteTitle, QString favoritePath, QTreeWidgetItem* item)
{
  QFileInfo original(favoritePath);
  QString newPath = original.canonicalPath() + QDir::separator() + newFavoriteTitle;
  if(!original.completeSuffix().isEmpty())
  {
    newPath += "." + original.completeSuffix();
  }

  newPath = QDir::toNativeSeparators(newPath);

  QFile f(favoritePath);
  bool success = f.rename(newPath);
  if(false == success)
  {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::critical(this, QObject::tr("Error renaming a file"), QObject::tr("Error renaming file '%1' to '%2'.").arg(favoritePath).arg(newPath), QMessageBox::Ok);
    Q_UNUSED(reply);
  }

  bookmarksTreeView->blockSignals(true);
  item->setData(0, Qt::UserRole, QVariant(newPath));
  bookmarksTreeView->blockSignals(false);

  return newPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksToolboxWidget::removeDir(const QString& dirName)
{
  bool result = true;
  QDir dir(dirName);

  if(dir.exists(dirName))
  {
    Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
    {
      if(info.isDir())
      {
        result = removeDir(info.absoluteFilePath());
      }
      else
      {
        result = QFile::remove(info.absoluteFilePath());
      }

      if(!result)
      {
        return result;
      }
    }
    result = dir.rmdir(dirName);
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::readSettings(QtSSettings* prefs)
{
  bool b = prefs->value(objectName(), QVariant(false)).toBool();
  setHidden(b);

  QByteArray headerState = prefs->value("Horizontal Header State", QByteArray());
  bookmarksTreeView->header()->restoreState(headerState);

  BookmarksModel* model = BookmarksModel::Instance();

  bookmarksTreeView->setModel(model);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::writeSettings()
{
  QtSSettings prefs;
  writeSettings(&prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::writeSettings(QtSSettings* prefs)
{
  prefs->setValue(objectName(), isHidden());

  prefs->setValue("Horizontal Header State", bookmarksTreeView->header()->saveState());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> BookmarksToolboxWidget::deserializeTreePath(QString treePath)
{
  QList<QString> list;
  int currentIndex = 0;
  int spaceIndex = 0;
  QString strPart = "";

  while(spaceIndex >= 0 && treePath.isEmpty() == false)
  {
    spaceIndex = treePath.indexOf('/');
    strPart = treePath.left(spaceIndex);
    strPart = strPart.simplified();
    if(strPart != "")
    {
      list.push_back(strPart);
    }
    treePath = treePath.remove(currentIndex, spaceIndex + 1);
  }

  return list;
}
