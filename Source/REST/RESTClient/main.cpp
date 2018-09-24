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


#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QCoreApplication>
#include <QtCore/QLoggingCategory>

#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QHostAddress>

#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/Plugin/SIMPLPluginConstants.h"

#define REST_UI 1

#if REST_UI
#include "SIMPLRestClient_UI.h"
#endif

#include "Core/SIMPLRestClient.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestLoadedPlugins(QUrl url)
{

  url.setPath("/api/v1/LoadedPlugins");
  qDebug() << url;
  
  QByteArray data; // No actual Application data is required.
  
  QNetworkRequest netRequest;
  netRequest.setUrl(url);
  netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  
  QEventLoop waitLoop;
  QNetworkAccessManager* connection = new QNetworkAccessManager();
  QNetworkReply* reply = connection->post(netRequest, data);
  QObject::connect(reply, SIGNAL(finished()), &waitLoop, SLOT(quit()));
  waitLoop.exec();

  int errorCode = reply->error();
  qDebug() << "ErrorCode: " << errorCode;
  
  if(errorCode != 0)
  {
   qDebug() << "An error occurred requesting the loaded plugins: " << errorCode;
  }
  
  std::string replyContent = reply->readAll().toStdString();
  
  std::cout << replyContent << std::endl;
  delete reply;
  delete connection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestFilterCount(QUrl url)
{
  url.setPath("/api/v1/NumFilters");
  qDebug() << url;
  
  QByteArray data; // No actual Application data is required.
  
  QNetworkRequest netRequest;
  netRequest.setUrl(url);
  netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  
  QEventLoop waitLoop;
  QNetworkAccessManager* connection = new QNetworkAccessManager();
  QNetworkReply* reply = connection->post(netRequest, data);
  QObject::connect(reply, SIGNAL(finished()), &waitLoop, SLOT(quit()));
  waitLoop.exec();
  
  int errorCode = reply->error();
  qDebug() << "ErrorCode: " << errorCode;
  
  if(errorCode != 0)
  {
    qDebug() << "An error occurred requesting the loaded plugins: " << errorCode;
  }
  
  std::string replyContent = reply->readAll().toStdString();
  
  std::cout << replyContent << std::endl;
  delete reply;
  delete connection;
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestAvailableFilters(QUrl url)
{
  url.setPath("/api/v1/AvailableFilters");
  qDebug() << url;
  
  QByteArray data; // No actual Application data is required.
  
  QNetworkRequest netRequest;
  netRequest.setUrl(url);
  netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  
  QEventLoop waitLoop;
  QNetworkAccessManager* connection = new QNetworkAccessManager();
  QNetworkReply* reply = connection->post(netRequest, data);
  QObject::connect(reply, SIGNAL(finished()), &waitLoop, SLOT(quit()));
  waitLoop.exec();
  
  int errorCode = reply->error();
  qDebug() << "ErrorCode: " << errorCode;
  
  if(errorCode != 0)
  {
    qDebug() << "An error occurred requesting the loaded plugins: " << errorCode;
  }
  
  std::string replyContent = reply->readAll().toStdString();
  
  std::cout << replyContent << std::endl;
  delete reply;
  delete connection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestSIMPLibVersion(QUrl url)
{
  url.setPath("/api/v1/SIMPLibVersion");
  qDebug() << url;

  QByteArray data; // No actual Application data is required.

  QNetworkRequest netRequest;
  netRequest.setUrl(url);
  netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QEventLoop waitLoop;
  QNetworkAccessManager* connection = new QNetworkAccessManager();
  QNetworkReply* reply = connection->post(netRequest, data);
  QObject::connect(reply, SIGNAL(finished()), &waitLoop, SLOT(quit()));
  waitLoop.exec();

  int errorCode = reply->error();
  qDebug() << "ErrorCode: " << errorCode;

  if(errorCode != 0)
  {
    qDebug() << "An error occurred requesting the loaded plugins: " << errorCode;
  }

  std::string replyContent = reply->readAll().toStdString();

  std::cout << replyContent << std::endl;
  delete reply;
  delete connection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  #if REST_UI
  QApplication app(argc, argv);
  #else
  QCoreApplication app(argc, argv);
  #endif

  app.setApplicationVersion("1.0.0");
  app.setOrganizationName("BlueQuartz Software");
  app.setApplicationName("REST API Communication");

#if defined(Q_OS_MAC)
#if REST_UI
  QGuiApplication::setQuitOnLastWindowClosed(false);
#endif
#endif

#if REST_UI
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

  SIMPLRestClient_UI* ui = new SIMPLRestClient_UI(nullptr);
  ui->show();

  return app.exec();
  
#else
  SIMPLRestClient client(nullptr);
  QUrl url;
  for (auto address : QNetworkInterface::allAddresses())
  {
    if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
    {
      url.setHost(address.toString());
      break;
    }

  }
  url.setScheme("http");
  url.setPort(8080);

  TestLoadedPlugins(url);
  TestFilterCount(url);
  TestAvailableFilters(url);
  TestSIMPLibVersion(url);
#endif
}
