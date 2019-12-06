/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"

class SIMPLib_EXPORT FileListInfo
{
public:
  virtual ~FileListInfo();

  qint32 PaddingDigits = 3;
  quint32 Ordering = 0; /* Ordering=0 = RowColumn, Ordering=1 = ColumnRow */
  qint32 IncrementIndex = 1;
  QString InputPath;
  QString FilePrefix;
  QString FileSuffix;
  QString FileExtension;

  /**
   * @brief writeJson
   * @param json
   */
  virtual void writeJson(QJsonObject& json) const = 0;

  /**
   * @brief readJson
   * @param json
   * @return
   */
  virtual bool readJson(QJsonObject& json) = 0;

protected:
  FileListInfo();

  /**
   * @brief writeSuperclassJson
   * @param json
   */
  void writeSuperclassJson(QJsonObject& json) const;

  /**
   * @brief readSuperclassJson
   * @param json
   * @return
   */
  bool readSuperclassJson(QJsonObject& json);

public:
  FileListInfo(const FileListInfo&) = default;            // Copy Constructor Not Implemented
  FileListInfo(FileListInfo&&) = default;                 // Move Constructor Not Implemented
  FileListInfo& operator=(const FileListInfo&) = default; // Copy Assignment Not Implemented
  FileListInfo& operator=(FileListInfo&&) = default;      // Move Assignment Not Implemented
};
