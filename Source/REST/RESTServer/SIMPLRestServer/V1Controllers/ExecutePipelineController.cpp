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
#include "ExecutePipelineController.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QProcess>
#include <QtCore/QVariant>
#include <QtNetwork/QNetworkInterface>
#include <QtWidgets/QApplication>

#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/InputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Plugin/SIMPLPluginConstants.h"

#include "QtWebApp/httpserver/httplistener.h"
#include "QtWebApp/httpserver/httpsessionstore.h"
#include "SIMPLRestServer/PipelineListener.h"
#include "SIMPLRestServer/V1Controllers/SIMPLStaticFileController.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExecutePipelineController::ExecutePipelineController(const QHostAddress& hostAddress, const int hostPort)
{
  setListenHost(hostAddress, hostPort);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineController::service(HttpRequest& request, HttpResponse& response)
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
  if (jsonParseError.error != QJsonParseError::ParseError::NoError)
  {
    // Form Error response
    QJsonObject rootObj;
    rootObj[SIMPL::JSON::ErrorMessage] = tr("%1: JSON Request Parsing Error - %2").arg(EndPoint()).arg(jsonParseError.errorString());
    rootObj[SIMPL::JSON::ErrorCode] = -30;
    QJsonDocument jdoc(rootObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  QJsonObject requestObj = requestDoc.object();
  if (!requestObj.contains(SIMPL::JSON::Pipeline))
  {
    QJsonObject rootObj;
    rootObj[SIMPL::JSON::ErrorMessage] = tr("%1: No Pipeline object found in the JSON request body.").arg(EndPoint());
    rootObj[SIMPL::JSON::ErrorCode] = -40;
    QJsonDocument jdoc(rootObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  QJsonObject pipelineObj = requestObj[SIMPL::JSON::Pipeline].toObject();
  FilterPipeline::Pointer pipeline = FilterPipeline::FromJson(pipelineObj);
  if (pipeline.get() == nullptr)
  {
    QJsonObject rootObj;
    rootObj[SIMPL::JSON::ErrorMessage] = tr("%1: Pipeline could not be created from the JSON request body.").arg(EndPoint());
    rootObj[SIMPL::JSON::ErrorCode] = -50;
    QJsonDocument jdoc(rootObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  qDebug() << "Number of Filters in Pipeline: " << pipeline->size();

  QString linkAddress = "http://" + getListenHost().toString() + ":" + QString::number(getListenPort()) + QDir::separator() + QString(session.getId()) + QDir::separator();
  SIMPLStaticFileController* staticFileController = SIMPLStaticFileController::Instance();
  QString docRoot = staticFileController->getDocRoot();

  QString newFilePath = docRoot + QDir::separator() + QString(session.getId()) + QDir::separator();
  QJsonArray outputLinks;
  // Look through the pipeline to find any input or output filter parameters.  Replace
  // the file paths in these filter parameters with session-id specific paths.
  QList<AbstractFilter::Pointer> filters = pipeline->getFilterContainer();
  for(int i = 0; i < filters.size(); i++)
  {
    AbstractFilter::Pointer filter = filters[i];
    QVector<FilterParameter::Pointer> filterParams = filter->getFilterParameters();

    for(QVector<FilterParameter::Pointer>::iterator iter = filterParams.begin(); iter != filterParams.end(); ++iter)
    {
      FilterParameter* parameter = (*iter).get();
      OutputFileFilterParameter* outFileParam = dynamic_cast<OutputFileFilterParameter*>(parameter);
      OutputPathFilterParameter* outPathParam = dynamic_cast<OutputPathFilterParameter*>(parameter);
      InputFileFilterParameter* inFileParam = dynamic_cast<InputFileFilterParameter*>(parameter);
      InputPathFilterParameter* inPathParam = dynamic_cast<InputPathFilterParameter*>(parameter);

      if(outFileParam != nullptr)
      {
        QString existingPath = outFileParam->getGetterCallback()();
        outFileParam->getSetterCallback()(newFilePath + existingPath);
        outputLinks.append(linkAddress + existingPath);
      }
      else if(outPathParam != nullptr)
      {
        QString existingPath = outPathParam->getGetterCallback()();
        outPathParam->getSetterCallback()(newFilePath + existingPath);
        outputLinks.append(linkAddress + existingPath);
      }
      else if(inFileParam != nullptr)
      {
        QString existingPath = inFileParam->getGetterCallback()();
        inFileParam->getSetterCallback()(newFilePath + existingPath);
        outputLinks.append(linkAddress + existingPath);
      }
      else if(inPathParam != nullptr)
      {
        QString existingPath = inPathParam->getGetterCallback()();
        inPathParam->getSetterCallback()(newFilePath + existingPath);
        outputLinks.append(linkAddress + existingPath);
      }
    }
  }

  // Log Files
  PipelineListener listener(nullptr);
  bool createErrorLog = requestObj[SIMPL::JSON::ErrorLog].toBool(false);
  bool createWarningLog = requestObj[SIMPL::JSON::WarningLog].toBool(false);
  bool createStatusLog = requestObj[SIMPL::JSON::StatusLog].toBool(false);

  QDir docRootDir(docRoot);
  docRootDir.mkpath(newFilePath);

  if(createErrorLog)
  {
    QString filename = pipeline->getName() + "-err.log";
    QString filepath = newFilePath + QDir::separator() + filename;
    listener.createErrorLogFile(filepath);

    outputLinks.append(linkAddress + filename);
  }

  if(createWarningLog)
  {
    QString filename = pipeline->getName() + "-warning.log";
    QString filepath = newFilePath + QDir::separator() + filename;
    listener.createWarningLogFile(filepath);

    outputLinks.append(linkAddress + filename);
  }

  if(createStatusLog)
  {
    QString filename = pipeline->getName() + "-status.log";
    QString filepath = newFilePath + QDir::separator() + filename;
    listener.createStatusLogFile(filepath);

    outputLinks.append(linkAddress + filename);
  }

  // Append to the json response payload all the output links
  rootObj[SIMPL::JSON::OutputLinks] = outputLinks;

  // Execute the pipeline
  Observer obs; // Create an Observer to report errors/progress from the executing pipeline
  pipeline->addMessageReceiver(&obs);
  pipeline->addMessageReceiver(&listener);

  int err = pipeline->preflightPipeline();
  qDebug() << "Preflight Error: " << err;

  if (listener.getErrorMessages().size() <= 0)
  {
    qDebug() << "Pipeline About to Execute....";
    pipeline->execute();

    qDebug() << "Pipeline Done Executing...." << pipeline->getErrorCondition();
  }

  // Return messages
  std::vector<PipelineMessage> errorMessages = listener.getErrorMessages();
  bool completed = (errorMessages.size() == 0);
  if(!completed)
  {
    QJsonArray errors;
    size_t numErrors = errorMessages.size();
    for(size_t i = 0; i < numErrors; i++)
    {
      QJsonObject error;
      error[SIMPL::JSON::Code] = errorMessages[i].generateErrorString();
      error[SIMPL::JSON::Message] = errorMessages[i].getText();
      error[SIMPL::JSON::FilterHumanLabel] = errorMessages[i].getFilterHumanLabel();
      error[SIMPL::JSON::FilterIndex] = errorMessages[i].getPipelineIndex();

      errors.push_back(error);
    }
    rootObj[SIMPL::JSON::Errors] = errors;
  }

  std::vector<PipelineMessage> warningMessages = listener.getWarningMessages();
  QJsonArray warnings;
  size_t numWarnings = warningMessages.size();
  for(size_t i = 0; i < numWarnings; i++)
  {
    QJsonObject warning;
    warning[SIMPL::JSON::Code] = warningMessages[i].generateWarningString();
    warning[SIMPL::JSON::Message] = warningMessages[i].getText();
    warning[SIMPL::JSON::FilterHumanLabel] = warningMessages[i].getFilterHumanLabel();
    warning[SIMPL::JSON::FilterIndex] = warningMessages[i].getPipelineIndex();

    warnings.push_back(warning);
  }

  std::vector<PipelineMessage> statusMessages = listener.getStatusMessages();
  QJsonArray statusMsgs;
  size_t numStatusMsgs = statusMessages.size();
  for(size_t i = 0; i < numStatusMsgs; i++)
  {
    QJsonObject msg;
    msg[SIMPL::JSON::Message] = statusMessages[i].generateStatusString();
    statusMsgs.push_back(msg);
  }
  // rootObj["StatusMessages"] = statusMsgs;
  rootObj[SIMPL::JSON::Warnings] = warnings;
  rootObj[SIMPL::JSON::Completed] = completed;

  // **************************************************************************
  // This section archives the working directory for this session
  QProcess tar;
  tar.setWorkingDirectory(docRoot);
  std::cout << "Working Directory from Process: " << tar.workingDirectory().toStdString() << std::endl;
  tar.start("/usr/bin/tar", QStringList() << "-cvf" << QString(session.getId() + ".tar.gz") << QString(session.getId()));
  tar.waitForStarted();
  tar.waitForFinished();

  QByteArray result = tar.readAllStandardError();
  std::cout << result.data() << std::endl;
  result = tar.readAllStandardOutput();
  std::cout << result.data() << std::endl;

  outputLinks.append("http://" + getListenHost().toString() + ":8080" + QDir::separator() + QString(session.getId()) + ".tar.gz");
  // **************************************************************************

  // Append to the json response payload all the output links
  rootObj[SIMPL::JSON::OutputLinks] = outputLinks;

  QJsonDocument jdoc(rootObj);
  response.write(jdoc.toJson(), true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecutePipelineController::EndPoint()
{
  return QString("ExecutePipeline");
}
