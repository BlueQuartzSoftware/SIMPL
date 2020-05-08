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

#pragma once

#if defined(ITK_VERSION_MAJOR) && ITK_VERSION_MAJOR == 4
#error This file can ONLY be used if ITK Version 5 with the Montage module is being used.
#endif

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QPointF>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "itkParseTileConfiguration.h"

namespace itk
{
struct FijiImageTileData
{
  QString filePath;
  QPointF coords;
  int row;
  int col;
};

using FijiRowData = std::vector<FijiImageTileData>;
using FijiFileData = std::vector<FijiRowData>;

class FijiConfigurationFileReader : public QObject
{
public:
  FijiFileData parseFijiConfigFile(const QString& configFilePath)
  {
    m_ErrorCondition = 0;
    itk::FijiFileData fileData;

    itk::TileLayout2D stageTiles = itk::ParseTileConfiguration2D(configFilePath.toStdString());
    for(int i = 0; i < stageTiles.size(); i++)
    {
      auto tile2D = stageTiles[i];
      FijiRowData rowData;
      for(int j = 0; j < tile2D.size(); j++)
      {
        auto tile = tile2D[j];

        QString imageFilePath = QString::fromStdString(tile.FileName);
        QFileInfo fi(imageFilePath);
        if(fi.isRelative())
        {
          QFileInfo configFi(configFilePath);
          imageFilePath = configFi.path() + QDir::separator() + imageFilePath;
        }

        itk::FijiImageTileData imageTileData;

        imageTileData.filePath = imageFilePath;

        uint32_t coordsSize = tile.Position.GetPointDimension();
        if(coordsSize != 2)
        {
          m_ErrorMessage = QObject::tr("The dimension size of all tiles in the fiji config file must be equal to 2.");
          m_ErrorCondition = -2004;
          return itk::FijiFileData();
        }

        imageTileData.coords = QPointF(tile.Position[0], tile.Position[1]);
        imageTileData.row = i;
        imageTileData.col = j;

        rowData.push_back(imageTileData);
      }

      fileData.push_back(rowData);
    }

    return fileData;
  }

  int getErrorCode()
  {
    return m_ErrorCondition;
  }

  QString getErrorMessage()
  {
    return m_ErrorMessage;
  }

private:
  int m_ErrorCondition;
  QString m_ErrorMessage;
};
} // namespace itk

//#ifndef ITK_MANUAL_INSTANTIATION
//#include "itkFijiConfigurationFileReader.cpp"
//#endif
