/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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

#include "SIMPLDataPathValidator.h"

#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

SIMPLDataPathValidator* SIMPLDataPathValidator::m_Self = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataPathValidator::SIMPLDataPathValidator() :
  QObject()
{
  // Data directory path for non-Mac operating systems
  QString simplDataDirectory = QCoreApplication::applicationDirPath() + QDir::separator() + "Data";

#if defined(Q_OS_MAC)
#if defined(SIMPL_CHOOSABLE_DATA_DIRECTORY)
  // Default data directory for Macs with choosable data directory turned on (generally for Release builds)
  simplDataDirectory = tr("%1%2%3Data").arg(QDir::homePath()).arg(QDir::separator()).arg(QCoreApplication::applicationName());
#else
  // Default data directory for Macs with choosable data directory turned off (generally for Debug and developer builds)
  simplDataDirectory = QCoreApplication::applicationDirPath();
  QDir dir (simplDataDirectory);
  dir.cdUp();
  dir.cdUp();
  dir.cdUp();
  simplDataDirectory = dir.absolutePath() + QDir::separator() + "Data";
#endif
#endif

  m_SIMPLDataDirectory = QDir::toNativeSeparators(simplDataDirectory);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataPathValidator::~SIMPLDataPathValidator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataPathValidator* SIMPLDataPathValidator::Instance()
{
  if(m_Self == nullptr)
  {
    m_Self = new SIMPLDataPathValidator();
  }

  return m_Self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SIMPLDataPathValidator::sanityCheckRelativePath(const QString &path)
{
  QString suggestedPath = path;
  QFileInfo fi(suggestedPath);

  if (fi.isRelative())
  {
    if (path.startsWith(QDir::separator()) == false && m_SIMPLDataDirectory.endsWith(QDir::separator()) == false)
    {
      suggestedPath.prepend(QDir::separator());
    }
    suggestedPath.prepend(m_SIMPLDataDirectory);
  }

  return suggestedPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLDataPathValidator::setSIMPLDataDirectory(const QString &path)
{
  m_SIMPLDataDirectory = path;

  emit dataDirectoryChanged(path);
}
