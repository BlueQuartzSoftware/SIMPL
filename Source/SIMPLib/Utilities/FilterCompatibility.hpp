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

#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

namespace SIMPL
{
inline void NullCheck(const void* ptr, const char* name, const char* fileName, size_t lineNumber)
{
  if(ptr == nullptr)
  {
    std::stringstream ss;
    ss << name << " is nullptr.";
    ss << " Occurred on line " << lineNumber << " in file \"" << fileName << "\".";
    throw std::invalid_argument(ss.str());
  }
}

template <class T>
T* FilterCompatibilityCheck(AbstractFilter* filter, const FilterParameter* parameter, const char* widgetClassName, const char* filterClassName, const char* fileName, size_t lineNumber)
{
  static_assert(std::is_base_of_v<AbstractFilter, T>);

  NullCheck(filter, "filter", fileName, lineNumber);
  NullCheck(parameter, "parameter", fileName, lineNumber);

  auto castedFilter = dynamic_cast<T*>(filter);

  if(castedFilter == nullptr)
  {
    std::stringstream ss;
    ss << widgetClassName << " can ONLY be used with " << filterClassName << " objects. The programmer of the filter has a bug.";
    ss << " The name of the filter was " << filter->getHumanLabel().toStdString() << " and the name of the Filter Parameter was " << parameter->getHumanLabel().toStdString();
    ss << " and is trying to get the propery " << parameter->getPropertyName().toStdString() << " in the filter.";
    ss << " Occurred on line " << lineNumber << " in file \"" << fileName << "\".";
    throw std::invalid_argument(ss.str());
  }

  return castedFilter;
}

template <class T>
T* FilterParameterCompatibilityCheck(const AbstractFilter* filter, FilterParameter* parameter, const char* widgetClassName, const char* filterParameterClassName, const char* fileName,
                                     size_t lineNumber)
{
  static_assert(std::is_base_of_v<FilterParameter, T>);

  NullCheck(filter, "filter", fileName, lineNumber);
  NullCheck(parameter, "parameter", fileName, lineNumber);

  auto castedParameter = dynamic_cast<T*>(parameter);

  if(castedParameter == nullptr)
  {
    std::stringstream ss;
    ss << widgetClassName << " can ONLY be used with " << filterParameterClassName << " objects. The programmer of the filter has a bug.";
    ss << " The name of the filter was " << filter->getHumanLabel().toStdString() << " and the name of the Filter Parameter was " << parameter->getHumanLabel().toStdString();
    ss << " and is trying to get the propery " << parameter->getPropertyName().toStdString() << " in the filter.";
    ss << " Occurred on line " << lineNumber << " in file \"" << fileName << "\".";
    throw std::invalid_argument(ss.str());
  }

  return castedParameter;
}
} // namespace SIMPL

#define SIMPL_FILTER_COMPATIBILITY_CHECK(filter, parameter, widgetClass, filterClass) SIMPL::FilterCompatibilityCheck<filterClass>(filter, parameter, #widgetClass, #filterClass, __FILE__, __LINE__)
#define SIMPL_FILTER_PARAMETER_COMPATIBILITY_CHECK(filter, parameter, widgetClass, filterParemeterClass)                                                                                               \
  SIMPL::FilterParameterCompatibilityCheck<filterParemeterClass>(filter, parameter, #widgetClass, #filterParemeterClass, __FILE__, __LINE__)
