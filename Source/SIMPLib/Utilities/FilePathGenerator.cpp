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

#include "FilePathGenerator.h"

#include <QtCore/QDir>
#include <QtCore/QTextStream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilePathGenerator::FilePathGenerator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilePathGenerator::~FilePathGenerator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> FilePathGenerator::GenerateFileList(int start, int end, int increment, bool& hasMissingFiles, bool stackLowToHigh, const QString& inputPath, const QString& filePrefix,
                                                     const QString& fileSuffix, const QString& fileExtension, int paddingDigits)
{
  QVector<QString> fileList;
  QDir dir(inputPath);
  if(!dir.exists())
  {
    return fileList;
  }
  int index = 0;

  QString filename;
  bool missingFiles = false;
  for(int i = 0; i < (end - start) + 1; i = i + increment)
  {
    if(stackLowToHigh)
    {
      index = start + i;
    }
    else
    {
      index = end - i;
    }
    filename = QString("%1%2%3.%4").arg(filePrefix).arg(QString::number(index), paddingDigits, '0').arg(fileSuffix).arg(fileExtension);
    QString filePath = inputPath + QDir::separator() + filename;
    filePath = QDir::toNativeSeparators(filePath);

    QFileInfo fi(filePath);
    if(!fi.exists())
    {
      missingFiles = true;
    }

    fileList.push_back(filePath);
  }

  hasMissingFiles = missingFiles;

  return fileList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> FilePathGenerator::GenerateVectorFileList(int start, int end, int compStart, int compEnd, bool& hasMissingFiles, bool stackLowToHigh, const QString& inputPath,
                                                           const QString& filePrefix, const QString& separator, const QString& fileSuffix, const QString& fileExtension, int paddingDigits)
{
  QVector<QString> fileList;
  QDir dir(inputPath);
  if(!dir.exists())
  {
    return fileList;
  }
  int index = 0;
  int index2 = 0;

  QString filename;
  for(int i = 0; i < (end - start) + 1; ++i)
  {
    for(int j = 0; j < (compEnd - compStart) + 1; j++)
    {
      if(stackLowToHigh)
      {
        index = start + i;
      }
      else
      {
        index = end - i;
      }

      index2 = compStart + j;

      filename =
          QString("%1%2%3%4%5.%6").arg(filePrefix).arg(QString::number(index), paddingDigits, '0').arg(separator).arg(QString::number(index2), paddingDigits, '0').arg(fileSuffix).arg(fileExtension);
      QString filePath = inputPath + QDir::separator() + filename;
      filePath = QDir::toNativeSeparators(filePath);
      fileList.push_back(filePath);
    }
  }
  return fileList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilePathGenerator::TileRCIncexLayout2D FilePathGenerator::GenerateRCIndexMontageFileList(int rowStart, int rowEnd, int colStart, int colEnd, bool& hasMissingFiles, bool rcOrdering,
                                                                                         const QString& inputPath, const QString& filePrefix, const QString& fileSuffix, const QString& fileExtension,
                                                                                         int paddingDigits)
{
  TileRCIncexLayout2D tileLayout2D;

  QDir dir(inputPath);
  if(!dir.exists())
  {
    return tileLayout2D;
  }

  bool missingFiles = false;

  for(int r = rowStart; r < rowEnd; r++)
  {
    TileRCIndexRow2D tileRow2D;
    for(int c = colStart; c < colEnd; c++)
    {
      TileRCIndex2D tile2D;

      QString filePath;
      QTextStream fn(&filePath);
      fn << inputPath;
      if(!inputPath.endsWith("/"))
      {
        fn << "/";
      }
      fn << filePrefix;

      if(rcOrdering)
      {
        fn.setFieldWidth(0);
        fn << "r";

        fn.setFieldWidth(paddingDigits);
        fn.setFieldAlignment(QTextStream::AlignRight);
        fn.setPadChar('0');
        fn << r;

        fn.setFieldWidth(0);
        fn << "c";

        fn.setFieldWidth(paddingDigits);
        fn.setFieldAlignment(QTextStream::AlignRight);
        fn.setPadChar('0');
        fn << c;
      }
      else
      {
        fn.setFieldWidth(0);
        fn << "c";

        fn.setFieldWidth(paddingDigits);
        fn.setFieldAlignment(QTextStream::AlignRight);
        fn.setPadChar('0');
        fn << c;

        fn.setFieldWidth(0);
        fn << "r";

        fn.setFieldWidth(paddingDigits);
        fn.setFieldAlignment(QTextStream::AlignRight);
        fn.setPadChar('0');
        fn << r;
      };

      fn.setFieldWidth(0);
      fn << fileSuffix << "." << fileExtension;

      filePath = QDir::toNativeSeparators(filePath);

      tile2D.FileName = filePath;
      tile2D.data = {{r, c}};

      QFileInfo fi(filePath);
      if(!fi.exists())
      {
        missingFiles = true;
      }

      tileRow2D.emplace_back(tile2D);
    }

    // Push the row back on the TileLayout2D
    tileLayout2D.emplace_back(tileRow2D);
  }
  hasMissingFiles = missingFiles;

  return tileLayout2D;
}
