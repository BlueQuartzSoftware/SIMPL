/**
* This code is taken from the Microsoft implementation of <span> which will
* be included in C++17. This is a temporary stand in for that class.
* https://github.com/Microsoft/GSL/blob/master/include/gsl/span
* 
*
*/


#pragma once


#include <array>     // for array
#include <cstddef>   // for ptrdiff_t, size_t, nullptr_t
#include <type_traits> // for enable_if_t, declval, is_convertible, inte...

#include "gsl_assert" // THIS IS OUR LOCAL COPY of gsl_assert.

namespace SIMPL {


template <class ElementType>
class span
{
  public:
    // constants and types
    using element_type = ElementType;
    using value_type = std::remove_cv<ElementType>;
    using index_type = size_t;
    using pointer = element_type*;
    using reference = element_type&;
    
    //    using iterator = details::span_iterator<span<ElementType, Extent>, false>;
    //    using const_iterator = details::span_iterator<span<ElementType, Extent>, true>;
    //    using reverse_iterator = std::reverse_iterator<iterator>;
    //    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    using size_type = index_type;
    
    constexpr index_type size() const noexcept { return m_Size; }
    
    
    constexpr span(pointer ptr, index_type count) : m_Data(ptr), m_Size(count) {}
    
    constexpr bool empty() const noexcept { return size() == 0; }
    
    // [span.elem], span element access
    constexpr reference operator[](index_type idx) const
    { 
#if defined(__APPLE__ ) || defined (__WIN32)
      Expects(idx >= 0 && idx < m_Size);
#endif
      return data()[idx];
    }
    
    
    constexpr reference at(index_type idx) const { return this->operator[](idx); }
    constexpr reference operator()(index_type idx) const { return this->operator[](idx); }
    constexpr pointer data() const noexcept { return m_Data; }
    
    
  private:
    pointer m_Data;
    size_type m_Size = 0; 
    
};

}

