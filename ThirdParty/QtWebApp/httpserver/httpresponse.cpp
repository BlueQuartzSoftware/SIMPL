/**
  @file
  @author Stefan Frings
*/

#include "httpresponse.h"

#include <QtCore/QDateTime>

HttpResponse::HttpResponse(QTcpSocket* socket)
{
  createStatusCodeLookup();

  this->socket = socket;
  statusCode = HttpStatusCode::OK;
  sentHeaders = false;
  sentLastPart = false;
  chunkedMode = false;
}

void HttpResponse::setHeader(QByteArray name, QByteArray value)
{
  Q_ASSERT(sentHeaders == false);
  headers.insert(name, value);
}

void HttpResponse::setHeader(QByteArray name, int value)
{
  Q_ASSERT(sentHeaders == false);
  headers.insert(name, QByteArray::number(value));
}

QMap<QByteArray, QByteArray>& HttpResponse::getHeaders()
{
  return headers;
}

void HttpResponse::setStatusCode(HttpStatusCode statusCode)
{
  this->statusCode = statusCode;
}

HttpResponse::HttpStatusCode HttpResponse::getStatusCode() const
{
  return this->statusCode;
}

void HttpResponse::writeHeaders()
{
  Q_ASSERT(sentHeaders == false);
  QByteArray buffer;
  buffer.append("HTTP/1.1 ");
  buffer.append(QByteArray::number(static_cast<int>(statusCode)));
  buffer.append(' ');
  buffer.append(m_StatusCodeLookup.value(statusCode));
  buffer.append("\r\n");
  foreach(QByteArray name, headers.keys())
  {
    buffer.append(name);
    buffer.append(": ");
    buffer.append(headers.value(name));
    buffer.append("\r\n");
  }
  foreach(HttpCookie cookie, cookies.values())
  {
    buffer.append("Set-Cookie: ");
    buffer.append(cookie.toByteArray());
    buffer.append("\r\n");
  }
  buffer.append("\r\n");
  writeToSocket(buffer);
  sentHeaders = true;
}

bool HttpResponse::writeToSocket(QByteArray data)
{
  int remaining = data.size();
  char* ptr = data.data();
  while(socket->isOpen() && remaining > 0)
  {
    // If the output buffer has become large, then wait until it has been sent.
    if(socket->bytesToWrite() > 16384)
    {
      socket->waitForBytesWritten(-1);
    }

    int written = socket->write(ptr, remaining);
    if(written == -1)
    {
      return false;
    }
    ptr += written;
    remaining -= written;
  }
  return true;
}

void HttpResponse::write(QByteArray data, bool lastPart)
{
  Q_ASSERT(sentLastPart == false);

  // Send HTTP headers, if not already done (that happens only on the first call to write())
  if(!sentHeaders)
  {
    // If the whole response is generated with a single call to write(), then we know the total
    // size of the response and therefore can set the Content-Length header automatically.
    if(lastPart)
    {
      // Automatically set the Content-Length header
      headers.insert("Content-Length", QByteArray::number(data.size()));

      // Automatically set the Date header
      QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
      headers.insert("Date", QByteArray::fromStdString(utcDateTime.toString("ddd, d MMM yyyy hh:mm:ss t").toStdString()));
    }

    // else if we will not close the connection at the end, them we must use the chunked mode.
    else
    {
      QByteArray connectionValue = headers.value("Connection", headers.value("connection"));
      bool connectionClose = QString::compare(connectionValue, "close", Qt::CaseInsensitive) == 0;
      if(!connectionClose)
      {
        headers.insert("Transfer-Encoding", "chunked");
        chunkedMode = true;
      }
    }

    writeHeaders();
  }

  // Send data
  if(data.size() > 0)
  {
    if(chunkedMode)
    {
      if(data.size() > 0)
      {
        QByteArray size = QByteArray::number(data.size(), 16);
        writeToSocket(size);
        writeToSocket("\r\n");
        writeToSocket(data);
        writeToSocket("\r\n");
      }
    }
    else
    {
      writeToSocket(data);
    }
  }

  // Only for the last chunk, send the terminating marker and flush the buffer.
  if(lastPart)
  {
    if(chunkedMode)
    {
      writeToSocket("0\r\n\r\n");
    }
    socket->flush();
    sentLastPart = true;
  }
}

bool HttpResponse::hasSentLastPart() const
{
  return sentLastPart;
}

void HttpResponse::setCookie(const HttpCookie& cookie)
{
  Q_ASSERT(sentHeaders == false);
  if(!cookie.getName().isEmpty())
  {
    cookies.insert(cookie.getName(), cookie);
  }
}

QMap<QByteArray, HttpCookie>& HttpResponse::getCookies()
{
  return cookies;
}

void HttpResponse::redirect(const QByteArray& url)
{
  setStatusCode(HttpResponse::HttpStatusCode::SeeOther);
  setHeader("Location", url);
  write("Redirect", true);
}

void HttpResponse::flush()
{
  socket->flush();
}

bool HttpResponse::isConnected() const
{
  return socket->isOpen();
}

void HttpResponse::createStatusCodeLookup()
{
  m_StatusCodeLookup.insert(HttpStatusCode::Continue, "Continue");
  m_StatusCodeLookup.insert(HttpStatusCode::SwitchingProtocols, "Switching Protocols");

  m_StatusCodeLookup.insert(HttpStatusCode::OK, "OK");
  m_StatusCodeLookup.insert(HttpStatusCode::Created, "Created");
  m_StatusCodeLookup.insert(HttpStatusCode::Accepted, "Accepted");
  m_StatusCodeLookup.insert(HttpStatusCode::NonAuthoritativeInformation, "Non-Authoritative Information");
  m_StatusCodeLookup.insert(HttpStatusCode::NoContent, "No Content");
  m_StatusCodeLookup.insert(HttpStatusCode::ResetContent, "Reset Content");
  m_StatusCodeLookup.insert(HttpStatusCode::PartialContent, "Partial Content");

  m_StatusCodeLookup.insert(HttpStatusCode::MultipleChoices, "Multiple Choices");
  m_StatusCodeLookup.insert(HttpStatusCode::MovedPermanently, "Moved Permanently");
  m_StatusCodeLookup.insert(HttpStatusCode::Found, "Found");
  m_StatusCodeLookup.insert(HttpStatusCode::SeeOther, "See Other");
  m_StatusCodeLookup.insert(HttpStatusCode::NotModified, "Not Modified");
  m_StatusCodeLookup.insert(HttpStatusCode::UseProxy, "Use Proxy");
  m_StatusCodeLookup.insert(HttpStatusCode::TemporaryRedirect, "Temporary Redirect");

  m_StatusCodeLookup.insert(HttpStatusCode::BadRequest, "Bad Request");
  m_StatusCodeLookup.insert(HttpStatusCode::Unauthorized, "Unauthorized");
  m_StatusCodeLookup.insert(HttpStatusCode::PaymentRequired, "Payment Required");
  m_StatusCodeLookup.insert(HttpStatusCode::Forbidden, "Forbidden");
  m_StatusCodeLookup.insert(HttpStatusCode::NotFound, "Not Found");
  m_StatusCodeLookup.insert(HttpStatusCode::MethodNotAllowed, "Method Not Allowed");
  m_StatusCodeLookup.insert(HttpStatusCode::NotAcceptable, "Not Acceptable");
  m_StatusCodeLookup.insert(HttpStatusCode::ProxyAuthenticationRequired, "Proxy Authentication Required");
  m_StatusCodeLookup.insert(HttpStatusCode::RequestTimeout, "Request Time-out");
  m_StatusCodeLookup.insert(HttpStatusCode::Conflict, "Conflict");
  m_StatusCodeLookup.insert(HttpStatusCode::Gone, "Gone");
  m_StatusCodeLookup.insert(HttpStatusCode::LengthRequired, "Length Required");
  m_StatusCodeLookup.insert(HttpStatusCode::PreconditionFailed, "Precondition Failed");
  m_StatusCodeLookup.insert(HttpStatusCode::RequestEntityTooLarge, "Request Entity Too Large");
  m_StatusCodeLookup.insert(HttpStatusCode::RequestURITooLarge, "Request-URI Too Large");
  m_StatusCodeLookup.insert(HttpStatusCode::UnsupportedMediaType, "Unsupported Media Type");
  m_StatusCodeLookup.insert(HttpStatusCode::RequestedRangeNotSatisfiable, "Requested Range Not Satisfiable");
  m_StatusCodeLookup.insert(HttpStatusCode::ExpectationFailed, "Expectation Failed");

  m_StatusCodeLookup.insert(HttpStatusCode::InternalServerError, "Internal Server Error");
  m_StatusCodeLookup.insert(HttpStatusCode::NotImplemented, "Not Implemented");
  m_StatusCodeLookup.insert(HttpStatusCode::BadGateway, "Bad Gateway");
  m_StatusCodeLookup.insert(HttpStatusCode::ServiceUnavailable, "Service Unavailable");
  m_StatusCodeLookup.insert(HttpStatusCode::GatewayTimeout, "Gateway Time-out");
  m_StatusCodeLookup.insert(HttpStatusCode::HTTPVersionNotSupported, "HTTP Version Not Supported");
}
