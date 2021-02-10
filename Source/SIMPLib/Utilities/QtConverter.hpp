/* ============================================================================
 * Copyright (c) 2021-2021 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <string>
#include <vector>

#include <QtCore/QStringList>

#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"

namespace QtConverter
{

/**
 * @brief Converts a std::vector<std::string> to a QStringList
 * @param items The list of strings to convert
 * @return QStringList of Items
 */
static QStringList ToQStringList(const std::vector<std::string>& items)
{
  QStringList out;
  for(const auto& item : items)
  {
    out << QString::fromStdString(item);
  }
  return out;
}

/**
 * @brief Converts a QStringList of assumed file paths to a std::vector<std::string>
 * @param items The file paths to convert
 * @return The converted list
 */
static std::vector<std::string> FromQStringFileList(const QStringList& items)
{
  std::vector<std::string> out;
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  for(const auto& filePath : items)
  {
    out.push_back(validator->convertToAbsolutePath(filePath).toStdString());
  }
  return out;
}

} // namespace QtConverter