/* ============================================================================
 * Copyright (c) 2016 BlueQuartz Software, LLC
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QtDebug>

#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"
#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("PreflightTest");

  QString filePath("/Users/mjackson/Workspace/DREAM3D-Build/Debug/Bin/Data/Output/SmallIN100Mesh_GBCD.dream3d");
  SIMPLH5DataReader::Pointer h5SimplReader = SIMPLH5DataReader::New();
  h5SimplReader->openFile(filePath);
  SIMPLH5DataReaderRequirements readerRequirements;
  int err = 0;
  DataContainerArrayProxy proxy = h5SimplReader->readDataContainerArrayStructure(&readerRequirements, err);

  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(filePath);
  reader->setOverwriteExistingDataContainers(true);
  reader->setInputFileDataContainerArrayProxy(proxy);
  
  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile("/tmp/cpp_CubicSingleEquiaxed.dream3d");
  writer->setWriteXdmfFile(true);
  
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  pipeline->pushBack(reader);
  pipeline->pushBack(writer);
  
  DataContainerArray::Pointer dca = pipeline->run();
  err = pipeline->getErrorCondition();

 
  // Load all the plugins and
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);
  // THIS IS A VERY IMPORTANT LINE: It will register all the known filters in the dream3d library. This
  // will NOT however get filters from plugins. We are going to have to figure out how to compile filters
  // into their own plugin and load the plugins from a command line.
  fm->RegisterKnownFilters(fm);

  QMetaObjectUtilities::RegisterMetaTypes();


  QJsonObject rootObj;

  PluginManager* pluginManager = PluginManager::Instance();
  QVector<ISIMPLibPlugin*> plugins = pluginManager->getPluginsVector();
  foreach(ISIMPLibPlugin* plugin, plugins)
  {
    QJsonObject jobj;

    QList<QString> filters = plugin->getFilters();

    QJsonArray jArray = QJsonArray::fromStringList(filters);

    foreach(QString filter, filters)
    {
      QJsonObject parameters;
      JsonFilterParametersWriter::Pointer writer = JsonFilterParametersWriter::New();

      AbstractFilter::Pointer fPtr = FilterManager::Instance()->getFactoryFromClassName(filter)->create();
      fPtr->writeFilterParameters(parameters);

      jobj[filter] = writer->getCurrentGroupObject();
    }
    // jobj["Filters"] = jArray;

    rootObj[plugin->getPluginBaseName()] = jobj;
  }

  QFile out(argv[1]);
  if(!out.open(QFile::WriteOnly))
  {
    qDebug() << "Error opening JSON file for writing. No output generated.";
    return EXIT_FAILURE;
  }

  qDebug() << "Writing JSON file.. ";
  QJsonDocument doc(rootObj);
  out.write(doc.toJson());
  out.close();

  return EXIT_SUCCESS;
}
