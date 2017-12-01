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

#include "SIMPLViewMenuItems.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>

#include <QtGui/QClipboard>

#include <QtWidgets/QApplication>

#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"

#include "SVWidgetsLib/Widgets/SIMPLViewToolbox.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data

SIMPLViewMenuItems* SIMPLViewMenuItems::self = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLViewMenuItems::SIMPLViewMenuItems(QObject* parent)
: QObject(parent)
{
  createMenus();
  createActions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLViewMenuItems::~SIMPLViewMenuItems()
{
  delete m_MenuRecentFiles;
  delete m_ActionNew;
  delete m_ActionOpen;
  delete m_ActionSave;
  delete m_ActionSaveAs;
  delete m_ActionLoadTheme;
  delete m_ActionSaveTheme;
  delete m_ActionClearRecentFiles;
  delete m_ActionExit;

  // Edit Menu
  delete m_ActionCut;
  delete m_ActionCopy;
  delete m_ActionPaste;

  // View Menu
  delete m_ActionShowFilterLibrary;
  delete m_ActionShowFilterList;
  delete m_ActionShowBookmarks;
  delete m_ActionShowToolbox;

  // Bookmarks Menu
  delete m_ActionAddBookmark;
  delete m_ActionNewFolder;
  delete m_ActionLocateFile;

  // Pipeline Menu
  delete m_ActionClearPipeline;

  // Help Menu
  delete m_ActionClearCache;
  delete m_ActionClearBookmarks;
  delete m_ActionShowSIMPLViewHelp;
  delete m_ActionCheckForUpdates;
  delete m_ActionAboutSIMPLView;
  delete m_ActionPluginInformation;

  // Contextual Menus
  delete m_ActionRenameBookmark;
  delete m_ActionRemoveBookmark;
  delete m_ActionShowBookmarkInFileSystem;

  delete m_ActionOpenBookmark;
  delete m_ActionOpenExecuteBookmark;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLViewMenuItems* SIMPLViewMenuItems::Instance()
{
  if(nullptr == self)
  {
    self = new SIMPLViewMenuItems();
  }

  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLViewMenuItems::createMenus()
{
  m_MenuRecentFiles = new QMenu("Recent Files");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLViewMenuItems::createActions()
{
  m_ActionRenameBookmark = new QAction("Rename Pipeline", this);
  m_ActionRemoveBookmark = new QAction("Remove Bookmark", this);
  m_ActionClearPipeline = new QAction("Clear Pipeline", this);
  m_ActionShowBookmarkInFileSystem = new QAction(this);
  m_ActionExit = new QAction("Exit " + QApplication::applicationName(), this);
  m_ActionOpen = new QAction("Open...", this);
  m_ActionNew = new QAction("New...", this);
  m_ActionClearRecentFiles = new QAction("Clear Recent Files", this);
  m_ActionShowSIMPLViewHelp = new QAction(QApplication::applicationName() + " Help", this);
  m_ActionAboutSIMPLView = new QAction("About " + QApplication::applicationName(), this);
  m_ActionCheckForUpdates = new QAction("Check For Updates", this);
  m_ActionPluginInformation = new QAction("Plugin Information", this);
  m_ActionSave = new QAction("Save", this);
  m_ActionSaveAs = new QAction("Save As...", this);
  m_ActionClearCache = new QAction("Clear Cache", this);
  m_ActionClearBookmarks = new QAction("Clear Bookmarks", this);
  m_ActionShowFilterLibrary = new QAction("Show Filter Library", this);
  m_ActionShowFilterLibrary->setCheckable(true);
  m_ActionShowFilterList = new QAction("Show Filter List", this);
  m_ActionShowFilterList->setCheckable(true);
  m_ActionShowBookmarks = new QAction("Show Bookmarks", this);
  m_ActionShowBookmarks->setCheckable(true);
  m_ActionShowToolbox = new QAction("Show Toolbox", this);
  m_ActionShowToolbox->setCheckable(true);
  m_ActionAddBookmark = new QAction("Add Bookmark", this);
  m_ActionNewFolder = new QAction("New Folder", this);
  m_ActionCut = new QAction("Cut", this);
  m_ActionCopy = new QAction("Copy", this);
  m_ActionPaste = new QAction("Paste", this);
  m_ActionLoadTheme = new QAction("Load Theme", this);
  m_ActionSaveTheme = new QAction("Save Theme", this);
  m_ActionLocateFile = new QAction("Locate File", this);

#if defined(Q_OS_WIN)
  m_ActionShowBookmarkInFileSystem->setText("Show in Windows Explorer");
#elif defined(Q_OS_MAC)
  m_ActionShowBookmarkInFileSystem->setText("Show in Finder");
#else
  m_ActionShowBookmarkInFileSystem->setText("Show in File System");
#endif

  m_ActionOpenBookmark = new QAction("Open Bookmark", this);
  m_ActionOpenExecuteBookmark = new QAction("Execute Bookmark", this);

  m_ActionClearPipeline->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Backspace));
  m_ActionOpen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
  m_ActionNew->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
  m_ActionExit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
  m_ActionShowSIMPLViewHelp->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
  m_ActionCheckForUpdates->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
  m_ActionPluginInformation->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
  m_ActionSave->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
  m_ActionSaveAs->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
  m_ActionAddBookmark->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
  m_ActionNewFolder->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
  m_ActionCut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
  m_ActionCopy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
  m_ActionPaste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));

  QClipboard* clipboard = QApplication::clipboard();
  connect(clipboard, SIGNAL(dataChanged()), this, SLOT(updatePasteAvailability()));

  // Run this once, so that the Paste button availability is updated for what is currently on the system clipboard
  updatePasteAvailability();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------s
void SIMPLViewMenuItems::updatePasteAvailability()
{
  QClipboard* clipboard = QApplication::clipboard();
  QString text = clipboard->text();

  JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
  FilterPipeline::Pointer pipeline = jsonReader->readPipelineFromString(text);
  if(text.isEmpty() || FilterPipeline::NullPointer() == pipeline)
  {
    m_CanPaste = false;
  }
  else
  {
    m_CanPaste = true;
  }

  emit clipboardHasChanged(m_CanPaste);
}
