/*******************************************************************************
Copyright (C) 2017 Milo Solutions
Contact: https://www.milosolutions.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#include "SIMPLRestClient.h"

#include <QtCore/QFile>

#include <QLoggingCategory>
#include <QSharedPointer>

#include "SIMPLClientRequest.h"

Q_LOGGING_CATEGORY(mrbrequest, "request")

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLRestClient::SIMPLRestClient(QObject* parent)
: MRestRequestManager(parent)
, m_ServerUrl(QUrl())
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLRestClient::SIMPLRestClient(QUrl serverUrl, QObject* parent)
: MRestRequestManager(parent)
, m_ServerUrl(serverUrl)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient::sendAPINotFoundRequest(int timeout)
{
  auto clientRequest = createRequest(SIMPLClientRequest::Command::APINotFound, SIMPLClientRequest::Type::Get);
  clientRequest->setRequestTimeout(timeout);
  send(clientRequest);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient::sendExecutePipelineRequest(QJsonObject pipelineObject, int timeout)
{
  auto clientRequest = createRequest(SIMPLClientRequest::Command::ExecutePipeline, SIMPLClientRequest::Type::Put, pipelineObject);
  clientRequest->setRequestTimeout(timeout);
  send(clientRequest);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient::sendListFilterParametersRequest(QJsonObject filterObject, int timeout)
{
  auto clientRequest = createRequest(SIMPLClientRequest::Command::ListFilterParameters, SIMPLClientRequest::Type::Put, filterObject);
  clientRequest->setRequestTimeout(timeout);
  send(clientRequest);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient::sendLoadedPluginsRequest(int timeout)
{
  auto clientRequest = createRequest(SIMPLClientRequest::Command::LoadedPlugins, SIMPLClientRequest::Type::Get);
  clientRequest->setRequestTimeout(timeout);
  send(clientRequest);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient::sendNamesOfFiltersRequest(int timeout)
{
  auto clientRequest = createRequest(SIMPLClientRequest::Command::NamesOfFilters, SIMPLClientRequest::Type::Get);
  clientRequest->setRequestTimeout(timeout);
  send(clientRequest);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient::sendNumberOfFiltersRequest(int timeout)
{
  auto clientRequest = createRequest(SIMPLClientRequest::Command::NumFilters, SIMPLClientRequest::Type::Get);
  clientRequest->setRequestTimeout(timeout);
  send(clientRequest);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient::sendPluginInformationRequest(QJsonObject pluginObject, int timeout)
{
  auto clientRequest = createRequest(SIMPLClientRequest::Command::PluginInfo, SIMPLClientRequest::Type::Put, pluginObject);
  clientRequest->setRequestTimeout(timeout);
  send(clientRequest);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient::sendPreflightPipelineRequest(QJsonObject pipelineObject, int timeout)
{
  auto clientRequest = createRequest(SIMPLClientRequest::Command::PreflightPipeline, SIMPLClientRequest::Type::Put, pipelineObject);
  clientRequest->setRequestTimeout(timeout);
  send(clientRequest);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSharedPointer<SIMPLClientRequest> SIMPLRestClient::createRequest(SIMPLClientRequest::Command cmd, SIMPLClientRequest::Type requestType, QJsonObject requestObj)
{
  auto clientRequest = QSharedPointer<SIMPLClientRequest>::create(m_ServerUrl, cmd, requestType, requestObj);
  m_NumOfActiveRequests++;
  QObject::connect(clientRequest.data(), &SIMPLClientRequest::finished, this, [=] {
    m_NumOfActiveRequests--;
    if(m_NumOfActiveRequests <= 0)
    {
      emit finished();
    }
  });

  QObject::connect(clientRequest.data(), &SIMPLClientRequest::notifyErrorMessage, this, &SIMPLRestClient::notifyErrorMessage);
  QObject::connect(clientRequest.data(), &SIMPLClientRequest::notifyWarningMessage, this, &SIMPLRestClient::notifyWarningMessage);
  QObject::connect(clientRequest.data(), &SIMPLClientRequest::notifyStatusMessage, this, &SIMPLRestClient::notifyStatusMessage);

  return clientRequest;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject SIMPLRestClient::extractPipelineFromFile(const QString& filePath)
{
  QFile file(filePath);
  if(file.open(QFile::ReadOnly) == false)
  {
    return QJsonObject();
  }

  QByteArray data = file.readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);

  QJsonObject pipelineObject;
  pipelineObject["Pipeline"] = doc.object();

  return pipelineObject;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient::setServerUrl(QUrl url)
{
  m_ServerUrl = url;
}
