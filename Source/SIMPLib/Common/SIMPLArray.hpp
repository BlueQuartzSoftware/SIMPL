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
#pragma once

#include <array>

#include <QtCore/QMetaObject>

template <class T, unsigned int Dimension> class SIMPLArray
{
public:
  SIMPLArray() = default;
  SIMPLArray(const SIMPLArray&) = default;
  SIMPLArray(SIMPLArray&&) noexcept = default;
  SIMPLArray& operator=(const SIMPLArray&) = default;
  SIMPLArray& operator=(SIMPLArray&&) noexcept = default;
  ~SIMPLArray() = default;

  //========================================= STL INTERFACE COMPATIBILITY =================================
  using size_type = size_t;
  using value_type = T;
  using reference = T&;
  using iterator_category = std::input_iterator_tag;
  using pointer = T*;
  using difference_type = value_type;
  using iterator = typename std::array<T, 3>::iterator;
  using const_iterator = typename std::array<T, 3>::const_iterator;
  //========================================= END STL INTERFACE COMPATIBILITY ==============================

  inline reference operator[](size_type index)
  {
    return m_Array[index];
  }

  inline const T& operator[](size_type index) const
  {
    return m_Array[index];
  }

  inline reference at(size_type index)
  {
    assert(index < Dimension);
    return m_Array[index];
  }

  inline const T& at(size_type index) const
  {
    assert(index < Dimension);
    return m_Array[index];
  }

  iterator begin()
  {
    return m_Array.begin();
  }

  iterator end()
  {
    return m_Array.end();
  }

  const_iterator begin() const
  {
    return m_Array.cbegin();
  }

  const_iterator end() const
  {
    return m_Array.cend();
  }

  value_type* data()
  {
    return m_Array.data();
  }

  size_type size()
  {
    return Dimension;
  }

protected:
  void setValue(size_t i, value_type value)
  {
    m_Array[i] = value;
  }

private:
  std::array<T, Dimension> m_Array;
};

// -----------------------------------------------------------------------------
template <typename T> class IVec3 : public SIMPLArray<T, 3>
{
public:
  IVec3(const IVec3&) = default;
  IVec3(IVec3&&) noexcept = default;
  IVec3& operator=(const IVec3&) = default;
  IVec3& operator=(IVec3&&) noexcept = default;
  ~IVec3() = default;

  IVec3(T x = 0.0f, T y = 0.0f, T z = 0.0f)
  {
    this->setValue(0, x);
    this->setValue(1, y);
    this->setValue(2, z);
  }

  IVec3(std::array<T, 3> data)
  {
    this->setValue(0, data[0]);
    this->setValue(1, data[1]);
    this->setValue(2, data[2]);
  }
  IVec3(std::tuple<T, T, T> data)
  {
    this->setValue(0, std::get<0>(data));
    this->setValue(1, std::get<1>(data));
    this->setValue(2, std::get<2>(data));
  }
  IVec3(const T* data)
  {
    this->setValue(0, data[0]);
    this->setValue(1, data[1]);
    this->setValue(2, data[2]);
  }

  T constexpr x()
  {
    return SIMPLArray<T, 3>::operator[](0);
  }
  T constexpr y()
  {
    return SIMPLArray<T, 3>::operator[](1);
  }
  T constexpr z()
  {
    return SIMPLArray<T, 3>::operator[](2);
  }

  std::tuple<T, T, T> toTuple() const
  {
    return std::make_tuple(SIMPLArray<T, 3>::operator[](0), SIMPLArray<T, 3>::operator[](1), SIMPLArray<T, 3>::operator[](2));
  }
};

using FloatVec3Type = IVec3<float>;
using IntVec3Type = IVec3<int>;
using SizeVec3Type = IVec3<size_t>;

// -----------------------------------------------------------------------------
template <typename T> class IVec2 : public SIMPLArray<T, 2>
{
public:
  IVec2(const IVec2&) = default;
  IVec2(IVec2&&) noexcept = default;
  IVec2& operator=(const IVec2&) = default;
  IVec2& operator=(IVec2&&) noexcept = default;
  ~IVec2() = default;

  IVec2(T x = 0.0f, T y = 0.0f)
  {
    this->setValue(0, x);
    this->setValue(1, y);
  }

  IVec2(std::array<T, 2> data)
  {
    this->setValue(0, data[0]);
    this->setValue(1, data[1]);
  }
  IVec2(std::tuple<T, T> data)
  {
    this->setValue(0, std::get<0>(data));
    this->setValue(1, std::get<1>(data));
  }
  IVec2(const T* data)
  {
    this->setValue(0, data[0]);
    this->setValue(1, data[1]);
  }

  T x() const
  {
    return SIMPLArray<T, 2>::operator[](0);
  }
  T y() const
  {
    return SIMPLArray<T, 2>::operator[](1);
  }

  std::tuple<T, T> toTuple() const
  {
    return std::make_tuple(SIMPLArray<T, 2>::operator[](0), SIMPLArray<T, 2>::operator[](1));
  }
};

using FloatVec2Type = IVec2<float>;
using IntVec2Type = IVec2<int>;
using SizeVec2Type = IVec2<size_t>;
