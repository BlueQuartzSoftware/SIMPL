#pragma once

#include <QtCore/QSettings>
#include <QtCore/QString>

#include "httpglobal.h"

class QtWebAppLib_EXPORT ServerSettings
{
public:
  explicit ServerSettings(QSettings& settings);
  ~ServerSettings();
  ServerSettings();

  ServerSettings(const ServerSettings&) = delete; // Copy Constructor Not Implemented
  ServerSettings(ServerSettings&&) = delete;      // Move Constructor Not Implemented
  ServerSettings& operator=(const ServerSettings&) = delete; // Copy Assignment Not Implemented
  ServerSettings& operator=(ServerSettings&&) = delete;      // Move Assignment Not Implemented

  QString configFileName;
  // [listener]
  QString host;
  uint16_t port = 0;
  int32_t minThreads = 4;
  int32_t maxThreads = 100;
  int32_t cleanupInterval = 60000;
  int32_t readTimeout = 6000;
  QString sslKeyFile = "ssl/my.key";
  QString sslCertFile = "ssl/my.cert";
  int32_t maxRequestSize = 16000;
  int64_t maxMultiPartSize = 4000000000;
  bool verbose = false;

  //  [templates]
  QString templatePath = "templates";
  QString suffix = ".tpl";
  QString encoding = "UTF-8";
  int32_t cacheSize = 1000000;
  int32_t cacheTime = 60000;

  //  [docroot]
  QString docRootPath = "docroot";
  //  QString  encoding="UTF-8";
  int32_t maxAge = 60000;
  //    int32_t  cacheSize=1000000;
  //    int32_t  cacheTime=60000;
  uint32_t maxCachedFileSize = 65536;

  //   [sessions]
  int32_t expirationTime = 3600000;
  QByteArray cookieName = "sessionid";
  QString cookiePath = "/";
  QString cookieComment = "Identifies the user";
  //    ;cookieDomain=stefanfrings.de

  //    [logging]
  //   ; The logging settings become effective after you comment in the related lines of code in main.cpp.
  QString logFileName = "Logs/SIMPLRestServer.log";
  int32_t minLevel = 1;
  int32_t bufferSize = 100;
  int32_t maxSize = 1000000;
  int32_t maxBackups = 2;
  QString timestampFormat = "dd.MM.yyyy hh:mm:ss.zzz";
  QString msgFormat = "{timestamp} {typeNr} {type} {thread} {msg}";
  // QT5 supports: msgFormat={timestamp} {typeNr} {type} {thread} {msg}\n  in {file} line {line} function {function}
};
