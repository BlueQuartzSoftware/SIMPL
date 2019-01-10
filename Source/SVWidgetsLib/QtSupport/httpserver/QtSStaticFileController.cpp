/**
  @file
  @author Stefan Frings
*/

/**
* @brief Small changes to add support for json text files
*/
#include "QtSStaticFileController.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

QtSStaticFileController::QtSStaticFileController(ServerSettings* settings, QObject* parent)
: HttpRequestHandler(parent)
{
  verbose = settings->verbose;
  maxAge = settings->maxAge;
  encoding = settings->encoding;
  docroot = settings->docRootPath;
  if(!(docroot.startsWith(":/") || docroot.startsWith("qrc://")))
  {
// Convert relative path to absolute, based on the directory of the config file.
    if(QDir::isRelativePath(docroot))
    {
      QFileInfo configFile(settings->configFileName);
      docroot = QFileInfo(configFile.absolutePath(), docroot).absoluteFilePath();
    }
  }
  if(verbose)
  {
    qDebug("QtSStaticFileController: docroot=%s, encoding=%s, maxAge=%i", qPrintable(docroot), qPrintable(encoding), maxAge);
  }
  maxCachedFileSize = settings->maxCachedFileSize;
  cache.setMaxCost(settings->cacheSize);
  cacheTimeout = settings->cacheTime;
  if(verbose)
  {
    qDebug("QtSStaticFileController: cache timeout=%i, size=%i", cacheTimeout, cache.maxCost());
  }
}

void QtSStaticFileController::service(HttpRequest& request, HttpResponse& response)
{
  QByteArray path = request.getPath();
  // Check if we have the file in cache
  qint64 now = QDateTime::currentMSecsSinceEpoch();
  mutex.lock();
  CacheEntry* entry = cache.object(path);
  if((entry != nullptr) && (cacheTimeout == 0 || entry->created > now - cacheTimeout))
  {
    QByteArray document = entry->document; // copy the cached document, because other threads may destroy the cached entry immediately after mutex unlock.
    QByteArray filename = entry->filename;
    mutex.unlock();
    if(verbose)
    {
      qDebug("QtSStaticFileController: Cache hit for %s", path.data());
    }
    setContentType(filename, response);
    response.setHeader("Cache-Control", "max-age=" + QByteArray::number(maxAge / 1000));
    response.write(document);
  }
  else
  {
    mutex.unlock();
    // The file is not in cache.
    if(verbose)
    {
      qDebug("QtSStaticFileController: Cache miss for %s", path.data());
    }
    // Forbid access to files outside the docroot directory
    if(path.contains("/.."))
    {
      if(verbose)
      {
        qWarning("QtSStaticFileController: detected forbidden characters in path %s", path.data());
      }
      response.setStatusCode(HttpResponse::HttpStatusCode::Forbidden);
      response.write("403 forbidden", true);
      return;
    }
    // If the filename is a directory, append index.html.
    if(QFileInfo(docroot + path).isDir())
    {
      path += "/index.html";
    }
    // Try to open the file
    QFile file(docroot + path);
    if(verbose)
    {
      qDebug("QtSStaticFileController: Open file %s", qPrintable(file.fileName()));
    }
    if(file.open(QIODevice::ReadOnly))
    {
      setContentType(path, response);
      response.setHeader("Cache-Control", "max-age=" + QByteArray::number(maxAge / 1000));
      if(file.size() <= maxCachedFileSize)
      {
        // Return the file content and store it also in the cache
        entry = new CacheEntry();
        while(!file.atEnd() && (file.error() == 0u))
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
        while(!file.atEnd() && (file.error() == 0u))
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
        if(verbose)
        {
          qWarning("QtSStaticFileController: Cannot open existing file %s for reading", qPrintable(file.fileName()));
        }
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

void QtSStaticFileController::setContentType(QString fileName, HttpResponse& response) const
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
  else if(fileName.endsWith(".json"))
  {
    response.setHeader("Content-Type", "text/json");
  }
  else
  {
    if(verbose)
    {
      qDebug("QtSStaticFileController: unknown MIME type for filename '%s'", qPrintable(fileName));
    }
  }
}
