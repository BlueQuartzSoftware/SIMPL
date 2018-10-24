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

#include "HttpResponseParser.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpResponseParser::HttpResponseParser(QObject* parent)
: QObject(parent)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpResponseParser::~HttpResponseParser()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpResponseParser::parseResponse(QString httpResponseData, HttpResponseParseError* error)
{
  QTextStream textStream(&httpResponseData);

  QString line = textStream.readLine();

  parseStartLine(line, error);
  if (error->error() != HttpResponseParseError::ParseError::NoError)
  {
    clearParser();
    return;
  }

  QStringList headers;
  line = textStream.readLine();
  while (line.contains(":") || line.isEmpty())
  {
    if (line.contains(":"))
    {
      headers.push_back(line);
    }
    line = textStream.readLine();
  }

  parseHeaders(headers, error);
  if (error->error() != HttpResponseParseError::ParseError::NoError)
  {
    clearParser();
    return;
  }

  QString bodyData = textStream.read(httpResponseData.size());

  parseBody(bodyData, error);
  if (error->error() != HttpResponseParseError::ParseError::NoError)
  {
    clearParser();
    return;
  }

  m_HttpResponseData = httpResponseData;
  m_Body = bodyData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpResponseParser::parseStartLine(QString startLine, HttpResponseParseError* error)
{
  QStringList list = startLine.split(' ');
  if(list.count() != 3)
  {
    error->m_Error = HttpResponseParseError::ParseError::InvalidStartLine;
    error->m_ErrorString = "The first line is invalid because there are not 3 space-separated strings.";
    return;
  }
  else if (!list.at(0).contains("HTTP"))
  {
    error->m_Error = HttpResponseParseError::ParseError::InvalidStartLine;
    error->m_ErrorString = "The first line is invalid because the term 'HTTP' could not be found.";
    return;
  }
  else
  {
    m_Version = list.at(0);
    m_ResponseCode = list.at(1);
    m_ResponseCodeDescription = list.at(2);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpResponseParser::parseHeaders(QStringList headerDataList, HttpResponseParseError* error)
{
  QString boundaryLabel = "boundary=";
  for (int i = 0; i < headerDataList.size(); i++)
  {
    QString headerData = headerDataList[i];

    int colonIndex = headerData.indexOf(':');
    QString headerName = headerData.left(colonIndex).trimmed();
    QString header = headerData.right(headerData.size() - colonIndex - 1).trimmed();

    if (header.contains(boundaryLabel))
    {
      int boundaryIndex = header.indexOf(boundaryLabel);
      QString boundary = header.mid(boundaryIndex + boundaryLabel.size());
      if (boundary.startsWith("\""))
      {
        boundary.remove(0, 1);
      }
      if (boundary.endsWith("\""))
      {
        boundary.chop(1);
      }

      m_Boundary = boundary;

      header = header.left(boundaryIndex).trimmed();
      if (header.endsWith(';'))
      {
        header.chop(1);
      }
    }

    m_Headers.insert(headerName, header);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpResponseParser::parseBody(QString bodyData, HttpResponseParseError* error)
{
  if (!m_Boundary.isEmpty())
  {
    QStringList parameterDataList = bodyData.split(tr("--%1").arg(m_Boundary), QString::SplitBehavior::SkipEmptyParts);
    QString parameterNameLabel = "name=";
    for (int i = 0; i < parameterDataList.size(); i++)
    {
      QString parameterData = parameterDataList[i];
      QTextStream textStream(&parameterData);
      QString parameterName = "";
      QString parameterValue = "";

      while (!textStream.atEnd() && parameterName.isEmpty())
      {
        QString line = textStream.readLine();

        if (line.isEmpty())
        {
          continue;
        }
        else if (!line.startsWith("Content-Disposition:"))
        {
          error->m_Error = HttpResponseParseError::ParseError::InvalidParameter;
          error->m_ErrorString = tr("Parameter %1 in the HTTP response does not start with Content-Disposition.").arg(i+1);
          clearParser();
          return;
        }
        else if (!line.contains("form-data"))
        {
          error->m_Error = HttpResponseParseError::ParseError::InvalidParameter;
          error->m_ErrorString = tr("Parameter %1 in the HTTP response does not list form-data as the Content Disposition.").arg(i+1);
          clearParser();
          return;
        }
        else if (!line.contains("name="))
        {
          error->m_Error = HttpResponseParseError::ParseError::InvalidParameter;
          error->m_ErrorString = tr("Parameter %1 in the HTTP response is missing its name.").arg(i+1);
          clearParser();
          return;
        }
        else
        {
          int nameLabelIndex = line.indexOf(parameterNameLabel);
          parameterName = line.right(line.size() - nameLabelIndex - parameterNameLabel.size());
          if (parameterName.startsWith("\""))
          {
            parameterName.remove(0, 1);
          }
          if (parameterName.endsWith("\""))
          {
            parameterName.chop(1);
          }
        }
      }

      // Read past any empty lines between the part header and the content
      parameterValue = textStream.readLine();
      while (parameterValue.isEmpty())
      {
        parameterValue = textStream.readLine();
      }

      parameterValue.append(textStream.read(parameterData.size()));

      m_Parameters.insert(parameterName, parameterValue);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HttpResponseParser::getHeader(const QString &headerName)
{
  QString header = m_Headers.value(headerName);
  return header;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HttpResponseParser::getParameter(const QString &parameterName)
{
  QString parameter = m_Parameters.value(parameterName);
  return parameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpResponseParser::clearParser()
{
  m_HttpResponseData.clear();
  m_Version.clear();
  m_ResponseCode.clear();
  m_ResponseCodeDescription.clear();
  m_Boundary.clear();
  m_Headers.clear();
  m_Parameters.clear();
}
