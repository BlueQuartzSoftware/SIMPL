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

#include <cctype>
#include <cstdlib>
#include <cstring> // Needed for memset
#include <iostream>
#include <string>
#include <vector>

// Qt Includes
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QTextStream>

// DREAM3DLib includes
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

#if !defined(_MSC_VER)
#include <unistd.h>
#endif

#if defined(_MSC_VER)
#include <direct.h>
#define UNLINK _unlink
#define MXA_PATH_MAX MAX_PATH
#define MXA_GET_CWD _getcwd
#else
#define UNLINK ::unlink
#include <dirent.h>
#define MXA_PATH_MAX PATH_MAX
#define MXA_GET_CWD ::getcwd
#endif

#include <sys/stat.h>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#define MXA_STATBUF struct _stati64       // non-ANSI defs
#define MXA_STATBUF4TSTAT struct _stati64 // non-ANSI defs
#define MXA_STAT _stati64
#define MXA_FSTAT _fstati64

#define MXA_STAT_REG _S_IFREG
#define MXA_STAT_DIR _S_IFDIR
#define MXA_STAT_MASK _S_IFMT
#if defined(_S_IFLNK)
#define MXA_STAT_LNK _S_IFLNK
#endif

#elif defined(__APPLE__)

#define MXA_STATBUF struct stat
#define MXA_STATBUF4TSTAT struct stat
#define MXA_STAT stat
#define MXA_FSTAT fstat

#define MXA_STAT_REG S_IFREG
#define MXA_STAT_DIR S_IFDIR
#define MXA_STAT_MASK S_IFMT
#define MXA_STAT_LNK S_IFLNK

#else
#define MXA_STATBUF struct stat
#define MXA_STATBUF4TSTAT struct stat
#define MXA_STAT stat
#define MXA_FSTAT fstat

#define MXA_STAT_REG S_IFREG
#define MXA_STAT_DIR S_IFDIR
#define MXA_STAT_MASK S_IFMT
#define MXA_STAT_LNK S_IFLNK
#endif

namespace SIMPLFileUtils
{

// -----------------------------------------------------------------------------
std::string CurrentPath()
{
  std::string currentPath;
  MXA_STATBUF st;
  if(0 == MXA_STAT(".", &st))
  {
    char currentName[MXA_PATH_MAX + 1];
    char* result = nullptr;
    ::memset(&currentName[0], 0, MXA_PATH_MAX + 1); // Clear everything to zeros.
    result = MXA_GET_CWD(currentName, MXA_PATH_MAX);
    if(nullptr == result)
    {
      std::cout << "Error: FILE_INFO_CLASS_NAME::currentPath result was NULL." << std::endl;
    }
    else
    {
      currentPath = std::string(currentName);
    }
  }
  else
  {
    std::cout << "Error: FILE_INFO_CLASS_NAME::currentPath stat function failed." << std::endl;
  }
#if defined(WIN32)
  currentPath = FILE_INFO_CLASS_NAME::cleanPath(currentPath);
#endif

  return currentPath;
}

} // namespace SIMPLFileUtils

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  //  QCoreApplication* app = new QCoreApplication(argc, argv);
  //  QCoreApplication::setOrganizationName("BlueQuartz Software");
  //  QCoreApplication::setOrganizationDomain("bluequartz.net");
  //  QCoreApplication::setApplicationName("PipelineRunner");
  //  QCoreApplication::setApplicationVersion(SIMPLib::Version::Major() + "." + SIMPLib::Version::Minor() + "." + SIMPLib::Version::Patch());

  QStringList arguments;
  for(int32_t arg = 0; arg < argc; arg++)
  {
    arguments << QString(argv[arg]);
  }

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
  parser.process(arguments);

  QString pipelineFile = parser.value(pipelineFileArg);

  std::cout << "PipelineRunner " << SIMPLib::Version::PackageComplete().toStdString() << std::endl;
  std::cout << "Input File: " << pipelineFile.toStdString() << std::endl;

  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  QString cwd = QString::fromStdString(SIMPLFileUtils::CurrentPath());

  SIMPLibPluginLoader::LoadPluginFilters(fm, cwd);

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

  std::cout << "Filter Count: " << pipeline->size() << std::endl;
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

  pipeline->removeMessageReceiver(&obs);
  err = pipeline->getErrorCode();
  if(err < 0)
  {
    std::cout << "Error Condition of Pipeline: " << err << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
