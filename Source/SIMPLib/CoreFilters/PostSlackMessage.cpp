/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
#include "PostSlackMessage.h"

#include <QtCore/QEventLoop>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PostSlackMessage::PostSlackMessage()
: m_SlackUser("")
, m_SlackUrl("")
, m_SlackMessage("")
, m_WarningsAsError(false)
{
  m_SlackUser = "[DREAM.3D@" + QSysInfo::machineHostName() + "]";
  initialize();
  m_NetworkManager = new QNetworkAccessManager(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PostSlackMessage::~PostSlackMessage()
{
  delete m_NetworkManager;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PostSlackMessage::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PostSlackMessage::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_STRING_FP("Slack User", SlackUser, FilterParameter::Parameter, PostSlackMessage));
  parameters.push_back(SIMPL_NEW_STRING_FP("Slack Url", SlackUrl, FilterParameter::Parameter, PostSlackMessage));
  parameters.push_back(SIMPL_NEW_STRING_FP("Slack Message", SlackMessage, FilterParameter::Parameter, PostSlackMessage));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Treat a Warning as an Error", WarningsAsError, FilterParameter::Parameter, PostSlackMessage));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PostSlackMessage::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PostSlackMessage::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  QString msg = getSlackMessage();
  msg = msg.replace("&", "&amp;");
  msg = msg.replace("<", "&lt;");
  msg = msg.replace(">", "&gt;");

  QJsonObject jsonRoot;

  jsonRoot["mrkdwn"] = true;
  jsonRoot["username"] = getSlackUser();
  jsonRoot["text"] = msg;

  QJsonDocument doc(jsonRoot);
  QByteArray data = doc.toJson(QJsonDocument::Compact);

  QNetworkRequest netRequest;
  netRequest.setUrl(getSlackUrl());
  netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QEventLoop waitLoop;
  QNetworkAccessManager* connection = new QNetworkAccessManager();
  QNetworkReply* reply = connection->post(netRequest, data);
  QObject::connect(reply, SIGNAL(finished()), &waitLoop, SLOT(quit()));
  waitLoop.exec();

  int errorCode = reply->error();
  if(errorCode != 0)
  {
    if(getWarningsAsError())
    {
      QString ss = QObject::tr("Slack returned the following error code: %1").arg(getErrorCode());
      setErrorCondition(-76000, ss);
    }
    else
    {
      QString ss = QObject::tr("Slack returned the following error code: %1").arg(getErrorCode());
      setWarningCondition(-76001, ss);
    }
  }

  delete reply;
  delete connection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PostSlackMessage::newFilterInstance(bool copyFilterParameters) const
{
  PostSlackMessage::Pointer filter = PostSlackMessage::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PostSlackMessage::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PostSlackMessage::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PostSlackMessage::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PostSlackMessage::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid PostSlackMessage::getUuid() const
{
  return QUuid("{0ca83462-8564-54ea-9f4e-e5141974f30b}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PostSlackMessage::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PostSlackMessage::getHumanLabel() const
{
  return "Post Slack Message";
}

// -----------------------------------------------------------------------------
PostSlackMessage::Pointer PostSlackMessage::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<PostSlackMessage> PostSlackMessage::New()
{
  struct make_shared_enabler : public PostSlackMessage
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString PostSlackMessage::getNameOfClass() const
{
  return QString("PostSlackMessage");
}

// -----------------------------------------------------------------------------
QString PostSlackMessage::ClassName()
{
  return QString("PostSlackMessage");
}

// -----------------------------------------------------------------------------
void PostSlackMessage::setSlackUser(const QString& value)
{
  m_SlackUser = value;
}

// -----------------------------------------------------------------------------
QString PostSlackMessage::getSlackUser() const
{
  return m_SlackUser;
}

// -----------------------------------------------------------------------------
void PostSlackMessage::setSlackUrl(const QString& value)
{
  m_SlackUrl = value;
}

// -----------------------------------------------------------------------------
QString PostSlackMessage::getSlackUrl() const
{
  return m_SlackUrl;
}

// -----------------------------------------------------------------------------
void PostSlackMessage::setSlackMessage(const QString& value)
{
  m_SlackMessage = value;
}

// -----------------------------------------------------------------------------
QString PostSlackMessage::getSlackMessage() const
{
  return m_SlackMessage;
}

// -----------------------------------------------------------------------------
void PostSlackMessage::setWarningsAsError(bool value)
{
  m_WarningsAsError = value;
}

// -----------------------------------------------------------------------------
bool PostSlackMessage::getWarningsAsError() const
{
  return m_WarningsAsError;
}
