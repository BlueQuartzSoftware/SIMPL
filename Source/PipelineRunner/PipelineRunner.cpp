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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

// C Includes
#include <cassert>
#include <cstdlib>

// C++ Includes
#include <fstream>
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
// #include "SIMPLib/Common/LogFileObserver.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#ifdef SIMPL_EMBED_PYTHON
#include "SIMPLib/Python/PythonLoader.h"
#endif

QFile s_LogFile;
QTextStream s_LogStream(&s_LogFile);

/**
 * @brief MessageToLogFileHandler
 * @param type
 * @param msg
 */
void MessageToLogFileHandler(QtMsgType type, const QMessageLogContext&, const QString& msg)
{
  QString txt;
  switch(type)
  {
  case QtDebugMsg:
    txt = QString("Debug: %1").arg(msg);
    break;
  case QtWarningMsg:
    txt = QString("Warning: %1").arg(msg);
    break;
  case QtCriticalMsg:
    txt = QString("Critical: %1").arg(msg);
    break;
  case QtFatalMsg:
    txt = QString("Fatal: %1").arg(msg);
    break;
  case QtInfoMsg:
    txt = QString("Info: %1").arg(msg);
    break;
  }
  s_LogStream << txt << "\n";
}

// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

#ifdef DREAM3D_ANACONDA
  {
    constexpr const char k_PYTHONHOME[] = "PYTHONHOME";
    QString condaPrefix = qgetenv("CONDA_PREFIX");

    QString pythonHome = qgetenv(k_PYTHONHOME);
    if(pythonHome.isEmpty() && !condaPrefix.isEmpty())
    {
      qputenv(k_PYTHONHOME, condaPrefix.toLocal8Bit());
    }

    qputenv("DREAM3D_PLUGINS_LOADED", "1");
  }
#endif

#ifdef SIMPL_EMBED_PYTHON
  bool hasPythonHome = PythonLoader::checkPythonHome();

  if(!hasPythonHome)
  {
    std::cout << "Warning: \"PYTHONHOME\" not set. This environment variable must be set for embedded Python to work.\n";
  }

  PythonLoader::ScopedInterpreter interpreter_guard{hasPythonHome};
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

  QCommandLineOption logFileOption(QStringList() << "l"
                                                 << "logfile",
                                   "Save output to file", "log");
  parser.addOption(logFileOption);
  // Process the actual command line arguments given by the user
  parser.process(app);

  QString pipelineFile = parser.value(pipelineFileArg);
  QString logFile = parser.value(logFileOption);

  if(!logFile.isEmpty())
  {
    s_LogFile.setFileName(logFile);
    s_LogFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    qInstallMessageHandler(MessageToLogFileHandler);
    qDebug() << QDateTime::currentDateTime();
  }
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);
  if(!logFile.isEmpty())
  {
    s_LogFile.close();
  }

  // Now hand off to the C++ streams for logging
  std::ofstream logFileStream;
  std::streambuf* stdCoutBuf = std::cout.rdbuf();

  if(!logFile.isEmpty())
  {
    logFileStream.open(logFile.toStdString(), std::ios_base::out | std::ios_base::app);
    if(!logFileStream.is_open())
    {
      std::cout << "Error opening log file '" << logFile.toStdString() << "'" << std::endl;
      return 1;
    }
    std::cout.rdbuf(logFileStream.rdbuf());
  }

  std::cout << "PipelineRunner " << SIMPLib::Version::PackageComplete().toStdString() << std::endl;
  std::cout << "Input File: " << pipelineFile.toStdString() << std::endl;
  if(!logFile.isEmpty())
  {
    std::cout << "Log File: " << logFile.toStdString() << std::endl;
  }

#ifdef SIMPL_EMBED_PYTHON
  if(hasPythonHome)
  {
    std::cout << "Loading plugin Python filters\n";
    if(!PythonLoader::loadPluginFilters([](const std::string& error) { std::cout << error << "\n"; }))
    {
      std::cout << "Failed to load python filters from plugins\n";
    }
    PythonLoader::addToPythonPath(PythonLoader::defaultSIMPLPythonLibPath());
    std::cout << "Loading Python filters:\n";
    auto paths = PythonLoader::defaultPythonFilterPaths();
    for(const auto& path : paths)
    {
      std::cout << "Searching \"" << path << "\"\n";
    }
    auto pythonErrorCallback = [](const std::string& message, const std::string& filePath) { std::cout << message << "\nSkipping file: \"" << filePath << "\"\n"; };
    auto pythonLoadedCallback = [](const std::string& className, const std::string& filePath) { std::cout << "Loaded \"" << className << "\" from \"" << filePath << "\"\n"; };
    size_t numLoaded = PythonLoader::loadPythonFilters(*fm, paths, pythonErrorCallback, pythonLoadedCallback);
    std::cout << "Loaded " << numLoaded << " filters\n";
  }
#endif

  QMetaObjectUtilities::RegisterMetaTypes();

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
    try
    {
      pipeline = dream3dReader->readPipelineFromFile(pipelineFile);
    } catch(const std::exception& exception)
    {
      std::cout << "Caught exception while reading pipeline file:\n";
      std::cout << exception.what() << "\n";
    }
  }
  else if(ext == "json")
  {
    JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
    try
    {
      pipeline = jsonReader->readPipelineFromFile(pipelineFile);
    } catch(const std::exception& exception)
    {
      std::cout << "Caught exception while reading pipeline file:\n";
      std::cout << exception.what() << "\n";
    }
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

  //  if(logFile.isEmpty())
  {
    pipeline->addMessageReceiver(&obs);
  }
  //  else
  //  {
  //    pipeline->addMessageReceiver(logFileObserver);
  //  }

  // Preflight the pipeline
  int err = -1;
  try
  {
    err = pipeline->preflightPipeline();
  } catch(const std::exception& exception)
  {
    std::cout << "Caught exception while preflighting pipeline:\n";
    std::cout << exception.what() << "\n";
  }
  if(err < 0)
  {
    std::cout << "Errors preflighting the pipeline. Exiting Now." << std::endl;
    return EXIT_FAILURE;
  }
  // Now actually execute the pipeline
  try
  {
    pipeline->execute();
  } catch(const std::exception& exception)
  {
    std::cout << "Caught exception while executing pipeline:\n";
    std::cout << exception.what() << "\n";
    std::cout << "Exiting now.\n";
    return EXIT_FAILURE;
  }
  err = pipeline->getErrorCode();
  if(err < 0)
  {
    std::cout << "Error Condition of Pipeline: " << err << std::endl;
    return EXIT_FAILURE;
  }

  // Return std::cout back to normal
  std::cout.rdbuf(stdCoutBuf);

  return EXIT_SUCCESS;
}
