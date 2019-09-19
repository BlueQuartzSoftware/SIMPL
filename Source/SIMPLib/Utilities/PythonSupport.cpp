/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
#include "PythonSupport.h"

PythonSupport::PythonSupport() = default;

PythonSupport::~PythonSupport() = default;

// -----------------------------------------------------------------------------
// This section defines all the Wrapping methods, methods that will wrap an existing pointer to data

#define PS_DEFINE_WRAP_METHOD(TYPE, NAME)                                                                                                                                                              \
  NAME::Pointer PythonSupport::Wrap##NAME(TYPE* array, size_t numTuples, NAME::comp_dims_type& cDims, const QString& name, bool ownsData)                                                              \
  {                                                                                                                                                                                                    \
    return NAME::WrapPointer(array, numTuples, cDims, name, ownsData);                                                                                                                                 \
  }

PS_DEFINE_WRAP_METHOD(int8_t, Int8ArrayType);
PS_DEFINE_WRAP_METHOD(uint8_t, UInt8ArrayType);

PS_DEFINE_WRAP_METHOD(int16_t, Int16ArrayType);
PS_DEFINE_WRAP_METHOD(uint16_t, UInt16ArrayType);

PS_DEFINE_WRAP_METHOD(int32_t, Int32ArrayType);
PS_DEFINE_WRAP_METHOD(uint32_t, UInt32ArrayType);

PS_DEFINE_WRAP_METHOD(int64_t, Int64ArrayType);
PS_DEFINE_WRAP_METHOD(uint64_t, UInt64ArrayType);

PS_DEFINE_WRAP_METHOD(float, FloatArrayType);
PS_DEFINE_WRAP_METHOD(double, DoubleArrayType);

// -----------------------------------------------------------------------------
// This section defines all the Create methods, methods that will simply create a DataArray<T> object and return it back to the caller

#define PS_DEFINE_CREATE_METHOD(TYPE, NAME)                                                                                                                                                            \
  NAME::Pointer PythonSupport::Create##NAME(size_t numTuples, const QString& name, TYPE initValue)                                                                                                     \
  {                                                                                                                                                                                                    \
    return std::shared_ptr<NAME>(new NAME(numTuples, name, initValue));                                                                                                                                \
  }                                                                                                                                                                                                    \
  NAME::Pointer PythonSupport::Create##NAME(size_t numTuples, NAME::comp_dims_type& cDims, const QString& name, TYPE initValue)                                                                        \
  {                                                                                                                                                                                                    \
    return std::shared_ptr<NAME>(new NAME(numTuples, name, cDims, initValue));                                                                                                                         \
  }

PS_DEFINE_CREATE_METHOD(int8_t, Int8ArrayType);
PS_DEFINE_CREATE_METHOD(uint8_t, UInt8ArrayType);

PS_DEFINE_CREATE_METHOD(int16_t, Int16ArrayType);
PS_DEFINE_CREATE_METHOD(uint16_t, UInt16ArrayType);

PS_DEFINE_CREATE_METHOD(int32_t, Int32ArrayType);
PS_DEFINE_CREATE_METHOD(uint32_t, UInt32ArrayType);

PS_DEFINE_CREATE_METHOD(int64_t, Int64ArrayType);
PS_DEFINE_CREATE_METHOD(uint64_t, UInt64ArrayType);

PS_DEFINE_CREATE_METHOD(float, FloatArrayType);
PS_DEFINE_CREATE_METHOD(double, DoubleArrayType);
