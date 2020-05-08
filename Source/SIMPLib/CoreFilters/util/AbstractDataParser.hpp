/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <QtCore/QString>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/CoreFilters/util/ParserFunctors.hpp"

class AbstractDataParser
{
public:
  using Self = AbstractDataParser;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  virtual QString getNameOfClass() const
  {
    return QString("AbstractDataParser");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("AbstractDataParser");
  }

  virtual ~AbstractDataParser() = default;

  void setColumnName(const QString& value)
  {
    m_ColumnName = value;
  }
  QString getColumnName()
  {
    return m_ColumnName;
  }

  void setColumnIndex(int32_t value)
  {
    m_ColumnIndex = value;
  }
  int32_t getColumnIndex()
  {
    return m_ColumnIndex;
  }

  virtual void setDataArray(IDataArray::Pointer value)
  {
    m_DataArray = value;
  }

  IDataArray::Pointer getDataArray()
  {
    return m_DataArray;
  }

  virtual IDataArray::Pointer initializeNewDataArray(size_t numTuples, const QString& name, bool allocate)
  {
    return IDataArray::NullPointer();
  }

  virtual ParserFunctor::ErrorObject parse(const QString& token, size_t index) = 0;

protected:
  AbstractDataParser() = default;

private:
  IDataArray::Pointer m_DataArray;
  int32_t m_ColumnIndex = 0;
  QString m_ColumnName = QString("");

  AbstractDataParser(const AbstractDataParser&); // Copy Constructor Not Implemented
  void operator=(const AbstractDataParser&);     // Move assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename ArrayType, class F>
class Parser : public AbstractDataParser
{
public:
  using SelfType = Parser<ArrayType, F>;

  using Self = SelfType;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  virtual QString getNameOfClass() const
  {
    return QString("Parser<ArrayType, F>");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("Parser<ArrayType, F>");
  }

  static Pointer New(typename ArrayType::Pointer ptr, const QString& name, int colIndex)
  {
    Pointer sharedPtr(new Parser(ptr, name, colIndex));
    return sharedPtr;
  }

  ~Parser() override = default;

  static IDataArray::Pointer InitializeNewDataArray(size_t numTuples, const QString& name, bool allocate)
  {
    typename ArrayType::Pointer array = ArrayType::CreateArray(numTuples, name, allocate);
    if(allocate)
    {
      array->initializeWithZeros();
    }
    return array;
  }

  void setDataArray(IDataArray::Pointer value) override
  {
    AbstractDataParser::setDataArray(value);
    m_Ptr = std::dynamic_pointer_cast<ArrayType>(value);
  }

  ParserFunctor::ErrorObject parse(const QString& token, size_t index) override
  {
    ParserFunctor::ErrorObject obj;
    obj.ok = true;
    (*m_Ptr).setValue(index, F()(token, obj));
    return obj;
  }

protected:
  Parser(typename ArrayType::Pointer ptr, const QString& name, int index)
  {
    setColumnName(name);
    setColumnIndex(index);
    setDataArray(ptr);
    m_Ptr = ptr;
  }

private:
  typename ArrayType::Pointer m_Ptr;

public:
  Parser(const Parser&) = delete;            // Copy Constructor Not Implemented
  Parser(Parser&&) = delete;                 // Move Constructor Not Implemented
  Parser& operator=(const Parser&) = delete; // Copy Assignment Not Implemented
  Parser& operator=(Parser&&) = delete;      // Move Assignment
};

using Int8ParserType = Parser<Int8ArrayType, Int8Functor>;
using UInt8ParserType = Parser<UInt8ArrayType, UInt8Functor>;

using Int16ParserType = Parser<Int16ArrayType, Int16Functor>;
using UInt16ParserType = Parser<UInt16ArrayType, UInt16Functor>;

using Int32ParserType = Parser<Int32ArrayType, Int32Functor>;
using UInt32ParserType = Parser<UInt32ArrayType, UInt32Functor>;

using Int64ParserType = Parser<Int64ArrayType, Int64Functor>;
using UInt64ParserType = Parser<UInt64ArrayType, UInt64Functor>;

using FloatParserType = Parser<FloatArrayType, FloatFunctor>;
using DoubleParserType = Parser<DoubleArrayType, DoubleFunctor>;

using StringParserType = Parser<StringDataArray, StringFunctor>;
