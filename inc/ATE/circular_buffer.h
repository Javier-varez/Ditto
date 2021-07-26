
#pragma once

#include <array>
#include <compare>
#include <optional>
#include <type_traits>

template <class T, std::size_t SIZE>
class CircularBuffer {
 public:
  CircularBuffer() = default;

  ~CircularBuffer() {
    while (!empty()) {
      discard();
    }
  }

  template <class... Args>
  bool emplace(Args... args) {
    if (!full()) {
      new (&m_element_storage[m_write++]) T{std::forward<Args>(args)...};
      if (m_write == m_read) {
        m_full = true;
      }
      return true;
    }
    return false;
  }

  bool push(T element) {
    if (!full()) {
      new (&m_element_storage[m_write++]) T{std::move(element)};
      if (m_write == m_read) {
        m_full = true;
      }
      return true;
    }
    return false;
  }

  [[nodiscard]] T* peek() {
    if (!empty()) {
      return reinterpret_cast<T*>(&m_element_storage[m_read]);
    }
    return nullptr;
  }

  [[nodiscard]] std::optional<T> pop() {
    std::optional<T> value;
    if (!empty()) {
      auto* ptr = reinterpret_cast<T*>(&m_element_storage[m_read++]);
      value = std::move(*ptr);
      ptr->~T();
      m_full = false;
    }
    return value;
  }

  void discard() {
    if (!empty()) {
      auto* ptr = reinterpret_cast<T*>(&m_element_storage[m_read++]);
      ptr->~T();
      m_full = false;
    }
  }

  bool empty() const { return (m_read == m_write) && !m_full; }
  bool full() const { return m_full; }

 private:
  class CircularIndex {
   public:
    CircularIndex(std::size_t initial_value) : m_value(initial_value) {}

    std::size_t operator++() {
      increment();
      return m_value;
    }
    std::size_t operator++(int) {
      auto val = m_value;
      increment();
      return val;
    }

    operator std::size_t() { return m_value; }
    auto operator<=>(const CircularIndex& other) const = default;

    std::size_t value() { return m_value; }

   private:
    std::size_t m_value{0};

    void increment() {
      m_value++;
      if (m_value >= SIZE) {
        m_value = 0;
      }
    }
  };

  CircularIndex m_read{0};
  CircularIndex m_write{0};
  bool m_full{false};
  std::array<typename std::aligned_storage<sizeof(T), alignof(T)>::type, SIZE>
      m_element_storage;
};
