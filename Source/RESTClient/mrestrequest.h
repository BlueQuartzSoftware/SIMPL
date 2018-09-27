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

#pragma once

#include <QJsonDocument>
#include <QNetworkReply>
#include <QObject>
#include <QUrl>

#include "RESTClient/RESTClient.h"

class QTimer;
class QNetworkReply;
class QNetworkAccessManager;

class RESTClient_EXPORT MRestRequest : public QObject
{
  Q_OBJECT
public:
  enum class Type
  {
    None,
    Put,
    Post,
    Get,
    Delete
  };
  Q_ENUM(Type)

  enum class Command
  {
    None,
    APINotFound,
    ExecutePipeline,
    ListFilterParameters,
    LoadedPlugins,
    NamesOfFilters,
    NumFilters,
    PluginInfo,
    PreflightPipeline
  };
  Q_ENUM(Command)

  explicit MRestRequest(MRestRequest::Type msgType);
  virtual ~MRestRequest();
  void setAddress(const QUrl& url);
  void setRequestTimeout(quint32 msec = 5000);
  bool isHigherPriority(const MRestRequest& request);
  void sendWith(QNetworkAccessManager* manager);
  QUrl address() const;
  QString lastError() const;
  QJsonDocument document() const;
  QByteArray rawData() const;

signals:
  void notifyStatusMessage(const QString& msg);
  void notifyErrorMessage(const QString& msg);
  void notifyWarningMessage(const QString& msg);

  void finished() const;
  void replyError(const QString& msgs) const;

public slots:

protected:
  enum class Priority
  {
    Bottom,
    Low,
    Normal,
    High,
    Top
  };
  void setPriority(Priority priority);

  void send();
  virtual void retry();
  virtual void parseReplyData() = 0;

  Priority mPriority = Priority::Normal;
  Type mType = Type::Get;
  QMap<Command, QString> m_CommandMap;
  QUrl mServerUrl;
  QUrl m_RequestUrl;
  QNetworkReply* mActiveReply;
  int mRequestRetryCounter;
  int mRequestTimeout;
  int mMaxRequestRetryCount = 3;
  QByteArray mReplyData;
  QJsonDocument mReplyDocument;
  QJsonDocument mRequestDocument;

private slots:
  void onReplyError(QNetworkReply::NetworkError code);
  void onReadyRead();
  void onReplyFinished();

private:
  QString mLastError;
  QTimer* mRequestTimer;
  QNetworkAccessManager* mNetworkManager = nullptr;

  /**
   * @brief createCommandMap
   */
  void createCommandMap();
};
