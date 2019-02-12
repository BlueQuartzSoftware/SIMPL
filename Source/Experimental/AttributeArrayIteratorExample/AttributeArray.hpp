#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <numeric>

// -----------------------------------------------------------------------------
template <typename T> class AttributeArray
{
  public:

    using comp_dims_type = std::vector<size_t>;

    using size_type = size_t;
    using value_type = T;
    using reference = T&;
    using iterator_category=std::input_iterator_tag;
    using pointer = T*;
    using difference_type = value_type;


    AttributeArray() = default;

    AttributeArray(size_t ntuples, std::string name)
      : m_Name(std::move(name))
    {
      m_Array = resizeAndExtend(ntuples);
    }

    AttributeArray(size_t ntuples, comp_dims_type cdims, const std::string& name) :
      m_NumTuples(ntuples)
    , m_CompDims(std::move(cdims))
    {
      m_NumComponents = std::accumulate(m_CompDims.begin(), m_CompDims.end(), 1, std::multiplies<T>());
      m_InitValue = static_cast<T>(0);
      m_Array = resizeAndExtend(m_NumTuples * m_NumComponents);
    }
    //
    ~AttributeArray()
    {
      deallocate();
    }

    AttributeArray(const AttributeArray&) = default;           // Copy Constructor Not Implemented
    AttributeArray(AttributeArray&&) = delete;                 // Move Constructor Not Implemented
    AttributeArray& operator=(const AttributeArray&) = delete; // Copy Assignment Not Implemented
    AttributeArray& operator=(AttributeArray&&) = delete;      // Move Assignment Not Implemented


    class tuple_iterator
    {
      public:
        using self_type = tuple_iterator;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using difference_type = value_type;
        using iterator_category = std::forward_iterator_tag;

        tuple_iterator(pointer ptr, size_type numComps)
          : ptr_(ptr), num_comps_(numComps)
        {
        }
        self_type operator++()
        {
          ptr_ = ptr_ + num_comps_;
          return *this;
        } // PREFIX
        self_type operator++(int junk)
        {
          self_type i = *this;
          ptr_ = ptr_ + num_comps_;
          return i;
        } // POSTFIX
        reference operator*()
        {
          return *ptr_;
        }
        pointer operator->()
        {
          return ptr_;
        }
        bool operator==(const self_type& rhs)
        {
          return ptr_ == rhs.ptr_;
        }
        bool operator!=(const self_type& rhs)
        {
          return ptr_ != rhs.ptr_;
        }
        reference comp_value(size_type comp)
        {
          return *(ptr_ + comp);
        }

      private:
        pointer ptr_;
        size_t num_comps_;
    };

    class const_tuple_iterator
    {
      public:
        using self_type = const_tuple_iterator;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using difference_type = value_type;
        using iterator_category = std::forward_iterator_tag;

        const_tuple_iterator(pointer ptr, size_type numComps)
          : ptr_(ptr), num_comps_(numComps)
        {
        }
        self_type operator++()
        {
          ptr_ = ptr_ + num_comps_;
          return *this;
        } // PREFIX
        self_type operator++(int junk)
        {
          self_type i = *this;
          ptr_ = ptr_ + num_comps_;
          return i;
        } // POSTFIX
        const value_type& operator*()
        {
          return *ptr_;
        }
        const pointer operator->()
        {
          return ptr_;
        }
        bool operator==(const self_type& rhs)
        {
          return ptr_ == rhs.ptr_;
        }
        bool operator!=(const self_type& rhs)
        {
          return ptr_ != rhs.ptr_;
        }
        const value_type& comp_value(size_type comp)
        {
          return *(ptr_ + comp);
        }

      private:
        pointer ptr_;
        size_t num_comps_;
    };


    class iterator
    {
      public:
        using self_type = iterator;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using difference_type = value_type;
        using iterator_category = std::forward_iterator_tag;

        iterator(pointer ptr)
          : ptr_(ptr)
        {
        }
        iterator(pointer ptr, size_type ununsed)
          : ptr_(ptr)
        {
        }

        self_type operator++()
        {
          ptr_++;
          return *this;
        } // PREFIX
        self_type operator++(int junk)
        {
          self_type i = *this;
          ptr_++;
          return i;
        } // POSTFIX
        reference operator*()
        {
          return *ptr_;
        }
        pointer operator->()
        {
          return ptr_;
        }
        bool operator==(const self_type& rhs)
        {
          return ptr_ == rhs.ptr_;
        }
        bool operator!=(const self_type& rhs)
        {
          return ptr_ != rhs.ptr_;
        }

      private:
        pointer ptr_;
    };

    class const_iterator
    {
      public:
        using self_type = const_iterator;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using difference_type = value_type;
        using iterator_category = std::forward_iterator_tag;
        const_iterator(pointer ptr)
          : ptr_(ptr)
        {
        }
        const_iterator(pointer ptr, size_type unused)
          : ptr_(ptr)
        {
        }

        self_type operator++()
        {
          ptr_++;
          return *this;
        } // PREFIX
        self_type operator++(int junk)
        {
          self_type i = *this;
          ptr_++;
          return i;
        } // POSTFIX
        const value_type& operator*()
        {
          return *ptr_;
        }
        const pointer operator->()
        {
          return ptr_;
        }
        bool operator==(const self_type& rhs)
        {
          return ptr_ == rhs.ptr_;
        }
        bool operator!=(const self_type& rhs)
        {
          return ptr_ != rhs.ptr_;
        }

      private:
        pointer ptr_;
    };


    // ######### Iterators #########

    template<typename IteratorType>
    IteratorType begin() { return IteratorType(m_Array, m_NumComponents); }
    iterator begin()
    {
      return iterator(m_Array);
    }

    template<typename IteratorType>
    IteratorType end() { return IteratorType(m_Array + m_Size, m_NumComponents); }
    iterator end()
    {
      return iterator(m_Array + m_Size);
    }

    const_iterator begin() const
    {
      return const_iterator(m_Array);
    }

    const_iterator end() const
    {
      return const_iterator(m_Array + m_Size);
    }

    //rbegin
    //rend
    //cbegin
    //cend
    //crbegin
    //crend


    // ######### Capacity #########

    size_type size() const
    {
      return m_Size;
    }

    size_type max_size() const
    {
      return m_Size;
    }
    void resize (size_type n)
    {
      resizeAndExtend(n);
    }
    //void resize (size_type n, const value_type& val);
    size_type capacity() const noexcept
    {
      return m_Size;
    }
    bool empty() const noexcept { return (m_Size == 0); }
    // reserve()
    // shrink_to_fit()


    // ######### Element Access #########

    reference operator[](size_type index)
    {
      return m_Array[index];
    }

    const T& operator[](size_type index) const
    {
      return m_Array[index];
    }

    reference at(size_type index)
    {
      assert(index < m_Size);
      return m_Array[index];
    }

    const T& at(size_type index) const
    {
      assert(index < m_Size);
      return m_Array[index];
    }

    reference front() { return m_Array[0];}
    const T& front() const { return m_Array[0]; }

    reference back() { return m_Array[m_MaxId]; }
    const T& back() const { return m_Array[m_MaxId]; }

    T* data() noexcept
    {
      return m_Array;
    }
    const T* data() const noexcept
    {
      return m_Array;
    }

    // ######### Modifiers #########

    /**
   * @brief In the range version (1), the new contents are elements constructed from each of the elements in the range
   * between first and last, in the same order.
   */
    template <class InputIterator>
    void assign (InputIterator first, InputIterator last) // range (1)
    {
      size_type size = last - first;
      resizeAndExtend(size);
      size_type idx = 0;
      while(first != last)
      {
        m_Array[idx] = *first;
        first++;
      }
    }

    /**
   * @brief In the fill version (2), the new contents are n elements, each initialized to a copy of val.
   * @param n
   * @param val
   */
    void assign (size_type n, const value_type& val) //fill (2)
    {
      resizeAndExtend(n);
      std::for_each(begin(), end(), [=](T &n){ n = val; });
    }

    /**
   * @brief In the initializer list version (3), the new contents are copies of the values passed as initializer list, in the same order.
   * @param il
   */
    void assign (std::initializer_list<value_type> il) //  initializer list (3)
    {
      assign(il.begin(), il.end());
    }

    /**
   * @brief push_back
   * @param val
   */
    void push_back (const value_type& val)
    {
      resizeAndExtend(m_Size + 1);
      m_Array[m_MaxId] = val;
    }
    void push_back (value_type&& val)
    {
      resizeAndExtend(m_Size + 1);
      m_Array[m_MaxId] = val;
    }

    void pop_back() { resizeAndExtend(m_Size - 1); }
    //insert
    //iterator erase (const_iterator position)
    //iterator erase (const_iterator first, const_iterator last);
    //swap

    /**
   * @brief Removes all elements from the array (which are destroyed), leaving the container with a size of 0.
   */
    void clear()
    {
      if(nullptr != m_Array && m_OwnsData)
      {
        deallocate();
      }
      m_Array = nullptr;
      m_Size = 0;
      m_OwnsData = true;
      m_MaxId = 0;
      m_IsAllocated = false;
      m_NumTuples = 0;
    }
    //emplace
    //emplace_back




    template <typename Range1, typename Range2>
    bool equal(Range1 const& range1, Range2 const& range2)
    {
      if(range1.size() != range2.size())
      {
        return false;
      }

      return std::equal(begin(range1), end(range1), begin(range2));
    }

    void initializeWithValue(T initValue)
    {
      std::for_each(begin(), end(), [=](T &n){ n = initValue; });
    }

  protected:
    T* resizeAndExtend(size_t size)
    {
      T* newArray;
      size_t newSize;
      size_t oldSize;

      if(size == m_Size) // Requested size is equal to current size.  Do nothing.
      {
        return m_Array;
      }
      newSize = size;
      oldSize = m_Size;

      // Wipe out the array completely if new size is zero.
      if(newSize == 0)
      {
        clear();
        return m_Array;
      }
      // OS X's realloc does not free memory if the new block is smaller.  This
      // is a very serious problem and causes huge amount of memory to be
      // wasted. Do not use realloc on the Mac.
      bool dontUseRealloc = false;
#if defined __APPLE__
      dontUseRealloc = true;
#endif

      // Allocate a new array if we DO NOT own the current array
      if((nullptr != m_Array) && !m_OwnsData)
      {
        // The old array is owned by the user so we cannot try to
        // reallocate it.  Just allocate new memory that we will own.
        newArray = (T*)malloc(newSize * sizeof(T));
        if(!newArray)
        {
          std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. ";
          return nullptr;
        }

        // Copy the data from the old array.
        std::memcpy(newArray, m_Array, (newSize < m_Size ? newSize : m_Size) * sizeof(T));
      }
      else if(!dontUseRealloc)
      {
        // Try to reallocate with minimal memory usage and possibly avoid copying.
        newArray = (T*)realloc(m_Array, newSize * sizeof(T));
        if(!newArray)
        {
          std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. ";
          return nullptr;
        }
      }
      else
      {
        newArray = (T*)malloc(newSize * sizeof(T));
        if(!newArray)
        {
          std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. ";
          return nullptr;
        }

        // Copy the data from the old array.
        if(m_Array != nullptr)
        {
          std::memcpy(newArray, m_Array, (newSize < m_Size ? newSize : m_Size) * sizeof(T));
        }
        // Free the old array
        deallocate();
      }

      // Allocation was successful.  Save it.
      m_Size = newSize;
      m_Array = newArray;

      // This object has now allocated its memory and owns it.
      m_OwnsData = true;

      m_MaxId = newSize - 1;
      m_IsAllocated = true;

      // Initialize the new tuples if newSize is larger than old size
      if(newSize > oldSize)
      {
        //initializeWithValue(m_InitValue, oldSize);
        iterator it(m_Array + oldSize);
        std::for_each(it, end(), [=](T &n){ n = m_InitValue; });
      }

      return m_Array;
    }

    /**
   * @brief deallocates the memory block
   */
    void deallocate()
    {
      // We are going to splat 0xABABAB across the first value of the array as a debugging aid
      auto* cptr = reinterpret_cast<unsigned char*>(m_Array);
      if(nullptr != cptr)
      {
        if(m_Size > 0)
        {
          if(sizeof(T) >= 1)
          {
            cptr[0] = 0xAB;
          }
          if(sizeof(T) >= 2)
          {
            cptr[1] = 0xAB;
          }
          if(sizeof(T) >= 4)
          {
            cptr[2] = 0xAB;
            cptr[3] = 0xAB;
          }
          if(sizeof(T) >= 8)
          {
            cptr[4] = 0xAB;
            cptr[5] = 0xAB;
            cptr[6] = 0xAB;
            cptr[7] = 0xAB;
          }
        }
      }
#if 0
      if (MUD_FLAP_0 != 0xABABABABABABABABul
          || MUD_FLAP_1 != 0xABABABABABABABABul
          || MUD_FLAP_2 != 0xABABABABABABABABul
          || MUD_FLAP_3 != 0xABABABABABABABABul
          || MUD_FLAP_4 != 0xABABABABABABABABul
          || MUD_FLAP_5 != 0xABABABABABABABABul)
      {
        Q_ASSERT(false);
      }
#endif

#if defined(AIM_USE_SSE) && defined(__SSE2__)
      _mm_free(m_buffer);
#else
      free(m_Array);
#endif
      m_Array = nullptr;
      m_IsAllocated = false;
    }

  private:
    T* m_Array = nullptr;
    size_t m_Size = 0;
    size_t m_MaxId = 0;
    size_t m_NumTuples = 0;
    size_t m_NumComponents = 1;
    T m_InitValue = static_cast<T>(0);
    std::vector<size_t> m_CompDims = {1};
    bool m_IsAllocated = false;
    bool m_OwnsData = true;
    std::string m_Name;
};
