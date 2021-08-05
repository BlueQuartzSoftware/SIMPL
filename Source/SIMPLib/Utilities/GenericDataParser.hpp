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

#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

class GenericDataParser
{
public:
  using Self = GenericDataParser;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  const QString getNameOfClass() const
  {
    return QString("GenericDataParser");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("GenericDataParser");
  }

  /**
   * @brief IsA
   * @return
   */
  virtual int32_t IsA() const
  {
    return 0;
  }

  virtual ~GenericDataParser() = default;

  virtual bool allocateArray(size_t numberOfElements)
  {
    (void)(numberOfElements);
    return false;
  }
  virtual void* getVoidPointer()
  {
    return nullptr;
  }
  virtual void setVoidPointer(void* p)
  {
  }

  SIMPL_INSTANCE_PROPERTY(bool, ManageMemory)
  SIMPL_INSTANCE_PROPERTY(size_t, Size)
  SIMPL_INSTANCE_STRING_PROPERTY(ColumnName)
  SIMPL_INSTANCE_PROPERTY(int, ColumnIndex)

  virtual void parse(const QByteArray& token, size_t index)
  {
  }

protected:
  GenericDataParser()
  : m_ManageMemory(false)
  , m_Size(0)
  , m_ColumnName("")
  , m_ColumnIndex(0)
  {
  }

public:
  GenericDataParser(const GenericDataParser&) = delete;            // Copy Constructor Not Implemented
  GenericDataParser(GenericDataParser&&) = delete;                 // Move Constructor Not Implemented
  GenericDataParser& operator=(const GenericDataParser&) = delete; // Copy Assignment Not Implemented
  GenericDataParser& operator=(GenericDataParser&&) = delete;      // Move Assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int32DataParser : public GenericDataParser
{
public:
  using Self = Int32DataParser;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  const QString getNameOfClass() const
  {
    return QString("Int32DataParser");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("Int32DataParser");
  }

  int32_t IsA() const override
  {
    return 1;
  }

  static Pointer New(int32_t* ptr, size_t size, const QString& name, int index)
  {
    Pointer sharedPtr(new Int32DataParser(ptr, size, name, index));
    return sharedPtr;
  }

  ~Int32DataParser() override
  {
    if(m_Ptr != nullptr && getManageMemory())
    {
      delete[](m_Ptr);

      m_Ptr = nullptr;
    }
  }

  void setPtr(int32_t* value)
  {
    this->m_Ptr = value;
  }
  int32_t* getPtr()
  {
    return m_Ptr;
  }

  bool allocateArray(size_t numberOfElements) override
  {
    m_Ptr = new int32_t[numberOfElements]();
    return (m_Ptr != nullptr);
  }

  void* getVoidPointer() override
  {
    return reinterpret_cast<void*>(m_Ptr);
  }
  void setVoidPointer(void* p) override
  {
    m_Ptr = reinterpret_cast<int32_t*>(p);
  }

  int32_t* getPointer(size_t offset)
  {
    return m_Ptr + offset;
  }

  void parse(const QByteArray& token, size_t index) override
  {
    if(index >= getSize())
    {
      throw std::out_of_range("Int32DataParser subscript out of range");
    }
    bool ok = false;
    m_Ptr[index] = token.toInt(&ok, 10);
  }

protected:
  Int32DataParser(int32_t* ptr, size_t size, const QString& name, int index)
  : m_Ptr(ptr)
  {
    setManageMemory(true);
    setSize(size);
    setColumnName(name);
    setColumnIndex(index);
  }

private:
  int32_t* m_Ptr;

public:
  Int32DataParser(const Int32DataParser&) = delete;            // Copy Constructor Not Implemented
  Int32DataParser(Int32DataParser&&) = delete;                 // Move Constructor Not Implemented
  Int32DataParser& operator=(const Int32DataParser&) = delete; // Copy Assignment Not Implemented
  Int32DataParser& operator=(Int32DataParser&&) = delete;      // Move Assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class FloatDataParser : public GenericDataParser
{
public:
  using Self = FloatDataParser;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  const QString getNameOfClass() const
  {
    return QString("FloatDataParser");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("FloatDataParser");
  }

  int32_t IsA() const override
  {
    return 2;
  }

  static Pointer New(float* ptr, size_t size, const QString& name, int index)
  {
    Pointer sharedPtr(new FloatDataParser(ptr, size, name, index));
    return sharedPtr;
  }

  ~FloatDataParser() override
  {
    if(m_Ptr != nullptr && getManageMemory())
    {
      delete[](m_Ptr);

      m_Ptr = nullptr;
    }
  }

  void setPtr(float* value)
  {
    this->m_Ptr = value;
  }
  float* getPtr()
  {
    return m_Ptr;
  }

  bool allocateArray(size_t numberOfElements) override
  {
    m_Ptr = new float[numberOfElements]();
    return (m_Ptr != nullptr);
  }

  void* getVoidPointer() override
  {
    return reinterpret_cast<void*>(m_Ptr);
  }
  void setVoidPointer(void* p) override
  {
    m_Ptr = reinterpret_cast<float*>(p);
  }

  float* getPointer(size_t offset)
  {
    return m_Ptr + offset;
  }

  void parse(const QByteArray& token, size_t index) override
  {
    if(index >= getSize())
    {
      throw std::out_of_range("FloatDataParser subscript out of range");
    }
    bool ok = false;
    m_Ptr[index] = token.toFloat(&ok);
  }

protected:
  FloatDataParser(float* ptr, size_t size, const QString& name, int index)
  : m_Ptr(ptr)
  {
    setManageMemory(true);
    setSize(size);
    setColumnName(name);
    setColumnIndex(index);
  }

private:
  float* m_Ptr;

public:
  FloatDataParser(const FloatDataParser&) = delete;            // Copy Constructor Not Implemented
  FloatDataParser(FloatDataParser&&) = delete;                 // Move Constructor Not Implemented
  FloatDataParser& operator=(const FloatDataParser&) = delete; // Copy Assignment Not Implemented
  FloatDataParser& operator=(FloatDataParser&&) = delete;      // Move Assignment Not Implemented
};
