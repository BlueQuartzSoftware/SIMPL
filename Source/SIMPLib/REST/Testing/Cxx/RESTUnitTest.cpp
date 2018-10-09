/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QUrl>
#include <QtCore/QJsonParseError>
#include <QtCore/QEventLoop>
#include <QtCore/QSettings>
#include <QtCore/QMimeDatabase>

#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QHttpPart>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/CreateAttributeMatrix.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLPluginConstants.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"

#include "SIMPLib/Testing/UnitTestSupport.hpp"

#include "RESTClient/SIMPLRestClient.h"

#include "REST/RESTServer/V1Controllers/SIMPLStaticFileController.h"
#include "REST/RESTServer/SIMPLRequestMapper.h"
#include "REST/RESTServer/PipelineListener.h"

#include "QtWebApp/httpserver/httplistener.h"
#include "QtWebApp/httpserver/httpsessionstore.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"

class RESTUnitTest
{
public:
  RESTUnitTest() = default;
  virtual ~RESTUnitTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
//    bool success = QFile::remove(UnitTest::RestTest::InputPipelineFile);
//    DREAM3D_REQUIRE_EQUAL(success, true);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  QUrl getConnectionURL()
  {
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

    return url;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  QSharedPointer<QNetworkReply> sendRequest(QUrl url, QString contentType, QByteArray data)
  {
    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, contentType);

    QEventLoop waitLoop;
    QSharedPointer<QNetworkReply> reply = QSharedPointer<QNetworkReply>(m_Connection->post(netRequest, data));
    QObject::connect(reply.data(), SIGNAL(finished()), &waitLoop, SLOT(quit()));
    waitLoop.exec();

    return reply;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  QSharedPointer<QNetworkReply> sendRequest(QUrl url, QHttpMultiPart* multiPart)
  {
    QNetworkRequest netRequest(url);

    QEventLoop waitLoop;
    QSharedPointer<QNetworkReply> reply = QSharedPointer<QNetworkReply>(m_Connection->post(netRequest, multiPart));
    multiPart->setParent(reply.data());
    QObject::connect(reply.data(), SIGNAL(finished()), &waitLoop, SLOT(quit()));
//    connect(reply.data(), SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(uploadProgress(qint64, qint64)));
    waitLoop.exec();

    return reply;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer CreateDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer m = DataContainer::New(SIMPL::Defaults::DataContainerName);
    dca->addDataContainer(m);
    AttributeMatrix::Pointer attrMatrix = AttributeMatrix::New(QVector<size_t>(1, 1), SIMPL::Defaults::AttributeMatrixName, AttributeMatrix::Type::Generic);
    m->addAttributeMatrix(SIMPL::Defaults::AttributeMatrixName, attrMatrix);
    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestExecutePipeline()
  {    
    QUrl url = getConnectionURL();

    url.setPath("/api/v1/ExecutePipeline");

    QByteArray data; // No actual Application data is required.

    // Test 'Incorrect Content Type'
    {
      QByteArray data;

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "text/plain", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -20);
    }

    // Test 'JSON Parse Error'
    {
      QByteArray data = QByteArray::fromStdString("{ CreateAttributeMatrix");

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -30);
    }

    // Test 'Missing Pipeline Object'
    {
      QJsonObject rootObj;
      rootObj["Foo"] = "{ }";
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -40);
    }

    // Test 'Pipeline Could Not Be Created'
    {
      QJsonObject rootObj;
      rootObj[SIMPL::JSON::Pipeline] = 2;
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -50);
    }

    // Test Pipeline Execution
    {
      QFile file(UnitTest::RestUnitTest::InputPipelineFilePath);
      DREAM3D_REQUIRE_EQUAL(file.open(QIODevice::ReadOnly), true);

      QTextStream in(&file);
      QString jsonString = in.readAll();
      QByteArray jsonByteArray = QByteArray::fromStdString(jsonString.toStdString());
      QJsonDocument doc = QJsonDocument::fromJson(jsonByteArray);

      QJsonObject rootObj;
      rootObj[SIMPL::JSON::Pipeline] = doc.object();

      QJsonDocument doc2(rootObj);
      QByteArray data = doc2.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 5);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Completed), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Completed].isBool(), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Completed].toBool(), true);

      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Errors), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Errors].isArray(), true);

      QJsonArray responseErrorsArray = responseObject[SIMPL::JSON::Errors].toArray();
      DREAM3D_REQUIRE_EQUAL(responseErrorsArray.size(), 0);

      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Warnings].isArray(), true);
      QJsonArray responseWarningsArray = responseObject[SIMPL::JSON::Warnings].toArray();
      DREAM3D_REQUIRE_EQUAL(responseWarningsArray.size(), 0);

      JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
      FilterPipeline::Pointer pipeline = reader->readPipelineFromFile(UnitTest::RestUnitTest::InputPipelineFilePath);

      PipelineListener listener(nullptr);
      pipeline->addMessageReceiver(&listener);

      pipeline->execute();

      std::vector<PipelineMessage> warningMessages = listener.getWarningMessages();
      DREAM3D_REQUIRE_EQUAL(warningMessages.size(), 0);

      std::vector<PipelineMessage> errorMessages = listener.getErrorMessages();
      DREAM3D_REQUIRE_EQUAL(errorMessages.size(), 0);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestExecutePipelineWithFiles()
  {
    QUrl url = getConnectionURL();

    url.setPath("/api/v1/ExecutePipeline");

    QStringList filePathList;
    filePathList.push_back(UnitTest::RestUnitTest::RESTDCADataFilePath);

    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    {
      QFile inputFile(UnitTest::RestUnitTest::RESTCreateDCAPipelineFilePath);
      DREAM3D_REQUIRE_EQUAL(inputFile.open(QIODevice::ReadOnly), true);

      QTextStream in(&inputFile);
      QString jsonString = in.readAll();
      QByteArray jsonByteArray = QByteArray::fromStdString(jsonString.toStdString());
      QJsonDocument doc = QJsonDocument::fromJson(jsonByteArray);

      QJsonObject rootObj;
      QJsonObject pipelineObj = doc.object();

      QJsonObject filterObj = pipelineObj["0"].toObject();
      filterObj["InputFile"] = "@@Replacement1@@";
      pipelineObj["0"] = filterObj;

      rootObj[SIMPL::JSON::Pipeline] = pipelineObj;

      QJsonDocument doc2(rootObj);
      QByteArray jsonData = doc2.toJson();

      QHttpPart jsonPart;
      jsonPart.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
      jsonPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"json\""));

      jsonPart.setBody(jsonData);

      multiPart->append(jsonPart);
    }

    {
      QHttpPart pipelineReplacementLookupPart;
      pipelineReplacementLookupPart.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
      pipelineReplacementLookupPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"pipelineReplacementLookup\""));

      QJsonObject rootObj;
      QJsonObject pipelineReplacementObject;
      pipelineReplacementObject["Type"] = "File";

      QJsonArray fileParameterNames;
      for (int i = 0; i < filePathList.size(); i++)
      {
        fileParameterNames.push_back(QObject::tr("DataFile%1").arg(i));
      }

      pipelineReplacementObject["FileParameterNames"] = fileParameterNames;

      rootObj["@@Replacement1@@"] = pipelineReplacementObject;

      QJsonDocument doc(rootObj);

      pipelineReplacementLookupPart.setBody(doc.toJson());

      multiPart->append(pipelineReplacementLookupPart);
    }

    for (int i = 0; i < filePathList.size(); i++)
    {
      QString filePath = filePathList[i];

      QHttpPart dataPart;
      QMimeDatabase mimeDb;
      QMimeType mimeType = mimeDb.mimeTypeForFile(filePath);
      QString contentType = mimeType.name();
      dataPart.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
      dataPart.setHeader(QNetworkRequest::ContentDispositionHeader, QObject::tr("form-data; name=\"DataFile%1\"").arg(i));
      QFile* file = new QFile(filePath);
      file->open(QIODevice::ReadOnly);
      dataPart.setBodyDevice(file);
      file->setParent(multiPart);

      multiPart->append(dataPart);
    }

    QSharedPointer<QNetworkReply> reply = sendRequest(url, multiPart);
    DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

    QJsonParseError jsonParseError;
    QByteArray jsonResponse = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
    DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

    QJsonObject responseObject = doc.object();
    DREAM3D_REQUIRE_EQUAL(responseObject.size(), 7);
    DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Completed), true);
    DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Completed].isBool(), true);
    DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Completed].toBool(), true);

    DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Errors), true);
    DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Errors].isArray(), true);

    QJsonArray responseErrorsArray = responseObject[SIMPL::JSON::Errors].toArray();
    DREAM3D_REQUIRE_EQUAL(responseErrorsArray.size(), 0);

    DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Warnings].isArray(), true);
    QJsonArray responseWarningsArray = responseObject[SIMPL::JSON::Warnings].toArray();
    DREAM3D_REQUIRE_EQUAL(responseWarningsArray.size(), 0);

    JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
    FilterPipeline::Pointer pipeline = reader->readPipelineFromFile(UnitTest::RestUnitTest::InputPipelineFilePath);

    PipelineListener listener(nullptr);
    pipeline->addMessageReceiver(&listener);

    pipeline->execute();

    std::vector<PipelineMessage> warningMessages = listener.getWarningMessages();
    DREAM3D_REQUIRE_EQUAL(warningMessages.size(), 0);

    std::vector<PipelineMessage> errorMessages = listener.getErrorMessages();
    DREAM3D_REQUIRE_EQUAL(errorMessages.size(), 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestListFilterParameters()
  {    
    QUrl url = getConnectionURL();

    url.setPath("/api/v1/ListFilterParameters");

    // Test 'Incorrect Content Type'
    {
      QByteArray data;

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "text/plain", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -20);
    }

    // Test 'JSON Parse Error'
    {
      QByteArray data = QByteArray::fromStdString("{ CreateAttributeMatrix");

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -30);
    }

    // Test 'Missing Filter Class Name Object'
    {
      QJsonObject rootObj;
      rootObj["Foo"] = "CreateAttributeMatrix";
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -40);
    }

    // Test 'Class Name Object Not a String'
    {
      QJsonObject rootObj;
      rootObj[SIMPL::JSON::ClassName] = 2;
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -50);
    }

    // Test 'Empty Class Name'
    {
      QJsonObject rootObj;
      rootObj[SIMPL::JSON::ClassName] = "";
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -60);
    }

    // Test 'Unidentified Filter Name'
    {
      QJsonObject rootObj;
      rootObj[SIMPL::JSON::ClassName] = "FooBarBaz";
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -70);
    }

    // Test correct outcome
    {
      QJsonObject rootObj;
      rootObj[SIMPL::JSON::ClassName] = "CreateAttributeMatrix";
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 4);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), 0);

      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::FilterParameters), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::FilterParameters].isArray(), true);

      QJsonArray responseFPArray = responseObject[SIMPL::JSON::FilterParameters].toArray();

      CreateAttributeMatrix::Pointer filter = CreateAttributeMatrix::New();
      QVector<FilterParameter::Pointer> parameters = filter->getFilterParameters();

      DREAM3D_REQUIRE_EQUAL(responseFPArray.size(), parameters.size());

      for (int i = 0; i < responseFPArray.size(); i++)
      {
        DREAM3D_REQUIRE_EQUAL(responseFPArray[i].isObject(), true);

        QJsonObject responseFPObject = responseFPArray[i].toObject();

        DREAM3D_REQUIRE_EQUAL(responseFPObject.contains(SIMPL::JSON::FilterParameterName), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterName].isString(), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterName].toString(), parameters[i]->getNameOfClass());

        DREAM3D_REQUIRE_EQUAL(responseFPObject.contains(SIMPL::JSON::FilterParameterWidget), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterWidget].isString(), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterWidget].toString(), parameters[i]->getWidgetType());

        DREAM3D_REQUIRE_EQUAL(responseFPObject.contains(SIMPL::JSON::FilterParameterCategory), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterCategory].isDouble(), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterCategory].toInt(), parameters[i]->getCategory());

        DREAM3D_REQUIRE_EQUAL(responseFPObject.contains(SIMPL::JSON::FilterParameterGroupIndex), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterGroupIndex].isDouble(), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterGroupIndex].toInt(), parameters[i]->getGroupIndex());

        DREAM3D_REQUIRE_EQUAL(responseFPObject.contains(SIMPL::JSON::FilterParameterHumanLabel), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterHumanLabel].isString(), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterHumanLabel].toString(), parameters[i]->getHumanLabel());

        DREAM3D_REQUIRE_EQUAL(responseFPObject.contains(SIMPL::JSON::FilterParameterPropertyName), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterPropertyName].isString(), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterPropertyName].toString(), parameters[i]->getPropertyName());

        DREAM3D_REQUIRE_EQUAL(responseFPObject.contains(SIMPL::JSON::FilterParameterReadOnly), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterReadOnly].isBool(), true);
        DREAM3D_REQUIRE_EQUAL(responseFPObject[SIMPL::JSON::FilterParameterReadOnly].toBool(), parameters[i]->getReadOnly());
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestLoadedPlugins()
  {    
    QUrl url = getConnectionURL();

    url.setPath("/api/v1/LoadedPlugins");

    QByteArray data; // No actual Application data is required.

    // Test incorrect content type
    {
      QSharedPointer<QNetworkReply> reply = sendRequest(url, "text/plain", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -20);
    }

    {
      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), 0);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Plugins), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Plugins].isArray(), true);

      QJsonArray responsePluginsArray = responseObject[SIMPL::JSON::Plugins].toArray();

      PluginManager* pluginManager = PluginManager::Instance();
      QJsonArray pluginManagerArray = pluginManager->toJsonArray();
      DREAM3D_REQUIRE_EQUAL(pluginManagerArray.size(), responsePluginsArray.size());

      for (int i = 0; i < responsePluginsArray.size(); i++)
      {
        DREAM3D_REQUIRE_EQUAL(responsePluginsArray[i].isObject(), true);

        QJsonObject responsePluginObject = responsePluginsArray[i].toObject();
        DREAM3D_REQUIRE_EQUAL(pluginManagerArray.contains(responsePluginObject), true);
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestNamesOfFilters()
  {
    QUrl url = getConnectionURL();

    url.setPath("/api/v1/AvailableFilters");

    QByteArray data; // No actual Application data is required.

    // Test incorrect content type
    {
      QSharedPointer<QNetworkReply> reply = sendRequest(url, "text/plain", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -20);
    }

    {
      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), 0);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Filters), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Filters].isArray(), true);

      QJsonArray responseFiltersArray = responseObject[SIMPL::JSON::Filters].toArray();

      FilterManager* fm = FilterManager::Instance();
      QJsonArray filterManagerArray = fm->toJsonArray();
      DREAM3D_REQUIRE_EQUAL(filterManagerArray.size(), responseFiltersArray.size());

      for (int i = 0; i < responseFiltersArray.size(); i++)
      {
        DREAM3D_REQUIRE_EQUAL(responseFiltersArray[i].isObject(), true);

        QJsonObject responseFilterObject = responseFiltersArray[i].toObject();
        DREAM3D_REQUIRE_EQUAL(filterManagerArray.contains(responseFilterObject), true);
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestNumFilters()
  {
    QUrl url = getConnectionURL();

    url.setPath("/api/v1/NumFilters");

    QByteArray data; // No actual Application data is required.

    // Test incorrect content type
    {
      QSharedPointer<QNetworkReply> reply = sendRequest(url, "text/plain", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -20);
    }

    {
      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), 0);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::NumFilters), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::NumFilters].isDouble(), true);

      int responseNumFilters = responseObject[SIMPL::JSON::NumFilters].toInt();

      FilterManager* fm = FilterManager::Instance();
      FilterManager::Collection factories = fm->getFactories();
      DREAM3D_REQUIRE_EQUAL(responseNumFilters, factories.size());
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestPluginInfo()
  {
    QUrl url = getConnectionURL();

    url.setPath("/api/v1/PluginInfo");

    // Test 'Incorrect Content Type'
    {
      QByteArray data;

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "text/plain", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -20);
    }

    // Test 'JSON Parse Error'
    {
      QByteArray data = QByteArray::fromStdString("{ CreateAttributeMatrix");

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -30);
    }

    // Test 'Missing Filter Class Name Object'
    {
      QJsonObject rootObj;
      rootObj["Foo"] = "Generic";
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -40);
    }

    // Test 'Class Name Object Not a String'
    {
      QJsonObject rootObj;
      rootObj[SIMPL::JSON::PluginBaseName] = 2;
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -50);
    }

    // Test 'Empty Class Name'
    {
      QJsonObject rootObj;
      rootObj[SIMPL::JSON::PluginBaseName] = "";
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -60);
    }

    // Test 'Unidentified Filter Name'
    {
      QJsonObject rootObj;
      rootObj[SIMPL::JSON::PluginBaseName] = "FooBarBaz";
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -60);
    }

    // Test correct outcome
    {
      QJsonObject rootObj;
      QString pluginName = "Generic";
      rootObj[SIMPL::JSON::PluginBaseName] = pluginName;
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 4);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), 0);

      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Plugin), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Plugin].isObject(), true);

      QJsonObject responseFPObj = responseObject[SIMPL::JSON::Plugin].toObject();

      PluginManager* pm = PluginManager::Instance();
      ISIMPLibPlugin* plugin = pm->findPlugin(pluginName);
      DREAM3D_REQUIRED_PTR(plugin, !=, nullptr);

      DREAM3D_REQUIRE_EQUAL(responseFPObj.contains(SIMPL::JSON::PluginFileName), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::PluginFileName].isString(), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::PluginFileName].toString(), plugin->getPluginFileName());

      DREAM3D_REQUIRE_EQUAL(responseFPObj.contains(SIMPL::JSON::DisplayName), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::DisplayName].isString(), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::DisplayName].toString(), plugin->getPluginDisplayName());

      DREAM3D_REQUIRE_EQUAL(responseFPObj.contains(SIMPL::JSON::Version), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::Version].isString(), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::Version].toString(), plugin->getVersion());

      DREAM3D_REQUIRE_EQUAL(responseFPObj.contains(SIMPL::JSON::CompatibilityVersion), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::CompatibilityVersion].isString(), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::CompatibilityVersion].toString(), plugin->getCompatibilityVersion());

      DREAM3D_REQUIRE_EQUAL(responseFPObj.contains(SIMPL::JSON::Vendor), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::Vendor].isString(), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::Vendor].toString(), plugin->getVendor());

      DREAM3D_REQUIRE_EQUAL(responseFPObj.contains(SIMPL::JSON::URL), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::URL].isString(), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::URL].toString(), plugin->getURL());

      DREAM3D_REQUIRE_EQUAL(responseFPObj.contains(SIMPL::JSON::Location), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::Location].isString(), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::Location].toString(), plugin->getLocation());

      DREAM3D_REQUIRE_EQUAL(responseFPObj.contains(SIMPL::JSON::Description), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::Description].isString(), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::Description].toString(), plugin->getDescription());

      DREAM3D_REQUIRE_EQUAL(responseFPObj.contains(SIMPL::JSON::Copyright), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::Copyright].isString(), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::Copyright].toString(), plugin->getCopyright());

      DREAM3D_REQUIRE_EQUAL(responseFPObj.contains(SIMPL::JSON::License), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::License].isString(), true);
      DREAM3D_REQUIRE_EQUAL(responseFPObj[SIMPL::JSON::License].toString(), plugin->getLicense());
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestPreflightPipeline()
  {
    QUrl url = getConnectionURL();

    url.setPath("/api/v1/PreflightPipeline");

    QByteArray data; // No actual Application data is required.

    // Test 'Incorrect Content Type'
    {
      QByteArray data;

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "text/plain", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -20);
    }

    // Test 'JSON Parse Error'
    {
      QByteArray data = QByteArray::fromStdString("{ CreateAttributeMatrix");

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -30);
    }

    // Test 'Missing Pipeline Object'
    {
      QJsonObject rootObj;
      rootObj["Foo"] = "{ }";
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -40);
    }

    // Test 'Pipeline Could Not Be Created'
    {
      QJsonObject rootObj;
      rootObj[SIMPL::JSON::Pipeline] = 2;
      QJsonDocument doc(rootObj);
      QByteArray data = doc.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -50);
    }

    // Test pipeline that has errors
    {
      QFile file(UnitTest::RestUnitTest::ErrorInputPipelineFilePath);
      DREAM3D_REQUIRE_EQUAL(file.open(QIODevice::ReadOnly), true);

      QTextStream in(&file);
      QString jsonString = in.readAll();
      QByteArray jsonByteArray = QByteArray::fromStdString(jsonString.toStdString());
      QJsonDocument doc = QJsonDocument::fromJson(jsonByteArray);

      QJsonObject rootObj;
      rootObj[SIMPL::JSON::Pipeline] = doc.object();

      QJsonDocument doc2(rootObj);
      QByteArray data = doc2.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 5);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Completed), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Completed].isBool(), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Completed].toBool(), false);

      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Errors), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Errors].isArray(), true);

      QJsonArray responseErrorsArray = responseObject[SIMPL::JSON::Errors].toArray();
      DREAM3D_REQUIRE_EQUAL(responseErrorsArray.size(), 3);

      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Warnings].isArray(), true);
      QJsonArray responseWarningsArray = responseObject[SIMPL::JSON::Warnings].toArray();
      DREAM3D_REQUIRE_EQUAL(responseWarningsArray.size(), 0);

      JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
      FilterPipeline::Pointer pipeline = reader->readPipelineFromFile(UnitTest::RestUnitTest::ErrorInputPipelineFilePath);

      PipelineListener listener(nullptr);
      pipeline->addMessageReceiver(&listener);

      pipeline->preflightPipeline();

      std::vector<PipelineMessage> warningMessages = listener.getWarningMessages();
      DREAM3D_REQUIRE_EQUAL(warningMessages.size(), 0);

      std::vector<PipelineMessage> errorMessages = listener.getErrorMessages();
      DREAM3D_REQUIRED(errorMessages.size(), >, 0);

      for (int i = 0; i < responseErrorsArray.size(); i++)
      {
        DREAM3D_REQUIRE_EQUAL(responseErrorsArray[i].isObject(), true);

        QJsonObject responseErrorObject = responseErrorsArray[i].toObject();
        DREAM3D_REQUIRE_EQUAL(responseErrorObject[SIMPL::JSON::Code].isDouble(), true);
        DREAM3D_REQUIRE_EQUAL(responseErrorObject[SIMPL::JSON::Message].isString(), true);
        DREAM3D_REQUIRE_EQUAL(responseErrorObject[SIMPL::JSON::FilterHumanLabel].isString(), true);
        DREAM3D_REQUIRE_EQUAL(responseErrorObject[SIMPL::JSON::FilterIndex].isDouble(), true);

        int responseErrorCode = responseErrorObject[SIMPL::JSON::Code].toInt();
        PipelineMessage errorMessage = errorMessages[i];
        DREAM3D_REQUIRE_EQUAL(responseErrorCode, errorMessage.getCode());
      }
    }

    // Test pipeline with no errors
    {
      QFile file(UnitTest::RestUnitTest::InputPipelineFilePath);
      DREAM3D_REQUIRE_EQUAL(file.open(QIODevice::ReadOnly), true);

      QTextStream in(&file);
      QString jsonString = in.readAll();
      QByteArray jsonByteArray = QByteArray::fromStdString(jsonString.toStdString());
      QJsonDocument doc = QJsonDocument::fromJson(jsonByteArray);

      QJsonObject rootObj;
      rootObj[SIMPL::JSON::Pipeline] = doc.object();

      QJsonDocument doc2(rootObj);
      QByteArray data = doc2.toJson();

      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 5);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Completed), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Completed].isBool(), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Completed].toBool(), true);

      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Errors), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Errors].isArray(), true);

      QJsonArray responseErrorsArray = responseObject[SIMPL::JSON::Errors].toArray();
      DREAM3D_REQUIRE_EQUAL(responseErrorsArray.size(), 0);

      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Warnings].isArray(), true);
      QJsonArray responseWarningsArray = responseObject[SIMPL::JSON::Warnings].toArray();
      DREAM3D_REQUIRE_EQUAL(responseWarningsArray.size(), 0);

      JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
      FilterPipeline::Pointer pipeline = reader->readPipelineFromFile(UnitTest::RestUnitTest::InputPipelineFilePath);

      PipelineListener listener(nullptr);
      pipeline->addMessageReceiver(&listener);

      pipeline->preflightPipeline();

      std::vector<PipelineMessage> warningMessages = listener.getWarningMessages();
      DREAM3D_REQUIRE_EQUAL(warningMessages.size(), 0);

      std::vector<PipelineMessage> errorMessages = listener.getErrorMessages();
      DREAM3D_REQUIRE_EQUAL(errorMessages.size(), 0);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestSIMPLibVersion()
  {
    QUrl url = getConnectionURL();

    url.setPath("/api/v1/SIMPLibVersion");

    QByteArray data; // No actual Application data is required.

    // Test incorrect content type
    {
      QSharedPointer<QNetworkReply> reply = sendRequest(url, "text/plain", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 2);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), -20);
    }

    {
      QSharedPointer<QNetworkReply> reply = sendRequest(url, "application/json", data);
      DREAM3D_REQUIRE_EQUAL(reply->error(), 0);

      QJsonParseError jsonParseError;
      QByteArray jsonResponse = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError);

      QJsonObject responseObject = doc.object();
      DREAM3D_REQUIRE_EQUAL(responseObject.size(), 3);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::ErrorCode), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::ErrorCode].toInt(), 0);
      DREAM3D_REQUIRE_EQUAL(responseObject.contains(SIMPL::JSON::Version), true);
      DREAM3D_REQUIRE_EQUAL(responseObject[SIMPL::JSON::Version].isString(), true);

      QString responseVersion = responseObject[SIMPL::JSON::Version].toString();

      DREAM3D_REQUIRE_EQUAL(responseVersion, SIMPLib::Version::Complete());
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void registerFilters()
  {
    // Register all the filters including trying to load those from Plugins
    FilterManager* fm = FilterManager::Instance();
    SIMPLibPluginLoader::LoadPluginFilters(fm);
    //
    QMetaObjectUtilities::RegisterMetaTypes();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void startServer()
  {
    // Find the configuration file
    QString configFileName = UnitTest::RestUnitTest::RestServerConfigFilePath;

    // Configure session store
    QSettings* sessionSettings = new QSettings(configFileName, QSettings::IniFormat);
    sessionSettings->beginGroup("sessions");
    m_SessionStore = QSharedPointer<HttpSessionStore>(HttpSessionStore::CreateInstance(sessionSettings));

    // Configure static file controller
    QSettings* fileSettings = new QSettings(configFileName, QSettings::IniFormat);
    fileSettings->beginGroup("docroot");
    SIMPLStaticFileController::CreateInstance(fileSettings);

    // Configure and start the TCP listener
    QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat);
    listenerSettings->beginGroup("listener");
    m_HttpListener = QSharedPointer<HttpListener>(new HttpListener(listenerSettings, new SIMPLRequestMapper()));

    m_Connection = QSharedPointer<QNetworkAccessManager>(new QNetworkAccessManager());
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void endServer()
  {
    m_SessionStore = QSharedPointer<HttpSessionStore>();
    m_HttpListener = QSharedPointer<HttpListener>();
    m_Connection = QSharedPointer<QNetworkAccessManager>();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### RestUnitTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(RemoveTestFiles());

    registerFilters();

    startServer();

    DREAM3D_REGISTER_TEST(TestExecutePipelineWithFiles());

//    DREAM3D_REGISTER_TEST(TestExecutePipeline());
//    DREAM3D_REGISTER_TEST(TestListFilterParameters());
//    DREAM3D_REGISTER_TEST(TestLoadedPlugins());
//    DREAM3D_REGISTER_TEST(TestNamesOfFilters());
//    DREAM3D_REGISTER_TEST(TestNumFilters());
//    DREAM3D_REGISTER_TEST(TestPluginInfo());
//    DREAM3D_REGISTER_TEST(TestPreflightPipeline());
//    DREAM3D_REGISTER_TEST(TestSIMPLibVersion());

    endServer();
  }

private:
  QSharedPointer<HttpSessionStore> m_SessionStore;
  QSharedPointer<HttpListener> m_HttpListener;

  QSharedPointer<QNetworkAccessManager> m_Connection;

  RESTUnitTest(const RESTUnitTest&); // Copy Constructor Not Implemented
  void operator=(const RESTUnitTest&);      // Move assignment Not Implemented
};
