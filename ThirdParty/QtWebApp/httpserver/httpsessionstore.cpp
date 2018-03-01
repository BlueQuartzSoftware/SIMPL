/**
  @file
  @author Stefan Frings
*/

#include "httpsessionstore.h"

#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QUuid>

HttpSessionStore* HttpSessionStore::self = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpSessionStore::HttpSessionStore(QSettings* settings, QObject* parent)
: QObject(parent)
{
  this->settings = settings;
  connect(&cleanupTimer, SIGNAL(timeout()), this, SLOT(sessionTimerEvent()));
  cleanupTimer.start(60000);
  if(settings)
  {
    cookieName = settings->value("cookieName", "sessionid").toByteArray();
    expirationTime = settings->value("expirationTime", 3600000).toInt();
  }
  qDebug() << "HttpSessionStore: Sessions expire after " << expirationTime << " milliseconds";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpSessionStore::~HttpSessionStore()
{
  cleanupTimer.stop();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpSessionStore* HttpSessionStore::CreateInstance(QSettings* settings, QObject* parent)
{
  if(self == nullptr)
  {
    self = new HttpSessionStore(settings, parent);
  }
  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpSessionStore* HttpSessionStore::Instance()
{
  if(self == nullptr)
  {
    self = new HttpSessionStore(nullptr, nullptr);
  }
  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QByteArray HttpSessionStore::getSessionId(HttpRequest& request, HttpResponse& response)
{
  Q_UNUSED(response);
  // The session ID in the response has priority because this one will be used in the next request.
  QMutexLocker locker(&mutex);
  QByteArray sessionId;

  QByteArray requestBody = request.bodyData;
  QJsonParseError jsonParseError;
  QJsonDocument requestJsonDoc = QJsonDocument::fromJson(requestBody, &jsonParseError);
  if(jsonParseError.error != QJsonParseError::NoError)
  {
    return sessionId;
  }

  // Get the session ID from the request JSON Payload
  QJsonObject requestJsonObj = requestJsonDoc.object();
  QJsonValue sessionIdValue = requestJsonObj["SessionID"];
  if(sessionIdValue.isString())
  {
    sessionId = sessionIdValue.toString().toLatin1();
  }

  // QByteArray sessionId = response.getCookies().value(cookieName).getValue();
  //  if(sessionId.isEmpty())
  //  {
  //    // Get the session ID from the request cookie
  //    sessionId = request.getCookie(cookieName);
  //  }
  // Clear the session ID if there is no such session in the storage.
  if(!sessionId.isEmpty())
  {
    if(!sessions.contains(sessionId))
    {
      qDebug() << "HttpSessionStore: received invalid session cookie with ID " << sessionId.data();
      sessionId.clear();
    }
  }
  return sessionId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpSession HttpSessionStore::getSession(HttpRequest& request, HttpResponse& response, bool allowCreate)
{
  QByteArray sessionId = getSessionId(request, response);
  QMutexLocker locker(&mutex);
  if(!sessionId.isEmpty())
  {
    HttpSession session = sessions.value(sessionId);
    if(!session.isNull())
    {
      locker.unlock();
      //      // Refresh the session cookie
      //      QByteArray cookieName = settings->value("cookieName", "sessionid").toByteArray();
      //      QByteArray cookiePath = settings->value("cookiePath").toByteArray();
      //      QByteArray cookieComment = settings->value("cookieComment").toByteArray();
      //      QByteArray cookieDomain = settings->value("cookieDomain").toByteArray();
      //      response.setCookie(HttpCookie(cookieName, session.getId(), expirationTime / 1000, cookiePath, cookieComment, cookieDomain));

      session.setLastAccess();
      return session;
    }
  }
  // Need to create a new session
  if(allowCreate)
  {
    //    QByteArray cookieName = settings->value("cookieName", "sessionid").toByteArray();
    //    QByteArray cookiePath = settings->value("cookiePath").toByteArray();
    //    QByteArray cookieComment = settings->value("cookieComment").toByteArray();
    //    QByteArray cookieDomain = settings->value("cookieDomain").toByteArray();

    HttpSession session(true);
    qDebug() << "HttpSessionStore: create new session with ID " << session.getId().data();
    sessions.insert(session.getId(), session);
    // response.setCookie(HttpCookie(cookieName, session.getId(), expirationTime / 1000, cookiePath, cookieComment, cookieDomain));
    return session;
  }
  // Return a null session
  return HttpSession(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpSession HttpSessionStore::getSession(const QByteArray id)
{
  QMutexLocker locker(&mutex);
  HttpSession session = sessions.value(id);
  locker.unlock();
  session.setLastAccess();
  return session;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpSessionStore::sessionTimerEvent()
{
  QMutexLocker locker(&mutex);
  qint64 now = QDateTime::currentMSecsSinceEpoch();
  QMap<QByteArray, HttpSession>::iterator i = sessions.begin();
  while(i != sessions.end())
  {
    QMap<QByteArray, HttpSession>::iterator prev = i;
    ++i;
    HttpSession session = prev.value();
    qint64 lastAccess = session.getLastAccess();
    if(now - lastAccess > expirationTime)
    {
      qDebug("HttpSessionStore: session %s expired", session.getId().data());
      sessions.erase(prev);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpSessionStore::removeSession(HttpSession session)
{
  mutex.lock();
  sessions.remove(session.getId());
  mutex.unlock();
}
