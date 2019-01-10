#include "ServerSettings.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ServerSettings::ServerSettings() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ServerSettings::ServerSettings(QSettings& settings)
{

  configFileName = settings.fileName();

  settings.beginGroup("listener");
  host = settings.value("host", "").toString();
  port = settings.value("port", 8080).toInt();
  minThreads = settings.value("minThreads", 4).toInt();
  maxThreads = settings.value("maxThreads", 100).toInt();
  cleanupInterval = settings.value("cleanupInterval", 60000).toInt();
  readTimeout = settings.value("readTimeout", 60000).toInt();
  sslKeyFile = settings.value("ssl/my.key", "").toString();
  sslCertFile = settings.value("ssl/my.cert", "").toString();
  maxRequestSize = settings.value("maxRequestSize", 16000).toInt();
  maxMultiPartSize = settings.value("maxMultiPartSize", 4000000000LL).toLongLong();
  verbose = settings.value("verbose", false).toBool();
  settings.endGroup();

  settings.beginGroup("templates");
  templatePath = settings.value("path", "templates").toString();
  suffix = settings.value("suffix", ".tpl").toString();
  encoding = settings.value("encoding", "UTF-8").toString();
  cacheSize = settings.value("cacheSize", 1000000).toInt();
  cacheTime = settings.value("cacheTime", 60000).toInt();
  settings.endGroup();

  settings.beginGroup("docroot");
  docRootPath = settings.value("path", "docroot").toString();
  encoding = settings.value("encoding", "UTF-8").toString();
  maxAge = settings.value("maxAge", 60000).toInt();
  cacheTime = settings.value("cacheTime", 60000).toInt();
  cacheSize = settings.value("cacheSize", 1000000).toInt();
  maxCachedFileSize = settings.value("maxCachedFileSize", 65536).toInt();
  settings.endGroup();

  settings.beginGroup("sessions");
  expirationTime = settings.value("expirationTime", 3600000).toInt();
  cookieName = settings.value("cookieName", "sessionid").toByteArray();
  cookiePath = settings.value("cookiePath", "/").toString();
  cookieComment = settings.value("cookieComment", "Identifies the user").toString();
  settings.endGroup();

  settings.beginGroup("logging");
  logFileName = settings.value("fileName", "Logs/SIMPLRestServer.log").toString();
  minLevel = settings.value("minLevel", 1).toInt();
  bufferSize = settings.value("bufferSize", 100).toInt();
  maxSize = settings.value("maxSize", 1000000).toInt();
  maxBackups = settings.value("maxBackups", 2).toInt();
  timestampFormat = settings.value("timestampFormat", "dd.MM.yyyy hh:mm:ss.zzz").toString();
  msgFormat = settings.value("msgFormat", "{timestamp} {typeNr} {type} {thread} {msg}").toString();
  settings.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ServerSettings::~ServerSettings() = default;
