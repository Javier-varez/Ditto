#ifndef DITTO_FIXED_VECTOR_H_
#define DITTO_FIXED_VECTOR_H_

#include <array>
#include <compare>
#include <cstdint>
#include <iterator>
#include <type_traits>

#include "ditto/result.h"

namespace Ditto {

template <class T, std::size_t CAPACITY>
class FixedVector {
 public:
  enum class Error { VectorIsFull, IndexOutOfRange, VectorIsEmpty };

  template <class U>
  class Iterator {
   public:
    using value_type = T;
    using reference_type = T&;
    using difference_type = ssize_t;

    Iterator() = default;

    Iterator(Iterator&& other)
        : m_vector(other.m_vector), m_index(other.m_index) {}

    Iterator(const Iterator& other)
        : m_vector(other.m_vector), m_index(other.m_index) {}

    Iterator& operator=(const Iterator& other) {
      if (this != &other) {
        m_vector = other.m_vector;
        m_index = other.m_index;
      }
      return *this;
    }

    Iterator& operator=(Iterator&& other) {
      if (this != &other) {
        m_vector = other.m_vector;
        m_index = other.m_index;
      }
      return *this;
    }

    Iterator& operator--() {
      if (m_index > 0) {
        m_index--;
      }
    }

    Iterator operator--(int) {
      Iterator prev = *this;
      if (m_index > 0) {
        m_index--;
      }
      return prev;
    }

    Iterator& operator++() {
      if (m_index < m_vector->size()) {
        m_index++;
      }
      return *this;
    }

    Iterator operator++(int) {
      Iterator prev = *this;
      if (m_index < m_vector->size()) {
        m_index++;
      }
      return prev;
    }

    Iterator& operator+=(size_t val) {
      const auto size = m_vector->size();
      auto index = m_index;
      if (index < size) {
        index += val;
        if (index > size) {
          index = size;
        }
      }
      m_index = index;
      return *this;
    }

    friend auto operator+(const difference_type diff, const Iterator iter)
        -> Iterator {
      return iter + diff;
    }

    auto operator+(const difference_type diff) const -> Iterator {
      const auto size = m_vector->size();
      auto index = m_index;
      if (diff > 0) {
        index += diff;
        if (index > size) {
          index = size;
        }
      } else {
        if ((-diff) > index) {
          index = 0;
        } else {
          index += diff;
        }
      }
      return Iterator{m_vector, index};
    }

    difference_type operator-(const Iterator& other) const {
      return (difference_type)this->m_index - (difference_type)other.m_index;
    }

    Iterator& operator-=(const difference_type diff) {
      if (diff > static_cast<difference_type>(m_index)) {
        m_index = 0;
      } else {
        m_index -= diff;
        const auto vec_size = m_vector->size();
        if (m_index > vec_size) {
          m_index = vec_size;
        }
      }
      return *this;
    }

    Iterator operator-(const difference_type diff) const {
      auto index = m_index;
      if (diff > static_cast<difference_type>(index)) {
        index = 0;
      } else {
        index -= diff;
        const auto vec_size = m_vector->size();
        if (index > vec_size) {
          index = vec_size;
        }
      }
      return Iterator{m_vector, index};
    }

    reference_type operator[](const difference_type offset) const {
      return *vector()[m_index + offset].ok_value();
    }

    T& operator*() const { return *DITTO_UNWRAP(vector()[m_index]); }

    // Default comparison operators between multiple iterators
    friend auto operator<=>(const Iterator&, const Iterator&) = default;

   private:
    FixedVector* m_vector;
    std::size_t m_index;

    auto vector() const -> FixedVector& { return *m_vector; }

    Iterator(FixedVector* vector, std::size_t index)
        : m_vector(vector), m_index(index) {}

    friend FixedVector;
  };

  constexpr FixedVector() = default;

  template <class... U>
  auto emplace(U... args) noexcept -> Result<T*, Error> {
    if (m_length == CAPACITY) {
      return Result<T*, Error>::error(Error::VectorIsFull);
    }

    auto* val = new (&m_storage[m_length++]) T{std::forward<U>(args)...};
    return Result<T*, Error>::ok(val);
  }

  auto push(T value) noexcept -> Result<T*, Error> {
    if (m_length == CAPACITY) {
      return Result<T*, Error>::error(Error::VectorIsFull);
    }

    auto* val = new (&m_storage[m_length++]) T{std::move(value)};
    return Result<T*, Error>::ok(val);
  }

  auto pop() noexcept -> Ditto::Result<T, Error> {
    if (m_length == 0) {
      return Result<T*, Error>::error(Error::VectorIsEmpty);
    }

    const auto idx = m_length;
    m_length--;
    T& inner_item = reinterpret_cast<T*>(m_storage[m_length]);

    const T ret_item = std::move(inner_item);

    inner_item->~T();
    return Result<T, Error>::oK(ret_item);
  }

  [[nodiscard]] auto size() const noexcept -> std::size_t { return m_length; }

  auto operator[](std::size_t index) noexcept -> Result<T*, Error> {
    if (index >= m_length) {
      return Result<T*, Error>::error(Error::IndexOutOfRange);
    }

    return Result<T*, Error>::ok(reinterpret_cast<T*>(&m_storage[index]));
  }

  auto operator[](std::size_t index) const noexcept -> Result<const T*, Error> {
    if (index >= m_length) {
      return Result<const T*, Error>::error(Error::IndexOutOfRange);
    }

    return Result<const T*, Error>::ok(
        reinterpret_cast<const T*>(&m_storage[index]));
  }

  auto cbegin() -> Iterator<const T> { return Iterator<const T>{this, 0}; }

  auto cend() -> Iterator<const T> { return Iterator<const T>{this, m_length}; }

  auto begin() const -> Iterator<const T> { return Iterator<const T>{this, 0}; }

  auto end() const -> Iterator<const T> {
    return Iterator<const T>{this, m_length};
  }

  auto begin() -> Iterator<T> { return Iterator<T>{this, 0}; }

  auto end() -> Iterator<T> { return Iterator<T>{this, m_length}; }

 private:
  using Value = std::aligned_storage_t<sizeof(T), std::alignment_of_v<T>>;

  std::array<Value, CAPACITY> m_storage;
  std::size_t m_length{0};
};

}  // namespace Ditto

#endif  // DITTO_FIXED_VECTOR_H_
