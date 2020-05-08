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

#include <memory>

#include <fstream>

#include <QtCore/QDebug>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/CoreFilters/FileReader.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

/**
 * @class VTKFileReader VTKFileReader.h PathToHeader/VTKFileReader.h
 * @brief This class holds some basic methods to read various items from a legacy
 * VTK file.
 *
 * <b>Legacy VTK File Header </b>
 * @code
   1: # vtk DataFile Version 2.0
   2: NRL TiBeta Stack
   3: BINARY
   4: DATASET STRUCTURED_POINTS
   5: DIMENSIONS 1670 770 201
   6: ORIGIN 0.00 0.00 0.00
   7: SPACING 0.665 0.665 1.48
   8: POINT_DATA 258465900
   9:
   10: SCALARS FeatureID int 1
   11: LOOKUP_TABLE default

   OR

   1: # vtk DataFile Version 2.0
   2: data set from FFT2dx_GB
   3: ASCII
   4: DATASET STRUCTURED_POINTS
   5: DIMENSIONS 189 201 100
   6: ORIGIN 0.0 0.0 0.0
   7: SPACING 0.25 0.25 0.25
   8: POINT_DATA 3798900
   9:
   10: SCALARS FeatureID int 1
   11: LOOKUP_TABLE default
 * @endcode
 *
 *
 *
 *
 * @date May 19, 2011
 * @version 1.0
 */
class SIMPLib_EXPORT VTKFileReader : public FileReader
{
  Q_OBJECT
public:
  using Self = VTKFileReader;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for VTKFileReader
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for VTKFileReader
   */
  static QString ClassName();

  static std::shared_ptr<VTKFileReader> New();

  ~VTKFileReader() override;

  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const QString& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  QString getDataContainerName() const;

  /**
   * @brief Setter property for InputFile
   */
  void setInputFile(const QString& value);
  /**
   * @brief Getter property for InputFile
   * @return Value of InputFile
   */
  QString getInputFile() const;

  /**
   * @brief Setter property for Comment
   */
  void setComment(const QString& value);
  /**
   * @brief Getter property for Comment
   * @return Value of Comment
   */
  QString getComment() const;

  /**
   * @brief Setter property for DatasetType
   */
  void setDatasetType(const QString& value);
  /**
   * @brief Getter property for DatasetType
   * @return Value of DatasetType
   */
  QString getDatasetType() const;

  /**
   * @brief Setter property for FileIsBinary
   */
  void setFileIsBinary(bool value);
  /**
   * @brief Getter property for FileIsBinary
   * @return Value of FileIsBinary
   */
  bool getFileIsBinary() const;

  /**
   * @brief Reads the VTK header and sets the values that are described in the header
   * @return Error Condition. Negative is Error.
   */
  int readHeader() override;

  /**
   * @brief This method should be re-implemented in a subclass
   */
  int readFile() override
  {
    return -1;
  }

  /**
   * @brief Parses the byte size from a data set declaration line
   * @param text
   * @return
   */
  size_t parseByteSize(char text[256]);

  /**
   *
   */
  int ignoreData(std::ifstream& in, int byteSize, char* type, int xDim, int yDim, int zDim);

  /**
   *
   */
  template <typename T>
  int skipVolume(std::ifstream& inStream, int byteSize, int xDim, int yDim, int zDim, T& diff)
  {
    int err = 0;
    size_t totalSize = xDim * yDim * zDim;
    if(getFileIsBinary() == true)
    {
      T* buffer = new T[totalSize];
      // Read all the xpoints in one shot into a buffer
      inStream.read(reinterpret_cast<char*>(buffer), (totalSize * sizeof(T)));
      if(inStream.gcount() != static_cast<std::streamsize>(totalSize * sizeof(T)))
      {
        qDebug() << " ERROR READING BINARY FILE. Bytes read was not the same as func->xDim *. " << byteSize << "." << inStream.gcount() << " vs " << (totalSize * sizeof(T));
        return -1;
      }
      if(totalSize > 1)
      {
        diff = buffer[totalSize - 1] - buffer[totalSize - 2];
      }
      else
      {
        diff = buffer[totalSize];
      }
      delete[] buffer;
    }
    else
    {
      T tmp;
      T t2;
      for(size_t x = 0; x < totalSize; ++x)
      {
        if(x == 1)
        {
          t2 = tmp;
        }
        inStream >> tmp;
        if(x == 1)
        {
          diff = tmp - t2;
        }
      }
    }
    return err;
  }

  template <typename T>
  int skipVolume(std::ifstream& inStream, int byteSize, size_t xDim, size_t yDim, size_t zDim)
  {
    int err = 0;
    if(getFileIsBinary() == true)
    {
      size_t totalSize = xDim * yDim * zDim;
      typename DataArray<T>::Pointer bufferPtr = DataArray<T>::CreateArray(totalSize, "JUNK", true);
      // Read all the xpoints in one shot into a buffer
      inStream.read(reinterpret_cast<char*>(bufferPtr->getPointer(0)), (totalSize * sizeof(T)));
      if(inStream.gcount() != static_cast<std::streamsize>(totalSize * sizeof(T)))
      {
        qDebug() << " ERROR READING BINARY FILE. Bytes read was not the same as func->xDim *. " << byteSize << "." << inStream.gcount() << " vs " << (totalSize * sizeof(T));
        return -1;
      }
    }
    else
    {
      T tmp;
      for(size_t z = 0; z < zDim; ++z)
      {
        for(size_t y = 0; y < yDim; ++y)
        {
          for(size_t x = 0; x < xDim; ++x)
          {
            inStream >> tmp;
          }
        }
      }
    }
    return err;
  }

protected:
  VTKFileReader();

public:
  VTKFileReader(const VTKFileReader&) = delete;            // Copy Constructor Not Implemented
  VTKFileReader(VTKFileReader&&) = delete;                 // Move Constructor Not Implemented
  VTKFileReader& operator=(const VTKFileReader&) = delete; // Copy Assignment Not Implemented
  VTKFileReader& operator=(VTKFileReader&&) = delete;      // Move Assignment Not Implemented

private:
  QString m_DataContainerName = {};
  QString m_InputFile = {};
  QString m_Comment = {};
  QString m_DatasetType = {};
  bool m_FileIsBinary = {};
};
