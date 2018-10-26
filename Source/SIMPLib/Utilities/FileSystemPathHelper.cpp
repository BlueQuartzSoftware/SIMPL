/* ============================================================================
 * Copyright (c) 2018 BlueQuartz Software, LLC
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
#include "FileSystemPathHelper.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "SIMPLib/Filtering/AbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileSystemPathHelper::FileSystemPathHelper() = default;

FileSystemPathHelper::~FileSystemPathHelper() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileSystemPathHelper::CheckOutputFile(AbstractFilter* filter,
                                           const QString &parameterName,
                                           const QString &filePath,
                                           bool requireExtension)
{
  if(filePath.isEmpty())
  {
    filter->setErrorCondition(-900);
    QString ss = QObject::tr("The output file for input parameter '%1' is empty. Please set a path to an output file.").arg(parameterName);
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  }
  QFileInfo fi(filePath);
  if(fi.isDir() && fi.suffix().isEmpty())
  {
    filter->setErrorCondition(-901);
    QString ss = QObject::tr("The output file for input parameter '%1' is a directory. Please set a path to an output file.").arg(parameterName);
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  }

  QDir parentPath = fi.path();
  if(!parentPath.exists())
  {
    filter->setWarningCondition(902);
    QString ss = QObject::tr("The directory path for input parameter '%1' does not exist. DREAM.3D will attempt to create this path during execution of the filter").arg(parameterName);
    filter->notifyWarningMessage(filter->getHumanLabel(), ss, filter->getWarningCondition());
  }

  if(requireExtension)
  {
    if(fi.suffix().isEmpty())
    {
      filter->setErrorCondition(-903);
      QString ss = QObject::tr("The file path for input parameter '%1' does not have a file extension. Please use a file extension to denote the type of file being written.").arg(parameterName);
      filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getWarningCondition());
    }

  }

#ifdef _WIN32
  // Turn file permission checking on, if requested
#ifdef SIMPL_NTFS_FILE_CHECK
// qt_ntfs_permission_lookup++;
#endif
#endif

  QFileInfo dirInfo(fi.path());
  if(dirInfo.isWritable() == false && parentPath.exists() == true)
  {
    filter->setErrorCondition(-10002);
    QString ss = QObject::tr("The user does not have the proper permissions to write to the output file");
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  }

#ifdef _WIN32
  // Turn file permission checking off, if requested
#ifdef SIMPL_NTFS_FILE_CHECK
// qt_ntfs_permission_lookup--;
#endif
#endif
}
