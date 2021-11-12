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
#include <vector>

#include "ditto/assert.h"
#include "ditto/non_null_ptr.h"

namespace Ditto {

template <class T>
class span {
 public:
  span(const span& other) : m_ptr(other.m_ptr), m_length(other.m_length) {}

  span(std::vector<T>& vec) : m_ptr(vec.data()), m_length(vec.size()) {}

  span(const std::vector<std::remove_const_t<T> >& vec)
      : m_ptr(vec.data()), m_length(vec.size()) {}

  template <std::size_t N>
  span(std::array<T, N>& array) : m_ptr(array.data()), m_length(N) {}

  template <std::size_t N>
  span(const std::array<std::remove_const_t<T>, N>& array)
      : m_ptr(array.data()), m_length(N) {}

  span(NonNullPtr<T> ptr, std::size_t length)
      : m_ptr(ptr.get()), m_length(length) {}

  span& operator=(const span& other) {
    if (this != &other) {
      m_ptr = other.m_ptr;
      m_length = other.m_length;
    }
    return *this;
  }

  T* data() const { return m_ptr; }

  T& operator[](std::size_t index) const {
    DITTO_VERIFY(index < m_length);
    return m_ptr[index];
  }

  T& front() const { return m_ptr[0]; }
  T& back() const { return m_ptr[m_length - 1]; }

  std::size_t size() const { return m_length; }
  std::size_t size_bytes() const { return m_length * sizeof(T); }
  bool empty() const { return m_length == 0; }

  span first(std::size_t count) const { return {m_ptr, count}; }
  span last(std::size_t count) const {
    return {m_ptr + m_length - count, count};
  }
  span subspan(std::size_t offset, std::size_t count) const {
    return {m_ptr + offset, count};
  }

 private:
  T* m_ptr;
  std::size_t m_length;
};

}  // namespace Ditto

#endif
