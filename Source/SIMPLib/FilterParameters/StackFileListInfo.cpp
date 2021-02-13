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

#include "StackFileListInfo.h"

// -----------------------------------------------------------------------------
StackFileListInfo::StackFileListInfo() = default;

// -----------------------------------------------------------------------------
StackFileListInfo::StackFileListInfo(const QString& extension, int32_t startIndex, int32_t endIndex, int32_t padding)
{
  FileExtension = extension;
  StartIndex = startIndex;
  EndIndex = endIndex;
  PaddingDigits = padding;
}

// -----------------------------------------------------------------------------
StackFileListInfo::StackFileListInfo(int32_t paddingDigits, uint32_t ordering, int32_t incIndex, const QString& inputPath, const QString& filePrefix, const QString& fileSuffix,
                                     const QString& extension, int32_t startIndex, int32_t endIndex)
{
  PaddingDigits = paddingDigits;
  Ordering = ordering;
  IncrementIndex = incIndex;
  InputPath = inputPath;
  FilePrefix = filePrefix;
  FileSuffix = fileSuffix;
  FileExtension = extension;
  StartIndex = startIndex;
  EndIndex = endIndex;
}

// -----------------------------------------------------------------------------
StackFileListInfo::~StackFileListInfo() = default;

// -----------------------------------------------------------------------------
void StackFileListInfo::writeJson(QJsonObject& json) const
{
  writeSuperclassJson(json);

  json["StartIndex"] = static_cast<qint32>(StartIndex);
  json["EndIndex"] = static_cast<qint32>(EndIndex);
}

// -----------------------------------------------------------------------------
bool StackFileListInfo::readJson(QJsonObject& json)
{
  bool result = readSuperclassJson(json);
  if(!result)
  {
    return false;
  }

  if(json["StartIndex"].isDouble() && json["EndIndex"].isDouble())
  {
    StartIndex = static_cast<qint32>(json["StartIndex"].toInt());
    EndIndex = static_cast<quint32>(json["EndIndex"].toInt());
    return true;
  }
  return false;
}
