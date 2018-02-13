/**
  @file
  @author Stefan Frings
*/

#include "httplistener.h"

#include <QtCore/QCoreApplication>
#include <QtNetwork/QNetworkInterface>

#include "httpconnectionhandler.h"
#include "httpconnectionhandlerpool.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpListener::HttpListener(QSettings* settings, HttpRequestHandler* requestHandler, QObject* parent)
: QTcpServer(parent)
{
  Q_ASSERT(settings != nullptr);
  Q_ASSERT(requestHandler != nullptr);
  pool = nullptr;
  this->settings = settings;
  this->requestHandler = requestHandler;
  // Reqister type of socketDescriptor for signal/slot handling
  qRegisterMetaType<tSocketDescriptor>("tSocketDescriptor");
  // Start listening
  listen();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpListener::~HttpListener()
{
  close();
  qDebug() << "HttpListener: destroyed";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpListener::listen()
{
  if(!pool)
  {
    pool = new HttpConnectionHandlerPool(settings, requestHandler);
  }
  QString host = settings->value("host").toString();
  QHostAddress hostAddress(host);
  if(hostAddress.isNull())
  {
    foreach(const QNetworkInterface& netInterface, QNetworkInterface::allInterfaces())
    {
      QNetworkInterface::InterfaceFlags flags = netInterface.flags();
      if(static_cast<bool>(flags & QNetworkInterface::IsRunning) && !static_cast<bool>(flags & QNetworkInterface::IsLoopBack))
      {
        foreach(const QNetworkAddressEntry& address, netInterface.addressEntries())
        {
          if(address.ip().protocol() == QAbstractSocket::IPv4Protocol)
          {
            hostAddress = QHostAddress(address.ip());
          }
        }
      }
    }
  }
  int port = settings->value("port").toInt();

  QTcpServer::listen(hostAddress, static_cast<quint16>(port));
  if(!isListening())
  {
    qCritical() << "HttpListener: Cannot bind on port " << port << ": " << qPrintable(errorString());
  }
  else
  {
    qDebug() << "HttpListener: Listening on " << hostAddress.toString() << ":" << port;
  }

  // Pass the IPv4 address into the request handler so it has this proper information
  this->requestHandler->setListenHost(hostAddress, getPort());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpListener::close()
{
  QTcpServer::close();
  qDebug() << "HttpListener: closed";
  if(pool)
  {
    delete pool;
    pool = nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HttpListener::getPort()
{
  return settings->value("port").toInt();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpListener::incomingConnection(tSocketDescriptor socketDescriptor)
{
#ifdef SUPERVERBOSE
  qDebug() << "HttpListener: New connection";
#endif

  HttpConnectionHandler* freeHandler = nullptr;
  if(pool)
  {
    freeHandler = pool->getConnectionHandler();
  }

  // Let the handler process the new connection.
  if(freeHandler)
  {
    // The descriptor is passed via event queue because the handler lives in another thread
    QMetaObject::invokeMethod(freeHandler, "handleConnection", Qt::QueuedConnection, Q_ARG(tSocketDescriptor, socketDescriptor));
  }
  else
  {
    // Reject the connection
    qDebug() << "HttpListener: Too many incoming connections";
    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    socket->write("HTTP/1.1 503 too many connections\r\nConnection: close\r\n\r\nToo many connections\r\n");
    socket->disconnectFromHost();
  }
}
