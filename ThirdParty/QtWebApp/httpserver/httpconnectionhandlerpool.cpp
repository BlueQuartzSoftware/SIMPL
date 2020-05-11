#ifndef QT_NO_OPENSSL
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslKey>
#include <QSslSocket>
#endif
#include "httpconnectionhandlerpool.h"
#include <QDir>

HttpConnectionHandlerPool::HttpConnectionHandlerPool(ServerSettings* settings, HttpRequestHandler* requestHandler)
{
  Q_ASSERT(settings != nullptr);
  this->settings = settings;
  this->requestHandler = requestHandler;
  this->sslConfiguration = nullptr;
  verbose = settings->verbose;
  loadSslConfig();
  cleanupTimer.start(settings->cleanupInterval);
  connect(&cleanupTimer, SIGNAL(timeout()), SLOT(cleanup()));
}

HttpConnectionHandlerPool::~HttpConnectionHandlerPool()
{
  // delete all connection handlers and wait until their threads are closed
  foreach(HttpConnectionHandler* handler, pool)
  {
    delete handler;
  }
  delete sslConfiguration;
  if(verbose)
  {
    qDebug() << "HttpConnectionHandlerPool (%p): destroyed" << this;
  }
}

HttpConnectionHandler* HttpConnectionHandlerPool::getConnectionHandler()
{
  HttpConnectionHandler* freeHandler = nullptr;
  mutex.lock();
  // find a free handler in pool
  foreach(HttpConnectionHandler* handler, pool)
  {
    if(!handler->isBusy())
    {
      freeHandler = handler;
      freeHandler->setBusy();
      break;
    }
  }
  // create a new handler, if necessary
  if(freeHandler == nullptr)
  {
    int maxConnectionHandlers = settings->maxThreads;
    if(pool.count() < maxConnectionHandlers)
    {
      freeHandler = new HttpConnectionHandler(settings, requestHandler, sslConfiguration);
      freeHandler->setBusy();
      pool.append(freeHandler);
    }
  }
  mutex.unlock();
  return freeHandler;
}

void HttpConnectionHandlerPool::cleanup()
{
  int maxIdleHandlers = settings->minThreads;
  int idleCounter = 0;
  mutex.lock();
  foreach(HttpConnectionHandler* handler, pool)
  {
    if(!handler->isBusy())
    {
      if(++idleCounter > maxIdleHandlers)
      {
        delete handler;
        pool.removeOne(handler);
        if(verbose)
        {
          qDebug("HttpConnectionHandlerPool: Removed connection handler (%p), pool size is now %i", handler, pool.size());
        }
        break; // remove only one handler in each interval
      }
    }
  }
  mutex.unlock();
}

void HttpConnectionHandlerPool::loadSslConfig()
{
  // If certificate and key files are configured, then load them
  QString sslKeyFileName = settings->sslKeyFile;
  QString sslCertFileName = settings->sslCertFile;
  if(!sslKeyFileName.isEmpty() && !sslCertFileName.isEmpty())
  {
#ifdef QT_NO_OPENSSL
    qWarning("HttpConnectionHandlerPool: SSL is not supported");
#else
    // Convert relative fileNames to absolute, based on the directory of the config file.
    QFileInfo configFile(settings->configFileName);
    if(QDir::isRelativePath(sslKeyFileName))
    {
      sslKeyFileName = QFileInfo(configFile.absolutePath(), sslKeyFileName).absoluteFilePath();
    }
    if(QDir::isRelativePath(sslCertFileName))
    {
      sslCertFileName = QFileInfo(configFile.absolutePath(), sslCertFileName).absoluteFilePath();
    }

    // Load the SSL certificate
    QFile certFile(sslCertFileName);
    if(!certFile.open(QIODevice::ReadOnly))
    {
      qCritical("HttpConnectionHandlerPool: cannot open sslCertFile %s", qPrintable(sslCertFileName));
      return;
    }
    QSslCertificate certificate(&certFile, QSsl::Pem);
    certFile.close();

    // Load the key file
    QFile keyFile(sslKeyFileName);
    if(!keyFile.open(QIODevice::ReadOnly))
    {
      qCritical("HttpConnectionHandlerPool: cannot open sslKeyFile %s", qPrintable(sslKeyFileName));
      return;
    }
    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    keyFile.close();

    // Create the SSL configuration
    sslConfiguration = new QSslConfiguration();
    sslConfiguration->setLocalCertificate(certificate);
    sslConfiguration->setPrivateKey(sslKey);
    sslConfiguration->setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration->setProtocol(QSsl::TlsV1SslV3);

    if(verbose)
    {
      qDebug("HttpConnectionHandlerPool: SSL settings loaded");
    }
#endif
  }
}
