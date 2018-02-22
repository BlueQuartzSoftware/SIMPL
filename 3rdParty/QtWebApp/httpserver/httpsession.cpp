/**
  @file
  @author Stefan Frings
*/

#include "httpsession.h"

#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpSession::HttpSession(bool canStore)
{
  if(canStore)
  {
    dataPtr = new HttpSessionData();
    dataPtr->refCount = 1;
    dataPtr->lastAccess = QDateTime::currentMSecsSinceEpoch();
#if 1
    uint l = 100;
    ushort w1 = 200;
    ushort w2 = 500;
    uchar b1 = 'S';
    uchar b2 = 'I';
    uchar b3 = 'M';
    uchar b4 = 'P';
    uchar b5 = 'L';
    uchar b6 = 'i';
    uchar b7 = 'b';
    uchar b8 = '1';
    QUuid uuid = QUuid(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
    QUuid p1 = QUuid::createUuidV5(uuid, QString::number(dataPtr->lastAccess, 10));
    QByteArray p1Bytes = p1.toByteArray();
    p1Bytes.chop(1);
    p1Bytes = p1Bytes.mid(1);
    dataPtr->m_SessionId = p1Bytes;
#else
    dataPtr->m_SessionId = QUuid::createUuid().toString().toLocal8Bit();
#endif
#ifdef SUPERVERBOSE
    qDebug("HttpSession: created new session data with id %s", dataPtr->id.data());
#endif
  }
  else
  {
    dataPtr = 0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpSession::HttpSession(const HttpSession& other)
{
  dataPtr = other.dataPtr;
  if(dataPtr)
  {
    dataPtr->lock.lockForWrite();
    dataPtr->refCount++;
#ifdef SUPERVERBOSE
    qDebug("HttpSession: refCount of %s is %i", dataPtr->id.data(), dataPtr->refCount);
#endif
    dataPtr->lock.unlock();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpSession& HttpSession::operator=(const HttpSession& other)
{
  HttpSessionData* oldPtr = dataPtr;
  dataPtr = other.dataPtr;
  if(dataPtr)
  {
    dataPtr->lock.lockForWrite();
    dataPtr->refCount++;
#ifdef SUPERVERBOSE
    qDebug("HttpSession: refCount of %s is %i", dataPtr->id.data(), dataPtr->refCount);
#endif
    dataPtr->lastAccess = QDateTime::currentMSecsSinceEpoch();
    dataPtr->lock.unlock();
  }
  if(oldPtr)
  {
    int refCount;
    oldPtr->lock.lockForRead();
    refCount = oldPtr->refCount--;
#ifdef SUPERVERBOSE
    qDebug("HttpSession: refCount of %s is %i", oldPtr->id.data(), oldPtr->refCount);
#endif
    oldPtr->lock.unlock();
    if(refCount == 0)
    {
      delete oldPtr;
    }
  }
  return *this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HttpSession::~HttpSession()
{
  if(dataPtr)
  {
    int refCount;
    dataPtr->lock.lockForRead();
    refCount = --dataPtr->refCount;
#ifdef SUPERVERBOSE
    qDebug("HttpSession: refCount of %s is %i", dataPtr->id.data(), dataPtr->refCount);
#endif
    dataPtr->lock.unlock();
    if(refCount == 0)
    {
      qDebug("HttpSession: deleting data");
      delete dataPtr;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QByteArray HttpSession::getId() const
{
  if(dataPtr)
  {
    return dataPtr->m_SessionId;
  }
  else
  {
    return QByteArray();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool HttpSession::isNull() const
{
  return dataPtr == 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpSession::set(const QByteArray& key, const QVariant& value)
{
  if(dataPtr)
  {
    dataPtr->lock.lockForWrite();
    dataPtr->values.insert(key, value);
    dataPtr->lock.unlock();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpSession::remove(const QByteArray& key)
{
  if(dataPtr)
  {
    dataPtr->lock.lockForWrite();
    dataPtr->values.remove(key);
    dataPtr->lock.unlock();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant HttpSession::get(const QByteArray& key) const
{
  QVariant value;
  if(dataPtr)
  {
    dataPtr->lock.lockForRead();
    value = dataPtr->values.value(key);
    dataPtr->lock.unlock();
  }
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool HttpSession::contains(const QByteArray& key) const
{
  bool found = false;
  if(dataPtr)
  {
    dataPtr->lock.lockForRead();
    found = dataPtr->values.contains(key);
    dataPtr->lock.unlock();
  }
  return found;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QByteArray, QVariant> HttpSession::getAll() const
{
  QMap<QByteArray, QVariant> values;
  if(dataPtr)
  {
    dataPtr->lock.lockForRead();
    values = dataPtr->values;
    dataPtr->lock.unlock();
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint64 HttpSession::getLastAccess() const
{
  qint64 value = 0;
  if(dataPtr)
  {
    dataPtr->lock.lockForRead();
    value = dataPtr->lastAccess;
    dataPtr->lock.unlock();
  }
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HttpSession::setLastAccess()
{
  if(dataPtr)
  {
    dataPtr->lock.lockForRead();
    dataPtr->lastAccess = QDateTime::currentMSecsSinceEpoch();
    dataPtr->lock.unlock();
  }
}
