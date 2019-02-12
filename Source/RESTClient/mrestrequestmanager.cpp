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

#include "mrestrequestmanager.h"
#include "mrestrequest.h"

#include <QLoggingCategory>
#include <QNetworkAccessManager>

Q_LOGGING_CATEGORY(rm, "requestManager")

/*!
 * \addtogroup Network
 * @{
 */

/*!
 * \class MRestRequestManager
 * \brief Allow sending request in queue
 *
 * Manager for general Request class. Provides way to
 * control number of concurrent downloads in network.
 */

/*!
 * Constructs a new network access manager.
 */
MRestRequestManager::MRestRequestManager(QObject* parent)
: QObject(parent)
{
  networkManager = new QNetworkAccessManager(this);
  connect(networkManager, &QNetworkAccessManager::sslErrors, this, &MRestRequestManager::onSslErrors);
  mIgnoreSslErrors = false;
}

/*!
 * \brief send request with use of queue
 * \param request
 * \param type what type of HTTP message to send (PUT, POST, GET, DELETE)
 *
 * MRestRequestManager takes ownership of the pointer (it will keep it alive until
 * the request result is received).
 */
void MRestRequestManager::send(MRestRequestPtr request)
{
  if(mActiveRequests.size() < MaxActiveRequestsCount)
  {
    mActiveRequests.append(request);

    request->sendWith(networkManager);
    QObject::connect(request.data(), &MRestRequest::finished, this, &MRestRequestManager::onRequestFinished);
  }
  else
  {
    pendingRequests.enqueue(request);
  }
}

/*!
 * \brief setting ignore to all certificate errors
 */
void MRestRequestManager::ignoreSslErrors()
{
  mIgnoreSslErrors = true;
}

/*!
 * \brief handle request queue
 */
void MRestRequestManager::onRequestFinished()
{
  removeActiveRequest(sender());
  if(!pendingRequests.isEmpty())
    send(pendingRequests.dequeue());
}

/*!
 * Removes request (taken from \a sender pointer) from the list of active requests.
 * Most usually, the request is being deleted here.
 */
void MRestRequestManager::removeActiveRequest(QObject* sender)
{
  if(!sender)
  {
    qCDebug(rm) << "Cannot remove null active request pointer";
    return;
  }

  foreach(const MRestRequestPtr& request, mActiveRequests)
  {
    if(request.data() == sender)
    {
      mActiveRequests.removeOne(request);
      return;
    }
  }

  qCDebug(rm) << "Could not find the request to remove" << sender;
}

/*!
 *
 * \brief handle networkManager onSslErrors
 * \param reply
 * \param errors
 * handle onSslErrors. ignore SSL errors or abort and emit sslErrorsChanged
 */

void MRestRequestManager::onSslErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{
  if(mIgnoreSslErrors)
  {
    reply->ignoreSslErrors();
    return;
  }

  reply->deleteLater();
  mLastSslErrors.clear();
  foreach(QSslError error, errors)
  {
    mLastSslErrors << error.errorString();
  }

  emit sslErrorsChanged(mLastSslErrors);
}

/*! @} */
