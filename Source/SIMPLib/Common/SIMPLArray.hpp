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
#include <tuple>
#include <cassert>
#include <vector>

/**
 * @brief This class is a facade pattern around a std::array<T,D> array to allow for some semantics
 * for either a 2D or 3D point such as X,Y,Z. We Provide 3 concrete implementations that are for 2, 3 and 4 element
 * arrays used in some of the filter parameters. The devloper can extend this quite easily to more
 * elements should they need them
 */
template <class T, unsigned int Dimension>
class SIMPLArray
{
public:
  SIMPLArray() = default;
  SIMPLArray(const SIMPLArray&) = default;
  SIMPLArray(SIMPLArray&&) noexcept = default;
  SIMPLArray& operator=(const SIMPLArray&) = default;
  SIMPLArray& operator=(SIMPLArray&&) noexcept = default;
  ~SIMPLArray() = default;

  /**
   * @brief Constructor using a random access container type as the input. This will copy the data
   */
  template <typename InType>
  SIMPLArray(const InType& data)
  {
    for(size_t i = 0; i < Dimension; i++)
    {
      m_Array[i] = data[i];
    }
  }

  //========================================= STL INTERFACE COMPATIBILITY =================================
  using size_type = size_t;
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using iterator_category = std::input_iterator_tag;
  using pointer = T*;
  using difference_type = value_type;
  using iterator = typename std::array<T, Dimension>::iterator;
  using const_iterator = typename std::array<T, Dimension>::const_iterator;
  //========================================= END STL INTERFACE COMPATIBILITY ==============================

  /**
   * @brief access specified element
   * @param index
   * @return
   */
  inline reference operator[](size_type index)
  {
    return m_Array[index];
  }

  /**
   * @brief access specified element
   * @param index
   * @return
   */
  inline const_reference operator[](size_type index) const
  {
    return m_Array[index];
  }

  /**
   * @brief access specified element with bounds checking
   * @param index
   * @return
   */
  inline reference at(size_type index)
  {
    assert(index < Dimension);
    return m_Array.at(index);
  }

  /**
   * @brief access specified element with bounds checking
   * @param index
   * @return
   */
  inline const_reference at(size_type index) const
  {
    assert(index < Dimension);
    return m_Array.at(index);
  }

  /**
   * @brief returns an iterator to the beginning
   * @return
   */
  iterator begin()
  {
    return m_Array.begin();
  }

  /**
   * @brief returns an iterator to the end
   * @return
   */
  iterator end()
  {
    return m_Array.end();
  }

  /**
   * @brief returns an iterator to the beginning
   * @return
   */
  const_iterator begin() const
  {
    return m_Array.cbegin();
  }

  /**
   * @brief returns an iterator to the end
   * @return
   */
  const_iterator end() const
  {
    return m_Array.cend();
  }

  /**
   * @brief direct access to the underlying array
   * @return
   */
  pointer data()
  {
    return m_Array.data();
  }

  /**
   * @brief Returns the number of elements
   * @return
   */
  size_type size() const
  {
    return Dimension;
  }

  /**
   * @brief operator == Tests for an element by element equivelance of the underlying data
   * @param rhs
   * @return
   */
  bool operator==(const SIMPLArray& rhs) const
  {
    return m_Array == rhs.m_Array;
  }

  /**
   * @brief Converts to another container type. The output type that is being used needs to have the "push_back()" method implemented.
   *
   *   For STL containers this includes Vector, Deque. QVector will also work.
   */
  template <typename OutContainerType>
  OutContainerType toContainer() const
  {
    OutContainerType dest(Dimension);
    for(typename OutContainerType::size_type i = 0; i < Dimension; i++)
    {
      dest[i] = m_Array[i];
    }
    return dest;
  }

  /**
   * @brief Returns a copy of the data as a std::array<T,Dimension>
   * @return
   */
  std::array<T, Dimension> toArray() const
  {
    return m_Array;
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
template <typename T>
class IVec2 : public SIMPLArray<T, 2>
{
  using ParentType = SIMPLArray<T, 2>;

public:
  IVec2(const IVec2&) = default;
  IVec2(IVec2&&) noexcept = default;
  IVec2& operator=(const IVec2&) = default;
  IVec2& operator=(IVec2&&) noexcept = default;
  ~IVec2() = default;

  /**
   * @brief IVec2 Default constructor initializes all values to ZERO.
   */
  IVec2()
  {
    (*this)[0] = static_cast<T>(0);
    (*this)[1] = static_cast<T>(0);
  }
  /**
   * @brief IVec2
   * @param x
   * @param y
   */
  IVec2(T x, T y)
  {
    (*this)[0] = x;
    (*this)[1] = y;
  }
  /**
   * @brief IVec2
   * @param data
   */
  IVec2(const std::array<T, 2>& data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
  }
  /**
   * @brief IVec2
   * @param data
   */
  IVec2(const std::tuple<T, T>& data)
  {
    (*this)[0] = std::get<0>(data);
    (*this)[1] = std::get<1>(data);
  }
  /**
   * @brief IVec2
   * @param data
   */
  IVec2(const T* data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
  }

  /**
   * @brief IVec2
   * @param data
   */
  IVec2(const std::vector<T>& data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
  }

  inline T getX() const
  {
    return ParentType::operator[](0);
  }
  inline T getY() const
  {
    return ParentType::operator[](1);
  }
  inline void setX(const T& x)
  {
    (*this)[0] = x;
  }
  inline void setY(const T& y)
  {
    (*this)[1] = y;
  }

  /**
   * @brief this is a convenience function that helps with the python bindings
   */
  inline void setValues(const T& x, const T& y)
  {
    (*this)[0] = x;
    (*this)[1] = y;
  }

  /**
   * @brief toTuple Converts the internal data structure to a std::tuple<T, T, T>
   * @return
   */
  std::tuple<T, T> toTuple() const
  {
    return std::make_tuple(getX(), getY());
  }
  /**
   *
   */
  template <typename OutType>
  IVec2<OutType> convertType()
  {
    return IVec2<OutType>(static_cast<OutType>((*this)[0]), static_cast<OutType>((*this)[1]));
  }
};

using FloatVec2Type = IVec2<float>;
using IntVec2Type = IVec2<int>;
using SizeVec2Type = IVec2<size_t>;

// -----------------------------------------------------------------------------
template <typename T>
class IVec3 : public SIMPLArray<T, 3>
{
  using ParentType = SIMPLArray<T, 3>;

public:
  IVec3(const IVec3&) = default;
  IVec3(IVec3&&) noexcept = default;
  IVec3& operator=(const IVec3&) = default;
  IVec3& operator=(IVec3&&) noexcept = default;
  ~IVec3() = default;

  /**
   * @brief IVec3 Default constructor initializes all values to ZERO.
   */
  IVec3()
  {
    (*this)[0] = static_cast<T>(0);
    (*this)[1] = static_cast<T>(0);
    (*this)[2] = static_cast<T>(0);
  }
  /**
   * @brief IVec3
   * @param x
   * @param y
   * @param z
   */
  IVec3(T x, T y, T z)
  {
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
  }
  /**
   * @brief IVec3
   * @param data
   */
  IVec3(const std::array<T, 3>& data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
  }
  /**
   * @brief IVec3
   * @param data
   */
  IVec3(const std::tuple<T, T, T>& data)
  {
    (*this)[0] = std::get<0>(data);
    (*this)[1] = std::get<1>(data);
    (*this)[2] = std::get<2>(data);
  }
  /**
   * @brief IVec3
   * @param data
   */
  IVec3(const T* data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
  }
  /**
   * @brief IVec3
   * @param data
   */
  IVec3(const std::vector<T>& data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
  }

  inline T getX() const
  {
    return ParentType::operator[](0);
  }
  inline T getY() const
  {
    return ParentType::operator[](1);
  }
  inline T getZ() const
  {
    return ParentType::operator[](2);
  }
  inline void setX(const T& x)
  {
    (*this)[0] = x;
  }
  inline void setY(const T& y)
  {
    (*this)[1] = y;
  }
  inline void setZ(const T& z)
  {
    (*this)[2] = z;
  }

  /**
   * @brief this is a convenience function that helps with the python bindings
   */
  inline void setValues(const T& x, const T& y, const T& z)
  {
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
  }

  /**
   * @brief toTuple Converts the internal data structure to a std::tuple<T, T, T>
   * @return
   */
  std::tuple<T, T, T> toTuple() const
  {
    return std::make_tuple(getX(), getY(), getZ());
  }

  /**
   * @brief Converts to a new SIMPLArray with a different storage data type
   */
  template <typename OutType>
  IVec3<OutType> convertType()
  {
    return IVec3<OutType>(static_cast<OutType>((*this)[0]), static_cast<OutType>((*this)[1]), static_cast<OutType>((*this)[2]));
  }
};

// -----------------------------------------------------------------------------
template <typename T>
class IVec4 : public SIMPLArray<T, 4>
{
  using ParentType = SIMPLArray<T, 4>;

public:
  IVec4(const IVec4&) = default;
  IVec4(IVec4&&) noexcept = default;
  IVec4& operator=(const IVec4&) = default;
  IVec4& operator=(IVec4&&) noexcept = default;
  ~IVec4() = default;

  /**
   * @brief IVec4 Default constructor initializes all values to ZERO.
   */
  IVec4()
  {
    (*this)[0] = static_cast<T>(0);
    (*this)[1] = static_cast<T>(0);
    (*this)[2] = static_cast<T>(0);
    (*this)[3] = static_cast<T>(0);
  }
  /**
   * @brief IVec4
   * @param x
   * @param y
   * @param z
   * @param w
   */
  IVec4(T x, T y, T z, T w)
  {
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
    (*this)[3] = w;
  }
  /**
   * @brief IVec4
   * @param data
   */
  IVec4(const std::array<T, 4>& data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
    (*this)[3] = data[3];
  }
  /**
   * @brief IVec4
   * @param data
   */
  IVec4(const std::tuple<T, T, T, T>& data)
  {
    (*this)[0] = std::get<0>(data);
    (*this)[1] = std::get<1>(data);
    (*this)[2] = std::get<2>(data);
    (*this)[3] = std::get<3>(data);
  }
  /**
   * @brief IVec4
   * @param data
   */
  IVec4(const T* data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
    (*this)[3] = data[3];
  }
  /**
   * @brief IVec4
   * @param data
   */
  IVec4(const std::vector<T>& data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
    (*this)[3] = data[3];
  }

  inline T getX() const
  {
    return ParentType::operator[](0);
  }
  inline T getY() const
  {
    return ParentType::operator[](1);
  }
  inline T getZ() const
  {
    return ParentType::operator[](2);
  }
  inline T getW() const
  {
    return ParentType::operator[](3);
  }
  inline void setX(const T& x)
  {
    (*this)[0] = x;
  }
  inline void setY(const T& y)
  {
    (*this)[1] = y;
  }
  inline void setZ(const T& z)
  {
    (*this)[2] = z;
  }
  inline void setW(const T& w)
  {
    (*this)[3] = w;
  }

  /**
   * @brief this is a convenience function that helps with the python bindings
   */
  inline void setValues(const T& x, const T& y, const T& z, const T& w)
  {
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
    (*this)[3] = w;
  }

  /**
   * @brief toTuple Converts the internal data structure to a std::tuple<T, T, T>
   * @return
   */
  std::tuple<T, T, T, T> toTuple() const
  {
    return std::make_tuple(getX(), getY(), getZ(), getW());
  }

  /**
   * @brief Converts this array into another array using a static_cast<OutType> mechanism
   */
  template <typename OutType>
  IVec4<OutType> convertType()
  {
    return IVec4<OutType>(static_cast<OutType>((*this)[0]), static_cast<OutType>((*this)[1]), static_cast<OutType>((*this)[2]), static_cast<OutType>((*this)[3]));
  }
};

// -----------------------------------------------------------------------------
template <typename T>
class IVec6 : public SIMPLArray<T, 6>
{
  using ParentType = SIMPLArray<T, 6>;

public:
  IVec6(const IVec6&) = default;
  IVec6(IVec6&&) noexcept = default;
  IVec6& operator=(const IVec6&) = default;
  IVec6& operator=(IVec6&&) noexcept = default;
  ~IVec6() = default;

  /**
   * @brief IVec6 Default constructor initializes all values to ZERO.
   */
  IVec6()
  {
    (*this)[0] = static_cast<T>(0);
    (*this)[1] = static_cast<T>(0);
    (*this)[2] = static_cast<T>(0);
    (*this)[3] = static_cast<T>(0);
    (*this)[4] = static_cast<T>(0);
    (*this)[5] = static_cast<T>(0);
  }
  /**
   * @brief IVec6
   * @param x
   * @param y
   * @param z
   * @param a
   * @param b
   * @param c
   */
  IVec6(T x, T y, T z, T a, T b, T c)
  {
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
    (*this)[3] = a;
    (*this)[4] = b;
    (*this)[5] = c;
  }
  /**
   * @brief IVec6
   * @param data
   */
  IVec6(const std::array<T, 6>& data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
    (*this)[3] = data[3];
    (*this)[4] = data[4];
    (*this)[5] = data[5];
  }
  /**
   * @brief IVec6
   * @param data
   */
  IVec6(const std::tuple<T, T, T, T, T, T>& data)
  {
    (*this)[0] = std::get<0>(data);
    (*this)[1] = std::get<1>(data);
    (*this)[2] = std::get<2>(data);
    (*this)[3] = std::get<3>(data);
    (*this)[4] = std::get<4>(data);
    (*this)[5] = std::get<5>(data);
  }
  /**
   * @brief IVec6
   * @param data
   */
  IVec6(const T* data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
    (*this)[3] = data[3];
    (*this)[4] = data[4];
    (*this)[5] = data[5];
  }
  /**
   * @brief IVec6
   * @param data
   */
  IVec6(const std::vector<T>& data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
    (*this)[3] = data[3];
    (*this)[4] = data[4];
    (*this)[5] = data[5];
  }

  /**
   * @brief this is a convenience function that helps with the python bindings
   */
  inline void setValues(const T& x, const T& y, const T& z, const T& i, const T& j, const T& k)
  {
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
    (*this)[3] = i;
    (*this)[4] = j;
    (*this)[5] = k;
  }

  /**
   * @brief toTuple Converts the internal data structure to a std::tuple<T, T, T>
   * @return
   */
  std::tuple<T, T, T, T, T, T> toTuple() const
  {
    return std::make_tuple((*this)[0], (*this)[1], (*this)[2], (*this)[3], (*this)[4], (*this)[5]);
  }

  /**
   * @brief Converts this array into another array using a static_cast<OutType> mechanism
   */
  template <typename OutType>
  IVec6<OutType> convertType()
  {
    return IVec6<OutType>(static_cast<OutType>((*this)[0]), static_cast<OutType>((*this)[1]), static_cast<OutType>((*this)[2]), static_cast<OutType>((*this)[3]), static_cast<OutType>((*this)[4]),
                          static_cast<OutType>((*this)[5]));
  }
};

// -----------------------------------------------------------------------------
template <typename T>
class IVec7 : public SIMPLArray<T, 7>
{
  using ParentType = SIMPLArray<T, 7>;

public:
  IVec7(const IVec7&) = default;
  IVec7(IVec7&&) noexcept = default;
  IVec7& operator=(const IVec7&) = default;
  IVec7& operator=(IVec7&&) noexcept = default;
  ~IVec7() = default;

  /**
   * @brief IVec6 Default constructor initializes all values to ZERO.
   */
  IVec7()
  {
    (*this)[0] = static_cast<T>(0);
    (*this)[1] = static_cast<T>(0);
    (*this)[2] = static_cast<T>(0);
    (*this)[3] = static_cast<T>(0);
    (*this)[4] = static_cast<T>(0);
    (*this)[5] = static_cast<T>(0);
    (*this)[6] = static_cast<T>(0);
  }
  /**
   * @brief IVec6
   * @param x
   * @param y
   * @param z
   * @param a
   * @param b
   * @param c
   */
  IVec7(T x, T y, T z, T a, T b, T c, T d)
  {
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
    (*this)[3] = a;
    (*this)[4] = b;
    (*this)[5] = c;
    (*this)[6] = d;
  }
  /**
   * @brief IVec6
   * @param data
   */
  IVec7(const std::array<T, 7>& data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
    (*this)[3] = data[3];
    (*this)[4] = data[4];
    (*this)[5] = data[5];
    (*this)[6] = data[6];
  }
  /**
   * @brief IVec6
   * @param data
   */
  IVec7(const std::tuple<T, T, T, T, T, T, T>& data)
  {
    (*this)[0] = std::get<0>(data);
    (*this)[1] = std::get<1>(data);
    (*this)[2] = std::get<2>(data);
    (*this)[3] = std::get<3>(data);
    (*this)[4] = std::get<4>(data);
    (*this)[5] = std::get<5>(data);
    (*this)[6] = std::get<6>(data);
  }
  /**
   * @brief IVec6
   * @param data
   */
  IVec7(const T* data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
    (*this)[3] = data[3];
    (*this)[4] = data[4];
    (*this)[5] = data[5];
    (*this)[6] = data[6];
  }
  /**
   * @brief IVec6
   * @param data
   */
  IVec7(const std::vector<T>& data)
  {
    (*this)[0] = data[0];
    (*this)[1] = data[1];
    (*this)[2] = data[2];
    (*this)[3] = data[3];
    (*this)[4] = data[4];
    (*this)[5] = data[5];
    (*this)[6] = data[6];
  }

  /**
   * @brief this is a convenience function that helps with the python bindings
   */
  inline void setValues(const T& x, const T& y, const T& z, const T& i, const T& j, const T& k, const T& l)
  {
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
    (*this)[3] = i;
    (*this)[4] = j;
    (*this)[5] = k;
    (*this)[6] = l;
  }

  /**
   * @brief toTuple Converts the internal data structure to a std::tuple<T, T, T>
   * @return
   */
  std::tuple<T, T, T, T, T, T, T> toTuple() const
  {
    return std::make_tuple((*this)[0], (*this)[1], (*this)[2], (*this)[3], (*this)[4], (*this)[5], (*this)[6]);
  }

  /**
   * @brief Converts this array into another array using a static_cast<OutType> mechanism
   */
  template <typename OutType>
  IVec7<OutType> convertType()
  {
    return IVec6<OutType>(static_cast<OutType>((*this)[0]), static_cast<OutType>((*this)[1]), static_cast<OutType>((*this)[2]), static_cast<OutType>((*this)[3]), static_cast<OutType>((*this)[4]),
                          static_cast<OutType>((*this)[5]), static_cast<OutType>((*this)[6]));
  }
};

using FloatVec3Type = IVec3<float>;
using IntVec3Type = IVec3<int>;
using SizeVec3Type = IVec3<size_t>;

using FloatVec4Type = IVec4<float>;
using IntVec4Type = IVec4<int>;
using SizeVec4Type = IVec4<size_t>;

using FloatVec6Type = IVec6<float>;
using IntVec6Type = IVec6<int32_t>;

using FloatVec7Type = IVec7<float>;
using IntVec7Type = IVec7<int32_t>;
