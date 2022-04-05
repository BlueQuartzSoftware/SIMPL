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

#include "SIMPLib/Utilities/StringLiteral.hpp"

#include <array>
#include <cctype>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

/*' '(0x20)space(SPC)
 * '\t'(0x09)horizontal tab(TAB)
 * '\n'(0x0a)newline(LF)
 * '\v'(0x0b)vertical tab(VT)
 * '\f'(0x0c)feed(FF)
 * '\r'(0x0d)carriage return (CR)
 */

namespace SIMPL
{
namespace StringUtilities
{
inline constexpr SIMPL::StringLiteral k_Whitespaces = " \t\f\v\n\r";

template <class InputIt, class ForwardIt, class BinOp>
void for_each_token(InputIt first, InputIt last, ForwardIt s_first, ForwardIt s_last, BinOp binary_op)
{
  while(true)
  {
    const auto pos = std::find_first_of(first, last, s_first, s_last);
    binary_op(first, pos);
    if(pos == last)
    {
      break;
    }
    first = std::next(pos);
  }
}

inline std::vector<std::string> split(std::string_view str, char delim)
{
  std::vector<std::string> tokens;
  std::array<char, 1> delims = {delim};
  auto endPos = str.end();
  for_each_token(str.begin(), endPos, delims.cbegin(), delims.cend(), [&tokens](auto first, auto second) {
    if(first != second)
    {
      tokens.push_back({first, second});
    }
  });
  return tokens;
}

inline std::vector<std::string> split_2(const std::string& line, char delimiter)
{
  std::stringstream ss(line);

  std::vector<std::string> tokens;
  std::string tempStr;

  while(std::getline(ss, tempStr, delimiter))
  {
    tokens.push_back(tempStr);
  }
  return tokens;
}

inline std::string join(const std::vector<std::string>& vec, char delim)
{
  std::string str;
  for(const auto& token : vec)
  {
    if(!str.empty())
    {
      str += delim;
    }
    str += token;
  }
  return str;
}

inline std::string replace(std::string str, std::string_view from, std::string_view to)
{
  size_t startPos = 0;
  while((startPos = str.find(from, startPos)) != std::string::npos)
  {
    str.replace(startPos, from.length(), to);
    startPos += to.length();
  }
  return str;
}

inline std::string ltrim(std::string_view str)
{
  if(str.empty())
  {
    return "";
  }

  std::string::size_type front = str.find_first_not_of(k_Whitespaces);
  if(front == std::string::npos)
  {
    return "";
  }

  return std::string(str.substr(front));
}

inline std::string rtrim(std::string_view str)
{
  if(str.empty())
  {
    return "";
  }

  std::string::size_type back = str.find_last_not_of(k_Whitespaces);
  if(back == std::string::npos)
  {
    return "";
  }

  return std::string(str.substr(0, back + 1));
}

inline std::string trimmed(std::string_view str)
{
  if(str.empty())
  {
    return "";
  }

  std::string::size_type back = str.find_last_not_of(k_Whitespaces);
  if(back == std::string::npos)
  {
    return "";
  }

  std::string::size_type front = str.find_first_not_of(k_Whitespaces);

  return std::string(str.substr(front, back - front + 1));
}

inline std::string chop(std::string_view str, size_t numElements)
{
  return std::string(str.substr(0, str.size() - numElements));
}

template <typename T>
inline std::string number(T arg)
{
  std::stringstream ss;
  ss << arg;
  return ss.str();
}

inline std::string simplified(std::string_view text)
{
  if(text.empty())
  {
    return "";
  }
  std::string out = trimmed(text);
  std::string finalString;
  for(char c : out)
  {
    if(std::isspace(static_cast<unsigned char>(c)) == 0)
    {
      finalString += c;
    }
    else
    {
      finalString += ' ';
    }
  }
  return finalString;
}
} // namespace StringUtilities
} // namespace SIMPL
