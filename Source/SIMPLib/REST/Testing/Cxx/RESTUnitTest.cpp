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
#include <QtCore/QProcess>

#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QHostAddress>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Plugin/SIMPLPluginConstants.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Testing/UnitTestSupport.hpp"

#include "RESTClient/SIMPLRestClient.h"

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
  int TestFilterAvailability()
  {
    // Now instantiate the CreateDataArray Filter from the FilterManager
    QString filtName = "CreateDataArray";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The RestUnitTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    // Now instantiate the CreateDataContainer Filter from the FilterManager
    filtName = "CreateDataContainer";
    filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The RestUnitTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    // Now instantiate the CreateDataContainer Filter from the FilterManager
    filtName = "CreateAttributeMatrix";
    fm = FilterManager::Instance();
    filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The RestUnitTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestCreateDataContainerAndAttributeMatrix()
  {

    int err = 0;
    bool propWasSet = false;
    // bool ok = false;
    QVariant var;

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Now instantiate the CreateDataArray Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();

    QString filtName = "CreateDataContainer";
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();

      filter->setDataContainerArray(dca);

      // Test 1 set int32 array with an initialization of -5 and read value for comparison
      var.setValue(QString::fromLatin1("Test Data Container"));
      propWasSet = filter->setProperty("CreatedDataContainer", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      filter->preflight();
      err = filter->getErrorCondition();
      DREAM3D_REQUIRED(err, >=, 0)
    }

    filtName = "CreateAttributeMatrix";
    filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();

      filter->setDataContainerArray(dca);

      var.setValue(QString::fromLatin1("Test Data Container"));
      propWasSet = filter->setProperty("CreatedDataContainer", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(QString::fromLatin1("Cell Attribute Matrix"));
      propWasSet = filter->setProperty("CreatedAttributeMatrix", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      filter->preflight();
      err = filter->getErrorCondition();
      DREAM3D_REQUIRED(err, >=, 0)
    }

    return EXIT_SUCCESS;
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
  bool TestExecutePipeline(QUrl url)
  {
    qDebug() << QObject::tr("Testing 'Execute Pipeline'...\n");

    qDebug() << QObject::tr("'Execute Pipeline' Test Complete!\n");
    return true;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool TestListFilterParameters(QUrl url)
  {
    qDebug() << QObject::tr("Testing 'List Filter Parameters'...\n");

    qDebug() << QObject::tr("'List Filter Parameters' Test Complete!\n");
    return true;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool TestLoadedPlugins(QUrl url)
  {
    qDebug() << QObject::tr("Testing 'Loaded Plugins'...\n");

    url.setPath("/api/v1/LoadedPlugins");

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

    QJsonParseError jsonParseError;
    QByteArray jsonResponse = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonResponse, &jsonParseError);
    if (jsonParseError.error != QJsonParseError::ParseError::NoError)
    {
      // Form Error response
      qDebug() << QObject::tr("Error: JSON Response Parsing Error - %2").arg(jsonParseError.errorString());
      DREAM3D_REQUIRE_EQUAL(jsonParseError.error, QJsonParseError::ParseError::NoError)
    }

    QJsonObject responseObject = doc.object();
    if (responseObject.isEmpty())
    {
      qDebug() << "Error: JSON Response is empty.";
      return false;
    }

    int responseErrorCode = responseObject[SIMPL::JSON::ErrorCode].toInt();
    if (responseErrorCode >= 0)
    {

    }

    if (!responseObject.contains(SIMPL::JSON::Plugins))
    {
      qDebug() << "Error: The response from the server does not include an object containing plugins.";
      return false;
    }
    else
    {
      QJsonArray pluginsArray = responseObject[SIMPL::JSON::Plugins].toArray();

    }

    std::string replyContent = jsonResponse.toStdString();

    std::cout << replyContent << std::endl;
    delete reply;
    delete connection;

    qDebug() << QObject::tr("'Loaded Plugins' Test Complete!\n");
    return true;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool TestNamesOfFilters(QUrl url)
  {
    qDebug() << QObject::tr("Testing 'Names Of Filters'...\n");

    qDebug() << QObject::tr("'Names Of Filters' Test Complete!\n");
    return true;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool TestNumFilters(QUrl url)
  {
    qDebug() << QObject::tr("Testing 'Num Filters'...\n");

    url.setPath("/api/v1/NumFilters");

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

    qDebug() << QObject::tr("'Num Filters' Test Complete!\n");
    return true;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool TestPluginInfo(QUrl url)
  {
    qDebug() << QObject::tr("Testing 'Plugin Info'...\n");

    qDebug() << QObject::tr("'Plugin Info' Test Complete!\n");
    return true;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool TestPreflightPipeline(QUrl url)
  {
    qDebug() << QObject::tr("Testing 'Preflight Pipeline'...\n");

    qDebug() << QObject::tr("'Preflight Pipeline' Test Complete!\n");
    return true;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool TestSIMPLibVersion(QUrl url)
  {
    qDebug() << QObject::tr("Testing 'SIMPLib Version'...\n");

    url.setPath("/api/v1/SIMPLibVersion");

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

    qDebug() << QObject::tr("'SIMPLib Version' Test Complete!\n");
    return true;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### RestUnitTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(RemoveTestFiles());

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

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

    // This must be changed to be made more generic!
    std::shared_ptr<QProcess> serverProcess = std::shared_ptr<QProcess>(new QProcess());
    serverProcess->start(UnitTest::RestUnitTest::RestServerExecutableFilePath);

//    DREAM3D_REGISTER_TEST(TestExecutePipeline(url));
//    DREAM3D_REGISTER_TEST(TestListFilterParameters(url));
//    DREAM3D_REGISTER_TEST(TestLoadedPlugins(url));
//    DREAM3D_REGISTER_TEST(TestNamesOfFilters(url));
//    DREAM3D_REGISTER_TEST(TestNumFilters(url));
//    DREAM3D_REGISTER_TEST(TestPluginInfo(url));
//    DREAM3D_REGISTER_TEST(TestPreflightPipeline(url));
//    DREAM3D_REGISTER_TEST(TestSIMPLibVersion(url));
  }

private:
  RESTUnitTest(const RESTUnitTest&); // Copy Constructor Not Implemented
  void operator=(const RESTUnitTest&);      // Move assignment Not Implemented
};
