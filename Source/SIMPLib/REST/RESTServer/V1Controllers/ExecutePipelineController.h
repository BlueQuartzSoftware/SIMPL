/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Softwae, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef ExecutePipelineController_H_
#define ExecutePipelineController_H_

#include <QtCore/QJsonObject>
#include <QtCore/QTemporaryDir>
#include <QtCore/QTemporaryFile>

#include "QtWebApp/httpserver/httprequest.h"
#include "QtWebApp/httpserver/httprequesthandler.h"
#include "QtWebApp/httpserver/httpresponse.h"

/**
  @brief This class responds to REST API endpoint
*/

class ExecutePipelineController : public HttpRequestHandler
{
  Q_OBJECT
  Q_DISABLE_COPY(ExecutePipelineController)
public:
  /** Constructor */
  ExecutePipelineController(const QHostAddress& hostAddress, const int hostPort);

  /** Generates the response */
  void service(HttpRequest& request, HttpResponse& response);

  /**
   * @brief Returns the name of the end point that is controller uses
   * @return
   */
  static QString EndPoint();

private:
  HttpRequest* m_Request = nullptr;
  HttpResponse* m_Response = nullptr;
  QJsonObject m_ResponseObj;

  QTemporaryDir* m_TempDir = nullptr;       // We need this to keep the temporary directories around until the pipeline is done executing
  QStringList m_OutputFilePaths;
  QStringList m_TemporaryOutputFilePaths;

  void cleanup();

  void serviceJSON(QJsonObject pipelineObj);
  void serviceJSON();

  // Functions that process multi-part requests
  void serviceMultiPart();

  QJsonObject getPipelineMetadata();
  QString getStringValue(const QString &key, QJsonObject pipelineReplacementObj);
  QJsonArray getFileParameterNames(QJsonObject pipelineReplacementObj);

  QJsonObject replacePipelineValuesUsingMetadata(QJsonObject pipelineJsonObj, QJsonObject pipelineMetadataObject);

};

#endif // ExecutePipelineController_H_
