/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <memory>

#include "QtSDocServer.h"

#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtWidgets/QApplication>

#include <QtCore/QDebug>

#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/IFilterFactory.hpp"

#include "QtWebApp/httpserver/httplistener.h"
#include "QtWebApp/httpserver/httpsessionstore.h"

#include "QtWebApp/logging/filelogger.h"

#include "SVWidgetsLib/QtSupport/httpserver/QtSStaticFileController.h"

#include "QtWebApp/httpserver/ServerSettings.h"

QtSDocServer* QtSDocServer::self = nullptr;

// static HttpListener* httpListener = nullptr;
// static HttpSessionStore* sessionStore = nullptr;

// static ServerSettings* s_ServerSettings = nullptr;

// static QtSStaticFileController* staticFileController = nullptr;

namespace
{
const int k_Port = 32456;
const QString k_IPAddress = QString("127.0.0.1");
} // namespace

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSDocServer::QtSDocServer(QObject* parent)
: QObject(parent)
{
  initializeDocServer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSDocServer::~QtSDocServer() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSDocServer* QtSDocServer::Instance()
{

  if(self == nullptr)
  {
    self = new QtSDocServer();
  }
  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QtSDocServer::GetHelpRootDir()
{
  QString appPath = QApplication::applicationDirPath();

  QDir helpDir = QDir(appPath);

#if defined(Q_OS_MAC)
  if(helpDir.dirName() == "MacOS")
  {
    helpDir.cdUp();
    // Check if we are running from a .app installation where the Help dir is embeded in the app bundle.
    QFileInfo fi(helpDir.absolutePath() + "/Resources/Help");

    if(fi.exists())
    {
      // qDebug() << "helpDir: " << fi.absolutePath();
      helpDir.cd("Resources");
      helpDir.cd("Help");
      helpDir.cd(QApplication::applicationName());
    }
    else
    {
      helpDir.cdUp();
      helpDir.cdUp();
    }
  }
#elif defined(Q_OS_WIN)
  // This is where the help _should_ be for either a deployed app or when NOT using VS IDE to run/build
  helpDir = QDir(appPath + "/Help/" + QApplication::applicationName());
  if(!helpDir.exists())
  {
    // Try one level up as we may be running from a Visual Studio Instance
    helpDir = QDir(appPath);
    helpDir.cdUp();
    helpDir.cd("Help");
    helpDir.cd(QApplication::applicationName());
  }
#else
  // This is where the help _should_ be for either a deployed app or when NOT using VS IDE to run/build
  helpDir = QDir(appPath + "/Help/" + QApplication::applicationName());
  if(!helpDir.exists())
  {
    // Try one level up as we may be running from a Visual Studio Instance
    helpDir = QDir(appPath);
    helpDir.cdUp();
    helpDir.cd("Help");
    helpDir.cd(QApplication::applicationName());
  }
#endif
  return helpDir.absolutePath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSDocServer::initializeDocServer()
{

  m_ServerSettings = std::shared_ptr<ServerSettings>(new ServerSettings());
  QString rootDir = QtSDocServer::GetHelpRootDir();
  m_ServerSettings->docRootPath = rootDir;
  m_HttpSessionStore = std::shared_ptr<HttpSessionStore>(HttpSessionStore::CreateInstance(m_ServerSettings.get(), this));
  m_QtSStaticFileController = std::shared_ptr<QtSStaticFileController>(new QtSStaticFileController(m_ServerSettings.get(), this));

  // Configure and start the TCP listener
  m_ServerSettings->host = QtSDocServer::GetIPAddress();
  m_ServerSettings->port = QtSDocServer::GetPort();

  m_HttpListener = std::shared_ptr<HttpListener>(new HttpListener(m_ServerSettings.get(), m_QtSStaticFileController.get(), this));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QtSDocServer::GetIPAddress()
{
  return ::k_IPAddress;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QtSDocServer::GetPort()
{
  return ::k_Port;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl QtSDocServer::GenerateHTMLUrl(const QString& className)
{

  QString s = QString("http://%1:%2").arg(QtSDocServer::GetIPAddress()).arg(QtSDocServer::GetPort());

#ifdef SIMPL_USE_MKDOCS
  {
    FilterManager* fm = FilterManager::Instance();

    IFilterFactory::Pointer factory = fm->getFactoryFromClassName(className);
    QString pluginName;
    if(factory)
    {
      pluginName = "/Filters/" + factory->getCompiledLibraryName() + "Filters";
    }
    s = s.append(pluginName).append("/").append(className).append("/index.html");
    return QUrl(s);
  }
#endif

#ifdef SIMPL_USE_DISCOUNT
  {
    QString appPath = QApplication::applicationDirPath();

    QDir helpDir = QDir(appPath);
    FilterManager* fm = FilterManager::Instance();

    IFilterFactory::Pointer factory = fm->getFactoryFromClassName(className);
    QString pluginName;
    if(factory.get())
    {
      pluginName = "/Plugins/" + factory->getCompiledLibraryName();
    }

    QString helpFilePath = QString("%1/Help/%2%3/%4/%4.html").arg(helpDir.absolutePath()).arg(QCoreApplication::instance()->applicationName()).arg(pluginName).arg(className);
    QFileInfo fi(helpFilePath);
    if(fi.exists() == false)
    {
      // The help file does not exist at the default location because we are probably running from Visual Studio or Xcode
      // Try up one more directory
      helpDir.cdUp();
      helpFilePath = QString("%1/Help/%2%3/%4/%4.html").arg(helpDir.absolutePath()).arg(QCoreApplication::instance()->applicationName()).arg(pluginName).arg(className);
    }

    s = s + helpFilePath;
  }
#endif

  return QUrl(s);
}

// -----------------------------------------------------------------------------
QString QtSDocServer::getNameOfClass() const
{
  return QString("QtSDocServer");
}

// -----------------------------------------------------------------------------
QString QtSDocServer::ClassName()
{
  return QString("QtSDocServer");
}
