#pragma once

#if defined(USE_STD_TEMPLATES) && defined(__has_include) && \
    __has_include(<span>) && defined(__cplusplus) && __cplusplus >= 201703L

#include <span>

namespace Ditto {
using std::span;
}  // namespace Ditto

#else  // std::span<T> not available, define our own

#include <array>
#include <cstdio>
#include <iosfwd>
#include <iterator>
#include <type_traits>
#include <vector>

#include "ditto/assert.h"
#include "ditto/non_null_ptr.h"

namespace Ditto {

template <class T>
class span_iterator {
 public:
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using reference = T&;
  using iterator_category = std::random_access_iterator_tag;

  constexpr span_iterator& operator+=(std::size_t incr) {
    m_ptr += incr;
    return *this;
  }

  constexpr span_iterator& operator-=(std::size_t incr) {
    m_ptr -= incr;
    return *this;
  }

  constexpr span_iterator& operator++() {
    m_ptr++;
    return *this;
  }

  constexpr span_iterator operator++(int) {
    span_iterator prev{m_ptr};
    m_ptr++;
    return prev;
  }

  constexpr span_iterator& operator--() {
    m_ptr--;
    return *this;
  }

  constexpr span_iterator operator--(int) {
    span_iterator prev{m_ptr};
    m_ptr--;
    return prev;
  }

  constexpr reference operator[](std::size_t index) const {
    return m_ptr[index];
  }

  constexpr difference_type operator-(span_iterator other) const {
    return m_ptr - other.m_ptr;
  }

  constexpr reference operator*() const { return *m_ptr; }

  constexpr pointer operator->() const { return m_ptr; }

 private:
  T* m_ptr;

  constexpr explicit span_iterator(T* ptr) : m_ptr(ptr) {}

  template <class U>
  friend class span;

  template <class U>
  friend constexpr bool operator==(const span_iterator<U> one,
                                   const span_iterator<U>& another);
  template <class U>
  friend constexpr bool operator>(const span_iterator<U> one,
                                  const span_iterator<U>& another);
  template <class U>
  friend constexpr bool operator<(const span_iterator<U> one,
                                  const span_iterator<U>& another);
  template <class U>
  friend constexpr span_iterator operator+(const span_iterator<U> one,
                                           std::size_t);
  template <class U>
  friend constexpr span_iterator operator-(const span_iterator<U> one,
                                           std::size_t);
};

template <class T>
constexpr bool operator==(const span_iterator<T> one,
                          const span_iterator<T>& another) {
  return one.m_ptr == another.m_ptr;
}

template <class T>
constexpr bool operator>(const span_iterator<T> one,
                         const span_iterator<T>& another) {
  return one.m_ptr > another.m_ptr;
}

template <class T>
constexpr bool operator<(const span_iterator<T> one,
                         const span_iterator<T>& another) {
  return one.m_ptr < another.m_ptr;
}

template <class T>
constexpr span_iterator<T> operator+(const span_iterator<T> iter,
                                     std::size_t offset) {
  return span_iterator<T>{iter.m_ptr + offset};
}

template <class T>
constexpr span_iterator<T> operator+(std::size_t offset,
                                     const span_iterator<T> iter) {
  return span_iterator<T>{iter.m_ptr + offset};
}

template <class T>
constexpr span_iterator<T> operator-(const span_iterator<T> iter,
                                     std::size_t offset) {
  return span_iterator<T>{iter.m_ptr - offset};
}

template <class T>
constexpr span_iterator<T> operator-(std::size_t offset,
                                     const span_iterator<T> iter) {
  return span_iterator<T>{iter.m_ptr - offset};
}

template <class T>
class span {
 public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using iterator = span_iterator<T>;
  using reverse_iterator = std::reverse_iterator<iterator>;

  constexpr span(const span& other)
      : m_ptr(other.m_ptr), m_length(other.m_length) {}

  constexpr span(std::vector<T>& vec)
      : m_ptr(vec.data()), m_length(vec.size()) {}

  constexpr span(const std::vector<std::remove_const_t<T> >& vec)
      : m_ptr(vec.data()), m_length(vec.size()) {}

  template <std::size_t N>
  constexpr span(std::array<T, N>& array) : m_ptr(array.data()), m_length(N) {}

  template <std::size_t N>
  constexpr span(const std::array<std::remove_const_t<T>, N>& array)
      : m_ptr(array.data()), m_length(N) {}

  constexpr span(NonNullPtr<T> ptr, std::size_t length)
      : m_ptr(ptr.get()), m_length(length) {}

  constexpr span& operator=(const span& other) {
    if (this != &other) {
      m_ptr = other.m_ptr;
      m_length = other.m_length;
    }
    return *this;
  }

  constexpr pointer data() const { return m_ptr; }

  constexpr reference operator[](std::size_t index) const {
    DITTO_VERIFY(index < m_length);
    return m_ptr[index];
  }

  constexpr reference front() const { return m_ptr[0]; }
  constexpr reference back() const { return m_ptr[m_length - 1]; }

  constexpr std::size_t size() const { return m_length; }
  constexpr std::size_t size_bytes() const { return m_length * sizeof(T); }
  constexpr bool empty() const { return m_length == 0; }

  constexpr span first(std::size_t count) const { return {m_ptr, count}; }
  constexpr span last(std::size_t count) const {
    return {m_ptr + m_length - count, count};
  }
  constexpr span subspan(std::size_t offset, std::size_t count) const {
    return {m_ptr + offset, count};
  }

  constexpr iterator begin() const { return iterator{m_ptr}; }
  constexpr iterator end() const { return iterator{&m_ptr[m_length]}; }
  constexpr reverse_iterator rbegin() const {
    return std::make_reverse_iterator(end());
  }
  constexpr reverse_iterator rend() const {
    return std::make_reverse_iterator(begin());
  }

 private:
  T* m_ptr;
  std::size_t m_length;
};

}  // namespace Ditto

#endif
