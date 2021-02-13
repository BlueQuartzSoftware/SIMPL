/* ============================================================================
 * Copyright (c) 2021 BlueQuartz Software, LLC
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

#include <algorithm>

namespace SIMPL
{

/**
 * @brief Returns boolean value if the container contains the value
 * @param first STL Compatible Iterator
 * @param last STL Compatible Iterator
 * @param value Value to find in the container
 * @return If the value is in the container using the "==" operator
 */
template <class Iterator, class T>
constexpr bool contains(Iterator first, Iterator last, const T& value)
{
  // Can't use std::find in the implemention for constexpr until C++20

  for(; first != last; ++first)
  {
    if(*first == value)
    {
      return true;
    }
  }

  return false;
}

/**
 * @brief Returns boolean value if the container contains the value. The container **must** be STL compatible and
 * implement the .cbegin() and cend() methods that return iterators
 * @param container The container to check
 * @param value The value to look for
 * @return If the container has the value
 */
template <class T, typename K>
bool contains(const T& container, const K& value)
{
  auto result = std::find(container.cbegin(), container.cend(), value);
  return result != container.cend();
}

} // namespace SIMPL
