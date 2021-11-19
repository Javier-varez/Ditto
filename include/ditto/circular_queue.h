#ifndef DITTO_CIRCULAR_QUEUE_H_
#define DITTO_CIRCULAR_QUEUE_H_

#include <array>
#include <compare>  // IWYU pragma: keep
#include <cstddef>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

namespace Ditto {
/**
 * @brief The CircularQueue is an abstraction of a ring buffer which acts like
 *        a FIFO memory. It has an associated size given in number of elements
 *        of type T.
 */
template <class T, std::size_t SIZE>
class CircularQueue {
 public:
  CircularQueue() = default;
  CircularQueue(const CircularQueue&) = default;
  CircularQueue& operator=(const CircularQueue&) = default;
  CircularQueue(CircularQueue&&) = delete;
  CircularQueue& operator=(CircularQueue&&) = delete;

  ~CircularQueue() {
    while (!empty()) {
      discard();
    }
  }

  /**
   * @brief Constructs an object of type T in place in the circular queue. All
   *        arguments are forwarded to the constructor of the T.
   * @retval true if inserted. false if the queue is full.
   *
   * It is only possible to call emplace on lvalue references, since it doesn't
   * make sense to push an element into the queue if it is going to be
   * destructed right away
   */
  template <class... Args>
  auto emplace(Args... args) & -> bool {
    if (!full()) {
      new (&m_element_storage[m_write++]) T{std::forward<Args>(args)...};
      if (m_write == m_read) {
        m_full = true;
      }
      return true;
    }
    return false;
  }

  /**
   * @brief Constructs an object of type T by copy/move.
   * @param element The element to push into the queue.
   * @retval true if inserted. false if the queue is full.
   *
   * It is only possible to call push on lvalue references, since it doesn't
   * make sense to push an element into the queue if it is going to be
   * destructed right away
   */
  auto push(T element) & -> bool {
    if (!full()) {
      new (&m_element_storage[m_write++]) T{std::move(element)};
      if (m_write == m_read) {
        m_full = true;
      }
      return true;
    }
    return false;
  }

  /**
   * @brief Returns a pointer to the next element in the queue.
   * @retval The pointer if the queue is not empty. nullptr otherwise.
   *
   * It is only possible to call push on lvalue references. Calling it on a
   * temporary is disallowed by the compailer or otherwise a dangling pointer
   * would be returned.
   */
  [[nodiscard]] auto peek() & -> T* {
    if (!empty()) {
      return reinterpret_cast<T*>(&m_element_storage[m_read]);
    }
    return nullptr;
  }

  /**
   * @brief Takes the next element from the queue and returns it.
   * @retval A T wrapped in an std::optional. It will be valid if there are
   * elements in the queue.
   */
  [[nodiscard]] auto pop() -> std::optional<T> {
    std::optional<T> value;
    if (!empty()) {
      auto* ptr = reinterpret_cast<T*>(&m_element_storage[m_read++]);
      value = std::move(*ptr);
      ptr->~T();
      m_full = false;
    }
    return value;
  }

  /**
   * @brief Discards the next element in the queue.
   */
  void discard() {
    if (!empty()) {
      auto* ptr = reinterpret_cast<T*>(&m_element_storage[m_read++]);
      ptr->~T();
      m_full = false;
    }
  }

  [[nodiscard]] auto empty() const -> bool {
    return (m_read == m_write) && !m_full;
  }
  [[nodiscard]] auto full() const -> bool { return m_full; }

 private:
  class CircularIndex {
   public:
    CircularIndex(std::size_t initial_value) : m_value(initial_value) {}

    auto operator++() -> std::size_t {
      increment();
      return m_value;
    }
    auto operator++(int) -> std::size_t {
      auto val = m_value;
      increment();
      return val;
    }

    [[nodiscard]] operator std::size_t() const { return m_value; }
    [[nodiscard]] auto operator<=>(const CircularIndex& other) const = default;

    [[nodiscard]] auto value() const -> std::size_t { return m_value; }

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

}  // namespace Ditto

#endif  // DITTO_CIRCULAR_QUEUE_H_
