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

class HttpResponseGenerator : public QObject
{
  Q_OBJECT
public:
  HttpResponseGenerator(QObject* parent = nullptr);
  virtual ~HttpResponseGenerator();

  typedef QMultiMap<QString, QString> StringMultiMap;

  enum class HttpResponseCode : unsigned int
  {
    Unknown = 0,

    Continue = 100,
    SwitchingProtocols = 101,

    OK = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInformation = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,

    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    UseProxy = 305,
    TemporaryRedirect = 307,

    BadRequest = 400,
    Unauthorized = 401,
    PaymentRequired = 402,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    ProxyAuthenticationRequired = 407,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    LengthRequired = 411,
    PreconditionFailed = 412,
    RequestEntityTooLarge = 413,
    RequestURITooLarge = 414,
    UnsupportedMediaType = 415,
    RequestedRangeNotSatisfiable = 416,
    ExpectationFailed = 417,

    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504,
    HTTPVersionNotSupported = 505
  };

  SIMPL_SET_PROPERTY(QString, Version)
  SIMPL_SET_PROPERTY(HttpResponseCode, ResponseCode)

  /**
   * @brief Generates a response using the given response data.
   * @return
   */
  QString generateResponse();

  /**
   * @brief Clears the generator's member variables
   */
  void clearGenerator();

  /**
   * @brief Sets a header with a given name.
   * @param headerName The name of the header to set.
   * @param headerData The header data.
   * @return
   */
  void setHeader(const QString &headerName, const QString &headerData);

  /**
   * @brief Sets a parameter with a given name.
   * @param parameterName The name of the parameter to set.
   * @param parameterData The parameter data.
   * @return
   */
  void setParameter(const QString &parameterName, const QString &parameterData);

  /**
   * @brief Gets the body size for the projected response message
   * @return
   */
  int getBodySize();

private:
  /* Response HTTP protocol version */
  QString m_Version;

  /* Response HTTP code */
  HttpResponseCode m_ResponseCode;

  /* Boundary for multipart/form-data body. Empty if the message is not a multipart message */
  QString m_Boundary;

  /* Response HTTP body. Empty if the message is a multipart message */
  QString m_Body;

  /* Response Headers */
  StringMultiMap m_Headers;

  /* Response Parameters */
  StringMultiMap m_Parameters;

  /* The mapping between response codes and their descriptions */
  QMap<HttpResponseCode,QString> m_ResponseCodeLookup;

  /**
   * @brief Creates the lookup for every possible response code
   */
  void createResponseCodeLookup();
};
