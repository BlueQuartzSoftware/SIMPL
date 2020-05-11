/* ============================================================================
 * Copyright (c) 2017-2019 BlueQuartz Software, LLC
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

#include "PreflightPipelineController.h"

#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLPluginConstants.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#include "QtWebApp/httpserver/httplistener.h"
#include "QtWebApp/httpserver/httpsessionstore.h"

#include "SIMPLib/REST/PipelineListener.h"
#include "SIMPLib/REST/V1Controllers/SIMPLStaticFileController.h"
#include "SIMPLib/REST/V1Controllers/PreflightPipelineMessageHandler.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreflightPipelineController::PreflightPipelineController(const QHostAddress& hostAddress, const int hostPort)
{
  setListenHost(hostAddress, hostPort);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreflightPipelineController::service(HttpRequest& request, HttpResponse& response)
{
  // Get current session, or create a new one
  HttpSessionStore* sessionStore = HttpSessionStore::Instance();
  HttpSession session = sessionStore->getSession(request, response);

  QString content_type = request.getHeader(QByteArray("content-type"));

  QJsonObject rootObj;
  rootObj[SIMPL::JSON::SessionID] = QString(session.getId());

  response.setHeader("Content-Type", "application/json");

  if(content_type.compare("application/json") != 0)
  {
    // Form Error response
    rootObj[SIMPL::JSON::ErrorMessage] = EndPoint() + ": Content Type is not application/json";
    rootObj[SIMPL::JSON::ErrorCode] = -20;
    QJsonDocument jdoc(rootObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  QJsonParseError jsonParseError;
  QString requestBody = request.getBody();
  QJsonDocument requestDoc = QJsonDocument::fromJson(requestBody.toUtf8(), &jsonParseError);
  if(jsonParseError.error != QJsonParseError::ParseError::NoError)
  {
    // Form Error response
    rootObj[SIMPL::JSON::ErrorMessage] = tr("%1: JSON Request Parsing Error - %2").arg(EndPoint()).arg(jsonParseError.errorString());
    rootObj[SIMPL::JSON::ErrorCode] = -30;
    QJsonDocument jdoc(rootObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  //  QString linkAddress = "http://" + getListenHost().toString() + ":" + QString::number(getListenPort()) + QDir::separator() + QString(session.getId()) + QDir::separator();
  //  SIMPLStaticFileController* staticFileController = SIMPLStaticFileController::Instance();
  //  QString docRoot = staticFileController->getDocRoot();
  //  QString newFilePath = docRoot + QDir::separator() + QString(session.getId()) + QDir::separator();
  //  QJsonArray outputLinks;

  QJsonObject requestObj = requestDoc.object();
  if(!requestObj.contains(SIMPL::JSON::Pipeline))
  {
    rootObj[SIMPL::JSON::ErrorMessage] = tr("%1: No Pipeline object found in the JSON request body.").arg(EndPoint());
    rootObj[SIMPL::JSON::ErrorCode] = -40;
    QJsonDocument jdoc(rootObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  // Pipeline
  QJsonObject pipelineObj = requestObj[SIMPL::JSON::Pipeline].toObject();
  FilterPipeline::Pointer pipeline = FilterPipeline::FromJson(pipelineObj);
  if(pipeline.get() == nullptr)
  {
    rootObj[SIMPL::JSON::ErrorMessage] = tr("%1: Pipeline could not be created from the JSON request body.").arg(EndPoint());
    rootObj[SIMPL::JSON::ErrorCode] = -50;
    QJsonDocument jdoc(rootObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  PipelineListener listener(nullptr);
  pipeline->addMessageReceiver(&listener);

  //  // Log Files
  //  bool createErrorLog = requestObj[SIMPL::JSON::ErrorLog].toBool(false);
  //  bool createWarningLog = requestObj[SIMPL::JSON::WarningLog].toBool(false);
  //  bool createStatusLog = requestObj[SIMPL::JSON::StatusLog].toBool(false);

  //  QDir docRootDir(docRoot);
  //  docRootDir.mkpath(newFilePath);

  //  if(createErrorLog)
  //  {
  //    QString filename = pipeline->getName() + "-err.log";
  //    QString filepath = newFilePath + QDir::separator() + filename;
  //    listener.createErrorLogFile(filepath);

  //    outputLinks.append(linkAddress + filename);
  //  }

  //  if(createWarningLog)
  //  {
  //    QString filename = pipeline->getName() + "-warning.log";
  //    QString filepath = newFilePath + QDir::separator() + filename;
  //    listener.createWarningLogFile(filepath);

  //    outputLinks.append(linkAddress + filename);
  //  }

  //  if(createStatusLog)
  //  {
  //    QString filename = pipeline->getName() + "-status.log";
  //    QString filepath = newFilePath + QDir::separator() + filename;
  //    listener.createStatusLogFile(filepath);

  //    outputLinks.append(linkAddress + filename);
  //  }

  //  // Append to the json response payload all the output links
  //  rootObj[SIMPL::JSON::OutputLinks] = outputLinks;

  // Preflight
  pipeline->preflightPipeline();

  //   response.setCookie(HttpCookie("firstCookie","hello",600,QByteArray(),QByteArray(),QByteArray(),false,true));
  //   response.setCookie(HttpCookie("secondCookie","world",600));

  QJsonArray errors;
  QJsonArray warnings;

  std::vector<const AbstractErrorMessage*> errorMessages = listener.getErrorMessages();
  bool completed = (errorMessages.size() == 0);
  rootObj[SIMPL::JSON::Completed] = completed;

  std::vector<const AbstractMessage*> allMessages = listener.getAllMessages();
  for(const AbstractMessage* msg : allMessages)
  {
    PreflightPipelineMessageHandler msgHandler(&errors, &warnings);
    msg->visit(&msgHandler);
  }

  rootObj[SIMPL::JSON::PipelineErrors] = errors;
  rootObj[SIMPL::JSON::PipelineWarnings] = warnings;

  QJsonDocument jdoc(rootObj);

  response.write(jdoc.toJson(), true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PreflightPipelineController::EndPoint()
{
  return QString("PreflightPipeline");
}
