/**
  @file
  @author Stefan Frings
*/

#include "httprequesthandler.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpRequestHandler::HttpRequestHandler(QObject* parent)
: QObject(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpRequestHandler::~HttpRequestHandler() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpRequestHandler::service(HttpRequest& request, HttpResponse& response)
{
  qCritical("HttpRequestHandler: you need to override the service() function");
  qDebug("HttpRequestHandler: request=%s %s %s", request.getMethod().data(), request.getPath().data(), request.getVersion().data());
  response.setStatusCode(HttpResponse::HttpStatusCode::NotImplemented);
  response.write("501 not implemented", true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpRequestHandler::setListenHost(const QHostAddress& hostAddress, const int port)
{
  m_HostAddress = hostAddress;
  m_Port = port;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHostAddress HttpRequestHandler::getListenHost() const
{
  return m_HostAddress;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HttpRequestHandler::getListenPort() const
{
  return m_Port;
}
