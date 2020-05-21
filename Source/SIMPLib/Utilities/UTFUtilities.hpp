/* ============================================================================
 * Copyright (c) 2020 BlueQuartz Software, LLC
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

#include <cstdio>
#include <string>
#include <tuple>
#include <cstdint>
#include <array>

namespace UTFUtilities
{

/**
 * @brief Determines if the file has the UTF8 BOM block of 3 bytes at the beginning of the file. This
 * does NOT mean definitively that the file is UTF8, just that the UTF8 BOM bytes are the first 3 bytes of the
 * file. If you know the file is a text file (and not some purely binary data file) then this method can be used
 * to determine if thie file has the UTF8 BOM marker at the start.
 * @param filePath The path to the file
 * @return std::pair<bool, int32_t> where the first value is whether the BOM showed up and the second
 * is an error code. 0 Denotes NO error where negative error values are errors.
 */
std::pair<bool, int32_t> IsUtf8(const std::string& filePath)
{
  FILE* f = fopen(filePath.c_str(), "rb");
  if(nullptr == f)
  {
    return {false, -1};
  }
  std::array<uint8_t, 3> buf = {0, 0, 0};
  if(fread(buf.data(), 1, 3, f) != 3)
  {
    std::ignore = fclose(f);
    return {false, -1};
  }
  std::ignore = fclose(f);
  if(buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF)
  {
    return {true, 0};
  }
  return {false, 0};
}

} // namespace UTFUtilities
