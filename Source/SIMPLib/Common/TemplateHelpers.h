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

//-- C++11 Includes
#include <memory>

#include <QtCore/QString>

#include "SIMPLib/DataContainers/DataArrayPath.h"

class AbstractFilter;

class IDataArray;
using IDataArrayWkPtr = std::weak_ptr<IDataArray>;
using IDataArrayShPtr = std::shared_ptr<IDataArray>;

/*
 * @brief This file contains a namespace and some C++ Functors that help to reduce the amout of code that filters
 * need when the filter needs to determine if an IDataArray is of a certain primitive type.
 */

namespace TemplateHelpers
{

#define EXECUTE_FUNCTION_TEMPLATE(observableObj, templateName, inputData, ...)                                                                                                                         \
  if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(inputData))                                                                                                                                     \
  {                                                                                                                                                                                                    \
    templateName<float>(__VA_ARGS__);                                                                                                                                                                  \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<DoubleArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    templateName<double>(__VA_ARGS__);                                                                                                                                                                 \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int8ArrayType>()(inputData))                                                                                                                                 \
  {                                                                                                                                                                                                    \
    templateName<int8_t>(__VA_ARGS__);                                                                                                                                                                 \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    templateName<uint8_t>(__VA_ARGS__);                                                                                                                                                                \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int16ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    templateName<int16_t>(__VA_ARGS__);                                                                                                                                                                \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    templateName<uint16_t>(__VA_ARGS__);                                                                                                                                                               \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int32ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    templateName<int32_t>(__VA_ARGS__);                                                                                                                                                                \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    templateName<uint32_t>(__VA_ARGS__);                                                                                                                                                               \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int64ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    templateName<int64_t>(__VA_ARGS__);                                                                                                                                                                \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    templateName<uint64_t>(__VA_ARGS__);                                                                                                                                                               \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<BoolArrayType>()(inputData))                                                                                                                                 \
  {                                                                                                                                                                                                    \
    templateName<bool>(__VA_ARGS__);                                                                                                                                                                   \
  }                                                                                                                                                                                                    \
  else                                                                                                                                                                                                 \
  {                                                                                                                                                                                                    \
    observableObj->notifyErrorMessage(#templateName, "The input array was of unsupported type", TemplateHelpers::Errors::UnsupportedType);                                                             \
  }

#define EXECUTE_TEMPLATE(observableObj, templateName, inputData, ...)                                                                                                                                  \
  if(templateName<int8_t>()(inputData))                                                                                                                                                                \
  {                                                                                                                                                                                                    \
    templateName<int8_t> pimpl;                                                                                                                                                                        \
    pimpl.Execute(__VA_ARGS__);                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  else if(templateName<uint8_t>()(inputData))                                                                                                                                                          \
  {                                                                                                                                                                                                    \
    templateName<uint8_t> pimpl;                                                                                                                                                                       \
    pimpl.Execute(__VA_ARGS__);                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  else if(templateName<int16_t>()(inputData))                                                                                                                                                          \
  {                                                                                                                                                                                                    \
    templateName<int16_t> pimpl;                                                                                                                                                                       \
    pimpl.Execute(__VA_ARGS__);                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  else if(templateName<uint16_t>()(inputData))                                                                                                                                                         \
  {                                                                                                                                                                                                    \
    templateName<uint16_t> pimpl;                                                                                                                                                                      \
    pimpl.Execute(__VA_ARGS__);                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  else if(templateName<int32_t>()(inputData))                                                                                                                                                          \
  {                                                                                                                                                                                                    \
    templateName<int32_t> pimpl;                                                                                                                                                                       \
    pimpl.Execute(__VA_ARGS__);                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  else if(templateName<uint32_t>()(inputData))                                                                                                                                                         \
  {                                                                                                                                                                                                    \
    templateName<uint32_t> pimpl;                                                                                                                                                                      \
    pimpl.Execute(__VA_ARGS__);                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  else if(templateName<int64_t>()(inputData))                                                                                                                                                          \
  {                                                                                                                                                                                                    \
    templateName<int64_t> pimpl;                                                                                                                                                                       \
    pimpl.Execute(__VA_ARGS__);                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  else if(templateName<uint64_t>()(inputData))                                                                                                                                                         \
  {                                                                                                                                                                                                    \
    templateName<uint64_t> pimpl;                                                                                                                                                                      \
    pimpl.Execute(__VA_ARGS__);                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  else if(templateName<float>()(inputData))                                                                                                                                                            \
  {                                                                                                                                                                                                    \
    templateName<float> pimpl;                                                                                                                                                                         \
    pimpl.Execute(__VA_ARGS__);                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  else if(templateName<double>()(inputData))                                                                                                                                                           \
  {                                                                                                                                                                                                    \
    templateName<double> pimpl;                                                                                                                                                                        \
    pimpl.Execute(__VA_ARGS__);                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  else if(templateName<bool>()(inputData))                                                                                                                                                             \
  {                                                                                                                                                                                                    \
    templateName<bool> pimpl;                                                                                                                                                                          \
    pimpl.Execute(__VA_ARGS__);                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  else                                                                                                                                                                                                 \
  {                                                                                                                                                                                                    \
    observableObj->notifyErrorMessage(#templateName, "The input array was of unsupported type", TemplateHelpers::Errors::UnsupportedType);                                                             \
  }

/**
 * @brief Define from Error Codes
 */
namespace Errors
{
const int UnsupportedType(-401);
const int MissingDataContainer(-402);
const int MissingAttributeMatrix(-403);
const int MissingArray(-404);
} // namespace Errors

/**
 * @brief This class (functor) simply returns true or false if the IDataArray can be downcast to a certain DataArray type
 * parameterized by the template parameter T.
 */
template <typename T> class CanDynamicCast
{
public:
  CanDynamicCast() = default;
  virtual ~CanDynamicCast() = default;
  bool operator()(IDataArrayShPtr p)
  {
    return (std::dynamic_pointer_cast<T>(p).get() != nullptr);
  }
};

/**
 * @brief The CreateNonPrereqArrayFromArrayType class will create a DataArray of the same type as another DataArray and attach it to
 * a supplied data container.
 */
class SIMPLib_EXPORT CreateNonPrereqArrayFromArrayType
{
public:
  CreateNonPrereqArrayFromArrayType() = default;
  ~CreateNonPrereqArrayFromArrayType() = default;

  /**
   * @brief operator ()
   * @param f
   * @param arrayPath
   * @param compDims
   * @param sourceArrayType
   * @return
   */
  IDataArrayWkPtr operator()(AbstractFilter* f, DataArrayPath arrayPath, QVector<size_t> compDims, IDataArrayShPtr sourceArrayType);
};

/**
 * @brief The CreateNonPrereqArrayFromArrayType class will create a DataArray of the same type as another DataArray and attach it to
 * a supplied data container.
 */
class SIMPLib_EXPORT CreateNonPrereqArrayFromTypeEnum
{
public:
  CreateNonPrereqArrayFromTypeEnum() = default;
  ~CreateNonPrereqArrayFromTypeEnum() = default;

  /**
   * @brief operator ()
   * @param f
   * @param arrayPath
   * @param compDims
   * @param arrayType
   * @param initValue
   * @return
   */
  IDataArrayWkPtr operator()(AbstractFilter* f, DataArrayPath arrayPath, QVector<size_t> compDims, int arrayType, double initValue);
};

/**
 * @brief The CreateArrayFromArrayType class will create a DataArray of the same type as another DataArray but not attach
 * it to any data container.
 */
class SIMPLib_EXPORT CreateArrayFromArrayType
{
public:
  CreateArrayFromArrayType() = default;
  ~CreateArrayFromArrayType() = default;

  IDataArrayShPtr operator()(AbstractFilter* f, size_t numTuples, QVector<size_t> compDims, QString arrayName, bool allocate, IDataArrayShPtr sourceArrayType)
  {
    CreateArrayFromArrayType classInstance;
    QVector<size_t> tupleDims(1, numTuples);
    return classInstance(f, tupleDims, compDims, arrayName, allocate, sourceArrayType);
  }

  /**
   * @brief operator ()
   * @param f
   * @param tupleDims
   * @param compDims
   * @param arrayName
   * @param allocate
   * @param sourceArrayType
   * @return
   */
  IDataArrayShPtr operator()(AbstractFilter* f, QVector<size_t> tupleDims, QVector<size_t> compDims, QString arrayName, bool allocate, IDataArrayShPtr sourceArrayType);
};

/**
 * @brief The CreateArrayFromType class will create a DataArray matching the type supplied as a QString.  Available types are listed
 * in Constants.h.
 */
class SIMPLib_EXPORT CreateArrayFromType
{
public:
  CreateArrayFromType() = default;
  ~CreateArrayFromType() = default;

  /**
   * @brief operator ()
   * @param f
   * @param numTuples
   * @param compDims
   * @param arrayName
   * @param allocate
   * @param type
   * @return
   */
  IDataArrayShPtr operator()(AbstractFilter* f, size_t numTuples, QVector<size_t> compDims, QString arrayName, bool allocate, QString type);

  /**
   * @brief operator ()
   * @param f
   * @param tupleDims
   * @param compDims
   * @param arrayName
   * @param allocate
   * @param type
   * @return
   */
  IDataArrayShPtr operator()(AbstractFilter* f, QVector<size_t> tupleDims, QVector<size_t> compDims, QString arrayName, bool allocate, QString type);
};

/**
 * @brief The GetPrereqArrayFromPath class will return a pointer to a DataArray of unknown type given the path.
 */
class SIMPLib_EXPORT GetPrereqArrayFromPath
{
public:
  GetPrereqArrayFromPath() = default;
  virtual ~GetPrereqArrayFromPath() = default;

  /**
   * @brief operator ()
   * @param f
   * @param arrayPath
   * @param compDims
   * @return
   */
  IDataArrayWkPtr operator()(AbstractFilter* f, DataArrayPath arrayPath, QVector<size_t>& compDims);

private:
  GetPrereqArrayFromPath(const GetPrereqArrayFromPath&); // Copy Constructor Not Implemented
  void operator=(const GetPrereqArrayFromPath&);         // Move assignment Not Implemented
};

} // namespace TemplateHelpers
