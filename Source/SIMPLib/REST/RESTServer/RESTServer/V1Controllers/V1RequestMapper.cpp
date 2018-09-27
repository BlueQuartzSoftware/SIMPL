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
#include "V1RequestMapper.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QJsonDocument>

#include "QtWebApp/logging/filelogger.h"

#include "ApiNotFoundController.h"
#include "ExecutePipelineController.h"
#include "ListFilterParametersController.h"
#include "LoadedPluginsController.h"
#include "NamesOfFiltersController.h"
#include "NumFiltersController.h"
#include "PluginInfoController.h"
#include "PreflightPipelineController.h"
#include "SIMPLStaticFileController.h"
#include "SIMPLibVersionController.h"

/** Redirects log messages to a file */
extern FileLogger* logger;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
V1RequestMapper::V1RequestMapper(QObject* parent)
: HttpRequestHandler(parent)
{
  // qDebug() <<"V1RequestMapper: created";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
V1RequestMapper::~V1RequestMapper()
{
  // qDebug() << "V1RequestMapper: deleted";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void V1RequestMapper::service(HttpRequest& request, HttpResponse& response)
{
  QMultiMap<QByteArray, QByteArray> headerMap = request.getHeaderMap();

  QString path(request.getPath());
  qDebug() << "V1RequestMapper: path=" << QString(path.data());

  // For the following paths, each request gets its own new instance of the related controller.

  if(path.endsWith(ExecutePipelineController::EndPoint()))
  {
    ExecutePipelineController(getListenHost(), getListenPort()).service(request, response);
  }
  else if(path.endsWith(ListFilterParametersController::EndPoint()))
  {
    ListFilterParametersController(getListenHost(), getListenPort()).service(request, response);
  }
  else if(path.endsWith(SIMPLibVersionController::EndPoint()))
  {
    SIMPLibVersionController(getListenHost(), getListenPort()).service(request, response);
  }
  else if(path.endsWith(LoadedPluginsController::EndPoint()))
  {
    LoadedPluginsController(getListenHost(), getListenPort()).service(request, response);
  }
  else if(path.endsWith(NamesOfFiltersController::EndPoint()))
  {
    NamesOfFiltersController(getListenHost(), getListenPort()).service(request, response);
  }
  else if(path.endsWith(NumFiltersController::EndPoint()))
  {
    NumFiltersController(getListenHost(), getListenPort()).service(request, response);
  }
  else if(path.endsWith(PluginInfoController::EndPoint()))
  {
    PluginInfoController(getListenHost(), getListenPort()).service(request, response);
  }
  else if(path.endsWith(PreflightPipelineController::EndPoint()))
  {
    PreflightPipelineController(getListenHost(), getListenPort()).service(request, response);
  }
  // All other pathes are mapped to the static file controller.
  // In this case, a single instance is used for multiple requests.
  else
  {
    SIMPLStaticFileController* staticFileController = SIMPLStaticFileController::Instance();
    staticFileController->setListenHost(getListenHost(), getListenPort());
    staticFileController->service(request, response);
  }

  qDebug() << "V1RequestMapper: finished request";

  // Clear the log buffer
  //    if (logger)
  //    {
  //       logger->clear();
  //    }
}
