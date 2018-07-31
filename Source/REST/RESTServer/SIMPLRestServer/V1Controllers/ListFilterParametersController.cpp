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
#include "ListFilterParametersController.h"

#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Plugin/SIMPLPluginConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ListFilterParametersController::ListFilterParametersController(const QHostAddress& hostAddress, const int hostPort)
{
  setListenHost(hostAddress, hostPort);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ListFilterParametersController::createFilterParametersJson(const QString& filterName, QJsonObject& rootObject)
{
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();

  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);
  AbstractFilter::Pointer filter = factory->create();

  QVector<FilterParameter::Pointer> parameters = filter->getFilterParameters();
  QJsonArray jsonParameters;

  for(int i = 0; i < parameters.size(); i++)
  {
    FilterParameter::Pointer parameter = parameters[i];
    QJsonObject filterObj;
    filterObj[SIMPL::JSON::FilterParameterName] = parameter->getNameOfClass();
    filterObj[SIMPL::JSON::FilterParameterWidget] = parameter->getWidgetType();
    filterObj[SIMPL::JSON::FilterParameterCategory] = parameter->getCategory();
    filterObj[SIMPL::JSON::FilterParameterGroupIndex] = parameter->getGroupIndex();
    filterObj[SIMPL::JSON::FilterParameterHumanLabel] = parameter->getHumanLabel();
    filterObj[SIMPL::JSON::FilterParameterPropertyName] = parameter->getPropertyName();
    filterObj[SIMPL::JSON::FilterParameterReadOnly] = parameter->getReadOnly();
    jsonParameters.append(filterObj);
  }
  rootObject[SIMPL::JSON::FilterParameters] = jsonParameters;
  rootObject[SIMPL::JSON::ClassName] = filterName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ListFilterParametersController::service(HttpRequest& request, HttpResponse& response)
{

  QString content_type = request.getHeader(QByteArray("content-type"));

  QJsonObject responseJsonRootObj;

  response.setHeader("Content-Type", "application/json");

  if(content_type.compare("application/json") != 0)
  {
    // Form Error response
    responseJsonRootObj[SIMPL::JSON::ErrorMessage]  = EndPoint() + ": Content Type is not application/json";
    responseJsonRootObj[SIMPL::JSON::ErrorCode] = -20;
    QJsonDocument jdoc(responseJsonRootObj);

    response.write(jdoc.toJson(), true);
    return;
  }

  QString filterName;

  {
    QJsonParseError jsonParseError;
    QByteArray jsonBytes = request.getBody();
    QJsonDocument requestJsonDoc = QJsonDocument::fromJson(jsonBytes, &jsonParseError);
    QJsonObject rootObject = requestJsonDoc.object();
    QJsonValue nameValue = rootObject[SIMPL::JSON::ClassName];
    if(nameValue.isString())
    {
      filterName = nameValue.toString();
    }
  }
 

  if(filterName.isEmpty())
  {
    responseJsonRootObj[SIMPL::JSON::ErrorCode] = -30;
    responseJsonRootObj[SIMPL::JSON::ErrorMessage] = "Filter with name " + filterName + " was not loaded or does not exist";
  }
  else
  {
    createFilterParametersJson(filterName, responseJsonRootObj);
  }

  QJsonDocument jdoc(responseJsonRootObj);

  response.write(jdoc.toJson(), true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ListFilterParametersController::EndPoint()
{
  return QString("ListFilterParameters");
}
