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

#pragma once

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

class HttpResponseParseError
{
  public:
    friend class HttpResponseParser;

    enum class ParseError : unsigned int
    {
      NoError,
      InvalidStartLine,
      InvalidParameter
    };

    QString errorString() const
    {
      return m_ErrorString;
    }

    ParseError error() const
    {
      return m_Error;
    }

  private:
    ParseError m_Error = ParseError::NoError;
    QString m_ErrorString = "";
};

class HttpResponseParser : public QObject
{
  Q_OBJECT
public:
  HttpResponseParser(QObject* parent = nullptr);
  virtual ~HttpResponseParser();

  typedef QMultiMap<QString, QString> StringMultiMap;

  SIMPL_GET_PROPERTY(QString, HttpResponseData)
  SIMPL_GET_PROPERTY(QString, Version)
  SIMPL_GET_PROPERTY(QString, ResponseCode)
  SIMPL_GET_PROPERTY(QString, ResponseCodeDescription)
  SIMPL_GET_PROPERTY(QString, Boundary)
  SIMPL_GET_PROPERTY(StringMultiMap, Headers)
  SIMPL_GET_PROPERTY(StringMultiMap, Parameters)

  /**
   * @brief Parses the given response data and fills all the private variables in this class with the data found.  The
   * HttpResponseParseError object can be checked to see if the parsing process was successful or not.
   * @param httpResponseData
   * @return
   */
  void parseResponse(QString httpResponseData, HttpResponseParseError* error = nullptr);

  /**
   * @brief Clears the parser's member variables
   */
  void clearParser();

  /**
   * @brief Retrieves a header with a given name.  Returns an empty string if the header with the given name does not exist.
   * @param headerName The name of the header to retrieve.
   * @return
   */
  QString getHeader(const QString &headerName);

  /**
   * @brief Retrieves a parameter with a given name.  Returns an empty string if the parameter with the given name does not exist.
   * @param parameterName The name of the parameter to retrieve.
   * @return
   */
  QString getParameter(const QString &parameterName);

private:
  /* Entire Response */
  QString m_HttpResponseData;

  /* Response HTTP protocol version */
  QString m_Version;

  /* Response HTTP code */
  QString m_ResponseCode;

  /* Response HTTP code description */
  QString m_ResponseCodeDescription;

  /* Boundary of multipart/form-data body. Empty if there is no such header */
  QString m_Boundary;

  /* Response Headers */
  StringMultiMap m_Headers;

  /* Response Parameters */
  StringMultiMap m_Parameters;

  /* Response Body */
  QString m_Body;

  void parseStartLine(QString startLine, HttpResponseParseError* error);
  void parseHeaders(QStringList headerDataList, HttpResponseParseError* error);
  void parseBody(QString bodyData, HttpResponseParseError* error);
};
