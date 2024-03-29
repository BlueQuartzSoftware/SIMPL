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

#include <iostream>
#include <vector>

#include <QtCore/QVector>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLibDLLExport.h"

class SIMPLib_EXPORT RgbColor
{
public:
  /**
   * @brief Rgb An ARGB quadruplet on the format #AARRGGBB, equivalent to an unsigned int and The type also holds a value for the alpha-channel.
   */
  inline static int dRed(SIMPL::Rgb rgb)
  {
    return ((rgb >> 16) & 0xff);
  }

  inline static int dGreen(SIMPL::Rgb rgb)
  {
    return ((rgb >> 8) & 0xff);
  }

  inline static int dBlue(SIMPL::Rgb rgb)
  {
    return (rgb & 0xff);
  }

  inline static int dAlpha(SIMPL::Rgb rgb)
  {
    return rgb >> 24;
  }

  inline static int dGray(SIMPL::Rgb rgb)
  {
    return (((rgb >> 16) & 0xff) * 11 + ((rgb >> 8) & 0xff) * 16 + (rgb & 0xff) * 5) / 32;
  }

  inline static SIMPL::Rgb dRgb(int r, int g, int b, int a)
  {
    return ((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
  }

  inline static void print(std::ostream& out, const char& sep, const SIMPL::Rgb& rgb)
  {
    out << "rgb: " << RgbColor::dRed(rgb) << sep << RgbColor::dGreen(rgb) << sep << RgbColor::dBlue(rgb);
  }

  inline static bool compare(const SIMPL::Rgb& left, const SIMPL::Rgb& right)
  {
    return left == right;
  }

  inline static std::tuple<float, float, float> fRgb(SIMPL::Rgb rgb)
  {
    return std::make_tuple(static_cast<float>(((rgb >> 16) & 0xff) / 255.0f), static_cast<float>(((rgb >> 8) & 0xff) / 255.0f), static_cast<float>((rgb & 0xff) / 255.0f));
  }

protected:
  RgbColor() = default;
};

class QJsonArray;

/**
 * @class ColorTable ColorTable.h /Utilities/ColorTable.h
 * @brief This class has functions that help create color tables and also define the ARGB Quadruplet
 * @author Mike Jackson for BlueQuartz Software
 * @date Aug 1 2013
 * @version 1.0
 */
class SIMPLib_EXPORT SIMPLColorTable
{
public:
  SIMPLColorTable();
  virtual ~SIMPLColorTable();

  /**
   * @brief Assumes you've already generated min and max -- the extrema for the data
   * to which you're applying the color map. Then define the number of colorNodes
   * and make sure there's a row of three float values (representing r, g, and b
   * in a 0.0-1.0 range) for each node. Then call this method for with parameter
   * val some float value between min and max inclusive. The corresponding rgb
   * values will be returned in the reference-to-float parameters r, g, and b.
   * @param val
   * @param r [output] The Red Value (0 <= 1.0)
   * @param g [output] The Green Value (0 <= 1.0)
   * @param b [output] The Blue Value (0 <= 1.0)
   * @param max
   * @param min
   */
  static void GetColorTable(int numColors, QVector<float>& colors);

  static std::vector<unsigned char> GetColorTable(size_t numColors, QJsonArray colorNodeArray);

private:
  SIMPLColorTable(const SIMPLColorTable&) = delete; // Copy Constructor Not Implemented
  void operator=(const SIMPLColorTable&) = delete;  // Move assignment Not Implemented
};
