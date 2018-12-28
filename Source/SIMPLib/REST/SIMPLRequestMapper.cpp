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
#include "SIMPLRequestMapper.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "QtWebApp/logging/filelogger.h"

#include "V1Controllers/SIMPLStaticFileController.h"
#include "V1Controllers/V1RequestMapper.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLRequestMapper::SIMPLRequestMapper(QObject* parent)
: HttpRequestHandler(parent)
{
  // qDebug() << "SIMPLRequestMapper: created";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLRequestMapper::~SIMPLRequestMapper()
{
  // qDebug() << "SIMPLRequestMapper: deleted";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRequestMapper::service(HttpRequest& request, HttpResponse& response)
{
  QString content_type = request.getHeader(QByteArray("content-type"));

  QByteArray path = request.getPath();
  // qDebug() << "SIMPLRequestMapper: path=%s"<< path.data();

  // For the following pathes, each request gets its own new instance of the related controller.
  if(path.startsWith("/api/v1"))
  {
    V1RequestMapper v1RequestMapper;
    v1RequestMapper.setListenHost(getListenHost(), getListenPort());
    v1RequestMapper.service(request, response);
  }
  else if(content_type.compare("application/json") != 0)
  {
    SIMPLStaticFileController* staticFileController = SIMPLStaticFileController::Instance();
    staticFileController->setListenHost(getListenHost(), getListenPort());
    staticFileController->service(request, response);
  }
  else
  {
    QJsonObject rootObj;
    QString msg;
    QTextStream ss(&msg);
    ss << "The end point '" << path << "' is not valid for this server. Please check your request settings.";

    rootObj["ErrorCode"] = -1;
    rootObj["ErrorMessage"] = msg;
    QJsonDocument jdoc(rootObj);
    response.write(jdoc.toJson(), true);
  }

  //  qDebug() << "SIMPLRequestMapper: finished request";

  // Clear the log buffer
  //    if (logger)
  //    {
  //       logger->clear();
  //    }
}
