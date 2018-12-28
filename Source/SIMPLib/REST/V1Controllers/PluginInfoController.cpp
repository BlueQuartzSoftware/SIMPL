/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Softwae, LLC
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
#include "PluginInfoController.h"

#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLPluginConstants.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#include <QtCore/QDateTime>
#include <QtCore/QVariant>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginInfoController::PluginInfoController(const QHostAddress& hostAddress, const int hostPort)
{
  setListenHost(hostAddress, hostPort);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginInfoController::createPluginJson(ISIMPLibPlugin* plugin, QJsonObject& rootObject)
{
  QJsonObject pluginObj;
  pluginObj[SIMPL::JSON::PluginFileName] = plugin->getPluginFileName();
  pluginObj[SIMPL::JSON::DisplayName] = plugin->getPluginDisplayName();
  pluginObj[SIMPL::JSON::Version] = plugin->getVersion();
  pluginObj[SIMPL::JSON::CompatibilityVersion] = plugin->getCompatibilityVersion();
  pluginObj[SIMPL::JSON::Vendor] = plugin->getVendor();
  pluginObj[SIMPL::JSON::URL] = plugin->getURL();
  pluginObj[SIMPL::JSON::Location] = plugin->getLocation();
  pluginObj[SIMPL::JSON::Description] = plugin->getDescription();
  pluginObj[SIMPL::JSON::Copyright] = plugin->getCopyright();
  pluginObj[SIMPL::JSON::License] = plugin->getLicense();

  rootObject[SIMPL::JSON::Plugin] = pluginObj;
  rootObject[SIMPL::JSON::ErrorCode] = 0;
  rootObject[SIMPL::JSON::ErrorMessage] = "";
  rootObject[SIMPL::JSON::PluginBaseName] = plugin->getPluginBaseName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginInfoController::service(HttpRequest& request, HttpResponse& response)
{
  QString content_type = request.getHeader(QByteArray("content-type"));

  QJsonObject responseJsonRootObj;
  response.setHeader("Content-Type", "application/json");

  if(content_type.compare("application/json") != 0)
  {
    // Form Error response
    QJsonObject rootObj;
    rootObj[SIMPL::JSON::ErrorMessage] = EndPoint() + ": Content Type is not application/json";
    rootObj[SIMPL::JSON::ErrorCode] = -20;
    QJsonDocument jdoc(rootObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  QString pluginName;

  QJsonParseError jsonParseError;
  QByteArray jsonBytes = request.getBody();
  QJsonDocument requestJsonDoc = QJsonDocument::fromJson(jsonBytes, &jsonParseError);
  if(jsonParseError.error != QJsonParseError::ParseError::NoError)
  {
    // Form Error response
    QJsonObject rootObj;
    rootObj[SIMPL::JSON::ErrorMessage] = tr("%1: JSON Request Parsing Error - %2").arg(EndPoint()).arg(jsonParseError.errorString());
    rootObj[SIMPL::JSON::ErrorCode] = -30;
    QJsonDocument jdoc(rootObj);
    response.write(jdoc.toJson(), true);
    return;
  }
  QJsonObject rootObject = requestJsonDoc.object();
  if(!rootObject.contains(SIMPL::JSON::PluginBaseName))
  {
    responseJsonRootObj[SIMPL::JSON::ErrorMessage] = "Key 'PluginBaseName' does not exist in the JSON payload.";
    responseJsonRootObj[SIMPL::JSON::ErrorCode] = -40;
    QJsonDocument jdoc(responseJsonRootObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  QJsonValue nameValue = rootObject.value(QString("PluginBaseName"));
  if(nameValue.isString())
  {
    pluginName = nameValue.toString();
  }
  else
  {
    responseJsonRootObj[SIMPL::JSON::ErrorMessage] = "Key 'PluginBaseName' does not have a paired value that is a string.";
    responseJsonRootObj[SIMPL::JSON::ErrorCode] = -50;
    QJsonDocument jdoc(responseJsonRootObj);
    response.write(jdoc.toJson(), true);
    return;
  }

  PluginManager* pm = PluginManager::Instance();
  ISIMPLibPlugin* plugin = pm->findPlugin(pluginName);

  if(nullptr == plugin)
  {
    responseJsonRootObj[SIMPL::JSON::ErrorCode] = -60;
    responseJsonRootObj[SIMPL::JSON::ErrorMessage] = "Plugin with name " + pluginName + " was not loaded or does not exist";
    QJsonDocument jdoc(responseJsonRootObj);
    response.write(jdoc.toJson(), true);
    return;
  }
  else
  {
    createPluginJson(plugin, responseJsonRootObj);
  }

  QJsonDocument jdoc(responseJsonRootObj);

  response.write(jdoc.toJson(), true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginInfoController::EndPoint()
{
  return QString("PluginInfo");
}
