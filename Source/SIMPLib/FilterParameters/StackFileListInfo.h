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

#include "SIMPLib/Common/SIMPLPythonMacros.h"
#include "SIMPLib/FilterParameters/FileListInfo.h"

#define FP_SET_PROPERTY(type, prpty)                                                                                                                                                                   \
  void set##prpty(const type& value)                                                                                                                                                                   \
  {                                                                                                                                                                                                    \
    this->prpty = value;                                                                                                                                                                               \
  }

#define FP_GET_PROPERTY(type, prpty)                                                                                                                                                                   \
  type get##prpty() const                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    return prpty;                                                                                                                                                                                      \
  }

#define FP_PROP_DEC_DEF(type, prpty)                                                                                                                                                                   \
  FP_SET_PROPERTY(type, prpty)                                                                                                                                                                         \
  FP_GET_PROPERTY(type, prpty)

class SIMPLib_EXPORT StackFileListInfo : public FileListInfo
{
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(StackFileListInfo)
  PYB11_CREATION(QString int32_t int32_t int32_t)
  PYB11_CREATION(int32_t uint32_t int32_t int32_t int32_t QString QString QString QString)
  PYB11_PROPERTY(int32_t PaddingDigits READ getPaddingDigits WRITE setPaddingDigits)
  PYB11_PROPERTY(uint32_t Ordering READ getOrdering WRITE setOrdering)
  PYB11_PROPERTY(int32_t IncrementIndex READ getIncrementIndex WRITE setIncrementIndex)
  PYB11_PROPERTY(QString InputPath READ getInputPath WRITE setInputPath)
  PYB11_PROPERTY(QString FilePrefix READ getFilePrefix WRITE setFilePrefix)
  PYB11_PROPERTY(QString FileSuffix READ getFileSuffix WRITE setFileSuffix)
  PYB11_PROPERTY(QString FileExtension READ getFileExtension WRITE setFileExtension)
  PYB11_PROPERTY(int32_t StartIndex READ getStartIndex WRITE setStartIndex)
  PYB11_PROPERTY(int32_t EndIndex READ getEndIndex WRITE setEndIndex)
  PYB11_END_BINDINGS()
  // End Python bindings declarations
public:
  StackFileListInfo();
  ~StackFileListInfo() override;
  /**
   * @brief StackFileListInfo
   * @param extension
   * @param startIndex
   * @param endIndex
   * @param padding
   */
  StackFileListInfo(const QString& extension, int32_t startIndex, int32_t endIndex, int32_t padding);

  /**
   * @brief StackFileListInfo
   * @param paddingDigits
   * @param ordering
   * @param startIndex
   * @param endIndex
   * @param incrementIndex
   * @param inputPath
   * @param filePrefix
   * @param fileSuffix
   * @param fileExtension
   */
  StackFileListInfo(int32_t paddingDigits, uint32_t ordering, int32_t startIndex, int32_t endIndex, int32_t incrementIndex, const QString& inputPath, const QString& filePrefix,
                    const QString& fileSuffix, const QString& fileExtension);

  // These are from the super class
  FP_PROP_DEC_DEF(int32_t, PaddingDigits)
  FP_PROP_DEC_DEF(uint32_t, Ordering)
  FP_PROP_DEC_DEF(int32_t, IncrementIndex)
  FP_PROP_DEC_DEF(QString, InputPath)
  FP_PROP_DEC_DEF(QString, FilePrefix)
  FP_PROP_DEC_DEF(QString, FileSuffix)
  FP_PROP_DEC_DEF(QString, FileExtension)
  // These are from this subclass
  FP_PROP_DEC_DEF(int32_t, StartIndex)
  FP_PROP_DEC_DEF(int32_t, EndIndex)

  qint32 StartIndex = 0;
  qint32 EndIndex = 1;

  /**
   * @brief writeJson
   * @param json
   */
  void writeJson(QJsonObject& json) const override;

  /**
   * @brief readJson
   * @param json
   * @return
   */
  bool readJson(QJsonObject& json) override;

public:
  StackFileListInfo(const StackFileListInfo&) = default;            // Copy Constructor Not Implemented
  StackFileListInfo(StackFileListInfo&&) = default;                 // Move Constructor Not Implemented
  StackFileListInfo& operator=(const StackFileListInfo&) = default; // Copy Assignment Not Implemented
  StackFileListInfo& operator=(StackFileListInfo&&) = default;      // Move Assignment Not Implemented
};

Q_DECLARE_METATYPE(StackFileListInfo)
