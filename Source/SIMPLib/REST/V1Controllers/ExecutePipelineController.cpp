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
#include "ExecutePipelineController.h"

#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QProcess>
#include <QtCore/QDebug>

#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/InputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLPluginConstants.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#include "QtWebApp/httpserver/httplistener.h"
#include "QtWebApp/httpserver/httpsessionstore.h"
#include "SIMPLStaticFileController.h"
#include "SIMPLib/REST/PipelineListener.h"
#include "SIMPLib/REST/V1Controllers/ExecutePipelineMessageHandler.h"

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
void ExecutePipelineController::serviceJSON(QJsonObject pipelineObj)
{
  FilterPipeline::Pointer pipeline = FilterPipeline::FromJson(pipelineObj);
  if(pipeline.get() == nullptr)
  {
    QString errMsg = tr("%1: Pipeline object could not be created from the provided JSON pipeline data.").arg(EndPoint());
    sendErrorResponse(HttpResponse::HttpStatusCode::BadRequest, errMsg, -50);
    return;
  }

  qDebug() << "Number of Filters in Pipeline: " << pipeline->size();

  //  QByteArray sessionId = m_ResponseObj[SIMPL::JSON::SessionID].toVariant().toByteArray();

  //  QString linkAddress = "http://" + getListenHost().toString() + ":" + QString::number(getListenPort()) + QDir::separator() + QString(sessionId) + QDir::separator();
  //  SIMPLStaticFileController* staticFileController = SIMPLStaticFileController::Instance();
  //  QString docRoot = staticFileController->getDocRoot();

  //  QString newFilePath = docRoot + QDir::separator() + QString(sessionId) + QDir::separator();
  //  QJsonArray outputLinks;
  //  // Look through the pipeline to find any input or output filter parameters.  Replace
  //  // the file paths in these filter parameters with session-id specific paths.
  //  QList<AbstractFilter::Pointer> filters = pipeline->getFilterContainer();
  //  for(int i = 0; i < filters.size(); i++)
  //  {
  //    AbstractFilter::Pointer filter = filters[i];
  //    FilterParameterVectorType filterParams = filter->getFilterParameters();

  //    for(FilterParameterVectorType::iterator iter = filterParams.begin(); iter != filterParams.end(); ++iter)
  //    {
  //      FilterParameter* parameter = (*iter).get();
  //      OutputFileFilterParameter* outFileParam = dynamic_cast<OutputFileFilterParameter*>(parameter);
  //      OutputPathFilterParameter* outPathParam = dynamic_cast<OutputPathFilterParameter*>(parameter);
  //      InputFileFilterParameter* inFileParam = dynamic_cast<InputFileFilterParameter*>(parameter);
  //      InputPathFilterParameter* inPathParam = dynamic_cast<InputPathFilterParameter*>(parameter);

  //      if(outFileParam != nullptr)
  //      {
  //        QString existingPath = outFileParam->getGetterCallback()();
  //        outFileParam->getSetterCallback()(newFilePath + existingPath);
  //        outputLinks.append(linkAddress + existingPath);
  //      }
  //      else if(outPathParam != nullptr)
  //      {
  //        QString existingPath = outPathParam->getGetterCallback()();
  //        outPathParam->getSetterCallback()(newFilePath + existingPath);
  //        outputLinks.append(linkAddress + existingPath);
  //      }
  //      else if(inFileParam != nullptr)
  //      {
  //        QString existingPath = inFileParam->getGetterCallback()();
  //        inFileParam->getSetterCallback()(newFilePath + existingPath);
  //        outputLinks.append(linkAddress + existingPath);
  //      }
  //      else if(inPathParam != nullptr)
  //      {
  //        QString existingPath = inPathParam->getGetterCallback()();
  //        inPathParam->getSetterCallback()(newFilePath + existingPath);
  //        outputLinks.append(linkAddress + existingPath);
  //      }
  //    }
  //  }

  //  // Log Files
  PipelineListener listener(nullptr);
  //  bool createErrorLog = pipelineObj[SIMPL::JSON::ErrorLog].toBool(false);
  //  bool createWarningLog = pipelineObj[SIMPL::JSON::WarningLog].toBool(false);
  //  bool createStatusLog = pipelineObj[SIMPL::JSON::StatusLog].toBool(false);

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
  //  if(outputLinks.isEmpty() == false)
  //  {
  //    m_ResponseObj[SIMPL::JSON::OutputLinks] = outputLinks;
  //  }

  // Execute the pipeline
  Observer obs; // Create an Observer to report errors/progress from the executing pipeline
  pipeline->addMessageReceiver(&obs);
  pipeline->addMessageReceiver(&listener);

  int err = pipeline->preflightPipeline();
  qDebug() << "Preflight Error: " << err;

  if(listener.getErrorMessages().size() <= 0)
  {
    qDebug() << "Pipeline About to Execute....";
    pipeline->execute();

    qDebug() << "Pipeline Done Executing...." << pipeline->getErrorCode();
  }

  // Return messages
  QJsonArray errors;
  QJsonArray warnings;
  QJsonArray statusMsgs;

  std::vector<const AbstractErrorMessage*> errorMessages = listener.getErrorMessages();
  bool completed = (errorMessages.size() == 0);
  m_ResponseObj[SIMPL::JSON::Completed] = completed;

  std::vector<const AbstractMessage*> allMessages = listener.getAllMessages();
  for(const AbstractMessage* msg : allMessages)
  {
    ExecutePipelineMessageHandler msgHandler(&errors, &warnings);
    msg->visit(&msgHandler);
  }

  m_ResponseObj[SIMPL::JSON::PipelineErrors] = errors;
  m_ResponseObj[SIMPL::JSON::PipelineWarnings] = warnings;
  // m_ResponseObj["StatusMessages"] = statusMsgs;

  //  // **************************************************************************
  //  // This section archives the working directory for this session
  //  QProcess tar;
  //  tar.setWorkingDirectory(docRoot);
  //  std::cout << "Working Directory from Process: " << tar.workingDirectory().toStdString() << std::endl;
  //  tar.start("/usr/bin/tar", QStringList() << "-cvf" << QString(sessionId + ".tar.gz") << QString(sessionId));
  //  tar.waitForStarted();
  //  tar.waitForFinished();

  //  QByteArray result = tar.readAllStandardError();
  //  std::cout << result.data() << std::endl;
  //  result = tar.readAllStandardOutput();
  //  std::cout << result.data() << std::endl;

  //  outputLinks.append("http://" + getListenHost().toString() + ":8080" + QDir::separator() + QString(sessionId) + ".tar.gz");
  //  // **************************************************************************

  //  // Append to the json response payload all the output links
  //  m_ResponseObj[SIMPL::JSON::OutputLinks] = outputLinks;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineController::serviceJSON()
{
  QJsonParseError jsonParseError;
  QString requestBody = m_Request->getBody();
  QJsonDocument requestDoc = QJsonDocument::fromJson(requestBody.toUtf8(), &jsonParseError);
  if(jsonParseError.error != QJsonParseError::ParseError::NoError)
  {
    QString errMsg = tr("%1: Error Parsing JSON Request Body - %2").arg(EndPoint()).arg(jsonParseError.errorString());
    sendErrorResponse(HttpResponse::HttpStatusCode::BadRequest, errMsg, -40);
    return;
  }

  QJsonObject requestObj = requestDoc.object();

  serviceJSON(requestObj);
  if(m_ResponseObj.contains(SIMPL::JSON::ErrorCode) && m_ResponseObj[SIMPL::JSON::ErrorCode].toInt() < 0)
  {
    return;
  }

  m_Response->setStatusCode(HttpResponse::HttpStatusCode::OK);

  m_Response->setHeader("Content-Type", "application/json");

  QJsonDocument jdoc(m_ResponseObj);
  m_Response->write(jdoc.toJson(), true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineController::serviceMultiPart()
{
  QJsonObject pipelineMetadataObject = getPipelineMetadata();
  if(m_ResponseObj.contains(SIMPL::JSON::ErrorCode) && m_ResponseObj[SIMPL::JSON::ErrorCode].toInt() < 0)
  {
    return;
  }

  QByteArray jsonData = m_Request->getParameter("Pipeline");

  QJsonParseError jsonParseError;
  QJsonDocument pipelineDoc = QJsonDocument::fromJson(jsonData, &jsonParseError);
  if(jsonParseError.error != QJsonParseError::ParseError::NoError)
  {
    QString errMsg = tr("%1: Error Parsing Request's Pipeline into JSON - %2").arg(EndPoint()).arg(jsonParseError.errorString());
    sendErrorResponse(HttpResponse::HttpStatusCode::BadRequest, errMsg, -30);
    return;
  }

  QJsonObject pipelineObj = pipelineDoc.object();

  pipelineObj = replacePipelineValuesUsingMetadata(pipelineObj, pipelineMetadataObject);
  if(m_ResponseObj.contains(SIMPL::JSON::ErrorCode) && m_ResponseObj[SIMPL::JSON::ErrorCode].toInt() < 0)
  {
    return;
  }

  serviceJSON(pipelineObj);
  if(m_ResponseObj.contains(SIMPL::JSON::ErrorCode) && m_ResponseObj[SIMPL::JSON::ErrorCode].toInt() < 0)
  {
    return;
  }

  QJsonDocument jdoc(m_ResponseObj);

  m_Response->setStatusCode(HttpResponse::HttpStatusCode::OK);

  QString boundary = "@@@@@@@@@@@@@@@@@@@@";

  m_Response->setHeader("Content-Type", QByteArray::fromStdString(tr("multipart/form-data; boundary=\"%1\"").arg(boundary).toStdString()));

  QByteArray body;

  // Add pipeline response part to the body
  body.append(boundary + "\r\n");
  body.append("Content-Disposition: form-data; name=\"pipelineResponse\"\r\n");
  body.append("\r\n");
  body.append(jdoc.toJson() + "\n");

  // Add output file data to the body
  for(int i = 0; i < m_TemporaryOutputFilePaths.size(); i++)
  {
    QString tempFilePath = m_TemporaryOutputFilePaths[i];
    QString filePath = m_OutputFilePaths[i];
    QFile file(tempFilePath);
    if(file.open(QFile::ReadOnly))
    {
      QByteArray fileData = file.readAll();
      fileData = fileData.toBase64();

      body.append(boundary + "\r\n");
      body.append(tr("Content-Disposition: form-data; name=\"%1\"\r\n").arg(filePath));
      body.append("\r\n");
      body.append(fileData + "\n");
    }
    else
    {
      QString errString = file.errorString();
      QString errMsg = tr("%1: Server Response Creation Error - %2").arg(EndPoint()).arg(errString);
      sendErrorResponse(HttpResponse::HttpStatusCode::InternalServerError, errMsg, -110);
      return;
    }
  }

  m_Response->write(body, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineController::sendErrorResponse(HttpResponse::HttpStatusCode statusCode, const QString& errorMsg, int errCode)
{
  m_Response->setStatusCode(statusCode);
  QByteArray contentType = "application/json";
  m_Response->setHeader("Content-Type", contentType);

  m_ResponseObj[SIMPL::JSON::ErrorMessage] = errorMsg;
  m_ResponseObj[SIMPL::JSON::ErrorCode] = errCode;
  QJsonDocument errDoc(m_ResponseObj);

  QByteArray json = errDoc.toJson();

  m_Response->write(json, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject ExecutePipelineController::replacePipelineValuesUsingMetadata(QJsonObject pipelineJsonObj, QJsonObject pipelineMetadataObject)
{
  // Loop over every filter object and filter metadata object in the pipeline metadata object
  for(QJsonObject::iterator pIter = pipelineMetadataObject.begin(); pIter != pipelineMetadataObject.end(); pIter++)
  {
    QString filterKey = pIter.key();
    QJsonObject filterObj = pipelineJsonObj[filterKey].toObject();

    // Get the filter metadata object - this contains metadata pertaining to the filter's properties
    if(!pIter.value().isObject())
    {
      m_ResponseObj[SIMPL::JSON::ErrorMessage] = tr("%1: Pipeline metadata is not in the right format: filter %2 does not have an associated metadata object.").arg(EndPoint()).arg(filterKey);
      m_ResponseObj[SIMPL::JSON::ErrorCode] = -60;
      QJsonDocument jdoc(m_ResponseObj);

      m_Response->write(jdoc.toJson(), true);
      return QJsonObject();
    }

    QJsonObject filterMetadataObj = pIter.value().toObject();

    filterObj = replaceFilterValuesUsingMetadata(filterKey, filterObj, filterMetadataObj);

    pipelineJsonObj[filterKey] = filterObj;
  }

  return pipelineJsonObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject ExecutePipelineController::replaceFilterValuesUsingMetadata(const QString& filterKey, QJsonObject filterObj, QJsonObject filterMetadataObj)
{
  // Loop over every entry in the filter metadata object
  for(QJsonObject::iterator fIter = filterMetadataObj.begin(); fIter != filterMetadataObj.end(); fIter++)
  {
    QString propertyName = fIter.key();
    QString propertyValue = filterObj[propertyName].toString();

    // Get the property metadata object - this contains information such as whether the property is an input or an output
    if(!fIter.value().isObject())
    {
      m_ResponseObj[SIMPL::JSON::ErrorMessage] =
          tr("%1: Pipeline metadata is not in the right format: the metadata for property '%2' from filter %3 is not an object.").arg(EndPoint()).arg(propertyName).arg(filterKey.toInt() + 1);
      m_ResponseObj[SIMPL::JSON::ErrorCode] = -60;
      QJsonDocument jdoc(m_ResponseObj);

      m_Response->write(jdoc.toJson(), true);
      return QJsonObject();
    }

    QJsonObject propertyMetadataObj = fIter.value().toObject();

    // Get the IO_Type value - This will tell us whether the property is an input or an output
    if(!propertyMetadataObj.contains("IO_Type"))
    {
      m_ResponseObj[SIMPL::JSON::ErrorMessage] = tr("%1: Pipeline metadata is not in the right format: the metadata for property '%2' from filter %3 does not contain 'IO_Type' value.")
                                                     .arg(EndPoint())
                                                     .arg(propertyName)
                                                     .arg(filterKey.toInt() + 1);
      m_ResponseObj[SIMPL::JSON::ErrorCode] = -70;
      QJsonDocument jdoc(m_ResponseObj);

      m_Response->write(jdoc.toJson(), true);
      return QJsonObject();
    }
    else if(!propertyMetadataObj["IO_Type"].isString())
    {
      m_ResponseObj[SIMPL::JSON::ErrorMessage] = tr("%1: Pipeline metadata is not in the right format: the metadata for property '%2' from filter %3 does not have an 'IO_Type' string value.")
                                                     .arg(EndPoint())
                                                     .arg(propertyName)
                                                     .arg(filterKey.toInt() + 1);

      m_ResponseObj[SIMPL::JSON::ErrorCode] = -70;
      QJsonDocument jdoc(m_ResponseObj);

      m_Response->write(jdoc.toJson(), true);
      return QJsonObject();
    }

    QString propertyIOType = propertyMetadataObj["IO_Type"].toString();

    if(propertyIOType == "Output")
    {
      replaceOutputFileParametersUsingMetadata(propertyName, propertyValue, filterObj);
      if(m_ResponseObj.contains(SIMPL::JSON::ErrorCode) && m_ResponseObj[SIMPL::JSON::ErrorCode].toInt() < 0)
      {
        return QJsonObject();
      }
    }
    else if(propertyIOType == "Input")
    {
      replaceInputFileParametersUsingMetadata(propertyName, propertyValue, filterObj);
      if(m_ResponseObj.contains(SIMPL::JSON::ErrorCode) && m_ResponseObj[SIMPL::JSON::ErrorCode].toInt() < 0)
      {
        return QJsonObject();
      }
    }
    else
    {
      // Form Error response
      m_ResponseObj[SIMPL::JSON::ErrorMessage] = tr("%1: Unrecognized 'IO_Type' value for property '%2'.").arg(EndPoint()).arg(propertyName);
      m_ResponseObj[SIMPL::JSON::ErrorCode] = -100;
      QJsonDocument jdoc(m_ResponseObj);

      m_Response->write(jdoc.toJson(), true);
      return QJsonObject();
    }
  }

  return filterObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineController::replaceOutputFileParametersUsingMetadata(const QString& propertyName, const QString& propertyValue, QJsonObject& filterObj)
{
  QString propValue = propertyValue;

  // If the property value is a path and it came from Windows, we must remove the colon because it isn't
  // allowed as part of the path on some file systems.
  propValue = propValue.remove(":");

  // The property is an output, so create the file path where we will temporarily store the file data on the server,
  // create the directory structure, and update the pipeline json with that file path.
  QString tempFilePath = m_TempDir->path();
  if(propValue.startsWith("/") == false)
  {
    tempFilePath.append(QDir::separator());
  }
  tempFilePath.append(propValue);
  tempFilePath = QDir::toNativeSeparators(tempFilePath);
  QFileInfo fi(tempFilePath);
  QDir dir;
  dir.mkpath(fi.path());

  m_OutputFilePaths.push_back(propertyValue);
  m_TemporaryOutputFilePaths.push_back(tempFilePath);

  filterObj[propertyName] = tempFilePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineController::replaceInputFileParametersUsingMetadata(const QString& propertyName, const QString& propertyValue, QJsonObject& filterObj)
{
  QMultiMap<QByteArray, QByteArray> parameterMap = m_Request->getParameterMap();
  QString propValue = propertyValue;

  // This property is an input, so we need to figure out which other request parameter is the matching input file data.
  // This parameter's name will match the file path to the file on the client's file system.  If the path is a folder, each
  // file in the folder will be listed as a separate request parameter and this algorithm will still match the files correctly
  // because it tests to see if each request parameter name starts with the client's file path.
  for(QMultiMap<QByteArray, QByteArray>::iterator mapIter = parameterMap.begin(); mapIter != parameterMap.end(); mapIter++)
  {
    QString parameterName = mapIter.key();
    QByteArray parameterData = mapIter.value();

    // If the property value is a path and it came from Windows, we must remove the colon because it isn't
    // allowed as part of the path on some file systems.
    parameterName = parameterName.remove(":");
    propValue = propValue.remove(":");

    if(parameterName.startsWith(propValue))
    {
      QString tempFilePath = m_TempDir->path();
      if(parameterName.startsWith("/") == false)
      {
        tempFilePath.append(QDir::separator());
      }
      tempFilePath.append(parameterName);
      tempFilePath = QDir::toNativeSeparators(tempFilePath);
      QFileInfo fi(tempFilePath);
      QDir dir;
      dir.mkpath(fi.path());

      QFile* tempFile = new QFile(tempFilePath);
      if(tempFile->open(QFile::ReadWrite))
      {
        // Write out the file data to the temporary file we created on the server
        tempFile->write(parameterData);
        tempFile->close();

        // Update the pipeline json with the file path that we just wrote out to
        QString tempPropertyValue = m_TempDir->path();
        if(propValue.startsWith("/") == false)
        {
          tempPropertyValue.append(QDir::separator());
        }
        tempPropertyValue.append(propValue);

        filterObj[propertyName] = tempPropertyValue;
      }
      else
      {
        QString errMsg = tr("%1: File data for file with path '%2' included in the request could not be written to a temporary file on the server.").arg(EndPoint()).arg(parameterName);
        sendErrorResponse(HttpResponse::HttpStatusCode::InternalServerError, errMsg, -90);
        return;
      }
    }
    //          else
    //          {
    //            QString errMsg = EndPoint() + ": File data included in the request could not be written to a temporary file on the server.";
    //            sendErrorResponse(HttpResponse::HttpStatusCode::InternalServerError, errMsg, -100);
    //            return QJsonObject();
    //          }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject ExecutePipelineController::getPipelineMetadata()
{
  QJsonParseError jsonParseError;
  QByteArray jsonData = m_Request->getParameter("PipelineMetadata");
  QJsonDocument pipelineReplacementLookupDoc = QJsonDocument::fromJson(jsonData, &jsonParseError);
  if(jsonParseError.error != QJsonParseError::ParseError::NoError)
  {
    // Form Error response
    m_ResponseObj[SIMPL::JSON::ErrorMessage] = tr("%1: Error Parsing PipelineMetadata Parameter - %2").arg(EndPoint()).arg(jsonParseError.errorString());
    m_ResponseObj[SIMPL::JSON::ErrorCode] = -30;
    QJsonDocument jdoc(m_ResponseObj);
    m_Response->write(jdoc.toJson(), true);
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

  m_Request = &request;
  m_Response = &response;
  m_ResponseObj = responseObj;

  m_TempDir = new QTemporaryDir();

  QString content_type = request.getHeader(QByteArray("content-type"));
  if(content_type == "application/json")
  {
    serviceJSON();
  }
  else if(content_type.startsWith("multipart/form-data"))
  {
    serviceMultiPart();
  }
  else
  {
    QString errMsg = EndPoint() + ": Content Type is not application/json or multipart/form-data";
    sendErrorResponse(HttpResponse::HttpStatusCode::BadRequest, errMsg, -20);
    return;
  }

  cleanup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineController::cleanup()
{
  m_Request = nullptr;
  m_Response = nullptr;
  m_ResponseObj = QJsonObject();

  delete m_TempDir;
  m_TempDir = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecutePipelineController::EndPoint()
{
  return QString("ExecutePipeline");
}
