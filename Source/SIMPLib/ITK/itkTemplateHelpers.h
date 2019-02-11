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

#include "SIMPLib/Common/TemplateHelpers.h"

/*
 * @brief This file contains some macros that help to reduce the amount of code needed to determine the data type
 * of an IDataArray and then call a function with template arguments
 */

namespace ITKTemplateHelpers
{

#define EXECUTE_REGISTER_FUNCTION_TEMPLATE_HELPER(DATATYPE, observableObj, templateName, inputData, ...)                                                                                               \
  int numOfComponents = inputData->getNumberOfComponents();                                                                                                                                            \
  if(numOfComponents == 3)                                                                                                                                                                             \
  {                                                                                                                                                                                                    \
    templateName<DATATYPE>(__VA_ARGS__);                                                                                                                                                               \
  }                                                                                                                                                                                                    \
  else if(numOfComponents == 4)                                                                                                                                                                        \
  {                                                                                                                                                                                                    \
    templateName<DATATYPE>(__VA_ARGS__);                                                                                                                                                               \
  }                                                                                                                                                                                                    \
  else if(numOfComponents == 1)                                                                                                                                                                        \
  {                                                                                                                                                                                                    \
    templateName<DATATYPE>(__VA_ARGS__);                                                                                                                                                               \
  }                                                                                                                                                                                                    \
  else                                                                                                                                                                                                 \
  {                                                                                                                                                                                                    \
    observableObj->notifyErrorMessage(#templateName,                                                                                                                                                   \
                                      "The input array's image type is not recognized.  Supported image types"                                                                                         \
                                      " are grayscale (1-component), RGB (3-component), and RGBA (4-component)",                                                                                       \
                                      TemplateHelpers::Errors::UnsupportedImageType);                                                                                                                  \
  }

#define EXECUTE_STITCH_FUNCTION_TEMPLATE_HELPER(DATATYPE, observableObj, templateName, inputData, ...)                                                                                                 \
  int numOfComponents = inputData->getNumberOfComponents();                                                                                                                                            \
  if(numOfComponents == 3)                                                                                                                                                                             \
  {                                                                                                                                                                                                    \
    templateName<itk::RGBPixel<DATATYPE>, itk::RGBPixel<uint64_t>>(__VA_ARGS__);                                                                                                                       \
  }                                                                                                                                                                                                    \
  else if(numOfComponents == 4)                                                                                                                                                                        \
  {                                                                                                                                                                                                    \
    templateName<itk::RGBAPixel<DATATYPE>, itk::RGBAPixel<uint64_t>>(__VA_ARGS__);                                                                                                                     \
  }                                                                                                                                                                                                    \
  else if(numOfComponents == 1)                                                                                                                                                                        \
  {                                                                                                                                                                                                    \
    templateName<DATATYPE, uint64_t>(__VA_ARGS__);                                                                                                                                                     \
  }                                                                                                                                                                                                    \
  else                                                                                                                                                                                                 \
  {                                                                                                                                                                                                    \
    observableObj->notifyErrorMessage(#templateName,                                                                                                                                                   \
                                      "The input array's image type is not recognized.  Supported image types"                                                                                         \
                                      " are grayscale (1-component), RGB (3-component), and RGBA (4-component)",                                                                                       \
                                      TemplateHelpers::Errors::UnsupportedImageType);                                                                                                                  \
  }

#define EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(DATATYPE, observableObj, templateName, inputData, algorithm, ...)                                                                                     \
  int numOfComponents = inputData->getNumberOfComponents();                                                                                                                                            \
  if(numOfComponents == 3)                                                                                                                                                                             \
  {                                                                                                                                                                                                    \
    EXECUTE_##algorithm##_FUNCTION_TEMPLATE_HELPER(DATATYPE, observableObj, templateName, inputData, __VA_ARGS__);                                                                                     \
  }                                                                                                                                                                                                    \
  else if(numOfComponents == 4)                                                                                                                                                                        \
  {                                                                                                                                                                                                    \
    EXECUTE_##algorithm##_FUNCTION_TEMPLATE_HELPER(DATATYPE, observableObj, templateName, inputData, __VA_ARGS__);                                                                                     \
  }                                                                                                                                                                                                    \
  else if(numOfComponents == 1)                                                                                                                                                                        \
  {                                                                                                                                                                                                    \
    EXECUTE_##algorithm##_FUNCTION_TEMPLATE_HELPER(DATATYPE, observableObj, templateName, inputData, __VA_ARGS__);                                                                                     \
  }                                                                                                                                                                                                    \
  else                                                                                                                                                                                                 \
  {                                                                                                                                                                                                    \
    observableObj->notifyErrorMessage(#templateName,                                                                                                                                                   \
                                      "The input array's image type is not recognized.  Supported image types"                                                                                         \
                                      " are grayscale (1-component), RGB (3-component), and RGBA (4-component)",                                                                                       \
                                      TemplateHelpers::Errors::UnsupportedImageType);                                                                                                                  \
  }

#define EXECUTE_DATATYPE_FUNCTION_TEMPLATE(observableObj, templateName, inputData, algorithm, ...)                                                                                                     \
  if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(inputData))                                                                                                                                     \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(float, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                   \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<DoubleArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(double, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                  \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int8ArrayType>()(inputData))                                                                                                                                 \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(int8_t, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                  \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(uint8_t, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                 \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int16ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(int16_t, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                 \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(uint16_t, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int32ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(int32_t, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                 \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(uint32_t, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int64ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(int64_t, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                 \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(uint64_t, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<BoolArrayType>()(inputData))                                                                                                                                 \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(bool, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                    \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<DataArray<size_t>>()(inputData))                                                                                                                             \
  {                                                                                                                                                                                                    \
    EXECUTE_ACCUMULATETYPE_FUNCTION_TEMPLATE(size_t, observableObj, templateName, inputData, algorithm, __VA_ARGS__);                                                                                  \
  }                                                                                                                                                                                                    \
  else                                                                                                                                                                                                 \
  {                                                                                                                                                                                                    \
    observableObj->notifyErrorMessage(#templateName, "The input array's data type is not supported", TemplateHelpers::Errors::UnsupportedDataType);                                                    \
  }

#define EXECUTE_STITCH_FUNCTION_TEMPLATE(observableObj, templateName, inputData, ...) EXECUTE_DATATYPE_FUNCTION_TEMPLATE(observableObj, templateName, inputData, STITCH, __VA_ARGS__)

#define EXECUTE_REGISTER_FUNCTION_TEMPLATE(observableObj, templateName, inputData, ...) EXECUTE_DATATYPE_FUNCTION_TEMPLATE(observableObj, templateName, inputData, REGISTER, __VA_ARGS__)

} // namespace ITKTemplateHelpers
