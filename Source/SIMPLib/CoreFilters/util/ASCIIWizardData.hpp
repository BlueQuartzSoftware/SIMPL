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

#pragma once

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include "SIMPLib/DataContainers/DataArrayPath.h"

#include <iostream>

class ASCIIWizardData
{
  // clang-format off
  PYB11_BEGIN_BINDINGS(ASCIIWizardData) 
  PYB11_CREATION()
  PYB11_METHOD(void updateDataArrayPath ARGS DataArrayPath::RenameType,renamePath)
  PYB11_METHOD(bool isEmpty)
  PYB11_END_BINDINGS()
  // clang-format on

public:
  ASCIIWizardData() = default;

  QString inputFilePath = "";
  QStringList dataHeaders;
  int beginIndex = -1;
  int numberOfLines = -1;
  QStringList dataTypes;
  QList<char> delimiters;
  std::vector<size_t> tupleDims;
  int attrMatType = 999;
  DataArrayPath selectedPath;

  int headerLine = 0;              // -1 means this selection is not used
  bool headerIsCustom = false;     // The user has set their own header names
  bool headerUsesDefaults = false; // The user just wants to use the automatically generated headers
  bool consecutiveDelimiters = false;
  bool automaticAM = false;

  void updateDataArrayPath(DataArrayPath::RenameType renamePath)
  {
    DataArrayPath oldPath;
    DataArrayPath newPath;
    std::tie(oldPath, newPath) = renamePath;

    bool hasAM = false == oldPath.getAttributeMatrixName().isEmpty();

    if(selectedPath.hasSameDataContainer(oldPath))
    {
      if(hasAM)
      {
        if(selectedPath.hasSameAttributeMatrix(oldPath))
        {
          selectedPath.setDataContainerName(newPath.getDataContainerName());
          selectedPath.setAttributeMatrixName(newPath.getAttributeMatrixName());
        }
      }
      else
      {
        selectedPath.setDataContainerName(newPath.getDataContainerName());
      }
    }
  }

  bool isEmpty()
  {
    if(inputFilePath.isEmpty() && dataHeaders.isEmpty() && dataTypes.isEmpty() && tupleDims.empty() && beginIndex < 0 && numberOfLines < 0 && selectedPath.isEmpty())
    {
      return true;
    }
    return false;
  }

  void debug()
  {
    qDebug() << "==== ASCIIWizardData Debug ====";
    qDebug() << "inputFilePath: " << inputFilePath;
    qDebug() << "dataHeaders: " << dataHeaders;
    qDebug() << "beginIndex: " << beginIndex;
    qDebug() << "numberOfLines: " << numberOfLines;
    qDebug() << "dataTypes:" << dataTypes;
    qDebug() << "delimiters: " << delimiters;
    qDebug() << "consecutiveDelimiters: " << consecutiveDelimiters;
    qDebug() << "tupleDims:" << tupleDims;
    qDebug() << "automaticAM:" << automaticAM;
    qDebug() << "headerLine:" << headerLine;
    qDebug() << "headerIsCustom:" << headerIsCustom;
    qDebug() << "headerUsesDefaults:" << headerUsesDefaults;
  }
};

Q_DECLARE_METATYPE(ASCIIWizardData)
