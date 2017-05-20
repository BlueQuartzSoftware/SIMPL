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

#include "LineCounterObject.h"

#include <iostream>
#include <istream>
#include <fstream>

#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QDebug>

#include "SIMPLib/SIMPLibTypes.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LineCounterObject::LineCounterObject(const QString& filePath, QObject* parent)
: QObject(parent)
, m_FilePath(filePath)
, m_NumOfLines(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LineCounterObject::LineCounterObject(const QString& filePath, int numLines, QObject* parent)
: QObject(parent)
, m_FilePath(filePath)
, m_NumOfLines(numLines)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LineCounterObject::~LineCounterObject()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LineCounterObject::run()
{

  // Obtain the file size
  if(m_FilePath.isEmpty())
  {
    m_NumOfLines = -1;
    emit finished();
    return;
  }
  QFile qFile(m_FilePath);
  int64_t fileSize = qFile.size();

 // auto start = std::chrono::system_clock::now();

  m_NumOfLines = 0;

  FILE* fp = nullptr;
  char* line = nullptr;
  size_t len = 0;
  int64_t read = 0;

  fp = fopen(m_FilePath.toStdString().c_str(), "r");
  if(fp == NULL)
  {
    QString errorStr = "Error: Unable to open file \"" + m_FilePath + "\"";
    qDebug() << errorStr;
    m_NumOfLines = -1;
    emit finished();
    return;
  }

  int64_t currentByte = 0;
  int64_t progIncrement = fileSize / 1000;
  int64_t currentThresh = progIncrement;

  while((read = getline(&line, &len, fp)) != -1)
  {
    currentByte += read;
    m_NumOfLines++;

    if(currentByte > currentThresh)
    {
      double progress = static_cast<double>(currentByte) / static_cast<double>(fileSize) * 100;
      emit progressUpdateGenerated(progress);
      currentThresh = currentThresh + progIncrement;
    }
  }

  fclose(fp);
  if(line) {
    free(line);
  }

//  std::cout << "Number of Lines: " << m_NumOfLines << std::endl;
//  auto end = std::chrono::system_clock::now();

//  std::chrono::duration<double> diff = end - start;
//  std::cout << "Millis to Read: " << diff.count() << std::endl;

  emit finished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LineCounterObject::getNumberOfLines()
{
  return m_NumOfLines;
}
