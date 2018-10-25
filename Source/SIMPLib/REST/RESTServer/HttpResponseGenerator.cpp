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

#include "HttpResponseGenerator.h"

#include <QtCore/QUuid>
#include <QtCore/QDateTime>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpResponseGenerator::HttpResponseGenerator(QObject* parent)
: QObject(parent)
, m_Boundary(QUuid::createUuid().toString())
{
  createResponseCodeLookup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpResponseGenerator::~HttpResponseGenerator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HttpResponseGenerator::generateResponse()
{
  QString responseMsg;

  QString responseHeader;
  QTextStream headerStream(&responseHeader);

  headerStream << tr("%1 %2 %3").arg(m_Version).arg(static_cast<int>(m_ResponseCode)).arg(m_ResponseCodeLookup.value(m_ResponseCode));

  headerStream << "\n";
  for (StringMultiMap::iterator iter = m_Headers.begin(); iter != m_Headers.end(); iter++)
  {
    QString headerName = iter.key();
    QString headerData = iter.value();

    if (headerName == "Content-Type" && headerData.startsWith("multipart"))
    {
      QString boundaryLabel = "boundary=\"";
      if (headerData.contains(boundaryLabel))
      {
        int boundaryLabelIndex = headerData.indexOf(boundaryLabel);
        int boundaryStartIndex = boundaryLabelIndex + boundaryLabel.size();
        int boundaryEndIndex = headerData.indexOf("\"", boundaryStartIndex);
        if (boundaryEndIndex >= 0)
        {
          m_Boundary = headerData.mid(boundaryStartIndex, boundaryEndIndex - boundaryStartIndex);
        }
      }
    }

    headerStream << "\n";
    headerStream << tr("%1: %2").arg(headerName).arg(headerData);
  }

  QString responseBody;
  QTextStream bodyStream(&responseBody);

  if (m_Headers.contains("Content-Type") && m_Headers.value("Content-Type").startsWith("multipart") && !m_Boundary.isEmpty())
  {
    bodyStream << tr("--%1\n").arg(m_Boundary);
  }

  for (StringMultiMap::iterator iter = m_Parameters.begin(); iter != m_Parameters.end(); iter++)
  {
    QString parameterName = iter.key();
    QString parameterData = iter.value();

    QString contentDispositionString = tr("Content-Disposition: form-data; name=\"%1\"\n").arg(parameterName);
    if (m_Headers.contains("Content-Type") && m_Headers.value("Content-Type").startsWith("multipart") && !m_Boundary.isEmpty())
    {
      bodyStream << contentDispositionString;
    }
    else
    {
      headerStream << contentDispositionString;
    }

    bodyStream << "\n";
    bodyStream << parameterData << "\n";
    bodyStream << "\n";

    if (m_Headers.contains("Content-Type") && m_Headers.value("Content-Type").startsWith("multipart") && !m_Boundary.isEmpty())
    {
      bodyStream << tr("--%1").arg(m_Boundary);
    }
  }

  responseMsg.append(responseHeader);
  responseMsg.append("\n");
  responseMsg.append(responseBody);

  return responseMsg;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HttpResponseGenerator::getBodySize()
{
  QString responseBody;
  QTextStream bodyStream(&responseBody);

  if (m_Headers.contains("Content-Type") && m_Headers.value("Content-Type").startsWith("multipart") && !m_Boundary.isEmpty())
  {
    bodyStream << tr("--%1\n").arg(m_Boundary);
  }

  for (StringMultiMap::iterator iter = m_Parameters.begin(); iter != m_Parameters.end(); iter++)
  {
    QString parameterName = iter.key();
    QString parameterData = iter.value();

    QString contentDispositionString = tr("Content-Disposition: form-data; name=\"%1\"\n").arg(parameterName);
    if (m_Headers.contains("Content-Type") && m_Headers.value("Content-Type").startsWith("multipart") && !m_Boundary.isEmpty())
    {
      bodyStream << contentDispositionString;
    }

    bodyStream << "\n";
    bodyStream << parameterData << "\n";
    bodyStream << "\n";

    if (m_Headers.contains("Content-Type") && m_Headers.value("Content-Type").startsWith("multipart") && !m_Boundary.isEmpty())
    {
      bodyStream << tr("--%1\n").arg(m_Boundary);
    }
  }

  QByteArray bodyArray = responseBody.toUtf8();
  int bodyLength = bodyArray.size();
  return bodyLength;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpResponseGenerator::setHeader(const QString &headerName, const QString &headerData)
{
  m_Headers.insert(headerName, headerData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpResponseGenerator::setParameter(const QString &parameterName, const QString &parameterData)
{
  m_Parameters.insert(parameterName, parameterData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpResponseGenerator::clearGenerator()
{
  m_Version.clear();
  m_ResponseCode = HttpResponseCode::Unknown;
  m_Boundary.clear();
  m_Body.clear();
  m_Headers.clear();
  m_Parameters.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpResponseGenerator::createResponseCodeLookup()
{
  m_ResponseCodeLookup.insert(HttpResponseCode::Continue, "Continue");
  m_ResponseCodeLookup.insert(HttpResponseCode::SwitchingProtocols, "Switching Protocols");

  m_ResponseCodeLookup.insert(HttpResponseCode::OK, "OK");
  m_ResponseCodeLookup.insert(HttpResponseCode::Created, "Created");
  m_ResponseCodeLookup.insert(HttpResponseCode::Accepted, "Accepted");
  m_ResponseCodeLookup.insert(HttpResponseCode::NonAuthoritativeInformation, "Non-Authoritative Information");
  m_ResponseCodeLookup.insert(HttpResponseCode::NoContent, "No Content");
  m_ResponseCodeLookup.insert(HttpResponseCode::ResetContent, "Reset Content");
  m_ResponseCodeLookup.insert(HttpResponseCode::PartialContent, "Partial Content");

  m_ResponseCodeLookup.insert(HttpResponseCode::MultipleChoices, "Multiple Choices");
  m_ResponseCodeLookup.insert(HttpResponseCode::MovedPermanently, "Moved Permanently");
  m_ResponseCodeLookup.insert(HttpResponseCode::Found, "Found");
  m_ResponseCodeLookup.insert(HttpResponseCode::SeeOther, "See Other");
  m_ResponseCodeLookup.insert(HttpResponseCode::NotModified, "Not Modified");
  m_ResponseCodeLookup.insert(HttpResponseCode::UseProxy, "Use Proxy");
  m_ResponseCodeLookup.insert(HttpResponseCode::TemporaryRedirect, "Temporary Redirect");

  m_ResponseCodeLookup.insert(HttpResponseCode::BadRequest, "Bad Request");
  m_ResponseCodeLookup.insert(HttpResponseCode::Unauthorized, "Unauthorized");
  m_ResponseCodeLookup.insert(HttpResponseCode::PaymentRequired, "Payment Required");
  m_ResponseCodeLookup.insert(HttpResponseCode::Forbidden, "Forbidden");
  m_ResponseCodeLookup.insert(HttpResponseCode::NotFound, "Not Found");
  m_ResponseCodeLookup.insert(HttpResponseCode::MethodNotAllowed, "Method Not Allowed");
  m_ResponseCodeLookup.insert(HttpResponseCode::NotAcceptable, "Not Acceptable");
  m_ResponseCodeLookup.insert(HttpResponseCode::ProxyAuthenticationRequired, "Proxy Authentication Required");
  m_ResponseCodeLookup.insert(HttpResponseCode::RequestTimeout, "Request Time-out");
  m_ResponseCodeLookup.insert(HttpResponseCode::Conflict, "Conflict");
  m_ResponseCodeLookup.insert(HttpResponseCode::Gone, "Gone");
  m_ResponseCodeLookup.insert(HttpResponseCode::LengthRequired, "Length Required");
  m_ResponseCodeLookup.insert(HttpResponseCode::PreconditionFailed, "Precondition Failed");
  m_ResponseCodeLookup.insert(HttpResponseCode::RequestEntityTooLarge, "Request Entity Too Large");
  m_ResponseCodeLookup.insert(HttpResponseCode::RequestURITooLarge, "Request-URI Too Large");
  m_ResponseCodeLookup.insert(HttpResponseCode::UnsupportedMediaType, "Unsupported Media Type");
  m_ResponseCodeLookup.insert(HttpResponseCode::RequestedRangeNotSatisfiable, "Requested Range Not Satisfiable");
  m_ResponseCodeLookup.insert(HttpResponseCode::ExpectationFailed, "Expectation Failed");

  m_ResponseCodeLookup.insert(HttpResponseCode::InternalServerError, "Internal Server Error");
  m_ResponseCodeLookup.insert(HttpResponseCode::NotImplemented, "Not Implemented");
  m_ResponseCodeLookup.insert(HttpResponseCode::BadGateway, "Bad Gateway");
  m_ResponseCodeLookup.insert(HttpResponseCode::ServiceUnavailable, "Service Unavailable");
  m_ResponseCodeLookup.insert(HttpResponseCode::GatewayTimeout, "Gateway Time-out");
  m_ResponseCodeLookup.insert(HttpResponseCode::HTTPVersionNotSupported, "HTTP Version Not Supported");
}
