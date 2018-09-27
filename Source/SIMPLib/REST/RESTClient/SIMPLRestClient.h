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

#ifndef simplrestclient_h
#define simplrestclient_h

#include "mrestrequestmanager.h"

#include <QtCore/QJsonObject>

#include <QObject>

class SIMPLClientRequest;

class SIMPLRestClient : public MRestRequestManager
{
  Q_OBJECT
public:
  SIMPLRestClient(QObject* parent = nullptr);
  SIMPLRestClient(QUrl serverUrl, QObject* parent = nullptr);

  QJsonObject extractPipelineFromFile(const QString& filePath);

  void sendAPINotFoundRequest(int timeout = 5000);

  void sendExecutePipelineRequest(QJsonObject pipelineObject, int timeout = 5000);

  void sendListFilterParametersRequest(QJsonObject filterObject, int timeout = 5000);

  void sendLoadedPluginsRequest(int timeout = 5000);

  void sendNamesOfFiltersRequest(int timeout = 5000);

  void sendNumberOfFiltersRequest(int timeout = 5000);

  void sendPluginInformationRequest(QJsonObject pluginObject, int timeout = 5000);

  void sendPreflightPipelineRequest(QJsonObject pipelineObject, int timeout = 5000);

  void setServerUrl(QUrl url);

signals:
  void notifyErrorMessage(const QString& msg);
  void notifyWarningMessage(const QString& msg);
  void notifyStatusMessage(const QString& msg);

  void finished();

private:
  QUrl m_ServerUrl;
  int m_NumOfActiveRequests = 0;

  QSharedPointer<SIMPLClientRequest> createRequest(MRestRequest::Command cmd, MRestRequest::Type requestType, QJsonObject requestObj = QJsonObject());
};

#endif // simplrestclient_h
