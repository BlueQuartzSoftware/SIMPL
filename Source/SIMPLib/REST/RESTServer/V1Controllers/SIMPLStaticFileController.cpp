
#include "SIMPLStaticFileController.h"
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "SIMPLib/Plugin/SIMPLPluginConstants.h"


#include "REST/RESTServer/SIMPLDirectoryListing.h"

SIMPLStaticFileController* SIMPLStaticFileController::m_Instance = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLStaticFileController* SIMPLStaticFileController::Instance()
{
  return m_Instance;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLStaticFileController::CreateInstance(QSettings* settings, QObject* parent)
{
  if (m_Instance != nullptr)
  {
    delete m_Instance;
  }

  m_Instance = new SIMPLStaticFileController(settings, parent);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SIMPLStaticFileController::getDocRoot() const
{
  return docroot;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLStaticFileController::SIMPLStaticFileController(QSettings* settings, QObject* parent)
: HttpRequestHandler(parent)
{
  maxAge = settings->value("maxAge", "60000").toInt();
  encoding = settings->value("encoding", "UTF-8").toString();
  docroot = settings->value("path", ".").toString();
  if(!(docroot.startsWith(":/") || docroot.startsWith("qrc://")))
  {
    // Convert relative path to absolute, based on the directory of the config file.
#ifdef Q_OS_WIN32
    if(QDir::isRelativePath(docroot) && settings->format() != QSettings::NativeFormat)
#else
    if(QDir::isRelativePath(docroot))
#endif
    {
      QFileInfo configFile(settings->fileName());
      docroot = QFileInfo(configFile.absolutePath(), docroot).absoluteFilePath();
    }
  }
  qDebug("SIMPLStaticFileController: docroot=%s, encoding=%s, maxAge=%i", qPrintable(docroot), qPrintable(encoding), maxAge);
  maxCachedFileSize = settings->value("maxCachedFileSize", "65536").toInt();
  cache.setMaxCost(settings->value("cacheSize", "1000000").toInt());
  cacheTimeout = settings->value("cacheTime", "60000").toInt();
  qDebug("SIMPLStaticFileController: cache timeout=%i, size=%i", cacheTimeout, cache.maxCost());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLStaticFileController::service(HttpRequest& request, HttpResponse& response)
{
  QByteArray path = request.getPath();
  // Check if we have the file in cache
  qint64 now = QDateTime::currentMSecsSinceEpoch();
  mutex.lock();
  CacheEntry* entry = cache.object(path);
  if(entry && (cacheTimeout == 0 || entry->created > now - cacheTimeout))
  {
    QByteArray document = entry->document; // copy the cached document, because other threads may destroy the cached entry immediately after mutex unlock.
    QByteArray filename = entry->filename;
    mutex.unlock();
    qDebug() << "SIMPLStaticFileController: Cache hit for %s", path.data();
    setContentType(filename, response);
    response.setHeader("Cache-Control", "max-age=" + QByteArray::number(maxAge / 1000));
    response.write(document);
  }
  else
  {
    mutex.unlock();
    // The file is not in cache.
    qDebug() << "SIMPLStaticFileController: Cache miss for %s", path.data();
    // Forbid access to files outside the docroot directory
    if(path.contains("/.."))
    {
      qWarning() << "SIMPLStaticFileController: detected forbidden characters in path " << path.data();
      response.setStatusCode(HttpResponse::HttpStatusCode::Forbidden);
      response.write("403 forbidden", true);
      return;
    }
    // If the filename is a directory, append index.html.
    if(QFileInfo(docroot + path).isDir())
    {
      QDir dir(docroot + path);
      QString html = SIMPLDirectoryListing::ParseDirForTable(dir);
      response.write(html.toUtf8(), true);
      return;
    }
    // Try to open the file
    QFile file(docroot + path);
    qDebug() << "SIMPLStaticFileController: Open file " << qPrintable(file.fileName());
    if(file.open(QIODevice::ReadOnly))
    {
      setContentType(path, response);
      response.setHeader("Cache-Control", "max-age=" + QByteArray::number(maxAge / 1000));
      if(file.size() <= maxCachedFileSize)
      {
        // Return the file content and store it also in the cache
        entry = new CacheEntry();
        while(!file.atEnd() && !file.error())
        {
          QByteArray buffer = file.read(65536);
          response.write(buffer);
          entry->document.append(buffer);
        }
        entry->created = now;
        entry->filename = path;
        mutex.lock();
        cache.insert(request.getPath(), entry, entry->document.size());
        mutex.unlock();
      }
      else
      {
        // Return the file content, do not store in cache
        while(!file.atEnd() && !file.error())
        {
          response.write(file.read(65536));
        }
      }
      file.close();
    }
    else
    {
      if(file.exists())
      {
        qWarning("SIMPLStaticFileController: Cannot open existing file %s for reading", qPrintable(file.fileName()));
        response.setStatusCode(HttpResponse::HttpStatusCode::Forbidden);
        response.write("403 forbidden", true);
      }
      else
      {
        response.setStatusCode(HttpResponse::HttpStatusCode::NotFound);
        response.write("404 not found", true);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLStaticFileController::setContentType(QString fileName, HttpResponse& response) const
{
  if(fileName.endsWith(".png"))
  {
    response.setHeader("Content-Type", "image/png");
  }
  else if(fileName.endsWith(".jpg"))
  {
    response.setHeader("Content-Type", "image/jpeg");
  }
  else if(fileName.endsWith(".gif"))
  {
    response.setHeader("Content-Type", "image/gif");
  }
  else if(fileName.endsWith(".pdf"))
  {
    response.setHeader("Content-Type", "application/pdf");
  }
  else if(fileName.endsWith(".txt"))
  {
    response.setHeader("Content-Type", qPrintable("text/plain; charset=" + encoding));
  }
  else if(fileName.endsWith(".html") || fileName.endsWith(".htm"))
  {
    response.setHeader("Content-Type", qPrintable("text/html; charset=" + encoding));
  }
  else if(fileName.endsWith(".css"))
  {
    response.setHeader("Content-Type", "text/css");
  }
  else if(fileName.endsWith(".js"))
  {
    response.setHeader("Content-Type", "text/javascript");
  }
  else if(fileName.endsWith(".svg"))
  {
    response.setHeader("Content-Type", "image/svg+xml");
  }
  else if(fileName.endsWith(".woff"))
  {
    response.setHeader("Content-Type", "font/woff");
  }
  else if(fileName.endsWith(".woff2"))
  {
    response.setHeader("Content-Type", "font/woff2");
  }
  else if(fileName.endsWith(".ttf"))
  {
    response.setHeader("Content-Type", "application/x-font-ttf");
  }
  else if(fileName.endsWith(".eot"))
  {
    response.setHeader("Content-Type", "application/vnd.ms-fontobject");
  }
  else if(fileName.endsWith(".otf"))
  {
    response.setHeader("Content-Type", "application/font-otf");
  }
  // Todo: add all of your content types
  else
  {
    qDebug("SIMPLStaticFileController: unknown MIME type for filename '%s'", qPrintable(fileName));
  }
}
