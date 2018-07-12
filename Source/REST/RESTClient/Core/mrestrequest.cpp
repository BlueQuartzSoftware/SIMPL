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

#include "mrestrequest.h"
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QtCore/QString>

/*!
 * \defgroup Network Network communication with Web API
 * @{
 */

/*!
 * \addtogroup Network
 * \class MRestRequest
 * \brief base equest for mrbabu.com API
 *
 * Base request does nothing but provides common functionality needed for
 * communication.
 *
 * When subclassing, implement your custom parse() method and add a signal to
 * return parsing result to logic classes.
 *
 * Also, you *have to* specify request type by setting mType member variable.
 * If you don't, your subclass will not work! It will print an error when
 * MRestRequest::Send() is used.
 */
MRestRequest::MRestRequest(Type msgType)
: QObject()
, mPriority(Priority::Normal)
, mType(msgType)
{
  mRequestTimeout = 5000;
  mRequestTimer = new QTimer(this);
  mRequestTimer->setSingleShot(true);
  mRequestTimer->setInterval(mRequestTimeout);
  connect(mRequestTimer, &QTimer::timeout, this, &MRestRequest::retry);

  createCommandMap();
}

MRestRequest::~MRestRequest()
{
}

/**
 * @brief createCommandMap
 */
void MRestRequest::createCommandMap()
{
  m_CommandMap.insert(Command::APINotFound, "ApiNotFound");
  m_CommandMap.insert(Command::ExecutePipeline, "ExecutePipeline");
  m_CommandMap.insert(Command::ListFilterParameters, "ListFilterParameters");
  m_CommandMap.insert(Command::LoadedPlugins, "LoadedPlugins");
  m_CommandMap.insert(Command::NamesOfFilters, "NamesOfFilters");
  m_CommandMap.insert(Command::NumFilters, "NumFilters");
  m_CommandMap.insert(Command::PluginInfo, "PluginInfo");
  m_CommandMap.insert(Command::PreflightPipeline, "PreflightPipeline");
}

/*!
 * \brief Set URL of this request. Overrides the QUrl set in the constructor
 * \param url of the destination server
 */
void MRestRequest::setAddress(const QUrl& url)
{
  m_RequestUrl = url;
}

/*!
 * \brief Sets request timeout.
 * \param milliseconds
 */
void MRestRequest::setRequestTimeout(quint32 msec)
{
  mRequestTimeout = msec;
  mRequestTimer->setInterval(mRequestTimeout);
}

/*!
 * Returns the address associated with this request.
 */
QUrl MRestRequest::address() const
{
  return m_RequestUrl;
}

/*!
 * Sets request priority to \a priority.
 */
void MRestRequest::setPriority(MRestRequest::Priority priority)
{
  mPriority = priority;
}

/*!
 * \brief Check if priority is higher then in given request
 * \param request for comparison
 * \return true if this priority is higher, false otherwise
 */
bool MRestRequest::isHigherPriority(const MRestRequest& request)
{
  return mPriority > request.mPriority;
}

/*!
 * \brief send request with given network manager
 * \param manager network access manager
 */
void MRestRequest::sendWith(QNetworkAccessManager* manager)
{
  mNetworkManager = manager;
  mRequestRetryCounter = 1;
  send();
  mReplyData.clear();
}

/*!
 * \return JSON document with reply received from server.
 */
QJsonDocument MRestRequest::document() const
{
  return mReplyDocument;
}

/*!
 * \return Reply network error received from server.
 */
QString MRestRequest::lastError() const
{
  return mLastError;
}

/*!
 * \return Raw data reply received from server.
 */
QByteArray MRestRequest::rawData() const
{
  return mReplyData;
}

/*!
 * \brief sends request using method specified in mType (Pup, Post, Get, Delete).
 *
 * If mType is not explicitly set, this function will bail out (with error).
 */
void MRestRequest::send()
{
  Q_ASSERT(mNetworkManager);
  QString msg = tr("%1 - Try %2").arg(m_RequestUrl.toDisplayString()).arg(QString::number(mRequestRetryCounter));
  emit notifyStatusMessage(msg);
  mReplyData.clear();
  QNetworkRequest request(m_RequestUrl);
  request.setOriginatingObject(this);

  if(mType == Type::None)
  {
    QString errMsg = "Request type is set to None - can't send. Please set request type to Put, Post, Get or Delete";
    emit notifyErrorMessage(errMsg);
  }
  else if(mType == Type::Put)
  {
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    mActiveReply = mNetworkManager->put(request, mRequestDocument.toJson());
  }
  else if(mType == Type::Get)
  {
    mActiveReply = mNetworkManager->get(request);
  }
  else if(mType == Type::Post)
  {
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    mActiveReply = mNetworkManager->post(request, mRequestDocument.toJson());
  }
  else if(mType == Type::Delete)
  {
    mActiveReply = mNetworkManager->deleteResource(request);
  }
  else
  {
    Q_UNREACHABLE();
  }

  connect(mActiveReply, &QNetworkReply::finished, this, &MRestRequest::onReplyFinished);
  connect(mActiveReply, &QNetworkReply::readyRead, mRequestTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
  connect(mActiveReply, &QNetworkReply::readyRead, this, &MRestRequest::onReadyRead);
  connect(mActiveReply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &MRestRequest::onReplyError);
  mRequestTimer->start();
}

/*!
 * \brief resend request if time limit has passed
 */
void MRestRequest::retry()
{
  mActiveReply->abort();
  mActiveReply->deleteLater();
  if(++mRequestRetryCounter > mMaxRequestRetryCount)
  {
    QString errMsg = "Request retry limit reached - operation aborted!";
    notifyErrorMessage(errMsg);
  }
  else
  {
    if(mActiveReply->bytesAvailable())
    {
      QString msg = QObject::tr("Retrying request, %1 bytes lost.").arg(mActiveReply->bytesAvailable());
      emit notifyStatusMessage(msg);
      send();
    }
  }
}

/*!
 * \brief catch network error code and store to mLastError as QString
 * \param network error code
 */
void MRestRequest::onReplyError(QNetworkReply::NetworkError code)
{
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
  if(reply != Q_NULLPTR && code != QNetworkReply::NoError)
  {
    reply->deleteLater();
    mRequestTimer->stop();
    mLastError = reply->errorString();
    emit notifyErrorMessage(mLastError);
  }
}

/*!
 * \brief collect data from server response
 */
void MRestRequest::onReadyRead()
{
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
  if(reply != nullptr)
  {
    mReplyData += reply->readAll();
  }
}

/*!
 * \brief handle collected data from server response
 */
void MRestRequest::onReplyFinished()
{
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
  if(reply == nullptr)
  {
    return;
  }

  mRequestTimer->stop();
  reply->deleteLater();

  const QString requestName(metaObject()->className());
  QJsonParseError parseError;

  const QString status(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString());

  if(mReplyData.isEmpty())
  {
    emit notifyErrorMessage(tr("%1 - %2 - Request reply is empty").arg(requestName).arg(status));
    emit finished();
    return;
  }

  // rawData can still be parsed in another formats
  mReplyDocument = QJsonDocument::fromJson(mReplyData, &parseError);
  if(parseError.error != QJsonParseError::NoError)
  {
    emit notifyWarningMessage(tr("%1 - %2 - Error while parsing json document: %3").arg(requestName).arg(status).arg(parseError.errorString()));
    emit finished();
    return;
  }

  emit notifyStatusMessage(tr("%1 - request response received").arg(requestName));

  if(mReplyDocument.isNull())
  {
    mLastError = "JSON document is invalid";
    emit notifyStatusMessage(tr("%1 - %2").arg(requestName).arg(mLastError));
    emit finished(); // rawData can still be parsed in another formats
    return;
  }

  // parse json document according to specific request reply format
  parseReplyData();
  emit finished();
}

/*!
 * \fn MRestRequest::parse
 * \brief read QJsonDocument or Rawreturned by server
 *
 * This method should emit parsed data in form of request specific signal
 * because Request object will be deleted by event queue soon after
 * returning from onReplyFinished
 * NOTE!! you should check mReplyDocument and use rawData() if mReplyDocument isNull
 */

/*! @} */
