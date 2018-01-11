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

#include "QtSHelpUrlGenerator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/IFilterFactory.hpp"

#if defined(SIMPL_DISCOUNT_DOCUMENTATION) && defined(SIMPL_DOXYGEN_DOCUMENTATION)
#error Both SIMPL_DISCOUNT_DOCUMENTATION and SIMPL_DOXYGEN_DOCUMENTATION are both defined and this can not happen.
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSHelpUrlGenerator::QtSHelpUrlGenerator()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSHelpUrlGenerator::~QtSHelpUrlGenerator() = default;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl QtSHelpUrlGenerator::generateHTMLUrl(QString htmlName)
{
  QString appPath = QApplication::applicationDirPath();

  QDir helpDir = QDir(appPath);
  QString s("file://");

#if defined(Q_OS_WIN)
  s = s + "/"; // Need the third slash on windows because file paths start with a drive letter
#elif defined(Q_OS_MAC)
  if(helpDir.dirName() == "MacOS")
  {
    helpDir.cdUp();
    // Check if we are running from a .app installation where the Help dir is embeded in the app bundle.
    QFileInfo fi(helpDir.absolutePath() + "/Resources/Help");
    
    if(fi.exists())
    {
     helpDir.cd("Resources");
    }
    else
    {
      helpDir.cdUp();
      helpDir.cdUp();
    }
  }
#endif

#ifdef SIMPL_DOXYGEN_DOCUMENTATION
  QString helpFilePath = QString("%1/Help/%2/%3.html").arg(helpDir.absolutePath()).arg(QCoreApplication::instance()->applicationName()).arg(htmlName);
  QFileInfo fi(helpFilePath);
  if(fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from Visual Studio or Xcode
    // Try up one more directory
    helpDir.cdUp();
  }

  s = s + helpDir.absolutePath() + "/Help/" + QCoreApplication::instance()->applicationName() + "/" + htmlName + ".html";
#endif

#ifdef SIMPL_DISCOUNT_DOCUMENTATION
  FilterManager* fm = FilterManager::Instance();

  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(htmlName);
  QString pluginName;
  if(factory.get())
  {
    pluginName = "/Plugins/" + factory->getCompiledLibraryName();
  }

  QString helpFilePath = QString("%1/Help/%2%3/%4.html").arg(helpDir.absolutePath()).arg(QCoreApplication::instance()->applicationName()).arg(pluginName).arg(htmlName);
  QFileInfo fi(helpFilePath);
  if(fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from Visual Studio or Xcode
    // Try up one more directory
    helpDir.cdUp();
	helpFilePath = QString("%1/Help/%2%3/%4.html").arg(helpDir.absolutePath()).arg(QCoreApplication::instance()->applicationName()).arg(pluginName).arg(htmlName);
  }

  s = s + helpFilePath;
#endif 
  
  return QUrl(s);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSHelpUrlGenerator::openHTMLUrl(QUrl URL, QWidget* parent)
{
  bool didOpen = QDesktopServices::openUrl(URL);
  if(false == didOpen)
  {
    QMessageBox::critical(parent, "Error Opening Help File", QString("%1 could not open the help file path %2").arg(QCoreApplication::instance()->applicationName()).arg(URL.path()), QMessageBox::Ok,
                          QMessageBox::Ok);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSHelpUrlGenerator::generateAndOpenHTMLUrl(QString helpName, QWidget* parent)
{
  QUrl URL = generateHTMLUrl(helpName);
  openHTMLUrl(URL, parent);
}
