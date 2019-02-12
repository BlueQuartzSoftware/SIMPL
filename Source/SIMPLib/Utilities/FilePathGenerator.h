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

#include <array>

#include <QtCore/QString>
#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"

class SIMPLib_EXPORT FilePathGenerator
{
public:
  /**
   * @brief This struct is used for generating Row/Column Indexes
   */
  template <unsigned Dimension, typename T, class String> struct Tile
  {
    std::array<int32_t, Dimension> data; // r, c index
    String FileName;
  };

  using TileRCIndex2D = Tile<2, int32_t, QString>;
  using TileRCIndexRow2D = std::vector<TileRCIndex2D>;
  using TileRCIncexLayout2D = std::vector<TileRCIndexRow2D>;

  /**
   * @brief This struct is used to hold actual coordinates of each tile. Where those coordinates reference is left
   * open to interpretation by the classes using this structure.
   */
  using Tile2D = Tile<2, double, QString>;
  using TileRow2D = std::vector<Tile2D>;
  using TileLayout2D = std::vector<TileRow2D>;

  virtual ~FilePathGenerator();

  /**
   * @brief generateFileList This method will generate a file list in the correct order of the files that should
   * be imported into an h5ebsd file
   * @param start Z Slice Start
   * @param end S Slice End
   * @param increment How much to increment each item
   * @param hasMissingFiles Are any files missing
   * @param stackLowToHigh Is the Slice order low to high
   * @param filename Example File Name
   * @return
   */
  static QVector<QString> GenerateFileList(int start, int end, int increment, bool& hasMissingFiles, bool stackLowToHigh, const QString& inputPath, const QString& filePrefix,
                                           const QString& fileSuffix, const QString& fileExtension, int paddingDigits);
  /**
   * @brief generateFileList This method will generate a file list in the correct order of the files that should
   * be imported into an h5ebsd file
   * @param start Z Slice Start
   * @param end S Slice End
   * @param hasMissingFiles Are any files missing
   * @param stackLowToHigh Is the Slice order low to high
   * @param filename Example File Name
   * @return
   */
  static QVector<QString> GenerateVectorFileList(int start, int end, int compStart, int compEnd, bool& hasMissingFiles, bool stackLowToHigh, const QString& inputPath, const QString& filePrefix,
                                                 const QString& separator, const QString& fileSuffix, const QString& fileExtension, int paddingDigits);

  /**
   * @brief This method will generate a list of file paths based on the inputs provided. This method is primarily aimed
   * at generating file paths based on a Row & Column naming convention. The inputs allow for the generation of zero
   * based or offset based file names.
   * @param rowStart The starting row
   * @param rowEnd The Ending row
   * @param colStart The starting column
   * @param colEnd The Ending column
   * @param hasMissingFiles Are there missing files
   * @param rcOrdering Is the Filename ordering Row-Column or Column-Row
   * @param inputPath The Path to the files
   * @param filePrefix The file prefix
   * @param fileSuffix The file suffix
   * @param fileExtension The file extension to use
   * @param paddingDigits The number of padding digits (ZERO character) to use for each of the indices.
   * @return
   */
  static TileRCIncexLayout2D GenerateRCIndexMontageFileList(int rowStart, int rowEnd, int colStart, int colEnd, bool& hasMissingFiles, bool rcOrdering, const QString& inputPath,
                                                            const QString& filePrefix, const QString& fileSuffix, const QString& fileExtension, int paddingDigits);

protected:
  FilePathGenerator();

public:
  FilePathGenerator(const FilePathGenerator&) = delete; // Copy Constructor Not Implemented
  FilePathGenerator(FilePathGenerator&&) = delete;      // Move Constructor Not Implemented
  FilePathGenerator& operator=(const FilePathGenerator&) = delete; // Copy Assignment Not Implemented
  FilePathGenerator& operator=(FilePathGenerator&&) = delete;      // Move Assignment Not Implemented
};

