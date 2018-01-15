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
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PostSlackMessage::PostSlackMessage()
: AbstractFilter()
, m_SlackUser("")
, m_SlackUrl("")
, m_SlackMessage("")
, m_WarningsAsError(false)
{
  m_SlackUser = "[DREAM.3D@" + QSysInfo::machineHostName() + "]";
  initialize();
  setupFilterParameters();
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
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PostSlackMessage::setupFilterParameters()
{
  FilterParameterVector parameters;

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
  setErrorCondition(0);
  setWarningCondition(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PostSlackMessage::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PostSlackMessage::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(getCancel() == true)
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
      setErrorCondition(-76000);
      QString ss = QObject::tr("Slack returned the following error code: %1").arg(getErrorCondition());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    else
    {
      setWarningCondition(-76001);
      QString ss = QObject::tr("Slack returned the following error code: %1").arg(getErrorCondition());
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
    }
  }

  delete reply;
  delete connection;

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PostSlackMessage::newFilterInstance(bool copyFilterParameters)
{
  PostSlackMessage::Pointer filter = PostSlackMessage::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PostSlackMessage::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PostSlackMessage::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PostSlackMessage::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PostSlackMessage::getGroupName()
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid PostSlackMessage::getUuid()
{
  return QUuid("{0ca83462-8564-54ea-9f4e-e5141974f30b}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PostSlackMessage::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PostSlackMessage::getHumanLabel()
{
  return "Post Slack Message";
}
