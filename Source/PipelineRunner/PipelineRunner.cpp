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

// C Includes
#include <cassert>
#include <cstdlib>

// C++ Includes
#include <iostream>

// Qt Includes
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QString>

// DREAM3DLib includes
#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#ifdef SIMPL_EMBED_PYTHON
// undef slots since a Python header uses slots
#undef slots

#include <pybind11/embed.h>

#include "SIMPLib/Python/PythonLoader.h"
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
#ifdef SIMPL_EMBED_PYTHON
  if(!PythonLoader::checkPythonHome())
  {
    std::cout << "\"PYTHONHOME\" not set. This environment variable must be set for embedded Python to work.";
    return 2;
  }

  pybind11::scoped_interpreter interpreter_guard{};
#endif

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("PipelineRunner");
  QCoreApplication::setApplicationVersion(SIMPLib::Version::Major() + "." + SIMPLib::Version::Minor() + "." + SIMPLib::Version::Patch());

  QCommandLineParser parser;
  QString str;
  QTextStream ss(&str);
  ss << "Pipeline Runner (" << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch()
     << "): This application will run a SIMPLView pipeline stored in a JSON formatted pipeline file. ";
  parser.setApplicationDescription(str);
  parser.addHelpOption();
  parser.addVersionOption();

  // A boolean option with a single name (-p)
  QCommandLineOption pipelineFileArg(QStringList() << "p"
                                                   << "pipeline",
                                     "Pipeline File as a JSON file.", "file");
  parser.addOption(pipelineFileArg);

  // Process the actual command line arguments given by the user
  parser.process(app);

  QString pipelineFile = parser.value(pipelineFileArg);

  std::cout << "PipelineRunner " << SIMPLib::Version::PackageComplete().toStdString() << std::endl;
  std::cout << "Input File: " << pipelineFile.toStdString() << std::endl;

  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

#ifdef SIMPL_EMBED_PYTHON
  {
    std::cout << "Loading Python filters:\n";
    auto pythonErrorCallback = [](const std::string& message, const std::string& filePath) { std::cout << message << "\nSkipping file: \"" << filePath << "\"\n"; };
    auto pythonLoadedCallback = [](const std::string& className, const std::string& filePath) { std::cout << "Loaded \"" << className << "\" from \"" << filePath << "\"\n"; };
    PythonLoader::loadPythonFilters(*fm, PythonLoader::defaultPythonFilterPaths(), pythonErrorCallback, pythonLoadedCallback);
    std::cout << '\n';
  }
#endif

  QMetaObjectUtilities::RegisterMetaTypes();

  int err = 0;

  // Sanity Check the filepath to make sure it exists, Report an error and bail if it does not
  QFileInfo fi(pipelineFile);
  if(!fi.exists())
  {
    std::cout << "The input file '" << pipelineFile.toStdString() << "' does not exist" << std::endl;
    return EXIT_FAILURE;
  }

  // Use the static method to read the Pipeline file and return a Filter Pipeline
  QString ext = fi.suffix();

  FilterPipeline::Pointer pipeline;
  if(ext == "dream3d")
  {
    H5FilterParametersReader::Pointer dream3dReader = H5FilterParametersReader::New();
    pipeline = dream3dReader->readPipelineFromFile(pipelineFile);
  }
  else if(ext == "json")
  {
    JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
    pipeline = jsonReader->readPipelineFromFile(pipelineFile);
  }
  else
  {
    std::cout << "Unsupported pipeline file type '" << ext.toStdString() << "'. Exiting now." << std::endl;
    return EXIT_FAILURE;
  }

  if(nullptr == pipeline.get())
  {
    std::cout << "An error occurred trying to read the pipeline file. Exiting now." << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Pipeline Count: " << pipeline->size() << std::endl;
  Observer obs; // Create an Observer to report errors/progress from the executing pipeline
  pipeline->addMessageReceiver(&obs);
  // Preflight the pipeline
  err = pipeline->preflightPipeline();
  if(err < 0)
  {
    std::cout << "Errors preflighting the pipeline. Exiting Now." << std::endl;
    return EXIT_FAILURE;
  }
  // Now actually execute the pipeline
  pipeline->execute();
  err = pipeline->getErrorCode();
  if(err < 0)
  {
    std::cout << "Error Condition of Pipeline: " << err << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
