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
#include <QtCore/QTemporaryDir>
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
#include "REST/RESTServer/PipelineListener.h"
#include "SIMPLStaticFileController.h"

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
void ExecutePipelineController::serviceJSON(HttpResponse& response, QJsonObject pipelineObj, QJsonObject &responseObj)
{
  if (!pipelineObj.contains(SIMPL::JSON::Pipeline))
  {
    responseObj[SIMPL::JSON::ErrorMessage] = tr("%1: No Pipeline object found in the JSON request body.").arg(EndPoint());
    responseObj[SIMPL::JSON::ErrorCode] = -60;
    QJsonDocument jdoc(responseObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  pipelineObj = pipelineObj[SIMPL::JSON::Pipeline].toObject();
  FilterPipeline::Pointer pipeline = FilterPipeline::FromJson(pipelineObj);
  if (pipeline.get() == nullptr)
  {
    responseObj[SIMPL::JSON::ErrorMessage] = tr("%1: Pipeline could not be created from the JSON request body.").arg(EndPoint());
    responseObj[SIMPL::JSON::ErrorCode] = -70;
    QJsonDocument jdoc(responseObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  qDebug() << "Number of Filters in Pipeline: " << pipeline->size();

  QByteArray sessionId = responseObj[SIMPL::JSON::SessionID].toVariant().toByteArray();

  QString linkAddress = "http://" + getListenHost().toString() + ":" + QString::number(getListenPort()) + QDir::separator() + QString(sessionId) + QDir::separator();
  SIMPLStaticFileController* staticFileController = SIMPLStaticFileController::Instance();
  QString docRoot = staticFileController->getDocRoot();

  QString newFilePath = docRoot + QDir::separator() + QString(sessionId) + QDir::separator();
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
  bool createErrorLog = pipelineObj[SIMPL::JSON::ErrorLog].toBool(false);
  bool createWarningLog = pipelineObj[SIMPL::JSON::WarningLog].toBool(false);
  bool createStatusLog = pipelineObj[SIMPL::JSON::StatusLog].toBool(false);

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
  responseObj[SIMPL::JSON::OutputLinks] = outputLinks;

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
  responseObj[SIMPL::JSON::Errors] = errors;

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
  // responseObj["StatusMessages"] = statusMsgs;
  responseObj[SIMPL::JSON::Warnings] = warnings;
  responseObj[SIMPL::JSON::Completed] = completed;

  // **************************************************************************
  // This section archives the working directory for this session
  QProcess tar;
  tar.setWorkingDirectory(docRoot);
  std::cout << "Working Directory from Process: " << tar.workingDirectory().toStdString() << std::endl;
  tar.start("/usr/bin/tar", QStringList() << "-cvf" << QString(sessionId + ".tar.gz") << QString(sessionId));
  tar.waitForStarted();
  tar.waitForFinished();

  QByteArray result = tar.readAllStandardError();
  std::cout << result.data() << std::endl;
  result = tar.readAllStandardOutput();
  std::cout << result.data() << std::endl;

  outputLinks.append("http://" + getListenHost().toString() + ":8080" + QDir::separator() + QString(sessionId) + ".tar.gz");
  // **************************************************************************

  // Append to the json response payload all the output links
  responseObj[SIMPL::JSON::OutputLinks] = outputLinks;

  QJsonDocument jdoc(responseObj);
  response.write(jdoc.toJson(), true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineController::serviceJSON(HttpRequest& request, HttpResponse &response, QJsonObject &responseObj)
{
  QJsonParseError jsonParseError;
  QString requestBody = request.getBody();
  QJsonDocument requestDoc = QJsonDocument::fromJson(requestBody.toUtf8(), &jsonParseError);
  if (jsonParseError.error != QJsonParseError::ParseError::NoError)
  {
    // Form Error response
    responseObj[SIMPL::JSON::ErrorMessage] = tr("%1: JSON Request Parsing Error - %2").arg(EndPoint()).arg(jsonParseError.errorString());
    responseObj[SIMPL::JSON::ErrorCode] = -30;
    QJsonDocument jdoc(responseObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  QJsonObject requestObj = requestDoc.object();

  serviceJSON(response, requestObj, responseObj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineController::serviceMultiPart(HttpRequest& request, HttpResponse &response, QJsonObject &responseObj)
{
  QJsonObject pipelineReplacementLookupObject = getPipelineReplacementLookupObject(request, response, responseObj);
  if (responseObj[SIMPL::JSON::ErrorCode].toInt() < 0)
  {
    return;
  }

  QByteArray jsonData = request.getParameter("json");
  QString pipelineString(jsonData);

  std::vector<QTemporaryDir*> tempDirs;       // We need this to keep the temporary directories around until the pipeline is done executing
  std::vector<QTemporaryFile*> tempFiles;     // We need this to keep the temporary files around until the pipeline is done executing

  for(QJsonObject::iterator iter = pipelineReplacementLookupObject.begin(); iter != pipelineReplacementLookupObject.end(); iter++)
  {
    QString pipelineReplacementString = iter.key();

    if (!iter.value().isObject())
    {
      responseObj[SIMPL::JSON::ErrorMessage] = EndPoint() + ": Pipeline replacement lookup is not in the right format: top-level value is not an object.";
      responseObj[SIMPL::JSON::ErrorCode] = -60;
      QJsonDocument jdoc(responseObj);

      response.write(jdoc.toJson(), true);
      return;
    }

    QJsonObject pipelineReplacementObj = iter.value().toObject();

    QJsonArray fileParameterNames = getFileParameterNames(pipelineReplacementObj, response, responseObj);
    if (responseObj[SIMPL::JSON::ErrorCode].toInt() < 0)
    {
      return;
    }

    QString typeString = getType(pipelineReplacementObj, response, responseObj);
    if (responseObj[SIMPL::JSON::ErrorCode].toInt() < 0)
    {
      return;
    }

    QString replacementToken = "";
    if (typeString == "Directory")
    {
      QTemporaryDir* tempDir = new QTemporaryDir();
      tempDirs.push_back(tempDir);
      replacementToken = tempDir->path();
    }

    for (int i = 0; i < fileParameterNames.size(); i++)
    {
      if (!fileParameterNames[i].isString())
      {
        responseObj[SIMPL::JSON::ErrorMessage] = tr("%1: Pipeline replacement lookup is not in the right format: FileParameterName at index %2 in top-level object"
                                                              " is not a string.").arg(EndPoint()).arg(i+1);
        responseObj[SIMPL::JSON::ErrorCode] = -80;
        QJsonDocument jdoc(responseObj);

        response.write(jdoc.toJson(), true);
        return;
      }

      QString fileParameterName = fileParameterNames[i].toString();
      QByteArray rawData = request.getParameter(QByteArray::fromStdString(fileParameterName.toStdString()));

      QTemporaryFile* tempFile = new QTemporaryFile(replacementToken);

      tempFiles.push_back(tempFile);
      if (tempFile->open())
      {
        if (replacementToken.isEmpty())
        {
          replacementToken = tempFile->fileName();
        }

        tempFile->write(rawData);
        tempFile->close();
      }
      else
      {
        // Form Error response
        responseObj[SIMPL::JSON::ErrorMessage] = EndPoint() + ": File data included in the request could not be written to a temporary file on the server.";
        responseObj[SIMPL::JSON::ErrorCode] = -90;
        QJsonDocument jdoc(responseObj);

        response.write(jdoc.toJson(), true);
        return;
      }
    }

    pipelineString.replace(pipelineReplacementString, replacementToken);
  }

  QJsonParseError jsonParseError;
  QJsonDocument pipelineDoc = QJsonDocument::fromJson(QByteArray::fromStdString(pipelineString.toStdString()), &jsonParseError);
  if (jsonParseError.error != QJsonParseError::ParseError::NoError)
  {
    // Form Error response
    responseObj[SIMPL::JSON::ErrorMessage] = tr("%1: JSON Request Parsing Error - %2").arg(EndPoint()).arg(jsonParseError.errorString());
    responseObj[SIMPL::JSON::ErrorCode] = -30;
    QJsonDocument jdoc(responseObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  QJsonObject pipelineObj = pipelineDoc.object();

  serviceJSON(response, pipelineObj, responseObj);

  for (int i = 0; i < tempFiles.size(); i++)
  {
    delete tempFiles[i];
  }

  for (int i = 0; i < tempDirs.size(); i++)
  {
    delete tempDirs[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecutePipelineController::getType(QJsonObject pipelineReplacementObj, HttpResponse &response, QJsonObject &responseObj)
{
  if (!pipelineReplacementObj.contains("Type"))
  {
    responseObj[SIMPL::JSON::ErrorMessage] = EndPoint() + ": Pipeline replacement lookup is not in the right format: Top-level object does not contain 'Type' value.";
    responseObj[SIMPL::JSON::ErrorCode] = -70;
    QJsonDocument jdoc(responseObj);

    response.write(jdoc.toJson(), true);
    return QString();
  }
  else if (!pipelineReplacementObj["Type"].isString())
  {
    responseObj[SIMPL::JSON::ErrorMessage] = EndPoint() + ": Pipeline replacement lookup is not in the right format: The 'Type' value in a top-level object"
                                                          " is not a string.";
    responseObj[SIMPL::JSON::ErrorCode] = -70;
    QJsonDocument jdoc(responseObj);

    response.write(jdoc.toJson(), true);
    return QString();
  }

  QString typeString = pipelineReplacementObj["Type"].toString();
  return typeString;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonArray ExecutePipelineController::getFileParameterNames(QJsonObject pipelineReplacementObj, HttpResponse &response, QJsonObject &responseObj)
{
  if (!pipelineReplacementObj.contains("FileParameterNames"))
  {
    responseObj[SIMPL::JSON::ErrorMessage] = EndPoint() + ": Pipeline replacement lookup is not in the right format: Top-level object does not contain 'FileParameterNames' value.";
    responseObj[SIMPL::JSON::ErrorCode] = -70;
    QJsonDocument jdoc(responseObj);

    response.write(jdoc.toJson(), true);
    return QJsonArray();
  }
  else if (!pipelineReplacementObj["FileParameterNames"].isArray())
  {
    responseObj[SIMPL::JSON::ErrorMessage] = EndPoint() + ": Pipeline replacement lookup is not in the right format: The 'FileParameterNames' value in a top-level object"
                                                          " is not an array.";
    responseObj[SIMPL::JSON::ErrorCode] = -70;
    QJsonDocument jdoc(responseObj);

    response.write(jdoc.toJson(), true);
    return QJsonArray();
  }

  QJsonArray fileParameterNames = pipelineReplacementObj["FileParameterNames"].toArray();
  if (fileParameterNames.size() <= 0)
  {
    responseObj[SIMPL::JSON::ErrorMessage] = EndPoint() + ": Pipeline replacement lookup is not in the right format: The 'FileParameterNames' value in a top-level object"
                                                          " is empty.";
    responseObj[SIMPL::JSON::ErrorCode] = -80;
    QJsonDocument jdoc(responseObj);

    response.write(jdoc.toJson(), true);
    return QJsonArray();
  }

  return fileParameterNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject ExecutePipelineController::getPipelineReplacementLookupObject(HttpRequest& request, HttpResponse &response, QJsonObject &responseObj)
{
  QJsonParseError jsonParseError;
  QByteArray jsonData = request.getParameter("pipelineReplacementLookup");
  QJsonDocument pipelineReplacementLookupDoc = QJsonDocument::fromJson(jsonData, &jsonParseError);
  if (jsonParseError.error != QJsonParseError::ParseError::NoError)
  {
    // Form Error response
    responseObj[SIMPL::JSON::ErrorMessage] = tr("%1: JSON Request Parsing Error - %2").arg(EndPoint()).arg(jsonParseError.errorString());
    responseObj[SIMPL::JSON::ErrorCode] = -30;
    QJsonDocument jdoc(responseObj);
    response.write(jdoc.toJson(), true);
    return QJsonObject();
  }

  QJsonObject pipelineReplacementLookupObj = pipelineReplacementLookupDoc.object();
  return pipelineReplacementLookupObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineController::service(HttpRequest& request, HttpResponse& response)
{
  // Get current session, or create a new one
  HttpSessionStore* sessionStore = HttpSessionStore::Instance();
  HttpSession session = sessionStore->getSession(request, response);

  QJsonObject responseObj;
  responseObj[SIMPL::JSON::SessionID] = QString(session.getId());
  responseObj[SIMPL::JSON::ErrorCode] = 0;
  responseObj[SIMPL::JSON::ErrorMessage] = "";
  response.setHeader("Content-Type", "application/json");

  QString content_type = request.getHeader(QByteArray("content-type"));
  if (content_type == "application/json")
  {
    serviceJSON(request, response, responseObj);
  }
  else if (content_type.startsWith("multipart/form-data"))
  {
    serviceMultiPart(request, response, responseObj);
  }
  else
  {
    // Form Error response
    responseObj[SIMPL::JSON::ErrorMessage] = EndPoint() + ": Content Type is not application/json";
    responseObj[SIMPL::JSON::ErrorCode] = -50;
    QJsonDocument jdoc(responseObj);

    response.write(jdoc.toJson(), true);
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecutePipelineController::EndPoint()
{
  return QString("ExecutePipeline");
}
