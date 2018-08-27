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

#include "SIMPLibPluginLoader.h"

#if !defined(_MSC_VER)
#include <unistd.h>
#endif

// Qt Includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>
#include <QtCore/QStringList>
#include <QtCore/QtDebug>

#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/PluginManager.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLibPluginLoader::SIMPLibPluginLoader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLibPluginLoader::~SIMPLibPluginLoader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLibPluginLoader::LoadPluginFilters(FilterManager* filterManager, bool quiet)
{
  QStringList pluginDirs;
  pluginDirs << qApp->applicationDirPath();

  QDir aPluginDir = QDir(qApp->applicationDirPath());
  if(!quiet) qDebug() << "Loading SIMPLib Plugins....";
  // qDebug() << "aPluginDir: " << aPluginDir.absolutePath() << "\n";
  QString thePath;

#if defined(Q_OS_WIN)
  if(aPluginDir.cd("Plugins"))
  {
    thePath = aPluginDir.absolutePath();
    pluginDirs << thePath;
  }
#elif defined(Q_OS_MAC)
  // Look to see if we are inside an .app package or inside the 'tools' directory
  if(aPluginDir.dirName() == "MacOS")
  {
    aPluginDir.cdUp();
    thePath = aPluginDir.absolutePath() + "/Plugins";
    if(!quiet) qDebug() << "  Adding Path " << thePath;
    pluginDirs << thePath;
    aPluginDir.cdUp();
    aPluginDir.cdUp();
    // We need this because Apple (in their infinite wisdom) changed how the current working directory is set in OS X 10.9 and above. Thanks Apple.
    chdir(aPluginDir.absolutePath().toLatin1().constData());
  }
  if(aPluginDir.dirName() == "bin")
  {
    aPluginDir.cdUp();
    // We need this because Apple (in their infinite wisdom) changed how the current working directory is set in OS X 10.9 and above. Thanks Apple.
    chdir(aPluginDir.absolutePath().toLatin1().constData());
  }
  // aPluginDir.cd("Plugins");
  thePath = aPluginDir.absolutePath() + "/Plugins";
  if(!quiet) qDebug() << "  Adding Path " << thePath;
  pluginDirs << thePath;

// This is here for Xcode compatibility
#ifdef CMAKE_INTDIR
  aPluginDir.cdUp();
  thePath = aPluginDir.absolutePath() + "/Plugins/" + CMAKE_INTDIR;
  pluginDirs << thePath;
#endif
#else
  // We are on Linux - I think
  // Try the current location of where the application was launched from which is
  // typically the case when debugging from a build tree
  if(aPluginDir.cd("Plugins"))
  {
    thePath = aPluginDir.absolutePath();
    pluginDirs << thePath;
    aPluginDir.cdUp(); // Move back up a directory level
  }

  if(thePath.isEmpty())
  {
    // Now try moving up a directory which is what should happen when running from a
    // proper distribution of DREAM3D
    aPluginDir.cdUp();
    if(aPluginDir.cd("Plugins"))
    {
      thePath = aPluginDir.absolutePath();
      pluginDirs << thePath;
      aPluginDir.cdUp(); // Move back up a directory level
      int no_error = chdir(aPluginDir.absolutePath().toLatin1().constData());
      if(no_error < 0)
      {
        if(!quiet) qDebug() << "Could not set the working directory.";
      }
    }
  }
#endif

  QByteArray pluginEnvPath = qgetenv("SIMPL_PLUGIN_PATH");
  if(!quiet) if(!quiet) qDebug() << "SIMPL_PLUGIN_PATH:" << pluginEnvPath;

  char sep = ';';
#if defined(Q_OS_WIN)
  sep = ':';
#endif
  QList<QByteArray> envPaths = pluginEnvPath.split(sep);
  foreach(QByteArray envPath, envPaths)
  {
    if(envPath.size() > 0)
    {
      pluginDirs << QString::fromLatin1(envPath);
    }
  }

  int dupes = pluginDirs.removeDuplicates();
  if(!quiet) qDebug() << "Removed " << dupes << " duplicate Plugin Paths";
  QStringList pluginFilePaths;

  foreach(QString pluginDirString, pluginDirs)
  {
    if(!quiet) qDebug() << "Plugin Directory being Searched: " << pluginDirString;
    aPluginDir = QDir(pluginDirString);
    foreach(QString fileName, aPluginDir.entryList(QDir::Files))
    {
//   qDebug() << "File: " << fileName() << "\n";
#ifdef QT_DEBUG
      if(fileName.endsWith("_debug.plugin", Qt::CaseSensitive))
#else
      if(fileName.endsWith(".plugin", Qt::CaseSensitive)            // We want ONLY Release plugins
         && !fileName.endsWith("_debug.plugin", Qt::CaseSensitive)) // so ignore these plugins
#endif
      {
        pluginFilePaths << aPluginDir.absoluteFilePath(fileName);
        // qWarning(aPluginDir.absoluteFilePath(fileName).toLatin1(), "%s");
        // qDebug() << "Adding " << aPluginDir.absoluteFilePath(fileName)() << "\n";
      }
    }
  }

  filterManager->RegisterKnownFilters(filterManager);

  PluginManager* pluginManager = PluginManager::Instance();
  QStringList pluginFileNames;
  QVector<ISIMPLibPlugin*> loadedPlugins;

  // Now that we have a sorted list of plugins, go ahead and load them all from the
  // file system and add each to the toolbar and menu
  foreach(QString path, pluginFilePaths)
  {
    if(!quiet) qDebug() << "Plugin Being Loaded:" << path;
    QPluginLoader loader(path);
    QFileInfo fi(path);
    QString fileName = fi.fileName();
    QObject* plugin = loader.instance();
    if(!quiet) qDebug() << "    Pointer: " << plugin << "\n";
    if(plugin && pluginFileNames.contains(fileName, Qt::CaseSensitive) == false)
    {
      ISIMPLibPlugin* ipPlugin = qobject_cast<ISIMPLibPlugin*>(plugin);
      if(ipPlugin)
      {
        loadedPlugins.push_back(ipPlugin);
        ipPlugin->registerFilters(filterManager);
        ipPlugin->setDidLoad(true);
        ipPlugin->setLocation(path);
        pluginManager->addPlugin(ipPlugin);
      }
      pluginFileNames += fileName;
    }
    else
    {
      if(!quiet) 
      {
        QString message("The plugin did not load with the following error\n");
        message.append(loader.errorString());
        message.append("\n\n");
        message.append("Possible causes include missing libraries that plugin depends on.");
        qDebug() << message;
      }
    }
  }
}
