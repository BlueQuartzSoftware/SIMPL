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

#include "SIMPLClientRequest.h"

#include <iostream>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QDebug>
#include <QLoggingCategory>

SIMPLClientRequest::SIMPLClientRequest(QUrl url, Command command, SIMPLClientRequest::Type msgType, QJsonObject jsonMsg)
: MRestRequest(msgType)
{
  QString serverUrlStr = url.toString();
  if(serverUrlStr.endsWith('/') == false)
  {
    serverUrlStr.append('/');
  }
  mServerUrl = QUrl(serverUrlStr);

  mRequestDocument = QJsonDocument(jsonMsg);

  QUrl cmdUrl = generateCommandUrl(command);
  setAddress(cmdUrl);
}

QUrl SIMPLClientRequest::generateCommandUrl(Command command)
{
  QString ipAddressPath = mServerUrl.toString();
  if(ipAddressPath.endsWith('/') == false)
  {
    ipAddressPath.append('/');
  }

  QString commandStr = m_CommandMap[command];
  QString urlStr = QObject::tr("%1api/v1/%2").arg(ipAddressPath).arg(commandStr);

  return QUrl(urlStr);
}

void SIMPLClientRequest::parseReplyData()
{
  const QJsonObject object(mReplyDocument.object());

  QByteArray out = mReplyDocument.toJson(QJsonDocument::Indented);
  emit notifyStatusMessage(out.data());

  emit finished();

  //    emit replyInfo(cityName, humidity, pressure, temp);
}
