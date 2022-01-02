#ifndef DITTO_STATIC_PTR_H
#define DITTO_STATIC_PTR_H

#include <algorithm>
#include <cstring>
#include <new>
#include <type_traits>
#include <utility>

#include "ditto/type_traits.h"  // IWYU pragma: keep

namespace Ditto {

/**
 * @brief Statically allocated polymorphic object. Can hold an object of any of
 * the declared types in the template list.
 *
 * All types must inherit from the Base. The object is initially not valid. To
 * construct it you can call the make function with the type you want to build.
 *
 * It statically ensures that the alignment and size of the internal memory
 * block is enough to hold any of the derived class types.
 */
template <class Base, class... DerivedClasses>
class static_ptr {
 public:
  static_ptr() = default;

  static_ptr(static_ptr&& other) = delete;
  static_ptr& operator=(static_ptr&& other) = delete;
  static_ptr(const static_ptr&) = delete;
  static_ptr& operator=(const static_ptr&) = delete;

  ~static_ptr() { reset(); }

  /**
   * @brief Constructs an object of the given type. The type must be one of
   *        DerivedClasses.
   *
   * All arguments to make use perfect forwarding to the class constructor.
   */
  template <class T, class... Args,
            std::enable_if_t<std::is_base_of_v<Base, T> &&
                                 is_one_of_v<T, DerivedClasses...>,
                             bool> = false>
  void make(Args... args) {
    reset();
    m_ptr = new (&m_memory_buffer) T{std::forward<Args>(args)...};
  }

  /**
   * @brief Statically destructs the object, but since it is statically
   *        allocated it does not free any memory.
   */
  void reset() {
    if (m_ptr) {
      m_ptr->~Base();
      m_ptr = nullptr;
    }
  }

  /**
   * @brief Gets a pointer to the allocated object. It could be nullptr if no
   *        type has been constructed.
   */
  [[nodiscard]] Base* get() const { return m_ptr; }

  [[nodiscard]] Base* operator->() const { return m_ptr; }

  [[nodiscard]] Base& operator*() const { return *m_ptr; }

  /**
   * @brief Implicit conversion to boolean.
   */
  [[nodiscard]] operator bool() const { return m_ptr != nullptr; }

 private:
  constexpr static size_t ALIGNMENT = std::max({alignof(DerivedClasses)...});
  constexpr static size_t BUFFER_SIZE = std::max({sizeof(DerivedClasses)...});
  typename std::aligned_storage<BUFFER_SIZE, ALIGNMENT>::type m_memory_buffer;
  Base* m_ptr = nullptr;

  static_assert((std::is_base_of_v<Base, DerivedClasses> && ...),
                "Not all classes derive from base");
};

}  // namespace Ditto

#endif  // DITTO_STATIC_PTR_H
